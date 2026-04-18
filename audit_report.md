# Executive Summary

- Diese Prüfung konnte **nicht** als Voll-Audit der Upstream-Implementierungen von `simdjson`, `jsoncons` und `sourcemeta/blaze` durchgeführt werden, weil die Submodule `others/simdjson`, `others/jsoncons` und `others/blaze` im vorliegenden Workspace keine Quell-Dateien enthalten (nur Verzeichnisstruktur vorhanden). **Nicht verifiziert** für Stage-1/Stage-2-Details, SIMD-Dispatch-Details, interne Parser-Algorithmen, Draft-Abdeckung und Referenzauflösungs-Interna der Upstream-Projekte.
- Verifiziert wurde stattdessen die **Integrationsschicht in ModSecurity**: JSON-Backend-Adapter (`simdjson`/`jsoncons`), Aufbau des Validation-Event-Tapes und die Blaze-C-Bridge.
- Verifizierter Hochrisiko-/Mittelrisiko-Punkt: Die Blaze-Bridge materialisiert die komplette Event-Tape-Eingabe als vollständigen JSON-Baum ohne explizite Größenbegrenzung in der Bridge selbst; bei direkter C-ABI-Nutzung kann dies Memory-DoS begünstigen.
- Verifizierter Korrektheitspunkt: Duplicate Keys werden im Blaze-Pfad deterministisch auf „last key wins“ reduziert (expliziter Kommentar + Verhalten + Unit-Test).
- Verifizierter Performance-Punkt: Der `jsoncons`-Pfad enthält einen zusätzlichen Roh-Token-Synchronisationsscanner (`RawJsonTokenCursor`) zusätzlich zum eigentlichen Parser-Event-Loop.

# Methodik

- Untersuchte Repositories/Verzeichnisse:
  - ModSecurity Integrationscode unter `src/request_body_processor/`, `src/json_schema/`, `others/blaze-wrapper/`, `test/unit/`, `configure.ac`, `src/Makefile.am`, `others/blaze-wrapper/CMakeLists.txt`.
- Direkt geprüfte Aspekte:
  - Datenfluss JSON-Parse -> EventSink -> ValidationInput -> Blaze-Validator.
  - Fehler-Mapping, Depth-Limits, Lebensdauer/Pointer-Übergaben, C-ABI-Ausnahmebehandlung.
  - Testabdeckung in `json_backend_depth_tests` und `json_schema_tests`.
- Nicht direkt prüfbar in diesem Workspace:
  - Upstream-Quellcode von `simdjson` (Stage 1/2, SIMD-Dispatch, UTF-8-Interna).
  - Upstream-Quellcode von `jsoncons` (Template-Architektur intern, Cursor-/Parser-Interna).
  - Upstream-Quellcode von `sourcemeta/blaze` (Compiler/Evaluator-Interna, Draft-Support, `$ref`/anchors/dynamic refs).
- Evidenz für die Einschränkung:
  - `configure.ac` und Build-Dateien erwarten diese Komponenten als Submodule, im aktuellen Tree sind die Verzeichnisse vorhanden, aber ohne Dateien.

# simdjson Analyse

## Verifizierte Befunde

1. **Thread-lokale Parser-Wiederverwendung ist explizit implementiert.**
   - Evidenz: `getReusableSimdjsonParser()` verwendet `thread_local std::unique_ptr<simdjson::ondemand::parser>`.
   - Bewertung: Keine Cross-Thread-Parser-Sharing-Race in dieser Schicht; Kapazität bleibt pro Thread erhalten (bewusstes Trade-off).

2. **Depth-Limit wird im Walker separat erzwungen.**
   - Evidenz: `enforceTechnicalDepth()` prüft `value.current_depth()` gegen `m_technical_max_depth`.
   - Bewertung: Zusätzliche Laufzeit-Grenze unabhängig von simdjson-internen Checks.

3. **Mutable Input wird absichtlich gepolstert (in-place).**
   - Evidenz: `prepareMutableSimdjsonInput()` ruft `simdjson::pad(*input)`; Tests prüfen, dass String wächst und nur Whitespace ergänzt wird.
   - Bewertung: API-Falle möglich, wenn Aufrufer mutierenden Parse-Pfad nicht erwartet; im Code jedoch bewusst und getestet.

