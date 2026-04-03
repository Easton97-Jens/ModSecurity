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

## Messstrategie

### 1) Experiment-Design (reproduzierbar)

**Messmatrix (mindestens):**
- Regelset: Minimal / CRS PL1 / CRS PL2+
- Payload: 1 KB / 16 KB / 256 KB / 2 MB
- Workload-Mix: static GET, JSON API, multipart upload
- Concurrency: 1, 8, 32, 128
- Logging: aus / minimal / full audit

**A/B-Varianten:**
- Baseline ohne WAF vs mit WAF
- WAF mit/ohne bestimmte Rule-Klassen (z. B. regex-lastige Gruppen)

### 2) Tooling

- **CPU Hotspots:** `perf record` + `perf report`, anschließend Flamegraph
- **Memory/Allokationen:** `valgrind --tool=massif`, optional `heaptrack`
- **Syscall/Locking:** `perf lock`, `strace -c`
- **Optional eBPF:** uprobes auf `RulesSet::evaluate`, `RuleWithOperator::evaluate`, `Regex::searchOneMatch`, `executeTransformations`

### 3) Instrumentierung und Metriken

Für jede Request-Phase (1–5 + logging) erfassen:

\[
T_{phase,i} = t_{end,i} - t_{start,i}
\]

Zusätzlich pro Request:
- `T_regex_total`, `N_regex_calls`, `T_regex_avg`
- `T_trans_total`, `N_transforms`
- `alloc_bytes`, `alloc_count`
- `audit_bytes_written`

### 4) KPIs (SLO-fähig)

- **Latency:** p50 / p95 / p99 (end-to-end und pro Phase)
- **CPU:** cycles/request, instructions/request, IPC
- **Memory:** bytes/request, peak RSS, allocations/request
- **Skalierung:** throughput (RPS) vs concurrency, Saturation-Punkt

### 5) Akzeptanzkriterien für Optimierungen

Eine Optimierung gilt als erfolgreich, wenn über 3 Läufe (95%-Konfidenz):
- p95-Latenz mindestens 10% verbessert **oder**
- cycles/request mindestens 15% sinken
- ohne Regression der Block-/Detection-Rate (Sicherheits-Fidelity unverändert)

---

## Priorisierung

### 1) Quantifizierte Impact-Aufteilung (CPU-Anteil)

**Best-Case (kleines Regelset, wenig Regex):**
- Regex Engine: **20–35%**
- Transformation Pipeline: **20–30%**
- Parsing: **15–25%**
- Logging: **5–15%**
- Locking/Synchronisation: **<5%**

**Worst-Case (CRS-heavy, hohe Paranoia, große Bodies):**
- Regex Engine: **45–70%**
- Transformation Pipeline: **15–30%**
- Parsing (inkl. multipart): **10–20%**
- Logging: **5–15%**
- Locking/Synchronisation: **3–10%**

### 2) Top-5 Bottlenecks mit Gewichtung

Gewichtung = erwarteter Gesamt-CPU-Impact in produktionsnahen CRS-Setups.

1. **Regex-Matching (`@rx`)** — **40%**
2. **Transformationen + multiMatch-Multiplikation** — **22%**
3. **Variablenexpansion / Zielwert-Fanout (V)** — **14%**
4. **Body-Parsing (v. a. multipart, große Payloads)** — **13%**
5. **Logging/Serialisierung + I/O** — **11%**

Summe: 100%.

### 3) Entscheidungsregel für Fokus

Wenn \(p_{rx} > 0.35\) oder `T_regex_total / C_req > 0.4`, zuerst Regex-Optimierung.

Wenn `N_transforms * V` sehr hoch ist, zuerst Transformations-/Target-Reduktion.

Wenn `audit_bytes_written` hoch und Latenz tail-lastig ist, Logging zuerst reduzieren.

---

## Optimierungen mit Impact

| Maßnahme | Erwarteter Impact | Geschätzte Verbesserung | Implementierungs-Komplexität | Hinweise |
|---|---:|---:|---|---|
| Regex-Prefilter (Literal/ACMP vor `@rx`) | Hoch | **15–35%** CPU | Mittel | Reduziert teure Regex-Aufrufe bei sicheren Non-Matches |
| Regelgruppierung + frühe Exits pro Phase | Hoch | **10–25%** Latenz/CPU | Mittel | Besonders wirksam bei großen Regelmengen |
| Target-Scope-Härtung (`V` senken, keine unnötigen Collections) | Hoch | **10–30%** CPU | Niedrig–Mittel | Direkte Multiplikatorreduktion in \(R\cdot V\cdot T\) |
| Transformation-Fusion/Caching häufiger Pipelines | Mittel–Hoch | **8–20%** CPU | Mittel–Hoch | Muss semantisch äquivalent bleiben |
| `FULL_REQUEST` lazy/materialize-on-demand | Mittel | **5–15%** CPU + RAM | Niedrig | Vermeidet große String-Kopien |
| Streaming-/Chunk-Body-Repräsentation statt `stringstream::str()` | Mittel–Hoch | **10–25%** RAM/CPU | Hoch | Größerer Refactor, aber hoher langfristiger Gewinn |
| Audit-Logging selektiv/asynchron (Parts minimal) | Mittel | **5–20%** Tail-Latency | Niedrig–Mittel | Schnell umsetzbar, operativ gut steuerbar |
| Collection-Locking optimieren (write batching, sharding) | Niedrig–Mittel | **3–10%** bei hoher Concurrency | Mittel | Nur relevant bei write-heavy Regeln |

### Umsetzungsreihenfolge (Roadmap)

1. **Quick Wins (1–2 Sprints):** Logging-Reduktion, Target-Scope-Härtung, `FULL_REQUEST` lazy.
2. **Mid-Term (2–4 Sprints):** Regex-Prefilter, Rule-Phasen-Gating, Transformationsoptimierung.
3. **Langfristig:** Streaming-Body-Refactor + tieferes Locking-Redesign.
