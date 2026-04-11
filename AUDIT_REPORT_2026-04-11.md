# Repo-Audit

## 1. Scope
- Geprüftes Repo: `ModSecurity` (aktueller Commit auf diesem Branch).
- Geprüfte Integrationsstellen für simdjson/jsoncons:
  - Build/Selection: `configure.ac`, `src/Makefile.am`, `src/request_body_processor/json_adapter.cc`
  - Produktcode: `src/request_body_processor/json_backend_simdjson.cc`, `src/request_body_processor/json_backend_jsoncons.cc`, `src/request_body_processor/json.cc`, `src/request_body_processor/json.h`, `src/transaction.cc`
  - Testcode: `test/common/json.h`
- Externe Doku als Bewertungsmaßstab (nicht Hauptgegenstand):
  - simdjson: `others/simdjson/doc/performance.md`, `others/simdjson/doc/basics.md`
  - jsoncons: `others/jsoncons/doc/ref/corelib/basic_json_cursor.md`

## 2. Fundstellen im Repo
### simdjson
- `configure.ac`: Backend-Auswahl und Simdjson-Präsenzprüfung.
- `src/Makefile.am`: Kompiliert `json_backend_simdjson.cc` + `others/simdjson/singleheader/simdjson.cpp`.
- `src/request_body_processor/json_backend_simdjson.cc`:
  - `parseDocumentWithSimdjson`
  - `JsonBackendWalker`
- `src/request_body_processor/json_adapter.cc`: Dispatch auf simdjson via `MSC_JSON_BACKEND_SIMDJSON`.

### jsoncons
- `configure.ac`: Backend-Auswahl und jsoncons-Präsenzprüfung.
- `src/Makefile.am`: Kompiliert `json_backend_jsoncons.cc`.
- `src/request_body_processor/json_backend_jsoncons.cc`:
  - `parseDocumentWithJsoncons`
  - `RawJsonTokenCursor`
  - `emitEvent`
- `src/request_body_processor/json_adapter.cc`: Dispatch auf jsoncons via `MSC_JSON_BACKEND_JSONCONS`.
- `test/common/json.h`: jsoncons-basierter Test-JSON-Reader (`jsoncons::ojson`).

## 3. Korrektheitsprüfung im Repo

- Stelle im Repo:
  - `src/request_body_processor/json_adapter.cc` (`JSONAdapter::parse`)
- Verwendete Bibliothek:
  - simdjson/jsoncons (compile-time selected)
- Was der Code macht:
  - Wählt exakt ein Backend und normalisiert Sink-Status auf Parse-Status.
- Relevante Doku-Regel:
  - N/A (Repo-Adapterlogik).
- Bewertung: Korrekt.
- Evidenz:
  - Null-Sink-Check, Empty-Input-Short-Circuit, Backend-Dispatch, `normalizeResult`.

- Stelle im Repo:
  - `src/request_body_processor/json_backend_jsoncons.cc` (`parseDocumentWithJsoncons`)
- Verwendete Bibliothek:
  - jsoncons `json_string_cursor`
- Was der Code macht:
  - Verwendet Pull-Parsing (`done/current/next/check_done`) mit Fehlercodes.
- Relevante Doku-Regel:
  - jsoncons-Doku beschreibt genau diese Cursor-Schnittstelle und Source-Lifetime-Vorbedingung.
- Bewertung: Korrekt.
- Evidenz:
  - Cursor-Lebensdauer ist vollständig innerhalb der Funktion; `input` bleibt gültig während Cursor-Nutzung.

- Stelle im Repo:
  - `src/request_body_processor/json_backend_simdjson.cc` (`parseDocumentWithSimdjson`)
- Verwendete Bibliothek:
  - simdjson On-Demand
- Was der Code macht:
  - Parsedokument + ereignisbasierter Walk; Fehler werden über `getResult` und Mapping behandelt.
- Relevante Doku-Regel:
  - simdjson On-Demand Fehlercode-/Iterator-Modell.
- Bewertung: Korrekt (funktional), aber mit Integrationslücke bzgl. Optionen (siehe nächster Punkt).
- Evidenz:
  - konsistente Fehlerpfade (`fromSimdjsonError`, `getResult`, `walk...`).

- Stelle im Repo:
  - `src/request_body_processor/json_backend.h`, `json_backend_jsoncons.cc`, `json_backend_simdjson.cc`, `json.cc`
- Verwendete Bibliothek:
  - beide
- Was der Code macht:
  - Es existiert `JsonBackendParseOptions::technical_max_depth`.
  - jsoncons-Backend verwendet es (`cursor_options.max_nesting_depth(options.technical_max_depth)`).
  - simdjson-Backend ignoriert `options` vollständig (`(void) options`).
  - `JSON::complete()` ruft `adapter.parse(m_data, sink)` ohne explizite Optionen auf.
- Relevante Doku-Regel:
  - N/A (repoeigene API-Konsistenz), indirekt relevant für „gleiche Semantik über Backends“.
- Bewertung: Problematisch.
- Evidenz:
  - Option ist im Interface vorgesehen, wird aber backend-inkonsistent verwendet.

- Stelle im Repo:
  - `src/transaction.cc` + `src/request_body_processor/json.h` + `src/request_body_processor/json.cc`
- Verwendete Bibliothek:
  - indirekt beide
- Was der Code macht:
  - Regelkonfigurierbarer JSON-Depth-Limit wird in `JSON`-Sink gesetzt (`setMaxDepth`) und bei Container-Events geprüft.
- Relevante Doku-Regel:
  - N/A (repoeigene Tiefenbegrenzungsschicht).
