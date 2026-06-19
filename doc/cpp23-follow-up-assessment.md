# Anschlussanalyse: C++23 nach/zusätzlich zu C++20 für ModSecurity (libmodsecurity)

## 1. Executive Summary

**Urteil:** **bedingt empfohlen**.

### 5 wichtigste Gründe
1. **C++20 liefert bereits den Großteil des realen Nutzens** für dieses Repository (API-Robustheit, Diagnostics, selektive Modernisierung) – C++23 ist überwiegend Feintuning, nicht strategischer Hebel.
2. **Für ein plattformübergreifendes Security-Projekt ist C++23-Feature-Reife uneinheitlich** (insb. Sprachmodus vs. Standardbibliotheks-Reife vs. Packaging).
3. **"Mit `-std=c++23` bauen" ist nicht gleichbedeutend mit "C++23 aktiv nutzen"**; letzteres erhöht Review-, Portabilitäts- und ABI-Risiken deutlich.
4. **Einige C++23-Features sind wertvoll (v. a. `std::expected`, `std::to_underlying`, `std::byteswap`, `std::unreachable`)**, aber nur selektiv in internen Pfaden.
5. **Die größten Hürden bleiben Toolchain/CI/Distributionen und ABI/API-Governance**, nicht fehlende Sprachfeatures.

### Klare Aussage
- **Reicht C++20 aus?** Ja, für den Großteil der operativen und sicherheitsrelevanten Verbesserungen.
- **Bringt C++23 echten Zusatznutzen?** Ja, aber punktuell (vor allem Fehlerpfad-Modellierung via `expected` und einige Utility-Features).
- **Ist C++23 eher Toolchain-Experiment oder praxisreifer Mehrwert?** Für ModSecurity aktuell **primär Toolchain-Experiment mit selektiven produktiven Inseln**.

---

## 2. Einordnung relativ zu C++20

### Was C++20 bereits ausreichend abdeckt
- Robuste API-Muster (`span`, `string_view`) und bessere Diagnostik (`source_location`) als Hauptgewinn.
- Solide Grundlage für inkrementelle Refactorings ohne exotische Sprachexperimente.
- Gute Balance aus Lesbarkeit, Performance-Kontrolle und Toolchain-Verfügbarkeit.

### Welche Probleme nach C++20 noch offen bleiben
- Explizite, typisierte Fehlerketten statt verstreuter Fehlercodes/Status-Werte.
- Teilweise uneinheitliche API-Verträge zwischen C-/C++-Grenzen.
- Weitere Reduktion von Utility-Boilerplate in Low-level-Code.

### Welche davon C++23 sinnvoll verbessern könnte
- `std::expected` (inkl. monadischer Operationen) für explizitere Fehlerpfade.
- Kleine Sicherheits-/Klarheitsgewinne mit `to_underlying`, `byteswap`, `unreachable`.
- Gewisse Standardbibliotheksverbesserungen (z. B. constexpr-Erweiterungen) selektiv.

### Unrealistische Erwartungen an C++23
- Kein automatischer Performancegewinn.
- Kein "einfaches" Ende von Komplexität in Parser-/Regel-Engine-Pfaden.
- Keine kurzfristig robuste Baseline für alle Distributionen/Toolchains nur durch Sprachmodus-Wechsel.

---

## 3. Toolchain- und Plattform-Reife für C++23

> Entscheidender Punkt: **Compiler-Sprachmodus** und **Standardbibliotheks-Feature-Reife** müssen getrennt bewertet werden.

### GCC / libstdc++
- Sprachmodus `-std=c++23` ist vorhanden; GCC selbst kennzeichnet C++23-Support weiterhin als "experimental" in seiner Statusdarstellung.
- libstdc++ dokumentiert C++23-Bibliotheksfeatures fortlaufend; Reife ist featureweise unterschiedlich und nicht als einheitlicher "alles fertig"-Zustand zu interpretieren.
- Für Maintainer heißt das: Feature-Test-Makros und konkrete Compiler-/Lib-Versionen sind Pflicht.

### Clang / libc++
- Clang führt C++23 als "partial" und listet Feature-Stand proposalweise.
- libc++ führt einen sehr granularen C++23-Status pro Feature/Paper, inkl. "Complete"/"In Progress".
- Praktisch: einige Features sind gut nutzbar, andere hängen von exakter Clang+libc++-Kombination ab.

