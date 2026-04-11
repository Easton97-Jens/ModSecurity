# Repo-Audit

## 1. Scope
- Geprüftes Repo: `ModSecurity` (aktueller Branch-Stand).
- Geprüfte Bereiche:
  - Build-/Dependency-Integration: `configure.ac`, `src/Makefile.am`, `test/Makefile.am`
  - Runtime-JSON-Parsing in Produktcode: `src/request_body_processor/json*.cc`, `src/request_body_processor/json_backend*.cc`, `src/request_body_processor/json_adapter.cc`
  - Test-Hilfscode mit jsoncons: `test/common/json.h`
- Wo simdjson/jsoncons in DIESEM Repo tatsächlich verwendet werden:
  - simdjson: produktiv nur in `src/request_body_processor/json_backend_simdjson.cc`
  - jsoncons: produktiv nur in `src/request_body_processor/json_backend_jsoncons.cc`; zusätzlich Test-Hilfscode in `test/common/json.h`

## 2. Fundstellen im Repo
### simdjson
- `configure.ac` / JSON-Backend-Auswahl (`--with-json-backend=simdjson|jsoncons`).
- `src/Makefile.am` / Build-Einbindung `json_backend_simdjson.cc` + `others/simdjson/singleheader/simdjson.cpp`.
- `src/request_body_processor/json_backend_simdjson.cc` / Symbol `parseDocumentWithSimdjson` + `JsonBackendWalker`.
- `src/request_body_processor/json_adapter.cc` / Backend-Dispatch via `MSC_JSON_BACKEND_SIMDJSON`.

### jsoncons
- `configure.ac` / JSON-Backend-Auswahl (`--with-json-backend=simdjson|jsoncons`).
- `src/Makefile.am` / Build-Einbindung `json_backend_jsoncons.cc` + Include-Pfad jsoncons.
- `src/request_body_processor/json_backend_jsoncons.cc` / Symbol `parseDocumentWithJsoncons` + `RawJsonTokenCursor` + `emitEvent`.
- `src/request_body_processor/json_adapter.cc` / Backend-Dispatch via `MSC_JSON_BACKEND_JSONCONS`.
- `test/common/json.h` / Test-Parsing via `jsoncons::ojson`.

## 3. Korrektheitsprüfung im Repo

- Stelle im Repo:
  - `src/request_body_processor/json_adapter.cc` (`JSONAdapter::parse`)
- Verwendete Bibliothek:
  - simdjson oder jsoncons (kompilierzeitabhängig)
- Was der Code macht:
  - Wählt genau ein Backend per Compile-Makro und normalisiert Backend-Resultate (`sink_status` -> `parse_status`).
- Relevante Doku-Regel:
  - N/A (Repo-Integrationslogik).
- Bewertung: Korrekt.
- Evidenz:
  - Backend-Dispatch und Result-Normalisierung sind explizit implementiert.

- Stelle im Repo:
  - `src/request_body_processor/json_backend_simdjson.cc` (`parseDocumentWithSimdjson`, `JsonBackendWalker`)
- Verwendete Bibliothek:
  - simdjson On-Demand
- Was der Code macht:
  - Parst Input mit `simdjson::ondemand::parser` und traversiert ereignisbasiert in eigenen Sink.
  - Nutzt `get(...)`-Fehlerpfade systematisch (`getResult`), mappt simdjson-Errors auf interne Status.
- Relevante Doku-Regel:
  - simdjson On-Demand: Ergebniszugriffe liefern Fehlercodes; Document/Parser-Lifetime relevant.
- Bewertung: Teilweise korrekt.
- Evidenz:
  - Korrekte Fehlerbehandlung vorhanden; Parser-Lifetime ist lokal konsistent.
  - Aber Parser wird pro Aufruf neu erzeugt (s. Performance-Abschnitt, doku-relevante Reuse-Empfehlung).

- Stelle im Repo:
  - `src/request_body_processor/json_backend_jsoncons.cc` (`parseDocumentWithJsoncons`)
- Verwendete Bibliothek:
  - jsoncons `json_string_cursor`
- Was der Code macht:
  - Cursor-basiertes STAJ-Lesen (`current/next/done/check_done`), Fehler via `std::error_code`, eventweises Mapping in internen Sink.
