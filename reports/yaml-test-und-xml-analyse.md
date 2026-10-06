# Analyse: YAML für Tests und stärkere XML-Nutzung

## 1. Vorgehen

Im Repository nachweisbar ist folgende Prüfung:

- Repo-weite Suche nach Dateiendungen und Begriffen (`.json`, `.yaml`, `.yml`, `.xml`, `yajl`, `libxml`, `request_body_processor`, `requestBodyProcessor`, `load_test_json`, `from_yajl_node`, `toJSON`, `SecXmlExternalEntity`, `SecParseXmlIntoArgs`).
- Fokus auf:
  - Testframework-Code (`test/common`, `test/regression`, `test/unit`)
  - Request-Body-Processor (`src/request_body_processor/*.cc`)
  - Regelparser/Scanner (`src/parser/seclang-parser.yy`, `src/parser/seclang-scanner.ll`)
  - XML-bezogene Variablen/Operatoren (`src/variables/xml.cc`, `src/operators/validate_*.cc`)
  - Build-/Dependency-Dateien (`configure.ac`, `test/Makefile.am`, `src/Makefile.am`)
- Quantitative Checks:
  - `test` enthält 196 `.json`-Dateien, 0 `.yaml`, 0 `.yml`.

**Evidenzgrad:** Hoch (direkt aus Code-/Dateifunden).

## 2. Nachweisbare Ausgangslage im Repository

### 2.1 Testframework und Testdaten

Im Repository nachweisbar ist:

- Testdaten liegen in JSON-Dateien (`test/test-suite.in` listet `.json`-Tests explizit).
- Der generische Testloader heißt `load_test_json(...)` und parst mit `yajl_tree_parse(...)`.
- Discovery in `load_tests(...)` filtert hart auf Suffix `.json`.
- Regressionstest- und Unittest-Modelle werden aus YAJL-Knoten aufgebaut (`from_yajl_node(...)`).

Relevante Stellen:

- `test/common/modsecurity_test.h` – `bool load_test_json(const std::string &file);`
- `test/common/modsecurity_test.cc` – `.json`-Suffixprüfung und `yajl_tree_parse`
- `test/regression/regression_test.h/.cc` – `from_yajl_node`, `toJSON`
- `test/unit/unit_test.h/.cc` – `from_yajl_node`
- `test/test-suite.in` – statische JSON-Testliste

**Evidenzgrad:** Hoch.

### 2.2 JSON-Abhängigkeit der Tests

Im Repository nachweisbar ist:

- Die Loader-/Parser-Einstiegspunkte sind YAJL-gebunden (`yajl_val`, `YAJL_GET_*`, `yajl_tree_parse`).
- Der Formatierer im Regression-Runner schreibt JSON (`tests[0]->toJSON()`), und der CLI-Hinweis spricht explizit von „format test case JSON files“.
- Buildlogik aktiviert Test-Utilities nur, wenn YAJL gefunden ist (`buildTestUtilities` abhängig von `YAJL_FOUND`).

Folge (repo-basiert):

- JSON ist nicht nur Dateiendung, sondern durchgehend in Loadern, Datenextraktion und Formatterpfad kodiert.

**Evidenzgrad:** Hoch.

### 2.3 Vorhandene XML-Unterstützung

Im Repository nachweisbar ist:

- XML-Request-Body-Processor existiert (`RequestBodyProcessor::XML`) und ist in `Transaction` integriert.
- Aktivierungspfad über `ctl:requestBodyProcessor=XML` (Scanner/Parser + Action-Klasse `RequestBodyProcessorXML`).
- XML-Konfigurationsdirektiven existieren:
  - `SecXmlExternalEntity` (`CONFIG_XML_EXTERNAL_ENTITY`)
  - `SecParseXmlIntoArgs` (`CONFIG_XML_PARSE_XML_INTO_ARGS`)
- XML-Variablenauswertung per XPath ist implementiert (`variables::XML::evaluate`).
- XML-Validierungsoperatoren sind implementiert:
  - `@validateSchema` (`ValidateSchema`)
  - `@validateDTD` (`ValidateDTD`)
