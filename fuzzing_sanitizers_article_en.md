# ModSecurity v3 (libmodsecurity): Hardening Test & Security Strategy with Fuzzing and Sanitizers

## Introduction: Why This Matters for a WAF

A Web Application Firewall continuously ingests hostile, high-entropy input: request lines, headers, bodies, multipart streams, JSON/XML payloads, and rule definitions that trigger complex transformation chains. In a native C/C++ engine, that creates a dual risk profile:

1. **Memory/runtime risk** (memory corruption, UB, races)
2. **Detection integrity risk** (parser differentials and normalization gaps causing bypass)

For ModSecurity v3 (libmodsecurity), traditional unit tests are necessary but insufficient. A resilient strategy combines:

- deterministic unit/integration tests,
- coverage-guided fuzzing (libFuzzer),
- sanitizer-instrumented builds (ASan/UBSan/TSan),
- CI regression using crash corpora.

---

## 1) ModSecurity v3 Architecture Through a Security-Testing Lens

### 1.1 Core objects and processing phases

The public API exposes a transaction-oriented pipeline:

- `ModSecurity` core context,
- `RulesSet` for loading/evaluating rules,
- `Transaction` as per-request/per-response state container.

Canonical phase flow:

1. `processConnection`
2. `processURI`
3. `processRequestHeaders`
4. `appendRequestBody` + `processRequestBody`
5. `processResponseHeaders`
6. `appendResponseBody` + `processResponseBody`
7. `processLogging`

This phase model is ideal for targeted fuzz harnesses (header-only, body-only, rule-only, end-to-end).

### 1.2 Request parsing and input channels

`Transaction` exposes multiple ingestion paths:

- Header paths (`addRequestHeader`, `addResponseHeader`)
- URI/method/version (`processURI`)
- Body streaming (`appendRequestBody`, `requestBodyFromFile`, `appendResponseBody`)

The variable surface indicates broad parser/state handling for:

- multipart anomaly tracking (`MULTIPART_*`)
- URL-encoded error paths
- JSON/XML related flows

### 1.3 Rule engine, operators, transformations

Rule execution is phase-driven via `RulesSet::evaluate()`. The pipeline combines:

- operators,
- transformations (`urlDecode`, `htmlEntityDecode`, `jsDecode`, `cssDecode`, `normalisePath`, ...),
- disruptive actions (`deny`, `drop`, `redirect`, ...).

**Security implication:** many real bypasses are emergent behavior from decoding + transformation + operator composition, not a single isolated parser bug.

---

## 2) Attack Surface and Failure Modes in ModSecurity v3

### 2.1 High-risk components

1. **Header/URI parsing paths**
   - control chars, duplicate headers, oversized values, separator ambiguity
2. **Request-body parsers**
   - multipart boundary ambiguity
   - chunked encoding edge behavior
   - TE/Content-Length inconsistencies
3. **Transformation chains**
   - multiple decoding passes, mixed encodings, canonicalization mismatch
4. **Rule parsing/loading**
   - complex chaining, escaping corner cases, collection semantics

### 2.2 Priority bug classes

- **Memory safety:** OOB read/write, use-after-free, double-free
- **Undefined behavior:** arithmetic/shift/conversion hazards
- **Parser defects:** invalid state transitions, inconsistent error propagation
- **Logic defects:** false negatives (bypass), false positives, wrong phase semantics

### 2.3 WAF-specific impact

In a WAF, parser/normalization defects can both:

- crash the process (availability/DoS risk), and
- silently miss malicious requests (protection failure).

---

## 3) Current Testing Posture (Conceptual Assessment)

### Typical strengths

- Unit coverage for utility and transformation functions
- Integration tests against representative rule sets
- Regression coverage for known defects

### Typical gaps

1. **Insufficient combinatorial input coverage**
   - manual tests rarely cover decoder/parser cross-products
2. **Lack of differential/state testing**
   - semantically identical payloads across encodings are not compared systematically
3. **Limited sanitizer execution in CI**
   - latent memory/UB bugs remain invisible without instrumentation
4. **Weak concurrency stress coverage**
   - TSan runs are often omitted due to cost

---

## 4) Implementing Fuzz Testing for ModSecurity v3

### 4.1 Recommended strategy

Use a **harness portfolio**, not one monolithic fuzzer:

- Harness A: URI + request headers
- Harness B: request body processors (multipart/urlencoded/json/xml)
- Harness C: rule parsing/loading
- Harness D: compact end-to-end transaction flow

Each harness should be coverage-guided and seeded with realistic HTTP samples.

### 4.2 Example libFuzzer harness using libmodsecurity API (C++)

```cpp
#include <cstdint>
#include <cstddef>
#include <string>
#include <memory>

#include "modsecurity/modsecurity.h"
#include "modsecurity/rules_set.h"
#include "modsecurity/transaction.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < 8) return 0;

    std::string input(reinterpret_cast<const char*>(data), size);

    size_t p = input.find("\r\n\r\n");
    std::string head = (p == std::string::npos) ? input : input.substr(0, p);
    std::string body = (p == std::string::npos) ? "" : input.substr(p + 4);

    modsecurity::ModSecurity ms;
    modsecurity::RulesSet rules;

    const char *rule =
      "SecRuleEngine On\n"
      "SecRule REQUEST_URI \"@contains ../\" \"id:1001,phase:2,deny,status:403\"\n";
    rules.load(rule);

    std::unique_ptr<modsecurity::Transaction> tx(
        new modsecurity::Transaction(&ms, &rules, nullptr));

    tx->processConnection("127.0.0.1", 12345, "127.0.0.1", 80);
    tx->processURI("/fuzz", "GET", "1.1");
    tx->addRequestHeader("Host", "example.test");
    tx->addRequestHeader("Content-Type", "application/octet-stream");
    tx->processRequestHeaders();

    tx->appendRequestBody(reinterpret_cast<const unsigned char*>(body.data()), body.size());
    tx->processRequestBody();

    tx->processResponseHeaders(200, "HTTP/1.1");
    tx->appendResponseBody(reinterpret_cast<const unsigned char*>(head.data()), head.size());
    tx->processResponseBody();
    tx->processLogging();

    return 0;
}
```

