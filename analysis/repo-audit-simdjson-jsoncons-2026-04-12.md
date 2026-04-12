# Fakten-Audit

## 1. Fundstellen
- `src/request_body_processor/json_backend_simdjson.cc` — Symbol: `parseDocumentWithSimdjson`, `JsonBackendWalker`, `fromSimdjsonError`. 
- `src/request_body_processor/json_backend_jsoncons.cc` — Symbol: `parseDocumentWithJsoncons`, `emitEvent`, `RawJsonTokenCursor`, `fromJsonconsError`.
- `src/request_body_processor/json_backend.h` — Symbol: `JsonEventSink`, `JsonParseStatus`, `JsonSinkStatus`.
- `src/request_body_processor/json_adapter.cc` — Symbol: `JSONAdapter::parse`, `normalizeResult`.
- `src/request_body_processor/json.cc` — Symbol: `JSON::on_string`, `JSON::on_number`, `JSON::complete`.
- `configure.ac` — Symbol: `AC_ARG_WITH([json-backend])`, `MSC_JSON_BACKEND_SIMDJSON`, `MSC_JSON_BACKEND_JSONCONS`.
- `src/Makefile.am` — Symbol: `JSON_BACKEND_SIMDJSON`, `JSON_BACKEND_JSONCONS` Build-Zweige.
- `test/run-json-backend-matrix.sh` — Symbol: `for backend in simdjson jsoncons`.
- Upstream-Doku/-Code im Repo:
  - `others/simdjson/include/simdjson/generic/ondemand/parser.h`
  - `others/simdjson/include/simdjson/generic/ondemand/value.h`
  - `others/jsoncons/include/jsoncons/json_options.hpp`
  - `others/jsoncons/include/jsoncons/json_cursor.hpp`
  - `others/jsoncons/doc/Examples.md`

## 2. Korrektheit

- Stelle: `src/request_body_processor/json_backend_simdjson.cc`, `parseDocumentWithSimdjson`.
- Befund: Vor `parser.iterate(...)` wird `simdjson::padded_string padded(input)` erstellt.
- Kategorie: BEWIESEN
- Evidenz: Code `simdjson::padded_string padded(input);` und danach `parser.iterate(padded)` in derselben Funktion; simdjson-Doku verlangt Padding (`SIMDJSON_PADDING`) für iterate-Eingabe.

- Stelle: `src/request_body_processor/json_backend_simdjson.cc`, `parseDocumentWithSimdjson`.
- Befund: `ondemand::document` wird lokal erstellt und vollständig innerhalb derselben Funktion traversiert (`walker.walk(&document)`), danach Rückkehr.
- Kategorie: BEWIESEN
- Evidenz: Lokale Variable `simdjson::ondemand::document document;`, direkter Aufruf `walker.walk(&document);`; simdjson-Doku fordert gültige Document/Parser-Lifetime während Zugriff.

- Stelle: `src/request_body_processor/json_backend_simdjson.cc`, `JsonBackendWalker::walkString`.
- Befund: Stringwerte werden über `get_string()` gelesen und an Sink als `std::string_view` weitergegeben.
- Kategorie: BEWIESEN
- Evidenz: `value.get_string()` -> `m_sink->on_string(decoded)`; simdjson-Doku: `get_string()` liefert UTF-8 String.

- Stelle: `src/request_body_processor/json_backend_jsoncons.cc`, `parseDocumentWithJsoncons`.
- Befund: Optionen `max_nesting_depth`, `lossless_number(true)`, `lossless_bignum(true)` werden gesetzt.
- Kategorie: BEWIESEN
- Evidenz: Direkte Option-Setter-Aufrufe im Code; jsoncons-Header enthält diese Setter.

- Stelle: `src/request_body_processor/json_backend_jsoncons.cc`, `parseDocumentWithJsoncons`.
- Befund: Event-Loop arbeitet mit `while (!cursor.done())`, `cursor.current()`, `cursor.next(error)`, abschließend `cursor.check_done(error)`.
- Kategorie: BEWIESEN
- Evidenz: Direkte Aufrufe im Code; jsoncons-Doku zeigt dieses Cursor-Muster.

