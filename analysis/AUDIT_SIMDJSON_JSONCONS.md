# Audit gegen die Dokumentation

## 1. Scope
- Geprüfter Stand: aktueller Arbeitsbaum in `/workspace/ModSecurity` (Commit-Hash siehe `git rev-parse HEAD`).
- Geprüfte Dokuquellen (im Repository vorhanden):
  - `analysis/simdjson/basics.md`
  - `analysis/simdjson/dom.md`
  - `analysis/simdjson/ondemand_design.md`
  - `analysis/simdjson/parse_many.md`
  - `analysis/simdjson/performance.md`
  - `analysis/jsoncons/Reference.md`
  - `analysis/jsoncons/Examples.md`
  - `analysis/jsoncons/build.md`
  - `analysis/jsoncons/Tutorials/Basics.md`
- Einschränkung: Die von diesen Analyse-Dateien referenzierten Original-Repositories unter `others/simdjson` und `others/jsoncons` sind im geprüften Stand nicht mit Quellcode/Docs befüllt. Deshalb ist ein direkter Doku-gegen-Upstream-Implementierung-Abgleich hier **nicht verifizierbar**.

## 2. Dokumentierte Kernbehauptungen

### simdjson (aus den vorhandenen Analyse-Dokumenten)
- Doku-Claims verweisen auf On-Demand/DOM-APIs und Header (`simdjson.h`, `ondemand`, `dom` etc.).
- `ondemand_design.md` markiert wesentliche Designaussagen selbst als nur teilweise belegbar.
- `parse_many.md` nennt Batch-/Streaming-Aspekte (`document_stream`, `batch_size`) als leistungsrelevant.
- `performance.md` behauptet im Analysefazit keinen klaren Widerspruch.

### jsoncons (aus den vorhandenen Analyse-Dokumenten)
- Doku-Claims verweisen auf StAJ/Streaming-API (`json_cursor`) sowie `json_options`/Extensions.
- Analyse-Dokumente behaupten überwiegend „korrekt“, aber ohne im aktuellen Tree direkt vorhandene jsoncons-Quellen.

## 3. Prüfung: simdjson gegen Doku

### 3.1 Bestätigte Punkte
1) **Doku-Aussage**: simdjson-Integration nutzt den On-Demand-Parser.
- **Fundstelle Doku**: `analysis/simdjson/basics.md` (Parser/ondemand-Bezug), `analysis/simdjson/ondemand_design.md`.
- **Befund im Code**: `parseDocumentWithSimdjson` instanziiert `simdjson::ondemand::parser` und verarbeitet `simdjson::ondemand::document`.
- **Evidenz**: `src/request_body_processor/json_backend_simdjson.cc`, Symbole `parseDocumentWithSimdjson`, `JsonBackendWalker::walk`.
- **Bewertung**: **Bestätigt**.

2) **Doku-Aussage**: JSON wird event-artig traversiert (Objekt/Array/Skalare).
- **Fundstelle Doku**: `analysis/simdjson/ondemand_design.md` (Design-/Event-Parsers-Bezug).
- **Befund im Code**: Dispatch über `json_type` in `walkValue`, rekursive Verarbeitung von Objekt-/Array-Events plus Sink-Callbacks.
- **Evidenz**: `src/request_body_processor/json_backend_simdjson.cc`, Symbole `walkValue`, `walkObject`, `walkArray`, `walkString`, `walkNumber`, `walkBoolean`.
- **Bewertung**: **Bestätigt**.

### 3.2 Widersprüche / Lücken
1) **Doku-Aussage**: Analyse-Dateien referenzieren Upstream-Dateien unter `others/simdjson/...`.
- **Problem im Code**: Der aktuelle Arbeitsbaum enthält diese Upstream-Dateien nicht; damit ist ein direkter Doku-vs-Upstream-Code-Abgleich nicht möglich.
- **Evidenz**: Build-Datei referenziert `../others/simdjson/singleheader/simdjson.cpp`, aber Audit-Umfang enthält nur ModSecurity-Quellen.
- **Risiko**: Korrektheitsaussagen über die eigentliche simdjson-Bibliothek wären spekulativ.
- **Bewertung**: **Nicht verifizierbar**.

