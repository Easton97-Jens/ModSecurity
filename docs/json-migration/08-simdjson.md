# SIMDJSON

## 1. Kurzfazit
- Exzellent für Performance, aber konzeptionell kein direkter YAJL-Klon.
- YAJL-Ersatz nur teilweise realistisch (DOM/on-demand ja, Writer nein).

## 2. Projektstatus und Wartbarkeit
- Repo: <https://github.com/simdjson/simdjson>
- Doku: <https://simdjson.org/>
- Stand 2026-04-07: sehr aktiv, Release v4.6.1 (2026-04-03).

## 3. Technisches Modell
- Sprache: C++.
- Modell: ondemand parser + DOM; stark auf throughput/low-latency.
- Writer/Generator: nicht primärer Bestandteil.
- Incremental SAX-Callbacks wie YAJL: nicht zentrales Nutzungsmodell.

## 4. Build-/Integration
- Systempaket: teils vorhanden, aber Versionen können hinterherhinken.
- Bundled/Submodule: häufig in Performance-Projekten.

## 5. Passung zum Repo
- Geeignet für schnelle Test-/Input-Reads.
- Schlechter Fit für bestehenden writer-heavy YAJL usage (`yajl_gen_*`).
- Request-body callback semantics müssten neu modelliert werden.

## 6. Modul vs. Systempaket
- Optional beides; bei Performance-Tuning oft bundled.

## 7. Integrationsstrategie
- Als **zusätzliches DOM/perf backend** hinter Capability `HAS_ZERO_COPY_DOM`.
- Nicht als primärer universeller YAJL-Ersatz.

## 8. Migrationsaufwand
- **hoch** bei Vollersatz.

## 9. Empfehlung
- **nur experimentell/spezial Use Cases**.
