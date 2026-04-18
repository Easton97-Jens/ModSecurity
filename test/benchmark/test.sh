#!/usr/bin/env bash

set -Eeuo pipefail

RUN_TS_UTC="$(date -u +"%Y%m%dT%H%M%SZ")"
DEBUG="${DEBUG:-0}"
TRACE_COMMANDS="${TRACE_COMMANDS:-1}"
VERBOSE="${VERBOSE:-1}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BENCH_DIR="$SCRIPT_DIR"

BASIC_RULES="$BENCH_DIR/basic_rules.conf"
JSON_RULES="$BENCH_DIR/json_benchmark_rules.conf"
BENCH_BIN="$BENCH_DIR/benchmark"
JSON_BENCH_BIN="$BENCH_DIR/json_benchmark"

RESULTS_ROOT="${RESULTS_ROOT:-$BENCH_DIR/results/$RUN_TS_UTC}"
WORK_ROOT="$RESULTS_ROOT/work"
GLOBAL_LOG="$RESULTS_ROOT/run.log"
GLOBAL_REPORT="$RESULTS_ROOT/report_summary.txt"
GLOBAL_STRUCTURED="$RESULTS_ROOT/comparison.csv"
RUN_META="$RESULTS_ROOT/run_metadata.txt"
SYSTEM_INFO_FILE="$RESULTS_ROOT/system_info.txt"

BACKUP_DIR="$WORK_ROOT/original_rules"

BENCH_ITERATIONS="${BENCH_ITERATIONS:-1000000}"
JSON_ITERATIONS="${JSON_ITERATIONS:-100}"

SIZES=(256 4096 51200 1048576)
VALID_SCENARIOS=("utf8" "numbers" "deep-nesting" "large-object")
INVALID_SCENARIOS=("truncated" "malformed")

VARIANTS=("baseline" "crs_v3" "crs_v4")

CRS_V3_SETUP="${CRS_V3_SETUP:-$BENCH_DIR/owasp-v3/crs-setup.conf.example}"
CRS_V3_RULES_GLOB="${CRS_V3_RULES_GLOB:-$BENCH_DIR/owasp-v3/rules/*.conf}"
CRS_V4_SETUP="${CRS_V4_SETUP:-$BENCH_DIR/owasp-v4/crs-setup.conf.example}"
CRS_V4_RULES_GLOB="${CRS_V4_RULES_GLOB:-$BENCH_DIR/owasp-v4/rules/*.conf}"
DOWNLOAD_CRS_V3_SCRIPT="$BENCH_DIR/download-owasp-v3-rules.sh"
DOWNLOAD_CRS_V4_SCRIPT="$BENCH_DIR/download-owasp-v4-rules.sh"

# Automatic CRS bootstrap:
# - If CRS v3/v4 setup file or rules are missing, this script triggers the
#   corresponding download script before benchmark execution starts.
# - If CRS artifacts already exist, nothing is downloaded (idempotent behavior).
# - You can override paths via:
#   CRS_V3_SETUP, CRS_V3_RULES_GLOB, CRS_V4_SETUP, CRS_V4_RULES_GLOB.

mkdir -p "$RESULTS_ROOT" "$WORK_ROOT" "$BACKUP_DIR"
touch "$GLOBAL_LOG"

cp "$BASIC_RULES" "$BACKUP_DIR/basic_rules.conf.orig"
cp "$JSON_RULES" "$BACKUP_DIR/json_benchmark_rules.conf.orig"

TOTAL_START_NS="$(date +%s%N)"
TOTAL_START_ISO="$(date -u +"%Y-%m-%dT%H:%M:%SZ")"

declare -A VARIANT_STATUS
declare -A VARIANT_DURATION_NS
declare -A VARIANT_BENCH_ELAPSED_S
declare -A VARIANT_BENCH_THROUGHPUT
declare -A VARIANT_JSON_AVG_TOTAL_NS
declare -A VARIANT_JSON_AVG_THROUGHPUT

ts_utc() {
    date -u +"%Y-%m-%dT%H:%M:%SZ"
}

if [[ -t 1 ]]; then
    COLOR_RESET=$'\033[0m'
    COLOR_INFO=$'\033[36m'
    COLOR_WARN=$'\033[33m'
    COLOR_ERROR=$'\033[31m'
    COLOR_STEP=$'\033[35m'
else
    COLOR_RESET=""
    COLOR_INFO=""
    COLOR_WARN=""
    COLOR_ERROR=""
    COLOR_STEP=""
fi

log() {
    local message="$1"
    printf '[%s] %s\n' "$(ts_utc)" "$message" | tee -a "$GLOBAL_LOG"
}

log_info() {
    local message="$1"
    printf '%s[%s] [INFO] %s%s\n' "$COLOR_INFO" "$(ts_utc)" "$message" "$COLOR_RESET" | tee -a "$GLOBAL_LOG"
}

log_warn() {
    local message="$1"
    printf '%s[%s] [WARN] %s%s\n' "$COLOR_WARN" "$(ts_utc)" "$message" "$COLOR_RESET" | tee -a "$GLOBAL_LOG"
}

log_error() {
    local message="$1"
    printf '%s[%s] [ERROR] %s%s\n' "$COLOR_ERROR" "$(ts_utc)" "$message" "$COLOR_RESET" | tee -a "$GLOBAL_LOG" >&2
}

log_step() {
    local message="$1"
    printf '%s[%s] [STEP] ===== %s =====%s\n' "$COLOR_STEP" "$(ts_utc)" "$message" "$COLOR_RESET" | tee -a "$GLOBAL_LOG"
}

log_debug() {
    local message="$1"
    if [[ "$DEBUG" == "1" ]]; then
        printf '[%s] [DEBUG] %s\n' "$(ts_utc)" "$message" | tee -a "$GLOBAL_LOG"
    fi
}

log_running_processes() {
    local context="${1:-snapshot}"
    if [[ "$VERBOSE" == "1" || "$DEBUG" == "1" ]]; then
        log_debug "Prozessliste ($context): benchmark/json_benchmark/modsecurity"
        ps -eo pid,ppid,etime,stat,comm,args \
            | awk 'NR==1 || /benchmark|json_benchmark|modsecurity|nginx|apache2|httpd/' \
            | tee -a "$GLOBAL_LOG"
    fi
}

