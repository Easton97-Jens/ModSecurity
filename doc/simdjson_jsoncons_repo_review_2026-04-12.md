# Bewertung simdjson

## 1. Fundstellen
- Datei: `configure.ac`
- Funktion: Backend-Auswahl und Submodule-Validierung
- Zweck: Aktiviert `simdjson` per `--with-json-backend` und prüft `others/simdjson`

- Datei: `src/Makefile.am`
- Funktion: Build-Integration
- Zweck: Baut `json_backend_simdjson.cc` und bindet `simdjson.cpp` + Include-Pfad ein

- Datei: `src/request_body_processor/json_adapter.cc`
- Funktion: `JSONAdapter::parse(...)`
- Zweck: Dispatch auf `parseDocumentWithSimdjson(...)`

- Datei: `src/request_body_processor/json_backend_simdjson.cc`
- Funktion: `parseDocumentWithSimdjson(...)`, `parsePreparedDocumentWithSimdjson(...)`, `JsonBackendWalker::*`
- Zweck: Parsing, Traversal, Fehler-Mapping, Zahlen-Lexeme, Tiefenprüfung

- Datei: `test/unit/json_backend_depth_tests.cc`
- Funktion: `expectMutableSimdjsonPathPreservesLogicalInput`, `expectConstSimdjsonPathLeavesInputUntouched`
- Zweck: Prüft mutable/const Input-Pfade und Padding-Verhalten

## 2. Ist die Implementierung korrekt?
- Stelle: Thread-local Parser-Reuse (`getReusableSimdjsonParser`)
- Bewertung: KORREKT
- Kurzbegründung: Ein Parser pro Thread und Reuse sind so implementiert; das entspricht der simdjson-Empfehlung.
- Evidenz: `src/request_body_processor/json_backend_simdjson.cc:getReusableSimdjsonParser` (120-134), `others/simdjson/doc/basics.md` (315-317, 2983-2984)

- Stelle: Mutable Input-Pfad mit `simdjson::pad(std::string&)`
- Bewertung: KORREKT
- Kurzbegründung: Der Code nutzt explizit In-Place-Padding für mutable Input.
- Evidenz: `src/request_body_processor/json_backend_simdjson.cc:prepareMutableSimdjsonInput` (475-483), `others/simdjson/doc/basics.md` (220-222)

- Stelle: Const Input-Pfad mit `padded_string_view::has_sufficient_padding()` und Fallback-Kopie
- Bewertung: KORREKT
- Kurzbegründung: Erst direkte Nutzung bei ausreichendem Padding, sonst sichere Kopie in `padded_string`.
- Evidenz: `src/request_body_processor/json_backend_simdjson.cc:prepareConstSimdjsonInput` (486-507), `others/simdjson/doc/basics.md` (211-221)

- Stelle: Zahlenweitergabe über `raw_json_token()`
- Bewertung: KORREKT
- Kurzbegründung: Rohlexem wird direkt übernommen und nur nachlaufender JSON-Whitespace entfernt.
- Evidenz: `src/request_body_processor/json_backend_simdjson.cc:walkNumber` (424-427), `src/request_body_processor/json_backend_simdjson.cc:trimTrailingJsonWhitespace` (98-107), `others/simdjson/doc/basics.md` (2762-2773)

- Stelle: Fehler-Mapping (`fromSimdjsonError`)
- Bewertung: NICHT BEWIESEN
- Kurzbegründung: Mapping ist sichtbar konsistent, vollständige Laufzeitvalidierung ist hier nicht ausgeführt.
- Evidenz: `src/request_body_processor/json_backend_simdjson.cc:fromSimdjsonError` (53-90)

## 3. Performance
- Stelle: Parser-Kapazität bei Reuse
- Problem: Große einmalige Inputs können hohe Kapazität pro Thread dauerhaft halten.
- Evidenz: `src/request_body_processor/json_backend_simdjson.cc:prepareParser` (161-168)
- Bewertung: PROBLEM

## 4. Speicher / Speicherverwaltung
- Stelle: Thread-local Parser-Lebensdauer
- Problem: Keine automatische Shrink/Freigabe-Heuristik, Retained Capacity bleibt bewusst bestehen.
- Evidenz: `src/request_body_processor/json_backend_simdjson.cc:getReusableSimdjsonParser` (116-119), `src/request_body_processor/json_backend_simdjson.cc:prepareParser` (161-164)
- Bewertung: PROBLEM

## 5. Einfaches Fazit simdjson
Die Implementierung ist insgesamt KORREKT. Die Nutzung von Parser-Reuse und Padding folgt der sichtbaren simdjson-Doku. Das wichtigste Problem ist die bewusst beibehaltene Speicherretention pro Thread nach großen Inputs. Die wichtigste Optimierung ist eine kontrollierte Recreate/Shrink-Strategie für Langläufer mit stark wechselnden JSON-Größen.