- Stelle: `src/request_body_processor/json_backend_simdjson.cc` vs `src/request_body_processor/json_backend_jsoncons.cc`.
- Befund: Tiefenfehler werden unterschiedlich klassifiziert.
- Kategorie: INKONSISTENT
- Evidenz: simdjson mappt `DEPTH_ERROR` auf `JsonParseStatus::ParseError`; jsoncons mappt `max_nesting_depth_exceeded` auf `JsonParseStatus::InternalError`.

- Stelle: `src/request_body_processor/json_adapter.cc`, `normalizeResult`.
- Befund: `JsonSinkStatus::DepthLimitExceeded` wird in `JsonParseStatus::ParseError` transformiert.
- Kategorie: BEWIESEN
- Evidenz: `case JsonSinkStatus::DepthLimitExceeded: result.parse_status = JsonParseStatus::ParseError;`.

## 3. Performance (NUR reale Fälle)

- Stelle: `src/request_body_processor/json_backend_simdjson.cc`, `parseDocumentWithSimdjson`.
- Problem: Zusätzliche Vollkopie des gesamten Inputs in `simdjson::padded_string`.
- Kategorie: BEWIESEN
- Evidenz: `simdjson::padded_string padded(input);`.

- Stelle: `src/request_body_processor/json_backend_jsoncons.cc`, `parseDocumentWithJsoncons` + `RawJsonTokenCursor`.
- Problem: Numerische Tokens werden zusätzlich mit eigenem Cursor über den Originaltext gescannt, parallel zum Parser-Eventstrom.
- Kategorie: BEWIESEN
- Evidenz: `RawJsonTokenCursor token_cursor(input);`, Aufrufe `consumeNextNumberToken(...)`/`advanceExactNumber(...)` im Event-Handling.

- Stelle: `src/request_body_processor/json.cc`, `JSON::on_string` und `JSON::on_number`.
- Problem: Für jeden String-/Number-Event wird ein neuer `std::string` erzeugt.
- Kategorie: BEWIESEN
- Evidenz: `addArgument(std::string(value.data(), value.size()))` in beiden Funktionen.

## 4. Speicher (NUR reale Fälle)

- Stelle: `src/request_body_processor/json_backend_simdjson.cc`, `parseDocumentWithSimdjson`.
- Problem: Zusätzliches Buffer-Objekt `padded` enthält Input-Kopie.
- Kategorie: BEWIESEN
- Evidenz: `simdjson::padded_string padded(input);`.

- Stelle: `src/request_body_processor/json.cc`, `JSON::on_string` und `JSON::on_number`.
- Problem: Pro Event temporäre `std::string`-Allokation durch Materialisierung aus `std::string_view`.
- Kategorie: BEWIESEN
- Evidenz: `std::string(value.data(), value.size())`.

## 5. Backend-Unterschiede

- Unterschied: simdjson nutzt `raw_json_token()` direkt für Zahlen; jsoncons nutzt `RawJsonTokenCursor` + Kontextdaten zur Token-Rekonstruktion.
- Kategorie: BEWIESEN
- Evidenz: `walkNumber` im simdjson-Backend vs `rawNumberFromContext` + `consumeNextNumberToken` im jsoncons-Backend.

- Unterschied: Fehlerklasse bei Nesting-Limit (ParseError vs InternalError).
- Kategorie: INKONSISTENT
- Evidenz: `fromSimdjsonError` und `fromJsonconsError` Mapping.

- Unterschied: Vollständige semantische Gleichheit der Eventfolgen für alle Inputs.
- Kategorie: NICHT BEWIESEN
- Evidenz: keine vollständige, ausgeführte Matrix-/Paritätsmessung im Auditlauf.

## 6. Nicht beweisbare Punkte
- Vollständige Backend-Parität über alle JSON-Randfälle: NICHT BEWIESEN.
- Laufzeitgewinn einzelner Änderungen: NICHT BEWIESEN.
- Peak-Memory-Gewinn einzelner Änderungen: NICHT BEWIESEN.
- Vollständige Verifikation aller vorhandenen Regressionstests gegen beide Backends im Auditlauf: NICHT BEWIESEN.
