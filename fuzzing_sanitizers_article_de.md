# ModSecurity v3 (libmodsecurity): Test- und Sicherheitsstrategie mit Fuzzing & Sanitizern

## Einleitung: Warum das für eine WAF besonders wichtig ist

Eine Web Application Firewall verarbeitet ungefilterte, potenziell bösartige Eingaben in hoher Vielfalt: Header, URI, Query-Strings, Form-Body, Multipart, JSON/XML, Response-Metadaten und Rule-Sets mit komplexen Transformationen. Genau hier treffen zwei Risiken aufeinander:

1. **Speicher- und Parser-Risiken** in C/C++ (Memory Corruption, UB, Race Conditions)
2. **Semantische Risiken** (Bypass durch Parser-Differenzen, fehlerhafte Rule-Evaluation)

Für ModSecurity v3 (libmodsecurity) reicht klassisches Unit-Testing allein nicht aus. Eine robuste Strategie kombiniert:

- deterministische Unit-/Integrationstests,
- coverage-guided Fuzzing (libFuzzer),
- Sanitizer-Instrumentierung (ASan/UBSan/TSan),
- und CI/CD-Regression mit Crash-Corpora.

---

## 1) Architekturüberblick ModSecurity v3 (relevant für Security Testing)

### 1.1 Core-Objekte und Processing-Phasen

Die öffentliche API zeigt einen klaren Transaktionsfluss:

- `ModSecurity` als Core-Kontext,
- `RulesSet` zum Laden und Evaluieren von Rules,
- `Transaction` als zustandsbehaftete Request/Response-Instanz.

Typische Verarbeitungsphasen:

1. `processConnection`
2. `processURI`
3. `processRequestHeaders`
4. `appendRequestBody` + `processRequestBody`
5. `processResponseHeaders`
6. `appendResponseBody` + `processResponseBody`
7. `processLogging`

Diese Phase-Struktur ist aus Security-Sicht ideal für komponentenspezifische Fuzz-Harnesses (z. B. Header-only, Body-only, Rule-only).

### 1.2 Request Parsing und Input-Kanäle

Über `Transaction` werden unterschiedliche Input-Pfade abgedeckt:

- Header-Ingestion (`addRequestHeader`, `addResponseHeader`)
- URI/Method/HTTP-Version (`processURI`)
- Body-Streams (`appendRequestBody`, `requestBodyFromFile`, `appendResponseBody`)

Die vorhandenen Variablenklassen deuten auf breite Parser-/Normalisierungslogik hin, u. a. für:

- Multipart-Anomalien (`MULTIPART_*` Variablen)
- URL-Encoded Fehlerfälle
- JSON/XML-bezogene Pfade

### 1.3 Rule Engine, Operatoren, Transformationen

Die Rule-Auswertung ist über `RulesSet::evaluate()` phasenbasiert organisiert. In der Engine wirken:

- Operatoren (Pattern-/Semantik-Prüfungen)
- Transformationen (`urlDecode`, `htmlEntityDecode`, `jsDecode`, `cssDecode`, `normalisePath`, …)
- disruptive Actions (`deny`, `drop`, `redirect`, …)

**Security-relevant:** Viele Bypass-Klassen entstehen nicht durch einen einzelnen Parserfehler, sondern durch inkonsistente Ketten aus Decoding + Transformation + Operator-Evaluation.

---

## 2) Schwachstellenanalyse: Kritische Attack Surface in ModSecurity v3

### 2.1 Hochrisiko-Bereiche

1. **Header-/URI-Parserpfade**
   - ungewöhnliche Separatoren, Steuerzeichen, überlange Werte, Duplicate Headers
2. **Request Body Parser**
   - Multipart Boundary Edge Cases
   - Chunked-Transfer-Sonderfälle
   - inkonsistente Content-Length/Transfer-Encoding Kombinationen
3. **Transformation-Pipelines**
   - Mehrfach-Dekodierung, mixed encodings, normalisierte vs. rohe Darstellung
4. **Rule DSL / Rules Parsing**
   - komplexe Rule-Chains, Grenzfälle bei Escape-Sequenzen und Collections

### 2.2 Fehlerklassen mit hoher Priorität

- **Memory Safety:** OOB read/write, UAF, Double-Free
- **UB:** integer overflows, invalid shifts/conversions, signedness issues
- **Parser Bugs:** inkorrekte State-Machine-Transitions, inkonsistentes Error-Handling
- **Logikfehler:** false negatives (Bypass), false positives, falsche Phase-Zuordnung

