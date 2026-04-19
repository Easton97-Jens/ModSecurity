#!/usr/bin/env bash

set -Eeuo pipefail

DEBUG="${DEBUG:-0}"
TRACE_COMMANDS="${TRACE_COMMANDS:-1}"
VERBOSE="${VERBOSE:-1}"
exec 19>&2

TRACE_FIRST_EXTERNAL_COMMAND_OVERALL=""
TRACE_FIRST_EXTERNAL_COMMAND_OVERALL_TS=""
TRACE_FIRST_BENCHMARK_RELATED_EXTERNAL_COMMAND=""
TRACE_FIRST_BENCHMARK_RELATED_EXTERNAL_COMMAND_TS=""
TRACE_FIRST_BENCHMARK_BINARY_EXECUTION=""
TRACE_FIRST_BENCHMARK_BINARY_EXECUTION_TS=""
TRACE_FIRST_JSON_BENCHMARK_BINARY_EXECUTION=""
TRACE_FIRST_JSON_BENCHMARK_BINARY_EXECUTION_TS=""

trace_now_utc() {
    printf '%(%Y-%m-%dT%H:%M:%SZ)T' -1
}

trace_format_command() {
    local formatted=""
    local arg escaped
    for arg in "$@"; do
        printf -v escaped '%q' "$arg"
        if [[ -n "$formatted" ]]; then
            formatted+=" "
        fi
        formatted+="$escaped"
    done
    printf '%s' "$formatted"
}

trace_append_global_log() {
    local line="$1"
    if [[ -n "${GLOBAL_LOG:-}" ]]; then
        local log_dir="${GLOBAL_LOG%/*}"
        if [[ -d "$log_dir" ]]; then
            printf '%s\n' "$line" >> "$GLOBAL_LOG"
        fi
    fi
}

trace_emit_line() {
    local line="$1"
    [[ "${TRACE_COMMANDS}" == "1" ]] || return 0
    printf '%s\n' "$line" >&19
    trace_append_global_log "$line"
}

trace_record_first() {
    local kind="$1"
    local timestamp="$2"
    local formatted="$3"

    if [[ -z "$TRACE_FIRST_EXTERNAL_COMMAND_OVERALL" ]]; then
        TRACE_FIRST_EXTERNAL_COMMAND_OVERALL="$formatted"
        TRACE_FIRST_EXTERNAL_COMMAND_OVERALL_TS="$timestamp"
    fi

    case "$kind" in
        benchmark_related|benchmark_binary|json_benchmark_binary)
            if [[ -z "$TRACE_FIRST_BENCHMARK_RELATED_EXTERNAL_COMMAND" ]]; then
                TRACE_FIRST_BENCHMARK_RELATED_EXTERNAL_COMMAND="$formatted"
                TRACE_FIRST_BENCHMARK_RELATED_EXTERNAL_COMMAND_TS="$timestamp"
            fi
            ;;
    esac

    case "$kind" in
        benchmark_binary)
            if [[ -z "$TRACE_FIRST_BENCHMARK_BINARY_EXECUTION" ]]; then
                TRACE_FIRST_BENCHMARK_BINARY_EXECUTION="$formatted"
                TRACE_FIRST_BENCHMARK_BINARY_EXECUTION_TS="$timestamp"
            fi
            ;;
        json_benchmark_binary)
            if [[ -z "$TRACE_FIRST_JSON_BENCHMARK_BINARY_EXECUTION" ]]; then
                TRACE_FIRST_JSON_BENCHMARK_BINARY_EXECUTION="$formatted"
                TRACE_FIRST_JSON_BENCHMARK_BINARY_EXECUTION_TS="$timestamp"
            fi
            ;;
    esac
}

run_external() {
    local kind="$1"
    shift
    [[ "$#" -gt 0 && "$1" == "--" ]] || return 2
    shift

    local formatted timestamp errexit_was_set=0 status
    formatted="$(trace_format_command "$@")"
    timestamp="$(trace_now_utc)"
    trace_emit_line "[TRACE] ${timestamp} PWD=${PWD} CMD=${formatted}"
    trace_record_first "$kind" "$timestamp" "$formatted"

    case $- in
        *e*)
            errexit_was_set=1
            set +e
            ;;
    esac

    "$@"
    status=$?

    if [[ "$errexit_was_set" -eq 1 ]]; then
        set -e
    fi
    return "$status"
}

