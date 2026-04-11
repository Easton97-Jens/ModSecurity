# Repo-Audit

## 1. Scope
- Geprüftes Repo: `ModSecurity` unter `/workspace/ModSecurity`.
- Aktueller Repo-Commit (Superproject): wird über `git rev-parse HEAD` bestimmt.
- Submodule-Ladevorgang wurde ausgeführt: `git submodule update --init --recursive`.
- Geladene relevante Submodule:
  - `others/simdjson` @ `fb83b114efcec4544eba8d45e3c7969ca756c086`
  - `others/jsoncons` @ `128553c8d1b222c30819656d123590accb60689d`
- Verwendete Dokuquellen (Upstream):
  - simdjson: `others/simdjson/doc/basics.md`, `others/simdjson/doc/ondemand_design.md`, `others/simdjson/doc/parse_many.md`, `others/simdjson/doc/performance.md`, `others/simdjson/doc/dom.md`
  - jsoncons: `others/jsoncons/doc/Reference.md`, `others/jsoncons/doc/Examples.md`, `others/jsoncons/doc/ref/corelib/basic_json_cursor.md`, `others/jsoncons/doc/ref/corelib/basic_json_options.md`, `others/jsoncons/doc/build.md`
- Verwendete Upstream-Quellen (Code):
  - simdjson: `others/simdjson/singleheader/simdjson.h`
  - jsoncons: `others/jsoncons/include/jsoncons/json_cursor.hpp`, `others/jsoncons/include/jsoncons/json_options.hpp`

## 2. Fundstellen im Repo
### simdjson
- `src/request_body_processor/json_backend_simdjson.cc` / `parseDocumentWithSimdjson` / Parser-Erzeugung + `padded_string`-Kopie + `iterate`.
- `src/request_body_processor/json_backend_simdjson.cc` / `JsonBackendWalker::{walk,walkObject,walkArray,walkString,walkNumber,walkBoolean}` / Event-Transformation Richtung `JsonEventSink`.
- `src/request_body_processor/json_backend.h` / `parseDocumentWithSimdjson` / Backend-Schnittstelle.
- `src/Makefile.am` / `JSON_BACKEND_SIMDJSON` / Einbindung `../others/simdjson/singleheader/simdjson.cpp`.

### jsoncons
- `src/request_body_processor/json_backend_jsoncons.cc` / `parseDocumentWithJsoncons` / `json_string_cursor`, Optionen, Eventloop.
- `src/request_body_processor/json_backend_jsoncons.cc` / `emitEvent` / Zuordnung STAJ-Events auf `JsonEventSink`.
- `src/request_body_processor/json_backend_jsoncons.cc` / `RawJsonTokenCursor` / parallele Roh-Token-Synchronisierung.
- `src/request_body_processor/json_backend.h` / `parseDocumentWithJsoncons` / Backend-Schnittstelle.
- `src/Makefile.am`, `test/Makefile.am` / Include-Pfade `others/jsoncons/include`.

## 3. Korrektheitsprüfung der Nutzung im Repo

1) **Stelle im Repo**: `parseDocumentWithSimdjson`
- **Bibliothek**: simdjson
- **Was der Code macht**: erstellt `ondemand::parser`, kopiert Input in `simdjson::padded_string`, ruft `parser.iterate(padded)` auf.
- **Relevante Doku-Regel**: simdjson verlangt Padding bzw. nutzt `padded_string`; Parser soll für mehrere Dokumente wiederverwendet werden (Performance-Empfehlung).
- **Upstream-Codebezug**: `raw_json_token`/ondemand APIs im singleheader vorhanden.
- **Bewertung**: **Korrekt** (API-Nutzung korrekt); **nicht optimal** bzgl. Reuse.
- **Evidenz**: `src/request_body_processor/json_backend_simdjson.cc` (`parseDocumentWithSimdjson`), `others/simdjson/doc/basics.md` (Padding/iterate), `others/simdjson/doc/performance.md` (Parser-Reuse), `others/simdjson/singleheader/simdjson.h` (`ondemand::parser`, `padded_string`).

