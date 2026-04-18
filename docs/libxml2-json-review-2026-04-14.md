# libxml2 Security-/Architektur-/Dependency-Audit (Stand: 2026-04-14)

## 1. Executive Summary

### Im Repo belegt
- libxml2 wird aktiv für XML-Request-Body-Parsing, XPath-Variablen und DTD/XSD-Validierung verwendet.
- Die Integration ist buildseitig optional (`WITH_LIBXML2`), funktional aber kritisch für XML-Regeln.
- JSON ist bereits über eine klarere Backend-Abstraktion (simdjson/jsoncons + `JSONAdapter`/`JsonEventSink`) modularisiert.
- YAJL ist im C/C++-Codepfad nicht mehr nachweisbar, aber in README/CI weiterhin als Dependency referenziert.

### Extern belegt
- Upstream libxml2 steht bei 2.15.2 (März 2026).
- Offizielle API-Doku markiert die im Repo genutzte globale Loader-Umschaltung als deprecated.
- Debian/Ubuntu zeigen, dass numerisch ältere Paketstände weiterhin Security-Backports erhalten; Fedora und Homebrew liegen näher an aktuellen Upstream-Ständen.

### Schlussfolgerung
- **libxml2 sollte modernisiert werden** (Version-/Security-/API-Härtung).
- **libxml2 ist aktuell nicht der Primär-Blocker für JSON-Migration**.
- **Modularisierung von libxml2 ist sinnvoll, aber gestaffelt** (nicht als Big-Bang parallel zur JSON-Ablösung).
- **Release-/Advisory-Monitoring-Workflow ist empfehlenswert**, aber als „monitor + report + manuelle Freigabe“, nicht als blindes Auto-Upgrade.

### Unsicherheit / nicht verifizierbar
- Produktiv tatsächlich eingesetzte libxml2-Versionen außerhalb CI/Windows-Conan: **Nicht verifizierbar.**

---

## 2. Repo-Befunde zu libxml2

### Im Repo belegt

#### Code-Nutzung
- Globale Initialisierung/Shutdown: `xmlInitParser()`/`xmlCleanupParser()` in `src/modsecurity.cc`.
- Request-Body-XML-Parsing: Push-Parser + SAX in `src/request_body_processor/xml.cc`.
- XPath-Auswertung: `src/variables/xml.cc`.
- DTD-Validierung: `src/operators/validate_dtd.cc`.
- XSD-Validierung: `src/operators/validate_schema.cc`.
- Dispatcher XML/JSON: `src/transaction.cc`.

#### Sicherheitsrelevante Befunde
- Externe Entitäten werden über `SecXMLExternalEntity` via globalen Loader-Callback gesteuert (`xmlParserInputBufferCreateFilenameDefault`).
- Parseroptionen setzen sichtbar `XML_PARSE_NOWARNING | XML_PARSE_NOERROR`.
- Sichtbarer Einsatz von `XML_PARSE_NO_XXE` im XML-Body-Parserpfad: nicht belegt.

#### Build/Dependency
- Autotools-Mindestversion für libxml2: `2.6.29`.
- Windows-Conan pinnt `libxml2/2.12.6`.
- CI testet mit und ohne `--without-libxml`.

#### Tests/Fuzzing
- Mehrere Regressionsfälle mit Resource-Gate `libxml2`.
- Expliziter XXE-Regressionsfall (`config-xml_external_entity.json`).
- Fuzzer-Linking enthält `$(LIBXML2_LDADD)`.

### Extern belegt
- Keine externen Quellen für diese Repo-internen Fakten erforderlich.

### Schlussfolgerung
- libxml2 ist für XML-Features technisch zentral.
- Sicherheitskontrolle ist vorhanden, aber über global/deprecated Mechanismus statt moderner, kontextbezogener API.
- Versionierungsstrategie wirkt inkonsistent (sehr alte Mindestgrenze vs. neuerer Windows-Pin).

### Unsicherheit / nicht verifizierbar
- Thread-Safety-Auswirkungen des globalen Loader-Umschaltens unter realer Parallelität sind aus dem Repo allein **nicht vollständig verifizierbar**.

---

## 3. Repo-Befunde zur JSON-Migration (YAJL → neue Bibliothek)