### 2.3 WAF-spezifische Sicherheitswirkung

In einer WAF sind Parser-/Normalisierungsfehler doppelt kritisch:

- Sie können Crashes auslösen (Verfügbarkeitsrisiko / DoS), **und/oder**
- gefährliche Requests an der Detection vorbei schleusen (Integritätsrisiko).

---

## 3) Bewertung der aktuellen Teststrategie (konzeptionell)

### Typische Stärken vorhandener Strategien

- Unit Tests für Utility-/Transformation-Funktionen
- Integrationstests mit bekannten Rule-Sets (z. B. CRS-ähnliche Szenarien)
- Regressionsfälle für bekannte CVEs/Bugs

### Typische Lücken

1. **Unzureichende Input-Raum-Abdeckung**
   - manuell geschriebene Tests decken selten Decoder-/Parser-Kombinatorik ab
2. **Fehlende Differential-/State-Tests**
   - gleiche Payload in Varianten (raw/encoded/multipart/chunked) wird nicht systematisch verglichen
3. **Zu wenig Sanitizer-Läufe in CI**
   - Memory/UB-Bugs bleiben ohne Instrumentierung oft lange latent
4. **Nebenläufigkeit kaum belastet**
   - TSan-Läufe fehlen häufig wegen Laufzeitkosten

---

## 4) Fuzz-Strategie für ModSecurity v3

## 4.1 Zielbild

Empfohlen ist ein **Harness-Portfolio** statt eines monolithischen Fuzzers:

- Harness A: Header + URI Parsing
- Harness B: Request Body (multipart/urlencoded/json/xml)
- Harness C: Rule Parsing/Loading
- Harness D: End-to-End Transaction (kleine, aber realistische Sequenz)

Jeder Harness nutzt coverage-guided Mutationen und erhält Seed-Corpora aus realistischen HTTP-Proben.

### 4.2 Beispiel: libFuzzer Harness gegen Transaction-API (C++)

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

    // 1) Splitte grob in request-line/header/body
    size_t p = input.find("\r\n\r\n");
    std::string head = (p == std::string::npos) ? input : input.substr(0, p);
    std::string body = (p == std::string::npos) ? "" : input.substr(p + 4);

    // 2) Minimales ModSecurity Setup
    modsecurity::ModSecurity ms;
    modsecurity::RulesSet rules;

    // Kleine Inline-Rule: nur als Beispiel
    const char *rule =
      "SecRuleEngine On\n"
      "SecRule REQUEST_URI \"@contains ../\" \"id:1001,phase:2,deny,status:403\"\n";
    rules.load(rule);

    std::unique_ptr<modsecurity::Transaction> tx(
        new modsecurity::Transaction(&ms, &rules, nullptr));

    // 3) Request simulieren
    tx->processConnection("127.0.0.1", 12345, "127.0.0.1", 80);
    tx->processURI("/fuzz", "GET", "1.1");
    tx->addRequestHeader("Host", "example.test");
    tx->addRequestHeader("Content-Type", "application/octet-stream");
    tx->processRequestHeaders();

    tx->appendRequestBody(reinterpret_cast<const unsigned char*>(body.data()), body.size());
    tx->processRequestBody();

    // 4) Response-Phase antriggern (optional für mehr Coverage)
    tx->processResponseHeaders(200, "HTTP/1.1");
    tx->appendResponseBody(reinterpret_cast<const unsigned char*>(head.data()), head.size());
    tx->processResponseBody();
    tx->processLogging();

    return 0;
}
```

Build mit Clang/libFuzzer/Sanitizern:

```bash
clang++ -std=c++17 -g -O1 -fno-omit-frame-pointer \
  -fsanitize=fuzzer,address,undefined \
  fuzz_modsec_tx.cc -o fuzz_modsec_tx \
  -I./headers -L./src/.libs -lmodsecurity