- Relevante Doku-Regel:
  - jsoncons `basic_json_cursor`: Pull-Parser mit `current()/next()/done()` und Fehlercode-Overloads.
- Bewertung: Korrekt.
- Evidenz:
  - Nutzung entspricht dokumentierter Pull-API.

- Stelle im Repo:
  - `src/request_body_processor/json_backend_jsoncons.cc` (`json_string_cursor cursor(input, ...)`)
- Verwendete Bibliothek:
  - jsoncons
- Was der Code macht:
  - Cursor wird mit `const std::string &input` erzeugt.
- Relevante Doku-Regel:
  - jsoncons-Doku: Cursor hält Pointer auf Source; Source muss länger leben als Cursor.
- Bewertung: Korrekt.
- Evidenz:
  - `input` lebt für gesamten Funktionsscope; Cursor wird innerhalb dieses Scopes vollständig konsumiert.

- Stelle im Repo:
  - `src/request_body_processor/json.cc` (`JSON::complete` + Sink-Callbacks)
- Verwendete Bibliothek:
  - indirekt beide Backends über `JSONAdapter`
- Was der Code macht:
  - Führt Parsing auf gesamtem gesammeltem Body aus, mappt Backend-Status in Fehlermeldungen, erzwingt Depth-Limit über Sink.
- Relevante Doku-Regel:
  - N/A (repoeigene Auswertungslogik).
- Bewertung: Korrekt.
- Evidenz:
  - Fehlerpfad/Depth-Limit-Propagation implementiert.

- Stelle im Repo:
  - `test/common/json.h` (`JsonDocument::parse`)
- Verwendete Bibliothek:
  - jsoncons DOM (`ojson::parse`)
- Was der Code macht:
  - Test-Hilfsparser auf Basis DOM.
- Relevante Doku-Regel:
  - jsoncons DOM/Parse-API.
- Bewertung: Korrekt (für Testcode).
- Evidenz:
  - Exception wird abgefangen; Fehlertext wird propagiert.

## 4. Performance-Optimierungen im Repo

- Stelle im Repo:
  - `src/request_body_processor/json_backend_simdjson.cc` (`parseDocumentWithSimdjson`)
- Aktuelles Verhalten:
  - Pro Aufruf neue Instanzen: `ondemand::parser parser; simdjson::padded_string padded(input);`.
- Warum konkret ineffizient:
  - simdjson-Doku empfiehlt Parser-Reuse zur Reduktion von Reallokationen/Initialisierungskosten.
- Bezug zur Bibliotheksdoku:
  - simdjson `doc/performance.md` („make a parser once and reuse it“).
- Konkrete Verbesserung:
  - Reuse eines Parser-Objekts im langlebigeren Kontext (z. B. pro `JSON`-Instanz/Transaktion), falls Threading/Lifetime sicher auflösbar.
- Erwarteter Effekt:
  - Weniger Allokationen/Init-Kosten bei mehreren JSON-Bodies.
- Evidenzgrad: Stark belegt (strukturell), aber ungemessen.

- Stelle im Repo:
  - `src/request_body_processor/json_backend_jsoncons.cc` (`RawJsonTokenCursor` + `emitEvent`)
- Aktuelles Verhalten:
  - Zusätzlich zum jsoncons-Cursor wird der Original-Input tokenweise erneut gescannt/synchronisiert, inkl. eigener Number/String-Lexik.
- Warum konkret ineffizient:
  - Doppelte Arbeit: jsoncons parse events + zusätzlicher Rohtext-Scan über denselben Input.
- Bezug zur Bibliotheksdoku:
  - jsoncons-Cursor liefert bereits parse-events; Repo ergänzt eine zweite Tokenisierung zur Rohzahlmaterialisierung.
- Konkrete Verbesserung:
  - Falls Fachanforderung es erlaubt, Rohzahl-Extraktion ohne Voll-Rescan lösen (z. B. konsolidierte Pfade mit `context`-Offsets, Reduktion redundanter Token-Validierung).
- Erwarteter Effekt:
  - Geringere CPU-Kosten im jsoncons-Backend.
- Evidenzgrad: Plausibel aber ungemessen.

## 5. Speicher- und Speicherverwaltungs-Optimierungen im Repo