4. **Const-Input-Pfad vermeidet Mutation und kann Copy-Fallback nutzen.**
   - Evidenz: `prepareConstSimdjsonInput()` prüft `has_sufficient_padding()`, sonst `simdjson::padded_string`.
   - Bewertung: Korrekte Lifetime-/Padding-Handhabung in dieser Integrationsschicht.

## Hypothesen / noch zu prüfen

- **Stage-1/Stage-2-Korrektheit, SIMD-Dispatch, architekturspezifische Pfade, UTF-8-Implementierungsdetails**: Nicht verifiziert (Upstream-Quellcode fehlt im Workspace).

## Architektur

- `JSONAdapter` dispatcht build-time auf simdjson/jsoncons.
- simdjson-Pfad traversiert On-Demand-Document per `JsonBackendWalker` und sendet Events an `JsonEventSink`.
- Ownership/Lifetime:
  - Parser: thread_local + Reuse.
  - Input-Lifetime: `padded_string_view` bleibt im Scope der Parse-Funktion.

## Security

- Positiv verifiziert:
  - Null-Sink wird als InternalError abgefangen.
  - Fehlercodes werden auf `JsonParseStatus` gemappt.
- Nicht verifiziert:
  - UB-/Bounds-Risiken innerhalb simdjson selbst.

## Correctness

- Verifiziert:
  - Root-Scalar-Pfade (string/number/bool/null) getrennt behandelt.
  - Truncated/UTF-8/Parse/Internal Fehler werden differenziert gemappt.
  - Zahl-Lexeme werden als Raw-Token (`raw_json_token`) an Sink weitergereicht.
- Nicht verifiziert:
  - Vollständige RFC-8259-Compliance der simdjson-Upstream-Engine.

## Performance

- Verifiziert:
  - Parser-Reuse reduziert Konstruktionskosten.
  - Mutable-Pfad vermeidet zusätzliche Kopie (Padding in-place).
  - Const-Pfad kann Kopie verursachen, wenn Padding fehlt.
- Hypothese:
  - Lang laufende Worker mit sehr großen Requests können pro Thread hohe Parserkapazität behalten (Trade-off, noch workload-spezifisch zu messen).

## Tests

- Verifiziert vorhanden:
  - Tiefe-Limit-Fälle, Truncated/Malformed, Root- und Container-Zahllexeme, mutable-vs-const Input-Verhalten.
- Schwachstelle:
  - Kein direkter Sanitizer-/Fuzz-Harness im geprüften Scope sichtbar.

## Wichtige Code-Referenzen

- `src/request_body_processor/json_backend_simdjson.cc`
- `src/request_body_processor/json_adapter.cc`
- `test/unit/json_backend_depth_tests.cc`

# jsoncons Analyse

## Verifizierte Befunde

1. **Zusätzlicher Roh-Token-Scanner (`RawJsonTokenCursor`) ist zentraler Teil des Zahlenpfads.**
   - Evidenz: `parseDocumentWithJsoncons()` erzeugt `json_string_cursor` und zusätzlich `RawJsonTokenCursor`; `emitEvent()` nutzt `emitNumberFromRawToken()`.
   - Bewertung: Zusätzliche CPU-Kosten durch zweiten Tokenisierungs-/Synchronisationspfad.

2. **Numerische String-Events (`bigint`/`bigdec`) werden als Zahlen behandelt.**
   - Evidenz: `isNumericStringEvent()` prüft `semantic_tag::bigint/bigdec`; `emitEvent()` leitet solche string-events in `on_number(...)`.
   - Bewertung: Für die ModSecurity-Event-Schnittstelle konsistente „Rohzahl-Lexem“-Semantik.

3. **Fehler-Mapping enthält spezifische UTF-8-Klassifikation.**
   - Evidenz: `isUtf8RelatedError()` + `fromJsonconsError()` mappt auf `Utf8Error`, `TruncatedInput`, `ParseError`, `InternalError`.
   - Bewertung: Saubere Fehlerkanäle in dieser Schicht.

## Hypothesen / noch zu prüfen

