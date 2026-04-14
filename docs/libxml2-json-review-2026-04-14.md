# libxml2- und JSON-Review (Stand: 2026-04-14)

Dieses Dokument fasst den technischen Befund im Repository und den Abgleich mit aktuellen öffentlichen Quellen zusammen.

## 1) Executive Summary

- **Repo-Befund:** libxml2 ist in ModSecurity funktional relevant für XML-Request-Body-Parsing, XPath-Variablenzugriff sowie DTD/XSD-Validierungsoperatoren. Build-seitig ist die Abhängigkeit optional, aber breit integriert (Autotools, Windows-CMake/Conan, Tests/Fuzzer).
- **Version-Lage im Repo:**
  - Autotools-Mindestversion ist historisch niedrig (**2.6.29**).
  - Windows/Conan pinnt **2.12.6**.
- **Externer Stand (öffentlich):** Aktuelle Release-Linie ist **2.15.x**, mit **2.15.2** als neuestem Stand (März 2026), inkl. mehrerer Security-Fixes.
- **Kernlücke:** Keine harte, moderne Mindestversion im Haupt-Build; die Security-Härtung erfolgt nicht über `XML_PARSE_NO_XXE`, sondern über einen global registrierten I/O-Callback (`xmlParserInputBufferCreateFilenameDefault`), der upstream als deprecated dokumentiert ist.
- **JSON-Integration:** JSON ist bereits über eine eigene Backend-Abstraktion (simdjson/jsoncons) von XML getrennt. libxml2 ist **nicht** der primäre Engpass für die Integration einer weiteren JSON-Bibliothek.

## 2) Befunde im Repo

### 2.1 Belegt im Repo

#### Nutzungspunkte von libxml2

- Initialisierung/Shutdown zentral in `ModSecurity`:
  - `xmlInitParser()` im Konstruktor, `xmlCleanupParser()` im Destruktor.
- XML-Request-Body-Parsing via Push-Parser (`xmlCreatePushParserCtxt`, `xmlParseChunk`) in `src/request_body_processor/xml.cc`.
- XPath-Auswertung für `XML`-Variablenzugriffe in `src/variables/xml.cc`.
- DTD-Validierung (`xmlParseDTD`, `xmlValidateDtd`) in `src/operators/validate_dtd.cc`.
- XSD-Validierung (`xmlSchema*`) in `src/operators/validate_schema.cc`.

#### Build-/Dependency-Einbindung

- Autotools-Makro `CHECK_LIBXML2` mit Mindestversion `2.6.29`.
- Windows-Conan pinnt `libxml2/2.12.6`.
- Windows-Doku nennt ebenfalls `libxml2 2.12.6`.

#### Security-relevantes Parser-Verhalten

- Externe Entity-Ladung wird in `XML::init()` über globale Callback-Registrierung gesteuert:
  - bei `SecXMLExternalEntity On`: Default-Lader aktiv,
  - sonst Callback, der immer `nullptr` liefert (blockiert externe Entitäten).
- Parseroptionen setzen aktuell primär `XML_PARSE_NOWARNING | XML_PARSE_NOERROR`.
- Es wird **nicht** sichtbar `XML_PARSE_NO_XXE` gesetzt.

#### Tests/Fuzzing

- Regression-Tests referenzieren `libxml2` explizit als benötigte Resource.
- Ein dedizierter XXE-Testfall (`config-xml_external_entity.json`) existiert.
- Fuzzer-Build linkt `$(LIBXML2_LDADD)`.

#### JSON-Befund im Repo

- JSON-Backend ist wählbar (`simdjson` oder `jsoncons`) via `--with-json-backend`.
- `JSONAdapter` kapselt Backendwahl über Compile-Defines.
- JSON-Verarbeitung läuft in eigener RequestBodyProcessor-Klasse (`JSON`), getrennt vom XML-Prozessor (`XML`).

### 2.2 Schlussfolgerung aus Repo-Befund

- libxml2 ist für XML-Features **kritisch**, aber **modular optional** (Compile-Flag `WITH_LIBXML2`).
- JSON-Pfad ist technisch bereits entkoppelt; XML und JSON teilen sich primär nur den Dispatcher in `Transaction::processRequestBody`.
- Der heikelste Punkt in der aktuellen XML-Härtung ist die Nutzung einer **globalen** (und upstream deprecated) Loader-Umschaltung.

