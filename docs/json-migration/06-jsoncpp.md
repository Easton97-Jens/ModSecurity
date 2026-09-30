# JSONCPP

## 1. Kurzfazit
- Klassische C++ JSON-Library, stabil einsetzbar.
- YAJL-Ersatz: gut für DOM/Writer, weniger direkt für callbackbasiertes incremental Parsing.

## 2. Projektstatus und Wartbarkeit
- Repo: <https://github.com/open-source-parsers/jsoncpp>
- Doku: Repo + generated docs.
- Stand 2026-04-07: aktive Releases, u. a. 1.9.7 (2026-03-19).

## 3. Technisches Modell
- Sprache: C++.
- Modell: DOM (`Json::Value`) + Reader/CharReader + StreamWriterBuilder.
- Incremental SAX: nicht primäres Modell.

## 4. Build-/Integration
- Systempaket: breit vorhanden.
- Bundled: möglich.
- CMake/pkg-config Unterstützung üblich.

## 5. Passung zum Repo
- Sehr brauchbar für Testdatei-Parsing und JSON-Output-Pfade.
- Request-body incremental parser erfordert Adapter/Architekturänderung.

## 6. Modul vs. Systempaket
- **Systempaket bevorzugt**, optional bundled für CI-Reproduzierbarkeit.

## 7. Integrationsstrategie
- Als DOM+Writer backend einführen.
- Streaming separat kapseln (anderes Backend) oder emulieren.

## 8. Migrationsaufwand
- **mittel** (wenn nicht als alleiniger Backend-Typ erzwungen).

## 9. Empfehlung
- **ja, nur mit Abstraktionsschicht**, eher für C++-Pfad/Testdomäne.
