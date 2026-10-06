# Analyse: JSON und mögliche Alternativen

## 1. Vorgehen

Im Repository nachweisbar ist folgende Such- und Prüfstrategie:

- Dateisuche nach Endungen: `.json`, `.jsonl`, `.ndjson`.
- Volltextsuchen (repo-weit) nach Schlüsselbegriffen wie `json`, `application/json`, `yajl`, `SecAuditLogFormat`, `requestBodyProcessor=JSON`, `processContentOffset`, `yaml`, `toml`, `csv`, `xml`, `protobuf`, `msgpack`, `cbor`, `avro`, `parquet`.
- Prüfung zentraler Implementierungsstellen:
  - Request-Body-Parser (`src/request_body_processor/json.cc`, `src/transaction.cc`)
  - Auditlog-Writer (`src/audit_log/writer/*.cc`, `src/transaction.cc`, `headers/modsecurity/audit_log.h`)
  - JSON-Ergebnisobjekte (`src/modsecurity.cc`, `headers/modsecurity/modsecurity.h`)
  - Test-Framework und Testfälle (`test/common/modsecurity_test.cc`, `test/regression/*`, `test/test-suite.in`, `test/test-cases/**/*.json`)
  - Build-/Dependency-Dateien (`configure.ac`, `src/Makefile.am`, `test/Makefile.am`)
  - Parser-/Konfigurationssprache (`src/parser/seclang-scanner.ll`, `src/parser/seclang-parser.yy`, `src/rules_set.cc`)
- Quantitative Bestandsaufnahme:
  - Nachweisbar: 196 `.json`-Dateien, 0 `.jsonl`-Dateien, 0 `.ndjson`-Dateien.

## 2. Nachweisbare JSON-Nutzung im Repository

| Bereich | Fundstelle | Art der Nutzung | Pflicht oder Konvention | Kurzbeleg |
|---|---|---|---|---|
| Build/Dependencies | `configure.ac` | YAJL wird als Bibliothek geprüft/konfiguriert | faktisch Pflicht für JSON-Funktionen | `PROG_YAJL`, `YAJL_FOUND`; README nennt YAJL als mandatory dependency für JSON-Logs und Test-Framework. |
| Request-Body-Parsing | `src/request_body_processor/json.cc` | Streaming-JSON-Parsing via YAJL-Callbacks | optional über Regel/Content-Type, aber im Code implementiert | `yajl_alloc`, `yajl_parse`, `yajl_complete_parse`, Callbacks für map/array/string/number. |
| Request-Body-Parsing Aktivierung | `src/actions/ctl/request_body_processor_json.cc`, `src/transaction.cc` | Umschalten auf JSON-Processor | regelgesteuert | `ctl:requestBodyProcessor=JSON` setzt `m_requestBodyProcessor = JSONRequestBody`; Verarbeitungspfad in `processRequestBody()`. |
| Request-Body JSON Tiefe | `src/parser/seclang-scanner.ll`, `src/parser/seclang-parser.yy`, `src/transaction.cc`, `src/request_body_processor/json.cc` | Direktive `SecRequestBodyJsonDepthLimit` | konfigurierbar | Scanner/Parser erkennen Direktive, Wert wird an JSON-Parser weitergegeben, Überschreitung erzeugt Fehler. |
| Auditlog-Serialisierung | `headers/modsecurity/audit_log.h`, `src/parser/seclang-parser.yy`, `src/audit_log/writer/serial.cc`, `src/audit_log/writer/parallel.cc`, `src/transaction.cc` | JSON und Native als auswählbare Auditlog-Formate | konfigurierbar, beide unterstützt | Enum mit `JSONAuditLogFormat` und `NativeAuditLogFormat`; Writer ruft je nach Format `toJSON()` oder `toOldAuditLogFormat()`. |
| Auditlog-HTTPS | `src/audit_log/writer/https.cc` | HTTP-Export als JSON mit `application/json` | aktuell fest verdrahtet | `transaction->toJSON(parts)` und `setRequestType("application/json")`. |
| Interne Ergebnisobjekte | `headers/modsecurity/modsecurity.h`, `src/modsecurity.cc` | `processContentOffset(..., std::string *json, ...)` erzeugt JSON | API-vertraglich JSON | Signatur enthält `json`; Implementation baut JSON mit YAJL. |
| Weitere JSON-Ausgabe | `src/transaction.cc` | Auditlog-JSON-Objekt | optional via Formatwahl | `toJSON(int parts)` erzeugt strukturierte JSON-Ausgabe per YAJL. |
| Tests/Fixtures | `test/common/modsecurity_test.cc`, `test/regression/regression_test.cc`, `test/test-suite.in`, `test/test-cases/**/*.json` | Testfälle als JSON, Laden/Formatieren via YAJL | aktuell stark gekoppelt | Loader akzeptiert `.json`, parst mit `yajl_tree_parse`; `test-suite.in` listet JSON-Dateien. |
| Interne Persistenz (Collections) | `src/collection/backend/collection_data.cc`, `src/collection/backend/lmdb.cc` | JSON-ähnliche String-Serialisierung in LMDB-Werten | intern, mit Fallback | `getSerialized()` erzeugt `{ "__expire_":..., "__value_":"..." }`; `setFromSerialized()` hat Legacy-Fallback auf Plain-String. |