### MSVC / Windows
- In den offiziellen Optionen ist C++23 als `/std:c++23preview` beschrieben; explizit mit Hinweis auf Preview-/ABI-Themen.
- Zusätzlich bleibt `/std:c++latest` als Sammelschalter für fortlaufende Features.
- Für ein Security-Projekt ist das ein klares Signal: C++23 auf Windows nur kontrolliert/experimentell als Baseline.

### Unix/Autotools, CI-Matrix, Distributionen
- Im Repository sind Autotools/CMake/Conan/cppcheck derzeit auf C++17 zentriert; C++20 ist bereits ein eigener Migrationsschritt.
- C++23 als Baseline setzt voraus: C++20 bereits stabil auf Linux/macOS/Windows + tragfähige CI + Packaging-Abstimmung.
- Distributionen liefern häufig konservative Compiler-/STL-Kombinationen; C++23-Featureeinsatz kann dort schneller brechen als der reine Sprachmodus.

### Tragfähigkeit als Baseline heute
- **Als projektweite Baseline für ein plattformübergreifendes Security-Projekt: derzeit nur bedingt tragfähig.**
- **Als zusätzlicher Build-Modus (experimentell) plus selektive Feature-Nutzung: realistisch.**

---

## 4. Detaillierte C++23-Feature-Bewertung für dieses Repository

### 4.1 `std::expected` (+ monadic operations)
- **Einsatz:** interne Fehlerpfade in Parser-/Rule-Load-/IO-nahen APIs.
- **Nutzen:** explizite, typisierte Fehlerbehandlung; weniger versteckte Kontrollflüsse.
- **Risiken:** API-Drift, wenn in öffentlichen Headern unkontrolliert eingeführt.
- **Portabilität/Reife:** mittel (abhängig von STL-Version).
- **Aufwand:** mittel.
- **Empfehlung:** **mittlerer bis hoher Nutzen**, selektiv intern einführen.

### 4.2 `if consteval`
- **Einsatz:** Compile-Time-Utilities/Meta-Helfer (falls nötig).
- **Nutzen:** sauberere compile-time branching-Semantik.
- **Risiken:** begrenzter Mehrwert für Kernlogik.
- **Reife:** gut in modernen Compilern.
- **Aufwand:** klein.
- **Empfehlung:** **geringer Nutzen**.

### 4.3 deducing this / explicit object parameter
- **Einsatz:** API-Design für member-/fluent-orientierte Utilities.
- **Nutzen:** potenziell elegantere Overload-Strukturen.
- **Risiken:** deutlich höhere Verständniskosten für Contributor.
- **Reife:** uneinheitlich in Toolchains.
- **Aufwand:** mittel bis groß.
- **Empfehlung:** **vermeiden** (vorerst).

### 4.4 multidimensional subscript operator
- **Einsatz:** nur relevant bei mehrdimensionalen Datenstrukturen.
- **Nutzen:** für ModSecurity gering.
- **Risiken:** praktisch kein ROI.
- **Reife:** mittel.
- **Aufwand:** klein.
- **Empfehlung:** **geringer Nutzen**.

### 4.5 static `operator()` / static `operator[]`
- **Einsatz:** spezielle Functor-/Proxy-Muster.
- **Nutzen:** niedrig im Projektkontext.
- **Risiken:** Lesbarkeits-/Style-Kosten ohne echten Gewinn.
- **Reife:** mittel.
- **Aufwand:** klein.
- **Empfehlung:** **vermeiden**.

### 4.6 size_t literal suffix (`z` / `uz`)
- **Einsatz:** klarere Literal-Typisierung in Low-level-Code.
- **Nutzen:** kleine Correctness-/Lesbarkeitsgewinne.
- **Risiken:** minimal.
- **Reife:** gut.
- **Aufwand:** klein.
- **Empfehlung:** **geringer Nutzen**.

### 4.7 constexpr-Erweiterungen in der Standardbibliothek
- **Einsatz:** statische Tabellen/Hilfsroutinen.
- **Nutzen:** potenziell bessere Immutability/Precomputation.
- **Risiken:** compile-time-Kosten.
- **Reife:** featureabhängig.
- **Aufwand:** klein bis mittel.
- **Empfehlung:** **mittlerer Nutzen** (gezielt).