- Template-Interaktionen, interne Parser-Optimierungen, Cursor-Positionsgarantien und vollständige JSON-Konformität von jsoncons intern: **Nicht verifiziert** (Upstream-Quellcode fehlt).

## Architektur

- Event-basierter Cursor (`json_string_cursor`) plus separater Scan auf Originalinput für Rohlexeme.
- Bei jedem Event: Dispatch in `emitEvent()`, Zahlen über `rawNumberFromContext()` + Fallbacks.

## Security

- Positiv verifiziert:
  - Null-Sink Check.
  - Fehlersituationen liefern Status+Detail statt Exceptions nach außen.
- Hypothese:
  - Bei extrem langen Inputs mit sehr vielen Zahlen kann zusätzlicher Scanner DoS-Oberfläche (CPU) erhöhen; quantitativ nicht benchmark-verifiziert in dieser Ausführung.

## Correctness

- Verifiziert:
  - Zahlensyntax wird lokal validiert (`isValidJsonNumber`).
  - Zahllexeme bleiben laut Tests exakt erhalten (Root + Container).
  - Unicode-String-Test in Schema-Pipeline vorhanden.
- Nicht verifiziert:
  - Vollständige upstream-jsoncons Compliance (z. B. alle Edgecases um Surrogates/NaN-artige Nicht-JSON-Inputs).

## Performance

- Verifiziert:
  - Zusätzlicher Scanner + Eventloop verursacht Mehrarbeit gegenüber reinem Parser-Event-Stream.
  - Instrumentation-Hooks für Cursor-/Token-Cost sind vorhanden.
- Nicht verifiziert:
  - Konkrete Throughput-/Latency-Zahlen im aktuellen Lauf.

## Tests

- Verifiziert vorhanden:
  - Zahllexeme-Regressionen, Tiefe-Limit, Truncated/Malformed.
- Fehlend/ausbaufähig:
  - Differential Testing simdjson vs jsoncons über große zufällige Korpora.
  - Fuzzing gezielt auf `RawJsonTokenCursor` Synchronisation.

## Wichtige Code-Referenzen

- `src/request_body_processor/json_backend_jsoncons.cc`
- `doc/jsoncons_number_scan_assessment.md`
- `test/unit/json_backend_depth_tests.cc`

# Blaze Analyse

## Verifizierte Befunde

1. **Bridge kompiliert Schema mit FastValidation-Modus.**
   - Evidenz: `sourcemeta::blaze::compile(..., Mode::FastValidation)` in `others/blaze-wrapper/bridge.cc`.
   - Bewertung: Performance-orientierter Kompilierungsmodus aktiv; mögliche Compliance-Trade-offs müssen gegen Upstream-Dokumentation geprüft werden (hier nicht verifiziert).

2. **C-ABI fängt Exceptions ab und mappt auf stabile Statuscodes + thread_local Fehlertext.**
   - Evidenz: try/catch in `msc_blaze_schema_create_from_file` und `msc_blaze_schema_validate`, `thread_local std::string g_last_error`.
   - Bewertung: Gute ABI-Härtung gegen Exception-Leaks.

3. **Event-Tape wird vollständig in JSON-Baum materialisiert.**
   - Evidenz: `materializeJson(...)` baut `std::vector<ContainerFrame>` + `std::optional<JSON> root`; bei Zahlen `parse_json(std::string(raw_number))`.
   - Bewertung: Zusätzliche Speicher- und Parse-Kosten vor eigentlicher Blaze-Validierung.

4. **Duplicate Keys: last wins ist explizit implementiert und getestet.**
   - Evidenz: Kommentar + `parent.value.assign(parent.pending_key, ...)` in Bridge; Test `expectDuplicateKeysUseLastValueDuringBlazeValidation`.
   - Bewertung: Deterministische Semantik, aber potenzielles Compliance-/Erwartungsrisiko je nach gewünschter Duplicate-Key-Policy.

5. **Keine explizite Größen-/Tiefenbegrenzung in Bridge-C-ABI selbst.**
   - Evidenz: `materializeJson` nutzt `event_count` als Reserve und verarbeitet bis Ende; kein eigener Hard-Limit-Guard.
   - Bewertung: Bei direkter C-ABI-Nutzung mögliches Memory-/CPU-DoS-Risiko.