## 3. Einsatzbereiche

### 3.1 Request/Response

Im Repository nachweisbar ist:

- JSON-Request-Body-Parsing über YAJL (`src/request_body_processor/json.cc`).
- Aktivierung über Regeln (`ctl:requestBodyProcessor=JSON`) und typischerweise `Content-Type: application/json` in Testfällen (`test/test-cases/regression/request-body-parser-json.json`).
- Alternativer XML-Request-Body-Parser ist implementiert (`src/request_body_processor/xml.cc`, `src/actions/ctl/request_body_processor_xml.cc`).
- URL-encoded ist ebenfalls als eigener Processor vorhanden (`src/actions/ctl/request_body_processor_urlencoded.cc`).

Kopplung:

- Mittel bis hoch: Regeln, Variablenpfade (z. B. `ARGS:json.*`), Fehlerpfade (`REQBODY_ERROR*`) und Tests hängen daran.

### 3.2 Auditlogs / Eventlogs

Im Repository nachweisbar ist:

- Zwei Auditlog-Formate: `JSON` und `NATIVE` (Enum + Parserdirektive `SecAuditLogFormat`).
- Serial und Parallel Writer wählen abhängig vom Format zwischen `toJSON()` und `toOldAuditLogFormat()`.
- HTTPS-Writer sendet ausschließlich JSON und setzt `application/json`.

Kopplung:

- Hoch für HTTPS-Export (JSON fest).
- Mittel für Datei-Auditlogs (Native bereits unterstützt).

### 3.3 Interne Ergebnisobjekte / processContentOffset-ähnliche Daten

Im Repository nachweisbar ist:

- `ModSecurity::processContentOffset(...)` schreibt das Ergebnis in einen Parameter namens `json` und baut JSON mit YAJL.
- Ohne YAJL liefert der Code explizit einen Fehlertext, dass JSON nicht generiert werden kann.

Kopplung:

- Hoch: API-Signatur und Fehlerverhalten sind direkt an JSON/YAJL gebunden.

### 3.4 Testfälle / Fixtures

Im Repository nachweisbar ist:

- Test-Loader lädt Dateien mit `.json`-Suffix.
- Parsing und Formatierung der Testfälle basiert auf YAJL (`yajl_tree_parse`, `toJSON()` im Regressionstestcode).
- Umfangreiche Test-Suite ist als JSON-Dateien hinterlegt (`test/test-cases/...`).

Kopplung:

- Sehr hoch: Dateiformat, Parser, Formatter, Testsuite-Listen.

