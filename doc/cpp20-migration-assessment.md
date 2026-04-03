# Bewertung: Migration von ModSecurity (libmodsecurity) von C++17 auf C++20

## 1. Executive Summary

**Urteil:** **bedingt empfohlen**.

### Wichtigste 5 Gründe
1. **Build-Systeme sind bereits explizit auf C++17 verdrahtet** (Autotools, Windows/CMake, vcbuild/Conan) – ein Flag-Wechsel ist nicht nur ein Compiler-Schalter, sondern ein koordinierter Toolchain-Wechsel über mehrere Pfade.  
2. **Öffentliche C- und C++-API existieren parallel**, damit steigt das ABI/API-Risiko bei unkontrollierter Modernisierung, v. a. wenn öffentliche Header verändert werden.  
3. **Plattform- und Paketierungsrealität** (Linux/macOS/Windows, optionale Dependencies, Distributionspakete) ist der dominante Risikofaktor, nicht fehlende C++20-Features.  
4. **Einige C++20-Features bringen echten, sicherheitsrelevanten Mehrwert** (insb. `std::span`, `std::string_view`, `std::source_location`, gezielte `constexpr`), aber selektiv und nur mit harten Guardrails.  
5. **Hochrisiko-Features (Modules, Coroutines, flächige Ranges/`std::format`)** liefern hier kurzfristig wenig Nutzen bei überproportionalem Build-/Portabilitäts- und Wartungsrisiko.

### Zeithorizont
- **Kurzfristig (0–2 Monate):** Keine Vollmigration; erst CI-/Toolchain-Härtung + C++20-Compile-Probe ohne Feature-Rollout.  
- **Mittelfristig (2–6 Monate):** Selektive C++20-Nutzung in internen, nicht-ABI-exponierten Pfaden.  
- **Langfristig (6+ Monate):** C++20 als Default realistisch, wenn Distribution-/Consumer-Kompatibilität und Regressionen stabil nachgewiesen sind.

---

## 2. Repository- und Architekturverständnis

### Fakten aus dem Repository
- Projekt ist **libmodsecurity** als Bibliothek, getrennt von Connectoren; explizit als Bibliothekskern für verschiedene Webserver-Connectoren beschrieben.  
- Quellstruktur zeigt große Kernbereiche: `src/` (Engine, Operatoren, Aktionen, Variablen, Parser, Collections), `headers/` (öffentliche API), `test/` (unit/regression/benchmark/fuzzer), `examples/`, `tools/`.  
- Öffentliche API-Header liegen unter `headers/modsecurity/` und umfassen sowohl C++-Klassen-Header als auch C-API-Funktionen (`extern "C"`).  
- Parser-Anteile sind klar vorhanden (`src/parser/*.yy`, `*.ll`, generierte `*.cc/*.hh`) plus optionale Parser-Generierung via Build-Option.  
- Benchmark- und Optimierungs-Targets sind vorhanden (`test/benchmark/benchmark.cc`, `test/optimization/optimization.cc`).

### Fundierte Annahmen
- Das Projekt ist **ABI-/API-sensitiv**, weil es als dynamische Bibliothek (`libmodsecurity`) von externen Connectoren konsumiert wird.
- Parser- und Regel-Engine-Pfade sind correctness-kritisch; kleine semantische Änderungen können funktionale/security-relevante Auswirkungen haben.

### Besonders sensible Bereiche für Sprachstandardmigration
1. **Öffentliche Header** (`headers/modsecurity/*.h`) wegen API/ABI-Exposition.  
2. **Transaktions- und Regelverarbeitung** (`src/transaction.cc`, `src/rules_set.cc`, Operator-/Action-Pfade) wegen Laufzeit- und Security-Semantik.  
3. **Parser/Gene­rator-Pipeline** (`src/parser`) wegen Tool-Versionen, deterministischer Artefakt-Erzeugung und möglicher ABI-/Diagnostik-Unterschiede.  
4. **Windows-Buildpfad (CMake+Conan+vcbuild)** wegen separater Toolchain-Logik gegenüber Autotools.

---

## 3. Ist-Zustand der Toolchain

