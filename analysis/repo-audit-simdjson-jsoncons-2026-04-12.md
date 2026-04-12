# Bewertung simdjson

## 1. Fundstellen
- Datei: `src/request_body_processor/json_backend_simdjson.cc`  
  Funktion: `parseDocumentWithSimdjson`  
  Zweck: Parse-Einstieg, Parser-Vorbereitung, `padded_string`, Traversal-Start.
- Datei: `src/request_body_processor/json_backend_simdjson.cc`  
  Funktion: `fromSimdjsonError`  
  Zweck: Mapping von simdjson-Fehlercodes auf `JsonParseStatus`.
- Datei: `src/request_body_processor/json_backend_simdjson.cc`  
  Funktion: `JsonBackendWalker::walk*`  
  Zweck: Umwandlung der JSON-Tokens in `JsonEventSink`-Events.
- Datei: `src/Makefile.am`  
  Funktion/Symbol: `JSON_BACKEND_SIMDJSON` Block  
  Zweck: Backend-spezifische Source/Include-Auswahl.
- Datei: `configure.ac`  
  Funktion/Symbol: `--with-json-backend=simdjson`, `MSC_JSON_BACKEND_SIMDJSON`  
  Zweck: Build-Zeit-Auswahl und Aktivierung des simdjson-Backends.

## 2. Ist die Implementierung korrekt?
- Stelle: `src/request_body_processor/json_backend_simdjson.cc`, `parseDocumentWithSimdjson`.
- Bewertung: KORREKT
- Kurzbegründung: Vor `iterate` wird `simdjson::padded_string` erstellt.
- Evidenz: Datei `src/request_body_processor/json_backend_simdjson.cc`, Funktion `parseDocumentWithSimdjson`, Zeile 447 und 458.

- Stelle: `src/request_body_processor/json_backend_simdjson.cc`, `parseDocumentWithSimdjson`.
- Bewertung: KORREKT
- Kurzbegründung: `ondemand::document` lebt bis zum Ende der kompletten Traversierung in derselben Funktion.
- Evidenz: Datei `src/request_body_processor/json_backend_simdjson.cc`, Funktion `parseDocumentWithSimdjson`, Zeile 454 und 476.

- Stelle: `src/request_body_processor/json_backend_simdjson.cc`, `JsonBackendWalker::walkString`.
- Bewertung: KORREKT
- Kurzbegründung: String wird mit `get_string()` gelesen und an Sink weitergegeben.
- Evidenz: Datei `src/request_body_processor/json_backend_simdjson.cc`, Funktion `walkString`, Zeile 374 und 379.

- Stelle: `src/request_body_processor/json_backend_simdjson.cc`, `JsonBackendWalker::walkNumber`.
- Bewertung: KORREKT
- Kurzbegründung: Zahl wird als Raw-Token an Sink weitergegeben.
- Evidenz: Datei `src/request_body_processor/json_backend_simdjson.cc`, Funktion `walkNumber`, Zeile 388 und 389.

- Stelle: `src/request_body_processor/json_backend_simdjson.cc`, `fromSimdjsonError`.
- Bewertung: KORREKT
- Kurzbegründung: Fehlercode-Mapping ist vollständig für die im Switch behandelten simdjson-Fälle.
- Evidenz: Datei `src/request_body_processor/json_backend_simdjson.cc`, Funktion `fromSimdjsonError`, Zeile 54 bis 90.

- Stelle: `others/simdjson/include/simdjson/generic/ondemand/parser.h` gegen Repo-Nutzung.
- Bewertung: KORREKT
- Kurzbegründung: Doku fordert Padding; Repo liefert Padding explizit über `padded_string`.
- Evidenz: Datei `others/simdjson/include/simdjson/generic/ondemand/parser.h`, Zeile 81 bis 87; Datei `src/request_body_processor/json_backend_simdjson.cc`, Zeile 447.

## 3. Performance
- Stelle: `src/request_body_processor/json_backend_simdjson.cc`, `parseDocumentWithSimdjson`.
- Problem: Vollständige Zusatzkopie des Inputs in `simdjson::padded_string`.
- Evidenz: Datei `src/request_body_processor/json_backend_simdjson.cc`, Funktion `parseDocumentWithSimdjson`, Zeile 447.
- Bewertung: PROBLEM

## 4. Speicher / Speicherverwaltung
- Stelle: `src/request_body_processor/json_backend_simdjson.cc`, `parseDocumentWithSimdjson`.
- Problem: Zusätzlicher Buffer `padded` hält Input-Kopie.
- Evidenz: Datei `src/request_body_processor/json_backend_simdjson.cc`, Funktion `parseDocumentWithSimdjson`, Zeile 447.
- Bewertung: PROBLEM

## 5. Einfaches Fazit simdjson
Die simdjson-Implementierung im Repo ist insgesamt KORREKT.  
Die Nutzung laut simdjson-Doku zum Thema Padding ist KORREKT.  
Das wichtigste Problem ist die zusätzliche Vollkopie pro Parse über `simdjson::padded_string`.  
Die wichtigste Optimierung ist ein Parse-Pfad ohne diese Zusatzkopie.

# Bewertung jsoncons

## 1. Fundstellen
- Datei: `src/request_body_processor/json_backend_jsoncons.cc`  
  Funktion: `parseDocumentWithJsoncons`  
  Zweck: Cursor-Initialisierung, Optionen, Event-Loop.