### 4.8 neue string/ranges/view-Verbesserungen
- **Einsatz:** interne Transform-/Parsing-Hilfen.
- **Nutzen:** bessere Ausdruckskraft.
- **Risiken:** template-bedingte Komplexität, mögliche Runtime-Unklarheit.
- **Reife:** unterschiedlich je STL.
- **Aufwand:** mittel.
- **Empfehlung:** **geringer bis mittlerer Nutzen**, sparsam.

### 4.9 print/formatting-nahe Verbesserungen (`print`)
- **Einsatz:** Logging/CLI-Tools/Tests.
- **Nutzen:** komfortabelere Ausgabe.
- **Risiken:** Verfügbarkeits-/ABI-/Binary-Size-Themen je STL.
- **Reife:** uneinheitlich.
- **Aufwand:** klein bis mittel.
- **Empfehlung:** **noch nicht reif genug** als Standardansatz.

### 4.10 `stacktrace`
- **Einsatz:** Diagnostik bei Fehlerfällen.
- **Nutzen:** forensischer Mehrwert theoretisch hoch.
- **Risiken:** sehr uneinheitliche Plattform-/Toolchain-Reife.
- **Reife:** niedrig bis mittel.
- **Aufwand:** mittel.
- **Empfehlung:** **noch nicht reif genug**.

### 4.11 `mdspan`
- **Einsatz:** nur relevant bei numerischen/mehrdimensionalen Speicherlayouts.
- **Nutzen:** im ModSecurity-Kern gering.
- **Risiken:** zusätzliche Abstraktion ohne klaren Bedarf.
- **Reife:** unterschiedlich.
- **Aufwand:** mittel.
- **Empfehlung:** **vermeiden**.

### 4.12 `flat_map` / `flat_set`
- **Einsatz:** read-heavy, cache-freundliche Lookups in ausgewählten Pfaden.
- **Nutzen:** potenziell Performance in speziellen Workloads.
- **Risiken:** Reife/Verfügbarkeit abhängig von STL-Stand.
- **Reife:** noch nicht durchgängig als sichere Basis.
- **Aufwand:** mittel.
- **Empfehlung:** **noch nicht reif genug** (pilotierbar nur lokal).

### 4.13 `std::to_underlying`
- **Einsatz:** enum→Integer-Konvertierungen in Flags/Bitmask/Interops.
- **Nutzen:** safer than casts, weniger Boilerplate.
- **Risiken:** minimal.
- **Reife:** gut.
- **Aufwand:** klein.
- **Empfehlung:** **mittlerer Nutzen**.

### 4.14 `std::unreachable`
- **Einsatz:** intern an nachweislich unerreichbaren Stellen.
- **Nutzen:** Optimierungshinweis + explizite Invariante.
- **Risiken:** UB-Footgun bei falscher Nutzung.
- **Reife:** gut.
- **Aufwand:** klein.
- **Empfehlung:** **mittlerer Nutzen** mit strikter Policy.

### 4.15 `std::byteswap`
- **Einsatz:** Netzwerk-/Endian-Konvertierungen in Utils/Parsernähe.
- **Nutzen:** klarere, portable Low-level-Operation.
- **Risiken:** minimal.
- **Reife:** gut.
- **Aufwand:** klein.
- **Empfehlung:** **mittlerer Nutzen**.

### 4.16 `spanstream`
- **Einsatz:** In-memory-Stream-Verarbeitung in Tests/Tools.
- **Nutzen:** begrenzt.
- **Risiken:** geringe Relevanz für Kern.
- **Reife:** unterschiedlich.
- **Aufwand:** klein.
- **Empfehlung:** **geringer Nutzen**.

### 4.17 `move_only_function`
- **Einsatz:** Callback-Pfade, wo Move-only-Callables sinnvoll sind.
- **Nutzen:** präzisere Ownership-Semantik.
- **Risiken:** STL-Reife + API-Komplexität.
- **Reife:** mittel.
- **Aufwand:** mittel.
- **Empfehlung:** **mittlerer Nutzen** (selektiv intern).