### Fakten aus dem Repository
- **C++17 ist explizit erzwungen**:
  - Autotools: `AX_CXX_COMPILE_STDCXX(17, noext, mandatory)`.
  - Windows CMake: `set(CMAKE_CXX_STANDARD 17)`.
  - `vcbuild.bat` setzt Conan-Profil mit `-s compiler.cppstd=17`.
  - `cppcheck` wird mit `--std=c++17` gefahren.
- **Build-Systeme**:
  - Unix/macOS: Autotools (`build.sh`, `configure`, `make`).
  - Windows: CMake + Conan + Visual Studio 2022 via `vcbuild.bat`.
- **CI-Matrix** (zwei Workflows `ci.yml`, `ci_new.yml`):
  - Linux GCC+Clang, macOS, Windows.
  - Verschiedene Optional-Dependency-Kombinationen (`--without-*`, `--with-*`).
  - 32-bit war im älteren Workflow noch teilweise enthalten, im neueren Linux-Workflow entfernt.
  - Separate cppcheck-Jobs.
- **Plattform-/Dependency-Hinweise**:
  - Viele optionale Dependencies (LUA, LMDB, libxml2, curl, maxmind, ssdeep, pcre/pcre2).
  - Windows-Doku nennt VS2022 und Conan 2.x.

### Bewertung C++20-Realismus
- **Technisch machbar**, da moderne CI-Umgebungen und VS2022 genutzt werden.
- **Operativ risikobehaftet**, weil C++17 hart in mehreren Build-Pfaden kodiert ist (Autotools+CMake+Conan+Static Analysis).
- Höchstes Risiko liegt **nicht im Core-Code allein**, sondern in:
  - inkonsistenten Toolchain-Settings über Build-Systeme,
  - Packager-/Distribution-Kompatibilität,
  - unterschiedlichen Standardbibliotheksimplementierungen (libstdc++, libc++, MSVC STL).

### Mögliche Probleme
- **GCC/Clang/MSVC Divergenzen** bei Corner Cases (Concepts, Ranges, `format`, constexpr-Auswertung, Diagnostics).
- **Windows/Conan-Profil** bleibt bei `cppstd=17`, wenn nicht bewusst migriert.
- **Distributionen** mit konservativen Toolchains könnten C++20-Baseline verzögern.
- **ABI-Nebenwirkungen** durch Header-Signaturen (z. B. `string_view`-Umstellungen in Public APIs).

---

## 4. Detaillierte Nutzenanalyse von C++20

> Klassifikation: **hoher Nutzen / mittlerer Nutzen / geringer Nutzen / vermeiden**

### `concepts` – **mittlerer Nutzen**
- **Sinnvoll wo:** interne Template-Utilities (falls vorhanden/neu), generische Hilfsfunktionen in `utils`/Parser-nahem Code.
- **Nutzen:** klarere Compile-Fehler, explizite Template-Verträge.
- **Risiko:** Compiler-Diagnostik-Uneinheitlichkeit, potenziell höhere Komplexität im Contributor-Onboarding.
- **Bewertung:** selektiv intern, nicht in breit genutzter Public API einführen.

### `ranges` – **geringer bis mittlerer Nutzen**
- **Sinnvoll wo:** interne Datenaufbereitung, Filter-/Transform-Pipelines in nicht-hot Paths.
- **Nutzen:** Lesbarkeit bei klaren Pipelines.
- **Risiko:** Compile-Zeit, potenziell intransparente Performance, höhere Template-Komplexität.
- **Bewertung:** nur punktuell; in Hot Paths eher klassische Schleifen bevorzugen.

### `std::span` – **hoher Nutzen**
- **Sinnvoll wo:** Buffer-/Body-/Header-Verarbeitung, Funktionen mit `(ptr, len)`-Signaturen.
- **Nutzen:** Bounds-semantik expliziter, API robuster, weniger Pointer+Längen-Fehler.
- **Risiko:** Public-API-Änderungen wären ABI/API-sensitiv.
- **Bewertung:** intern stark empfohlen; Public API nur mit sauberem Übergangspfad.

### `std::string_view` – **hoher Nutzen (selektiv)**
- **Sinnvoll wo:** read-only String-Parameter in internen APIs.
- **Nutzen:** weniger unnötige Kopien, klarere Intent-Semantik.
- **Risiko:** Lifetime-Footguns, besonders gefährlich in Security-Code wenn Views persistiert werden.
- **Bewertung:** empfohlen mit strikten Review-Regeln (keine persistierten dangling views).

