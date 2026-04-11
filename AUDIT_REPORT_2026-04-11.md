# Audit gegen die Dokumentation

## 1. Scope
- Geprüfter Super-Repo-Commit: `a26b6ea` (ModSecurity).
- Geprüfter Submodule-Commit simdjson: `fb83b114efcec4544eba8d45e3c7969ca756c086`.
- Geprüfter Submodule-Commit jsoncons: `128553c8d1b222c30819656d123590accb60689d`.
- Referenz-Doku simdjson:
  - `others/simdjson/doc/basics.md`
  - `others/simdjson/doc/dom.md`
  - `others/simdjson/doc/ondemand_design.md`
  - `others/simdjson/doc/parse_many.md`
  - `others/simdjson/doc/performance.md`
- Referenz-Doku jsoncons:
  - `others/jsoncons/doc/Reference.md`
  - `others/jsoncons/doc/Examples.md`
  - `others/jsoncons/doc/build.md`
  - `others/jsoncons/doc/ref/corelib/decode_json.md`
  - `others/jsoncons/doc/ref/corelib/basic_json_cursor.md`
  - weitere Strukturprüfung in `others/jsoncons/doc/ref/`, `others/jsoncons/doc/Tutorials/`, `others/jsoncons/doc/Pages/` (stichprobenhaft, ohne Vollabdeckung aller APIs).

## 2. Dokumentierte Kernbehauptungen

### simdjson (aus Doku extrahiert)
- On-Demand benötigt Lifetime-Regeln (Parser/Input/Document müssen leben; ein Parser nur ein offenes Document).
- `parse_many` arbeitet mit `document_stream`, Batch-Window (Sweet Spot um 1MB), und Dokumente müssen in Batch passen.
- Für Performance: Parser wiederverwenden; interne Buffers sollen über Parses hinweg gehalten werden; Allokationen primär in `iterate()`.
- `parse_many` ist auf Streams vieler kleiner JSON-Dokumente ausgerichtet.

### jsoncons (aus Doku extrahiert)
- Core-API ist im Namespace `jsoncons`.
- `decode_json` (werfend) und `try_decode_json` (nicht-werfend) sind zentrale Decode-APIs.
- `basic_json_cursor` ist Pull-Parser (`current()/next()/done()`), mit Ownership/Lifetime-Hinweis bzgl. Source.
- Laut Doku `basic_json_cursor` „noncopyable and nonmoveable“.

## 3. Prüfung: simdjson gegen Doku

### 3.1 Bestätigte Punkte

- Doku-Aussage:
  - `parse_many` nutzt `document_stream`; Batchgröße ist relevant; zu kleine Batchgröße führt zu Kapazitätsproblemen.
- Fundstelle in der Doku:
  - `doc/parse_many.md` Zeilen 75-83, 84-93.
- Befund im Code:
  - `DEFAULT_BATCH_SIZE = 1000000`; `parse_many`/`load_many` normalisieren zu `MINIMAL_BATCH_SIZE`; `document_stream` speichert `batch_size` und nutzt `ensure_capacity(batch_size)`.
- Evidenz:
  - `others/simdjson/singleheader/simdjson.h` (Symbole: `DEFAULT_BATCH_SIZE`, `parser::parse_many`, `parser::load_many`, `document_stream::document_stream`, `document_stream::start`).
- Bewertung: Bestätigt.

- Doku-Aussage:
  - Parser-Reuse reduziert Reallokationen; Buffers bleiben erhalten.
- Fundstelle in der Doku:
  - `doc/performance.md` Zeilen 45-48; `doc/ondemand_design.md` Zeilen 298-301.
- Befund im Code:
  - `iterate()` allokiert nur falls `capacity() < json.length() || !string_buf`; `ensure_capacity()` allokiert nur bei Unterkapazität.
- Evidenz:
  - `others/simdjson/singleheader/simdjson.h` (Symbole: `ondemand::parser::iterate`, `ondemand::parser::ensure_capacity`).
- Bewertung: Bestätigt.

- Doku-Aussage:
  - Temporäre rvalue-Strings für `parse_many` sind unsicher.
- Fundstelle in der Doku:
  - `doc/basics.md` Zeile 2408.
- Befund im Code:
  - rvalue-overloads für `parse_many(const std::string&&, ...)` und `parse_many(const padded_string&&, ...)` sind gelöscht (`= delete // unsafe`).
- Evidenz:
  - `others/simdjson/singleheader/simdjson.h` (Symbol: `dom::parser::parse_many` Overloads).
- Bewertung: Bestätigt.

### 3.2 Widersprüche / Lücken

- Doku-Aussage:
  - „A parser may have at most one document open at a time“.
- Problem im Code:
  - In den geprüften On-Demand-Pfaden wurde kein eindeutiger Laufzeit-Guard gefunden, der bei parallelem offenem Document deterministisch `PARSER_IN_USE` liefert; es existiert der Error-Code, aber im geprüften Pfad keine direkt sichtbare Auslösung.
