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

## Measurement Strategy

### 1) Reproducible experiment design

**Minimum matrix:**
- Rule set: Minimal / CRS PL1 / CRS PL2+
- Payload: 1 KB / 16 KB / 256 KB / 2 MB
- Workload mix: static GET, JSON API, multipart upload
- Concurrency: 1, 8, 32, 128
- Logging mode: off / minimal / full audit

**A/B variants:**
- Baseline without WAF vs with WAF
- WAF with/without specific rule classes (e.g., regex-heavy groups)

### 2) Tooling

- **CPU hotspots:** `perf record` + `perf report`, then flamegraphs
- **Memory/allocations:** `valgrind --tool=massif`, optionally `heaptrack`
- **Syscalls/locking:** `perf lock`, `strace -c`
- **Optional eBPF:** uprobes on `RulesSet::evaluate`, `RuleWithOperator::evaluate`, `Regex::searchOneMatch`, `executeTransformations`

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

### 4) KPIs

- **Latency:** p50 / p95 / p99 (end-to-end and per phase)
- **CPU:** cycles/request, instructions/request, IPC
- **Memory:** bytes/request, peak RSS, allocations/request
- **Scalability:** throughput (RPS) vs concurrency, saturation point

### 5) Optimization acceptance criteria

An optimization is accepted if across 3 runs (95% confidence):
- p95 latency improves by at least 10% **or**
- cycles/request decrease by at least 15%
- with no regression in block/detection fidelity.

---

## Prioritization

### 1) Quantified bottleneck split (CPU share)

**Best case (small rule set, low regex pressure):**
- Regex engine: **20–35%**
- Transformation pipeline: **20–30%**
- Parsing: **15–25%**
- Logging: **5–15%**
- Locking/synchronization: **<5%**

**Worst case (CRS-heavy, high paranoia, large bodies):**
- Regex engine: **45–70%**
- Transformation pipeline: **15–30%**
- Parsing (incl. multipart): **10–20%**
- Logging: **5–15%**
- Locking/synchronization: **3–10%**

### 2) Weighted top-5 bottlenecks

Weight = expected total CPU impact in production-like CRS setups.

1. **Regex matching (`@rx`)** — **40%**
2. **Transformations + multiMatch amplification** — **22%**
3. **Variable expansion / target fanout (V)** — **14%**
4. **Body parsing (especially multipart, large payloads)** — **13%**
5. **Logging/serialization + I/O** — **11%**

Total: 100%.

### 3) Focus rules

If \(p_{rx} > 0.35\) or `T_regex_total / C_req > 0.4`, optimize regex first.

If `N_transforms * V` is high, prioritize transformation/target reduction.

If `audit_bytes_written` is high and tail latency dominates, optimize logging first.

---

## Optimizations with Impact

| Measure | Expected Impact | Estimated Improvement | Implementation Complexity | Notes |
|---|---:|---:|---|---|
| Regex prefilter (literal/ACMP before `@rx`) | High | **15–35%** CPU | Medium | Reduces expensive regex calls on easy non-matches |
| Rule grouping + early exits per phase | High | **10–25%** latency/CPU | Medium | Strong effect with large rule volumes |
| Target scope hardening (reduce `V`) | High | **10–30%** CPU | Low–Medium | Directly reduces \(R\cdot V\cdot T\) multiplier |
| Transformation fusion/caching | Medium–High | **8–20%** CPU | Medium–High | Must preserve exact semantics |
| Lazy `FULL_REQUEST` materialization | Medium | **5–15%** CPU+RAM | Low | Avoids large string copies |
| Streaming/chunk body representation | Medium–High | **10–25%** RAM/CPU | High | Larger refactor, high long-term value |
| Selective/async audit logging | Medium | **5–20%** tail latency | Low–Medium | Fast operational win |
| Collection locking optimization (batch/shard) | Low–Medium | **3–10%** at high concurrency | Medium | Relevant for write-heavy rules |

### Suggested rollout order

1. **Quick wins (1–2 sprints):** logging reduction, target-scope hardening, lazy `FULL_REQUEST`.
2. **Mid-term (2–4 sprints):** regex prefilter, phase gating, transformation optimization.
3. **Long-term:** streaming body refactor + deeper locking redesign.