### `std::format` – **geringer Nutzen / vorsichtig**
- **Sinnvoll wo:** Logging/Diagnostics.
- **Nutzen:** klarere Formatierung als Streams/printf-Mix.
- **Risiko:** historisch uneinheitliche STL-Unterstützung/Performance; Binärgröße.
- **Bewertung:** eher später prüfen; kein früher Migrations-Treiber.

### `std::source_location` – **mittlerer bis hoher Nutzen**
- **Sinnvoll wo:** Debug-/Error-Logging, Assertions, Diagnosepfade.
- **Nutzen:** bessere Diagnostik ohne Makro-Overhead, schnellere Incident-Analyse.
- **Risiko:** geringe Portabilitätsrisiken bei modernen Compilern.
- **Bewertung:** empfohlen für interne Logging-Utilities.

### Designated Initializers (C++20) – **geringer Nutzen**
- **Sinnvoll wo:** klar strukturierte POD-Initialisierung in Testcode.
- **Nutzen:** Lesbarkeit.
- **Risiko:** begrenzter realer Mehrwert; potenzielle Konsistenzprobleme mit C/C++-gemischten Strukturen.
- **Bewertung:** optional, niedrige Priorität.

### `constexpr`-Erweiterungen – **mittlerer Nutzen**
- **Sinnvoll wo:** statische Tabellen, Token-/Mapping-Hilfen, kleine Parser-Hilfsfunktionen.
- **Nutzen:** potenziell weniger Runtime-Overhead, klarere Immutability.
- **Risiko:** Compile-Zeit-Anstieg, komplexeres Debugging.
- **Bewertung:** gezielt einsetzen.

### `consteval` – **geringer Nutzen**
- **Sinnvoll wo:** sehr spezielle Compile-Time-Prüfungen.
- **Nutzen:** harte Compile-Time-Garantien.
- **Risiko:** unnötige Komplexität.
- **Bewertung:** derzeit vermeiden außer klarer Spezialfall.

### Coroutines – **vermeiden**
- **Nutzen hier:** gering; ModSecurity-Kern ist nicht erkennbar coroutine-zentriert.
- **Risiko:** enorme Komplexitäts-/Debug-/Portabilitätskosten.
- **Bewertung:** nicht als Migrationsthema.

### Modules – **vermeiden (vorerst)**
- **Nutzen:** potenziell Compile-Time langfristig.
- **Risiko:** inkompatibel mit bestehender Build-/Generator-/Ökosystemstruktur (Autotools, generated parser sources).
- **Bewertung:** klar später, nicht im C++17→20-Programm.

### Calendar/Time Zone – **geringer Nutzen**
- **Sinnvoll wo:** Zeitvariablen/Logging.
- **Nutzen:** sauberere Zeitabstraktionen.
- **Risiko:** gering, aber wenig strategischer Hebel.
- **Bewertung:** optional.

### Atomic smart pointers / Sync-Verbesserungen – **geringer bis mittlerer Nutzen**
- **Sinnvoll wo:** Shared-State/Log-Writer, falls contention-kritisch.
- **Nutzen:** sichere Patterns möglich.
- **Risiko:** keine gratis Performance; lock-free-Annahmen oft falsch.
- **Bewertung:** nur nach Profiling.

### `[[likely]]` / `[[unlikely]]` – **geringer Nutzen**
- **Sinnvoll wo:** klar dominierte Branches in Hot Paths.
- **Nutzen:** ggf. Mikro-Optimierung.
- **Risiko:** falsch gesetzte Hinweise verschlechtern Performance.
- **Bewertung:** nur benchmarkgetrieben.

### Dreiwegevergleich (`<=>`) – **geringer Nutzen**
- **Sinnvoll wo:** value types mit vielen Vergleichen.
- **Nutzen:** weniger Boilerplate.
- **Risiko:** API-/Semantikänderungen ohne großen Gewinn.
- **Bewertung:** niedrige Priorität.

---

## 5. Risikoanalyse

### a) Build-/Compiler-Risiko
- **Beschreibung:** inkonsistente C++-Standardumschaltung über Autotools/CMake/Conan/cppcheck.
- **Wahrscheinlichkeit:** hoch
- **Auswirkung:** hoch
- **Gegenmaßnahmen:** zentraler Migrations-Check, alle Pfade auf C++20 umstellen, CI-Gate auf alle Build-Systeme.