- Umfangreiche Regressionstests für XML-Pfade existieren (`request-body-parser-xml*.json`, `variable-XML.json`, `config-xml_external_entity.json`, `action-xmlns.json`).

**Evidenzgrad:** Hoch.

### 2.4 Relevante Build- und Dependency-Stellen

Im Repository nachweisbar ist:

- YAJL: `configure.ac` (`PROG_YAJL`), Tests bauen nur mit YAJL (`YAJL_FOUND`), `src/Makefile.am` und `test/Makefile.am` linken YAJL.
- LibXML2: `configure.ac` (`CHECK_LIBXML2`), `src/Makefile.am` und `test/Makefile.am` linken LIBXML2.
- YAML-Library/Dependency konnte nicht belegt werden (`libyaml`, `yaml-cpp`, `yaml`-Treffer fehlen in Builddateien).

**Evidenzgrad:** Hoch.

## 3. Prüfung: YAML als Testfallformat

### 3.1 Derzeitiger JSON-gebundener Testpfad

Im Repository nachweisbar ist:

1. **Dateifilter**: `load_tests(...)` akzeptiert nur Dateien mit `.json`.
2. **Parse-Einstieg**: `load_test_json(...)` verwendet `yajl_tree_parse`.
3. **Model-Befüllung**: `T::from_yajl_node(...)` in Unit/Regression.
4. **Formatter-Pfad**: Regression-Formatter schreibt JSON über `RegressionTests::toJSON()`.
5. **Testsuite-Discovery (Automake)**: `test/test-suite.in` enthält eine statische JSON-Liste.

**Evidenzgrad:** Hoch.

### 3.2 Welche Stellen für YAML angepasst werden müssten

Im Repository nachweisbar ist, dass mindestens folgende Stellen betroffen wären:

- `test/common/modsecurity_test.h/.cc`
  - neuer formatneutraler Loader oder zusätzlicher YAML-Loader
  - Erweiterung der Dateifilterlogik (`.yaml`/`.yml`)
- `test/regression/regression_test.h/.cc`
  - zusätzlicher Parserpfad neben `from_yajl_node(...)` (oder neue Adapterschicht)
  - optionaler YAML-Writer, falls der bisherige `format`-Modus YAML ausgeben soll
- `test/unit/unit_test.h/.cc`
  - zusätzlicher Parserpfad neben `from_yajl_node(...)`
- `test/regression/regression.cc`
  - `format`-Modus ist derzeit JSON-zentriert (Ausgabe/CLI-Text)
- `test/test-suite.in`
  - Discovery-Liste bzw. Generierung berücksichtigt aktuell `.json`
- Buildsystem (`configure.ac`, `test/Makefile.am`, ggf. `src/Makefile.am`)
  - falls YAML-Parserlibrary eingeführt wird: neue Dependency-Checks, CFLAGS/LDFLAGS/LDADD

**Evidenzgrad:** Hoch.

### 3.3 Welche Funktionen konkret gebraucht würden

Im Repository nachweisbar ist (aus aktueller Struktur ableitbar), dass für YAML-Unterstützung folgende konkrete Erweiterungspunkte nötig wären:

- **Loader/Discovery**
  - Neue Funktion analog `load_test_json(...)`, z. B. `load_test_yaml(...)`.
  - Erweiterung `load_tests(...)` für `.yaml`/`.yml`.
- **Formatneutrale Modell-Befüllung (Adapter)**
  - Derzeit: `RegressionTest::from_yajl_node(...)`, `RegressionTests::from_yajl_node(...)`, `UnitTest::from_yajl_node(...)`.
  - Für YAML: zusätzlicher parser-spezifischer Einstieg oder gemeinsames Zwischendatenmodell.
- **Formatter/Pretty-Printer**
  - Derzeit nur `RegressionTests::toJSON()`.
  - Für YAML-Ausgabe wäre ein zusätzlicher Writer nötig, sofern `format`-Modus nicht JSON-only bleiben soll.
- **Build/Dependency**
  - YAML-Library-Einbindung ist derzeit nicht vorhanden; bei Einführung wären Build-Hooks nötig.

Wichtig:

- Konkrete YAML-API-Namen/Library-Funktionen sind **nicht nachweisbar im Repository**, da keine YAML-Library vorhanden ist.

