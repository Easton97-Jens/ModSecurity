# Repo-Audit

## 1. Scope
- Geprüftes Repo: `/workspace/ModSecurity`
- Audit-Datum: 2026-04-12
- Aktueller Commit: `0abd07983377cbbc8d860fdd43632e67d5d54922`
- Submodule wurden vor Audit geladen mit `git submodule update --init --recursive`.
- Geladene Submodule (rekursiv) inkl. Commits:
  - `others/simdjson` -> `fb83b114efcec4544eba8d45e3c7969ca756c086`
  - `others/jsoncons` -> `128553c8d1b222c30819656d123590accb60689d`
  - plus nicht-JSON-relevante Submodule laut `git submodule status --recursive`.
- Verwendete Repo-Quellen:
  - `src/request_body_processor/json_backend_simdjson.cc`
  - `src/request_body_processor/json_backend_jsoncons.cc`
  - `src/request_body_processor/json_adapter.cc`
  - `src/request_body_processor/json.cc`
  - `configure.ac`, `src/Makefile.am`, `test/run-json-backend-matrix.sh`
  - `test/test-cases/regression/request-body-parser-json*.json`
- Verwendete Upstream-Quellen im Repo (Submodule):
  - simdjson: `others/simdjson/include/simdjson/generic/ondemand/parser.h`, `.../value.h`
  - jsoncons: `others/jsoncons/include/jsoncons/json_cursor.hpp`, `.../json_options.hpp`, `others/jsoncons/doc/Examples.md`

## 2. Fundstellen im Repo
### simdjson
- `src/request_body_processor/json_backend_simdjson.cc`: komplette Backend-Implementierung (Parser-Lifecycle, Event-Walking, Fehler-Mapping, Number/String Handling).
- `src/Makefile.am` + `configure.ac`: Build-Auswahl, Include-Path, Singleheader-Source `others/simdjson/singleheader/simdjson.cpp`.
- `build/win32/CMakeLists.txt`: Backend-Umschaltung auch für Windows-Build.
- `test/run-json-backend-matrix.sh`: expliziter Lauf gegen `simdjson`.

### jsoncons
- `src/request_body_processor/json_backend_jsoncons.cc`: komplette Backend-Implementierung (Cursor, Event-Loop, Raw-Token-Synchronisierung, Fehler-Mapping).
- `src/Makefile.am` + `configure.ac`: Build-Auswahl, Include-Path `others/jsoncons/include`.
- `build/win32/CMakeLists.txt`: Backend-Umschaltung für Windows.
- `test/run-json-backend-matrix.sh`: expliziter Lauf gegen `jsoncons`.

## 3. Korrektheitsprüfung der Nutzung im Repo
### Stelle: `parseDocumentWithSimdjson`
- Bibliothek: simdjson
- Was der Code macht:
  - nutzt einen `thread_local` wiederverwendeten `ondemand::parser`.
  - reserviert Parser-Kapazität/Max-Tiefe via `allocate`.
  - erstellt immer `simdjson::padded_string` aus Input und ruft `parser.iterate(padded)` auf.
- Relevante Doku-Regel:
  - simdjson fordert Padding (`SIMDJSON_PADDING`) und dokumentiert Lifetime-Anforderungen für Buffer/Document.
- Upstream-Codebezug:
  - `others/simdjson/include/simdjson/generic/ondemand/parser.h` (Padding- und Lifetime-Hinweise).
- Bewertung: **Korrekt**.
  - Die explizite `padded_string`-Kopie ist defensiv korrekt bzgl. Padding/Lifetime, wenn Aufrufer nur `const std::string` liefert.
- Evidenz:
  - Repo: parser reuse + padded copy + iterate.
  - Upstream: Padding- und Lifetime-Anforderungen.

### Stelle: simdjson-Event-Walker (`walk*`)
- Bibliothek: simdjson
- Was der Code macht:
  - unterscheidet Wurzel-Scalar vs Container und emittiert sink events (`on_start_object`, `on_key`, `on_number`, ...).
  - Zahlen werden als Raw-Lexem via `raw_json_token()` weitergegeben.
  - Strings über `get_string()` als UTF-8-validierter View.
- Relevante Doku-Regel:
  - `get_string()` liefert UTF-8 und ist konsumierend (value once).
- Upstream-Codebezug:
  - `others/simdjson/include/.../value.h`.
