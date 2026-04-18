# Benchmark Runner

## 🇩🇪 Deutsch

## 1. Überblick
Das Benchmark-System unter `test/benchmark/` dient dazu, die Verarbeitungskosten von ModSecurity reproduzierbar zu messen. Es kombiniert:
- einen allgemeinen Transaktions-Benchmark (`benchmark`),
- einen JSON-spezifischen Benchmark (`json_benchmark`),
- einen Orchestrator (`test.sh`) für vollständige Vergleichsläufe über mehrere Varianten.

Ziel ist ein nachvollziehbarer Vergleich zwischen Regelkonfigurationen (Baseline, CRS v3, CRS v4) mit getrennten Artefakten pro Variante.

## 2. Architektur
### Komponenten
| Komponente | Rolle |
|---|---|
| `benchmark` | Führt wiederholt HTTP-Transaktionen aus und liefert Laufzeit-Metriken (`elapsed_seconds`, `avg_transaction_ns`, `throughput_tx_per_sec`, `interventions`). |
| `json_benchmark` | Führt JSON-Request-Body-Benchmarks für Szenarien aus und gibt Metriken (u. a. `process_request_body_ns`, `total_transaction_ns`, `parse_*`, `ru_maxrss_kb`) aus. |
| `test.sh` | Führt einen Gesamtlauf über `baseline`, `crs_v3`, `crs_v4` aus, sammelt Rohdaten, parst Metriken und erzeugt Vergleichsreports. |
| `basic_rules.conf` | Regelbasis für `benchmark`. |
| `json_benchmark_rules.conf` | Regelbasis für `json_benchmark`. |
| CRS-Downloads (`download-owasp-v3-rules.sh`, `download-owasp-v4-rules.sh`) | Holen CRS-Repositories und ermöglichen Include-basierte Erweiterung der Rules. |

### Zusammenspiel
1. `test.sh` prüft Binaries und Rule-Dateien.
2. Für jede Variante werden Rule-Dateien aus Backups wiederhergestellt und optional CRS-Includes ergänzt.
3. `benchmark` und `json_benchmark` werden pro Variante ausgeführt.
4. Ergebnisse landen getrennt in variantenspezifischen Verzeichnissen, plus globalem Vergleichsreport.

## 3. Build-Anleitung
### Voraussetzungen (aus Build-/Skriptstruktur ableitbar)
- POSIX-Shell (`sh`/`bash`)
- Autotools-Toolchain (`autoreconf`-Schritt über `build.sh`)
- C/C++-Build-Umgebung und `make`
- Git (für Submodule)

Eine vollständige Liste aller Systempakete ist im vorhandenen Code nicht zentral dokumentiert. **This cannot be verified from the available code.**

### Empfohlene Befehlsfolge
```bash
git submodule update --init --recursive
./build.sh
./configure
make
make check
```

### Warum diese Schritte
- `git submodule update --init --recursive`: holt benötigte Submodule.
- `./build.sh`: erzeugt/aktualisiert Autotools-Generierungsdateien.
- `./configure`: erstellt plattformspezifische Build-Konfiguration.
- `make`: baut Bibliothek und Benchmark-Binaries.
- `make check`: führt die vorhandenen Checks aus.

### Fallback: Wenn Benchmark-Binaries nach `make` fehlen
So prüfst du, ob die Binaries gebaut wurden:
```bash
ls -l test/benchmark/benchmark test/benchmark/json_benchmark
```

Wenn eine Datei fehlt oder nicht ausführbar ist, kannst du gezielt nur die Benchmark-Binaries nachbauen:
```bash
make -C test/benchmark benchmark json_benchmark
```

Hinweis zur Reihenfolge:
- Wenn `./configure` oder der Hauptbuild noch nicht gelaufen sind, zuerst den vollständigen Build-Flow ausführen (oben).
- Der `make -C test/benchmark ...`-Befehl ist als Nachbau/Fallback gedacht, nicht als Ersatz für fehlende Konfiguration.

## 4. Benchmarks ausführen
### A) Direkte Nutzung

#### `benchmark`
Beispiele:
```bash
cd test/benchmark
./benchmark
./benchmark 1000
./benchmark 1000 --scenario legacy-full --rules-file basic_rules.conf
./benchmark 1000 --scenario request-only --rules-file basic_rules.conf
```