### 3.5 Konfiguration

Im Repository nachweisbar ist:

- Engine-Konfiguration erfolgt über SecLang/Textregeln (Scanner/Parser-Dateien und `RulesSet::load*` via Driver-Parser).
- Für Auditlog-Format sind die Werte `JSON` und `NATIVE` direkt in der Konfigurationssprache vorgesehen.
- Es konnte nicht belegt werden, dass YAML/TOML/JSON als primäres Konfigurationsdateiformat für die Engine unterstützt werden.

Kopplung:

- Hoch an SecLang-Parser und vorhandene Direktiven.

### 3.6 Sonstige Verwendungen

Im Repository nachweisbar ist:

- Interne Collection/LMDB-Serialisierung nutzt ein JSON-ähnliches Stringformat mit Spezialschlüsseln `__expire_` und `__value_`, plus Legacy-Fallback auf Plain-String.
- Es konnte nicht belegt werden, dass für Export/Import zwischen externen Komponenten Formate wie Protobuf/Avro/Parquet/MessagePack/CBOR verwendet werden.

## 4. Mögliche Alternativen pro Einsatzbereich

### Request/Response

- **Geeignete Alternativen (repo-basiert):**
  - **XML** als bereits implementierter alternativer Body-Processor (nicht Ersatz für JSON, aber parallele Option).
  - **URLENCODED** für flachere Formdaten (bereits implementiert).
- **Ungeeignete Alternativen (repo-basiert):**
  - YAML/TOML/CSV/Protobuf/MessagePack/CBOR/Avro/Parquet: Es konnte im Repository kein Parserpfad für Request-Body-Inspektion dieser Formate belegt werden.
- **Begründung:** Vorhanden sind konkrete Pfade für JSON/XML/URLENCODED; für die anderen Formate fehlen nachweisbare Parser, Direktiven und Tests.
- **Offene Unsicherheiten:** Ob Connector-seitig Vortransformationen existieren, ist **nicht nachweisbar im Repository**.

### Auditlogs / Eventlogs

- **Geeignete Alternativen (repo-basiert):**
  - **Native Auditlog-Format** ist bereits first-class unterstützt (`SecAuditLogFormat NATIVE`).
- **Ungeeignete Alternativen (repo-basiert):**
  - Für YAML/TOML/XML/CSV/Protobuf/MessagePack/CBOR/Avro/Parquet existiert im Auditlog-Writer kein nachweisbarer Serialisierungspfad.
- **Begründung:** Writer-Code kennt nur `toJSON()` und `toOldAuditLogFormat()`.
- **Offene Unsicherheiten:** Ob externe Log-Pipelines nachgelagert transformieren, ist **nicht nachweisbar im Repository**.

### Interne Ergebnisobjekte / processContentOffset

- **Geeignete Alternativen (repo-basiert):**
  - Keine direkt nachweisbar implementierte Alternative.
- **Technisch möglich (ohne Repo-Implementierung):**
  - Ein zusätzlicher Text-/Struct-Rückgabepfad wäre grundsätzlich denkbar, ist aber **nicht nachweisbar im Repository**.
- **Ungeeignete Alternativen (repo-basiert):**
  - Alles außer JSON ist derzeit nicht über API/Codepfad belegt.
- **Offene Unsicherheiten:** API-Consumer-Anforderungen außerhalb dieses Repos sind **nicht nachweisbar im Repository**.

### Testfälle / Fixtures

- **Geeignete Alternativen (repo-basiert):**
  - Keine sofort nutzbare Alternative belegt.
- **Ungeeignete Alternativen (repo-basiert):**
  - YAML/TOML/CSV/XML/... ohne Loader/Parser/Formatter-Unterstützung im Testframework.
- **Begründung:** Test-Lader filtert `.json` und nutzt YAJL-Baumparser.
- **Offene Unsicherheiten:** Migrationsskripte für andere Formate sind **nicht nachweisbar im Repository**.

