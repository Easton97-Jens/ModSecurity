# Benchmark Runner


## 1. Overview
The benchmark system in `test/benchmark/` provides reproducible ModSecurity performance measurements. It combines:
- a general transaction benchmark (`benchmark`),
- a JSON-specific benchmark (`json_benchmark`),
- an orchestrator (`test.sh`) for full multi-variant comparison runs.

Its purpose is to compare rule configurations (baseline, CRS v3, CRS v4) with strictly separated artifacts per variant.

## 2. Architecture
### Components
| Component | Role |
|---|---|
| `benchmark` | Runs repeated HTTP transactions and reports runtime metrics (`elapsed_seconds`, `avg_transaction_ns`, `throughput_tx_per_sec`, `interventions`). |
| `json_benchmark` | Runs JSON request-body scenarios and reports metrics (including `process_request_body_ns`, `total_transaction_ns`, `parse_*`, `ru_maxrss_kb`). |
| `test.sh` | Executes one full run across `baseline`, `crs_v3`, `crs_v4`, collects raw data, parses metrics, and generates comparison reports. |
| `basic_rules.conf` | Rule base for `benchmark`. |
| `json_benchmark_rules.conf` | Rule base for `json_benchmark`. |
| CRS download scripts (`download-owasp-v3-rules.sh`, `download-owasp-v4-rules.sh`) | Fetch CRS repositories and enable include-based rule extension. |

### Interaction
1. `test.sh` validates binaries and rule files.
2. For each variant, rule files are restored from backups and CRS includes are optionally appended.
3. `benchmark` and `json_benchmark` run per variant.
4. Outputs are written to isolated per-variant folders plus global comparison reports.

## 3. Build Instructions
### Prerequisites (derived from scripts/build files)
- POSIX shell (`sh`/`bash`)
- Autotools toolchain (`autoreconf` flow via `build.sh`)
- C/C++ toolchain and `make`
- Git (for submodules)

A complete package-by-package dependency list is not centrally documented in the available files. **This cannot be verified from the available code.**

### Recommended command sequence
```bash
git submodule update --init --recursive
./build.sh
./configure
make
make check
```

### Why each step is needed
- `git submodule update --init --recursive`: fetches required submodules.
- `./build.sh`: generates/refreshes Autotools-generated files.
- `./configure`: creates platform-specific build configuration.
- `make`: builds the library and benchmark binaries.
- `make check`: runs available checks.

### Fallback: if benchmark binaries were not built by `make`
Check whether binaries exist:
```bash
ls -l test/benchmark/benchmark test/benchmark/json_benchmark
```

If one binary is missing or not executable, rebuild only the benchmark binaries:
```bash
make -C test/benchmark benchmark json_benchmark
```

Order/dependency note:
- If `./configure` or the main build has not been run yet, use the full build flow first (above).
- `make -C test/benchmark ...` is a targeted rebuild fallback, not a replacement for missing project configuration.

## 4. Running Benchmarks
### A) Direct usage

#### `benchmark`
Examples:
```bash
cd test/benchmark
./benchmark
./benchmark 1000
./benchmark 1000 --scenario legacy-full --rules-file basic_rules.conf
./benchmark 1000 --scenario request-only --rules-file basic_rules.conf
```

Supported parameters (per code usage string):
- optional positional iteration count
- `--scenario legacy-full|request-only`
- `--rules-file PATH`

Short parameter explanation:
- `num_iterations` (positional): positive integer; controls how many loop transactions are measured.
- `--scenario`: selects the flow (`legacy-full` with request+response phases or `request-only` with request phases).
- `--rules-file`: path to the rules file loaded for this run.

Output summary includes:
- `scenario`: indicates which benchmark flow actually ran (`legacy-full` or `request-only`), so results are tied to a specific transaction path.
- `rules_file`: indicates which rule file was loaded; this is required to interpret results against a concrete ruleset.
- `elapsed_seconds`: total loop runtime across all iterations; for equal iteration counts, lower values indicate faster overall execution.
- `avg_transaction_ns`: average per-transaction duration in nanoseconds; this normalizes runtime to a single transaction cost.
- `throughput_tx_per_sec`: runtime-derived throughput in transactions per second; higher values indicate more processed transactions per unit time.
- `interventions`: number of detected intervention events during the run; higher values indicate more frequent rule-triggered flow interventions.