2) **Stelle im Repo**: `JsonBackendWalker` (simdjson)
- **Bibliothek**: simdjson
- **Was der Code macht**: erkennt Typ (`json_type`) und emittiert für Object/Array/String/Number/Bool/Null entsprechende Sink-Events.
- **Relevante Doku-Regel**: On-Demand ist forward-only/iteratorbasiert.
- **Upstream-Codebezug**: On-Demand Design und Value-/Document-API.
- **Bewertung**: **Teilweise korrekt**: Traversal passt zum On-Demand-Modell; vollständige Gleichheit zu allen Upstream-Semantiken **nicht verifizierbar** ohne vollständige Gegen-Tests je Randfall.
- **Evidenz**: `src/request_body_processor/json_backend_simdjson.cc` (`walk*`), `others/simdjson/doc/ondemand_design.md` (forward-only, single-pass).

3) **Stelle im Repo**: simdjson Fehlerabbildung `fromSimdjsonError`
- **Bibliothek**: simdjson
- **Was der Code macht**: mappt simdjson-Fehlercodes auf `JsonParseStatus`.
- **Relevante Doku-Regel**: keine vollständige normative Mapping-Tabelle in den genannten Docs.
- **Upstream-Codebezug**: simdjson error enums vorhanden.
- **Bewertung**: **Teilweise korrekt / Nicht vollständig verifizierbar** (Mapping-Policy repo-spezifisch).
- **Evidenz**: `src/request_body_processor/json_backend_simdjson.cc` (`fromSimdjsonError`), `others/simdjson/singleheader/simdjson.h` (Fehlercodes).

4) **Stelle im Repo**: `parseDocumentWithJsoncons`
- **Bibliothek**: jsoncons
- **Was der Code macht**: setzt `max_nesting_depth`, `lossless_number(true)`, `lossless_bignum(true)`, iteriert `json_string_cursor` via `current()/next()/done()`.
- **Relevante Doku-Regel**: Cursor-Pull-API (`current/next/done`), Optionen für `max_nesting_depth`, `lossless_number`, `lossless_bignum`.
- **Upstream-Codebezug**: Optionen und Cursor-Typen im Header.
- **Bewertung**: **Korrekt**.
- **Evidenz**: `src/request_body_processor/json_backend_jsoncons.cc` (`parseDocumentWithJsoncons`), `others/jsoncons/doc/ref/corelib/basic_json_cursor.md`, `others/jsoncons/doc/ref/corelib/basic_json_options.md`, `others/jsoncons/include/jsoncons/json_options.hpp`, `others/jsoncons/include/jsoncons/json_cursor.hpp`.

5) **Stelle im Repo**: jsoncons Zahlbehandlung (`isNumericStringEvent`, `rawNumberFromContext`, `emitEvent`)
- **Bibliothek**: jsoncons
- **Was der Code macht**: behandelt `semantic_tag::bigint`/`bigdec` als numerische JSON-Tokens und reicht diese an `on_number` weiter.
- **Relevante Doku-Regel**: bei `lossless_number`/`lossless_bignum` werden Zahlen ggf. als Strings mit `bigint`/`bigdec` repräsentiert.
- **Upstream-Codebezug**: Optionen + semantic tags.
- **Bewertung**: **Korrekt** (innerhalb repo-Ziel: numerische Semantik erhalten).
- **Evidenz**: `src/request_body_processor/json_backend_jsoncons.cc` (`isNumericStringEvent`, `rawNumberFromContext`, `emitEvent`), `others/jsoncons/doc/ref/corelib/basic_json_options.md` (lossless-Regeln).

6) **Stelle im Repo**: Lifetime / Ownership
- **Bibliothek**: beide
- **Was der Code macht**: `std::string_view`-Daten werden an Sink gereicht; Sink kopiert zu `std::string` (`on_string`, `on_number`).
- **Relevante Doku-Regel**: Views sind an Input/Parser-Lifetime gebunden (simdjson); Cursor darf Source nicht überleben (jsoncons).
- **Upstream-Codebezug**: Doku stellt Lifetime-Pflichten klar.
- **Bewertung**: **Korrekt** (keine offensichtliche Dangling-View-Stelle im gezeigten Pfad).
- **Evidenz**: `src/request_body_processor/json.cc` (`on_string`, `on_number`), `src/request_body_processor/json_backend_simdjson.cc`, `src/request_body_processor/json_backend_jsoncons.cc`, `others/simdjson/doc/basics.md`, `others/jsoncons/doc/ref/corelib/basic_json_cursor.md`.

