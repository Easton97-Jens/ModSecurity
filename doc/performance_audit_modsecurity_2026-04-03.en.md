# OWASP ModSecurity v3 – Technical Performance Audit (Code-focused)

This document summarizes an in-depth, code-based performance analysis of libmodsecurity, focusing on hot paths, rule engine behavior, memory characteristics, and scalability.

## Scope

Analyzed core paths:
- `Transaction` request/response lifecycle
- `RulesSet` + `RuleWithOperator` evaluation
- Request body processors (URLENCODED/JSON/XML/MULTIPART)
- Regex and pattern-matching operators (`@rx`, `@pm`)
- Collection backends and locking
- Audit logging and serialization

## Key Findings (Summary)

1. **Dominant CPU path**: `RulesSet::evaluate()` → `RuleWithOperator::evaluate()` → transformations → operator (`@rx`, `@pm`, ...).
2. **Regex is the primary cost driver** in CRS-heavy rule sets; match limits mitigate impact but do not eliminate expensive patterns.
3. **Significant string/copy overhead** in request-body and logging paths (`stringstream::str()`, header concatenation, JSON/audit serialization).
4. **Multipart parsing** is byte-wise and state-machine heavy, with high branching cost.
5. **Concurrency**: mostly lock-free per transaction; shared collections use `shared_mutex` and may contend in write-heavy workloads.

## Top Optimization Opportunities

- Replace `stringstream`-centric body handling with chunk-/span-based buffering.
- Compute `FULL_REQUEST` lazily or behind feature gates.
- Add prefilters (literal guards) before expensive regex operators.
- Cache/fuse frequent transformation pipelines.
- Reduce and/or async-offload audit logging where possible.

## Performance Model

### 1) Request Cost Model

We model total CPU cost per request as:

\[
C_{req} = C_{conn} + C_{parse} + C_{rules} + C_{log} + C_{sync}
\]

where:

- \(C_{conn}\): connection/context cost (small, near-constant)
- \(C_{parse}\): URI/header/body parsing
- \(C_{rules}\): rule evaluation (dominant)
- \(C_{log}\): audit/debug serialization
- \(C_{sync}\): locking/contention cost on shared collections

For the rule engine:

\[
C_{rules} = \sum_{r=1}^{R} \left( V_r \cdot \left(\sum_{t=1}^{T_r} C_{trans}(t)\right) + C_{op}(r) \right) + C_{actions}(r)
\]

In aggregated form:

\[
C_{rules} \approx R \cdot V \cdot (T \cdot \bar c_{trans} + \bar c_{op}) + R \cdot \bar c_{actions}
\]

with:
- \(R\): number of active rules per phase
- \(V\): average number of target values per rule
- \(T\): average number of transformations per rule
- \(\bar c_{op}\): average operator cost

This makes the multiplicative cost in \(R, V, T\) explicit.

### 2) Regex Cost Model

For regex-heavy workloads:

\[
\bar c_{op} = p_{rx}\cdot c_{rx} + (1-p_{rx})\cdot c_{other}
\]

where \(p_{rx}\) is the ratio of regex-based rules.

- **Best case (JIT + early fail/match):**
  \[
  c_{rx}^{best} = O(n)
  \]
- **Worst case (catastrophic backtracking):**
  \[
  c_{rx}^{worst} = O(e^n)
  \]
  practical behavior is bounded by match limits, but still expensive up to abort.

### 3) Big-O by subsystem

- **Rule evaluation (overall):**
  \[
  O\left(\sum_{r=1}^{R} V_r\cdot(T_r + O_r)\right)
  \]
  typically approximated as \(O(R\cdot V\cdot(T+O))\).
- **Parsing:**
  - URI/Header/Cookies: \(O(H + Q)\)
  - URL-encoded body: \(O(B)\)
  - Multipart: \(O(B\cdot\kappa)\), with \(\kappa\) as state/boundary-check overhead
- **Transformation pipeline:**
  \[
  O(R\cdot V\cdot T\cdot L)
  \]
  where \(L\) is average target string length.

---

## Measurement Limitations and Review Context

This document is a **code-focused qualitative performance audit**, not a completed benchmark report. The numeric CPU-share estimates that previously appeared in this section have been converted into measurement hypotheses after reviewer feedback, because no reproducible traces were attached to substantiate exact percentages.