- Bewertung: **Teilweise korrekt, mit offenem Risiko**.
  - Positiv: einmalige konsumierende Nutzung pro Wert (kein doppeltes `get_string()` auf demselben value erkennbar).
  - Risiko/Hypothese: `walkNumber` nutzt `value.raw_json_token()` ohne explizites Fehlerobjekt; bei API-Änderungen oder inkonsistentem Iteratorzustand wäre Fehlerbehandlung asymmetrisch zu anderen Pfaden.
- Evidenz:
  - Repo-Walker-Code und Number/String-Pfade.
  - Upstream-Doku zu `get_string()`-Semantik.

### Stelle: `parseDocumentWithJsoncons`
- Bibliothek: jsoncons
- Was der Code macht:
  - setzt `max_nesting_depth`, `lossless_number(true)`, `lossless_bignum(true)`.
  - parst per `json_string_cursor` und iteriert STAJ-Events.
  - nutzt eigenen `RawJsonTokenCursor`, um exaktes numerisches Raw-Lexem aus Originalinput zu rekonstruieren.
- Relevante Doku-Regel:
  - jsoncons bietet Optionen `lossless_number`, `lossless_bignum`, `max_nesting_depth`.
  - STAJ Event Loop (`current`/`next`/`done`) ist vorgesehen.
- Upstream-Codebezug:
  - `others/jsoncons/include/jsoncons/json_options.hpp`, `json_cursor.hpp`, `doc/Examples.md`.
- Bewertung: **Korrekt, aber komplex**.
  - Numerik wird bewusst semantisch/lossless behandelt; zusätzliche Token-Synchronisierung ist im Repo implementiert, nicht nativer jsoncons-Standardpfad.
- Evidenz:
  - Repo-Options + Event-Loop + Token-Cursor.
  - Upstream-Optionen und Event-Beispiel.

### Stelle: Fehlerklassifikation beider Backends
- Bibliotheken: simdjson/jsoncons
- Was der Code macht:
  - simdjson: `DEPTH_ERROR` -> `ParseError`.
  - jsoncons: `max_nesting_depth_exceeded` -> `InternalError`.
- Relevante Doku-Regel:
  - beide Fehler sind parse/input-bezogen (nicht zwingend interne Speicherkorruption).
- Bewertung: **Problematisch (Backend-Inkonsistenz)**.
  - Gleichartige Inputs können unterschiedliche Fehlerklassen (`ParseError` vs `InternalError`) liefern.
- Evidenz:
  - error mapping in beiden Backend-Dateien.

## 4. Vergleich der Backends im Repo
- Gemeinsamkeiten:
  - Beide Backends rufen denselben `JsonEventSink` mit denselben Eventtypen auf.
  - Beide behandeln root scalar explizit.
  - Beide geben Zahlen als Textrepräsentation an `on_number` weiter.
- Unterschiede:
  - simdjson liest Zahlentoken direkt via `raw_json_token()`.
  - jsoncons rekonstruiert Zahlentoken zusätzlich über `RawJsonTokenCursor` + Kontextpositionen; bei `semantic_tag::bigint/bigdec` existiert Sonderpfad.
  - Fehlerklassifikation bei Depth-Limit differiert (siehe oben).
- Potenzielle Semantikabweichungen:
  - Unterschiedliche Fehlerklassifikation bei tief verschachtelten Inputs.
  - jsoncons-Pfad für numerische String-Events ist komplexer und fehleranfälliger bei Synchronisationsdrift.
- Paritätsstatus:
  - **Backend-Parität nicht vollständig verifizierbar**.
  - Grund: Im Audit wurden keine vollständigen Backend-Matrix-Testläufe/Benchmarks ausgeführt; nur Code- und Testfall-Inspektion.
- Evidenz:
  - Backend-Implementierungen, Matrix-Script, vorhandene Regression-Cases.

## 5. Performance-Optimierungen im Repo
1) Stelle: `json_backend_simdjson.cc` (`simdjson::padded_string padded(input)`)
- Aktuelles Verhalten: immer vollständige Input-Kopie in padded buffer.
- Warum teuer: O(n)-Copy je Request-Body.
- Konkrete Verbesserung: optionaler zero-copy Pfad nur wenn Aufrufer garantierte Padding/Lifetime liefern kann.
- Evidenzgrad: **stark belegt** (Code + simdjson Padding-Doku).
- Gemessen: **nicht gemessen**.