### b) ABI-/API-Risiko
- **Beschreibung:** Änderung öffentlicher Header-Signaturen (z. B. `string_view`, `span`) kann Consumer brechen.
- **Wahrscheinlichkeit:** mittel
- **Auswirkung:** hoch
- **Gegenmaßnahmen:** Public API freeze, ABI-Reports, C-API stabil halten, Adapter-Overloads statt Breaking Changes.

### c) Plattform-Risiko
- **Beschreibung:** unterschiedliche Compiler/STL-Verhalten (Linux/macOS/Windows).
- **Wahrscheinlichkeit:** mittel
- **Auswirkung:** hoch
- **Gegenmaßnahmen:** erweiterte Matrix, explizite Compiler-Minima, Plattform-spezifische Regression-Suites.

### d) Dependency-Risiko
- **Beschreibung:** optionale Dependencies + Paketmanager (Conan, distro packages) können C++20-Kompatibilitätskanten haben.
- **Wahrscheinlichkeit:** mittel
- **Auswirkung:** mittel-hoch
- **Gegenmaßnahmen:** Feature-Matrix je Dependency, known-good Versionen, Fallback-Pfade testen.

### e) Test-/Regression-Risiko
- **Beschreibung:** Funktionale Regressions trotz erfolgreichem Build.
- **Wahrscheinlichkeit:** mittel
- **Auswirkung:** hoch
- **Gegenmaßnahmen:** volle Regression-Suite, Cross-OS-Tests, Parser-Roundtrip-/Golden-Tests.

### f) Performance-Risiko
- **Beschreibung:** Modernisierung ohne Profiling kann compile time/binary size/runtime verschlechtern.
- **Wahrscheinlichkeit:** mittel
- **Auswirkung:** mittel-hoch
- **Gegenmaßnahmen:** Benchmark-Baselines, Budget-Schwellen, Hot-path-Regeln.

### g) Security-Risiko
- **Beschreibung:** neue Sprachmittel können neue Lifetime-/UB-Footguns freilegen (z. B. `string_view` dangling).
- **Wahrscheinlichkeit:** mittel
- **Auswirkung:** hoch
- **Gegenmaßnahmen:** secure-coding-Guidelines für C++20, ASan/UBSan-Gates, gezielte Code-Reviews.

### h) Contributor-/Maintainer-Risiko
- **Beschreibung:** höhere Komplexität bei stark template-lastigen Features.
- **Wahrscheinlichkeit:** mittel
- **Auswirkung:** mittel
- **Gegenmaßnahmen:** Feature-Policy (was erlaubt/verboten), Stilguide, Reviewer-Checklisten.

---

## 6. Codebasierte Migrationschancen

### Geeignete Bereiche
- **String-/Buffer-Handhabung:** `std::span<std::byte/char>` intern für Body-/Header-Pfade.
- **Parsing-nahe Logik:** compile-time Konstanten/Tabellen via `constexpr` dort, wo stabil.
- **Container-Nutzung:** gezielte API-Signaturverbesserungen intern mit `string_view`.
- **Fehlerbehandlung/Diagnostics:** `source_location` in Logging/Debug-Makro-Ersatz.
- **Threading/Concurrency:** nur nach Nachweis von Contention.
- **Performance-Hot-Paths:** konservativ, benchmark-first.

### Konkrete Refactoring-Muster
1. `const char* + size_t` → `std::span<const std::byte>`/`std::span<const char>` (intern).  
2. `const std::string&` (read-only) → `std::string_view` (intern, nicht persistieren).  
3. Logging-Helfer um `std::source_location` ergänzen.  
4. Template-Utilities mit `concepts` absichern (nur intern).  
5. Kleine, häufige Utility-Funktionen auf `constexpr` prüfen.

### Wo moderne Features eher schaden
- Flächige Einführung von Ranges in performance-kritischen Schleifen.
- Coroutines/Modules ohne harte Notwendigkeit.
- Public-Header-Umbau auf neue Typen ohne ABI-Plan.

---

## 7. C++20-Feature-Matrix für ModSecurity