```

Hinweis: Linkerpfade hängen vom lokalen Build-System (autotools/cmake, static/shared) ab.

### 4.3 Coverage-guided Fuzzing Setup

- Seeds: valide + leicht defekte HTTP Requests
- Dictionaries: HTTP-Token (`Content-Type`, `Transfer-Encoding`, `boundary=`, `chunked`, `multipart/form-data`)
- Corpus-Minimierung regelmäßig (Dedup + minimize)
- Crash-Artefakte versionieren und als Regression weiterverwenden

---

## 5) Sanitizer-Einsatz für ModSecurity v3

### 5.1 AddressSanitizer (ASan)

Nutzen:

- OOB, UAF, Double-Free, Heap/Stack-Memory-Issues
- idealer Standard für tägliche Fuzz-Läufe

### 5.2 UndefinedBehaviorSanitizer (UBSan)

Nutzen:

- UB-Klassen sichtbar machen, die funktional „noch laufen“, aber unsicher sind
- insbesondere wertvoll in Parser-Arithmetik und Längenberechnungen

### 5.3 ThreadSanitizer (TSan)

Nutzen:

- Datenrennen in globalen/Shared-Komponenten erkennen
- eher in separaten Nightly-Jobs wegen Overhead

### 5.4 Kombinierte Konfiguration

Für Fuzzing im CI meist:

```bash
-fsanitize=fuzzer,address,undefined
```

Für Race-Analysen getrennt:

```bash
-fsanitize=thread
```

(typischerweise ohne gleichzeitiges ASan im selben Binary).

---

## 6) Praktische Fuzzing-Inputs und Bug-Szenarien

### 6.1 Beispiele für Seed-Inputs

1. **Malformed Header Folding**

```http
GET / HTTP/1.1
Host: example.test
X-Test: value
	continued-with-tab

```

2. **Chunked Edge Case**

```http
POST /upload HTTP/1.1
Host: example.test
Transfer-Encoding: chunked

A
1234567890
0

GARBAGE
```

3. **Multipart Boundary Mismatch**

```http
POST /form HTTP/1.1
Host: example.test
Content-Type: multipart/form-data; boundary=abc

--abx
Content-Disposition: form-data; name="x"

test
--abc--
```

4. **Encoding-Mehrdeutigkeit**

- doppelt URL-encodete Traversal-Sequenzen
- Mischung aus UTF-8, `%uXXXX`, HTML-Entities

### 6.2 Erwartete Findings

- Parser-State-Desync
- falsche Fehlerflag-Propagation (`REQBODY_ERROR*`, `MULTIPART_*`)
- inkonsistente Transformationsergebnisse
- Abstürze unter sanitizter Instrumentierung

---

## 7) CI/CD-Integration (GitHub Actions / GitLab CI)

### 7.1 Empfohlene Pipeline-Stufen

1. **PR-Gate (schnell, 2–5 min)**
   - Build von 1–3 kritischen Harnesses
   - `-max_total_time=60..180` je Harness
2. **Nightly Fuzz (langlaufend)**
   - mehrere Stunden, parallelisierte Jobs
   - Corpus-Sync, Crash-Dedup
3. **Crash-Repro Job**
   - reproduziert neue Crash-Artefakte deterministisch
4. **Regression Job**
   - alle gefixten Crash-Seeds als „must-pass“

### 7.2 Beispiel (GitHub Actions, verkürzt)

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

### 7.3 Crash Detection & Reporting

- eindeutige Crash-Signaturen (Stacktrace + PC + Sanitizer-Typ)
- Auto-Issue mit Repro-Kommando und Artifact-Link
- Priorisierung nach: Reachability, Exploitability, Traffic-Wahrscheinlichkeit

---

## 8) Best Practices & strategische Empfehlungen

1. **Rule-/Parser-Differentialtests etablieren**
   - gleiche semantische Payload in mehreren Encodings muss konsistent bewertet werden
2. **Harnesses klein und stabil halten**
   - bessere Reproduzierbarkeit, schnellere Iteration
3. **Seed-Corpus aus realem (anonymisiertem) Traffic**
   - höhere Relevanz als rein synthetische Inputs
4. **Sanitizer verpflichtend in Non-Release-Pipelines**
5. **Fix-to-Regression Policy**
   - jeder Crash-Fix wird permanenter Regressionstest
6. **Metriken tracken**
   - coverage growth, unique crashes/week, MTTR für Security-Bugs

---

## Fazit

Für ModSecurity v3 ist die Kombination aus **Unit Tests + libFuzzer + Sanitizern + CI-Regression** der praktikabelste Weg zu höherer Robustheit. Besonders in WAF-Kontexten entscheidet die Qualität von Parsing, Normalisierung und Rule-Evaluation direkt über Sicherheitswirkung. Eine systematische Fuzzing-Strategie reduziert sowohl Crash-Risiko als auch Bypass-Risiko signifikant.