2) **Doku-Aussage**: Performance-/Designclaims aus simdjson-Docs.
- **Problem im Code**: Nur Adapter-/Integrationscode sichtbar; keine verifizierbaren Upstream-Implementierungsdetails (SIMD-Kerne, parser internals).
- **Evidenz**: Nur Integration über `#include "simdjson.h"` sichtbar.
- **Risiko**: „In der Doku behauptet, im Code nicht eindeutig nachweisbar“.
- **Bewertung**: **Teilweise bestätigt** (nur Adapterebene).

### 3.3 Performance-Optimierungen
1) **Stelle im Code**: `parseDocumentWithSimdjson`.
- **Bezug zur Doku**: `analysis/simdjson/parse_many.md` betont Parser-/Batch-Wiederverwendung als relevant.
- **Aktuelles Verhalten**: Pro Parse-Aufruf neue `simdjson::ondemand::parser`-Instanz und `simdjson::padded_string`-Kopie.
- **Warum potenziell teuer**: Setup + Kopie pro Request, auch bei vielen kleinen Bodies.
- **Konkrete Verbesserung**: Parser-Lebensdauer auf Objekt-/Thread-Ebene wiederverwenden (nur wenn API/Lifetime im Projektkontext sicher bleibt).
- **Evidenzgrad**: **Plausible Optimierung, aber nicht gemessen**.

2) **Stelle im Code**: `walkNumber`.
- **Bezug zur Doku**: On-demand/low-overhead-Designgedanke.
- **Aktuelles Verhalten**: `value.raw_json_token()` wird direkt als `std::string_view` an Sink weitergegeben.
- **Bewertung**: Bereits allokationsarm auf Adapterebene; **keine weitere belegte Optimierung**.
- **Evidenzgrad**: **Bestätigt (kein zusätzlicher Hotspot aus Code ableitbar)**.

### 3.4 Speicher-Optimierungen
1) **Stelle im Code**: `simdjson::padded_string padded(input);`
- **Bezug zur Doku**: performance-orientierte Verarbeitung.
- **Aktuelles Verhalten**: explizite Besitzkopie des gesamten Inputs.
- **Warum speicherineffizient**: zusätzlicher Buffer proportional zur Bodygröße.
- **Konkrete Verbesserung**: Nur falls sicher möglich: auf wiederverwendbaren/extern gepaddeten Buffer umstellen.
- **Evidenzgrad**: **Plausible Optimierung, aber nicht gemessen**.

## 4. Prüfung: jsoncons gegen Doku

### 4.1 Bestätigte Punkte
1) **Doku-Aussage**: Streaming/StAJ-API (Cursor-Ansatz) ist zentral.
- **Fundstelle Doku**: `analysis/jsoncons/Reference.md` (Streaming API for JSON / StAJ).
- **Befund im Code**: `jsoncons::json_string_cursor` wird erstellt und per `current()/next()/done()` iteriert.
- **Evidenz**: `src/request_body_processor/json_backend_jsoncons.cc`, Symbol `parseDocumentWithJsoncons`.
- **Bewertung**: **Bestätigt**.

2) **Doku-Aussage**: Optionen sind relevant (`json_options`).
- **Fundstelle Doku**: `analysis/jsoncons/Examples.md`, `analysis/jsoncons/build.md`.
- **Befund im Code**: `max_nesting_depth`, `lossless_number(true)`, `lossless_bignum(true)` gesetzt.
- **Evidenz**: `src/request_body_processor/json_backend_jsoncons.cc`, Symbol `parseDocumentWithJsoncons`.
- **Bewertung**: **Bestätigt**.

### 4.2 Widersprüche / Lücken
1) **Doku-Aussage**: Referenzen auf Upstream-Pfade `others/jsoncons/...`.
- **Problem im Code**: Upstream-Quellen im aktuellen Stand nicht vorhanden; direkter Abgleich gegen Original-Implementierung daher nicht möglich.
- **Evidenz**: Build nutzt Include-Pfad `others/jsoncons/include`, aber Inhalte sind im Auditkontext nicht vorhanden.
- **Risiko**: Aussagen über jsoncons-internes Verhalten wären spekulativ.
- **Bewertung**: **Nicht verifizierbar**.

2) **Doku-Aussage**: Allgemeine „korrekt“-Fazits in Analyse-Dateien.
- **Problem im Code**: Diese Fazits lassen sich im aktuellen Tree nicht gegen Upstream-Code reproduzieren.
- **Evidenz**: Analyse-Dokumente referenzieren Dateien, die lokal nicht prüfbar sind.
- **Bewertung**: **In der Doku behauptet, im Code nicht eindeutig nachweisbar**.