trace_preexec() {
    local __trace_rc=$?
    [[ "$TRACE_COMMANDS" == "1" ]] || return 0
    [[ "${__IN_TRACE:-0}" == "1" ]] && return 0
    __IN_TRACE=1
    printf '[%s] [CMD] %s\n' "$(ts_utc)" "$BASH_COMMAND" | tee -a "$GLOBAL_LOG" >&2
    __IN_TRACE=0
    return "$__trace_rc"
}

error_handler() {
    local exit_code=$?
    local line_no="${BASH_LINENO[0]:-unknown}"
    local source_file="${BASH_SOURCE[1]:-${BASH_SOURCE[0]}}"
    log_error "Abbruch mit Exit-Code $exit_code in ${source_file}:${line_no}"
    log_error "Fehlgeschlagener Befehl: ${BASH_COMMAND}"
    log_running_processes "error"
    exit "$exit_code"
}

trap trace_preexec DEBUG
trap error_handler ERR

if [[ "$DEBUG" == "1" ]]; then
    export PS4='+ [$(date -u +"%Y-%m-%dT%H:%M:%SZ")] [xtrace:${BASH_SOURCE##*/}:${LINENO}] '
    set -x
    log_info "Debug-Modus aktiviert (DEBUG=1)"
fi

is_number() {
    local value="${1:-}"
    [[ "$value" =~ ^-?[0-9]+([.][0-9]+)?$ ]]
}

format_duration_from_ns() {
    local ns="${1:-}"
    local decimals="${2:-2}"
    local forced_unit="${3:-}"

    if [[ -z "$ns" ]] || ! is_number "$ns"; then
        echo "n/a"
        return 1
    fi
    if [[ -z "$decimals" ]] || ! [[ "$decimals" =~ ^[0-9]+$ ]]; then
        echo "n/a"
        return 1
    fi
    if awk -v v="$ns" 'BEGIN {exit !(v < 0)}'; then
        echo "n/a"
        return 1
    fi
    if [[ -n "$forced_unit" ]] && [[ ! "$forced_unit" =~ ^(ns|us|ms|s)$ ]]; then
        echo "n/a"
        return 1
    fi

    awk -v ns="$ns" -v d="$decimals" -v forced="$forced_unit" '
    function round_to(value, precision, factor) {
        factor = 10 ^ precision
        return int(value * factor + 0.5) / factor
    }
    BEGIN {
        unit_count = 4
        unit_name[1] = "ns"; unit_scale[1] = 1
        unit_name[2] = "us"; unit_scale[2] = 1000
        unit_name[3] = "ms"; unit_scale[3] = 1000000
        unit_name[4] = "s";  unit_scale[4] = 1000000000

        idx = 1
        if (forced != "") {
            for (i = 1; i <= unit_count; i++) {
                if (unit_name[i] == forced) {
                    idx = i
                    break
                }
            }
        } else {
            if (ns >= 1000000000) {
                idx = 4
            } else if (ns >= 1000000) {
                idx = 3
            } else if (ns >= 1000) {
                idx = 2
            } else {
                idx = 1
            }
        }

        value = ns / unit_scale[idx]
        rounded = round_to(value, d)

        # Smart boundary handling:
        # if rounding crosses 1000 in this unit, switch to next unit.
        while (forced == "" && idx < unit_count && rounded >= 1000) {
            idx++
            value = ns / unit_scale[idx]
            rounded = round_to(value, d)
        }

        fmt = "%." d "f %s"
        printf fmt, rounded, unit_name[idx]
    }'
}

format_time_dynamic_from_ns() {
    local ns="${1:-}"
    local decimals="${2:-2}"
    local forced_unit="${3:-}"
    format_duration_from_ns "$ns" "$decimals" "$forced_unit"
}

format_seconds_dynamic() {
    local sec="${1:-}"
    local decimals="${2:-2}"
    local forced_unit="${3:-}"

    if [[ -z "$sec" ]] || ! is_number "$sec"; then
        echo "n/a"
        return 1
    fi
    local ns
    ns="$(awk -v s="$sec" 'BEGIN { printf "%.12f", s * 1000000000 }')"
    format_duration_from_ns "$ns" "$decimals" "$forced_unit"
}

format_memory_kb() {
    local value="${1:-}"
    local decimals="${2:-2}"

    if [[ -z "$value" ]]; then
        echo "n/a"
        return 1
    fi
    if ! is_number "$value"; then
        echo "invalid($value)"
        return 1
    fi
    if awk -v v="$value" 'BEGIN {exit !(v < 0)}'; then
        echo "invalid_negative($value)"
        return 1
    fi

    awk -v kb="$value" -v d="$decimals" '
    BEGIN {
        bytes = kb * 1024.0
        unit = "B"
        scaled = bytes
        if (bytes >= 1024.0) {
            unit = "KB"
            scaled = bytes / 1024.0
        }
        if (bytes >= 1024.0 * 1024.0) {
            unit = "MB"
            scaled = bytes / (1024.0 * 1024.0)
        }
        if (bytes >= 1024.0 * 1024.0 * 1024.0) {
            unit = "GB"
            scaled = bytes / (1024.0 * 1024.0 * 1024.0)
        }
        if (bytes >= 1024.0 * 1024.0 * 1024.0 * 1024.0) {
            unit = "TB"
            scaled = bytes / (1024.0 * 1024.0 * 1024.0 * 1024.0)
        }
        fmt = "%." d "f %s"
        printf fmt, scaled, unit
    }'
}

format_throughput() {
    local value="${1:-}"
    local unit="${2:-tx/s}"
    local decimals="${3:-2}"

    if [[ -z "$value" ]]; then
        echo "n/a"
        return 1
    fi
    if ! is_number "$value"; then
        echo "invalid($value)"
        return 1
    fi
    if awk -v v="$value" 'BEGIN {exit !(v < 0)}'; then
        echo "invalid_negative($value)"
        return 1
    fi

    awk -v v="$value" -v u="$unit" -v d="$decimals" '
    BEGIN {
        fmt = "%." d "f %s"
        printf fmt, v, u
    }'
}