## Hypothesen / noch zu prüfen

- Draft-Support, `$ref`-Auflösung, Anchors, Dynamic Refs, Annotation/Assertion-Compliance von Blaze intern: **Nicht verifiziert** (Upstream-Blaze-Code im Workspace nicht verfügbar).

## Architektur

- ModSecurity-`ValidationInput` -> `validator_blaze.cc` (Bridge-Events) -> C-ABI (`blaze_bridge.h`) -> C++ Bridge (`others/blaze-wrapper/bridge.cc`) -> Blaze Evaluator.
- Ownership/Lifetime:
  - `validator_blaze.cc` hält `msc_blaze_schema_handle*` und zerstört in Destructor.
  - Event-Text-Pointer zeigen auf Strings im `ValidationInput` während des synchronen `msc_blaze_schema_validate`-Aufrufs.

## Security

- Positiv verifiziert:
  - Exception-Isolation über C-ABI.
  - Null-Handle/Null-Path Checks vorhanden.
- Kritischer Befund:
  - Fehlender Bridge-interner Input-Limit-Guard (event_count/MaxDepth/MaxBytes) -> DoS-Fläche bei direktem ABI-Aufruf.

## Correctness

- Verifiziert:
  - Fehlersignale `VALID/INVALID/INPUT_ERROR/SCHEMA_ERROR` werden gemappt.
  - String-vs-Number-Verhalten wird in Tests abgedeckt.
  - Duplicate-key-Verhalten ist konsistent dokumentiert/implementiert/getestet.
- Nicht verifiziert:
  - Vollständige JSON-Schema-Spec-Compliance der Blaze-Evaluator-Engine.

## Performance

- Verifiziert:
  - Doppelte Arbeit: Event-Tape -> JSON-Materialisierung -> Blaze-Evaluation.
  - Zusätzliche Allokation pro Zahl durch `std::string(raw_number)` für `parse_json`.
- Hypothese:
  - Für große numerische Payloads ist dieser Konvertierungspfad messbar dominant; Benchmark notwendig.

## Tests

- Verifiziert vorhanden:
  - Bridge-Fehler über C-ABI, fehlendes Schema, String/Const-Validation, Duplicate-Key-Semantik.
- Schwachstellen:
  - Keine sichtbaren Tests für sehr tiefe/umfangreiche Event-Tapes am C-ABI.
  - Kein sichtbares Fuzzing direkt gegen `msc_blaze_schema_validate`.

## Wichtige Code-Referenzen

- `src/json_schema/validator_blaze.cc`
- `src/json_schema/blaze_bridge.h`
- `others/blaze-wrapper/bridge.cc`
- `test/unit/json_schema_tests.cc`

# Projektvergleich

- Verifiziert:
  - `simdjson`-Integration setzt auf streamingartige Traversierung ohne vollständige Re-Materialisierung in dieser Schicht.
  - `jsoncons`-Integration hat einen zusätzlichen Rohzahl-Token-Synchronisationspfad.
  - Blaze-Integration materialisiert das komplette Event-Tape in einen JSON-Baum vor der Validierung.
- Hypothese:
  - Bei großen Instanzen ist die Blaze-Brücke (Materialisierung + erneutes Parsen von Zahllexemen) tendenziell speicherintensiver als reine Event-Forwarding-Ansätze.

# Findings Tabelle

