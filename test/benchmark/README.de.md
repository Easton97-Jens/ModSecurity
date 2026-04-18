# Benchmark-Runner (`test/benchmark/test.sh`) – Deutsche Dokumentation

## Zweck des Skripts
`test/benchmark/test.sh` führt in **einem Gesamtlauf** drei klar getrennte Benchmark-Varianten aus:
1. `baseline` (ohne CRS)
2. `crs_v3` (mit OWASP CRS v3 Includes)
3. `crs_v4` (mit OWASP CRS v4 Includes)

Das Skript erzeugt nachvollziehbare Rohdaten, strukturierte Metriken und eine menschenlesbare Vergleichsausgabe pro Variante sowie global für den Gesamtlauf.

## Überblick über die 3 Varianten
- **baseline**
  - Nutzt nur die Original-Regeldateien `basic_rules.conf` und `json_benchmark_rules.conf`.
- **crs_v3**
  - Hängt CRS-v3-Include-Zeilen an beide Regeldateien an.
  - Standardpfade:
    - `test/benchmark/owasp-v3/crs-setup.conf.example`
    - `test/benchmark/owasp-v3/rules/*.conf`
- **crs_v4**
  - Hängt CRS-v4-Include-Zeilen an beide Regeldateien an.
  - Standardpfade:
    - `test/benchmark/owasp-v4/crs-setup.conf.example`
    - `test/benchmark/owasp-v4/rules/*.conf`

Alle Pfade können über Umgebungsvariablen überschrieben werden (`CRS_V3_SETUP`, `CRS_V3_RULES_GLOB`, `CRS_V4_SETUP`, `CRS_V4_RULES_GLOB`).

## Warum die Varianten strikt getrennt sind
Die Trennung erfolgt technisch über:
- Neuaufbau der aktiven Regeldateien pro Variante aus den Original-Backups.
- Eigene Verzeichnisse je Variante (`results/<run-ts>/<variante>/...`).
- Eigene Logs, Rohdaten und Metrikdateien je Variante.
- Getrennte Statusführung und Fehlererfassung je Variante.

Dadurch werden keine Zwischenergebnisse zwischen Varianten wiederverwendet oder überschrieben.

## Ablauf des Gesamtlaufs
1. Preflight-Checks (Binaries + Regeldateien vorhanden).
2. Backup der Original-Regeldateien.
3. Nacheinander: `baseline`, `crs_v3`, `crs_v4`.
4. Pro Variante:
   - Abhängigkeitsprüfung (CRS-Dateien bei CRS-Varianten)
   - Regelvorbereitung
   - Ausführung `benchmark`
   - Ausführung `json_benchmark` für alle Größen/Szenarien
   - Parsing + Metrikablage
5. Globaler Vergleichsreport + Vergleichs-CSV.
6. Restore der Original-Regeldateien (via `trap`).

## Verzeichnisstruktur der Ergebnisse
Standardziel:
- `test/benchmark/results/<YYYYMMDDTHHMMSSZ>/`

Beispielstruktur:
- `run.log` (globales Laufprotokoll)
- `run_metadata.txt` (globale Laufmetadaten)
- `comparison.csv` (strukturierter Vergleich aller Varianten)
- `report_summary.txt` (menschenlesbarer Vergleich)
- `<variant>/`
  - `metadata.txt`
  - `config/effective_basic_rules.conf`
  - `config/effective_json_benchmark_rules.conf`
  - `logs/variant.log`
  - `logs/commands.log`
  - `logs/errors.log`
  - `raw/benchmark.raw.txt`
  - `raw/json_benchmark.jsonl`
  - `metrics/benchmark_metrics.csv`
  - `metrics/json_metrics.csv`
  - `metrics/parsed_summary.txt`

## Logging-Konzept
### Global
- Start/Ende Gesamtlauf
- Gesamtstatus
- Gesamtdauer
- Pfade zu den globalen Reports

### Pro Variante
- Start/Ende inklusive Dauer
- aufgerufene Kommandos (`commands.log`)
- Exit-Codes jedes Kommandos
- Fehler/Warnungen (`errors.log`)
- Metadaten (`metadata.txt`)
- aktive Regeldateien als Kopie in `config/`

## Report-Konzept
### Rohdaten
- `benchmark.raw.txt`: Originalausgabe von `./benchmark`
- `json_benchmark.jsonl`: JSON-Ausgaben von `./json_benchmark`

### Strukturierte Auswertung
- `benchmark_metrics.csv`:
  - elapsed seconds, durchschnittliche ns/Transaktion, Throughput, Interventions
- `json_metrics.csv`:
  - pro Szenario/Größe die JSON-Metriken und abgeleiteter Throughput

### Aufbereitete Zusammenfassung
- Global: `report_summary.txt`
- Vergleich baseline vs crs_v3 vs crs_v4
- Enthält Status, Dauer, Throughput und JSON-Aggregate

## Bedeutung der Kennzahlen
- **elapsed_seconds**: Gesamtzeit für `BENCH_ITERATIONS` Transaktionen (`benchmark`)
- **avg_transaction_ns**: Durchschnittliche Zeit pro Transaktion
- **throughput_tx_per_sec**: Durchsatz in Transaktionen/Sekunde
- **interventions**: erkannte Interventionsereignisse aus `benchmark`
- **process_request_body_ns / total_transaction_ns**: JSON-spezifische Zeiten aus `json_benchmark`
- **ru_maxrss_kb**: Maximaler RSS (KB) aus `json_benchmark`