**Evidenzgrad:** Mittel (Struktur ist belegt, konkrete YAML-Implementierung fehlt im Repo).

### 3.4 Vollumstellung vs. Dual-Support

Im Repository nachweisbar ist:

- 196 Test-JSON-Dateien in `test`, statische JSON-Listung in `test/test-suite.in`, JSON-Formatterpfad und YAJL-Parserpfad.

Daraus folgt repo-basiert:

- **Vollumstellung** würde breite, gekoppelte Änderungen in Discovery, Loadern, Parseradaptern, Formatter und Testdaten erfordern.
- **Dual-Support (JSON + YAML)** ist strukturell realistischer, weil bestehende JSON-Pfade erhalten bleiben können und YAML zusätzlich an Loader-Eintritt und Discovery ergänzt werden kann.

Hinweis:

- Eine exakte Aufwandszahl (Personentage/Storypoints) ist **nicht nachweisbar im Repository**.

**Evidenzgrad:** Mittel bis Hoch.

### 3.5 Risiken und offene Punkte

Im Repository nachweisbar sind folgende Risikofelder:

- **Bestehende Testdaten:** 196 JSON-Dateien + statische `.json`-Listung.
- **Parser-Fehlermeldungen:** Derzeit YAJL-Fehlertexte; bei YAML wäre anderes Fehlerverhalten zu erwarten, aber konkrete Formate sind nicht belegt.
- **Snapshot-/Vergleichslogik:** Formatter und Teile der Unit-Logik arbeiten mit JSON-zentrierten Annahmen (`toJSON`, `json2bin`).
- **Tooling/Test Runner:** `format`-Flow und CLI-Text sind auf JSON zugeschnitten.

Nicht nachweisbar im Repository:

- CI-Skripte außerhalb des Repos oder externe Pipelines, die `.json` voraussetzen.

**Evidenzgrad:** Hoch (für interne Risiken), Niedrig bis Mittel (für externe Risiken).

### 3.6 Bewertung

- **Aktuell vorhanden:** JSON-basierter End-to-End-Testpfad. **Evidenzgrad: Hoch**.
- **Mit überschaubarem Umbau realistisch:** Dual-Support (JSON beibehalten, YAML zusätzlich einführen), sofern neue Loader/Adapter/Discovery ergänzt werden. **Evidenzgrad: Mittel**.
- **Nur theoretisch möglich:** sofortige komplette YAML-Ablösung ohne Dual-Phase. **Evidenzgrad: Niedrig bis Mittel**.
- **Aktuell nicht sinnvoll:** hartes Entfernen von JSON ohne vorhandenen YAML-Parserpfad und ohne Migration der 196 Testdateien. **Evidenzgrad: Hoch**.

## 4. Prüfung: XML-Ansatz weiterverfolgen

### 4.1 Bereits vorhandene XML-Funktionen

Im Repository nachweisbar ist:

- `RequestBodyProcessor::XML` (Init, Chunk-Parsing, Complete, External-Entity-Schalter, SAX-Pfad für ARGS-Mapping).
- Aktivierung:
  - `ctl:requestBodyProcessor=XML` (Scanner/Parser + `actions::ctl::RequestBodyProcessorXML::evaluate`).
  - `SecParseXmlIntoArgs` (`On`/`Off`/`OnlyArgs`) inkl. runtime-ctl (`actions::ctl::ParseXmlIntoArgs`).
  - `SecXmlExternalEntity` (`On`/`Off`).
- XML-Auswertung:
  - `variables::XML::evaluate` (XPath + `xmlns`-Action-Unterstützung).
- XML-Validierung:
  - `ValidateSchema`, `ValidateDTD`.

**Evidenzgrad:** Hoch.

### 4.2 Real nutzbare XML-Pfade im aktuellen Repo

Im Repository nachweisbar ist:

- XML wird als echter Request-Body-Pfad verarbeitet (nicht nur Stub).
- XML kann in Variablenzugriffen (`XML:...`) und Regeln verwendet werden.
- XML-bezogene Konfigurationsdirektiven und Actions sind parserseitig integriert.
- Regressionstests decken XML-Parsing, ARGS-Mapping, DTD/Schema-Validierung und XXE-Optionen ab.