| Status | Schweregrad | Projekt | Bereich | Befund | Evidenz | Unsicherheit | Fix-Vorschlag |
|--------|-------------|---------|---------|--------|---------|--------------|---------------|
| Verifiziert | Mittel | Blaze (Bridge) | Security/DoS | Keine expliziten Eingabe-Limits in `msc_blaze_schema_validate`/`materializeJson`; vollständige Materialisierung abhängig von `event_count` | `others/blaze-wrapper/bridge.cc` (`materializeJson`, `msc_blaze_schema_validate`) |  | Harte Limits für `event_count`, max. Tiefe und max. String/Number-Länge in Bridge ergänzen; bei Überschreitung früh mit `INPUT_ERROR` abbrechen |
| Verifiziert | Niedrig-Mittel | Blaze (Bridge) | Performance | Jede Zahl wird über `parse_json(std::string(raw_number))` neu geparst und allokiert | `others/blaze-wrapper/bridge.cc` (`MSC_BLAZE_EVENT_NUMBER` Zweig) |  | Direkte Zahlkonstruktion ohne Re-Parse prüfen (falls Core-API geeignet), andernfalls String-Reuse/arena-basierte Strategie evaluieren |
| Verifiziert | Niedrig | jsoncons-Integration | Performance | Zusätzlich zum `jsoncons` Cursor wird ein zweiter Roh-Token-Scanner betrieben | `src/request_body_processor/json_backend_jsoncons.cc` (`RawJsonTokenCursor`, `emitNumberFromRawToken`, `parseDocumentWithJsoncons`) |  | Optionalen Fast-Path ohne Scan für unkritische Modi oder verbesserte Positionsnutzung evaluieren |
| Verifiziert | Niedrig | simdjson-Integration | Correctness/API-Falle | Mutable Parse-Pfad verändert Input (Padding) absichtlich | `src/request_body_processor/json_backend_simdjson.cc` (`prepareMutableSimdjsonInput`), `test/unit/json_backend_depth_tests.cc` |  | API-Vertrag prominent dokumentieren; optional ausschließlich const-Pfad in sicherheitskritischen Aufrufern verwenden |
| Hypothese | Mittel | Blaze (Upstream) | Correctness/Compliance | `Mode::FastValidation` könnte je nach Upstream-Definition Compliance-Trade-offs enthalten | `others/blaze-wrapper/bridge.cc` (compile with `Mode::FastValidation`) | Upstream-Dokumentation/Code im Workspace nicht verifiziert | Gegen Blaze-Dokumentation und Compliance-Suite (Draft-spezifisch) gegenprüfen |
| Hypothese | Mittel | simdjson/jsoncons/Blaze (Upstream) | Security/Correctness | Interne UB-/Bounds-/Spec-Edgecases der Upstream-Libraries nicht beurteilbar | Submodule-Verzeichnisse ohne Quell-Dateien im Workspace | Fehlender Upstream-Code | Submodule vollständig auschecken und dediziertes Upstream-Audit durchführen |

# Fehlende Evidenz / Grenzen der Analyse

- Keine direkte Einsicht in Upstream-Implementierungen von `simdjson`, `jsoncons`, `blaze` (Submodule ohne Dateien im aktuellen Workspace).
- Dadurch nicht belastbar beurteilbar:
  - simdjson Stage-1/Stage-2-Details, SIMD-Dispatch, UTF-8-Kernroutinen.
  - jsoncons interne Template- und Parser-Interna.
  - Blaze Draft-Matrix, `$ref`-Resolver-Mechanik, Dynamic-Ref-Handling.
- Keine ausgeführten Benchmarks in diesem Lauf; Performance-Bewertungen basieren auf statischer Codeanalyse der Integrationsschicht.

# Empfohlene nächste Prüfungen

1. **Submodule vollständig bereitstellen** und Upstream-Code auditieren (harte Voraussetzung).
2. **Fuzzing**:
   - `JSONAdapter` (beide Backends) mit libFuzzer + ASan/UBSan.
   - `msc_blaze_schema_validate` direkt (Event-Tape-Fuzzer inkl. malformed Sequenzen).
3. **Differential Testing**:
   - Gleiches JSON-Korpus gegen simdjson- und jsoncons-Backend, Event-Tape und Fehlerstatus vergleichen.
4. **DoS-Tests**:
   - Sehr tiefe und sehr große Event-Tapes gegen Blaze-Bridge, Speichergrenzen und Laufzeit messen.
5. **Compliance-Tests Blaze**:
   - Offizielle JSON-Schema-Test-Suite Draft-weise (inkl. `$ref`, anchors, dynamic refs), Ergebnisse mit aktiviertem `FastValidation` dokumentieren.
6. **Sanitizer-/Hardening-Läufe**:
   - ASan, UBSan, MSan (wo möglich), TSan für Multithread-Workloads.
7. **Mikrobenchmarks**:
   - Kosten von `RawJsonTokenCursor` isoliert messen.
   - Kosten von Zahl-Reparse in Blaze-Bridge (`MSC_BLAZE_EVENT_NUMBER`) isoliert messen.