## Herleitung der Zeitformatierung
Das Skript nutzt dynamische Skalierung:
- `< 1.000 ns` → `ns`
- `< 1.000.000 ns` → `µs` (`us` im Text)
- `< 1.000.000.000 ns` → `ms`
- sonst `s`

Die Implementierung verwendet eine einheitliche Pipeline über Nanosekunden:
- `format_duration_from_ns()` ist die zentrale Funktion.
- `format_time_dynamic_from_ns()` und `format_seconds_dynamic()` sind Wrapper, die dieselbe Formatierungslogik nutzen.

Boundary-Rounding:
- Es wird auf 2 Dezimalstellen (konfigurierbar) gerundet.
- Wenn die Rundung den Grenzwert einer Einheit überschreitet (`>= 1000`), wird automatisch in die nächste Einheit gewechselt.
- Beispiel: `999.999 us` wird als `1.00 ms` dargestellt.

Ungültige Eingaben (leer, nicht numerisch, negative Werte) liefern `n/a`.

## Robuste Hilfsfunktionen
- `format_memory_kb(value, decimals)`:
  - Eingabe in KB, dynamische Ausgabe in `B`, `KB`, `MB`, `GB`, `TB`.
  - Rundung ist konfigurierbar (`decimals`).
  - Grenzfälle:
    - leerer Wert → `n/a`
    - ungültiger Wert → `invalid(...)`
    - negativer Wert → `invalid_negative(...)`
- `format_throughput(value, unit, decimals)`:
  - Einheit ist konfigurierbar (z. B. `tx/s`, `req/s`, `iter/s`).
  - Rundung ist konfigurierbar.
  - Grenzfälle wie bei `format_memory_kb` werden explizit gekennzeichnet.
- `json_get(key, json_line)`:
  - nutzt bevorzugt `jq`, wenn verfügbar.
  - unterscheidet Fehlerfälle via Exit-Code und Fehlermeldung:
    - fehlender Key,
    - leerer/null Wert,
    - Parsing-Fehler.
  - Fallback ohne `jq` ist bewusst als begrenzt dokumentiert und meldet diesen Zustand im Log.
- `write_csv_header(file, columns...)`:
  - Header sind zentral/dynamisch aufbaubar statt hart codiert.
  - dadurch leichter erweiterbar bei geänderten Metrik-Schemata.

## Herleitung der Durchsatzberechnung
- `benchmark`: Wert wird direkt aus der Programmausgabe (`throughput_tx_per_sec`) übernommen.
- `json_benchmark` (abgeleitet):
  - `derived_tps = iterations / (total_transaction_ns / 1e9)`
  - wird pro Messpunkt in `json_metrics.csv` abgelegt.

## Fehlerbehandlung
- `set -euo pipefail` aktiv.
- Fail-fast pro Variante bei Kommando-/Dependency-Fehlern.
- Fehlermeldungen in `errors.log` und globalem `run.log`.
- Gesamtlauf läuft weiter, damit Status aller Varianten sichtbar ist.
- Gesamt-Exit-Code ist `1`, wenn mindestens eine Variante fehlschlägt.

## Reproduzierbarkeit
- Konfigurierbare Iterationen via `BENCH_ITERATIONS` und `JSON_ITERATIONS`.
- Konsistente Szenarien/Größenlisten für alle Varianten.
- Effektive Regeldateien werden pro Variante archiviert.
- Original-Regeln werden nach Lauf wiederhergestellt.

## Beispielaufrufe
```bash
cd test/benchmark
./test.sh
```

Mit angepassten Iterationen:
```bash
BENCH_ITERATIONS=200000 JSON_ITERATIONS=50 ./test.sh
```

Mit eigener Ergebniswurzel:
```bash
RESULTS_ROOT="$(pwd)/results/custom-run" ./test.sh
```

## Beispielausgaben (gekürzt)
`comparison.csv`:
```csv
variant,status,duration_ns,duration_dynamic,benchmark_elapsed_seconds,benchmark_elapsed_dynamic,benchmark_throughput_tx_per_sec,json_avg_total_transaction_ns,json_avg_total_transaction_dynamic,json_avg_derived_throughput_tx_per_sec
baseline,ok,1234567890,1.23 s,0.81,810.00 ms,1234567.89,54231.11,54.23 us,1854321.120000
crs_v3,ok,2234567890,2.23 s,1.52,1.52 s,657894.12,88321.44,88.32 us,1154321.550000
crs_v4,ok,2134567890,2.13 s,1.44,1.44 s,694444.44,81234.78,81.23 us,1234567.890000
```

## Erweiterung um weitere Varianten
Für eine neue Variante:
1. Namen in `VARIANTS` ergänzen.
2. In `validate_variant_dependencies` und `prepare_variant_rules` entsprechende Logik ergänzen.
3. Optional neue Umgebungsvariablen für Rule-Pfade hinzufügen.

Die restliche Ausführung/Logging/Reporting-Pipeline bleibt unverändert.
