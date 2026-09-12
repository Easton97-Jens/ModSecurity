# OWASP ModSecurity v3 – Technischer Performance-Audit (Code-fokussiert)

Dieses Dokument fasst eine tiefgehende, codebasierte Performance-Analyse von libmodsecurity zusammen, mit Fokus auf Hot Paths, Regel-Engine, Speicherverhalten und Skalierung.

> English version: `doc/performance_audit_modsecurity_2026-04-03.en.md`

## Scope

Analysierte Kernpfade:
- `Transaction` Request/Response Lifecycle
- `RulesSet` + `RuleWithOperator` Evaluierung
- Request-Body-Prozessoren (URLENCODED/JSON/XML/MULTIPART)
- Regex- und Pattern-Matching Operatoren (`@rx`, `@pm`)
- Collection-Backends und Locking
- Audit-Logging und Serialisierung

## Wichtigste Erkenntnisse (Kurzfassung)

1. **Dominanter CPU-Pfad**: `RulesSet::evaluate()` → `RuleWithOperator::evaluate()` → Transformationen → Operator (`@rx`, `@pm`, …).
2. **Regex ist Hauptkostentreiber** bei CRS-lastigen Regelsets; Match-Limits sind vorhanden, aber nur Schadensbegrenzung.
3. **Signifikante String-/Copy-Kosten** in Request-Body- und Logging-Pfaden (`stringstream::str()`, Header-Konkatenation, JSON/Audit-Serialisierung).
4. **Multipart-Parsing** arbeitet byteweise mit hohem Branching- und State-Machine-Aufwand.
5. **Concurrency**: transaktionslokal weitgehend lock-frei; globale Collections nutzen `shared_mutex` und können bei write-lastigen Workloads kontendieren.

## Potenzielle Optimierungen (Top)

- Request-Body intern von `stringstream` auf chunk-/span-basierte Buffering-Strategie umstellen.
- `FULL_REQUEST` lazy oder feature-gated berechnen (TODO im Code vorhanden).
- Regex-basierte Regeln per Vorfilter (Literal prefilter, cheap guards) reduzieren.
- Transformation-Pipeline für häufige Kombinationen cachen/fusen.
- Audit-Logging asynchron und selektiv (Parts minimieren, JSON nur wenn nötig).

## Performance-Modell

### 1) Kostenmodell pro Request

Wir modellieren die gesamte CPU-Zeit pro Request als:

\[
C_{req} = C_{conn} + C_{parse} + C_{rules} + C_{log} + C_{sync}
\]

mit:

- \(C_{conn}\): Verbindungs-/Kontextkosten (klein, nahezu konstant)
- \(C_{parse}\): URI/Header/Body-Parsing
- \(C_{rules}\): Regelbewertung (dominant)
- \(C_{log}\): Audit-/Debug-Serialisierung
- \(C_{sync}\): Locking-/Contention-Kosten auf shared Collections

Für die Regel-Engine:

\[
C_{rules} = \sum_{r=1}^{R} \left( V_r \cdot \left(\sum_{t=1}^{T_r} C_{trans}(t)\right) + C_{op}(r) \right) + C_{actions}(r)
\]

Praktisch (aggregiert) gilt näherungsweise:

\[
C_{rules} \approx R \cdot V \cdot (T \cdot \bar c_{trans} + \bar c_{op}) + R \cdot \bar c_{actions}
\]

wobei:
- \(R\): Anzahl aktiver Regeln pro Phase
- \(V\): durchschnittliche Anzahl Zielwerte pro Regel (z. B. ARGS, Header, Cookies)
- \(T\): mittlere Anzahl Transformationen pro Regel
- \(\bar c_{op}\): mittlere Operatorkosten

Damit wird klar: **lineare Skalierung in \(R, V, T\)**, aber \(\bar c_{op}\) kann bei Regex nichtlinear eskalieren.

### 2) Regex-Spezialmodell

Für Regex-Regeln zerlegen wir \(\bar c_{op}\) in:

\[
\bar c_{op} = p_{rx}\cdot c_{rx} + (1-p_{rx})\cdot c_{other}
\]

mit \(p_{rx}\) als Anteil regex-basierter Regeln.

- **Best Case (JIT + frühes Match/Miss, wenig Backtracking):**
  \[
  c_{rx}^{best} = O(n)
  \]
