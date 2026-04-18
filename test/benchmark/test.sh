#!/usr/bin/env bash

set -euo pipefail

TS=$(date +"%Y-%m-%d_%H-%M-%S")

# ------------------------------------------------------------
# Pfade
# ------------------------------------------------------------
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BENCH_DIR="$SCRIPT_DIR"

BASIC_RULES="$BENCH_DIR/basic_rules.conf"
JSON_RULES="$BENCH_DIR/json_benchmark_rules.conf"

BACKUP_DIR="$BENCH_DIR/.benchmark_rule_backups_$TS"
RESULTS_ROOT="$BENCH_DIR/benchmark_runs_$TS"

mkdir -p "$BACKUP_DIR"
mkdir -p "$RESULTS_ROOT"

cp "$BASIC_RULES" "$BACKUP_DIR/basic_rules.conf.orig"
cp "$JSON_RULES" "$BACKUP_DIR/json_benchmark_rules.conf.orig"

# ------------------------------------------------------------
# Einstellungen
# ------------------------------------------------------------
BENCH_ITERATIONS="${BENCH_ITERATIONS:-1000000}"
JSON_ITERATIONS="${JSON_ITERATIONS:-100}"

SIZES=(256 4096 51200 1048576)
VALID_SCENARIOS=("utf8" "numbers" "deep-nesting" "large-object")
INVALID_SCENARIOS=("truncated" "malformed")

# CRS-Include-Pfade anpassen, falls bei dir anders
CRS_V3_SETUP="${CRS_V3_SETUP:-$BENCH_DIR/owasp-crs-v3/crs-setup.conf}"
CRS_V3_RULES_GLOB="${CRS_V3_RULES_GLOB:-$BENCH_DIR/owasp-crs-v3/rules/*.conf}"

CRS_V4_SETUP="${CRS_V4_SETUP:-$BENCH_DIR/owasp-crs-v4/crs-setup.conf}"
CRS_V4_RULES_GLOB="${CRS_V4_RULES_GLOB:-$BENCH_DIR/owasp-crs-v4/rules/*.conf}"

# ------------------------------------------------------------
# Hilfsfunktionen
# ------------------------------------------------------------
cleanup() {
    if [ -f "$BACKUP_DIR/basic_rules.conf.orig" ]; then
        cp "$BACKUP_DIR/basic_rules.conf.orig" "$BASIC_RULES"
    fi
    if [ -f "$BACKUP_DIR/json_benchmark_rules.conf.orig" ]; then
        cp "$BACKUP_DIR/json_benchmark_rules.conf.orig" "$JSON_RULES"
    fi
}
trap cleanup EXIT

log() {
    printf '[%s] %s\n' "$(date +"%H:%M:%S")" "$*"
}

log_raw() {
    local raw_file="$1"
    local title="$2"
    local content="$3"

    {
        echo "=================================================="
        echo "$title"
        echo "=================================================="
        echo "$content"
        echo
    } >> "$raw_file"
}

format_time_ns() {
    local ns="$1"
    awk -v ns="$ns" '
    BEGIN {
        if (ns < 1000) {
            printf "%.2f ns", ns
        } else if (ns < 1000000) {
            printf "%.2f us", ns / 1000
        } else if (ns < 1000000000) {
            printf "%.2f ms", ns / 1000000
        } else {
            printf "%.2f s", ns / 1000000000
        }
    }'
}

format_seconds() {
    local s="$1"
    awk -v s="$s" '
    BEGIN {
        if (s < 0.001) {
            printf "%.2f us", s * 1000000
        } else if (s < 1) {
            printf "%.2f ms", s * 1000
        } else {
            printf "%.2f s", s
        }
    }'
}

format_memory_kb() {
    local kb="$1"
    awk -v kb="$kb" '
    BEGIN {
        if (kb < 1024) {
            printf "%.2f KB", kb
        } else if (kb < 1048576) {
            printf "%.2f MB", kb / 1024
        } else {
            printf "%.2f GB", kb / 1048576
        }
    }'
}

format_throughput() {
    local tps="$1"
    awk -v tps="$tps" 'BEGIN { printf "%.2f tx/s", tps }'
}

json_get() {
    local key="$1"
    local input="$2"

    if command -v jq >/dev/null 2>&1; then
        echo "$input" | jq -r ".$key"
    else
        echo "$input" | sed -n "s/.*\"$key\":\([^,}]*\).*/\1/p" | tr -d '"'
    fi
}

write_csv_header() {
    local out_file="$1"
    echo "variant,type,scenario,size_bytes,iterations,elapsed,avg_transaction,throughput,process_request_body,total_transaction,memory,parse_success_count,parse_error_count" > "$out_file"
}

restore_original_rules() {
    cp "$BACKUP_DIR/basic_rules.conf.orig" "$BASIC_RULES"
    cp "$BACKUP_DIR/json_benchmark_rules.conf.orig" "$JSON_RULES"
}

append_crs_v3() {
    {
        echo
        echo "# --- CRS v3 includes added by benchmark runner ---"
        echo "Include \"$CRS_V3_SETUP\""
        echo "Include \"$CRS_V3_RULES_GLOB\""
    } >> "$BASIC_RULES"

    {
        echo
        echo "# --- CRS v3 includes added by benchmark runner ---"
        echo "Include \"$CRS_V3_SETUP\""
        echo "Include \"$CRS_V3_RULES_GLOB\""
    } >> "$JSON_RULES"
}