| Feature | Potenzieller Einsatz im Projekt | Nutzen | Risiko | Migrationsaufwand | Empfehlung |
|---|---|---|---|---|---|
| `std::span` | Buffer-/Body-APIs intern | Hoch (Sicherheit/Lesbarkeit) | Mittel (API-Lifetime) | Mittel | **einführen** |
| `std::string_view` | Read-only String-Parameter intern | Hoch | Mittel (dangling) | Mittel | **einführen** |
| `std::source_location` | Logging/Diagnostics | Mittel-Hoch | Niedrig | Klein | **einführen** |
| `constexpr`-Erweiterungen | Tabellen/Utility | Mittel | Niedrig-Mittel | Klein-Mittel | **einführen** |
| `concepts` | Interne Templates | Mittel | Mittel | Mittel | **später prüfen** |
| `ranges` | Interne Transform-Pipelines | Mittel | Mittel-Hoch | Mittel | **später prüfen** |
| `std::format` | Logging/Textformat | Niedrig-Mittel | Mittel | Mittel | **später prüfen** |
| `[[likely]]`/`[[unlikely]]` | Hot-Path-Branches | Niedrig | Mittel (Fehlhint) | Klein | **später prüfen** |
| `<=>` | Value-type Vergleiche | Niedrig | Niedrig-Mittel | Klein | **später prüfen** |
| Designated Initializers | Test-/POD-Init | Niedrig | Niedrig | Klein | **später prüfen** |
| Coroutines | Async-Flow | Niedrig | Hoch | Groß | **vermeiden** |
| Modules | Build-Organisation | Niedrig (kurzfristig) | Hoch | Sehr groß | **vermeiden** |
| `consteval` | Spezial-Compile-Time Checks | Niedrig | Mittel | Mittel | **vermeiden** (vorerst) |
| Calendar/Time zone | Zeit-Handling | Niedrig | Niedrig | Klein-Mittel | **später prüfen** |

---

## 8. Toolchain- und CI-Readiness

### Zwingend vor Umstellung
1. C++20-Build in **allen** Build-Pfaden (Autotools, CMake/Windows, Conan profile, cppcheck mode).
2. Matrix mit GCC/Clang/MSVC in Debug+Release.
3. Sanitizer-Jobs (mind. Linux ASan+UBSan; TSan selektiv, da ggf. flaky/teuer).
4. Vollständige Unit+Regression-Suite auf Linux/macOS/Windows.
5. Optional-Dependency-Matrix (ON/OFF-Kombinationen der kritischen Features).
6. Benchmark-Vergleich als verpflichtendes Gate für definierte Hot Paths.

### Ziel-CI-Matrix (Mindestziel)
- **Linux:** GCC (2 Versionen), Clang (2 Versionen), Debug+Release, ASan+UBSan, optional 32-bit wenn support-claim besteht.
- **macOS:** Apple Clang (mind. 2 Runner-Generationen), Debug+Release.
- **Windows:** VS2022 (MSVC), Debug+Release (+ ggf. RelWithDebInfo), ASan wo stabil.
- **Static Analysis:** cppcheck + optional clang-tidy-Subset.
- **Performance:** nightly benchmark job mit Baseline-Vergleich.

---

## 9. Performance-Betrachtung

### Erwartung
- **Compile Time:** tendenziell eher schlechter bei stärkerer Template-/Ranges-/Concepts-Nutzung.
- **Binary Size:** kann steigen (insb. generische Abstraktionen, `format`).
- **Runtime:** neutral bis leicht besser/schlechter je Refactoring; keine automatische Verbesserung durch C++20.
- **Memory Footprint:** meist neutral; abhängig von konkreten Datenstrukturen/APIs.

> Moderne Syntax = **kein** automatischer Performancegewinn.

### Benchmark-Designs
- **Zu messen:**
  1. End-to-end Transaction-Pfad (Request/Response-Phasen).
  2. Regel-Ladezeit + Parser-Durchsatz.
  3. Operator-lastige Pfade (Regex, String-Operatoren).
  4. Logging-/Audit-Overhead.
- **Baselines:** C++17-Release-Build als Referenz je Plattform/Compiler.
- **Regressionsschwellen (Vorschlag):**
  - Runtime: >3% Verschlechterung = Blocker (Hot Paths).
  - Binary size: >5% = Review erforderlich.
  - Compile time: >10% = akzeptabel nur mit klarer Begründung.

---

## 10. Security-Betrachtung