- Bewertung: Teilweise korrekt.
- Evidenz:
  - Sink-Limit ist vorhanden, aber technische Backend-Depth-Option wird nicht einheitlich durchgereicht.

## 4. Performance-Optimierungen im Repo

- Stelle im Repo:
  - `src/request_body_processor/json_backend_simdjson.cc` (`parseDocumentWithSimdjson`)
- Aktuelles Verhalten:
  - Parser wird pro Aufruf neu erzeugt.
- Warum konkret ineffizient:
  - simdjson-Doku empfiehlt explizit Parser-Reuse, um Allokations-/Initialisierungskosten zu reduzieren.
- Bezug zur Bibliotheksdoku:
  - `others/simdjson/doc/performance.md` („make a parser once and reuse it“).
- Konkrete Verbesserung:
  - Parser-Reuse im langlebigeren Kontext (z. B. pro JSON-Processor-Instanz), mit klaren Thread-/Lifetime-Grenzen.
- Erwarteter Effekt:
  - Weniger Reallokationen und geringere Parse-Overhead-Kosten bei mehreren Requests.
- Evidenzgrad: Stark belegt (strukturell), ungemessen.

- Stelle im Repo:
  - `src/request_body_processor/json_backend_jsoncons.cc` (`RawJsonTokenCursor`)
- Aktuelles Verhalten:
  - Neben jsoncons-Event-Parsing wird derselbe Input erneut tokenweise gescannt.
- Warum konkret ineffizient:
  - Zusätzliche Tokenisierung + Validierung derselben Zeichenkette.
- Bezug zur Bibliotheksdoku, falls relevant:
  - jsoncons-Cursor liefert bereits STAJ-Events; Repo fügt eine zweite Scan-Schicht hinzu.
- Konkrete Verbesserung:
  - Falls fachlich zulässig, Rohzahlgewinnung enger an Cursor-Kontext koppeln und redundanten Voll-Rescan reduzieren.
- Erwarteter Effekt:
  - Reduzierte CPU-Zeit im jsoncons-Backend.
- Evidenzgrad: Plausibel aber ungemessen.

## 5. Speicher- und Speicherverwaltungs-Optimierungen im Repo

- Stelle im Repo:
  - `src/request_body_processor/json.cc` (`JSON::processChunk`, `JSON::complete`)
- Aktuelles Verhalten:
  - Vollständige Body-Materialisierung in `m_data` vor dem Parse.
- Warum konkret speicherineffizient oder riskant:
  - Peak-Memory steigt mit Body-Größe; keine Streaming-Verarbeitung.
- Ownership-/Lifetime-/Buffer-Bezug:
  - `m_data` hält Ownership bis Parseende.
- Konkrete Verbesserung:
  - Streaming-Ansatz nur bei gesichert gleicher Semantik (`addArgument`/Pfad-Logik) umsetzbar.
- Erwarteter Effekt:
  - Potenziell geringerer Peak-Memory.
- Evidenzgrad: Plausibel aber ungemessen.

- Stelle im Repo:
  - `src/request_body_processor/json_backend_simdjson.cc` (`simdjson::padded_string padded(input)`)
- Aktuelles Verhalten:
  - Zusätzliche gepolsterte Kopie des Inputs.
- Warum konkret speicherineffizient oder riskant:
  - zusätzlicher Vollbuffer pro Parse.
- Ownership-/Lifetime-/Buffer-Bezug:
  - lokale Besitzkopie je Aufruf.
- Konkrete Verbesserung:
  - Nicht pauschal ohne genaue Padding-/Mutabilitätsprüfung umstellbar.
- Erwarteter Effekt:
  - Potenziell weniger temporärer Speicher.
- Evidenzgrad: Plausibel aber ungemessen / Nicht verifizierbar für sichere Umstellung.

## 6. Probleme oder Fehlverwendungen
- Nachweisbare API-Fehlverwendung (simdjson/jsoncons): Keine Evidenz gefunden.
- Nachweisbare Lifetime-Verletzung beim jsoncons-Cursor: Keine Evidenz gefunden.
- Nachweisbare schwere Fehlerbehandlungslücke in Backend-Adaptern: Keine Evidenz gefunden.
- Konkretes Repo-Problem:
  - Backend-Option `technical_max_depth` wird inkonsistent genutzt (jsoncons ja, simdjson nein), obwohl gemeinsames Interface existiert.

## 7. Nicht gefundene oder nicht verifizierbare Punkte
- Vollständiger Gleichheitsnachweis der Semantik beider Backends für alle JSON-Eckfälle: Nicht verifizierbar (im Repo keine dedizierten Cross-Backend-Compliance-Tests gefunden).
- Quantifizierte Performance-/Speichergewinne der vorgeschlagenen Maßnahmen: Leistungsgewinn nicht belegt (keine Messläufe in diesem Audit).
- Aussage „Backend A ist insgesamt schneller/besser“: Im Repo nicht nachweisbar.

## 8. Wichtigste Maßnahmen
1. **Korrektheit/Konsistenz:** `JsonBackendParseOptions` backend-konsistent anwenden (simdjson-Pfad nicht ignorieren) oder Interface vereinfachen, wenn technisch nicht unterstützt.
2. **Performance:** simdjson-Parser-Reuse in der ModSecurity-Integration prüfen und mit klaren Threading-Regeln umsetzen.
3. **Performance:** Reduktion der doppelten Tokenisierung im jsoncons-Backend prüfen.
4. **Verifizierbarkeit:** Cross-Backend-Compliance-Tests (gleiche Inputs, gleiche Events/Fehlerklassen) ergänzen.
5. **Messbarkeit:** Reproduzierbare Benchmarks für JSON-Request-Body-Pfade im Repo ergänzen (CPU + Peak-Memory).