### Konfiguration

- **Geeignete Alternativen (repo-basiert):**
  - Für Auditlog-Ausgabeformat: **NATIVE** als konfigurierbare Alternative zu JSON.
- **Ungeeignete Alternativen (repo-basiert):**
  - YAML/TOML/JSON als Ersatz für SecLang-Konfigurationsdateien sind nicht belegt.
- **Begründung:** Parser/Scanner sind auf SecLang-Tokens ausgerichtet.
- **Offene Unsicherheiten:** Externe Generierung von SecLang aus anderen Formaten ist **nicht nachweisbar im Repository**.

### Sonstige (Persistenz/Interner Austausch)

- **Geeignete Alternativen (repo-basiert):**
  - **Plain-String-Fallback** ist bereits implementiert (Legacy-Kompatibilität in `setFromSerialized`).
- **Ungeeignete Alternativen (repo-basiert):**
  - Binärformate (MessagePack/CBOR/Protobuf/Avro/Parquet) ohne nachweisbare Reader/Writer/Schema-Definitionen.
- **Begründung:** Der aktuelle LMDB-Pfad erwartet explizit das vorhandene JSON-ähnliche Schema oder Legacy-Plain-String.
- **Offene Unsicherheiten:** Datenmigrationswerkzeuge sind **nicht nachweisbar im Repository**.

## 5. Vergleich der Formate

| Format | Im Repo bereits vorhanden? | Für verschachtelte Daten geeignet? | Für tabellarische Daten geeignet? | Menschenlesbar? | Streaming-freundlich? | Schema/Validierung im Repo vorhanden? | Externe Kompatibilität im Repo nachweisbar? | Migrationsaufwand | Evidenz |
|---|---|---|---|---|---|---|---|---|---|
| JSON | Ja (Parser, Generator, Tests, Auditlog) | Ja (nachweisbar genutzt) | Eingeschränkt (keine CSV-ähnliche Nutzung belegt) | Ja (Testfälle/Auditlog) | Teilweise (Auditlog `toJSON()` hängt `\n` an; NDJSON nicht explizit deklariert) | Teilweise (Parsing + Tiefenlimit; kein JSON-Schema belegt) | Ja (`application/json` im HTTPS-Auditwriter) | Bereits im Einsatz | Hoch |
| Native Auditlog-Format | Ja (Auditlog) | Teilweise (parts-basiert, textuell) | Nein/nicht belegt | Teilweise | Teilweise (append in Dateien) | Nicht als formales Schema belegt | Ja (historisches Auditlog innerhalb Repo-Kontext) | Niedrig (bereits unterstützt) | Hoch |
| XML | Ja (Request-Body-Parser) | Ja | Nein/nicht belegt | Ja | Nicht nachweisbar | Teilweise (XML Parser + DTD/Schema-bezogene Operatoren) | Ja (Regeln/Tests nutzen XML-Processing) | Mittel bis hoch (für JSON-Use-Cases) | Mittel |
| YAML | Nein | Technisch möglich, aber nicht belegt | Technisch möglich, aber nicht belegt | Technisch möglich, aber nicht belegt | Nicht nachweisbar | Nein | Nein | Hoch | Niedrig |
| TOML | Nein | Nicht nachweisbar im Repo-Kontext | Technisch möglich, aber nicht belegt | Technisch möglich, aber nicht belegt | Nicht nachweisbar | Nein | Nein | Hoch | Niedrig |
| CSV | Nein | Für verschachtelte Daten ungeeignet im Repo-Kontext | Technisch geeignet für Tabellen, aber nicht belegt | Ja | Ja (zeilenbasiert), aber nicht belegt | Nein | Nein | Hoch | Niedrig |
| NDJSON / JSONL | Nein (keine Dateien mit Endung), implizite Nähe über newline-JSON im Auditpfad | Ja (JSON-basiert) | Eingeschränkt | Ja | Ja (zeilenbasiert) | Nein | Nein (nicht explizit) | Mittel | Niedrig bis Mittel |
| Protocol Buffers | Nein | Nicht belegt | Nicht belegt | Nein (binär) | Nicht belegt | Nein | Nein | Sehr hoch | Niedrig |
| MessagePack | Nein | Nicht belegt | Nicht belegt | Nein (binär) | Nicht belegt | Nein | Nein | Sehr hoch | Niedrig |
| CBOR | Nein | Nicht belegt | Nicht belegt | Nein (binär) | Nicht belegt | Nein | Nein | Sehr hoch | Niedrig |
| Avro | Nein | Nicht belegt | Nicht belegt | Eher nein (typisch schema+binär/json encoding), im Repo nicht belegt | Nicht belegt | Nein | Nein | Sehr hoch | Niedrig |
| Parquet | Nein | Nicht belegt | Spaltenformat, aber im Repo nicht belegt | Nein | Nicht belegt | Nein | Nein | Sehr hoch | Niedrig |