### Potenzielle Sicherheitsgewinne
- `span` kann Boundaries expliziter machen und Pointer+Length-Fehler reduzieren.
- `string_view` kann unnötige Kopien reduzieren (weniger transienter Speicher), **wenn** Lifetimes korrekt bleiben.
- `source_location` verbessert Incident-Diagnostik und Forensik-Tiefe.

### Potenzielle neue Risiken
- `string_view`/`span`-Lifetime-Missbrauch erzeugt subtile UAF-/Dangling-Probleme.
- Höhere Ausdrucksstärke (Ranges/Concepts) kann Review-Aufwand und Fehlerentdeckungszeit erhöhen.
- Toolchain-/Supply-Chain-Risiken steigen, wenn Mindestcompiler und Packaging nicht synchron angehoben werden.

### Gesamtbewertung Security
- C++20 kann Sicherheit **selektiv real verbessern**, primär durch robustere API-Formen (`span`, disziplinierte `string_view`-Nutzung) und bessere Diagnostik.
- Ein pauschaler "Modernisierungsgewinn" ohne Governance ist sicherheitstechnisch nicht belastbar.

---

## 11. Aufwandsschätzung

| Teilbereich | Qualitativ | T-Shirt-Size | Kommentar |
|---|---|---|---|
| Voranalyse | mittel | M | API/ABI-Inventur, Consumer-Impact |
| Prototyping | mittel | M | C++20-Flag-only Builds + Smoke |
| CI-/Toolchain-Anpassung | groß | L | Mehrere Build-Systeme + Matrix |
| Code-Refactoring | mittel bis groß | M-L | Selektive, review-intensive Umstellungen |
| Testanpassung | mittel | M | Regression + neue Edge-Cases |
| Dokumentation | klein bis mittel | S-M | Build-Anforderungen, Policy |
| Release-/Packaging-Folgen | groß | L | Distro-/Conan-/Consumer-Kompatibilität |

**Gesamt:** **groß (L)** für sichere, produktionsreife Migration.

---

## 12. Migrationsstrategie

### Phase 0: Assessment
- **Ziel:** belastbare Ist-Aufnahme (Toolchains, APIs, ABI).
- **Aufgaben:** API/ABI-Inventur, Consumer-Liste, Compiler-/Distro-Mindeststände dokumentieren.
- **Erfolg:** vollständige Kompatibilitätsmatrix + Risiko-Backlog.
- **Abbruchkriterien:** unbekannte kritische Consumer/Plattformanforderungen.
- **Risiken:** unterschätzte Downstream-Abhängigkeiten.

### Phase 1: Toolchain/CI Readiness
- **Ziel:** C++20 buildbar in allen Pfaden.
- **Aufgaben:** Flags in Autotools/CMake/Conan/cppcheck auf C++20 umstellbar machen; Matrix erweitern; Sanitizer-Jobs.
- **Erfolg:** grünes CI in C++17 und C++20 parallel.
- **Abbruchkriterien:** reproduzierbare Plattformfehler ohne kurzfristigen Fix.
- **Risiken:** Windows-/Conan-Drift, flaky Tests.

### Phase 2: Build-Flag-Umstellung ohne Feature-Nutzung
- **Ziel:** Sprachstandardwechsel als Infrastrukturänderung isolieren.
- **Aufgaben:** Default auf C++20, Code semantisch unverändert; Regression+Benchmark.
- **Erfolg:** keine signifikanten Regressionen, ABI stabil.
- **Abbruchkriterien:** Performance-/ABI-Regressions über Schwelle.
- **Risiken:** versteckte Verhaltensänderungen durch Compiler/STL.

### Phase 3: Selektive C++20-Nutzung (geringes Risiko)
- **Ziel:** sichere Quick Wins.
- **Aufgaben:** intern `span`, `string_view` (regelbasiert), `source_location`; keine Public ABI-Brüche.
- **Erfolg:** messbare Qualität/Diagnostikgewinne, keine neuen Crashes.
- **Abbruchkriterien:** Lifetime-Bugs, erhöhte Incident-Rate.
- **Risiken:** inkonsistente Coding-Patterns.