### Im Repo belegt
- `configure.ac` unterstützt `--with-json-backend=simdjson|jsoncons`.
- JSON-Backends sind separat implementiert (`json_backend_simdjson.cc`, `json_backend_jsoncons.cc`) und über `JSONAdapter` abstrahiert.
- `JsonEventSink`/`JsonParseResult` bilden ein konsistentes Contract-Modell.
- Es existieren dedizierte Backend-Tests (`json_backend_depth_tests`) und eine Matrix-Ausführung (`test/run-json-backend-matrix.sh`).
- YAJL-Nutzung im C/C++-Codepfad: durch Suche nach `yajl`/YAJL-Includes nicht belegt.
- YAJL-Referenzen verbleiben in README und CI-Paketinstallation.

### Extern belegt
- Nicht erforderlich für Repo-Befund.

### Schlussfolgerung
- Die neue JSON-Schicht ist klar modularer als XML.
- Der Zustand ist konsistent mit einer laufenden/weit fortgeschrittenen Ablösung von YAJL im Kerncode, bei verbleibenden Dokumentations-/CI-Resten.

### Unsicherheit / nicht verifizierbar
- Exakter Migrationsstatus in nicht sichtbaren Branches/Downstreams: **Nicht verifizierbar.**

---

## 4. Aktueller externer Stand zu libxml2

### Extern belegt
- GNOME Release-Index führt libxml2 2.15.x, inklusive `LATEST-IS-2.15.2`.
- 2.14.0 Release Notes: SONAME-Änderung (`libxml2.so.2` → `libxml2.so.16`) und ABI-Hinweise.
- 2.15.0 Release Notes: u. a. Wegfall built-in HTTP/LZMA-Komponenten.
- 2.15.2 Release Notes: mehrere Security-Fixes/CVE-Referenzen.
- 2.12.10 Release Notes enthalten u. a. CVE-2025-24928 / CVE-2024-56171.
- NVD führt CVE-2025-24928 mit betroffenen älteren Versionen.
- Offizielle API-Doku:
  - `xmlParserInputBufferCreateFilenameDefault` deprecated (`xmlIO_8h`).
  - `XML_PARSE_NO_XXE` als relevante Sicherheitsoption (`parser_8h`).

### Im Repo belegt
- Nicht anwendbar.

### Schlussfolgerung
- Upstream entwickelt sich sicherheits- und API-seitig aktiv; mittelfristiges „stehen bleiben“ erhöht Wartungs- und Security-Risiko.

### Unsicherheit / nicht verifizierbar
- Vollständige CVE-Enrichment-Konsistenz über alle Datenbanken am selben Tag: **Nicht verifizierbar.**

---

## 5. Linux- und macOS-Versions-/Packaging-Vergleich

### Extern belegt

#### Linux
- Debian Security Tracker zeigt für stabile Releases numerisch ältere Versionsstände mit separaten Security-Updates/DSA/DLA (Backport-Modell).
- Ubuntu Paket-/USN-Seiten zeigen ebenfalls ältere Versionsnummern mit Security-Notices/Updates.
- Fedora-Paketseite listet deutlich neuere Stände (z. B. 2.12.10 in Fedora-Releases laut gelisteter Übersicht).

#### macOS
- Homebrew `libxml2`-API zeigt `stable: 2.15.2`, `keg_only` mit Grund `provided_by_macos`.
- Das belegt zugleich die Trennung zwischen systembereitgestellter libxml2 und explizit installierter Homebrew-Variante.

### Im Repo belegt
- CI installiert auf Linux `libxml2-dev` (APT) und auf macOS `brew install libxml2`.

### Schlussfolgerung
- Reiner Versionsnummernvergleich reicht nicht: Debian/Ubuntu können numerisch ältere, aber security-gepflegte Pakete liefern.
- Für reproduzierbare Sicherheitsbewertung muss das Projekt klarer festlegen, ob Systempakete, Homebrew/Conan oder pin-basierte Vendor-Strategie maßgeblich sind.

### Unsicherheit / nicht verifizierbar
- Exakte Backport-Abdeckung aller relevanten CVEs je Distribution/Release ohne vollständige Advisory-Matrix: **Nicht verifizierbar.**

---

## 6. Sicherheitsbewertung