Unterstützte Parameter (laut Usage im Code):
- optionale Iterationszahl (positional)
- `--scenario legacy-full|request-only`
- `--rules-file PATH`

Kurzer Parameterhinweis:
- `num_iterations` (positional): positive Ganzzahl; steuert, wie viele Transaktionen im Loop gemessen werden.
- `--scenario`: wählt den Ablauf (`legacy-full` mit Request+Response-Phasen oder `request-only` mit Request-Phasen).
- `--rules-file`: Pfad zur Rule-Datei, die der Lauf laden soll.

Ausgabe enthält eine Summary mit:
- `scenario`: zeigt, welcher Benchmark-Ablauf tatsächlich verwendet wurde (`legacy-full` oder `request-only`); damit ist klar, auf welchen Transaktionspfad sich die Messung bezieht.
- `rules_file`: zeigt die geladene Rule-Datei; dieser Wert ist wichtig, um Messergebnisse einer konkreten Regelbasis zuzuordnen.
- `elapsed_seconds`: gesamte Laufzeit des Loops über alle Iterationen; bei gleicher Iterationszahl bedeuten kleinere Werte schnellere Gesamtausführung.
- `avg_transaction_ns`: durchschnittliche Zeit pro Transaktion in Nanosekunden; dieser Wert normalisiert die Laufzeit auf eine Einzeltransaktion.
- `throughput_tx_per_sec`: aus der Laufzeit abgeleiteter Durchsatz in Transaktionen pro Sekunde; höhere Werte bedeuten mehr verarbeitete Transaktionen pro Zeit.
- `interventions`: Anzahl erkannter Interventionsereignisse im Lauf; ein hoher Wert zeigt, dass Regeln häufiger in den Request-/Response-Fluss eingegriffen haben.

#### `json_benchmark`
Beispiele:
```bash
cd test/benchmark
./json_benchmark --scenario utf8 --output json
./json_benchmark --scenario large-object --iterations 200 --target-bytes 1048576 --output json
./json_benchmark --scenario truncated --include-invalid --output json
```

Wichtige Parameter:
- `--scenario NAME` (Pflicht): wählt das JSON-Szenario, dessen Body erzeugt und gemessen wird; ohne diesen Parameter startet der Benchmark nicht.
- `--iterations N`: bestimmt die Anzahl der Wiederholungen; größere Werte stabilisieren typischerweise den Mittelwert, erhöhen aber die Laufzeit.
- `--target-bytes N`: setzt die Zielgröße für größenabhängige Szenarien; damit wird der Lastumfang der JSON-Verarbeitung gesteuert.
- `--depth N`: setzt die Verschachtelungstiefe für `deep-nesting`; relevant zur Bewertung tiefer JSON-Strukturen.
- `--include-invalid`: aktiviert absichtlich ungültige JSON-Szenarien (`truncated`, `malformed`) und ist für diese Szenarien erforderlich.
- `--output json`: schaltet auf maschinenlesbare JSON-Ausgabe um; wichtig für automatisches Parsing im Runner.

### B) Vollständiger Runner (`test.sh`)
```bash
cd test/benchmark
./test.sh
```

`test.sh` existiert, um einen standardisierten Vergleichslauf über alle Varianten mit getrennten Logs/Artefakten und zusammengefasstem Vergleich zu erzeugen. Das ist umfangreicher und reproduzierbarer als manuelle Einzelaufrufe.

`test.sh` startet:
- einen `benchmark`-Lauf pro Variante,
- mehrere `json_benchmark`-Läufe pro Variante über feste Größen und Szenarien,
- Parsing/CSV-Erzeugung und globale Vergleichsausgabe.

Standardkonfiguration aus dem Skript:
- Varianten: `baseline`, `crs_v3`, `crs_v4`
- Größen: `256`, `4096`, `51200`, `1048576`
- gültige JSON-Szenarien: `utf8`, `numbers`, `deep-nesting`, `large-object`
- ungültige JSON-Szenarien: `truncated`, `malformed` (werden mit `--include-invalid` gefahren)
- Iterationen:
  - `BENCH_ITERATIONS` Standard: `1000000`
  - `JSON_ITERATIONS` Standard: `100`