### Phase 4: Gezielte Refactorings
- **Ziel:** strukturelle Verbesserungen in ausgewählten Modulen.
- **Aufgaben:** benchmark-gesteuerte Hot-Path-Optimierungen, ggf. `constexpr`/Concepts punktuell.
- **Erfolg:** Performance mindestens parity, Wartbarkeit steigt.
- **Abbruchkriterien:** Compile-time/Binary-size explodiert, Review-Stau.
- **Risiken:** Over-engineering.

### Phase 5: Release-Härtung
- **Ziel:** releasefähige Migration inklusive Downstream-Sicherheit.
- **Aufgaben:** Release-Candidates, Packaging-Tests, Migrationshinweise, Fallback-Plan.
- **Erfolg:** stabile RC-Zyklen ohne kritische Regressions.
- **Abbruchkriterien:** kritische Consumer-Inkompatibilitäten.
- **Risiken:** unterschätzte Distributionseffekte.

---

## 13. Empfehlung für Maintainer

**Empfehlung:** **erst CI/Compiler modernisieren, dann umstellen**.

Technisch-pragmatische Begründung:
1. Aktuell ist C++17 in mehreren Build-Ebenen fest verankert; ein isolierter Codewechsel reicht nicht.
2. Sicherheitskritischer Bibliothekscharakter verlangt konservative, regressionsarme Migration.
3. Höchster Hebel liegt in Toolchain- und Qualitäts-Gates, nicht in frühzeitiger Feature-Adoption.
4. Selektive C++20-Nutzung liefert den besten Nutzen-Risiko-Quotienten.

---

## 14. Konkrete nächste Schritte (Top 15)

1. **Build-/Toolchain-Inventur finalisieren** – Priorität: hoch – Nutzen: klare Entscheidungsbasis.  
2. **C++20-Schalter in allen Buildsystemen parametrisieren** – hoch – verhindert Drift.  
3. **Dual-Standard-CI (C++17 + C++20) temporär einführen** – hoch – risikoarme Transition.  
4. **Windows `vcbuild.bat`/Conan auf umschaltbares `cppstd` erweitern** – hoch – plattformparität.  
5. **cppcheck-Konfiguration C++20-fähig machen** – hoch – statische Analyse konsistent.  
6. **Compiler-/Runner-Minimalversionen dokumentieren** – hoch – klare Support-Grenzen.  
7. **Sanitizer-Gates (ASan/UBSan) für C++20 verpflichtend machen** – hoch – Security-Härtung.  
8. **ABI-Check-Workflow etablieren** – hoch – Public-API-Schutz.  
9. **Performance-Baselines je Plattform erfassen** – hoch – objektive Regressionserkennung.  
10. **Policy für erlaubte C++20-Features definieren** – mittel-hoch – verhindert Overuse.  
11. **Pilot-Refactor in internem Modul (`span`/`string_view`)** – mittel – schneller Nutzenbeweis.  
12. **Review-Checkliste für Lifetime-/UB-Risiken ergänzen** – mittel-hoch – weniger Security-Footguns.  
13. **Public-Header-Freeze während Initialmigration** – mittel-hoch – ABI-Sicherheit.  
14. **Downstream/Connector-Feedback-Runde vor Default-Switch** – mittel – Integrationssicherheit.  
15. **Release-Plan mit Stop/Go-Gates publizieren** – hoch – transparente Steuerung.

---

## Abschluss

**A) Gesamturteil in einem Satz:**
Eine Migration auf C++20 ist für ModSecurity **sinnvoll, aber nur schrittweise und toolchain-getrieben**, nicht als schnelle Vollmodernisierung.

**B) Risikoampel:**
- **Build/Toolchain:** 🔴
- **ABI/API:** 🟠
- **Plattform/Packaging:** 🔴
- **Performance:** 🟠
- **Security (bei disziplinierter Einführung):** 🟠 → 🟢

**C) Was ich als Maintainer morgen tun würde:**
Ich würde sofort eine **C++20-Shadow-CI** (parallel zu C++17) aufsetzen, alle Buildpfade umschaltbar machen und erst danach einen kleinen internen `span`/`string_view`-Pilot mergen.

**D) Was auf keinen Fall vorschnell getan werden sollte:**
1. Public Header ohne ABI-Plan auf neue Typen umstellen.  
2. Coroutines/Modules als Modernisierungsziel aufnehmen.  
3. C++20 als Default setzen, bevor Windows/Conan und Distribution-Story stabil sind.  
4. Performance-/Security-Baselines vor der Umstellung nicht messen.