### Im Repo belegt
- XML-Eingaben sind untrusted Request-Body-Daten.
- Schutz gegen externe Entitäten basiert aktuell auf globalem Loader-Override.
- `XML_PARSE_NO_XXE`-Setzung im gezeigten XML-Body-Pfad ist nicht belegt.
- XML und JSON laufen beide über denselben Request-Body-Dispatcher, aber mit separaten Prozessoren.

### Extern belegt
- Deprecated-Status des globalen Loader-Mechanismus.
- Vorhandensein aktueller Sicherheitsfixes in neueren libxml2-Releases.

### Schlussfolgerung
- Sicherheit ist **teilweise abgesichert**, aber nicht auf dem robustesten verfügbaren Mechanismus.
- Zentralisierte Sicherheitsdefaults in einer XML-Fassade würden das Risiko inkonsistenter Parser-Konfiguration senken.

### Unsicherheit / nicht verifizierbar
- Ob alle Angriffsvektoren (insb. Race-/Global-State-Randfälle) unter Produktionslast abgedeckt sind: **Nicht verifizierbar.**

---

## 7. Architekturvergleich: libxml2 vs. neue JSON-Schicht

### Im Repo belegt
- JSON: klarer Backend-Contract (`JsonEventSink`, Result-Typen), Adapter (`JSONAdapter`), austauschbare Implementierungen, dedizierte Backendspezifik-Tests.
- XML: direkte libxml2-Aufrufe in mehreren Fachkomponenten ohne zentrales XML-Backend-Interface.

### Extern belegt
- Nicht erforderlich.

### Schlussfolgerung
- JSON ist architektonisch stärker entkoppelt.
- Das JSON-Muster ist ein konkreter Referenzentwurf für ein XML-Modul (Facade/Adapter/Options/Result-Contract).

### Unsicherheit / nicht verifizierbar
- Netto-Performanceeffekt einer XML-Adapter-Schicht ohne Benchmarks: **Nicht verifizierbar.**

---

## 8. Bewertung: libxml2 als Modul integrieren oder nicht

### Kategorie
**Ja, aber nur in einem gestaffelten Ansatz.**

### Im Repo belegt
- XML ist aktuell weniger modular als JSON.
- Direkte libxml2-Nutzung existiert an mehreren Stellen.

### Extern belegt
- Upstream/API-Situation (deprecated global loader + laufende Security-Fixes) stützt die Notwendigkeit zentraler Kontrolle.

### Schlussfolgerung
- Modulgrenze bringt Vorteile bei Wartbarkeit, Testbarkeit, Sicherheitsstandardisierung und späteren Upgrades.
- Parallel als Big-Bang zur JSON-Migration wäre unnötig riskant; sinnvoll ist eine sequenzierte Einführung.

### Unsicherheit / nicht verifizierbar
- Konkreter Refactoring-Aufwand bis auf Funktions-/Dateiebene ohne Spike-Implementierung: **Nicht verifizierbar.**

---

## 9. Bewertung: Ist direkte Modulbildung sicher

### Im Repo belegt
- Es gibt aktuell globale libxml2-Steuerungspunkte und direkte API-Aufrufe in Fachcode.

### Extern belegt
- Deprecated API-Hinweise unterstützen eine Umstellung auf zentral kontrollierte, kontextbezogene Ressourcen-/Parserkonfiguration.

### Schlussfolgerung
- **Direkte Modulbildung ist nicht automatisch „sicher“, aber kann sicherer werden**, wenn Guardrails verpflichtend sind:
  1. Zentrale Parser-Defaults (inkl. XXE-/Resource-Policy) im Modul.
  2. Harte Limits (Depth/Size/Entity-Policy) im Modul-Contract.
  3. Einheitliche Fehlerklassifikation und Auditierbarkeit.
  4. Explizite Trennung trusted/untrusted Inputs.
  5. Verbot neuer direkter libxml2-Aufrufe außerhalb des Moduls (CI-Lint/Codeowners).

### Unsicherheit / nicht verifizierbar
- Ohne diese Guardrails ist die Aussage „Modulbildung ist sicher genug“ **nicht verifizierbar**.

---

## 10. Bewertung: Release-/Security-Monitoring-Workflow

### Im Repo belegt
- CI testet Build-Varianten, aber ein dediziertes Release-/Advisory-Monitoring für libxml2 + JSON-Backend ist nicht als eigener Workflow ersichtlich.