### 4.18 generator / coroutine-nahe Ergänzungen
- **Einsatz:** nur bei klaren Streaming-Use-Cases.
- **Nutzen:** im aktuellen Kern gering.
- **Risiken:** hohe Komplexität/Debugkosten.
- **Reife:** noch nicht ideal für konservative Baseline.
- **Aufwand:** groß.
- **Empfehlung:** **vermeiden**.

### 4.19 import/std::modules-nahe Entwicklungen
- **Einsatz:** Build/Compile-Organisation.
- **Nutzen:** theoretisch compile-time.
- **Risiken:** Tooling-/Buildsystem-/Ökosystem-Komplexität sehr hoch.
- **Reife:** für dieses Projekt (Autotools, Generatorfiles, Multi-OS) nicht robust genug.
- **Aufwand:** sehr groß.
- **Empfehlung:** **vermeiden**.

---

## 5. Spezifischer Mehrwert gegenüber C++20

### Realer Zusatznutzen
- `std::expected` (+ monadische Operationen) als klarster funktionaler Mehrwert gegenüber C++20-Alternativen.
- `to_underlying`, `byteswap`, `unreachable` als kleine, aber saubere Verbesserungen mit gutem Nutzen/Risiko-Verhältnis.

### Spürbare Verbesserungen möglich bei
- API-Design interner Fehlerpfade (Expected statt ad-hoc Status + Out-Parameter).
- Diagnostik-Qualität in kontrollierten Utility-Layern.
- Sicherheit indirekt durch explizitere Kontrollflüsse und weniger Cast-/Endian-Boilerplate.

### Eher kosmetisch/operativ schwach
- Viele Syntax-/Template-Neuerungen (deducing this, static operators, etc.) ohne klare Projekthebel.
- Große Themen wie Modules/Coroutine-Ökosystem liefern hier kurzfristig mehr Risiko als Nutzen.

---

## 6. Risikoanalyse

### a) Toolchain-/Compiler-Risiko
- **Beschreibung:** C++23-Sprachfeatures unterschiedlich implementiert.
- **Wahrscheinlichkeit:** mittel-hoch
- **Auswirkung:** hoch
- **Gegenmaßnahmen:** C++23 nur experimentell in CI, feature-test-macro Gates.

### b) Standardbibliotheks-Risiko
- **Beschreibung:** library feature gaps/inkonsistente Reife.
- **Wahrscheinlichkeit:** hoch
- **Auswirkung:** hoch
- **Gegenmaßnahmen:** explizite Min-Versionen + Fallbacks + feature probing.

### c) ABI-/API-Risiko
- **Beschreibung:** neue Typen in Public Headers brechen Downstream.
- **Wahrscheinlichkeit:** mittel
- **Auswirkung:** hoch
- **Gegenmaßnahmen:** Public API freeze, Adapter-Overloads, ABI-Checks.

### d) Plattform-Risiko
- **Beschreibung:** Linux/macOS/Windows verhalten sich divergierend.
- **Wahrscheinlichkeit:** mittel
- **Auswirkung:** hoch
- **Gegenmaßnahmen:** Multi-OS-Matrix, Release-Candidate-Zyklen.

### e) Packaging-/Distributions-Risiko
- **Beschreibung:** konservative Distros bremsen C++23-Featureeinsatz.
- **Wahrscheinlichkeit:** hoch
- **Auswirkung:** mittel-hoch
- **Gegenmaßnahmen:** Baseline konservativ halten, C++23 feature-guarded.

### f) Test-/Regression-Risiko
- **Beschreibung:** neue Fehlerpfad-/Template-Semantik erzeugt subtile Regressions.
- **Wahrscheinlichkeit:** mittel
- **Auswirkung:** hoch
- **Gegenmaßnahmen:** erweiterte Regressionen + Sanitizer + differential benchmarks.

### g) Security-/Review-Risiko
- **Beschreibung:** höhere Sprachkomplexität erschwert Audits.
- **Wahrscheinlichkeit:** mittel
- **Auswirkung:** hoch
- **Gegenmaßnahmen:** klare Feature-Policy, Review-Checklisten, kleine PRs.