7) **Stelle im Repo**: Parse-Optionen-Parität
- **Bibliothek**: beide
- **Was der Code macht**: jsoncons nutzt `options.technical_max_depth`; simdjson ignoriert `options` explizit.
- **Relevante Doku-Regel**: keine gemeinsame Cross-Backend-Spezifikation in Upstream-Dokus; dies ist Repo-API-Vertrag.
- **Upstream-Codebezug**: nicht erforderlich.
- **Bewertung**: **Problematisch** (Backend-Inkonsistenz im Repo-Vertrag möglich).
- **Evidenz**: `src/request_body_processor/json_backend_jsoncons.cc` (max depth gesetzt), `src/request_body_processor/json_backend_simdjson.cc` (`(void) options;`).

## 4. Vergleich der Backends im Repo
- **Gemeinsamkeiten (belegt)**:
  - Beide emittieren Start/Ende für Objekt/Array, sowie Key/String/Number/Bool/Null über `JsonEventSink`.
  - Beide geben Fehler in `JsonParseResult` zurück und werden durch `JSONAdapter::normalizeResult` vereinheitlicht.
- **Unterschiede (belegt)**:
  1. **Depth-Option**: jsoncons nutzt technische Tiefe (`max_nesting_depth`), simdjson ignoriert sie.
  2. **Zahlpfad**: jsoncons rekonstruiert numerische Roh-Tokens über zusätzlichen Scanner; simdjson nutzt direkt `raw_json_token()`.
  3. **Fehlerklassifikation**: jsoncons mappt `max_nesting_depth_exceeded` auf `InternalError`, simdjson mappt `DEPTH_ERROR` auf `ParseError`.
- **Potenzielle Semantikabweichungen**:
  - Unterschiedliche `JsonParseStatus`-Klassifikation bei Tiefenüberschreitung.
  - Unterschiedliches Verhalten bei exotischen numerischen Tokens möglich (jsoncons-Fallbackpfade vs. simdjson-Roh-Token), **nicht vollständig verifiziert**.
- **Paritätsstatus**:
  - Es gibt ein Matrix-Skript, das Regressionsergebnisse beider Backends vergleicht.
  - Abdeckung laut Skript: zwei konkrete Regression-Dateien.
  - **Backend-Parität nicht vollständig verifizierbar**.
- **Evidenz**:
  - `src/request_body_processor/json_backend_simdjson.cc`, `src/request_body_processor/json_backend_jsoncons.cc`, `src/request_body_processor/json_adapter.cc`, `test/run-json-backend-matrix.sh`.

## 5. Performance-Optimierungen im Repo
1) **Stelle**: `parseDocumentWithSimdjson`
- **Aktuelles Verhalten**: neuer Parser + neue `padded_string`-Kopie pro Aufruf.
- **Warum teuer**: wiederholte Setup-/Allokationskosten.
- **Konkrete Verbesserung**: Parser-/Puffer-Reuse über Aufrufe prüfen (z. B. langlebige Parser-Instanz pro Worker).
- **Bezug Doku**: simdjson empfiehlt Parser-Reuse.
- **Evidenzgrad**: **Stark belegt** (strukturell im Code sichtbar).
- **Gemessen?**: **nicht gemessen**.

2) **Stelle**: `parseDocumentWithJsoncons` + `RawJsonTokenCursor`
- **Aktuelles Verhalten**: Cursor-Parsing plus zusätzlicher Token-Scanner über denselben Input.
- **Warum teuer**: doppelte Token-Arbeit im Hot Path.
- **Konkrete Verbesserung**: prüfen, ob Rohzahlrepräsentation ohne separaten Scanner aus Event+Context robust gewonnen werden kann.
- **Bezug Doku**: jsoncons-Cursor ist Pull-Event-API; zusätzlicher Scanner ist repo-eigene Schicht.
- **Evidenzgrad**: **Stark belegt** (Doppelarbeit sichtbar).
- **Gemessen?**: **nicht gemessen**.