Hinweis: Aussagen wie „technisch möglich“ sind hier bewusst als Möglichkeit gekennzeichnet; sie sind nicht als bestehende Repo-Funktion zu verstehen.

## 6. Empfehlungen

### Bei JSON belassen

1. **Request-Body-JSON-Parsing** belassen.
   - Grund: Vollständig implementiert, regelgesteuert, mit Tests und spezifischer Tiefenlimit-Konfiguration.
   - Evidenzgrad: **Hoch**.

2. **`processContentOffset`-JSON-Ausgabe** belassen.
   - Grund: API-Signatur und Implementierung sind explizit JSON-gebunden; ohne YAJL wird Fehler zurückgegeben.
   - Evidenzgrad: **Hoch**.

3. **JSON-Testfallformat** belassen (kurz- bis mittelfristig).
   - Grund: Testloader, Formatter und große Testbasis sind direkt auf JSON/YAJL ausgerichtet.
   - Evidenzgrad: **Hoch**.

### Als Alternative prüfen

1. **Auditlog-Dateiformat: NATIVE statt JSON** in ausgewählten Deployments prüfen.
   - Grund: Bereits produktiv unterstützte Alternative via `SecAuditLogFormat NATIVE`.
   - Einschränkung: HTTPS-Auditwriter bleibt JSON-gebunden.
   - Evidenzgrad: **Hoch**.

2. **NDJSON-ähnliche Audit-Ausgabe als evolutive Option** prüfen.
   - Grund: `toJSON()` liefert pro Ereignis JSON plus Newline; Writer appenden Logs.
   - Einschränkung: Keine explizite NDJSON-Deklaration/Tests/Schnittstellen im Repo.
   - Evidenzgrad: **Mittel** (Code-Indizien vorhanden, aber nicht als offizielles Format spezifiziert).

### Gute Kandidaten für Umstellung

1. **Auditlog-Dateiausgabe (nicht HTTPS)** als Pilot auf NATIVE, wenn JSON dort nicht benötigt wird.
   - Grund: Keine Codeänderung nötig, nur Konfigurationswahl.
   - Evidenzgrad: **Hoch**.

2. **Interne Collection-Serialisierung (`CollectionData`)**: gezielte Härtung/Migration innerhalb des bestehenden Schemas prüfen.
   - Grund: Aktuelle JSON-ähnliche Serialisierung ist handgebaut und hat Legacy-Fallback; Umstellungen sind intern kapselbar.
   - Wichtig: Ersatzformat selbst ist im Repo nicht vorimplementiert.
   - Evidenzgrad: **Mittel**.

### Nicht sinnvoll umzustellen

1. **HTTPS-Audit-Export weg von JSON** derzeit nicht sinnvoll.
   - Grund: Der Writer setzt fest `application/json` und sendet `toJSON()`.
   - Evidenzgrad: **Hoch**.