- **Worst Case (katastrophales Backtracking):**
  \[
  c_{rx}^{worst} = O(e^n)
  \]
  in der Praxis durch Match-Limits gedeckelt, aber weiterhin extrem teuer bis zum Abbruch.

### 3) Big-O nach Subsystem

- **Rule Evaluation gesamt:**
  \[
  O\left(\sum_{r=1}^{R} V_r\cdot(T_r + O_r)\right)
  \]
  i. d. R. näherungsweise \(O(R\cdot V\cdot(T+O))\).
- **Parsing:**
  - URI/Header/Cookies: \(O(H + Q)\)
  - URL-Encoded Body: \(O(B)\)
  - Multipart: \(O(B\cdot\kappa)\), \(\kappa\) = Zustands-/Boundary-Prüfaufwand
- **Transformation Pipeline:**
  \[
  O(R\cdot V\cdot T\cdot L)
  \]
  mit \(L\) = durchschnittliche Stringlänge je Zielwert.

---

## Messgrenzen und Review-Kontext

Dieses Dokument ist ein **code-fokussierter qualitativer Performance-Audit**, kein abgeschlossener Benchmark-Report. Die früher in diesem Abschnitt enthaltenen numerischen CPU-Anteils-Schätzungen wurden nach Reviewer-Feedback in Messhypothesen umgewandelt, weil keine reproduzierbaren Traces beigefügt waren, die exakte Prozentwerte belegen.

Aktuelle Grenzen:
- Es sind keine `perf`-Traces, Flamegraphs oder Profiler-Artefakte an diesen Report angehängt.
- Es sind keine konkrete CRS-Version, kein Paranoia-Level und kein Payload-Korpus im Dokument fixiert.
- Exakte PCRE/PCRE2-Version, JIT-Status, Match-Limits, Parallelität und Logging-Modus sind hier nicht festgelegt.
- CPU-Anteile hängen stark von CRS-Version, Paranoia-Level, aktivierten Regeln, PCRE/PCRE2-JIT-Verhalten, Match-Limits, Payload-Form, Body-Größe, Logging-Konfiguration, Connector-Verhalten und Parallelität ab.

Regex-Regeln können in CRS-lastigen oder adversarialen Workloads teuer werden, aber dieser Report darf nicht als Behauptung eines konkreten Regex-CPU-Prozentsatzes gelesen werden. Jeder solche Prozentsatz muss durch reproduzierbare Benchmarks und Traces validiert werden. Reviewer-Reproduktion deutet außerdem darauf hin, dass PL=2-/Large-Match-Limit-Szenarien deutliche Gesamteinbußen zeigen können, während Regex-Symbole selbst weiterhin angemessen skalieren; das verantwortliche Subsystem bleibt daher eine offene Messfrage.

---

## Messstrategie

### 1) Reproduzierbarer Benchmark-Plan

Ein zukünftiger Benchmark zur Validierung von Regex-Overhead oder anderen CPU-Anteils-Hypothesen sollte folgende Eingaben fixieren:

- **ModSecurity Commit SHA:** exakter libmodsecurity-Commit unter Test.
- **Connector- und Server-Version:** z. B. nginx/OpenResty-Connector-Version bei End-to-End-Benchmarks.
- **CRS-Version und Paranoia-Level:** exakter CRS-Tag/Commit und PL1/PL2/PL3/PL4-Einstellung.
- **PCRE/PCRE2-Version und JIT-Status:** Library-Version, Build-Flags, JIT an/aus, Match-Limit- und Recursion/Depth-Limit-Einstellungen.
- **Exaktes Regelset:** vollständige Liste geladener Rule-Files plus lokale Exclusions, deaktivierte Regeln und `SecRuleRemove*`-Direktiven.
- **Payload-Korpus:** legitimer Traffic, Angriffstraffic, adversariale/pathologische Regex-Payloads, JSON, URL-Encoded Forms, Multipart-Uploads sowie produktionsnahe Header/Cookies.
  Das Tool `test/benchmark/benchmark` unterstützt `--request-file` und `--request-dir`, um diese rohen HTTP-Request-Nachrichten reproduzierbar abzuspielen.
- **Body-Größen:** mindestens 0 B, 1 KB, 16 KB, 256 KB, 2 MB; größere Uploads nur, wenn Deployment-Limits sie erlauben.
- **Parallelitätsstufen:** mindestens 1, 8, 32, 128 Worker/Clients oder deployment-spezifische Äquivalente.
- **Logging-Modus:** Audit-Logging aus, minimales relevant-only Logging und vollständiges Audit-Logging als getrennte Testarme.
- **Runtime-Umgebung:** CPU-Modell, Core-Anzahl, Kernel, Compiler-Flags, Allocator, Container/cgroup-Limits und CPU-Frequency-Governor.

