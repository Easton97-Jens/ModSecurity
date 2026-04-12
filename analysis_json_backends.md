# Bewertung simdjson

## 1. Fundstellen
- `src/request_body_processor/json_backend_simdjson.cc` – `parseDocumentWithSimdjson`, `JsonBackendWalker::*` – kompletter JSON-Parser-Backend-Pfad für Request-Body Events.
- `src/request_body_processor/json_adapter.cc` – `JSONAdapter::parse` – Backend-Auswahl und Aufruf.
- `src/Makefile.am`, `configure.ac`, `build/win32/CMakeLists.txt` – Build-Auswahl und Einbindung von simdjson.

## 2. Ist die Implementierung korrekt?
- Parser-Nutzung mit `ondemand::parser` + `iterate(padded_string)`:
  - Bewertung: KORREKT
  - Kurzbegründung: Nutzung entspricht simdjson-Pattern mit gepaddetem Input und On-Demand-Parser.
  - Evidenz: `src/request_body_processor/json_backend_simdjson.cc` `parseDocumentWithSimdjson`; `others/simdjson/doc/basics.md`.
- Thread-lokaler Parser-Reuse:
  - Bewertung: KORREKT
  - Kurzbegründung: simdjson beschreibt thread-lokalen Parser, solange pro Thread nur ein Dokument gleichzeitig verarbeitet wird.
  - Evidenz: `src/request_body_processor/json_backend_simdjson.cc` `getReusableSimdjsonParser`; `others/simdjson/doc/basics.md`.
- Fehlerabbildung (UTF-8 / Parse / Internal):
  - Bewertung: KORREKT
  - Kurzbegründung: Fehlercodes werden differenziert auf interne Status gemappt.
  - Evidenz: `src/request_body_processor/json_backend_simdjson.cc` `fromSimdjsonError`.

## 3. Performance
- Stelle: `src/request_body_processor/json_backend_simdjson.cc:447-453`
- Problem: Für jeden Parse wird `simdjson::padded_string` aus `std::string` erzeugt (Kopie + zusätzliche Allocation).
- Evidenz: Explizite Kopie im Code; simdjson-Doku beschreibt Copy-Vermeidung nur mit garantiert gepaddetem Buffer.
- Bewertung: PROBLEM

## 4. Speicher / Speicherverwaltung
- Stelle: `src/request_body_processor/json_backend_simdjson.cc:101-115`
- Problem: `thread_local` Parser bleibt über Thread-Lebenszeit erhalten; nach großem Input kann reservierte Kapazität hoch bleiben.
- Evidenz: Persistenter `thread_local std::unique_ptr<...>`; simdjson-Doku sagt, On-Demand-Kapazität schrumpft in Long-Running-Prozessen nicht automatisch.
- Bewertung: PROBLEM

## 5. Einfaches Fazit simdjson
Die simdjson-Implementierung ist insgesamt korrekt. Das wichtigste Problem ist die erzwungene `padded_string`-Kopie pro Parse. Das kostet CPU-Zeit und Speicherbandbreite bei großen Request-Bodies. Die wichtigste Optimierung ist ein sicherer Zero-Copy-/padded-buffer-Pfad mit garantierter Padding- und Lifetime-Garantie. Der Parser-Reuse pro Thread ist korrekt, hält aber Speicherkapazität langfristig.

# Bewertung jsoncons

## 1. Fundstellen
- `src/request_body_processor/json_backend_jsoncons.cc` – `parseDocumentWithJsoncons`, `emitEvent`, `RawJsonTokenCursor` – kompletter JSON-Parser-Backend-Pfad für Request-Body Events.
- `src/request_body_processor/json_adapter.cc` – `JSONAdapter::parse` – Backend-Auswahl und Aufruf.
- `src/Makefile.am`, `configure.ac`, `build/win32/CMakeLists.txt` – Build-Auswahl und Einbindung von jsoncons.
- `test/common/json.h` – Test-JSON-Typ basiert auf jsoncons.

## 2. Ist die Implementierung korrekt?
- Cursor-Nutzung (`json_string_cursor`, Event-Loop mit `current()/next()/done()`):
  - Bewertung: KORREKT
  - Kurzbegründung: Entspricht jsoncons-Pull-Parsing-Muster.
  - Evidenz: `src/request_body_processor/json_backend_jsoncons.cc` `parseDocumentWithJsoncons`; `others/jsoncons/README.md`.
- Optionen (`max_nesting_depth`, `lossless_number`, `lossless_bignum`):
  - Bewertung: KORREKT
  - Kurzbegründung: Setter sind dokumentiert; Einstellungen passen zum Ziel, Zahlen verlustfrei als Lexem zu erhalten.
  - Evidenz: `src/request_body_processor/json_backend_jsoncons.cc`; `others/jsoncons/doc/ref/corelib/basic_json_options.md`.
- Positionsbasierte Zahlentoken-Rekonstruktion (`begin_position/end_position` + Fallback-Scanner):
  - Bewertung: KORREKT
  - Kurzbegründung: Nutzt dokumentierte Kontextpositionen; bei Abweichung gibt es abgesicherte Fallbacks.
  - Evidenz: `src/request_body_processor/json_backend_jsoncons.cc` `rawNumberFromContext`, `RawJsonTokenCursor`; `others/jsoncons/doc/ref/corelib/ser_context.md`.

## 3. Performance
- Stelle: `src/request_body_processor/json_backend_jsoncons.cc:688-695` und `242-539`
- Problem: Bei numerischen Events wird zusätzlich zum Cursor ein eigener Raw-Token-Scan über den Input ausgeführt.
- Evidenz: `consumeNextNumberToken`/`skipToNextNumberToken` laufen zusätzlich zur Event-Verarbeitung.
- Bewertung: PROBLEM

## 4. Speicher / Speicherverwaltung
- Stelle: Backend gesamt
- Problem: Kein konkretes Speicherverwaltungsproblem im sichtbaren Code belegt.
- Evidenz: Keine manuellen `new/delete`, kein persistenter globaler Parser-State, Referenz-/View-Nutzung lokal im Parse-Aufruf.
- Bewertung: KORREKT

## 5. Einfaches Fazit jsoncons
Die jsoncons-Implementierung ist insgesamt korrekt. Das wichtigste Problem ist der zusätzliche Token-Synchronisations-Scan für Zahlen. Das erhöht die CPU-Arbeit bei Zahl-lastigen JSON-Daten. Die wichtigste Optimierung ist, die Zahlentoken-Rekonstruktion direkt aus Cursor-Kontext ohne zusätzlichen Scan zu nutzen, wenn die Positionsdaten stabil sind. Speicherverwaltung ist im sichtbaren Code sauber.

# Vergleich am Ende
- Sicher belegt: simdjson hat pro Parse eine explizite Padded-Kopie; jsoncons hat stattdessen zusätzlichen Zahlentoken-Synchronisations-Scan.
- NICHT BEWIESEN: Dass ein Backend in allen realen Workloads immer schneller ist als das andere.
