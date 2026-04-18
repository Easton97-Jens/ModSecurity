#!/usr/bin/env bash

set -euo pipefail

RUN_TS_UTC="$(date -u +"%Y%m%dT%H%M%SZ")"

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

mkdir -p "$RESULTS_ROOT" "$WORK_ROOT" "$BACKUP_DIR"

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

log() {
    local message="$1"
    local ts
    ts="$(date -u +"%Y-%m-%dT%H:%M:%SZ")"
    printf '[%s] %s\n' "$ts" "$message" | tee -a "$GLOBAL_LOG"
}

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

    log "START variant=$variant"
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
        for scenario in "${VALID_SCENARIOS[@]}"; do
            include_invalid="no"
            local json_cmd=("$JSON_BENCH_BIN" "--scenario" "$scenario" "--iterations" "$JSON_ITERATIONS" "--target-bytes" "$size" "--output" "json")
            printf 'command=%q ' "${json_cmd[@]}" >> "$commands_log"
            printf '\n' >> "$commands_log"

            if output="$("${json_cmd[@]}" 2>> "$errors_log")"; then
                json_exit=0
            else
                json_exit=$?
            fi
            printf 'exit_code=%s\n' "$json_exit" >> "$commands_log"

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
            printf 'command=%q ' "${json_cmd[@]}" >> "$commands_log"
            printf '\n' >> "$commands_log"

            if output="$("${json_cmd[@]}" 2>> "$errors_log")"; then
                json_exit=0
            else
                json_exit=$?
            fi
            printf 'exit_code=%s\n' "$json_exit" >> "$commands_log"

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
    [[ -x "$BENCH_BIN" ]] || { echo "Fehler: benchmark binary fehlt: $BENCH_BIN" >&2; exit 1; }
    [[ -x "$JSON_BENCH_BIN" ]] || { echo "Fehler: json_benchmark binary fehlt: $JSON_BENCH_BIN" >&2; exit 1; }
    [[ -f "$BASIC_RULES" ]] || { echo "Fehler: rules file fehlt: $BASIC_RULES" >&2; exit 1; }
    [[ -f "$JSON_RULES" ]] || { echo "Fehler: rules file fehlt: $JSON_RULES" >&2; exit 1; }
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
    } > "$RUN_META"
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
    preflight_checks
    write_global_metadata

    log "Benchmark Gesamtlauf gestartet"
    log "Results root: $RESULTS_ROOT"

    local overall_status="ok"
    local variant

    for variant in "${VARIANTS[@]}"; do
        if ! run_variant "$variant"; then
            overall_status="failed"
            log "Variant failed: $variant"
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

    log "Benchmark Gesamtlauf beendet"
    log "overall_status=$overall_status"
    log "total_duration=$total_duration_fmt"
    log "comparison_csv=$GLOBAL_STRUCTURED"
    log "summary_report=$GLOBAL_REPORT"

    if [[ "$overall_status" != "ok" ]]; then
        return 1
    fi

    return 0
}

main "$@"