write_csv_header() {
    local output_file="$1"
    shift
    if [[ "$#" -eq 0 ]]; then
        echo "write_csv_header: no columns provided for $output_file" >&2
        return 1
    fi
    local first=1
    : > "$output_file"
    for col in "$@"; do
        if [[ "$first" -eq 1 ]]; then
            printf '%s' "$col" >> "$output_file"
            first=0
        else
            printf ',%s' "$col" >> "$output_file"
        fi
    done
    printf '\n' >> "$output_file"
}

restore_original_rules() {
    cp "$BACKUP_DIR/basic_rules.conf.orig" "$BASIC_RULES"
    cp "$BACKUP_DIR/json_benchmark_rules.conf.orig" "$JSON_RULES"
}

cleanup() {
    restore_original_rules
}
trap cleanup EXIT

append_crs_v3_rules() {
    {
        echo
        echo "# --- CRS v3 includes added by benchmark test.sh ---"
        echo "Include \"$CRS_V3_SETUP\""
        echo "Include \"$CRS_V3_RULES_GLOB\""
    } >> "$BASIC_RULES"

    {
        echo
        echo "# --- CRS v3 includes added by benchmark test.sh ---"
        echo "Include \"$CRS_V3_SETUP\""
        echo "Include \"$CRS_V3_RULES_GLOB\""
    } >> "$JSON_RULES"
}

append_crs_v4_rules() {
    {
        echo
        echo "# --- CRS v4 includes added by benchmark test.sh ---"
        echo "Include \"$CRS_V4_SETUP\""
        echo "Include \"$CRS_V4_RULES_GLOB\""
    } >> "$BASIC_RULES"

    {
        echo
        echo "# --- CRS v4 includes added by benchmark test.sh ---"
        echo "Include \"$CRS_V4_SETUP\""
        echo "Include \"$CRS_V4_RULES_GLOB\""
    } >> "$JSON_RULES"
}

validate_variant_dependencies() {
    local variant="$1"
    case "$variant" in
        baseline)
            return 0
            ;;
        crs_v3)
            [[ -f "$CRS_V3_SETUP" ]] || {
                log "ERROR: CRS v3 setup file not found: $CRS_V3_SETUP"
                return 1
            }
            if ! compgen -G "$CRS_V3_RULES_GLOB" > /dev/null; then
                log "ERROR: CRS v3 rules glob has no matches: $CRS_V3_RULES_GLOB"
                return 1
            fi
            ;;
        crs_v4)
            [[ -f "$CRS_V4_SETUP" ]] || {
                log "ERROR: CRS v4 setup file not found: $CRS_V4_SETUP"
                return 1
            }
            if ! compgen -G "$CRS_V4_RULES_GLOB" > /dev/null; then
                log "ERROR: CRS v4 rules glob has no matches: $CRS_V4_RULES_GLOB"
                return 1
            fi
            ;;
        *)
            log "ERROR: unknown variant: $variant"
            return 1
            ;;
    esac

    return 0
}

is_crs_v3_available() {
    [[ -f "$CRS_V3_SETUP" ]] || return 1
    compgen -G "$CRS_V3_RULES_GLOB" > /dev/null || return 1
    return 0
}

is_crs_v4_available() {
    [[ -f "$CRS_V4_SETUP" ]] || return 1
    compgen -G "$CRS_V4_RULES_GLOB" > /dev/null || return 1
    return 0
}

ensure_crs_v3() {
    if is_crs_v3_available; then
        log_info "CRS v3 bereits vorhanden, überspringe Download"
        return 0
    fi

    log_step "CRS v3 Download"
    log_info "CRS v3 not found -> triggering download"
    if [[ ! -x "$DOWNLOAD_CRS_V3_SCRIPT" ]]; then
        log_error "CRS v3 download script is not executable: $DOWNLOAD_CRS_V3_SCRIPT"
        return 1
    fi
    if ! "$DOWNLOAD_CRS_V3_SCRIPT" >> "$GLOBAL_LOG" 2>&1; then
        log_error "CRS v3 download failed"
        return 1
    fi

    if ! is_crs_v3_available; then
        log_error "CRS v3 download failed"
        return 1
    fi

    log_info "CRS v3 download completed"
    return 0
}

ensure_crs_v4() {
    if is_crs_v4_available; then
        log_info "CRS v4 bereits vorhanden, überspringe Download"
        return 0
    fi

    log_step "CRS v4 Download"
    log_info "CRS v4 not found -> triggering download"
    if [[ ! -x "$DOWNLOAD_CRS_V4_SCRIPT" ]]; then
        log_error "CRS v4 download script is not executable: $DOWNLOAD_CRS_V4_SCRIPT"
        return 1
    fi
    if ! "$DOWNLOAD_CRS_V4_SCRIPT" >> "$GLOBAL_LOG" 2>&1; then
        log_error "CRS v4 download failed"
        return 1
    fi

    if ! is_crs_v4_available; then
        log_error "CRS v4 download failed"
        return 1
    fi

    log_info "CRS v4 download completed"
    return 0
}

prepare_variant_rules() {
    local variant="$1"
    restore_original_rules

    case "$variant" in
        baseline)
            ;;
        crs_v3)
            append_crs_v3_rules
            ;;
        crs_v4)
            append_crs_v4_rules
            ;;
        *)
            log "ERROR: unknown variant in prepare_variant_rules: $variant"
            return 1
            ;;
    esac

    return 0
}

write_variant_metadata() {
    local variant="$1"
    local variant_dir="$2"
    local metadata_file="$variant_dir/metadata.txt"

    {
        echo "variant=$variant"
        echo "run_ts_utc=$RUN_TS_UTC"
        echo "bench_dir=$BENCH_DIR"
        echo "results_root=$RESULTS_ROOT"
        echo "variant_dir=$variant_dir"
        echo "basic_rules_path=$BASIC_RULES"
        echo "json_rules_path=$JSON_RULES"
        echo "benchmark_binary=$BENCH_BIN"
        echo "json_benchmark_binary=$JSON_BENCH_BIN"
        echo "bench_iterations=$BENCH_ITERATIONS"
        echo "json_iterations=$JSON_ITERATIONS"
        echo "sizes=${SIZES[*]}"
        echo "valid_scenarios=${VALID_SCENARIOS[*]}"
        echo "invalid_scenarios=${INVALID_SCENARIOS[*]}"
        echo "crs_v3_setup=$CRS_V3_SETUP"
        echo "crs_v3_rules_glob=$CRS_V3_RULES_GLOB"
        echo "crs_v4_setup=$CRS_V4_SETUP"
        echo "crs_v4_rules_glob=$CRS_V4_RULES_GLOB"
    } > "$metadata_file"
}