### 2) Tooling

- **CPU-Hotspots:** `perf record` + `perf report`, anschließend Flamegraphs.
- **Memory/Allokationen:** `valgrind --tool=massif`, optional `heaptrack`.
- **Syscalls/Locking:** `perf lock`, `strace -c`.
- **Optional eBPF:** uprobes auf `RulesSet::evaluate`, `RuleWithOperator::evaluate`, `Regex::searchOneMatch` und `executeTransformations`.

Beispiel-Workflow:

```sh
perf record -F 999 -g -- ./test/benchmark/benchmark 100000
perf report --stdio
perf script > perf.script
# Flamegraph mit den üblichen FlameGraph stackcollapse/perl-Skripten erzeugen.
```

Für End-to-End-nginx/OpenResty-Tests sollte `perf record` gegen den Worker-Prozess oder gegen den vollständigen Benchmark-Befehl laufen, der Traffic erzeugt. Request-Generator, Konfiguration, Regelset und Payload-Korpus sollten versioniert werden.

### 3) Instrumentierung und Metriken

Für jede Request-Phase (1–5 + logging):

\[
T_{phase,i} = t_{end,i} - t_{start,i}
\]

Zusätzlich pro Request:
- `T_regex_total`, `N_regex_calls`, `T_regex_avg`
- `T_trans_total`, `N_transforms`
- `alloc_bytes`, `alloc_count`
- `audit_bytes_written`

Berechnung des Regex-Anteils:

\[
regex\_share = \frac{T_{regex\_total}}{T_{request\_total}}
\]

Dabei gilt:
- `T_regex_total` ist die akkumulierte inklusive oder exklusive Zeit in Regex-Ausführung; der Report muss klar angeben, welche Variante verwendet wird.
- `T_request_total` ist die gemessene End-to-End-Transaktionszeit oder die Summe der Phasen-Timer; der Report muss den Nenner klar definieren.
- Wenn `perf`-Samples statt Wall-Clock-Instrumentierung genutzt werden, sollte der Sample-Anteil aus klar benannten Regex-Symbolen berechnet und mit Folded Stacks/Flamegraphs belegt werden.

### 4) KPIs

- **Latency:** p50 / p95 / p99 (end-to-end und pro Phase).
- **CPU:** cycles/request, instructions/request, IPC und Sample-Anteil nach Symbolen.
- **Memory:** bytes/request, peak RSS, allocations/request.
- **Skalierung:** throughput (RPS) vs concurrency, Saturation-Punkt.

### 5) Akzeptanzkriterien für Optimierungen

Eine Optimierung sollte über wiederholte Läufe mit Konfidenzintervallen bewertet werden. Sinnvolle Gates:
- p95-Latenz verbessert sich im Ziel-Workload materiell, **oder**
- cycles/request sinken materiell, **und**
- Block-/Detection-Verhalten regressiert nicht.

Exakte Erfolgsschwellen sollten erst durch den Benchmark-Verantwortlichen festgelegt werden, nachdem die Baseline-Varianz gemessen wurde.

---

## Priorisierung

### 1) Erwartete Bottleneck-Bereiche zur Validierung

Die folgenden Punkte sind **Messhypothesen**, keine in diesem Dokument gemessenen CPU-Anteile:

- **Regex-Operatoren (`@rx`):** können in CRS-lastigen, hohen Paranoia-Leveln oder adversarialen Workloads ein Hotspot werden, aber Reviewer-Feedback legt nahe, dass Regex in manchen PL=2-/Large-Match-Limit-Tests angemessen skalieren kann; vor einer Einstufung als Primärursache über Regex-Symbol-Samples und Operator-Timer validieren.
- **Transformationspipeline:** kann dominieren, wenn viele Variablen wiederholt transformiert werden; zu validieren über Transformations-Timer und Allokationsprofile.
- **Variablenexpansion / Target-Fanout (`V`):** kann Regelkosten multiplizieren, wenn breite Collections wie ARGS, Header, Cookies und body-abgeleitete Variablen wiederholt evaluiert werden.
- **Body-Parsing:** URL-Encoded, JSON, XML und besonders Multipart sollten getrennt gemessen werden, weil Payload-Form die Kosten weg von Regex verschieben kann.
- **Logging und Serialisierung:** können p95/p99 beeinflussen, wenn Audit-Parts Bodies, Response-Daten oder synchrone Disk-Writes enthalten.
- **Collection-Locking/Synchronisation:** sollte unter hoher Parallelität und write-lastigen Regeln geprüft werden, aber nicht ohne Lock-Traces als dominant angenommen werden.