### h) Maintainer-/Contributor-Risiko
- **Beschreibung:** Einstiegshürde steigt, Review-Latenz steigt.
- **Wahrscheinlichkeit:** mittel
- **Auswirkung:** mittel
- **Gegenmaßnahmen:** style guide, begrenzte erlaubte Featureliste, Schulung via Beispiele.

---

## 7. C++23-Feature-Matrix

| Feature | möglicher Einsatz im Projekt | Nutzen gegenüber C++20 | Toolchain-Reife | Risiko | Aufwand | Empfehlung |
|---|---|---|---|---|---|---|
| `std::expected` | interne Fehlerpfade | Hoch | Mittel | Mittel | Mittel | **einführen (selektiv)** |
| Monadic ops für `expected` | Fehlerketten | Mittel-Hoch | Mittel | Mittel | Mittel | **einführen (selektiv)** |
| `std::to_underlying` | enum-Interops | Mittel | Hoch | Niedrig | Klein | **einführen** |
| `std::byteswap` | Endian-Utilities | Mittel | Hoch | Niedrig | Klein | **einführen** |
| `std::unreachable` | Invarianten/Hot paths | Mittel | Hoch | Mittel (UB bei Missbrauch) | Klein | **einführen mit Guardrails** |
| constexpr-lib Erweiterungen | statische Utilities | Mittel | Mittel | Niedrig-Mittel | Klein-Mittel | **später prüfen** |
| `move_only_function` | Callback-APIs intern | Mittel | Mittel | Mittel | Mittel | **später prüfen** |
| string/ranges/view additions | interne Pipelines | Gering-Mittel | Mittel | Mittel | Mittel | **später prüfen** |
| size_t literal suffix | Low-level literals | Gering | Hoch | Niedrig | Klein | **optional** |
| `print`/format improvements | tooling/logging | Gering-Mittel | Mittel | Mittel | Klein-Mittel | **noch nicht reif genug** |
| `stacktrace` | Diagnostics | Mittel (theoretisch) | Niedrig-Mittel | Mittel-Hoch | Mittel | **noch nicht reif genug** |
| `flat_map`/`flat_set` | Spezial-Container | Mittel (nischig) | Niedrig-Mittel | Mittel | Mittel | **noch nicht reif genug** |
| `spanstream` | Tests/Tools | Gering | Mittel | Niedrig | Klein | **optional** |
| deducing this / explicit object parameter | API-Design | Gering | Mittel | Mittel-Hoch | Mittel-Groß | **vermeiden** |
| multidimensional subscript | numerische Datenstrukturen | Gering | Mittel | Niedrig | Klein | **vermeiden** |
| static `operator()`/`operator[]` | Spezialmuster | Gering | Mittel | Mittel | Klein | **vermeiden** |
| generator/coroutine additions | streaming abstractions | Gering-Mittel | Niedrig-Mittel | Hoch | Groß | **vermeiden** |
| import `std` / modules-nahe Nutzung | Build-Struktur | Gering (kurzfristig) | Niedrig-Mittel | Hoch | Sehr groß | **vermeiden** |

---

## 8. Security-Betrachtung

### Positiv
- `std::expected` kann Fehlerpfade expliziter machen und implizite Fehlerzustände reduzieren.
- `to_underlying`/`byteswap` reduzieren fragile low-level Cast-/Byteorder-Muster.
- Bessere API-Modellierung kann Reviewbarkeit erhöhen, wenn Featureeinsatz diszipliniert bleibt.

### Negativ
- Zusätzliche Sprachkomplexität kann Security-Reviews erschweren.
- Falsch eingesetztes `unreachable` erzeugt UB-Risiken.
- Bei zu früher Breiteinführung steigen Angriffsflächen durch Portabilitäts-/Semantikabweichungen.

### Ehrliche Bewertung
- **Verbessert C++23 die Sicherheit?** Selektiv ja (vor allem via `expected` und robuste Utility-APIs).
- **Oder steigt primär Komplexität?** Ohne strikte Governance überwiegt die Komplexität.

---

## 9. Performance- und Ressourcenbetrachtung

- **Runtime:** keine automatische Verbesserung; `expected` kann in manchen Pfaden klarer und ähnlich performant wie Statuscodes sein, muss aber gemessen werden.
- **Compile time:** tendenziell eher steigend bei mehr Template-/Metaprogrammierung.
- **Binary size:** kann leicht steigen (zusätzliche Template-Instanziierungen, neue Bibliothekskomponenten).
- **Memory footprint:** meist neutral; abhängig von konkreten Datentypen und Fehlerobjekten.