Build command (Clang + libFuzzer + sanitizers):

```bash
clang++ -std=c++17 -g -O1 -fno-omit-frame-pointer \
  -fsanitize=fuzzer,address,undefined \
  fuzz_modsec_tx.cc -o fuzz_modsec_tx \
  -I./headers -L./src/.libs -lmodsecurity
```

(Exact link flags depend on local build layout.)

### 4.3 Coverage-guided operational setup

- seed with valid + malformed HTTP requests
- provide HTTP dictionaries (`boundary=`, `chunked`, `Content-Type`, etc.)
- periodically minimize/deduplicate corpora
- keep crash artifacts as long-lived regression assets

---

## 5) Sanitizer Strategy

### 5.1 AddressSanitizer (ASan)

Best default for fuzz pipelines:

- catches OOB/UAF/double-free classes,
- high signal quality with actionable stack traces.

### 5.2 UndefinedBehaviorSanitizer (UBSan)

Essential for parser correctness hardening:

- catches arithmetic and conversion UB that often escapes functional testing.

### 5.3 ThreadSanitizer (TSan)

Run in dedicated jobs for concurrent/shared-state scenarios:

- identifies race conditions and sync hazards,
- higher overhead, so typically nightly rather than per-PR.

### 5.4 Recommended combinations

Primary fuzz profile:

```bash
-fsanitize=fuzzer,address,undefined
```

Dedicated race profile:

```bash
-fsanitize=thread
```

(usually not combined with ASan in the same binary).

---

## 6) Practical Fuzz Inputs and Bug Scenarios

### 6.1 Useful seed examples

1. **Malformed header folding**

```http
GET / HTTP/1.1
Host: example.test
X-Test: value
	continued-with-tab

```

2. **Chunked encoding edge case**

```http
POST /upload HTTP/1.1
Host: example.test
Transfer-Encoding: chunked

A
1234567890
0

GARBAGE
```

3. **Multipart boundary mismatch**

```http
POST /form HTTP/1.1
Host: example.test
Content-Type: multipart/form-data; boundary=abc

--abx
Content-Disposition: form-data; name="x"

test
--abc--
```

4. **Encoding ambiguity**

- double URL-encoding of traversal payloads,
- mixed UTF-8, `%uXXXX`, HTML entities.

### 6.2 Expected findings

- parser state desynchronization,
- incorrect error-flag propagation (`REQBODY_ERROR*`, `MULTIPART_*`),
- transformation inconsistencies,
- sanitizer-detected crashes.

---

## 7) CI/CD Integration (GitHub Actions / GitLab CI)

### 7.1 Suggested stages

1. **PR gate (2–5 min)**
   - build critical harnesses
   - run each with `-max_total_time=60..180`
2. **Nightly fuzzing**
   - multi-hour runs, parallel jobs, corpus sync
3. **Crash reproduction job**
   - deterministic replay of new artifacts
4. **Regression job**
   - all fixed crash seeds must pass

### 7.2 Example GitHub Actions job

```yaml
name: fuzz-modsecurity
on:
  pull_request:
  schedule:
    - cron: "0 2 * * *"

jobs:
  fuzz-asan-ubsan:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - run: ./build.sh clang
      - run: clang++ -std=c++17 -g -O1 -fno-omit-frame-pointer \
              -fsanitize=fuzzer,address,undefined fuzz_modsec_tx.cc \
              -I./headers -L./src/.libs -lmodsecurity -o fuzz_modsec_tx
      - run: ./fuzz_modsec_tx -max_total_time=120 -artifact_prefix=./artifacts/
      - uses: actions/upload-artifact@v4
        if: always()
        with:
          name: fuzz-artifacts
          path: artifacts/
```

### 7.3 Crash handling and reporting

- normalized crash signatures (sanitizer type + stack + PC),
- auto-issue with exact repro command and artifact URL,
- triage by reachability, exploitability, and traffic likelihood.

---

## 8) Best Practices and Long-Term Recommendations

1. **Introduce parser/rule differential tests**
   - semantically identical payloads across encodings should produce consistent outcomes
2. **Keep harnesses small and deterministic**
   - improves reproducibility and triage speed
3. **Seed from anonymized production traffic**
   - better relevance than purely synthetic data
4. **Make sanitizer builds mandatory in non-release CI**
5. **Enforce fix-to-regression policy**
   - every fixed crash becomes a permanent test case
6. **Track metrics continuously**
   - coverage growth, unique crashes/week, MTTR for security defects

---

## Conclusion

For ModSecurity v3, the most effective hardening path is a combined model: **unit tests + coverage-guided fuzzing + sanitizer instrumentation + CI regression discipline**. In WAF systems, parsing and normalization quality directly determine security outcomes. A structured fuzzing program reduces both crash exposure and bypass exposure in a measurable way.
