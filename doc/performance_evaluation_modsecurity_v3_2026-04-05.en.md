# OWASP ModSecurity (libmodsecurity v3) – Deep Technical Performance Evaluation (English)

> Scope: Engine-level behavior in libmodsecurity v3, with quantified estimates for CPU, RAM, I/O and scaling under typical enterprise CRS deployments.

## 0) Executive Summary

1. **Primary cost center is rule execution fanout**: effective CPU cost scales approximately with `R × V × T × O` (rules × target values × transformations × operator cost), with regex-heavy operator sets dominating runtime in CRS-like deployments.
2. **Biggest bottleneck is regex evaluation (`@rx`) under transformation-heavy pipelines**, especially when payload entropy and rule overlap are high.
3. **Biggest systemic risk is tail-latency collapse at high concurrency** when regex pressure, body parsing, and synchronous audit logging coincide.
4. **Best quick win optimization is regex call reduction** (prefilter/literal gating + target scope reduction), typically yielding **15–35% CPU reduction** and **10–25% p95 latency improvement** in rule-heavy profiles.
5. **Operationally, logging mode is a first-order tuning lever**: moving from full synchronous audit logging to selective/asynchronous modes can reduce p99 inflation by **5–20%**.

## 1) CPU Performance Analysis

### 1.1 Complexity and hotspots
- Dominant execution path: transaction phase loop -> per-rule evaluation -> per-variable extraction -> transformation chain -> operator execution.
- Approximate request CPU model:
  - `C_total ≈ C_parse + Σ_r Σ_v (Σ_t C_trans + C_op) + C_logging + C_sync`
  - In compact form: `C_total ≈ C_parse + R·V·(T·c_trans + c_op) + C_logging + C_sync`.
- Hotspots (descending):
  1. `@rx` operator execution and capture handling.
  2. Transformation pipeline (especially with multi-match semantics).
  3. Variable fanout and repeated evaluation over large collections.
  4. Multipart/stateful body parsing for large requests.

### 1.2 Estimated CPU distribution (rule-heavy CRS profile)
- Regex engine: **45–70%** (worst-case), **20–35%** (best-case tuned profile).
- Transformations: **15–30%**.
- Parsing (URI/headers/body): **10–25%**.
- Logging serialization in hot path: **5–15%**.
- Locking/sync overhead: **3–10%** (higher in write-heavy collection usage).

### 1.3 PCRE/JIT/backtracking behavior
- With stable patterns and JIT-friendly traffic: near-linear behavior at runtime for most matches/misses.
- Under adversarial inputs / pathological overlap: backtracking can dominate and explode tail latency until match limits cut execution.
- Best-case regex contribution: **~0.5–2.0 µs per call** (simple anchored patterns, warm instruction cache).
- Worst-case regex contribution: **10–1000+ µs per call** before limit-triggered abort in pathological cases.

## 2) RAM / Memory Analysis

### 2.1 Per-request memory profile
- Average request (small body, moderate headers): **~80–300 KB/request** incremental working set.
- Rule-heavy + body-inspection profile: **~300 KB–1.5 MB/request**.
- Large multipart uploads (buffering + metadata + temp-file bookkeeping): **~1.5–8 MB/request** transient peak.

### 2.2 Major memory drivers
- String duplication around full request composition and body extraction.
- Temporary objects: variable vectors, capture containers, transformed value copies.
- Repeated allocations for per-rule/per-variable evaluation loops.
- Multipart parser buffers and temporary file metadata structures.

### 2.3 Peak vs average and scaling
- Average memory scales roughly with active variable cardinality and body size.
- Peak memory scales superlinearly when large body buffering overlaps with high concurrency and verbose logging.
- At concurrency `N`, practical envelope:
  - `Mem_total ≈ Mem_base + N × Mem_req_avg + Mem_fragmentation`
  - Fragmentation/allocator pressure can add **10–30%** overhead under bursty loads.

## 3) I/O Performance Analysis

### 3.1 Audit logging and disk I/O
- Full audit logging can become the dominant I/O sink for blocked/high-detail traffic.
- Estimated per-request audit payload:
  - Minimal mode: **0.5–3 KB**
  - Full body/headers mode: **5–100+ KB** (payload dependent)
- At high RPS, synchronous writes can induce queueing and elevate p99 latency.

### 3.2 Temp-file and upload path I/O
- Multipart with file inspection creates disk pressure via temp-file lifecycle.
- Under sustained upload traffic, IOPS and fsync behavior may become limiting before CPU saturates.

### 3.3 Network overhead (reverse-proxy deployments)
- Reverse-proxy topologies add an extra hop and buffering domain.
- Typical additional network/service overhead: **+0.2–2.0 ms** median, **+1–10 ms** tail depending on topology and TLS placement.

### 3.4 Sync vs async logging impact
- Sync logging: stronger consistency, higher latency coupling.
- Async logging: better tail-latency isolation, potential loss window under crash scenarios.
- Expected effect of async + selective parts: **throughput +5–20%**, **p99 latency -5–25%**.

## 4) System Interaction (CPU + RAM + I/O)

- Bottlenecks amplify multiplicatively, not additively:
  - Higher regex time increases request residency -> higher concurrent memory footprint -> higher GC/allocator pressure -> longer queues for synchronous logging.
- Real-world under burst load:
  - Phase-2 body parsing and regex-heavy phase execution increase CPU stall time.
  - Concurrent logging/file I/O induces scheduler and storage contention.
- High concurrency effect:
  - Throughput scales near-linearly at low concurrency, then bends at regex + I/O saturation knee.
  - Typical knee region in rule-heavy profiles: **16–64 workers/threads**, environment-dependent.

## 5) Performance Heatmap

| Category | Score (0-10) | Impact | Bottleneck Severity |
|---|---:|---:|---:|
| CPU | 4.5 | Very High | Critical |
| RAM | 6.0 | Medium-High | Major |
| I/O | 5.5 | High in audit/upload-heavy profiles | Major |

### Interpretation
- **CPU is the limiting dimension** in most CRS-grade deployments due to regex/transformation amplification.
- **RAM is manageable in average traffic** but shows steep transient peaks under large payload + concurrency overlap.
- **I/O is scenario-sensitive**: benign under minimal logging, but can become first-order bottleneck with full audit and upload inspection.

## 6) Optimization Impact Table

| Optimization | Area | Impact (%) | Effort | Priority |
|---|---|---:|---|---|
| Regex prefilter (literal/ACMP gate before `@rx`) | CPU | 15–35 | Medium | P0 |
| Target scope reduction (`V` minimization) | CPU/RAM | 10–30 | Low-Medium | P0 |
| Transformation pipeline reduction/fusion | CPU | 8–20 | Medium-High | P1 |
| Lazy full-request materialization | RAM/CPU | 5–15 | Low | P1 |
| Selective + async audit logging | I/O/CPU | 5–25 | Low-Medium | P0 |
| Multipart buffering optimization / streaming strategy | RAM/I/O | 10–25 | High | P1 |
| Collection write-path contention tuning | CPU | 3–10 | Medium | P2 |

## 7) Final Score

- **Overall performance score: 5.3 / 10** (security-effective, but cost-intensive without strict tuning).
- **Use when:** deep rule-level inspection quality and policy flexibility are required, and tuning/benchmarking capacity exists.
- **Avoid or strongly gate when:** ultra-low-latency budgets (<2–5 ms), very high concurrency with large payloads, or limited observability/tuning maturity.