**Evidenzgrad:** Hoch.

### 4.3 Fehlende Funktionen für stärkere Nutzung

Im Repository nachweisbar bzw. nicht nachweisbar ist:

- Für Auditlog/Export gibt es keinen XML-Writer-Pfad analog `toJSON()` (nicht belegt).
- `processContentOffset` hat JSON-API-Signatur; ein XML-Ergebnispfad ist nicht belegt.
- XML-Pfad ist primär auf Request-Body-Inspection fokussiert; XML als generelles Austausch-/Persistenzformat über weitere Subsysteme ist nicht belegt.

**Evidenzgrad:** Hoch.

### 4.4 Wo XML JSON ergänzen könnte

Im Repository nachweisbar ist:

- XML kann **im Bereich Request-Body-Inspection** neben JSON genutzt werden (separater Processor, eigene Regeln/Variablen, eigene Tests).
- XML-zu-ARGS-Mapping ist vorhanden (`SecParseXmlIntoArgs` + SAX-Extraktion in `addArgument("XML", ...)`).

**Evidenzgrad:** Hoch.

### 4.5 Wo XML JSON nicht sinnvoll ersetzt

Im Repository nachweisbar ist:

- Testframework und Fixtures sind YAJL/JSON-zentriert; XML ersetzt diesen Pfad nicht direkt.
- Auditlog-JSON-Pfade (`toJSON`, HTTPS `application/json`) haben keinen XML-Gegenpfad.
- API-Rückgabe `processContentOffset(..., std::string *json, ...)` ist JSON-fixiert.

**Evidenzgrad:** Hoch.

### 4.6 Bewertung

- **Aktuell vorhanden:** belastbarer XML-Pfad für Request-Body + Regeln/Variablen/Validierung. **Evidenzgrad: Hoch**.
- **Mit überschaubarem Umbau realistisch:** weitere XML-Testabdeckung und Feinschliff innerhalb Request-Body-Inspection-Pfads. **Evidenzgrad: Mittel**.
- **Nur theoretisch möglich:** XML als breiter Ersatz für JSON in Audit/API/Testframework. **Evidenzgrad: Niedrig bis Mittel**.
- **Aktuell nicht sinnvoll:** JSON-Pfade (Tests, Audit-HTTPS, `processContentOffset`) unmittelbar durch XML ersetzen. **Evidenzgrad: Hoch**.

## 5. Konkrete Funktions- und Änderungsinventur

### 5.1 Für YAML-Testsupport

| Bereich | Datei / Symbol | Aktueller Zustand | Benötigte Änderung | Evidenzgrad |
|---|---|---|---|---|
| Loader-API | `test/common/modsecurity_test.h` / `ModSecurityTest::load_test_json` | JSON-spezifischer Loadername/-vertrag | zusätzlicher YAML-Loader oder formatneutraler Loader | Hoch |
| Datei-Discovery | `test/common/modsecurity_test.cc` / `load_tests` | Filtert auf `.json` | `.yaml`/`.yml` ergänzen (oder zentralisierte Ext-Liste) | Hoch |
| Parsing-Einstieg | `test/common/modsecurity_test.cc` / `yajl_tree_parse` | YAJL-Tree-Parse hart codiert | zusätzlicher Parserpfad für YAML + Fehlerpfad | Hoch |
| Regression-Model-Import | `test/regression/regression_test.h/.cc` / `from_yajl_node` | YAJL-Node-basiert | zusätzlicher YAML-Import oder Adapter auf gemeinsames Modell | Hoch |
| Unit-Model-Import | `test/unit/unit_test.h/.cc` / `from_yajl_node` | YAJL-Node-basiert | zusätzlicher YAML-Import oder Adapter | Hoch |
| Formatter | `test/regression/regression_test.cc` / `RegressionTests::toJSON` | JSON-Pretty-Writer vorhanden | optional `toYAML` falls Formatter YAML ausgeben soll | Mittel |
| Runner-Formatmodus | `test/regression/regression.cc` / `if (test.m_format) ... toJSON()` | Formatiert explizit JSON-Dateien | Modus auf duales Ausgabeformat erweitern oder JSON-only dokumentieren | Hoch |
| Testlisten/Automake | `test/test-suite.in` | statische `.json`-Dateiliste | YAML-Dateien zusätzlich auflisten/erzeugen | Hoch |
| Build-Dependency | `configure.ac`, `test/Makefile.am` | YAJL/LIBXML2 vorhanden, YAML fehlt | YAML-Library-Check + Link/CFLAGS (falls YAML-Parser genutzt wird) | Hoch |
| Datenmodell-Abstraktion | `RegressionTest`, `UnitTest` Klassen | Felder selbst sind weitgehend formatneutral; Import ist YAJL-gebunden | optionale Abstraktionsschicht „ParsedFixture -> Model“ | Mittel |