capture_external() {
    local outvar_name="$1"
    local -n outref="$outvar_name"
    local kind="$2"
    shift 2
    [[ "$#" -gt 0 && "$1" == "--" ]] || return 2
    shift

    local trace_formatted trace_timestamp captured_output errexit_was_set=0 status
    trace_formatted="$(trace_format_command "$@")"
    trace_timestamp="$(trace_now_utc)"
    trace_emit_line "[TRACE] ${trace_timestamp} PWD=${PWD} CMD=${trace_formatted}"
    trace_record_first "$kind" "$trace_timestamp" "$trace_formatted"

    case $- in
        *e*)
            errexit_was_set=1
            set +e
            ;;
    esac

    captured_output="$("$@")"
    status=$?

    if [[ "$errexit_was_set" -eq 1 ]]; then
        set -e
    fi

    outref="$captured_output"
    return "$status"
}

run_external_env() {
    local kind="$1"
    shift

    local -a env_assignments=()
    while [[ "$#" -gt 0 && "$1" == "--env" ]]; do
        [[ "$#" -ge 2 ]] || return 2
        env_assignments+=("$2")
        shift 2
    done
    [[ "$#" -gt 0 && "$1" == "--" ]] || return 2
    shift

    local -a saved_decls=()
    local -a saved_present=()
    local -a formatted_args=()
    local assignment name value idx formatted timestamp errexit_was_set=0 status

    for assignment in "${env_assignments[@]}"; do
        formatted_args+=("$assignment")
        name="${assignment%%=*}"
        value="${assignment#*=}"
        if declare -p "$name" >/dev/null 2>&1; then
            saved_present+=("1")
            saved_decls+=("$(declare -p "$name")")
        else
            saved_present+=("0")
            saved_decls+=("")
        fi
        printf -v "$name" '%s' "$value"
        export "$name"
    done

    formatted="$(trace_format_command "${formatted_args[@]}" "$@")"
    timestamp="$(trace_now_utc)"
    trace_emit_line "[TRACE] ${timestamp} PWD=${PWD} CMD=${formatted}"
    trace_record_first "$kind" "$timestamp" "$formatted"

    case $- in
        *e*)
            errexit_was_set=1
            set +e
            ;;
    esac

    "$@"
    status=$?

    for idx in "${!env_assignments[@]}"; do
        name="${env_assignments[$idx]%%=*}"
        if [[ "${saved_present[$idx]}" == "1" ]]; then
            builtin eval "${saved_decls[$idx]}"
        else
            unset "$name"
        fi
    done

    if [[ "$errexit_was_set" -eq 1 ]]; then
        set -e
    fi
    return "$status"
}

capture_external_env() {
    local outvar_name="$1"
    local -n outref="$outvar_name"
    local kind="$2"
    shift 2

    local -a env_assignments=()
    while [[ "$#" -gt 0 && "$1" == "--env" ]]; do
        [[ "$#" -ge 2 ]] || return 2
        env_assignments+=("$2")
        shift 2
    done
    [[ "$#" -gt 0 && "$1" == "--" ]] || return 2
    shift

    local -a saved_decls=()
    local -a saved_present=()
    local -a formatted_args=()
    local assignment name value idx trace_formatted trace_timestamp captured_output errexit_was_set=0 status

    for assignment in "${env_assignments[@]}"; do
        formatted_args+=("$assignment")
        name="${assignment%%=*}"
        value="${assignment#*=}"
        if declare -p "$name" >/dev/null 2>&1; then
            saved_present+=("1")
            saved_decls+=("$(declare -p "$name")")
        else
            saved_present+=("0")
            saved_decls+=("")
        fi
        printf -v "$name" '%s' "$value"
        export "$name"
    done

    trace_formatted="$(trace_format_command "${formatted_args[@]}" "$@")"
    trace_timestamp="$(trace_now_utc)"
    trace_emit_line "[TRACE] ${trace_timestamp} PWD=${PWD} CMD=${trace_formatted}"
    trace_record_first "$kind" "$trace_timestamp" "$trace_formatted"

    case $- in
        *e*)
            errexit_was_set=1
            set +e
            ;;
    esac

    captured_output="$("$@")"
    status=$?

    for idx in "${!env_assignments[@]}"; do
        name="${env_assignments[$idx]%%=*}"
        if [[ "${saved_present[$idx]}" == "1" ]]; then
            builtin eval "${saved_decls[$idx]}"
        else
            unset "$name"
        fi
    done

    if [[ "$errexit_was_set" -eq 1 ]]; then
        set -e
    fi

    outref="$captured_output"
    return "$status"
}