# Bewertung jsoncons

## 1. Fundstellen
- Datei: `configure.ac`
- Funktion: Backend-Auswahl und Submodule-Validierung
- Zweck: Aktiviert `jsoncons` per `--with-json-backend` und prüft `others/jsoncons`

- Datei: `src/Makefile.am`
- Funktion: Build-Integration
- Zweck: Baut `json_backend_jsoncons.cc` und setzt Include-Pfad

- Datei: `src/request_body_processor/json_adapter.cc`
- Funktion: `JSONAdapter::parse(...)`
- Zweck: Dispatch auf `parseDocumentWithJsoncons(...)`

- Datei: `src/request_body_processor/json_backend_jsoncons.cc`
- Funktion: `parseDocumentWithJsoncons(...)`, `emitEvent(...)`, `RawJsonTokenCursor::*`
- Zweck: Event-basiertes Parsing, Fehler-Mapping, exakte Zahlen-Lexeme via zusätzlichem Rohscan

- Datei: `test/common/json.h`
- Funktion: `using JsonNode = jsoncons::ojson`
- Zweck: Test-Hilfstypen auf jsoncons

## 2. Ist die Implementierung korrekt?
- Stelle: Optionen `max_nesting_depth`, `lossless_number(true)`, `lossless_bignum(true)`
- Bewertung: KORREKT
- Kurzbegründung: Optionen werden gesetzt wie in der jsoncons-Doku beschrieben.
- Evidenz: `src/request_body_processor/json_backend_jsoncons.cc:parseDocumentWithJsoncons` (723-726), `others/jsoncons/doc/ref/corelib/basic_json_options.md` (25-28)

- Stelle: Numerische Ereignisse mit exaktem Rohzahl-Lexem
- Bewertung: KORREKT
- Kurzbegründung: Zahlen gehen als Rohlexem an `on_number(...)`; dafür wird `RawJsonTokenCursor` synchronisiert.
- Evidenz: `src/request_body_processor/json_backend_jsoncons.cc:emitEvent` (577-705), `src/request_body_processor/json_backend_jsoncons.cc:RawJsonTokenCursor` (190-543)

- Stelle: Kontextpositionen + Fallback
- Bewertung: KORREKT
- Kurzbegründung: Der Code versucht erst `begin_position/end_position`, nutzt dann Scan-Fallback. Das ist mit sichtbarer Cursor-/Context-Implementierung konsistent.
- Evidenz: `src/request_body_processor/json_backend_jsoncons.cc:rawNumberFromContext` (545-560), `others/jsoncons/doc/ref/corelib/basic_json_cursor.md` (117-119), `others/jsoncons/include/jsoncons/ser_utils.hpp` (38-45), `others/jsoncons/include/jsoncons/json_cursor.hpp` (405-407, 450-457)

- Stelle: Fehler-Mapping (`fromJsonconsError`)
- Bewertung: NICHT BEWIESEN
- Kurzbegründung: Mapping ist sichtbar definiert, vollständige Laufzeitvalidierung ist hier nicht ausgeführt.
- Evidenz: `src/request_body_processor/json_backend_jsoncons.cc:fromJsonconsError` (77-101)

## 3. Performance
- Stelle: Zusätzlicher Zahlentoken-Scan
- Problem: Für numerische Events wird zusätzlich zum jsoncons-Eventpfad ein eigener Token-Scan ausgeführt.
- Evidenz: `src/request_body_processor/json_backend_jsoncons.cc:consumeNextNumberToken` (242-253), `src/request_body_processor/json_backend_jsoncons.cc:skipToNextNumberToken` (523-539), `src/request_body_processor/json_backend_jsoncons.cc:emitEvent` (639-647, 688-695)
- Bewertung: PROBLEM

## 4. Speicher / Speicherverwaltung
- Stelle: jsoncons-Backend Speicherverwaltung
- Problem: Kein konkreter Leak/UAF ist im sichtbaren Code belegt.
- Evidenz: `src/request_body_processor/json_backend_jsoncons.cc:parseDocumentWithJsoncons` (731-756)
- Bewertung: NICHT BEWIESEN

## 5. Einfaches Fazit jsoncons
Die Implementierung ist insgesamt KORREKT. Die gesetzten Optionen und die Event-Verarbeitung sind im sichtbaren Code stimmig zur Doku. Das wichtigste Problem ist der zusätzliche Zahlentoken-Scan mit klaren Zusatzkosten. Die wichtigste Optimierung ist die Reduktion dieses zweiten Scans, wenn belastbar verwertbare Positionsdaten im Cursorpfad verfügbar sind.

# Vergleich am Ende

- Sicher belegt: jsoncons hat im Repo einen zusätzlichen Rohzahl-Scanner; simdjson nutzt direkt `raw_json_token()`.
- Nicht bewiesen: Gleiche End-to-End-Performance beider Backends unter realer Last.