3) **Stelle**: `JSON::processChunk` + Backend-Parse
- **Aktuelles Verhalten**: vollständige Body-Akkumulation in `m_data` vor Parsing.
- **Warum teuer**: keine echte Streaming-Verarbeitung bis zum Parse-Zeitpunkt.
- **Konkrete Verbesserung**: nur mit größerem Refactoring (inkrementelle Event-Verarbeitung) möglich.
- **Evidenzgrad**: **Stark belegt**.
- **Gemessen?**: **nicht gemessen**.

## 6. Speicher- und Speicherverwaltungs-Optimierungen im Repo
1) **Stelle**: `JSON::processChunk`
- **Aktuelles Verhalten**: gesamte Nutzlast wird in `std::string m_data` gehalten.
- **Warum speicherrelevant**: Peak-Memory wächst mit gesamter Bodygröße.
- **Konkrete Verbesserung**: inkrementelles Parsing statt Vollpufferung (größerer Architekturwechsel).
- **Evidenzgrad**: **Stark belegt**.
- **Gemessen?**: **nicht gemessen**.

2) **Stelle**: simdjson-Backend (`padded_string padded(input)`)
- **Aktuelles Verhalten**: zusätzliche Besitzkopie des kompletten Inputs.
- **Warum speicherrelevant**: doppelter Input-Footprint (mindestens temporär).
- **Konkrete Verbesserung**: wiederverwendbare gepaddete Buffer-Strategie prüfen.
- **Evidenzgrad**: **Stark belegt**.
- **Gemessen?**: **nicht gemessen**.

3) **Stelle**: jsoncons Zahlpfad (`rawNumberFromContext`)
- **Aktuelles Verhalten**: `std::string`-Materialisierung numerischer Tokens.
- **Warum speicherrelevant**: temporäre Allokationen je numerischem Event.
- **Konkrete Verbesserung**: `std::string_view`-Pfad bevorzugen, wo Lifetime sicher ist.
- **Evidenzgrad**: **Plausible Hypothese** (Allokation sichtbar, Nutzen nicht gemessen).
- **Gemessen?**: **nicht gemessen**.

## 7. Test- und Evidenzlücken
- Keine dedizierten Unit-Tests im Repo gefunden, die beide Backends auf identische Event-Sequenzen für dieselben Inputs prüfen.
- Matrixvergleich deckt laut Skript nur zwei Regression-Dateien ab; keine explizite Randfallmatrix für alle JSON-Typ-/Fehlerfälle.
- Keine gemessenen Zahlen in diesem Auditlauf (Benchmark-Skripte vorhanden, aber nicht ausgeführt).
- Deshalb:
  - Leistungsgewinne sind **nicht gemessen**.
  - Vollständige Backend-Parität ist **nicht verifizierbar**.

Konkrete fehlende Tests (beide Backends, gleicher Expected-Event-Trace):
- leeres Objekt `{}`
- leeres Array `[]`
- tiefe Verschachtelung knapp unter/über Grenzwert
- großes Integer (`> uint64`) / `bigint`
- Float mit Exponent / hoher Präzision (`bigdec`-Pfad)
- negative Zahl / `-0`
- ungültiges JSON (Syntaxfehler)
- abgeschnittenes JSON
- invalides UTF-8
- Escape-Randfälle in Strings
- große Payloads
- viele kleine Dokumente hintereinander

## 8. Wichtigste Maßnahmen
1. **Parität herstellen:** `JsonBackendParseOptions` konsistent anwenden (simdjson nutzt `technical_max_depth` derzeit nicht).
2. **Fehlerklassifikation angleichen:** konsistente Mapping-Policy für Tiefen-/Syntaxfehler zwischen Backends definieren und testen.
3. **Doppel-Tokenisierung prüfen:** jsoncons-Backend auf Vermeidung des zusätzlichen `RawJsonTokenCursor` evaluieren.
4. **Memory/Perf verbessern:** Parser-/Buffer-Reuse im simdjson-Pfad prüfen; Vollpufferungsstrategie (`m_data`) gegen inkrementelle Verarbeitung abwägen.
5. **Testabdeckung erhöhen:** dedizierte backend-paritätische Event-Trace-Tests + numerische Edgecases + UTF-8/Truncation-Fehlerfälle ergänzen.