- Evidenz:
  - Doku: `others/simdjson/doc/basics.md` Zeilen 309, 319.
  - Code: `others/simdjson/singleheader/simdjson.h` (Symbole: `error_code::PARSER_IN_USE`, `ondemand::parser::iterate`, `document::start`).
- Risiko:
  - Regel wirkt derzeit als Usage-Vorbedingung; Enforcement im geprüften Ausschnitt nicht eindeutig nachweisbar.
- Bewertung: In der Doku behauptet, im Code nicht eindeutig nachweisbar.

- Doku-Aussage:
  - Vollständige Abdeckung gegen `doc/dom.md` Semantik.
- Problem im Code:
  - Nicht vollständig durchgeführt (dieses Audit fokussiert On-Demand/parse_many/performance-nahe Aussagen; DOM komplett über alle APIs nicht voll verifiziert).
- Evidenz:
  - Umfang dieses Audits.
- Risiko:
  - Teilabdeckung.
- Bewertung: Nicht verifizierbar (für nicht geprüfte DOM-Teilflächen).

### 3.3 Performance-Optimierungen

- Stelle im Code:
  - simdjson: `ondemand::parser::iterate`, `ondemand::parser::ensure_capacity`.
- Bezug zur Doku:
  - Doku fordert Parser-Reuse, um Allokationen zu minimieren.
- aktuelles Verhalten:
  - Allokation nur bei Kapazitätsunterschreitung.
- warum potenziell teuer:
  - Kein direkter Widerspruch; Verhalten entspricht Doku.
- konkrete Verbesserung:
  - Keine belastbare zusätzliche Optimierung aus Doku+Code abgeleitet.
- Evidenzgrad:
  - Keine Evidenz gefunden (für weitere konkrete Verbesserung über dokumentiertes Verhalten hinaus).

### 3.4 Speicher-Optimierungen

- Stelle im Code:
  - simdjson: `document_stream` (Pointer auf Buffer), `parse_many` rvalue-Overloads gelöscht.
- Bezug zur Doku:
  - Doku betont geringe Zusatzallokationen und Lifetime-Regeln.
- aktuelles Verhalten:
  - Stream hält Zeiger auf Input; unsafe temporaries werden compile-time blockiert.
- warum speicherineffizient:
  - Kein speicherineffizienter Widerspruch aus geprüften Stellen ableitbar.
- konkrete Verbesserung:
  - Keine belastbare, neue Speicheroptimierung aus Doku+Code ableitbar.
- Evidenzgrad:
  - Keine Evidenz gefunden.

## 4. Prüfung: jsoncons gegen Doku

### 4.1 Bestätigte Punkte

- Doku-Aussage:
  - Core-Klassen/Funktionen im Namespace `jsoncons`.
- Fundstelle in der Doku:
  - `doc/Reference.md` Zeile 1.
- Befund im Code:
  - Header verwenden Namespace `jsoncons` konsistent (u.a. `decode_json.hpp`, `encode_json.hpp`, `json_cursor.hpp`).
- Evidenz:
  - `others/jsoncons/include/jsoncons/decode_json.hpp`, `.../encode_json.hpp`, `.../json_cursor.hpp`.
- Bewertung: Bestätigt.

- Doku-Aussage:
  - `decode_json` wirft bei Fehlern, `try_decode_json` ist nicht-werfend.
- Fundstelle in der Doku:
  - `doc/ref/corelib/decode_json.md` Zeilen 73-74, 104-106.
- Befund im Code:
  - `try_decode_json` liefert `read_result<T>` mit Fehlercodepfad; `decode_json` ruft `try_decode_json` und wirft `ser_error` bei Fehler.
- Evidenz:
  - `others/jsoncons/include/jsoncons/decode_json.hpp` (Symbole: `try_decode_json`, `decode_json`).
- Bewertung: Bestätigt.

- Doku-Aussage:
  - `basic_json_cursor` Pull-Parser mit `current()/next()/done()`.
- Fundstelle in der Doku:
  - `doc/ref/corelib/basic_json_cursor.md` Zeilen 12-14, 94-116.
- Befund im Code:
  - Methoden `current`, `next`, `done` vorhanden und implementiert.
- Evidenz:
  - `others/jsoncons/include/jsoncons/json_cursor.hpp` (Symbol: `class basic_json_cursor`).
- Bewertung: Bestätigt.

### 4.2 Widersprüche / Lücken

- Doku-Aussage:
  - `basic_json_cursor` ist „noncopyable and nonmoveable“.
- Problem im Code:
  - Copy ist gelöscht, Move ist explizit erlaubt (`basic_json_cursor(basic_json_cursor&&) = default; operator=(...&&) = default;`).