- Stelle im Repo:
  - `src/request_body_processor/json.cc` (`processChunk`, `complete`)
- Aktuelles Verhalten:
  - Vollständige Request-Body-Materialisierung in `m_data` vor Parsing.
- Warum konkret speicherineffizient oder riskant:
  - Peak-Memory mindestens `m_data` + backend-interne Strukturen; kein Streaming-Parse pro Chunk.
- Ownership-/Lifetime-/Buffer-Bezug:
  - `m_data` besitzt gesamten Body bis `complete()`.
- Konkrete Verbesserung:
  - Streaming-Strategie nur wenn semantisch mit `addArgument`/Pfadbildung kompatibel; sonst keine sichere Änderung behauptbar.
- Erwarteter Effekt:
  - Potenziell geringerer Peak-Memory bei großen Bodies.
- Evidenzgrad: Plausibel aber ungemessen.

- Stelle im Repo:
  - `src/request_body_processor/json_backend_simdjson.cc` (`simdjson::padded_string padded(input)`)
- Aktuelles Verhalten:
  - Zusätzliche gepolsterte Kopie des gesamten Inputs.
- Warum konkret speicherineffizient oder riskant:
  - Zusätzlicher Voll-Buffer im Parsepfad.
- Ownership-/Lifetime-/Buffer-Bezug:
  - Lokale Kopie pro Parse-Aufruf.
- Konkrete Verbesserung:
  - Nicht pauschal ersetzbar ohne sichere Prüfung der Padding-/Mutabilitäts-Anforderungen im konkreten Callpath.
- Erwarteter Effekt:
  - Potenziell weniger temporärer Speicher.
- Evidenzgrad: Plausibel aber ungemessen / Nicht verifizierbar für sichere Umstellung.

## 6. Probleme oder Fehlverwendungen
- Nachweisbare Fehlverwendung von simdjson/jsoncons-APIs im Produktcode: **Keine Evidenz gefunden**.
- Nachweisbare unsichere Source-Lifetime bei jsoncons-Cursor: **Keine Evidenz gefunden**.
- Nachweisbare falsche Fehlerpfadbehandlung in den Backend-Adaptern: **Keine Evidenz gefunden**.
- Repospezifischer Effizienzpunkt: simdjson-Parser-Reuse wird im aktuellen Produktpfad nicht genutzt (doku-konträr als Performance-Empfehlung, kein Korrektheitsfehler).
- Repospezifischer Effizienzpunkt: jsoncons-Backend führt zusätzliche Rohtext-Synchronisierung durch (funktional nachvollziehbar, aber potenziell teuer).

## 7. Nicht gefundene oder nicht verifizierbare Punkte
- Vollständiger semantischer Gleichheitsbeweis zwischen simdjson- und jsoncons-Backend-Ausgabe für alle JSON-Eckfälle: Nicht verifizierbar (im Repo nicht mit dedizierten Cross-Backend-Compliance-Tests belegt).
- Quantifizierter Performance-Gewinn durch vorgeschlagene Optimierungen: Leistungsgewinn nicht belegt (keine Messung in diesem Audit durchgeführt).
- Quantifizierter Speichergewinn: Keine Evidenz gefunden.
- Aussagen wie „Backend X ist insgesamt schneller/besser“ dürfen ausdrücklich nicht gemacht werden.

## 8. Wichtigste Maßnahmen
1. **Hohe Priorität (Performance, produktiv):** Parser-Reuse-Strategie im simdjson-Backend für wiederholte Parses im selben Lebenszyklus prüfen und bei sicherer Lifetime einführen.
2. **Hohe Priorität (Performance, produktiv):** jsoncons-Backend auf Reduktion der doppelten Tokenisierung (`RawJsonTokenCursor`) prüfen, ohne Rohzahl-Semantik zu verlieren.
3. **Mittlere Priorität (Korrektheit/Regression):** Repository-interne Cross-Backend-Tests ergänzen, die dieselben Inputs gegen beide Backends vergleichen (inkl. Zahlen, UTF-8, Tiefenlimit, Trunkierung).
4. **Mittlere Priorität (Messbarkeit):** Reproduzierbare Benchmarks für JSON-Request-Body-Verarbeitung im ModSecurity-Repo ergänzen (gleiche Inputs, beide Backends, CPU + Peak-Memory).