Klarstellung: **Modernerer Sprachstandard bedeutet nicht automatisch schnelleren Code.**

---

## 10. Aufwandsschätzung

### Nur Build auf C++23 umstellen
- **Qualitativ:** mittel bis groß
- **T-Shirt:** M-L
- Grund: Buildsysteme/CI/Conan/Packaging konsistent anpassen.

### Selektive Nutzung einzelner Features
- **Qualitativ:** mittel
- **T-Shirt:** M
- Grund: gezielte interne Refactorings + Tests + Review-Policy.

### Breitere Refactorings mit C++23-Idiomen
- **Qualitativ:** groß bis sehr groß
- **T-Shirt:** L-XL
- Grund: hoher Reviewaufwand, Portabilitäts-/Regressionstests, potenzielle API-Auswirkungen.

---

## 11. Empfohlene Strategie

### Phase A: C++20 stabilisieren
- **Ziel:** robuste C++20-Baseline auf allen Zielplattformen.
- **Aufgaben:** CI-Matrix vervollständigen, Sanitizer/Benchmarks/ABI-Checks etablieren.
- **Erfolgskriterien:** dauerhaft grüne C++20-Pipeline + stabile Releases.
- **Abbruchkriterien:** anhaltende Plattform-/ABI-Regressions.
- **Risiken:** C++20-Baustellen werden unterschätzt.

### Phase B: C++23-Toolchain experimentell in CI aufnehmen
- **Ziel:** Kompatibilitäts-Transparenz ohne Produktivzwang.
- **Aufgaben:** zusätzliche non-blocking C++23-Jobs für GCC/Clang/MSVC.
- **Erfolgskriterien:** reproduzierbare Ergebnisse, dokumentierte Gaps.
- **Abbruchkriterien:** unwartbar hohe Flake-/False-Positive-Rate.
- **Risiken:** erhöhte CI-Kosten.

### Phase C: Build-Kompatibilität mit C++23 bewerten
- **Ziel:** klären, ob "builds with C++23" stabil genug ist.
- **Aufgaben:** alle optionalen Dependency-Kombinationen testen.
- **Erfolgskriterien:** definierte Mindesttoolchain + belastbare Erfolgsrate.
- **Abbruchkriterien:** kritische Plattform/Dependency-Lücken.
- **Risiken:** distro-spezifische Brüche.

### Phase D: 1–3 risikoarme C++23-Features pilotieren
- **Ziel:** realen Mehrwert nachweisen.
- **Aufgaben:** Pilot mit `expected`, `to_underlying`, `byteswap` in internen Modulen.
- **Erfolgskriterien:** bessere Lesbarkeit/Fehlerbehandlung ohne Regression.
- **Abbruchkriterien:** Performance-/Stabilitätsverschlechterung.
- **Risiken:** schleichende Ausweitung ohne Policy.

### Phase E: Entscheidung über breitere Nutzung
- **Ziel:** datengetriebene Produktiventscheidung.
- **Aufgaben:** Pilot auswerten, Downstream-/Packaging-Feedback einholen.
- **Erfolgskriterien:** klarer Nutzen > Risiko, stabile Toolchain-Breite.
- **Abbruchkriterien:** kein messbarer Mehrwert.
- **Risiken:** vorschneller Baseline-Wechsel.

---

## 12. Konkrete Empfehlung für Maintainer

**Empfehlung:** **erst C++20 stabilisieren, C++23 nur experimentell prüfen; anschließend selektiv wenige C++23-Features übernehmen.**

Technisch-pragmatische Begründung:
1. C++20 adressiert bereits den Großteil der relevanten Modernisierung.
2. C++23-Reife ist für ein Multi-Plattform-Security-Projekt nicht homogen genug für sofortige Baseline.
3. Höchster ROI liegt in selektiven Features (`expected`, `to_underlying`, `byteswap`) statt Vollumstieg.
4. Stabilität, ABI/API-Kontrolle und Packaging sind aktuell wichtiger als Sprachversions-Branding.

---

## 13. Konkrete nächste Schritte (12 Maßnahmen)

