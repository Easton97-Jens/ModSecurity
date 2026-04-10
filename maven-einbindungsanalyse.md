## 1. Belegte Fakten

- Das Repository nutzt auf Unix explizit **Autotools** (`./build.sh`, `./configure`, `make`, `make install`) laut Build-Anleitung in `README.md`.
- `build.sh` führt `libtoolize`, `autoreconf`, `autoheader`, `automake` und `autoconf` aus (also klassische Autotools-Generierung).
- `configure.ac` enthält `AC_INIT`, `AM_INIT_AUTOMAKE`, Compiler-Checks für C/C++, C++17-Anforderung und viele Feature-/Lib-Checks (u. a. YAJL, GeoIP, MaxMind, LMDB, Lua, Curl, LibXML, PCRE/PCRE2).
- Die Top-Level-Projektstruktur baut über `SUBDIRS` mindestens `others`, `src`, `doc`, `tools` sowie optional `examples` und `test`.
- Das Hauptartefakt auf Unix ist `libmodsecurity.la` (Libtool-Library), definiert in `src/Makefile.am`.
- Es gibt eine **separate Windows-Buildstrecke** via CMake + Conan (`build/win32/CMakeLists.txt`, `build/win32/conanfile.txt`, `vcbuild.bat`).
- Windows-README benennt explizit Visual Studio Build Tools + Conan als Voraussetzung und `vcbuild.bat` als Build-Einstiegspunkt.
- Es sind Git-Submodule hinterlegt, u. a. `others/libinjection`, `others/mbedtls`, `test/test-cases/secrules-language-tests`, `bindings/python`.
- Tests sind in Automake integriert (`make check`, `test/test-suite.in`, eigener Test-Driver), und die CI führt `make check` bzw. unter Windows `ctest` aus.
- Release-Helferskript `build/release.sh` erzeugt ein Tarball (`tar.gz`) plus SHA256 und GPG-Signatur.

## 2. Technische Hürden für Maven

1. **Kein Maven-Setup im Repository belegt**  
   - In den untersuchten Dateien gibt es keinen `pom.xml`-basierten Buildpfad; stattdessen sind Autotools/CMake dokumentiert und genutzt.  
   - **Schlussfolgerung:** Maven müsste neu eingeführt werden, nicht nur angepasst.

2. **Primär natives C/C++-Build mit Autotools/Libtool**  
   - `configure.ac` + `Makefile.am` + `libmodsecurity.la` zeigen eine etablierte native Toolchain mit Feature-Detection und Libtool-Artefakten.  
   - **Schlussfolgerung:** Maven-Integration muss native Build-Schritte orchestrieren oder ersetzen.

3. **Plattform-spezifische Buildpfade (Unix vs. Windows)**  
   - Unix: `build.sh`/`configure`/`make`; Windows: CMake + Conan + `vcbuild.bat`.  
   - **Schlussfolgerung:** Ein einheitlicher Maven-Prozess muss mehrere bestehende Buildpfade abbilden.

4. **Submodule-abhängige Buildvoraussetzungen**  
   - `configure.ac` bricht ab, wenn `others/libinjection` oder `others/mbedtls` fehlen; `.gitmodules` bestätigt Submodule-Setup.  
   - **Schlussfolgerung:** Maven-Einstieg muss Submodule-Handling berücksichtigen.

5. **Umfangreiches Test-Setup außerhalb von Maven-Konventionen**  
   - Testfälle werden über `test/test-suite.in` und Automake-Mechanik definiert; CI nutzt `make check`/`ctest`.  
   - **Schlussfolgerung:** Testintegration in Maven ist zusätzlicher Integrationsaufwand.

## 3. Technische Hinweise, die Maven erleichtern könnten

1. **Klar dokumentierte bestehende Build-Sequenz**  
   - `README.md` beschreibt eine reproduzierbare Schrittfolge (`build.sh` → `configure` → `make`).  
   - Das kann als Ausgangspunkt für Maven-Wrapper-Schritte dienen (Schlussfolgerung).

2. **Explizite Build-Skripte vorhanden**  
   - `build.sh`, `vcbuild.bat`, `build/release.sh` kapseln bereits zentrale Abläufe.  
   - **Schlussfolgerung:** Maven könnte zunächst diese Skripte orchestrieren statt sofort alles neu zu modellieren.