#### `json_benchmark`
Examples:
```bash
cd test/benchmark
./json_benchmark --scenario utf8 --output json
./json_benchmark --scenario large-object --iterations 200 --target-bytes 1048576 --output json
./json_benchmark --scenario truncated --include-invalid --output json
```

Key parameters:
- `--scenario NAME` (required): selects the JSON scenario whose body is generated and measured; without it, the benchmark does not start.
- `--iterations N`: sets the number of repetitions; higher values typically stabilize averages but increase runtime.
- `--target-bytes N`: sets target payload size for size-driven scenarios; this controls JSON processing workload size.
- `--depth N`: sets nesting depth for `deep-nesting`; relevant when evaluating deeply nested JSON structures.
- `--include-invalid`: enables intentionally invalid JSON scenarios (`truncated`, `malformed`) and is required for those scenarios.
- `--output json`: switches to machine-readable JSON output; important for automated parsing in the runner.

### B) Full runner (`test.sh`)
```bash
cd test/benchmark
./test.sh
```

`test.sh` exists to produce a standardized multi-variant run with isolated logs/artifacts and a combined comparison view. It is broader and more reproducible than manual one-off runs.

`test.sh` starts:
- one `benchmark` run per variant,
- multiple `json_benchmark` runs per variant across fixed sizes and scenarios,
- parsing/CSV generation plus global comparison output.

Default script configuration:
- variants: `baseline`, `crs_v3`, `crs_v4`
- sizes: `256`, `4096`, `51200`, `1048576`
- valid JSON scenarios: `utf8`, `numbers`, `deep-nesting`, `large-object`
- invalid JSON scenarios: `truncated`, `malformed` (run with `--include-invalid`)
- iterations:
  - `BENCH_ITERATIONS` default: `1000000`
  - `JSON_ITERATIONS` default: `100`

Why separate sizes/scenarios:
- different sizes expose scaling behavior (small to large payloads),
- valid scenarios represent normal parsing paths,
- invalid scenarios exercise parser/error handling paths.

## 5. Variants
| Variant | Rule state |
|---|---|
| `baseline` | Original `basic_rules.conf` + `json_benchmark_rules.conf` |
| `crs_v3` | Original rules plus CRS v3 include lines |
| `crs_v4` | Original rules plus CRS v4 include lines |

Separation is guaranteed by:
- restoring original backups before each variant,
- variant-specific output folders,
- separate logs/raw/metrics files.

## 6. Execution Flow
1. Build (see section 3).
2. `test.sh` starts with preflight checks (binaries and rule files exist / are usable).
3. `test.sh` can auto-download missing CRS v3/v4 rules (only when missing).
4. Original rule files are backed up; restore is handled via `trap`.
5. Per variant:
   - rule preparation (baseline/crs_v3/crs_v4),
   - `benchmark` run,
   - `json_benchmark` runs across all configured sizes and scenarios.
6. JSON output is parsed; per-variant metrics are written to CSV/TXT artifacts.
7. Global comparison artifacts are generated (`comparison.csv`, `report_summary.txt`).
8. Original rule files are restored at the end.

## 7. Output & Results
Default output root:
- `test/benchmark/results/<timestamp>/`

Structure:
- `run.log`: global run log
- `run_metadata.txt`: global metadata
- `system_info.txt`: run-time system/environment data (OS, kernel, host, CPU, RAM, virtualization hint)
- `comparison.csv`: structured variant comparison
- `report_summary.txt`: human-readable summary
- `<variant>/`
  - `metadata.txt`
  - `config/effective_basic_rules.conf`
  - `config/effective_json_benchmark_rules.conf`
  - `logs/variant.log`, `logs/commands.log`, `logs/errors.log`
  - `raw/benchmark.raw.txt`, `raw/json_benchmark.jsonl`
  - `metrics/benchmark_metrics.csv`, `metrics/json_metrics.csv`, `metrics/parsed_summary.txt`