### Extern belegt
- Upstream-Releases (libxml2, simdjson/jsoncons) ändern sich regelmäßig.
- Distributionen liefern teils Backports, daher reicht „nur Upstream-Version vergleichen“ nicht.

### Schlussfolgerung
**Empfehlung: beobachten + reporten + manuell freigegebene Updates.**

- **Release Monitoring:** Ja (Upstream libxml2 + verwendetes JSON-Backend).
- **Security Advisory Monitoring:** Ja (CVE/NVD + distro advisories).
- **Automatisches Upgrade:** Nein, nicht blind.
- **Manuell freigegebener Update-Prozess:** Ja.

Pragmatisches Modell:
1. Geplanter Workflow (z. B. wöchentlich) sammelt Versionen/Advisories.
2. Erzeugt Report + optional GitHub Issue/PR-Kommentar.
3. Menschen entscheiden über Upgrade/Patchbackport nach Testmatrix.

### Unsicherheit / nicht verifizierbar
- Konkretes Signal-zu-Rauschen-Verhältnis ohne Pilotlauf: **Nicht verifizierbar.**

---

## 11. Klare Gesamtentscheidungen

1. **Soll libxml2 aktualisiert oder erneuert werden?**
   - **Ja, Aktualisierung/Modernisierung empfohlen.**
   - Belege: Upstream 2.15.2, Security-Fixes, deprecated API-Hinweise, Repo nutzt alte Mindestgrenze + globalen Mechanismus.
   - Hauptrisiko: Migrations-/Kompatibilitätsaufwand.
   - Unsicherheit: konkrete Upgrade-Auswirkungen je Zielplattform **nicht vollständig verifizierbar**.

2. **Soll libxml2 als eigenes Modul integriert werden?**
   - **Ja, aber nur in gestaffeltem Ansatz.**
   - Belege: JSON-Adaptermuster vorhanden; XML aktuell weniger entkoppelt.
   - Hauptrisiko: Refactoring-Komplexität.

3. **Ist jetzt wegen YAJL-Ablösung ein guter Zeitpunkt dafür?**
   - **Teilweise ja: vorbereiten jetzt, harte Umstellung gestaffelt.**
   - Belege: JSON-Migrationsarchitektur liefert Muster; gleichzeitiger Big-Bang erhöht Risiko.

4. **Ist direkte Modulbildung sicher genug?**
   - **Nur mit Guardrails.**
   - Ohne zentrale Sicherheitsdefaults/Limits/Policy-Enforcement: **nicht sicher genug belastbar belegt**.

5. **Soll ein Workflow zur Abfrage aktueller Releases/Sicherheitsstände eingeführt werden?**
   - **Ja: Monitoring + Reporting + manuelle Freigabe.**
   - Kein blindes Auto-Upgrade.

---

## 12. Priorisierte Maßnahmen

## Kurzfristig
1. **Repo-Hygiene YAJL-Reste bereinigen**
- Änderung: README/CI-Dependency-Liste gegen tatsächliche JSON-Backends konsolidieren.
- Ziel: Architektur- und Security-Transparenz.
- Risiko bei Nicht-Umsetzung: falsche Annahmen in Betrieb/Audit.
- Eingriffsgrad: niedrig.
- Plattformauswirkung: gering.
- Testbedarf: CI-Konfig-Tests.
- Sicherheitsauswirkung: indirekt positiv (weniger Drift).

2. **XML-Sicherheits-Baseline dokumentieren und härten**
- Änderung: explizite Policy für XXE/DTD/externes Laden, Limits, Fehlermeldungsstrategie.
- Eingriffsgrad: niedrig-mittel.
- Sicherheitsauswirkung: hoch.

3. **Monitoring-Workflow als Report-only einführen**
- Änderung: geplanter Job sammelt libxml2 + JSON-Backend Releases, CVE/NVD, Debian/Ubuntu/Fedora, Homebrew-Stände.
- Ausgabe: maschinenlesbarer Report + optional Issue.
- Sicherheitsauswirkung: hoch.

## Mittelfristig
4. **XML-Fassade (intern) einführen**
- Änderung: `XmlParseOptions`, `XmlParseResult`, zentrale `XmlProcessor`-Schnittstelle.
- Ziel: Entkopplung/Standardisierung.
- Risiko: Regressionen.
- Eingriffsgrad: mittel-hoch.
- Testbedarf: neue Unit-/Integrationstests + bestehende Regressionen.