Current limitations:
- No `perf` traces, flamegraphs, or profiler artifacts are attached to this report.
- No concrete CRS version, paranoia level, or payload corpus is provided with the document.
- No exact PCRE/PCRE2 version, JIT status, match-limit settings, concurrency level, or logging mode is pinned here.
- CPU shares can vary materially with CRS version, paranoia level, enabled rules, PCRE/PCRE2 JIT behavior, match limits, payload shape, body size, logging configuration, connector behavior, and concurrency.

Regex rules can be expensive in CRS-heavy or adversarial workloads, but this report should not be read as claiming a specific regex CPU percentage. Any such percentage must be validated with a reproducible benchmark and trace set. Reviewer reproduction feedback also indicates that PL=2 / large match-limit scenarios can show significant total slowdown while regex symbols themselves still scale reasonably, so the responsible subsystem remains an open measurement question.

---

## Measurement Strategy

### 1) Reproducible benchmark plan

A future benchmark intended to validate regex overhead or any other CPU-share hypothesis should pin the following inputs:

- **ModSecurity commit SHA:** exact libmodsecurity commit under test.
- **Connector and server version:** e.g., nginx/OpenResty connector version if the benchmark is end-to-end.
- **CRS version and paranoia level:** exact CRS tag/commit and PL1/PL2/PL3/PL4 setting.
- **PCRE/PCRE2 version and JIT status:** library version, build flags, JIT enabled/disabled, match-limit and recursion/depth-limit settings.
- **Exact rule set:** full list of loaded rule files plus local exclusions, disabled rules, and `SecRuleRemove*` directives.
- **Payload corpus:** benign traffic, attack traffic, adversarial/pathological regex payloads, JSON, URL-encoded forms, multipart uploads, and representative production-like headers/cookies.
  The `test/benchmark/benchmark` utility supports `--request-file` and `--request-dir` for replaying these raw HTTP request messages reproducibly.
- **Body sizes:** at minimum 0 B, 1 KB, 16 KB, 256 KB, 2 MB; include larger uploads only if deployment limits allow them.
- **Concurrency levels:** at minimum 1, 8, 32, 128 workers/clients, or deployment-specific equivalents.
- **Logging mode:** audit logging disabled, minimal relevant-only logging, and full audit logging as separate test arms.
- **Runtime environment:** CPU model, core count, kernel, compiler flags, allocator, container/cgroup limits, and CPU frequency governor.

### 2) Tooling

- **CPU hotspots:** `perf record` + `perf report`, then flamegraphs.
- **Memory/allocations:** `valgrind --tool=massif`, optionally `heaptrack`.
- **Syscalls/locking:** `perf lock`, `strace -c`.
- **Optional eBPF:** uprobes on `RulesSet::evaluate`, `RuleWithOperator::evaluate`, `Regex::searchOneMatch`, and `executeTransformations`.

Example trace workflow:

```sh
perf record -F 999 -g -- ./test/benchmark/benchmark 100000
perf report --stdio
perf script > perf.script
# Generate flamegraph with the standard FlameGraph stackcollapse/perl scripts.
```

For end-to-end nginx/OpenResty tests, run `perf record` against the worker process or the full benchmark command that drives traffic, and keep the request generator, configuration, rule set, and payload corpus under version control.

### 3) Instrumentation and metrics

For each request phase (1–5 + logging):

\[
T_{phase,i} = t_{end,i} - t_{start,i}
\]

Additional per-request metrics:
- `T_regex_total`, `N_regex_calls`, `T_regex_avg`
- `T_trans_total`, `N_transforms`
- `alloc_bytes`, `alloc_count`
- `audit_bytes_written`

To calculate regex share:

\[
regex\_share = \frac{T_{regex\_total}}{T_{request\_total}}
\]

Where:
- `T_regex_total` is the accumulated inclusive or exclusive time spent in regex execution; the report must state which one is used.
- `T_request_total` is the measured end-to-end transaction time or the sum of phase timers; the report must state which denominator is used.
- If using `perf` samples rather than wall-clock instrumentation, calculate sample share from clearly named regex symbols and include folded stacks/flamegraphs.

### 4) KPIs