json_get() {
    local key="${1:-}"
    local json_line="${2:-}"

    if [[ -z "$key" ]]; then
        echo "json_get: missing key parameter" >&2
        return 13
    fi

    if [[ -z "$json_line" ]]; then
        echo "json_get: empty JSON input for key '$key'" >&2
        return 12
    fi

    if command -v jq >/dev/null 2>&1; then
        local jq_output
        if ! jq_output="$(printf '%s\n' "$json_line" | jq -r --arg k "$key" '
            if has($k) | not then "__JSON_GET_MISSING__"
            else .[$k] end
        ' 2>/dev/null)"; then
            echo "json_get: jq parse error for key '$key'" >&2
            return 12
        fi

        if [[ "$jq_output" == "__JSON_GET_MISSING__" ]]; then
            echo "json_get: missing key '$key'" >&2
            return 10
        fi

        if [[ -z "$jq_output" || "$jq_output" == "null" ]]; then
            echo "json_get: empty value for key '$key'" >&2
            return 11
        fi

        printf '%s\n' "$jq_output"
        return 0
    fi

    echo "json_get: jq not found, using limited fallback parser" >&2
    if [[ "$json_line" != *"\"$key\""* ]]; then
        echo "json_get: missing key '$key' (fallback)" >&2
        return 10
    fi

    local fallback_value
    fallback_value="$(printf '%s\n' "$json_line" | sed -n "s/.*\"$key\":\([^,}]*\).*/\1/p" | tr -d '\"')"
    if [[ -z "$fallback_value" ]]; then
        echo "json_get: parse error or empty value for key '$key' (fallback)" >&2
        return 12
    fi
    printf '%s\n' "$fallback_value"
    return 0
}