append_crs_v4() {
    {
        echo
        echo "# --- CRS v4 includes added by benchmark runner ---"
        echo "Include \"$CRS_V4_SETUP\""
        echo "Include \"$CRS_V4_RULES_GLOB\""
    } >> "$BASIC_RULES"

    {
        echo
        echo "# --- CRS v4 includes added by benchmark runner ---"
        echo "Include \"$CRS_V4_SETUP\""
        echo "Include \"$CRS_V4_RULES_GLOB\""
    } >> "$JSON_RULES"
}

activate_variant_rules() {
    local variant="$1"

    restore_original_rules

    case "$variant" in
        base)
            log "Aktiviere Variante: base (ohne CRS)"
            ;;
        crs_v3)
            log "Aktiviere Variante: crs_v3"
            append_crs_v3
            ;;
        crs_v4)
            log "Aktiviere Variante: crs_v4"
            append_crs_v4
            ;;
        *)
            echo "Unbekannte Variante: $variant" >&2
            exit 1
            ;;
    esac
}

run_benchmark_cmd() {
    local variant="$1"
    local out_file="$2"
    local raw_file="$3"

    log "Starte ./benchmark für $variant"
    local output
    output=$(./benchmark "$BENCH_ITERATIONS")

    log_raw "$raw_file" "benchmark variant=$variant iterations=$BENCH_ITERATIONS" "$output"

    local elapsed avg_ns tps
    elapsed=$(echo "$output" | awk '/elapsed_seconds:/ {print $2}')
    avg_ns=$(echo "$output" | awk '/avg_transaction_ns:/ {print $2}')
    tps=$(echo "$output" | awk '/throughput_tx_per_sec:/ {print $2}')

    local elapsed_fmt avg_fmt tps_fmt
    elapsed_fmt=$(format_seconds "$elapsed")
    avg_fmt=$(format_time_ns "$avg_ns")
    tps_fmt=$(format_throughput "$tps")

    echo "$variant,benchmark,all,0,$BENCH_ITERATIONS,\"$elapsed_fmt\",\"$avg_fmt\",\"$tps_fmt\",,,,," >> "$out_file"
}

run_json_cmd() {
    local variant="$1"
    local scenario="$2"
    local size="$3"
    local iterations="$4"
    local invalid_flag="$5"
    local out_file="$6"
    local raw_file="$7"

    log "Starte json_benchmark: variant=$variant scenario=$scenario size=$size iterations=$iterations invalid=$invalid_flag"

    local output
    if [ "$invalid_flag" = "yes" ]; then
        output=$(./json_benchmark --scenario "$scenario" --include-invalid --iterations "$iterations" --target-bytes "$size" --output json)
    else
        output=$(./json_benchmark --scenario "$scenario" --iterations "$iterations" --target-bytes "$size" --output json)
    fi

    log_raw "$raw_file" "json_benchmark variant=$variant scenario=$scenario size=$size iterations=$iterations invalid=$invalid_flag" "$output"

    local process_ns total_ns mem_kb success_count error_count
    process_ns=$(json_get "process_request_body_ns" "$output")
    total_ns=$(json_get "total_transaction_ns" "$output")
    mem_kb=$(json_get "ru_maxrss_kb" "$output")
    success_count=$(json_get "parse_success_count" "$output")
    error_count=$(json_get "parse_error_count" "$output")

    local process_fmt total_fmt mem_fmt
    process_fmt=$(format_time_ns "$process_ns")
    total_fmt=$(format_time_ns "$total_ns")
    mem_fmt=$(format_memory_kb "$mem_kb")

    echo "$variant,json,$scenario,$size,$iterations,,,\"$process_fmt\",\"$total_fmt\",\"$mem_fmt\",$success_count,$error_count" >> "$out_file"
}

run_variant() {
    local variant="$1"

    local variant_dir="$RESULTS_ROOT/$variant"
    local out_file="$variant_dir/results_${variant}_${TS}.csv"
    local raw_file="$variant_dir/raw_${variant}_${TS}.log"

    mkdir -p "$variant_dir"
    write_csv_header "$out_file"

    activate_variant_rules "$variant"

    run_benchmark_cmd "$variant" "$out_file" "$raw_file"

    for size in "${SIZES[@]}"; do
        for scenario in "${VALID_SCENARIOS[@]}"; do
            run_json_cmd "$variant" "$scenario" "$size" "$JSON_ITERATIONS" "no" "$out_file" "$raw_file"
        done

        for scenario in "${INVALID_SCENARIOS[@]}"; do
            run_json_cmd "$variant" "$scenario" "$size" "$JSON_ITERATIONS" "yes" "$out_file" "$raw_file"
        done
    done

    log "Fertig mit Variante $variant"
    log "CSV: $out_file"
    log "RAW: $raw_file"
}

# ------------------------------------------------------------
# Vorab-Prüfungen
# ------------------------------------------------------------
cd "$BENCH_DIR"

if [ ! -x "./benchmark" ]; then
    echo "Fehler: ./benchmark nicht gefunden oder nicht ausführbar" >&2
    exit 1
fi

if [ ! -x "./json_benchmark" ]; then
    echo "Fehler: ./json_benchmark nicht gefunden oder nicht ausführbar" >&2
    exit 1
fi

if [ ! -f "$BASIC_RULES" ]; then
    echo "Fehler: $BASIC_RULES fehlt" >&2
    exit 1
fi

if [ ! -f "$JSON_RULES" ]; then
    echo "Fehler: $JSON_RULES fehlt" >&2
    exit 1
fi

# ------------------------------------------------------------
# Lauf
# ------------------------------------------------------------
log "Benchmark-Run startet"
log "Ergebnisse unter: $RESULTS_ROOT"

run_variant "base"
run_variant "crs_v3"
run_variant "crs_v4"

log "Alles fertig"
log "Root-Ergebnisordner: $RESULTS_ROOT"