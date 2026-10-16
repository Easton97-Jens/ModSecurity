# OWASP ModSecurity (libmodsecurity v3) – Tiefgehende technische Performance-Evaluierung (Deutsch)

> Scope: Engine-Verhalten in libmodsecurity v3 mit quantifizierten Schätzungen für CPU, RAM, I/O und Skalierung in typischen Enterprise-CRS-Deployments.

## 0) Executive Summary

1. **Das primäre Kostenzentrum ist die Regel-Ausführungsfächerung**: die effektiven CPU-Kosten skalieren näherungsweise mit `R × V × T × O` (Regeln × Zielwerte × Transformationen × Operatorkosten), wobei regex-lastige Operatoren in CRS-ähnlichen Setups dominieren.
2. **Der größte Bottleneck ist die Regex-Auswertung (`@rx`) in transformationslastigen Pipelines**, insbesondere bei hoher Payload-Entropie und stark überlappenden Regeln.
3. **Das größte systemische Risiko ist ein Tail-Latency-Kollaps bei hoher Parallelität**, wenn Regex-Last, Body-Parsing und synchrones Audit-Logging gleichzeitig auftreten.
4. **Der beste Quick Win ist die Reduktion von Regex-Aufrufen** (Prefilter/Literal-Gating + engerer Target-Scope), mit typischerweise **15–35% CPU-Reduktion** und **10–25% Verbesserung bei p95-Latenz** in regelintensiven Profilen.
5. **Operativ ist der Logging-Modus ein Hebel erster Ordnung**: der Wechsel von vollständigem synchronem Audit-Logging auf selektive/asynchrone Modi kann p99-Aufblähungen um **5–20%** reduzieren.

## 1) CPU Performance Analysis

### 1.1 Komplexität und Hotspots
- Dominanter Ausführungspfad: Transaktions-Phasenloop -> Regelbewertung pro Regel -> Variablen-Extraktion pro Ziel -> Transformationskette -> Operatorausführung.
- Näherungsmodell für CPU pro Request:
  - `C_total ≈ C_parse + Σ_r Σ_v (Σ_t C_trans + C_op) + C_logging + C_sync`
  - Kompakt: `C_total ≈ C_parse + R·V·(T·c_trans + c_op) + C_logging + C_sync`.
- Hotspots (absteigend):
  1. Ausführung von `@rx` inkl. Capture-Handling.
  2. Transformationspipeline (insbesondere mit Multi-Match-Semantik).
  3. Variablen-Fanout und wiederholte Evaluation über große Collections.
  4. Multipart-/zustandsbehaftetes Body-Parsing bei großen Requests.

### 1.2 Geschätzte CPU-Verteilung (regelintensives CRS-Profil)
- Regex-Engine: **45–70%** (Worst Case), **20–35%** (Best Case mit Tuning).
- Transformationen: **15–30%**.
- Parsing (URI/Header/Body): **10–25%**.
- Logging-Serialisierung im Hot Path: **5–15%**.
- Locking/Sync-Overhead: **3–10%** (höher bei write-lastiger Collection-Nutzung).

### 1.3 Verhalten von PCRE/JIT/Backtracking
- Bei stabilen Patterns und JIT-freundlichem Traffic: für die meisten Matches/Misses näherungsweise lineares Laufzeitverhalten.
- Bei adversarialen Inputs / pathologischer Überlappung: Backtracking dominiert und kann Tail-Latenz stark erhöhen, bis Match-Limits greifen.
- Best-Case-Regex-Beitrag: **~0.5–2.0 µs pro Aufruf** (einfache verankerte Muster, warmer Instruction Cache).
- Worst-Case-Regex-Beitrag: **10–1000+ µs pro Aufruf** bis zum limitbedingten Abbruch in pathologischen Fällen.

## 2) RAM / Memory Analysis

### 2.1 Speicherprofil pro Request
- Durchschnittsrequest (kleiner Body, moderate Header): **~80–300 KB/Request** zusätzlicher Working Set.
- Regelintensives Profil mit Body-Inspection: **~300 KB–1.5 MB/Request**.
- Große Multipart-Uploads (Buffering + Metadaten + Tempfile-Verwaltung): **~1.5–8 MB/Request** transiente Peaks.

### 2.2 Haupttreiber für Speicherverbrauch
- String-Duplizierung beim Aufbau vollständiger Requests und bei Body-Extraktion.
- Temporäre Objekte: Variablen-Vektoren, Capture-Container, Kopien transformierter Werte.
- Wiederholte Allokationen in per-Regel/per-Variable-Evaluationsschleifen.
- Multipart-Parser-Buffer und Metadatenstrukturen für temporäre Dateien.

### 2.3 Peak vs. Average und Skalierung
- Der Durchschnittsspeicher skaliert näherungsweise mit der aktiven Variablenkardinalität und der Body-Größe.
- Peak-Speicher skaliert überproportional, wenn großes Body-Buffering, hohe Parallelität und ausführliches Logging überlappen.
- Bei Parallelität `N` gilt praktisch:
  - `Mem_total ≈ Mem_base + N × Mem_req_avg + Mem_fragmentation`
  - Fragmentierung/Allocator-Druck kann bei Burst-Lasten **10–30%** Overhead erzeugen.