run_variant() {
    local variant="$1"

    local variant_dir="$RESULTS_ROOT/$variant"
    local log_dir="$variant_dir/logs"
    local raw_dir="$variant_dir/raw"
    local metrics_dir="$variant_dir/metrics"
    local config_dir="$variant_dir/config"

    mkdir -p "$variant_dir" "$log_dir" "$raw_dir" "$metrics_dir" "$config_dir"

    local variant_log="$log_dir/variant.log"
    local commands_log="$log_dir/commands.log"
    local errors_log="$log_dir/errors.log"
    local benchmark_raw="$raw_dir/benchmark.raw.txt"
    local json_raw="$raw_dir/json_benchmark.jsonl"
    local benchmark_metrics_csv="$metrics_dir/benchmark_metrics.csv"
    local json_metrics_csv="$metrics_dir/json_metrics.csv"
    local parsed_summary="$metrics_dir/parsed_summary.txt"

    : > "$variant_log"
    : > "$commands_log"
    : > "$errors_log"
    : > "$benchmark_raw"
    : > "$json_raw"

    write_csv_header "$benchmark_metrics_csv" \
        "variant" "iterations" "elapsed_seconds" "elapsed_dynamic" \
        "avg_transaction_ns" "avg_dynamic" "throughput_tx_per_sec" \
        "throughput_formatted" "interventions"
    write_csv_header "$json_metrics_csv" \
        "variant" "scenario" "size_bytes" "iterations" "include_invalid" \
        "process_request_body_ns" "total_transaction_ns" "total_transaction_dynamic" \
        "ru_maxrss_kb" "ru_maxrss_dynamic" "parse_success_count" "parse_error_count" \
        "derived_throughput_tx_per_sec" "derived_throughput_formatted"

    local vstart_ns="$(date +%s%N)"
    local vstart_iso="$(date -u +"%Y-%m-%dT%H:%M:%SZ")"

    log_step "START variant=$variant"
    printf '[%s] start variant=%s\n' "$vstart_iso" "$variant" >> "$variant_log"

    if ! validate_variant_dependencies "$variant"; then
        printf '[%s] dependency check failed for variant=%s\n' "$(date -u +"%Y-%m-%dT%H:%M:%SZ")" "$variant" >> "$errors_log"
        VARIANT_STATUS["$variant"]="failed"
        VARIANT_DURATION_NS["$variant"]=$(( $(date +%s%N) - vstart_ns ))
        return 1
    fi

    if ! prepare_variant_rules "$variant"; then
        printf '[%s] rules preparation failed for variant=%s\n' "$(date -u +"%Y-%m-%dT%H:%M:%SZ")" "$variant" >> "$errors_log"
        VARIANT_STATUS["$variant"]="failed"
        VARIANT_DURATION_NS["$variant"]=$(( $(date +%s%N) - vstart_ns ))
        return 1
    fi

    cp "$BASIC_RULES" "$config_dir/effective_basic_rules.conf"
    cp "$JSON_RULES" "$config_dir/effective_json_benchmark_rules.conf"
    write_variant_metadata "$variant" "$variant_dir"

    local bench_cmd=("$BENCH_BIN" "$BENCH_ITERATIONS" "--scenario" "legacy-full" "--rules-file" "$BASIC_RULES")
    log_info "Starte benchmark binary für variant=$variant"
    log_running_processes "before benchmark variant=$variant"
    printf 'command=%q ' "${bench_cmd[@]}" >> "$commands_log"
    printf '\n' >> "$commands_log"

    if "${bench_cmd[@]}" > "$benchmark_raw" 2>> "$errors_log"; then
        printf 'exit_code=0\n' >> "$commands_log"
    else
        local bench_exit=$?
        printf 'exit_code=%s\n' "$bench_exit" >> "$commands_log"
        printf '[%s] benchmark command failed exit=%s\n' "$(date -u +"%Y-%m-%dT%H:%M:%SZ")" "$bench_exit" >> "$errors_log"
        VARIANT_STATUS["$variant"]="failed"
        VARIANT_DURATION_NS["$variant"]=$(( $(date +%s%N) - vstart_ns ))
        return 1
    fi
    log_running_processes "after benchmark variant=$variant"

    local elapsed_s avg_ns throughput interventions
    elapsed_s="$(awk '/elapsed_seconds:/ {print $2}' "$benchmark_raw" | tail -n1)"
    avg_ns="$(awk '/avg_transaction_ns:/ {print $2}' "$benchmark_raw" | tail -n1)"
    throughput="$(awk '/throughput_tx_per_sec:/ {print $2}' "$benchmark_raw" | tail -n1)"
    interventions="$(awk '/interventions:/ {print $2}' "$benchmark_raw" | tail -n1)"

    local elapsed_dynamic avg_dynamic throughput_fmt
    elapsed_dynamic="$(format_seconds_dynamic "$elapsed_s")"
    avg_dynamic="$(format_time_dynamic_from_ns "$avg_ns")"
    throughput_fmt="$(format_throughput "$throughput" "tx/s" 2 || true)"

    echo "$variant,$BENCH_ITERATIONS,$elapsed_s,$elapsed_dynamic,$avg_ns,$avg_dynamic,$throughput,$throughput_fmt,$interventions" >> "$benchmark_metrics_csv"

    local scenario size include_invalid output json_exit process_ns total_ns rss_kb success_count error_count derived_tps total_dynamic rss_dynamic derived_tps_fmt

    for size in "${SIZES[@]}"; do
        log_step "JSON-Benchmark variant=$variant size=$size"
        for scenario in "${VALID_SCENARIOS[@]}"; do
            include_invalid="no"
            local json_cmd=("$JSON_BENCH_BIN" "--scenario" "$scenario" "--iterations" "$JSON_ITERATIONS" "--target-bytes" "$size" "--output" "json")
            log_info "JSON benchmark start variant=$variant scenario=$scenario size=$size include_invalid=$include_invalid"
            log_running_processes "before json scenario=$scenario size=$size"
            printf 'command=%q ' "${json_cmd[@]}" >> "$commands_log"
            printf '\n' >> "$commands_log"

            if output="$("${json_cmd[@]}" 2>> "$errors_log")"; then
                json_exit=0
            else
                json_exit=$?
            fi
            printf 'exit_code=%s\n' "$json_exit" >> "$commands_log"
            log_running_processes "after json scenario=$scenario size=$size"

            if [[ "$json_exit" -ne 0 ]]; then
                printf '[%s] json_benchmark failed variant=%s scenario=%s size=%s include_invalid=%s exit=%s\n' \
                    "$(date -u +"%Y-%m-%dT%H:%M:%SZ")" "$variant" "$scenario" "$size" "$include_invalid" "$json_exit" >> "$errors_log"
                VARIANT_STATUS["$variant"]="failed"
                VARIANT_DURATION_NS["$variant"]=$(( $(date +%s%N) - vstart_ns ))
                return 1
            fi

            printf '%s\n' "$output" >> "$json_raw"

            if ! process_ns="$(json_get "process_request_body_ns" "$output" 2>> "$errors_log")" \
                || ! total_ns="$(json_get "total_transaction_ns" "$output" 2>> "$errors_log")" \
                || ! rss_kb="$(json_get "ru_maxrss_kb" "$output" 2>> "$errors_log")" \
                || ! success_count="$(json_get "parse_success_count" "$output" 2>> "$errors_log")" \
                || ! error_count="$(json_get "parse_error_count" "$output" 2>> "$errors_log")"; then
                printf '[%s] json metrics parse failed variant=%s scenario=%s size=%s include_invalid=%s\n' \
                    "$(date -u +"%Y-%m-%dT%H:%M:%SZ")" "$variant" "$scenario" "$size" "$include_invalid" >> "$errors_log"
                VARIANT_STATUS["$variant"]="failed"
                VARIANT_DURATION_NS["$variant"]=$(( $(date +%s%N) - vstart_ns ))
                return 1
            fi
            derived_tps="$(awk -v it="$JSON_ITERATIONS" -v ns="$total_ns" 'BEGIN{ if (ns > 0) printf "%.6f", it / (ns / 1000000000); else print "0" }')"
            total_dynamic="$(format_time_dynamic_from_ns "$total_ns")"
            rss_dynamic="$(format_memory_kb "$rss_kb" 2 || true)"
            derived_tps_fmt="$(format_throughput "$derived_tps" "iter/s" 2 || true)"

            echo "$variant,$scenario,$size,$JSON_ITERATIONS,$include_invalid,$process_ns,$total_ns,$total_dynamic,$rss_kb,$rss_dynamic,$success_count,$error_count,$derived_tps,$derived_tps_fmt" >> "$json_metrics_csv"
        done

        for scenario in "${INVALID_SCENARIOS[@]}"; do
            include_invalid="yes"
            local json_cmd=("$JSON_BENCH_BIN" "--scenario" "$scenario" "--include-invalid" "--iterations" "$JSON_ITERATIONS" "--target-bytes" "$size" "--output" "json")
            log_info "JSON benchmark start variant=$variant scenario=$scenario size=$size include_invalid=$include_invalid"
            log_running_processes "before json invalid scenario=$scenario size=$size"
            printf 'command=%q ' "${json_cmd[@]}" >> "$commands_log"
            printf '\n' >> "$commands_log"

            if output="$("${json_cmd[@]}" 2>> "$errors_log")"; then
                json_exit=0
            else
                json_exit=$?
            fi
            printf 'exit_code=%s\n' "$json_exit" >> "$commands_log"
            log_running_processes "after json invalid scenario=$scenario size=$size"

            if [[ "$json_exit" -ne 0 ]]; then
                printf '[%s] json_benchmark failed variant=%s scenario=%s size=%s include_invalid=%s exit=%s\n' \
                    "$(date -u +"%Y-%m-%dT%H:%M:%SZ")" "$variant" "$scenario" "$size" "$include_invalid" "$json_exit" >> "$errors_log"
                VARIANT_STATUS["$variant"]="failed"
                VARIANT_DURATION_NS["$variant"]=$(( $(date +%s%N) - vstart_ns ))
                return 1
            fi

            printf '%s\n' "$output" >> "$json_raw"

            if ! process_ns="$(json_get "process_request_body_ns" "$output" 2>> "$errors_log")" \
                || ! total_ns="$(json_get "total_transaction_ns" "$output" 2>> "$errors_log")" \
                || ! rss_kb="$(json_get "ru_maxrss_kb" "$output" 2>> "$errors_log")" \
                || ! success_count="$(json_get "parse_success_count" "$output" 2>> "$errors_log")" \
                || ! error_count="$(json_get "parse_error_count" "$output" 2>> "$errors_log")"; then
                printf '[%s] json metrics parse failed variant=%s scenario=%s size=%s include_invalid=%s\n' \
                    "$(date -u +"%Y-%m-%dT%H:%M:%SZ")" "$variant" "$scenario" "$size" "$include_invalid" >> "$errors_log"
                VARIANT_STATUS["$variant"]="failed"
                VARIANT_DURATION_NS["$variant"]=$(( $(date +%s%N) - vstart_ns ))
                return 1
            fi
            derived_tps="$(awk -v it="$JSON_ITERATIONS" -v ns="$total_ns" 'BEGIN{ if (ns > 0) printf "%.6f", it / (ns / 1000000000); else print "0" }')"
            total_dynamic="$(format_time_dynamic_from_ns "$total_ns")"
            rss_dynamic="$(format_memory_kb "$rss_kb" 2 || true)"
            derived_tps_fmt="$(format_throughput "$derived_tps" "iter/s" 2 || true)"

            echo "$variant,$scenario,$size,$JSON_ITERATIONS,$include_invalid,$process_ns,$total_ns,$total_dynamic,$rss_kb,$rss_dynamic,$success_count,$error_count,$derived_tps,$derived_tps_fmt" >> "$json_metrics_csv"
        done
    done

    local json_avg_total_ns json_avg_tps
    json_avg_total_ns="$(awk -F, 'NR>1 {sum+=$7; n++} END { if (n>0) printf "%.2f", sum/n; else print "0" }' "$json_metrics_csv")"
    json_avg_tps="$(awk -F, 'NR>1 {sum+=$11; n++} END { if (n>0) printf "%.6f", sum/n; else print "0" }' "$json_metrics_csv")"

    local vend_ns="$(date +%s%N)"
    local vend_iso="$(date -u +"%Y-%m-%dT%H:%M:%SZ")"
    local vdur_ns=$((vend_ns - vstart_ns))

    VARIANT_STATUS["$variant"]="ok"
    VARIANT_DURATION_NS["$variant"]="$vdur_ns"
    VARIANT_BENCH_ELAPSED_S["$variant"]="$elapsed_s"
    VARIANT_BENCH_THROUGHPUT["$variant"]="$throughput"
    VARIANT_JSON_AVG_TOTAL_NS["$variant"]="$json_avg_total_ns"
    VARIANT_JSON_AVG_THROUGHPUT["$variant"]="$json_avg_tps"

    {
        echo "variant=$variant"
        echo "status=ok"
        echo "start_time_utc=$vstart_iso"
        echo "end_time_utc=$vend_iso"
        echo "duration_ns=$vdur_ns"
        echo "duration_dynamic=$(format_time_dynamic_from_ns "$vdur_ns")"
        echo "benchmark_elapsed_seconds=$elapsed_s"
        echo "benchmark_elapsed_dynamic=$(format_seconds_dynamic "$elapsed_s")"
        echo "benchmark_throughput_tx_per_sec=$throughput"
        echo "benchmark_interventions=$interventions"
        echo "json_average_total_transaction_ns=$json_avg_total_ns"
        echo "json_average_total_transaction_dynamic=$(format_time_dynamic_from_ns "$json_avg_total_ns")"
        echo "json_average_derived_throughput_tx_per_sec=$json_avg_tps"
        echo "benchmark_raw=$benchmark_raw"
        echo "json_raw=$json_raw"
        echo "benchmark_metrics_csv=$benchmark_metrics_csv"
        echo "json_metrics_csv=$json_metrics_csv"
        echo "variant_log=$variant_log"
        echo "commands_log=$commands_log"
        echo "errors_log=$errors_log"
    } > "$parsed_summary"

    printf '[%s] end variant=%s duration_ns=%s\n' "$vend_iso" "$variant" "$vdur_ns" >> "$variant_log"
    log "END variant=$variant status=ok duration=$(format_time_dynamic_from_ns "$vdur_ns")"
    return 0
}