2) Stelle: `json_backend_jsoncons.cc` (`RawJsonTokenCursor`)
- Aktuelles Verhalten: zusätzliche Zeichen-für-Zeichen Token-Synchronisierung parallel zum jsoncons Parser.
- Warum teuer: zusätzlicher CPU-Scan; bei vielen Zahlen steigt Overhead.
- Konkrete Verbesserung: prüfen, ob Kontext-Offsets alleine stabil ausreichen, und Fallback-Scanning nur im Fehlerfall aktivieren.
- Evidenzgrad: **plausible Hypothese** (strukturell erkennbar, keine Laufzeitmessung).
- Gemessen: **nicht gemessen**.

3) Stelle: `json.cc` (`on_string`, `on_number`)
- Aktuelles Verhalten: `std::string_view` wird in `std::string` materialisiert (`addArgument(std::string(...))`).
- Warum teuer: zusätzliche Allokationen/Kopien pro atomarem Wert.
- Konkrete Verbesserung: falls `Transaction::addArgument` erweitert werden kann, `string_view`-Pfad ohne sofortige Kopie.
- Evidenzgrad: **stark belegt**.
- Gemessen: **nicht gemessen**.

## 6. Speicher- und Speicherverwaltungs-Optimierungen im Repo
1) Stelle: simdjson Backend
- Aktuelles Verhalten: Originalbody (`m_data`) + `padded_string` gehalten.
- Warum speicherrelevant: kurzfristig doppelte Repräsentation großer Payloads.
- Verbesserung: optionaler gepaddeter Inputpfad schon beim Chunk-Akkumulator.
- Evidenzgrad: **stark belegt**.
- Gemessen: **nicht gemessen**.

2) Stelle: jsoncons Backend
- Aktuelles Verhalten: hält nur Originalstring, aber zusätzlicher `RawJsonTokenCursor`-State.
- Warum speicherrelevant: gering, primär CPU-lastig; Memory-Mehrbedarf relativ klein.
- Verbesserung: minimaler Nutzen; Fokus eher CPU.
- Evidenzgrad: **plausible Hypothese**.
- Gemessen: **nicht gemessen**.

3) Stelle: JSON sink integration (`json.cc`)
- Aktuelles Verhalten: String/Number Werte werden als neue `std::string` an `addArgument` übergeben.
- Warum speicherrelevant: häufige temporäre Allokationen, besonders bei großen Arrays vieler Scalars.
- Verbesserung: API-Erweiterung für nicht-owning Übergabe oder arena/reuse strategy.
- Evidenzgrad: **stark belegt**.
- Gemessen: **nicht gemessen**.

## 7. Test- und Evidenzlücken
- Fehlende direkte Messungen:
  - keine Benchmark-Messung im Audit ausgeführt (`test/benchmark/...` nur inspiziert).
- Fehlende harte Paritätsbeweise:
  - vorhandenes Matrix-Skript existiert, aber nicht im Audit-Lauf ausgeführt.
- Konkrete zusätzliche Testfälle empfohlen:
  - leeres Objekt `{}` und leeres Array `[]` (explizite parity asserts je Backend)
  - sehr tiefe Verschachtelung an/über konfiguriertem Limit
  - sehr große Integer >64-bit, negative große Integer
  - Floats mit Exponent, führenden/folgenden Nullen, `-0`, `1e-9999`
  - invalides UTF-8 (nicht nur invalid escape)
  - abgeschnittene Tokens an verschiedenen Positionen
  - große Payloads + viele kleine JSON-Dokumente (Allocator/throughput)
- Nicht verifizierbar:
  - exakte Laufzeit-/Speichergewinne der vorgeschlagenen Optimierungen.
  - vollständige Backend-Parität über alle Randfälle.

## 8. Wichtigste Maßnahmen
1) Fehlerklassifikation angleichen (Depth-Limit in jsoncons auf ParseError-Mapping prüfen) – hohe Priorität.
2) Backend-Matrix-Skript in CI als verpflichtenden Paritäts-Check ausführen (mindestens die Edgecase-Suite).
3) Simdjson-Kopiepfad messbar machen und optionalen zero-copy Pfad hinter klarer Contract-API einführen.
4) String/Number Übergabe an `addArgument` auf unnötige Kopien überprüfen und ggf. API erweitern.
