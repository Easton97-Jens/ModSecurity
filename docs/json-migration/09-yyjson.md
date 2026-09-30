# YYJSON

## 1. Kurzfazit
- Sehr attraktiver moderner C-Kandidat (Performance + C-API).
- YAJL-Ersatz: gut für DOM und teilweise reader/writer; incremental-SAX-Mapping prüfen.

## 2. Projektstatus und Wartbarkeit
- Repo: <https://github.com/ibireme/yyjson>
- Doku: <https://ibireme.github.io/yyjson/>
- Stand 2026-04-07: aktiv, Release 0.12.0 (2025-08-18).

## 3. Technisches Modell
- Sprache: C.
- Modelle: immutable/mutable DOM, reader, writer APIs.
- Gute Performancecharakteristik.
- Fehlerreports und memory allocator Optionen vorhanden.

## 4. Build-/Integration
- Systempaket: je Distro unterschiedlich; nicht überall default.
- Bundled: sehr gut machbar (klein, C).

## 5. Passung zum Repo
- Writer-Ersatz gut denkbar.
- Test-DOM gut migrierbar.
- Für bestehendes callback-incremental Verhalten evtl. Adapter nötig; genaue Semantik im PoC verifizieren.

## 6. Modul vs. Systempaket
- **beides optional**, praktisch oft bundled + optional system fallback.

## 7. Integrationsstrategie
- Früh als PoC nach json-c/jansson testen.
- Capability-Flag gesteuert nutzen.

## 8. Migrationsaufwand
- **mittel** (bei klarer Abstraktionsschicht).

## 9. Empfehlung
- **ja (starker Kandidat)**, besonders für C-zentrierte Backend-Option.