preflight_checks() {
    log_step "Preflight checks"
    log_info "Prüfe Binärdateien und Konfigurationsdateien"
    [[ -x "$BENCH_BIN" ]] || { echo "Fehler: benchmark binary fehlt: $BENCH_BIN" >&2; exit 1; }
    [[ -x "$JSON_BENCH_BIN" ]] || { echo "Fehler: json_benchmark binary fehlt: $JSON_BENCH_BIN" >&2; exit 1; }
    [[ -f "$BASIC_RULES" ]] || { echo "Fehler: rules file fehlt: $BASIC_RULES" >&2; exit 1; }
    [[ -f "$JSON_RULES" ]] || { echo "Fehler: rules file fehlt: $JSON_RULES" >&2; exit 1; }
    log_info "Preflight checks abgeschlossen"
}

write_global_metadata() {
    {
        echo "run_ts_utc=$RUN_TS_UTC"
        echo "total_start_utc=$TOTAL_START_ISO"
        echo "bench_dir=$BENCH_DIR"
        echo "results_root=$RESULTS_ROOT"
        echo "global_log=$GLOBAL_LOG"
        echo "bench_iterations=$BENCH_ITERATIONS"
        echo "json_iterations=$JSON_ITERATIONS"
        echo "sizes=${SIZES[*]}"
        echo "variants=${VARIANTS[*]}"
        echo "system_info_file=$SYSTEM_INFO_FILE"
    } > "$RUN_META"
}