### 4.3 Performance-Optimierungen
1) **Stelle im Code**: `emitEvent` + `RawJsonTokenCursor`.
- **Bezug zur Doku**: StAJ-Streaming sollte sequentiell/inkrementell sein.
- **Aktuelles Verhalten**: Zusätzlich zur Cursor-Iteration wird das Original-JSON per `RawJsonTokenCursor` parallel gescannt, um rohe Tokens zu rekonstruieren.
- **Warum potenziell teuer**: Doppelte Token-Arbeit (Parser + eigener Scanner) im Hot Path.
- **Konkrete Verbesserung**: Prüfen, ob numerische Rohdarstellung ausschließlich aus `ser_context`/Event ohne zweiten Scanner ableitbar ist.
- **Evidenzgrad**: **Stark belegte Optimierung** (strukturelle Doppelarbeit klar im Code sichtbar), **Leistungsgewinn nicht gemessen**.

2) **Stelle im Code**: `rawNumberFromContext`.
- **Bezug zur Doku**: lossless number/bignum aktiviert.
- **Aktuelles Verhalten**: Mehrere Fallback-Pfade, dabei `std::string`-Materialisierung.
- **Warum potenziell teuer**: wiederholte Kopien für numerische Tokens.
- **Konkrete Verbesserung**: Falls API-seitig möglich: `string_view` bis zum Sink durchreichen.
- **Evidenzgrad**: **Plausible Optimierung, aber nicht gemessen**.

### 4.4 Speicher-Optimierungen
1) **Stelle im Code**: `rawNumberFromContext` und numerische Event-Zweige.
- **Bezug zur Doku**: lossless number impliziert exakte Repräsentation.
- **Aktuelles Verhalten**: Rückgabe als `std::string`, danach Übergabe an Sink.
- **Warum speicherineffizient**: temporäre String-Allokationen pro Zahl möglich.
- **Konkrete Verbesserung**: Wo möglich `std::string_view` statt `std::string` nutzen.
- **Evidenzgrad**: **Plausible Optimierung, aber nicht gemessen**.

## 5. Vergleich beider Bibliotheken nur auf Basis der Evidenz
- **Belastbar**:
  - simdjson-Backend nutzt on-demand Value/Document-Walker.
  - jsoncons-Backend nutzt StAJ-Cursor und zusätzliche Roh-Token-Synchronisierung.
- **Nicht belastbar**:
  - Direkter Performancevergleich der Bibliotheken selbst (Upstream intern nicht im Tree).
  - Aussagen über SIMD-Implementierungsdetails oder jsoncons-interne Datenstrukturen.

## 6. Fehlende Evidenz
- Upstream-Repositories (`others/simdjson`, `others/jsoncons`) im geprüften Stand nicht vorhanden.
- Keine dedizierten Unit-Tests gefunden, die `parseDocumentWithSimdjson` und `parseDocumentWithJsoncons` direkt gegeneinander auf identische Semantik prüfen.
- Benchmark-Skripte/Binaries sind vorhanden, aber in diesem Audit nicht ausgeführt.
- Daher dürfen nicht gemacht werden:
  - „Bibliothek X ist schneller als Y“
  - „Doku-Garantie Z ist vollständig implementiert“ (ohne Upstream-Codezugriff)

## 7. Wichtigste Maßnahmen (priorisiert)
1) **Korrektheit/Auditierbarkeit**: Upstream-Quellen der beiden Bibliotheken in den Audit-Arbeitsstand aufnehmen (oder Commit-Pins + Submodule sichern), sonst bleibt der geforderte Doku-Abgleich **nicht verifizierbar**.
2) **Performance (jsoncons-Backend)**: Doppeltes Tokenisieren (Cursor + RawJsonTokenCursor) minimieren; zuerst mit bestehendem Benchmark-Setup messen.
3) **Performance/Speicher (simdjson-Backend)**: Parser- und Buffer-Lebensdauer auf Wiederverwendung prüfen; erwarteter Gewinn derzeit nur strukturelle Hypothese.
4) **Testbarkeit**: Direkte Backend-Paritätstests (identische Eingaben, identische Sink-Events/Fehlerklassifikation) ergänzen.