### 2) Qualitative Priorisierung

Validierungsreihenfolge für typische CRS-nahe Deployments:

1. **Regex-Matching (`@rx`)** — hoch priorisierte Hypothese, wenn regex-lastige Regelgruppen aktiv sind oder adversariale Inputs getestet werden.
2. **Transformationen + multiMatch-Verstärkung** — hoch priorisierte Hypothese, wenn Regelsets mehrere Transformationen auf breite Targets anwenden.
3. **Variablenexpansion / Target-Fanout** — hoch priorisierte Hypothese, wenn Regeln große Collections oder geparste Body-Argumente inspizieren.
4. **Body-Parsing** — workload-abhängig; besonders wichtig bei Multipart und großen Request-Bodies.
5. **Logging/Serialisierung + I/O** — workload-abhängig; besonders wichtig bei vollständigem Audit-Logging und Blocked-Request-Traces.

### 3) Entscheidungsregeln für zukünftige Messungen

- Wenn gemessenes `T_regex_total / T_request_total` im fixierten Workload hoch ist, Regex-Prefilter, Regex-Rule-Review und Target-Narrowing priorisieren.
- Wenn `N_transforms * V` hoch ist oder Transformationssymbole Flamegraphs dominieren, Transformationsreduktion/-Fusion und Target-Narrowing priorisieren.
- Wenn `audit_bytes_written` mit Tail-Latenz korreliert, selektives/asynchrones Logging und Audit-Part-Reduktion priorisieren.
- Wenn Parsing-Symbole dominieren, Body-Limits, Parser-Konfiguration und workload-spezifisches Request-Body-Handling optimieren, bevor Regex-Regeln geändert werden.

---

## Optimierungen mit erwartetem Impact

Die folgende Tabelle ist qualitativ, bis Benchmark-Daten angehängt sind.

| Maßnahme | Erwarteter Impact | Implementierungs-Komplexität | Validierungsmetrik |
|---|---:|---|---|
| Regex-Prefilter (Literal/ACMP vor `@rx`) | Potenziell hoch, wenn Regex-Anteil hoch gemessen wird | Mittel | Niedrigeres `T_regex_total / T_request_total`, weniger Regex-Aufrufe |
| Regelgruppierung + frühe Exits pro Phase | Potenziell hoch bei großen Regelmengen | Mittel | Niedrigere cycles/request und Phasenzeit |
| Target-Scope-Härtung (`V` senken) | Hoch, wenn breite Collections dominieren | Niedrig–Mittel | Weniger evaluierte Variablenwerte pro Regel |
| Transformation-Fusion/Caching häufiger Pipelines | Mittel bis hoch, wenn Transformationssymbole dominieren | Mittel–Hoch | Niedrigere Transformationszeit und Allokationen |
| `FULL_REQUEST` lazy/materialize-on-demand | Mittel, wenn Full-Request-Variablen selten genutzt werden | Niedrig | Weniger allocations/request und niedrigerer peak RSS |
| Streaming-/Chunk-Body-Repräsentation | Mittel bis hoch bei großen Bodies | Hoch | Niedrigerer peak RSS und weniger Body-Copy-Zeit |
| Audit-Logging selektiv/asynchron | Mittel bis hoch bei Full-Audit- oder Blocked-Request-Workloads | Niedrig–Mittel | Niedrigere p95/p99-Latenz und Audit-Write-Zeit |
| Collection-Locking optimieren (write batching, sharding) | Workload-abhängig | Mittel | Niedrigere Lock-Wait-Zeit unter Parallelität |

### Umsetzungsreihenfolge (Roadmap)

1. **Quick Wins:** Logging-Reduktion, Target-Scope-Härtung, `FULL_REQUEST` lazy.
2. **Mid-Term:** Regex-Prefilter, Rule-Phasen-Gating, Transformationsoptimierung.
3. **Langfristig:** Streaming-Body-Refactor + tieferes Locking-Redesign.
