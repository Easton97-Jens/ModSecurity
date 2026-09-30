# NLOHMANN/JSON

## 1. Kurzfazit
- Sehr gut wartbar und weit verbreitet, aber primär DOM-/value-basiert.
- YAJL-Ersatz: **für DOM/Tests gut**, für SAX-incremental Request-Parsing nur bedingt.
- Repo-Fit: gut für C++-Teile, weniger gut als direkter YAJL-SAX-Ersatz.

## 2. Projektstatus und Wartbarkeit
- Repo: <https://github.com/nlohmann/json>
- Doku: <https://json.nlohmann.me/>
- GitHub API (2026-04-07): aktive Entwicklung, Release v3.12.0 (2025-04-11).

## 3. Technisches Modell
- Sprache: C++ (header-only).
- Modelle: DOM, SAX-Interface vorhanden, JSON Pointer/Patch, binary formats.
- Incremental: kein YAJL-ähnlicher low-level incremental stream parser als Kernpfad.
- Fehler: Exceptions oder error_code-Varianten.
- Memory: DOM-lastig, potentiell höherer Overhead.

## 4. Build-/Integration
- Systempaket: sehr gut verfügbar.
- Bundled: trivial (single include tree).
- pkg-config: uneinheitlich; CMake package verbreitet.

## 5. Passung zum Repo
- Sehr passend als Ersatz für `yajl_tree_parse`-basierte Tests.
- Writer-Ersatz möglich, aber weniger effizient/streaming-nativ als YAJL-Gen.
- RequestBodyProcessor-JSON (chunkweise, callback-zentriert) braucht Adapter oder Designänderung.

## 6. Modul vs. Systempaket
- Beides realistisch.
- Für Distros/Systemintegration: **Systempaket bevorzugt**.
- Für einheitliche CI: optional vendor snapshot.

## 7. Integrationsstrategie
- Erst in **Test-DOM-Schicht** einführen.
- Optional später als Writer-Backend.
- Nicht als erster Ersatz für produktiven incremental parser.

## 8. Migrationsaufwand
- **mittel bis hoch** (abhängig davon, ob nur Tests oder auch Runtime-Writer/Parser).

## 9. Empfehlung
- **ja, aber primär für C++-DOM/Tests**.
- Priorisierung: **starker Kandidat für Testmodernisierung**, nicht Primärkandidat für RequestBody SAX.