Warum Größen/Szenarien getrennt:
- unterschiedliche Größen zeigen Skalierungseffekte (klein bis groß),
- gültige Szenarien zeigen Normalpfade,
- ungültige Szenarien prüfen Fehlerpfade des JSON-Parsers.

## 5. Varianten
| Variante | Regelzustand |
|---|---|
| `baseline` | Original `basic_rules.conf` + `json_benchmark_rules.conf` |
| `crs_v3` | Original-Regeln plus CRS-v3-Includes |
| `crs_v4` | Original-Regeln plus CRS-v4-Includes |

Trennung wird sichergestellt durch:
- Restore aus Original-Backups vor jeder Variante,
- variantenspezifische Output-Verzeichnisse,
- getrennte Logs/Rohdaten/Metriken.

## 6. Ablauf (Execution Flow)
1. Build (siehe Abschnitt 3).
2. `test.sh` startet mit Preflight-Prüfungen (Binaries/Rule-Dateien vorhanden und ausführbar/lesbar).
3. `test.sh` kann fehlende CRS v3/v4 Rules automatisch nachladen (nur wenn sie fehlen).
4. Original-Regeldateien werden gesichert (Backup), spätere Wiederherstellung via `trap`.
5. Pro Variante:
   - Regelvorbereitung (baseline/crs_v3/crs_v4),
   - `benchmark`-Lauf,
   - `json_benchmark`-Läufe über alle Größen und Szenarien.
6. JSON-Ausgaben werden geparst; Metriken werden in CSV/TXT pro Variante geschrieben.
7. Globaler Vergleich (`comparison.csv`, `report_summary.txt`) wird erzeugt.
8. Am Ende werden die ursprünglichen Regeldateien wiederhergestellt.

## 7. Outputs & Ergebnisse
Standard-Ausgabeort:
- `test/benchmark/results/<timestamp>/`

Struktur:
- `run.log`: globales Protokoll
- `run_metadata.txt`: globale Metadaten
- `system_info.txt`: System-/Umgebungsdaten des Laufs (OS, Kernel, Host, CPU, RAM, Virtualisierungshinweis)
- `comparison.csv`: strukturierter Variantenvergleich
- `report_summary.txt`: menschenlesbare Zusammenfassung
- `<variant>/`
  - `metadata.txt`
  - `config/effective_basic_rules.conf`
  - `config/effective_json_benchmark_rules.conf`
  - `logs/variant.log`, `logs/commands.log`, `logs/errors.log`
  - `raw/benchmark.raw.txt`, `raw/json_benchmark.jsonl`
  - `metrics/benchmark_metrics.csv`, `metrics/json_metrics.csv`, `metrics/parsed_summary.txt`

## 8. Metriken
### `benchmark`
- `elapsed_seconds`: gesamte Laufzeit des Benchmark-Loops; je kleiner bei gleicher Iterationszahl, desto schneller der Lauf.
- `avg_transaction_ns`: durchschnittliche Transaktionsdauer in ns (`elapsed / iterations`).
- `throughput_tx_per_sec`: verarbeitete Transaktionen pro Sekunde; höher ist besser.
- `interventions`: Anzahl erkannter Interventionsereignisse.

### `json_benchmark`
- `append_request_body_ns`: kumulierte Zeit für `appendRequestBody` über alle Iterationen.
- `process_request_body_ns`: kumulierte Zeit für `processRequestBody` über alle Iterationen.
- `total_transaction_ns`: kumulierte Gesamtdauer der JSON-Transaktionen über alle Iterationen.
- `parse_success_count`: Anzahl Iterationen mit erfolgreicher JSON-Verarbeitung.
- `parse_error_count`: Anzahl Iterationen mit JSON-Fehlerergebnis.
- `ru_maxrss_kb`: vom Prozess gemeldeter maximaler RSS-Wert in KB.

Im Runner werden diese Werte pro Szenario/Größe erfasst und um Format-/Derived-Felder ergänzt (z. B. `total_transaction_dynamic`, `derived_throughput_tx_per_sec`).