2. **Testformat vollständig weg von JSON** derzeit nicht sinnvoll.
   - Grund: Große bestehende Suite + YAJL-Loader/Formatter + `.json`-Filterlogik.
   - Evidenzgrad: **Hoch**.

3. **Einführung von Protobuf/MessagePack/CBOR/Avro/Parquet für bestehende JSON-Pfade** derzeit nicht sinnvoll.
   - Grund: Keine nachweisbaren Dependencies, Reader/Writer, Schemas oder Tests.
   - Evidenzgrad: **Hoch** (für den Befund „nicht vorhanden“), **Niedrig** (für Nutzenbehauptungen außerhalb Repo).

## 7. Offene Punkte

- Es konnte **nicht belegt werden**, dass im Repository Benchmarks existieren, die JSON gegenüber Alternativen für die konkret betroffenen Pfade messen.
- Es konnte **nicht belegt werden**, dass externe API-Verträge (außer den im Code sichtbaren) bestimmte Austauschformate erzwingen.
- Es konnte **nicht belegt werden**, dass Migrationswerkzeuge für Testfälle von JSON auf andere Formate vorhanden sind.
- Es konnte **nicht belegt werden**, dass für YAML/TOML/Protobuf/MessagePack/CBOR/Avro/Parquet bereits interne Schema- oder Validierungsartefakte existieren.

## 8. Anhang

Relevante Fundstellen (Auszug):

- **JSON-Abhängigkeit / Build**
  - `configure.ac` (`PROG_YAJL`, `YAJL_FOUND`)
  - `README.md` (YAJL als mandatory dependency für JSON-Logs und Tests)
  - `src/Makefile.am`, `test/Makefile.am` (YAJL Flags/Libraries)

- **Request-Body JSON**
  - `src/request_body_processor/json.cc` (YAJL-Callbacks, parse/complete, depth handling)
  - `src/request_body_processor/json.h` (`setMaxDepth`)
  - `src/transaction.cc` (`processRequestBody`, JSON-Zweig, Fehlerpfade)
  - `src/actions/ctl/request_body_processor_json.cc`
  - `src/parser/seclang-scanner.ll` / `src/parser/seclang-parser.yy` (`SecRequestBodyJsonDepthLimit`)
  - `test/test-cases/regression/request-body-parser-json.json`

- **Alternative Body-Parser im Repo**
  - `src/request_body_processor/xml.cc`, `src/request_body_processor/xml.h`
  - `src/actions/ctl/request_body_processor_xml.cc`
  - `src/actions/ctl/request_body_processor_urlencoded.cc`

- **Auditlogs**
  - `headers/modsecurity/audit_log.h` (AuditLogFormat Enum)
  - `src/parser/seclang-parser.yy` (`SecAuditLogFormat JSON|NATIVE`)
  - `src/audit_log/writer/serial.cc`, `src/audit_log/writer/parallel.cc`, `src/audit_log/writer/https.cc`
  - `src/transaction.cc` (`toJSON`, `toOldAuditLogFormat`)
  - `test/test-cases/regression/auditlog.json`

- **JSON-Ergebnisobjekte / API**
  - `headers/modsecurity/modsecurity.h` (`processContentOffset` Signatur)
  - `src/modsecurity.cc` (`processContentOffset` JSON-Erzeugung)

- **Tests/Fixtures**
  - `test/common/modsecurity_test.cc` (lädt `.json`, YAJL-Parse)
  - `test/regression/regression_test.cc` (JSON-Serialisierung für Formatierung)
  - `test/regression/regression.cc` (Formatierungsmodus für JSON-Dateien)
  - `test/test-suite.in` (JSON-Testdateiliste)
  - `test/test-cases/**/*.json`

- **Interne Persistenz**
  - `src/collection/backend/collection_data.cc` (JSON-ähnliche Serialisierung + Legacy-Fallback)
  - `src/collection/backend/lmdb.cc` (Nutzung serialisierter Werte)