### 2.3 Offen / unklar / nicht verifizierbar

- Welche libxml2-Version produktiv auf Linux tatsächlich zur Laufzeit genutzt wird, ist aus dem Repo allein nicht feststellbar.
- Ob Distributionen lokale Backports einspielen, ist ohne konkrete Zielplattform nicht verifizierbar.

## 3) Aktueller externer Stand zu libxml2

### 3.1 Belegt durch aktuelle externe Quellen

- GNOME-Release-Archiv listet für libxml2 die Reihe **2.15** mit `LATEST-IS-2.15.2` (Datum 2026-03-04).
- 2.15.0 nennt u. a.:
  - Entfernen des eingebauten HTTP-Clients,
  - Entfernen von LZMA-Support,
  - geänderte Build-Anforderungen (Docs/Doxygen),
  - geplante weitere Entfernungen.
- 2.14.0 nennt API/ABI-relevante Änderungen:
  - Binärkompatibilität nur für 2.14+,
  - SONAME-Bump von `libxml2.so.2` auf `libxml2.so.16` (ELF).
- 2.15.2 nennt mehrere Security-Fixes, inklusive CVE-IDs (z. B. CVE-2026-1757, CVE-2026-0990, CVE-2026-0992, CVE-2026-0989) laut offiziellen Release Notes.
- 2.12.10 (Feb 2025) adressiert laut Release Notes u. a. CVE-2025-24928 und CVE-2024-56171.
- NVD bestätigt für CVE-2025-24928: betroffen sind Versionen vor 2.12.10 bzw. 2.13.x vor 2.13.6.
- Aktuelle libxml2-API-Doku (`parser.h`) empfiehlt bei untrusted Daten `XML_PARSE_NO_XXE`; außerdem ist dokumentiert, dass `XML_PARSE_NONET` seit Wegfall der eingebauten Netz-Clients in 2.15 praktisch keine Wirkung mehr hat (außer Weitergabe an Custom Loader).
- `xmlParserInputBufferCreateFilenameDefault()` ist in aktueller API-Doku als deprecated markiert; empfohlen werden kontextbezogene Loader-APIs (`xmlCtxtSetResourceLoader` o. ä.).

### 3.2 Schlussfolgerung aus externem Stand

- Das libxml2-Ökosystem bewegt sich schnell (2.14/2.15 mit relevanten API/Build-/Security-Änderungen).
- Alte Mindestversionsgrenzen (2.6.x) sind nicht mehr zeitgemäß als Security-Basis.

### 3.3 Offen / unklar / nicht verifizierbar

- Ob alle in 2.15.2 genannten CVE-Einträge bereits vollständig in allen öffentlichen Datenbanken normalisiert/enriched sind, ist nicht vollständig verifizierbar.

## 4) Gap-Analyse: Repo vs. aktueller Stand

## Belegt im Repo
- Mindestversion 2.6.29 (Autotools), Windows-Pin 2.12.6.
- XML-Schutz gegen externe Entitäten über globalen Callback-Schalter, nicht über `XML_PARSE_NO_XXE`.

## Belegt extern
- Aktueller Stand 2.15.2 mit Security-Fixes und API/Build-Änderungen.
- Deprecation des im Repo verwendeten globalen Callback-Mechanismus.

## Schlussfolgerung / Empfehlung
- Es gibt eine reale Modernisierungslücke, v. a. bei:
  1) Versionspolicy,
  2) Security-Optionierung,
  3) Nutzung deprecated globaler Loader-Hooks.

## 5) Einfluss auf die Integration einer neuen JSON-Bibliothek

### Belegt im Repo
- JSON ist bereits über `JSONAdapter` von XML entkoppelt.
- Backend-Auswahl für JSON existiert (`simdjson`/`jsoncons`).

### Schlussfolgerung / Empfehlung
- Ein libxml2-Upgrade ist **nicht Voraussetzung**, um eine neue JSON-Bibliothek technisch zu integrieren.
- Für saubere Gesamtarchitektur ist eher eine gemeinsame Parsing-Abstraktion (einheitliche Fehler-/Limits-/Telemetry-Schnittstelle) relevant als ein Austausch von libxml2.

## 6) Klare Entscheidungsantwort