## 9. Formatierungslogik
- Zeitformatierung: zentrale Pipeline in ns mit dynamischer Unit-Wahl (`ns`, `us`, `ms`, `s`), inklusive Grenzfall-Rounding über Unit-Grenzen.
- Throughput-Formatierung: einheitliche Ausgabe mit konfigurierbarer Einheit (z. B. `tx/s`, `iter/s`).
- Speicherformatierung: `format_memory_kb` skaliert von KB dynamisch bis TB.

Warum: Die Reports sollen direkt lesbar sein, ohne Rohwerte zu verlieren (Rohwerte bleiben in CSV/Raw erhalten).

## 10. Logging & Traceability
- Global: Start/Ende, Status, Report-Pfade.
- Pro Variante: Start/Ende, Kommandos, Exit-Codes, Fehler, Metadaten.
- Effektive Rule-Dateien pro Variante werden archiviert (`config/`), damit die exakte Konfiguration nachvollziehbar bleibt.
- Systembasis wird je Lauf separat protokolliert (`system_info.txt`) und in `run_metadata.txt` referenziert/teilweise gespiegelt:
  - Distribution/Version aus `/etc/os-release` (falls vorhanden),
  - Kernel/Plattform aus `uname`,
  - Host/UTC-Zeit/Benutzerkontext,
  - CPU-Daten aus `lscpu` und/oder `/proc/cpuinfo`,
  - RAM aus `/proc/meminfo`,
  - optionaler Virtualisierungshinweis aus belastbaren Indikatoren.
- Diese Daten dokumentieren technische Fakten der Laufumgebung; sie beweisen nicht automatisch „echte Hardware“.

## 11. Fehlerbehandlung
- `set -euo pipefail` aktiv.
- Fail-fast bei fehlenden Kernartefakten (Binaries/Rule-Dateien) im Preflight.
- Variantenfehler werden protokolliert; der Gesamtlauf versucht weitere Varianten und setzt am Ende einen Fehlerstatus, wenn mindestens eine Variante fehlschlug.

## 12. Reproduzierbarkeit
Wichtige Umgebungsvariablen:
- `BENCH_ITERATIONS`
- `JSON_ITERATIONS`
- `RESULTS_ROOT`
- `CRS_V3_SETUP`, `CRS_V3_RULES_GLOB`
- `CRS_V4_SETUP`, `CRS_V4_RULES_GLOB`

Zusätzlich sorgen feste Szenario-/Größenlisten und gespeicherte Rule-Snapshots für reproduzierbare Vergleiche.

## 13. Beispiele
### Beispiel-Lauf
```bash
cd test/benchmark
BENCH_ITERATIONS=200000 JSON_ITERATIONS=50 ./test.sh
```

### Beispiel (gekürzt) `comparison.csv`
```csv
variant,status,duration_ns,duration_dynamic,benchmark_elapsed_seconds,benchmark_elapsed_dynamic,benchmark_throughput_tx_per_sec,json_avg_total_transaction_ns,json_avg_total_transaction_dynamic,json_avg_derived_throughput_tx_per_sec
baseline,ok,1234567890,1.23 s,0.81,810.00 ms,1234567.89,54231.11,54.23 us,1854321.120000
crs_v3,ok,2234567890,2.23 s,1.52,1.52 s,657894.12,88321.44,88.32 us,1154321.550000
crs_v4,ok,2134567890,2.13 s,1.44,1.44 s,694444.44,81234.78,81.23 us,1234567.890000
```

## 14. Grenzen
Nicht Ziel dieses Systems (im Code nicht implementiert):
- verteilte Lasttests,
- Real-Traffic-Replay,
- Latenz-Perzentile (p95/p99),
- End-to-End-Netzwerkmessung.

## 15. System erweitern
- Neue Variante: `VARIANTS` erweitern sowie `validate_variant_dependencies` und `prepare_variant_rules` anpassen.
- Neue Metriken: Parsing/CSV-Schreiber im Runner ergänzen.
- Regeländerungen: `basic_rules.conf` / `json_benchmark_rules.conf` oder CRS-Include-Pfade über Umgebungsvariablen anpassen.

### Abschlussprüfung
- Keine erfundenen Features beschrieben.
- Alle Hauptkomponenten entsprechen den Dateien/Skripten im Verzeichnis.
- Deutsche und englische Sektion sind strukturell gleich.
