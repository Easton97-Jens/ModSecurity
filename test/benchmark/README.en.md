# Benchmark Runner (`test/benchmark/test.sh`) – English Documentation

## Script purpose
`test/benchmark/test.sh` runs three benchmark variants in **one full run**, with strict separation:
1. `baseline` (without CRS)
2. `crs_v3` (with OWASP CRS v3 includes)
3. `crs_v4` (with OWASP CRS v4 includes)

The script produces traceable raw outputs, structured metrics, and readable summary reports per variant and globally.

## Overview of the 3 variants
- **baseline**
  - Uses original `basic_rules.conf` and `json_benchmark_rules.conf` only.
- **crs_v3**
  - Appends CRS v3 include lines to both rule files.
  - Default paths:
    - `test/benchmark/owasp-v3/crs-setup.conf.example`
    - `test/benchmark/owasp-v3/rules/*.conf`
- **crs_v4**
  - Appends CRS v4 include lines to both rule files.
  - Default paths:
    - `test/benchmark/owasp-v4/crs-setup.conf.example`
    - `test/benchmark/owasp-v4/rules/*.conf`

All paths are overrideable via environment variables (`CRS_V3_SETUP`, `CRS_V3_RULES_GLOB`, `CRS_V4_SETUP`, `CRS_V4_RULES_GLOB`).

## Why strict separation is enforced
Separation is implemented by:
- Rebuilding active rule files from pristine backups for each variant.
- Dedicated output tree per variant (`results/<run-ts>/<variant>/...`).
- Dedicated logs, raw outputs, and metrics files per variant.
- Variant-level status and error tracking.

This prevents cross-variant mixing and hidden side effects.

## Full run flow
1. Preflight checks (required binaries and rule files exist).
2. Backup original rule files.
3. Run `baseline`, `crs_v3`, `crs_v4` sequentially.
4. Per variant:
   - dependency validation (CRS files for CRS variants)
   - rule preparation
   - `benchmark` execution
   - `json_benchmark` execution for all sizes/scenarios
   - parsing and metrics generation
5. Build global comparison CSV and text summary.
6. Restore original rule files via `trap`.

## Result directory structure
Default root:
- `test/benchmark/results/<YYYYMMDDTHHMMSSZ>/`

Example structure:
- `run.log` (global run log)
- `run_metadata.txt` (global metadata)
- `comparison.csv` (structured comparison across variants)
- `report_summary.txt` (human-readable summary)
- `<variant>/`
  - `metadata.txt`
  - `config/effective_basic_rules.conf`
  - `config/effective_json_benchmark_rules.conf`
  - `logs/variant.log`
  - `logs/commands.log`
  - `logs/errors.log`
  - `raw/benchmark.raw.txt`
  - `raw/json_benchmark.jsonl`
  - `metrics/benchmark_metrics.csv`
  - `metrics/json_metrics.csv`
  - `metrics/parsed_summary.txt`

## Logging design
### Global logging
- full-run start/end
- overall status
- total duration
- output paths for final reports

### Per-variant logging
- start/end timestamps and duration
- executed commands (`commands.log`)
- command exit codes
- errors/warnings (`errors.log`)
- metadata snapshot (`metadata.txt`)
- archived effective rule files (`config/`)

## Reporting design
### Raw outputs
- `benchmark.raw.txt`: original output from `./benchmark`
- `json_benchmark.jsonl`: line-delimited JSON output from `./json_benchmark`

### Structured outputs
- `benchmark_metrics.csv`:
  - elapsed seconds, avg ns/transaction, throughput, interventions
- `json_metrics.csv`:
  - per scenario/size JSON metrics and derived throughput

### Human-readable summary
- Global summary: `report_summary.txt`
- Includes baseline vs crs_v3 vs crs_v4 comparison view

## Metric definitions
- **elapsed_seconds**: total time for `BENCH_ITERATIONS` transactions (`benchmark`)
- **avg_transaction_ns**: average transaction duration
- **throughput_tx_per_sec**: transactions/second
- **interventions**: intervention count reported by `benchmark`
- **process_request_body_ns / total_transaction_ns**: JSON benchmark timings
- **ru_maxrss_kb**: max RSS (KB) from `json_benchmark`

## Dynamic time formatting derivation
The script scales time units dynamically:
- `< 1,000 ns` → `ns`
- `< 1,000,000 ns` → `µs` (`us` in output)
- `< 1,000,000,000 ns` → `ms`
- otherwise `s`

For second-based values, it internally converts to ns and applies the same rendering logic.

## Robust helper functions
- `format_memory_kb(value, decimals)`:
  - Accepts KB input and dynamically renders `B`, `KB`, `MB`, `GB`, `TB`.
  - Rounding is configurable (`decimals`).
  - Edge cases:
    - empty value → `n/a`
    - invalid value → `invalid(...)`
    - negative value → `invalid_negative(...)`
- `format_throughput(value, unit, decimals)`:
  - Unit is configurable (`tx/s`, `req/s`, `iter/s`, ...).
  - Rounding is configurable.
  - Edge cases are explicitly marked (same policy as memory formatting).
- `json_get(key, json_line)`:
  - prefers `jq` when available.
  - distinguishes via exit code + stderr message:
    - missing key,
    - empty/null value,
    - parse error.
  - non-`jq` fallback is intentionally limited and explicitly logged.
- `write_csv_header(file, columns...)`:
  - supports centralized/dynamic column definitions instead of a single hard-coded schema.
  - makes CSV layout easier to evolve.

## Throughput derivation
- `benchmark`: uses `throughput_tx_per_sec` directly from benchmark output.
- `json_benchmark` derived throughput:
  - `derived_tps = iterations / (total_transaction_ns / 1e9)`
  - persisted per row in `json_metrics.csv`.

## Error handling
- `set -euo pipefail` is enabled.
- Variant-level fail-fast on dependency or command errors.
- Errors are logged both per variant (`errors.log`) and globally (`run.log`).
- The script continues with remaining variants to provide full status visibility.
- Final exit code is `1` if any variant failed.

## Reproducibility notes
- Iterations are configurable (`BENCH_ITERATIONS`, `JSON_ITERATIONS`).
- Scenario and size sets are fixed and reused for every variant.
- Effective rule files are archived per variant.
- Original rule files are restored at the end.

## Example invocations
```bash
cd test/benchmark
./test.sh
```

With custom iterations:
```bash
BENCH_ITERATIONS=200000 JSON_ITERATIONS=50 ./test.sh
```

With custom output root:
```bash
RESULTS_ROOT="$(pwd)/results/custom-run" ./test.sh
```

## Example output (short)
`comparison.csv`:
```csv
variant,status,duration_ns,duration_dynamic,benchmark_elapsed_seconds,benchmark_elapsed_dynamic,benchmark_throughput_tx_per_sec,json_avg_total_transaction_ns,json_avg_total_transaction_dynamic,json_avg_derived_throughput_tx_per_sec
baseline,ok,1234567890,1.23 s,0.81,810.00 ms,1234567.89,54231.11,54.23 us,1854321.120000
crs_v3,ok,2234567890,2.23 s,1.52,1.52 s,657894.12,88321.44,88.32 us,1154321.550000
crs_v4,ok,2134567890,2.13 s,1.44,1.44 s,694444.44,81234.78,81.23 us,1234567.890000
```

## Extending with additional variants
To add a new variant:
1. Add its name to `VARIANTS`.
2. Add corresponding logic in `validate_variant_dependencies` and `prepare_variant_rules`.
3. Optionally define new env vars for custom rule include paths.

Execution, logging, and reporting pipelines can remain unchanged.