capture_external RUN_TS_UTC benchmark_related -- date -u +"%Y%m%dT%H%M%SZ"

local_script_dir=""
capture_external local_script_dir benchmark_related -- dirname "${BASH_SOURCE[0]}"
SCRIPT_DIR="$(cd "$local_script_dir" && pwd)"

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

run_external benchmark_related -- mkdir -p "$RESULTS_ROOT" "$WORK_ROOT" "$BACKUP_DIR"
run_external benchmark_related -- touch "$GLOBAL_LOG"

run_external benchmark_related -- cp "$BASIC_RULES" "$BACKUP_DIR/basic_rules.conf.orig"
run_external benchmark_related -- cp "$JSON_RULES" "$BACKUP_DIR/json_benchmark_rules.conf.orig"

capture_external TOTAL_START_NS benchmark_related -- date +%s%N
capture_external TOTAL_START_ISO benchmark_related -- date -u +"%Y-%m-%dT%H:%M:%SZ"

declare -A VARIANT_STATUS
declare -A VARIANT_DURATION_NS
declare -A VARIANT_BENCH_ELAPSED_S
declare -A VARIANT_BENCH_THROUGHPUT
declare -A VARIANT_JSON_AVG_TOTAL_NS
declare -A VARIANT_JSON_AVG_THROUGHPUT

ts_utc() {
    trace_now_utc
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
    local line
    line="[$(ts_utc)] $message"
    printf '%s\n' "$line"
    trace_append_global_log "$line"
}

log_info() {
    local message="$1"
    local line
    line="${COLOR_INFO}[$(ts_utc)] [INFO] ${message}${COLOR_RESET}"
    printf '%s\n' "$line"
    trace_append_global_log "$line"
}

log_warn() {
    local message="$1"
    local line
    line="${COLOR_WARN}[$(ts_utc)] [WARN] ${message}${COLOR_RESET}"
    printf '%s\n' "$line"
    trace_append_global_log "$line"
}

log_error() {
    local message="$1"
    local line
    line="${COLOR_ERROR}[$(ts_utc)] [ERROR] ${message}${COLOR_RESET}"
    printf '%s\n' "$line" >&2
    trace_append_global_log "$line"
}

log_step() {
    local message="$1"
    local line
    line="${COLOR_STEP}[$(ts_utc)] [STEP] ===== ${message} =====${COLOR_RESET}"
    printf '%s\n' "$line"
    trace_append_global_log "$line"
}

log_debug() {
    local message="$1"
    if [[ "$DEBUG" == "1" ]]; then
        local line
        line="[$(ts_utc)] [DEBUG] $message"
        printf '%s\n' "$line"
        trace_append_global_log "$line"
    fi
}

log_running_processes() {
    local context="${1:-snapshot}"
    if [[ "$VERBOSE" == "1" || "$DEBUG" == "1" ]]; then
        log_debug "Prozessliste ($context): benchmark/json_benchmark/modsecurity"
        run_external benchmark_related -- ps -eo pid,ppid,etime,stat,comm,args \
            | run_external benchmark_related -- awk 'NR==1 || /benchmark|json_benchmark|modsecurity|nginx|apache2|httpd/' \
            | run_external benchmark_related -- tee -a "$GLOBAL_LOG"
    fi
}