### 5.2 Für stärkere XML-Nutzung

| Bereich | Datei / Symbol | Aktueller Zustand | Benötigte Änderung | Evidenzgrad |
|---|---|---|---|---|
| XML-Body-Processor | `src/request_body_processor/xml.cc` / `XML::init/processChunk/complete` | Implementiert, inkl. SAX-ARGS-Pfad | Ausbau primär als Robustheits-/Coverage-Thema (keine fehlende Grundfunktion) | Hoch |
| Aktivierung per ctl | `src/actions/ctl/request_body_processor_xml.cc` | Setzt `XMLRequestBody` + `REQBODY_PROCESSOR=XML` | vorhanden; ggf. nur zusätzliche Tests/Docs | Hoch |
| XML-in-ARGS Umschaltung | `src/actions/ctl/parse_xml_into_args.cc` + `SecParseXmlIntoArgs` Parserregeln | On/Off/OnlyArgs vorhanden | vorhanden; ggf. mehr Tests für Laufzeitumschaltung | Hoch |
| XXE-Steuerung | `src/parser/seclang-parser.yy` (`SecXmlExternalEntity`) + `XML::init` | On/Off vorhanden, wirkt auf Entity-Lader | vorhanden; ggf. mehr Negativtests | Hoch |
| XML-Variablenzugriff | `src/variables/xml.cc` / `variables::XML::evaluate` | XPath + xmlns-Registration vorhanden | vorhanden; ggf. zusätzliche XPath-/Namespace-Tests | Hoch |
| Schema-/DTD-Validierung | `src/operators/validate_schema.cc`, `src/operators/validate_dtd.cc` | vorhanden | vorhanden; ggf. Fehlerszenarien stärker testen | Hoch |
| Integration in Transaktion | `src/transaction.cc` / `processRequestBody` XML-Zweig | XML-Fehler setzt `REQBODY_ERROR*` Variablen | vorhanden; ggf. erweitertes Error-Mapping nur bei Bedarf | Hoch |
| XML außerhalb Request-Inspection | Audit/API/Testframework | nicht belegt | neue Writer/API-Adapter wären nötig, derzeit nicht vorhanden | Hoch |

## 6. Entscheidungsempfehlung

### 6.1 YAML für Tests

#### Sofort sinnvoll

- **Formatneutrale Vorarbeit im Testloader (ohne JSON abzuschalten):**
  - Loader-API so vorbereiten, dass neben `load_test_json` ein zweiter Pfad ergänzt werden kann.
  - Dateifilter zentralisieren statt harter `.json`-Stelle.
- Begründung: direkt aus aktueller Struktur ableitbar, ohne bestehende JSON-Tests zu brechen.
- **Evidenzgrad:** Mittel bis Hoch.

#### Als Pilot sinnvoll

- **Dual-Support JSON + YAML** für einen kleinen Teilbereich (z. B. ausgewählte Regression-Subset-Dateien), während JSON weiterhin Standard bleibt.
- Begründung: bestehende JSON-Basis (196 Dateien) bleibt stabil, Migrationsrisiko sinkt.
- **Evidenzgrad:** Mittel.

#### Nur mit größerem Umbau

- Vollständige YAML-Migration inkl. Discovery, Loader, Parseradapter, Formatter, Testlisten und Datenkonvertierung aller JSON-Files.
- **Evidenzgrad:** Hoch (dass viele Stellen betroffen sind), Aufwandshöhe numerisch nicht nachweisbar.

#### Aktuell nicht sinnvoll