## 8. Metrics
### `benchmark`
- `elapsed_seconds`: total runtime of the benchmark loop; with the same iteration count, lower is faster.
- `avg_transaction_ns`: average transaction duration in ns (`elapsed / iterations`).
- `throughput_tx_per_sec`: processed transactions per second; higher is better.
- `interventions`: number of detected interventions.

### `json_benchmark`
- `append_request_body_ns`: cumulative time spent in `appendRequestBody` across all iterations.
- `process_request_body_ns`: cumulative time spent in `processRequestBody` across all iterations.
- `total_transaction_ns`: cumulative end-to-end JSON transaction time across all iterations.
- `parse_success_count`: number of iterations with successful JSON processing outcome.
- `parse_error_count`: number of iterations with JSON error outcome.
- `ru_maxrss_kb`: process-reported max RSS in KB.

The runner records these per scenario/size and adds derived/formatted fields (for example `total_transaction_dynamic`, `derived_throughput_tx_per_sec`).

## 9. Formatting Logic
- Time formatting: single ns-based pipeline with dynamic unit selection (`ns`, `us`, `ms`, `s`) and boundary-aware rounding.
- Throughput formatting: consistent output with configurable unit (for example `tx/s`, `iter/s`).
- Memory formatting: `format_memory_kb` dynamically scales from KB up to TB.

Reasoning: reports should be readable while preserving raw values in raw/CSV files.

## 10. Logging & Traceability
- Global: start/end, status, report paths.
- Per variant: start/end, commands, exit codes, errors, metadata.
- Effective per-variant rule files are archived in `config/` to preserve exact run configuration.
- The system baseline is captured per run in `system_info.txt` and referenced/partly mirrored in `run_metadata.txt`:
  - distribution/version from `/etc/os-release` (when available),
  - kernel/platform from `uname`,
  - host/UTC timestamp/user context,
  - CPU data from `lscpu` and/or `/proc/cpuinfo`,
  - RAM from `/proc/meminfo`,
  - optional virtualization hint from reliable indicators.
- These values are factual environment metadata and do not, by themselves, prove “real hardware”.

## 11. Error Handling
- `set -euo pipefail` is enabled.
- Preflight performs fail-fast checks for core artifacts (binaries/rule files).
- Variant failures are logged; the overall run continues with remaining variants and exits with failure status if any variant failed.

## 12. Reproducibility
Key environment variables:
- `BENCH_ITERATIONS`
- `JSON_ITERATIONS`
- `RESULTS_ROOT`
- `CRS_V3_SETUP`, `CRS_V3_RULES_GLOB`
- `CRS_V4_SETUP`, `CRS_V4_RULES_GLOB`

In addition, fixed scenario/size sets and stored effective rule snapshots support reproducible comparisons.

## 13. Examples
### Example run
```bash
cd test/benchmark
BENCH_ITERATIONS=200000 JSON_ITERATIONS=50 ./test.sh
```

### Example (shortened) `comparison.csv`
```csv
variant,status,duration_ns,duration_dynamic,benchmark_elapsed_seconds,benchmark_elapsed_dynamic,benchmark_throughput_tx_per_sec,json_avg_total_transaction_ns,json_avg_total_transaction_dynamic,json_avg_derived_throughput_tx_per_sec
baseline,ok,1234567890,1.23 s,0.81,810.00 ms,1234567.89,54231.11,54.23 us,1854321.120000
crs_v3,ok,2234567890,2.23 s,1.52,1.52 s,657894.12,88321.44,88.32 us,1154321.550000
crs_v4,ok,2134567890,2.13 s,1.44,1.44 s,694444.44,81234.78,81.23 us,1234567.890000
```

## 14. Limitations
Out of scope for this system (not implemented in the current code):
- distributed load generation,
- real-traffic replay,
- latency percentiles (p95/p99),
- end-to-end network latency measurement.

## 15. Extending the System
- Add variant: extend `VARIANTS`, then update `validate_variant_dependencies` and `prepare_variant_rules`.
- Extend metrics: add parsing/CSV emission in runner logic.
- Modify rules: update `basic_rules.conf` / `json_benchmark_rules.conf` or set CRS include env vars.

### Final check
- No invented features are described.
- Documented components match files/scripts in the repository.
- German and English sections are structurally identical.