- Evidenz:
  - Doku: `others/jsoncons/doc/ref/corelib/basic_json_cursor.md` Zeile 21.
  - Code: `others/jsoncons/include/jsoncons/json_cursor.hpp` (Move-Konstruktor/Move-Assignment).
- Risiko:
  - Falsche API-Erwartung bei Nutzern.
- Bewertung: Widerspruch.

- Doku-Aussage:
  - Vollständige Korrektheit aller Referenzseiten unter `doc/ref/`.
- Problem im Code:
  - Nicht vollständig verifiziert (nur gezielte Kernseiten geprüft).
- Evidenz:
  - Audit-Umfang.
- Risiko:
  - Teilabdeckung.
- Bewertung: Nicht verifizierbar.

### 4.3 Performance-Optimierungen

- Stelle im Code:
  - jsoncons Decode/Cursor-Pfade (`decode_json.hpp`, `json_cursor.hpp`).
- Bezug zur Doku:
  - Doku beschreibt API-Semantik, aber keine belastbaren Micro-Performance-Ziele für diese konkreten Pfade.
- aktuelles Verhalten:
  - Kein direkter dokumentationswidriger Performance-Pfad im geprüften Ausschnitt.
- warum potenziell teuer:
  - Nicht belastbar ohne zusätzliche Benchmarks.
- konkrete Verbesserung:
  - Nur strukturelle Optimierungshypothese wäre möglich; hier nicht aufgenommen.
- Evidenzgrad:
  - Keine Evidenz gefunden.

### 4.4 Speicher-Optimierungen

- Stelle im Code:
  - jsoncons Cursor/Decode.
- Bezug zur Doku:
  - Doku macht für diese Stellen keine quantifizierten Speicher-Guarantees, die verletzt wären.
- aktuelles Verhalten:
  - Kein eindeutiger Dokumentationswiderspruch bei Speicherstrategie in den geprüften Pfaden.
- warum speicherineffizient:
  - Nicht belastbar belegt.
- konkrete Verbesserung:
  - Keine belastbare konkrete Maßnahme aus Doku+Code ableitbar.
- Evidenzgrad:
  - Keine Evidenz gefunden.

## 5. Vergleich beider Bibliotheken nur auf Basis der Evidenz
- simdjson ist laut Doku+Code klar auf Reuse/geringe Reallokation im Parse-Pfad ausgelegt (`iterate`/`ensure_capacity`, `parse_many`-Batchmodell).
- jsoncons-Doku in den geprüften Dateien ist API-zentriert (Decode/Cursor/Referenzstruktur), mit weniger expliziten Low-Level-Performance-Mechanik-Aussagen als in simdjsons `performance.md`/`ondemand_design.md`.
- Belastbar belegbar:
  - simdjson dokumentiert und implementiert konkrete Reuse-/Batch-Mechanismen.
  - jsoncons dokumentiert und implementiert Throwing/Non-Throwing Decode-API sowie Cursor-Pull-API.
- NICHT belastbar:
  - Direkter Geschwindigkeitsvergleich simdjson vs jsoncons in diesem Audit (nicht gemessen).
  - Quantitativer Speichervergleich (nicht gemessen).

## 6. Fehlende Evidenz
- Was konnte nicht verifiziert werden?
  - Vollständiger API- und Invariantenabgleich über *alle* simdjson/jsoncons-Dokuseiten: Nicht verifizierbar im Rahmen dieses fokussierten Audits.
  - Harte Laufzeit-Enforcement-Mechanik für „ein offenes Document pro Parser“ im geprüften simdjson-Pfad: In der Doku behauptet, im Code nicht eindeutig nachweisbar.
- Welche Benchmarks oder Tests fehlen?
  - Benchmark-Ergebnisse wurden nicht im Audit ausgeführt/ausgewertet.
  - Obwohl Benchmark-/Test-Verzeichnisse existieren, wurde kein reproduzierbarer Messlauf als Evidenz genutzt.
- Welche Aussagen dürfen nicht gemacht werden?
  - „Bibliothek X ist schneller als Y“: Leistungsgewinn nicht belegt.
  - „Speicherverbrauch ist geringer“: Leistungsgewinn nicht belegt / Keine Evidenz gefunden.

## 7. Wichtigste Maßnahmen
1. jsoncons-Doku korrigieren: `basic_json_cursor` nicht als „nonmoveable“ dokumentieren (Move ist im Code erlaubt).
2. simdjson-Doku präzisieren: bei „ein Dokument pro Parser“ klarer trennen zwischen Vorbedingung und (falls vorhanden) Laufzeit-Checks.
3. Für beide Projekte: dokumentationsnahe, versionierte Compliance-Tests ergänzen (API-Signaturen/Move-Copy-Eigenschaften/Lifetime-Regeln).
4. Für belastbare Performance-/Speicher-Aussagen: reproduzierbare Benchmark-Protokolle im Repo als referenzierbare Evidenz pflegen.