- JSON-Unterstützung sofort entfernen.
- Begründung: zentrale YAJL-Bindung in Testpfaden + statische JSON-Suite.
- **Evidenzgrad:** Hoch.

### 6.2 XML stärker verfolgen

#### Sofort sinnvoll

- XML im bestehenden Scope (Request-Body-Inspection) weiter nutzen; bestehende Features sind real vorhanden.
- **Evidenzgrad:** Hoch.

#### Als Pilot sinnvoll

- Gezielte Erweiterung von XML-Testabdeckung (Fehler-/Randfälle, Namespace/XPath-Varianten, ParseXmlIntoArgs-Varianten).
- **Evidenzgrad:** Mittel.

#### Nur mit größerem Umbau

- XML über den aktuellen Scope hinaus als Ersatzformat für Audit-Ausgaben/API-Ergebnisobjekte etablieren.
- Begründung: entsprechende Writer/API-Hooks sind nicht nachweisbar vorhanden.
- **Evidenzgrad:** Hoch.

#### Aktuell nicht sinnvoll

- XML als pauschalen Ersatz für JSON in Testframework, Audit-HTTPS und `processContentOffset` ansetzen.
- **Evidenzgrad:** Hoch.

## 7. Offene Punkte

- Es konnte nicht belegt werden, dass im Repository bereits eine YAML-Library oder YAML-Parserintegration existiert.
- Es konnte nicht belegt werden, wie externe CI/Pipelines außerhalb dieses Repositories Fixture-Formate erwarten.
- Es konnte nicht belegt werden, dass Benchmarks für JSON-vs-XML oder JSON-vs-YAML im Testframework vorliegen.
- Es konnte nicht belegt werden, dass ein bestehender Migrationskonverter JSON->YAML im Repository vorhanden ist.

## 8. Anhang

Relevante Fundstellen (Pfad, Symbol, Relevanz):

- `test/common/modsecurity_test.h` – `load_test_json`, `load_tests`; zeigt JSON-zentrierten Loadervertrag.
- `test/common/modsecurity_test.cc` – `yajl_tree_parse`, `.json`-Suffixfilter; zentraler Discovery-/Parsing-Punkt.
- `test/regression/regression_test.h` – `from_yajl_node`, `toJSON`; Parser-/Serializer-Kopplung.
- `test/regression/regression_test.cc` – YAJL-getriebene Feldextraktion, JSON-Formatter.
- `test/regression/regression.cc` – Formatierungsmodus schreibt JSON; CLI-Meldung zu JSON-Dateien.
- `test/unit/unit_test.h/.cc` – YAJL-basiertes Unit-Fixture-Parsing.
- `test/test-suite.in` – statische `.json`-Testsuite-Liste.
- `configure.ac` – `PROG_YAJL`, `CHECK_LIBXML2`, test utility gating auf `YAJL_FOUND`.
- `test/Makefile.am`, `src/Makefile.am` – YAJL/LIBXML2 Link-/Compiler-Flags.
- `src/request_body_processor/xml.cc` – XML-Parser, SAX-ARGS-Mapping, XXE-Entity-Schalter.
- `src/actions/ctl/request_body_processor_xml.cc` – Aktivierung XML-Processor.
- `src/actions/ctl/parse_xml_into_args.cc` – runtime-Umschaltung `ctl:parseXmlIntoArgs=*`.
- `src/parser/seclang-scanner.ll` / `src/parser/seclang-parser.yy` – Tokens/Regeln für `ctl:requestBodyProcessor=XML`, `SecXmlExternalEntity`, `SecParseXmlIntoArgs`.
- `src/variables/xml.cc` – XPath-Auswertung und `xmlns`-Registration.
- `src/operators/validate_schema.cc`, `src/operators/validate_dtd.cc` – XML-Validierungsoperatoren.
- `src/transaction.cc` – XML-Verarbeitungspfad in `processRequestBody` und Fehler-Variablenbehandlung.
- `test/test-cases/regression/request-body-parser-xml.json`, `request-body-parser-xml-validade-dtd.json`, `variable-XML.json`, `config-xml_external_entity.json`, `action-xmlns.json` – nachweisbare XML-Testabdeckung.