- **Latency:** p50 / p95 / p99 (end-to-end and per phase).
- **CPU:** cycles/request, instructions/request, IPC, and sampled symbol share.
- **Memory:** bytes/request, peak RSS, allocations/request.
- **Scalability:** throughput (RPS) vs concurrency, saturation point.

### 5) Optimization acceptance criteria

An optimization should be evaluated across repeated runs with confidence intervals. Suggested gates:
- p95 latency improves materially in the target workload, **or**
- cycles/request decrease materially, **and**
- block/detection behavior does not regress.

Exact success thresholds should be set by the benchmark owner after baseline variance is measured.

---

## Prioritization

### 1) Expected bottleneck areas to validate

The following are **measurement hypotheses**, not measured CPU shares in this document:

- **Regex operators (`@rx`):** may become a hotspot in CRS-heavy, high-paranoia, or adversarial workloads, but reviewer feedback suggests regex may scale reasonably in some PL=2 / large match-limit tests; validate with regex symbol samples and per-operator timers before treating it as primary.
- **Transformation pipeline:** can dominate when many variables receive repeated transformations before operator evaluation; validate with transformation timers and allocation profiles.
- **Variable expansion / target fanout (`V`):** can multiply rule cost when broad collections such as ARGS, headers, cookies, and body-derived variables are evaluated repeatedly.
- **Body parsing:** URL-encoded, JSON, XML, and especially multipart parsing should be measured separately because payload shape can shift cost away from regex.
- **Logging and serialization:** can affect p95/p99 latency when audit parts include bodies, response data, or synchronous disk writes.
- **Collection locking/synchronization:** should be checked under high concurrency and write-heavy rules, but should not be assumed to dominate without lock traces.

### 2) Qualitative prioritization

Priority order to validate in most CRS-like deployments:

1. **Regex matching (`@rx`)** — high-priority hypothesis when regex-heavy rule groups are enabled or adversarial inputs are tested.
2. **Transformations + multiMatch amplification** — high-priority hypothesis when rule sets apply multiple transformations to broad targets.
3. **Variable expansion / target fanout** — high-priority hypothesis when rules inspect large collections or parsed body arguments.
4. **Body parsing** — workload-dependent; especially important for multipart and large request bodies.
5. **Logging/serialization + I/O** — workload-dependent; especially important for full audit logging and blocked-request traces.

### 3) Decision rules for future measurements

- If measured `T_regex_total / T_request_total` is high for the pinned workload, prioritize regex prefilters, regex rule review, and target narrowing.
- If `N_transforms * V` is high or transformation symbols dominate flamegraphs, prioritize transformation reduction/fusion and target narrowing.
- If `audit_bytes_written` correlates with tail latency, prioritize selective/asynchronous logging and audit-part reduction.
- If parsing symbols dominate, optimize body limits, parser configuration, and workload-specific request-body handling before changing regex rules.

---

## Optimizations with Expected Impact

The following table is qualitative until benchmark data is attached.

| Measure | Expected Impact | Implementation Complexity | Validation metric |
|---|---:|---|---|
| Regex prefilter (literal/ACMP before `@rx`) | Potentially high when regex share is measured as high | Medium | Lower `T_regex_total / T_request_total`, fewer regex calls |
| Rule grouping + early exits per phase | Potentially high with large rule volumes | Medium | Lower cycles/request and phase time |
| Target scope hardening (reduce `V`) | High when broad collections dominate | Low–Medium | Fewer variable values evaluated per rule |
| Transformation fusion/caching | Medium to high when transformation symbols dominate | Medium–High | Lower transformation time and allocations |
| Lazy `FULL_REQUEST` materialization | Medium when full-request variables are rarely used | Low | Lower allocations/request and peak RSS |
| Streaming/chunk body representation | Medium to high for large bodies | High | Lower peak RSS and body-copy time |
| Selective/async audit logging | Medium to high for full-audit or blocked-request workloads | Low–Medium | Lower p95/p99 latency and audit write time |
| Collection locking optimization (batch/shard) | Workload-dependent | Medium | Lower lock wait time under concurrency |

### Suggested rollout order

1. **Quick wins:** logging reduction, target-scope hardening, lazy `FULL_REQUEST`.
2. **Mid-term:** regex prefilter, phase gating, transformation optimization.
3. **Long-term:** streaming body refactor + deeper locking redesign.