collect_system_metadata() {
    local os_pretty="not_available"
    local os_name="not_available"
    local os_version="not_available"
    local os_version_id="not_available"

    local uname_s="not_available"
    local uname_r="not_available"
    local uname_m="not_available"
    local uname_a="not_available"

    local host_name="not_available"
    local run_utc_now="not_available"
    local user_name="not_available"

    local cpu_model="not_available"
    local cpu_arch="not_available"
    local cpu_logical="not_available"
    local cpu_physical_cores="not_available"
    local cpu_freq_mhz="not_available"

    local ram_total_kb="not_available"
    local virtualization_hint="not_available"

    if [[ -r "/etc/os-release" ]]; then
        os_pretty="$(awk -F= '/^PRETTY_NAME=/{gsub(/"/,"",$2); print $2; exit}' /etc/os-release)"
        os_name="$(awk -F= '/^NAME=/{gsub(/"/,"",$2); print $2; exit}' /etc/os-release)"
        os_version="$(awk -F= '/^VERSION=/{gsub(/"/,"",$2); print $2; exit}' /etc/os-release)"
        os_version_id="$(awk -F= '/^VERSION_ID=/{gsub(/"/,"",$2); print $2; exit}' /etc/os-release)"
        os_pretty="${os_pretty:-not_available}"
        os_name="${os_name:-not_available}"
        os_version="${os_version:-not_available}"
        os_version_id="${os_version_id:-not_available}"
    fi

    uname_s="$(uname -s 2>/dev/null || echo not_available)"
    uname_r="$(uname -r 2>/dev/null || echo not_available)"
    uname_m="$(uname -m 2>/dev/null || echo not_available)"
    uname_a="$(uname -a 2>/dev/null || echo not_available)"

    host_name="$(hostname 2>/dev/null || echo not_available)"
    run_utc_now="$(date -u +"%Y-%m-%dT%H:%M:%SZ" 2>/dev/null || echo not_available)"
    user_name="${USER:-$(id -un 2>/dev/null || echo not_available)}"

    if command -v lscpu >/dev/null 2>&1; then
        cpu_model="$(LC_ALL=C lscpu 2>/dev/null | awk -F: '/Model name:/{sub(/^[ \t]+/,"",$2); print $2; exit}')"
        cpu_arch="$(LC_ALL=C lscpu 2>/dev/null | awk -F: '/Architecture:/{sub(/^[ \t]+/,"",$2); print $2; exit}')"
        cpu_logical="$(LC_ALL=C lscpu 2>/dev/null | awk -F: '/^CPU\\(s\\):/{sub(/^[ \t]+/,"",$2); print $2; exit}')"
        local cores_per_socket sockets
        cores_per_socket="$(LC_ALL=C lscpu 2>/dev/null | awk -F: '/Core\\(s\\) per socket:/{sub(/^[ \t]+/,"",$2); print $2; exit}')"
        sockets="$(LC_ALL=C lscpu 2>/dev/null | awk -F: '/Socket\\(s\\):/{sub(/^[ \t]+/,"",$2); print $2; exit}')"
        if [[ "$cores_per_socket" =~ ^[0-9]+$ ]] && [[ "$sockets" =~ ^[0-9]+$ ]]; then
            cpu_physical_cores="$((cores_per_socket * sockets))"
        fi
        cpu_freq_mhz="$(LC_ALL=C lscpu 2>/dev/null | awk -F: '
            /CPU max MHz:/{sub(/^[ \t]+/,"",$2); if ($2 != "") {print $2; exit}}
            /CPU MHz:/{sub(/^[ \t]+/,"",$2); if ($2 != "") {print $2; exit}}
        ')"
    fi

    if [[ "$cpu_model" == "not_available" || -z "$cpu_model" ]] && [[ -r "/proc/cpuinfo" ]]; then
        cpu_model="$(awk -F: '/^model name/{sub(/^[ \t]+/,"",$2); print $2; exit}' /proc/cpuinfo)"
    fi
    if [[ "$cpu_arch" == "not_available" || -z "$cpu_arch" ]]; then
        cpu_arch="$uname_m"
    fi
    if [[ "$cpu_logical" == "not_available" || -z "$cpu_logical" ]]; then
        if command -v getconf >/dev/null 2>&1; then
            cpu_logical="$(getconf _NPROCESSORS_ONLN 2>/dev/null || true)"
        fi
        if [[ -z "$cpu_logical" ]] && command -v nproc >/dev/null 2>&1; then
            cpu_logical="$(nproc 2>/dev/null || true)"
        fi
        if [[ -z "$cpu_logical" ]] && [[ -r "/proc/cpuinfo" ]]; then
            cpu_logical="$(awk -F: '/^processor/{count++} END{if (count>0) print count}' /proc/cpuinfo)"
        fi
    fi
    if [[ "$cpu_physical_cores" == "not_available" || -z "$cpu_physical_cores" ]] && [[ -r "/proc/cpuinfo" ]]; then
        cpu_physical_cores="$(awk -F: '
            /^physical id/{pid=$2; gsub(/^[ \t]+/,"",pid)}
            /^cpu cores/{cores=$2; gsub(/^[ \t]+/,"",cores); if (pid != "" && cores ~ /^[0-9]+$/) map[pid]=cores}
            END {sum=0; for (k in map) sum+=map[k]; if (sum>0) print sum}
        ' /proc/cpuinfo)"
    fi
    if [[ "$cpu_freq_mhz" == "not_available" || -z "$cpu_freq_mhz" ]] && [[ -r "/proc/cpuinfo" ]]; then
        cpu_freq_mhz="$(awk -F: '/^cpu MHz/{sub(/^[ \t]+/,"",$2); print $2; exit}' /proc/cpuinfo)"
    fi

    cpu_model="${cpu_model:-not_available}"
    cpu_arch="${cpu_arch:-not_available}"
    cpu_logical="${cpu_logical:-not_available}"
    cpu_physical_cores="${cpu_physical_cores:-not_available}"
    cpu_freq_mhz="${cpu_freq_mhz:-not_available}"

    if [[ -r "/proc/meminfo" ]]; then
        ram_total_kb="$(awk '/^MemTotal:/{print $2; exit}' /proc/meminfo)"
        ram_total_kb="${ram_total_kb:-not_available}"
    fi

    if command -v systemd-detect-virt >/dev/null 2>&1; then
        local virt_out
        virt_out="$(systemd-detect-virt 2>/dev/null || true)"
        if [[ -n "$virt_out" && "$virt_out" != "none" ]]; then
            virtualization_hint="$virt_out"
        fi
    fi
    if [[ "$virtualization_hint" == "not_available" ]]; then
        if [[ -f "/.dockerenv" ]]; then
            virtualization_hint="docker_env_file_detected"
        elif [[ -r "/proc/1/cgroup" ]] && grep -Eq '(docker|containerd|kubepods|lxc)' /proc/1/cgroup; then
            virtualization_hint="container_cgroup_detected"
        else
            virtualization_hint="not_detected"
        fi
    fi

    {
        echo "system_info_collected_utc=$run_utc_now"
        echo "os_pretty_name=$os_pretty"
        echo "os_name=$os_name"
        echo "os_version=$os_version"
        echo "os_version_id=$os_version_id"
        echo "uname_s=$uname_s"
        echo "uname_r=$uname_r"
        echo "uname_m=$uname_m"
        echo "uname_a=$uname_a"
        echo "hostname=$host_name"
        echo "user_name=$user_name"
        echo "cpu_model=$cpu_model"
        echo "cpu_architecture=$cpu_arch"
        echo "cpu_logical_count=$cpu_logical"
        echo "cpu_physical_cores=$cpu_physical_cores"
        echo "cpu_frequency_mhz=$cpu_freq_mhz"
        echo "ram_total_kb=$ram_total_kb"
        echo "virtualization_hint=$virtualization_hint"
    } > "$SYSTEM_INFO_FILE"

    {
        echo "system_info_collected_utc=$run_utc_now"
        echo "system_os_pretty_name=$os_pretty"
        echo "system_uname_s=$uname_s"
        echo "system_uname_r=$uname_r"
        echo "system_uname_m=$uname_m"
        echo "system_hostname=$host_name"
        echo "system_cpu_model=$cpu_model"
        echo "system_cpu_architecture=$cpu_arch"
        echo "system_cpu_logical_count=$cpu_logical"
        echo "system_cpu_physical_cores=$cpu_physical_cores"
        echo "system_ram_total_kb=$ram_total_kb"
        echo "system_virtualization_hint=$virtualization_hint"
    } >> "$RUN_META"

    log "System metadata collected: $SYSTEM_INFO_FILE"
}