## 3) I/O Performance Analysis

### 3.1 Audit-Logging und Disk-I/O
- Vollständiges Audit-Logging kann bei blockiertem/hochdetailliertem Traffic zur dominanten I/O-Senke werden.
- Geschätzte Audit-Nutzlast pro Request:
  - Minimalmodus: **0.5–3 KB**
  - Vollmodus mit Body/Headern: **5–100+ KB** (payloadabhängig)
- Bei hoher RPS können synchrone Writes Queueing erzeugen und p99-Latenz erhöhen.

### 3.2 Tempfile- und Upload-Pfad-I/O
- Multipart mit Dateiinspektion erzeugt Disk-Druck über den Tempfile-Lebenszyklus.
- Bei dauerhaftem Upload-Traffic können IOPS und fsync-Verhalten limitieren, bevor CPU saturiert.

### 3.3 Netzwerk-Overhead (Reverse-Proxy-Deployments)
- Reverse-Proxy-Topologien fügen zusätzlichen Hop und zusätzliche Buffering-Domänen hinzu.
- Typischer zusätzlicher Netzwerk-/Service-Overhead: **+0.2–2.0 ms** Median, **+1–10 ms** im Tail, abhängig von Topologie und TLS-Platzierung.

### 3.4 Einfluss von synchronem vs asynchronem Logging
- Synchrones Logging: stärkere Konsistenz, höhere Latenzkopplung.
- Asynchrones Logging: bessere Entkopplung der Tail-Latenz, mögliches Verlustfenster bei Crash-Szenarien.
- Erwarteter Effekt von async + selektiven Parts: **Throughput +5–20%**, **p99-Latenz -5–25%**.

## 4) System Interaction (CPU + RAM + I/O)

- Bottlenecks verstärken sich multiplikativ statt additiv:
  - Höhere Regex-Zeiten erhöhen die Request-Residency -> größerer gleichzeitiger Speicher-Footprint -> höherer GC/Allocator-Druck -> längere Queues bei synchronem Logging.
- Realverhalten unter Burst-Last:
  - Phase-2-Body-Parsing und regex-lastige Phasen erhöhen CPU-Stall-Zeiten.
  - Gleichzeitiges Logging/Datei-I/O erzeugt Scheduler- und Storage-Contention.
- Effekt hoher Parallelität:
  - Throughput skaliert bei niedriger Parallelität nahezu linear und knickt dann am Sättigungspunkt von Regex + I/O ab.
  - Typischer Knee-Bereich in regelintensiven Profilen: **16–64 Worker/Threads**, umgebungsabhängig.

## 5) Performance Heatmap

| Kategorie | Score (0-10) | Impact | Bottleneck Severity |
|---|---:|---:|---:|
| CPU | 4.5 | Sehr hoch | Kritisch |
| RAM | 6.0 | Mittel-Hoch | Major |
| I/O | 5.5 | Hoch bei Audit-/Upload-lastigen Profilen | Major |

### Interpretation
- **CPU ist in den meisten CRS-nahen Deployments die limitierende Dimension** durch Regex-/Transformationsverstärkung.
- **RAM ist bei Durchschnittstraffic beherrschbar**, zeigt jedoch steile transiente Peaks bei Überlappung von großen Payloads und hoher Parallelität.
- **I/O ist szenariosensitiv**: bei minimalem Logging meist unkritisch, bei vollem Audit-Logging und Upload-Inspektion jedoch häufig Bottleneck erster Ordnung.

## 6) Optimization Impact Table

| Optimierung | Bereich | Impact (%) | Aufwand | Priorität |
|---|---|---:|---|---|
| Regex-Prefilter (Literal/ACMP vor `@rx`) | CPU | 15–35 | Mittel | P0 |
| Target-Scope-Reduktion (`V` minimieren) | CPU/RAM | 10–30 | Niedrig-Mittel | P0 |
| Transformationspipeline reduzieren/fusen | CPU | 8–20 | Mittel-Hoch | P1 |
| Lazy Full-Request-Materialisierung | RAM/CPU | 5–15 | Niedrig | P1 |
| Selektives + asynchrones Audit-Logging | I/O/CPU | 5–25 | Niedrig-Mittel | P0 |
| Multipart-Buffering-Optimierung / Streaming-Strategie | RAM/I/O | 10–25 | Hoch | P1 |
| Collection-Write-Path-Contention-Tuning | CPU | 3–10 | Mittel | P2 |

## 7) Final Score

- **Gesamt-Performance-Score: 5.3 / 10** (sicherheitswirksam, aber ohne konsequentes Tuning kostenintensiv).
- **Einsetzen, wenn:** tiefe, regelbasierte Inspektionsqualität und flexible Policy-Steuerung benötigt werden und Tuning/Benchmarking-Kapazität vorhanden ist.
- **Nicht einsetzen bzw. stark begrenzen, wenn:** ultra-niedrige Latenzbudgets (<2–5 ms), sehr hohe Parallelität mit großen Payloads oder geringe Observability-/Tuning-Reife vorliegen.