5. **Direkte libxml2-Aufrufe schrittweise konsolidieren**
- Kandidaten: `request_body_processor/xml.cc`, `variables/xml.cc`, `operators/validate_*`.
- Ziel: keine direkten libxml2-Calls außerhalb XML-Modul.
- Sicherheitsauswirkung: mittel-hoch.

## Langfristig
6. **Formatübergreifendes Parsing-Contract (JSON/XML) harmonisieren**
- Änderung: gemeinsame Prinzipien für Optionen, Limits, Errors, Telemetrie.
- Ziel: konsistente Security- und Wartbarkeitseigenschaften.

7. **Governance-Prozess für Abhängigkeiten**
- Änderung: feste Review-Kadenz, Security-Triage, manuelle Freigabegates.
- Sicherheitsauswirkung: hoch.

---

## 13. Offene Unsicherheiten / nicht verifizierbare Punkte

- Produktiv verwendete libxml2-Version je Deployment: **Nicht verifizierbar.**
- Vollständige Backport-Abdeckung aller relevanten CVEs je Linux-Distribution/Release: **Nicht verifizierbar.**
- Genaue Upgrade-Kompatibilität (ABI/API) für alle Downstreams: **Nicht verifizierbar.**
- Exakter Performance-Impact einer XML-Fassade ohne Benchmarks: **Nicht verifizierbar.**
- Eindeutiger Abschlussstatus der YAJL-Migration außerhalb sichtbarer Repo-Artefakte: **Nicht verifizierbar.**

---

## 14. Quellenliste

## Repository
- `src/modsecurity.cc`
- `src/transaction.cc`
- `src/request_body_processor/xml.cc`
- `src/variables/xml.cc`
- `src/operators/validate_dtd.cc`
- `src/operators/validate_schema.cc`
- `src/request_body_processor/json_backend.h`
- `src/request_body_processor/json_adapter.cc`
- `src/request_body_processor/json_backend_simdjson.cc`
- `src/request_body_processor/json_backend_jsoncons.cc`
- `configure.ac`
- `build/libxml.m4`
- `build/win32/conanfile.txt`
- `build/win32/README.md`
- `.github/workflows/ci.yml`
- `.github/workflows/ci_new.yml`
- `README.md`
- `test/run-json-backend-matrix.sh`
- `test/unit/json_backend_depth_tests.cc`
- `test/test-cases/regression/config-xml_external_entity.json`
- `test/fuzzer/Makefile.am`

## Externe Primärquellen
- libxml2 Upstream Releases/Archive:
  - https://download.gnome.org/sources/libxml2/
  - https://download.gnome.org/sources/libxml2/2.15/
  - https://download.gnome.org/sources/libxml2/2.15/libxml2-2.15.2.news
  - https://download.gnome.org/sources/libxml2/2.15/libxml2-2.15.0.news
  - https://download.gnome.org/sources/libxml2/2.14/libxml2-2.14.0.news
  - https://download.gnome.org/sources/libxml2/2.12/libxml2-2.12.10.news
- Offizielle API-Dokumentation:
  - https://gnome.pages.gitlab.gnome.org/libxml2/html/parser_8h.html
  - https://gnome.pages.gitlab.gnome.org/libxml2/html/xmlIO_8h.html
- CVE-Datenbank:
  - https://nvd.nist.gov/vuln/detail/CVE-2025-24928
- Linux-Paketquellen:
  - Debian Security Tracker: https://security-tracker.debian.org/tracker/source-package/libxml2
  - Ubuntu Packages (noble): https://packages.ubuntu.com/noble/libxml2-dev
  - Ubuntu Security Notice (USN-7743-1): https://ubuntu.com/security/notices/USN-7743-1
  - Fedora Packages: https://packages.fedoraproject.org/pkgs/libxml2/libxml2/
- macOS/Packaging:
  - Homebrew libxml2 API: https://formulae.brew.sh/api/formula/libxml2.json
  - Homebrew simdjson API: https://formulae.brew.sh/api/formula/simdjson.json
- JSON-Backend Upstream-Releases:
  - simdjson Releases: https://github.com/simdjson/simdjson/releases
  - jsoncons Releases: https://github.com/danielaparker/jsoncons/releases