build_comparison_outputs() {
    echo "variant,status,duration_ns,duration_dynamic,benchmark_elapsed_seconds,benchmark_elapsed_dynamic,benchmark_throughput_tx_per_sec,json_avg_total_transaction_ns,json_avg_total_transaction_dynamic,json_avg_derived_throughput_tx_per_sec" > "$GLOBAL_STRUCTURED"

    : > "$GLOBAL_REPORT"
    {
        echo "Benchmark comparison report"
        echo "run_ts_utc=$RUN_TS_UTC"
        echo "results_root=$RESULTS_ROOT"
        echo
    } >> "$GLOBAL_REPORT"

    local variant
    for variant in "${VARIANTS[@]}"; do
        local status="${VARIANT_STATUS[$variant]:-not_run}"
        local dur_ns="${VARIANT_DURATION_NS[$variant]:-0}"
        local dur_fmt="$(format_time_dynamic_from_ns "$dur_ns")"
        local b_elapsed="${VARIANT_BENCH_ELAPSED_S[$variant]:-0}"
        local b_elapsed_fmt="$(format_seconds_dynamic "$b_elapsed")"
        local b_tps="${VARIANT_BENCH_THROUGHPUT[$variant]:-0}"
        local j_total_ns="${VARIANT_JSON_AVG_TOTAL_NS[$variant]:-0}"
        local j_total_fmt="$(format_time_dynamic_from_ns "$j_total_ns")"
        local j_tps="${VARIANT_JSON_AVG_THROUGHPUT[$variant]:-0}"

        echo "$variant,$status,$dur_ns,$dur_fmt,$b_elapsed,$b_elapsed_fmt,$b_tps,$j_total_ns,$j_total_fmt,$j_tps" >> "$GLOBAL_STRUCTURED"

        {
            echo "variant=$variant"
            echo "  status=$status"
            echo "  duration=$dur_fmt ($dur_ns ns)"
            echo "  benchmark_elapsed=$b_elapsed_fmt ($b_elapsed s)"
            echo "  benchmark_throughput_tx_per_sec=$b_tps"
            echo "  json_avg_total_transaction=$j_total_fmt ($j_total_ns ns)"
            echo "  json_avg_derived_throughput_tx_per_sec=$j_tps"
            echo
        } >> "$GLOBAL_REPORT"
    done

    {
        echo "Comparison view (baseline vs crs_v3 vs crs_v4)"
        awk -F, 'NR==1 {next} {printf "%-10s | %-8s | %-12s | %-14s | %-14s\n", $1, $2, $4, $7, $10}' "$GLOBAL_STRUCTURED"
    } >> "$GLOBAL_REPORT"
}

main() {
    log_step "Initialisierung"
    log_info "Konfiguration: DEBUG=$DEBUG TRACE_COMMANDS=$TRACE_COMMANDS VERBOSE=$VERBOSE"
    log_info "Artefakte: RESULTS_ROOT=$RESULTS_ROOT"
    preflight_checks
    write_global_metadata
    collect_system_metadata
    ensure_crs_v3
    ensure_crs_v4

    log_step "Benchmark Gesamtlauf gestartet"
    log_info "Results root: $RESULTS_ROOT"

    local overall_status="ok"
    local variant

    for variant in "${VARIANTS[@]}"; do
        if ! run_variant "$variant"; then
            overall_status="failed"
            log_error "Variant failed: $variant"
        fi
    done

    local total_end_ns="$(date +%s%N)"
    local total_end_iso="$(date -u +"%Y-%m-%dT%H:%M:%SZ")"
    local total_duration_ns=$((total_end_ns - TOTAL_START_NS))
    local total_duration_fmt
    total_duration_fmt="$(format_time_dynamic_from_ns "$total_duration_ns")"

    build_comparison_outputs

    {
        echo "total_end_utc=$total_end_iso"
        echo "total_duration_ns=$total_duration_ns"
        echo "total_duration_dynamic=$total_duration_fmt"
        echo "overall_status=$overall_status"
    } >> "$RUN_META"

    log_step "Benchmark Gesamtlauf beendet"
    log_info "overall_status=$overall_status"
    log_info "total_duration=$total_duration_fmt"
    log_info "comparison_csv=$GLOBAL_STRUCTURED"
    log_info "summary_report=$GLOBAL_REPORT"

    if [[ "$overall_status" != "ok" ]]; then
        return 1
    fi

    return 0
}

main "$@"