**Teilweise: libxml2 kann bleiben, aber die Einbindung sollte modernisiert werden.**

Begründung:
- **Ja zur Modernisierung** wegen Security-/Wartbarkeitsaspekten (deprecated Loader-Hook, veraltete Mindestversion, fehlende explizite NO_XXE-Optionierung).
- **Nein zur These „nur wegen JSON muss libxml2 raus“**: JSON-Pfade sind bereits gekapselt; der Integrationsengpass liegt nicht primär in libxml2.

## 7) Priorisierte Maßnahmen

### Kurzfristig (niedriger bis mittlerer Eingriff)

1. **Versions-Policy anheben**
   - Mindestversion für libxml2 im Build auf einen sicher gepflegten Bereich anheben (z. B. 2.12.10+ oder 2.14+ je Plattformstrategie).
   - Warum: reduziert Risiko bekannter Schwachstellen auf Altversionen.

2. **Explizite XXE-Härtung ergänzen**
   - Zusätzlich zu bestehender Logik `XML_PARSE_NO_XXE` (wo verfügbar) setzen.
   - Warum: dokumentierter Best-Practice-Mechanismus für untrusted XML.

3. **Tests erweitern**
   - Regressionen für XXE/DTD/XInclude/Entity-Edgecases in Matrix über mehrere libxml2-Versionen.

### Mittelfristig (mittlerer Eingriff)

4. **Globalen Loader-Hook ablösen**
   - Migration von `xmlParserInputBufferCreateFilenameDefault` zu kontextbezogenen Loader-APIs.
   - Warum: deprecated API, globaler Schalter ist fehleranfällig in Multi-Thread-/Multi-Request-Szenarien.

5. **Build-Matrix harmonisieren**
   - Linux/Windows auf konsistentere libxml2-Zielversionen bringen; Policy dokumentieren.

### Langfristig (mittlerer bis höherer Eingriff)

6. **Parser-Abstraktion vereinheitlichen (XML/JSON)**
   - Gemeinsame Schicht für Limits, Fehlernormalisierung, Telemetrie, Cancellation.
   - Nutzen: Neue JSON-Bibliotheken und XML-Änderungen lassen sich mit geringerem Risiko integrieren.

## 8) Offene Unsicherheiten / nicht verifizierbare Punkte

- Reale Produktionsversionen pro Zielplattform: **Nicht verifizierbar.**
- Exakte Laufzeit-ABI-Risiken in allen Downstream-Packages ohne Zielumgebungen: **Nicht verifizierbar.**

## 9) Quellen

### Repository-Quellen
- `build/libxml.m4`
- `build/win32/conanfile.txt`
- `build/win32/README.md`
- `src/modsecurity.cc`
- `src/transaction.cc`
- `src/request_body_processor/xml.cc`
- `src/variables/xml.cc`
- `src/operators/validate_dtd.cc`
- `src/operators/validate_schema.cc`
- `src/request_body_processor/json_adapter.cc`
- `configure.ac`
- `test/test-cases/regression/config-xml_external_entity.json`
- `test/regression/regression.cc`
- `test/fuzzer/Makefile.am`

### Externe Primärquellen
- GNOME libxml2 Release-Archiv (Index):
  - https://download.gnome.org/sources/libxml2/
  - https://download.gnome.org/sources/libxml2/2.15/
  - https://download.gnome.org/sources/libxml2/2.14/
  - https://download.gnome.org/sources/libxml2/2.12/
- Release Notes:
  - https://download.gnome.org/sources/libxml2/2.15/libxml2-2.15.2.news
  - https://download.gnome.org/sources/libxml2/2.15/libxml2-2.15.0.news
  - https://download.gnome.org/sources/libxml2/2.14/libxml2-2.14.0.news
  - https://download.gnome.org/sources/libxml2/2.14/libxml2-2.14.5.news
  - https://download.gnome.org/sources/libxml2/2.14/libxml2-2.14.6.news
  - https://download.gnome.org/sources/libxml2/2.12/libxml2-2.12.10.news
- Offizielle API-Doku:
  - https://gnome.pages.gitlab.gnome.org/libxml2/html/parser_8h.html
  - https://gnome.pages.gitlab.gnome.org/libxml2/html/xmlIO_8h.html
- NVD (CVE-Referenz):
  - https://nvd.nist.gov/vuln/detail/CVE-2025-24928