- Datei: `src/request_body_processor/json_backend_jsoncons.cc`  
  Funktion: `emitEvent`  
  Zweck: Mapping von STAJ-Events auf `JsonEventSink`-Events.
- Datei: `src/request_body_processor/json_backend_jsoncons.cc`  
  Funktion: `RawJsonTokenCursor::*`  
  Zweck: Roh-Token-Synchronisierung für Zahlen.
- Datei: `src/request_body_processor/json_backend_jsoncons.cc`  
  Funktion: `fromJsonconsError`  
  Zweck: Mapping von jsoncons-Fehlern auf `JsonParseStatus`.
- Datei: `src/Makefile.am`  
  Funktion/Symbol: `JSON_BACKEND_JSONCONS` Block  
  Zweck: Backend-spezifische Include-Auswahl.
- Datei: `configure.ac`  
  Funktion/Symbol: `--with-json-backend=jsoncons`, `MSC_JSON_BACKEND_JSONCONS`  
  Zweck: Build-Zeit-Auswahl und Aktivierung des jsoncons-Backends.

## 2. Ist die Implementierung korrekt?
- Stelle: `src/request_body_processor/json_backend_jsoncons.cc`, `parseDocumentWithJsoncons`.
- Bewertung: KORREKT
- Kurzbegründung: `max_nesting_depth`, `lossless_number`, `lossless_bignum` werden gesetzt.
- Evidenz: Datei `src/request_body_processor/json_backend_jsoncons.cc`, Funktion `parseDocumentWithJsoncons`, Zeile 721 bis 724.

- Stelle: `src/request_body_processor/json_backend_jsoncons.cc`, `parseDocumentWithJsoncons`.
- Bewertung: KORREKT
- Kurzbegründung: Event-Loop nutzt `done/current/next/check_done` vollständig.
- Evidenz: Datei `src/request_body_processor/json_backend_jsoncons.cc`, Funktion `parseDocumentWithJsoncons`, Zeile 756 bis 776.

- Stelle: `src/request_body_processor/json_backend_jsoncons.cc`, `emitEvent`.
- Bewertung: KORREKT
- Kurzbegründung: Objekt/Array/String/Bool/Null/Number werden jeweils auf passende Sink-Methoden abgebildet.
- Evidenz: Datei `src/request_body_processor/json_backend_jsoncons.cc`, Funktion `emitEvent`, Zeile 583 bis 704.

- Stelle: `others/jsoncons/include/jsoncons/json_options.hpp` gegen Repo-Nutzung.
- Bewertung: KORREKT
- Kurzbegründung: Die verwendeten Option-Setter existieren in der jsoncons-API.
- Evidenz: Datei `others/jsoncons/include/jsoncons/json_options.hpp`, Zeile 656 bis 665 und 717 bis 720; Datei `src/request_body_processor/json_backend_jsoncons.cc`, Zeile 722 bis 724.

- Stelle: `others/jsoncons/doc/Examples.md` gegen Repo-Nutzung.
- Bewertung: KORREKT
- Kurzbegründung: Das im Repo verwendete Cursor-Muster entspricht dem dokumentierten Event-Loop-Muster.
- Evidenz: Datei `others/jsoncons/doc/Examples.md`, Zeile 761 bis 779; Datei `src/request_body_processor/json_backend_jsoncons.cc`, Zeile 756 bis 767.

- Stelle: `src/request_body_processor/json_backend_jsoncons.cc`, `fromJsonconsError`.
- Bewertung: PROBLEM
- Kurzbegründung: `max_nesting_depth_exceeded` wird als `InternalError` klassifiziert.
- Evidenz: Datei `src/request_body_processor/json_backend_jsoncons.cc`, Funktion `fromJsonconsError`, Zeile 87 bis 90.

## 3. Performance
- Stelle: `src/request_body_processor/json_backend_jsoncons.cc`, `parseDocumentWithJsoncons` + `RawJsonTokenCursor`.
- Problem: Zusätzlicher Textscan für numerische Tokens parallel zum Parser-Eventstrom.
- Evidenz: Datei `src/request_body_processor/json_backend_jsoncons.cc`, Funktion `parseDocumentWithJsoncons`, Zeile 742; Funktion `emitEvent`, Zeile 686 und 692; Klasse `RawJsonTokenCursor`, Zeile 240 bis 251.
- Bewertung: PROBLEM

## 4. Speicher / Speicherverwaltung
- Stelle: `src/request_body_processor/json.cc`, `on_string` und `on_number`.
- Problem: Für jedes String-/Number-Event wird aus `string_view` ein neuer `std::string` erzeugt.
- Evidenz: Datei `src/request_body_processor/json.cc`, Funktion `on_string`, Zeile 248; Funktion `on_number`, Zeile 254.
- Bewertung: PROBLEM

## 5. Einfaches Fazit jsoncons
Die jsoncons-Implementierung im Repo ist insgesamt KORREKT.  
Die Nutzung der jsoncons-Optionen und des Cursor-Event-Loops ist laut Doku KORREKT.  
Das wichtigste Problem ist die Fehlerklassifikation `max_nesting_depth_exceeded` als `InternalError`.  
Die wichtigste Optimierung ist die Reduktion des zusätzlichen Zahlentoken-Scans.

# Vergleich am Ende
Der im Repo sicher belegte Unterschied ist die Fehlerklassifikation beim Nesting-Limit (`ParseError` in simdjson, `InternalError` in jsoncons).  
Die vollständige Gleichheit der Event-Semantik für alle Eingaben ist NICHT BEWIESEN.