1. **C++20-Stabilitäts-Gate formal definieren**  
   - Zweck: Spezialregel absichern.  
   - Priorität: hoch.  
   - Nutzen: klare Voraussetzung für C++23-Schritte.

2. **C++23-Shadow-Jobs in CI hinzufügen (non-blocking)**  
   - Zweck: reale Kompatibilitätsdaten sammeln.  
   - Priorität: hoch.  
   - Nutzen: faktenbasierte Entscheidung statt Annahmen.

3. **Feature-Test-Makro-Policy verbindlich machen**  
   - Zweck: library feature drift beherrschen.  
   - Priorität: hoch.  
   - Nutzen: weniger Portabilitätsbrüche.

4. **Toolchain-Minimum pro Plattform dokumentieren**  
   - Zweck: klare Supportgrenzen.  
   - Priorität: hoch.  
   - Nutzen: planbare Releases.

5. **`std::expected` Pilot in internem Modul starten**  
   - Zweck: praktischen Mehrwert validieren.  
   - Priorität: hoch.  
   - Nutzen: bessere Fehlerpfade.

6. **`to_underlying` und `byteswap` als Low-risk Cleanups einführen**  
   - Zweck: Utility-Code robuster machen.  
   - Priorität: mittel-hoch.  
   - Nutzen: sichere Kleingewinne.

7. **`unreachable`-Nutzungsrichtlinie definieren**  
   - Zweck: UB-Risiko begrenzen.  
   - Priorität: mittel-hoch.  
   - Nutzen: kontrollierter Einsatz.

8. **Packaging-Check mit Ziel-Distros durchführen**  
   - Zweck: Realitätsabgleich außerhalb CI.  
   - Priorität: hoch.  
   - Nutzen: weniger Überraschungen downstream.

9. **ABI/API-Überwachung für C++23-Pilots aktivieren**  
   - Zweck: Public-Surface schützen.  
   - Priorität: hoch.  
   - Nutzen: connector-sichere Evolution.

10. **Benchmark-Baselines für Pilotpfade erfassen**  
   - Zweck: Performance-Neutralität belegen.  
   - Priorität: mittel-hoch.  
   - Nutzen: datengetriebene Entscheidungen.

11. **Security-Review-Checklist um C++23-Footguns erweitern**  
   - Zweck: Reviewqualität sichern.  
   - Priorität: mittel-hoch.  
   - Nutzen: geringeres Introduktionsrisiko.

12. **Go/No-Go-Entscheidung nach 1–2 Releasezyklen treffen**  
   - Zweck: keine Dauer-Experimente.  
   - Priorität: hoch.  
   - Nutzen: klare Roadmap.

---

## Abschluss

**A) Gesamturteil in einem Satz:**
C++23 ist für ModSecurity **als experimenteller Buildmodus mit selektiver Feature-Nutzung sinnvoll**, aber **nicht als sofortige produktive Baseline**.

**B) Risikoampel:**
- Toolchain-/Compiler-Reife: 🟠
- Standardbibliothek/Feature-Reife: 🔴
- ABI/API: 🟠
- Plattform/Packaging: 🔴
- Security/Review-Komplexität: 🟠

**C) Was ich als Maintainer als Nächstes tun würde:**
Ich würde zuerst die C++20-Baseline verbindlich stabilisieren, parallel C++23 als non-blocking CI-Modus laufen lassen und nur `expected` + `to_underlying` + `byteswap` pilotieren.

**D) Welche C++23-Themen ich ausdrücklich noch nicht produktiv einführen würde:**
- Modules / `import std`
- coroutine-/generator-basierte Architekturänderungen
- breite Nutzung von deducing-this/explicit-object-Parametern
- `stacktrace` als plattformübergreifende Pflichtdiagnostik

**Spezialregel-Entscheidung für dieses Repository:**
Solange nicht nachweislich gilt, dass
1) C++20 auf allen Zielplattformen stabil läuft,  
2) die CI-Matrix tragfähig ist,  
3) öffentliche API-/ABI-Risiken verstanden und überwacht sind, und  
4) Downstream-Packaging realistisch abgesichert ist,  
empfehle ich explizit: **erst C++20 stabilisieren, C++23 nur experimentell prüfen**.