trace_preexec() {
    return 0
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

trap error_handler ERR

if [[ "$DEBUG" == "1" ]]; then
    export PS4='+ [$(printf "%(%Y-%m-%dT%H:%M:%SZ)T" -1)] [xtrace:${BASH_SOURCE##*/}:${LINENO}] '
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
    if run_external benchmark_related -- awk -v v="$ns" 'BEGIN {exit !(v < 0)}'; then
        echo "n/a"
        return 1
    fi
    if [[ -n "$forced_unit" ]] && [[ ! "$forced_unit" =~ ^(ns|us|ms|s)$ ]]; then
        echo "n/a"
        return 1
    fi

    local formatted
    capture_external formatted benchmark_related -- awk -v ns="$ns" -v d="$decimals" -v forced="$forced_unit" '
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
    printf '%s\n' "$formatted"
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
    capture_external ns benchmark_related -- awk -v s="$sec" 'BEGIN { printf "%.12f", s * 1000000000 }'
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
    if run_external benchmark_related -- awk -v v="$value" 'BEGIN {exit !(v < 0)}'; then
        echo "invalid_negative($value)"
        return 1
    fi

    local formatted
    capture_external formatted benchmark_related -- awk -v kb="$value" -v d="$decimals" '
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
    printf '%s\n' "$formatted"
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
    if run_external benchmark_related -- awk -v v="$value" 'BEGIN {exit !(v < 0)}'; then
        echo "invalid_negative($value)"
        return 1
    fi

    local formatted
    capture_external formatted benchmark_related -- awk -v v="$value" -v u="$unit" -v d="$decimals" '
    BEGIN {
        fmt = "%." d "f %s"
        printf fmt, v, u
    }'
    printf '%s\n' "$formatted"
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
    run_external benchmark_related -- cp "$BACKUP_DIR/basic_rules.conf.orig" "$BASIC_RULES"
    run_external benchmark_related -- cp "$BACKUP_DIR/json_benchmark_rules.conf.orig" "$JSON_RULES"
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
    if ! run_external benchmark_related -- "$DOWNLOAD_CRS_V3_SCRIPT" >> "$GLOBAL_LOG" 2>&1; then
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
    if ! run_external benchmark_related -- "$DOWNLOAD_CRS_V4_SCRIPT" >> "$GLOBAL_LOG" 2>&1; then
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
        if ! capture_external jq_output benchmark_related -- jq -r --arg k "$key" '
            if has($k) | not then "__JSON_GET_MISSING__"
            else .[$k] end
        ' <<< "$json_line" 2>/dev/null; then
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
    capture_external fallback_value benchmark_related -- sed -n "s/.*\"$key\":\([^,}]*\).*/\1/p" <<< "$json_line"
    fallback_value="${fallback_value//\"/}"
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

    run_external benchmark_related -- mkdir -p "$variant_dir" "$log_dir" "$raw_dir" "$metrics_dir" "$config_dir"

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

    local vstart_ns
    local vstart_iso
    capture_external vstart_ns benchmark_related -- date +%s%N
    vstart_iso="$(trace_now_utc)"

    log_step "START variant=$variant"
    printf '[%s] start variant=%s\n' "$vstart_iso" "$variant" >> "$variant_log"

    if ! validate_variant_dependencies "$variant"; then
        local current_ns failure_ts
        failure_ts="$(trace_now_utc)"
        printf '[%s] dependency check failed for variant=%s\n' "$failure_ts" "$variant" >> "$errors_log"
        VARIANT_STATUS["$variant"]="failed"
        capture_external current_ns benchmark_related -- date +%s%N
        VARIANT_DURATION_NS["$variant"]=$(( current_ns - vstart_ns ))
        return 1
    fi

    if ! prepare_variant_rules "$variant"; then
        local current_ns failure_ts
        failure_ts="$(trace_now_utc)"
        printf '[%s] rules preparation failed for variant=%s\n' "$failure_ts" "$variant" >> "$errors_log"
        VARIANT_STATUS["$variant"]="failed"
        capture_external current_ns benchmark_related -- date +%s%N
        VARIANT_DURATION_NS["$variant"]=$(( current_ns - vstart_ns ))
        return 1
    fi

    run_external benchmark_related -- cp "$BASIC_RULES" "$config_dir/effective_basic_rules.conf"
    run_external benchmark_related -- cp "$JSON_RULES" "$config_dir/effective_json_benchmark_rules.conf"
    write_variant_metadata "$variant" "$variant_dir"

    local bench_cmd=("$BENCH_BIN" "$BENCH_ITERATIONS" "--scenario" "legacy-full" "--rules-file" "$BASIC_RULES")
    log_info "Starte benchmark binary für variant=$variant"
    log_running_processes "before benchmark variant=$variant"
    printf 'command=%q ' "${bench_cmd[@]}" >> "$commands_log"
    printf '\n' >> "$commands_log"

    if run_external benchmark_binary -- "${bench_cmd[@]}" > "$benchmark_raw" 2>> "$errors_log"; then
        printf 'exit_code=0\n' >> "$commands_log"
    else
        local bench_exit=$?
        local current_ns failure_ts
        printf 'exit_code=%s\n' "$bench_exit" >> "$commands_log"
        failure_ts="$(trace_now_utc)"
        printf '[%s] benchmark command failed exit=%s\n' "$failure_ts" "$bench_exit" >> "$errors_log"
        VARIANT_STATUS["$variant"]="failed"
        capture_external current_ns benchmark_related -- date +%s%N
        VARIANT_DURATION_NS["$variant"]=$(( current_ns - vstart_ns ))
        return 1
    fi
    log_running_processes "after benchmark variant=$variant"

    local elapsed_s avg_ns throughput interventions
    elapsed_s="$(
        run_external benchmark_related -- awk '/elapsed_seconds:/ {print $2}' "$benchmark_raw" \
            | run_external benchmark_related -- tail -n1
    )"
    avg_ns="$(
        run_external benchmark_related -- awk '/avg_transaction_ns:/ {print $2}' "$benchmark_raw" \
            | run_external benchmark_related -- tail -n1
    )"
    throughput="$(
        run_external benchmark_related -- awk '/throughput_tx_per_sec:/ {print $2}' "$benchmark_raw" \
            | run_external benchmark_related -- tail -n1
    )"
    interventions="$(
        run_external benchmark_related -- awk '/interventions:/ {print $2}' "$benchmark_raw" \
            | run_external benchmark_related -- tail -n1
    )"

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

            if capture_external output json_benchmark_binary -- "${json_cmd[@]}" 2>> "$errors_log"; then
                json_exit=0
            else
                json_exit=$?
            fi
            printf 'exit_code=%s\n' "$json_exit" >> "$commands_log"
            log_running_processes "after json scenario=$scenario size=$size"

            if [[ "$json_exit" -ne 0 ]]; then
                local current_ns failure_ts
                failure_ts="$(trace_now_utc)"
                printf '[%s] json_benchmark failed variant=%s scenario=%s size=%s include_invalid=%s exit=%s\n' \
                    "$failure_ts" "$variant" "$scenario" "$size" "$include_invalid" "$json_exit" >> "$errors_log"
                VARIANT_STATUS["$variant"]="failed"
                capture_external current_ns benchmark_related -- date +%s%N
                VARIANT_DURATION_NS["$variant"]=$(( current_ns - vstart_ns ))
                return 1
            fi

            printf '%s\n' "$output" >> "$json_raw"

            if ! process_ns="$(json_get "process_request_body_ns" "$output" 2>> "$errors_log")" \
                || ! total_ns="$(json_get "total_transaction_ns" "$output" 2>> "$errors_log")" \
                || ! rss_kb="$(json_get "ru_maxrss_kb" "$output" 2>> "$errors_log")" \
                || ! success_count="$(json_get "parse_success_count" "$output" 2>> "$errors_log")" \
                || ! error_count="$(json_get "parse_error_count" "$output" 2>> "$errors_log")"; then
                local current_ns failure_ts
                failure_ts="$(trace_now_utc)"
                printf '[%s] json metrics parse failed variant=%s scenario=%s size=%s include_invalid=%s\n' \
                    "$failure_ts" "$variant" "$scenario" "$size" "$include_invalid" >> "$errors_log"
                VARIANT_STATUS["$variant"]="failed"
                capture_external current_ns benchmark_related -- date +%s%N
                VARIANT_DURATION_NS["$variant"]=$(( current_ns - vstart_ns ))
                return 1
            fi
            capture_external derived_tps benchmark_related -- awk -v it="$JSON_ITERATIONS" -v ns="$total_ns" 'BEGIN{ if (ns > 0) printf "%.6f", it / (ns / 1000000000); else print "0" }'
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

            if capture_external output json_benchmark_binary -- "${json_cmd[@]}" 2>> "$errors_log"; then
                json_exit=0
            else
                json_exit=$?
            fi
            printf 'exit_code=%s\n' "$json_exit" >> "$commands_log"
            log_running_processes "after json invalid scenario=$scenario size=$size"

            if [[ "$json_exit" -ne 0 ]]; then
                local current_ns failure_ts
                failure_ts="$(trace_now_utc)"
                printf '[%s] json_benchmark failed variant=%s scenario=%s size=%s include_invalid=%s exit=%s\n' \
                    "$failure_ts" "$variant" "$scenario" "$size" "$include_invalid" "$json_exit" >> "$errors_log"
                VARIANT_STATUS["$variant"]="failed"
                capture_external current_ns benchmark_related -- date +%s%N
                VARIANT_DURATION_NS["$variant"]=$(( current_ns - vstart_ns ))
                return 1
            fi

            printf '%s\n' "$output" >> "$json_raw"

            if ! process_ns="$(json_get "process_request_body_ns" "$output" 2>> "$errors_log")" \
                || ! total_ns="$(json_get "total_transaction_ns" "$output" 2>> "$errors_log")" \
                || ! rss_kb="$(json_get "ru_maxrss_kb" "$output" 2>> "$errors_log")" \
                || ! success_count="$(json_get "parse_success_count" "$output" 2>> "$errors_log")" \
                || ! error_count="$(json_get "parse_error_count" "$output" 2>> "$errors_log")"; then
                local current_ns failure_ts
                failure_ts="$(trace_now_utc)"
                printf '[%s] json metrics parse failed variant=%s scenario=%s size=%s include_invalid=%s\n' \
                    "$failure_ts" "$variant" "$scenario" "$size" "$include_invalid" >> "$errors_log"
                VARIANT_STATUS["$variant"]="failed"
                capture_external current_ns benchmark_related -- date +%s%N
                VARIANT_DURATION_NS["$variant"]=$(( current_ns - vstart_ns ))
                return 1
            fi
            capture_external derived_tps benchmark_related -- awk -v it="$JSON_ITERATIONS" -v ns="$total_ns" 'BEGIN{ if (ns > 0) printf "%.6f", it / (ns / 1000000000); else print "0" }'
            total_dynamic="$(format_time_dynamic_from_ns "$total_ns")"
            rss_dynamic="$(format_memory_kb "$rss_kb" 2 || true)"
            derived_tps_fmt="$(format_throughput "$derived_tps" "iter/s" 2 || true)"

            echo "$variant,$scenario,$size,$JSON_ITERATIONS,$include_invalid,$process_ns,$total_ns,$total_dynamic,$rss_kb,$rss_dynamic,$success_count,$error_count,$derived_tps,$derived_tps_fmt" >> "$json_metrics_csv"
        done
    done

    local json_avg_total_ns json_avg_tps
    capture_external json_avg_total_ns benchmark_related -- awk -F, 'NR>1 {sum+=$7; n++} END { if (n>0) printf "%.2f", sum/n; else print "0" }' "$json_metrics_csv"
    capture_external json_avg_tps benchmark_related -- awk -F, 'NR>1 {sum+=$11; n++} END { if (n>0) printf "%.6f", sum/n; else print "0" }' "$json_metrics_csv"

    local vend_ns
    local vend_iso
    capture_external vend_ns benchmark_related -- date +%s%N
    vend_iso="$(trace_now_utc)"
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
        capture_external os_pretty benchmark_related -- awk -F= '/^PRETTY_NAME=/{gsub(/"/,"",$2); print $2; exit}' /etc/os-release
        capture_external os_name benchmark_related -- awk -F= '/^NAME=/{gsub(/"/,"",$2); print $2; exit}' /etc/os-release
        capture_external os_version benchmark_related -- awk -F= '/^VERSION=/{gsub(/"/,"",$2); print $2; exit}' /etc/os-release
        capture_external os_version_id benchmark_related -- awk -F= '/^VERSION_ID=/{gsub(/"/,"",$2); print $2; exit}' /etc/os-release
        os_pretty="${os_pretty:-not_available}"
        os_name="${os_name:-not_available}"
        os_version="${os_version:-not_available}"
        os_version_id="${os_version_id:-not_available}"
    fi

    if ! capture_external uname_s benchmark_related -- uname -s 2>/dev/null; then
        uname_s="not_available"
    fi
    if ! capture_external uname_r benchmark_related -- uname -r 2>/dev/null; then
        uname_r="not_available"
    fi
    if ! capture_external uname_m benchmark_related -- uname -m 2>/dev/null; then
        uname_m="not_available"
    fi
    if ! capture_external uname_a benchmark_related -- uname -a 2>/dev/null; then
        uname_a="not_available"
    fi

    if ! capture_external host_name benchmark_related -- hostname 2>/dev/null; then
        host_name="not_available"
    fi
    run_utc_now="$(trace_now_utc)"
    if [[ -n "${USER:-}" ]]; then
        user_name="$USER"
    elif ! capture_external user_name benchmark_related -- id -un 2>/dev/null; then
        user_name="not_available"
    fi

    if command -v lscpu >/dev/null 2>&1; then
        cpu_model="$(
            run_external_env benchmark_related --env LC_ALL=C -- lscpu 2>/dev/null \
                | run_external benchmark_related -- awk -F: '/Model name:/{sub(/^[ \t]+/,"",$2); print $2; exit}'
        )"
        cpu_arch="$(
            run_external_env benchmark_related --env LC_ALL=C -- lscpu 2>/dev/null \
                | run_external benchmark_related -- awk -F: '/Architecture:/{sub(/^[ \t]+/,"",$2); print $2; exit}'
        )"
        cpu_logical="$(
            run_external_env benchmark_related --env LC_ALL=C -- lscpu 2>/dev/null \
                | run_external benchmark_related -- awk -F: '/^CPU\\(s\\):/{sub(/^[ \t]+/,"",$2); print $2; exit}'
        )"
        local cores_per_socket sockets
        cores_per_socket="$(
            run_external_env benchmark_related --env LC_ALL=C -- lscpu 2>/dev/null \
                | run_external benchmark_related -- awk -F: '/Core\\(s\\) per socket:/{sub(/^[ \t]+/,"",$2); print $2; exit}'
        )"
        sockets="$(
            run_external_env benchmark_related --env LC_ALL=C -- lscpu 2>/dev/null \
                | run_external benchmark_related -- awk -F: '/Socket\\(s\\):/{sub(/^[ \t]+/,"",$2); print $2; exit}'
        )"
        if [[ "$cores_per_socket" =~ ^[0-9]+$ ]] && [[ "$sockets" =~ ^[0-9]+$ ]]; then
            cpu_physical_cores="$((cores_per_socket * sockets))"
        fi
        cpu_freq_mhz="$(
            run_external_env benchmark_related --env LC_ALL=C -- lscpu 2>/dev/null \
                | run_external benchmark_related -- awk -F: '
                    /CPU max MHz:/{sub(/^[ \t]+/,"",$2); if ($2 != "") {print $2; exit}}
                    /CPU MHz:/{sub(/^[ \t]+/,"",$2); if ($2 != "") {print $2; exit}}
                '
        )"
    fi

    if [[ "$cpu_model" == "not_available" || -z "$cpu_model" ]] && [[ -r "/proc/cpuinfo" ]]; then
        capture_external cpu_model benchmark_related -- awk -F: '/^model name/{sub(/^[ \t]+/,"",$2); print $2; exit}' /proc/cpuinfo
    fi
    if [[ "$cpu_arch" == "not_available" || -z "$cpu_arch" ]]; then
        cpu_arch="$uname_m"
    fi
    if [[ "$cpu_logical" == "not_available" || -z "$cpu_logical" ]]; then
        if command -v getconf >/dev/null 2>&1; then
            if ! capture_external cpu_logical benchmark_related -- getconf _NPROCESSORS_ONLN 2>/dev/null; then
                cpu_logical=""
            fi
        fi
        if [[ -z "$cpu_logical" ]] && command -v nproc >/dev/null 2>&1; then
            if ! capture_external cpu_logical benchmark_related -- nproc 2>/dev/null; then
                cpu_logical=""
            fi
        fi
        if [[ -z "$cpu_logical" ]] && [[ -r "/proc/cpuinfo" ]]; then
            capture_external cpu_logical benchmark_related -- awk -F: '/^processor/{count++} END{if (count>0) print count}' /proc/cpuinfo
        fi
    fi
    if [[ "$cpu_physical_cores" == "not_available" || -z "$cpu_physical_cores" ]] && [[ -r "/proc/cpuinfo" ]]; then
        capture_external cpu_physical_cores benchmark_related -- awk -F: '
            /^physical id/{pid=$2; gsub(/^[ \t]+/,"",pid)}
            /^cpu cores/{cores=$2; gsub(/^[ \t]+/,"",cores); if (pid != "" && cores ~ /^[0-9]+$/) map[pid]=cores}
            END {sum=0; for (k in map) sum+=map[k]; if (sum>0) print sum}
        ' /proc/cpuinfo
    fi
    if [[ "$cpu_freq_mhz" == "not_available" || -z "$cpu_freq_mhz" ]] && [[ -r "/proc/cpuinfo" ]]; then
        capture_external cpu_freq_mhz benchmark_related -- awk -F: '/^cpu MHz/{sub(/^[ \t]+/,"",$2); print $2; exit}' /proc/cpuinfo
    fi

    cpu_model="${cpu_model:-not_available}"
    cpu_arch="${cpu_arch:-not_available}"
    cpu_logical="${cpu_logical:-not_available}"
    cpu_physical_cores="${cpu_physical_cores:-not_available}"
    cpu_freq_mhz="${cpu_freq_mhz:-not_available}"

    if [[ -r "/proc/meminfo" ]]; then
        capture_external ram_total_kb benchmark_related -- awk '/^MemTotal:/{print $2; exit}' /proc/meminfo
        ram_total_kb="${ram_total_kb:-not_available}"
    fi

    if command -v systemd-detect-virt >/dev/null 2>&1; then
        local virt_out
        if ! capture_external virt_out benchmark_related -- systemd-detect-virt 2>/dev/null; then
            virt_out=""
        fi
        if [[ -n "$virt_out" && "$virt_out" != "none" ]]; then
            virtualization_hint="$virt_out"
        fi
    fi
    if [[ "$virtualization_hint" == "not_available" ]]; then
        if [[ -f "/.dockerenv" ]]; then
            virtualization_hint="docker_env_file_detected"
        elif [[ -r "/proc/1/cgroup" ]] && run_external benchmark_related -- grep -Eq '(docker|containerd|kubepods|lxc)' /proc/1/cgroup; then
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
        run_external benchmark_related -- awk -F, 'NR==1 {next} {printf "%-10s | %-8s | %-12s | %-14s | %-14s\n", $1, $2, $4, $7, $10}' "$GLOBAL_STRUCTURED"
    } >> "$GLOBAL_REPORT"
}