3. **CI-Pipelines dokumentieren funktionierende Build-/Test-Kommandos**  
   - GitHub Actions enthält vollständige Befehlsfolgen inkl. Dependency-Install, Build, Test auf Linux/macOS/Windows.  
   - Diese Kommandos liefern eine belastbare Referenz für eine erste Maven-Integration (Schlussfolgerung).

## 4. Nicht sicher beurteilbar

- Gewünschtes **Maven-Zielbild** (nur Orchestrierung vorhandener nativer Builds vs. vollständige Maven-native Modellierung) ist **im Repository nicht belegt**.
- Gewünschte **Java-Version**, falls Maven wegen Java-Teilprojekten eingeführt werden soll, ist **im Repository nicht belegt**.
- Gewünschte **Artefaktform im Maven-Kontext** (z. B. nur ZIP/TAR, JNI-Artefakte, Repository-Publishing) ist **im Repository nicht belegt**.
- Gewünschte **Modulgrenzen für Maven-Multi-Module** sind **im Repository nicht belegt**.
- Ob bestehende Nutzer/Release-Prozesse zwingend Maven verlangen, ist **im Repository nicht belegt**.

## 5. Belastbare Aufwandseinschätzung

**Einstufung: hoch**

**Begründung (nur repo-basiert):**
- Bestehendes Build-System ist bereits komplex und nativ (Autotools + Libtool + viele Feature-Checks).
- Zusätzlich existiert ein separater Windows-Stack (CMake + Conan + Batch-Einstieg).
- Tests und CI sind auf `make check`/`ctest` und nicht auf Maven-Lifecycle ausgerichtet.
- Submodule sind buildkritisch und müssen beim neuen Einstieg berücksichtigt werden.

## 6. Minimaler Migrationspfad

### Variante A (kleinstmögliche Einführung, Maven als Orchestrator)
1. Maven-Projektgerüst hinzufügen (Annahme: Maven soll zunächst nur bestehende Skripte aufrufen).
2. Maven-Buildschritte so verdrahten, dass auf Unix die dokumentierte Sequenz `./build.sh && ./configure && make` ausgeführt wird.
3. Maven-Testphase zunächst an `make check` anbinden.
4. Windows-Pfad separat anbinden über `vcbuild.bat` (Annahme: gleicher Maven-Einstieg für Windows gewünscht).
5. CI ergänzen: zusätzlicher Maven-Job parallel zum bestehenden Workflow, ohne den bestehenden Build sofort zu ersetzen (Annahme).

### Variante B (stärkerer Umbau, Maven als primäres Buildmodell)
- **Nicht belastbar bewertbar**, da konkrete Zielarchitektur (Artefakte, Modulgrenzen, Zielplattformabdeckung) im Repository nicht belegt.

## 7. Kurzfazit

Das Repository ist klar als natives C/C++-Projekt mit Autotools/Libtool (Unix) und CMake+Conan (Windows) aufgebaut.  
Ein Maven-Einstieg wäre deshalb kein kleiner „Schalter“, sondern eine zusätzliche Orchestrierungs- oder Migrationsschicht über bestehende Buildpfade hinweg.  
Die vorhandenen Skripte und CI-Kommandos liefern aber eine belastbare Basis für eine minimale, schrittweise Einführung als Wrapper.  
Für eine vollständige Maven-native Neuabbildung fehlen im Repository belastbare Zielvorgaben.  
Daher ist die Aufwandseinstufung auf Basis der vorhandenen Evidenz **hoch**.

## 8. Selbstkontrolle gegen Halluzinationen

- **Welche Aussagen sind direkt im Repository belegt?**  
  Buildsysteme (Autotools/CMake/Conan), Build- und Testkommandos, Submodule, CI-Abläufe, Release-Skript.

- **Welche Aussagen sind nur Schlussfolgerungen?**  
  Aufwandseinstufung „hoch“, Einordnung als zusätzlicher Integrationsaufwand und die Migrationsvarianten.

- **Welche Aussagen wären ohne ausreichende Evidenz spekulativ und wurden deshalb bewusst nicht getroffen?**  
  Java-Version, gewünschtes Maven-Packaging, endgültige Modulgrenzen, genaue Zielarchitektur einer vollständigen Maven-Migration.