emit_trace_findings() {
    local -a lines=(
        'STATIC_FINDING_FIRST_EXTERNAL_COMMAND=date -u +"%Y%m%dT%H%M%SZ"'
        'STATIC_FINDING_FIRST_BENCHMARK_RELATED_EXTERNAL_COMMAND=date -u +"%Y%m%dT%H%M%SZ"'
        'STATIC_FINDING_FIRST_BENCHMARK_EXEC="${bench_cmd[@]}" > "$benchmark_raw" 2>> "$errors_log"'
        'LITERAL_DOT_SLASH_BENCHMARK_FOUND=no'
        "RUNTIME_FIRST_TRACE_COMMAND=${TRACE_FIRST_EXTERNAL_COMMAND_OVERALL:-not_observed}"
        "RUNTIME_FIRST_BENCHMARK_RELATED_COMMAND=${TRACE_FIRST_BENCHMARK_RELATED_EXTERNAL_COMMAND:-not_observed}"
        "RUNTIME_FIRST_BENCHMARK_COMMAND=${TRACE_FIRST_BENCHMARK_BINARY_EXECUTION:-not_observed}"
        'VERDICT=./benchmark wird NICHT als erster Befehl ausgeführt'
    )
    local line

    for line in "${lines[@]}"; do
        printf '%s\n' "$line"
    done

    {
        echo
        echo "Trace findings"
        for line in "${lines[@]}"; do
            printf '%s\n' "$line"
        done
    } >> "$RUN_META"

    {
        echo
        echo "Trace findings"
        for line in "${lines[@]}"; do
            printf '%s\n' "$line"
        done
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

    local total_end_ns
    local total_end_iso
    capture_external total_end_ns benchmark_related -- date +%s%N
    total_end_iso="$(trace_now_utc)"
    local total_duration_ns=$((total_end_ns - TOTAL_START_NS))
    local total_duration_fmt
    total_duration_fmt="$(format_time_dynamic_from_ns "$total_duration_ns")"

    build_comparison_outputs
    emit_trace_findings

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
