# Bauen, testen und Fehler untersuchen

## Geltungsbereich und Hinweise zum Lesen

Dieser Leitfaden beschreibt den untersuchten Upstream-Quellstand
`effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729` von `owasp-modsecurity/ModSecurity`,
Branch `v3/master`. Die Quellverweise zeigen auf diesen unveränderlichen Commit.
Die folgenden Befehle sind aus Build-Dateien, Testprogrammen und der
CI-Konfiguration abgeleitet. Dass ein Befehl hier aufgeführt ist, bedeutet
**nicht**, dass er in dieser Sitzung ausgeführt wurde oder auf deinem Rechner
bereits funktioniert. Der aktuelle Projektstand hält fest, welche Prüfungen
tatsächlich durchgeführt wurden.

## 1. Was der Build erzeugt

Unter Unix verwendet das Projekt Autotools, um `libmodsecurity.la` und die
zugehörige dynamische Bibliothek zu bauen. Der Build auf oberster Ebene
durchläuft `others`, `src`, `doc` und `tools` sowie `examples` und `test`, wenn
diese durch die Konfiguration aktiviert sind. Dabei wird nicht automatisch ein
Webserver-Connector gebaut. Die Beispiele und das Regelprüfprogramm sind
eigenständige Programme, die die Bibliothek verwenden.
Quellen: [Makefile auf oberster Ebene][root-make],
[Bibliotheksziel und öffentliche Header][src-make],
[Abgrenzung zu Connectors][readme-connectors].

| Bestandteil | Tatsächliche Voraussetzung oder Zweck | Quelle |
| --- | --- | --- |
| C- und C++-Compiler | `AC_PROG_CC` und `AC_PROG_CXX`; `AX_CXX_COMPILE_STDCXX(17, noext, mandatory)` schreibt C++17 vor. | [configure.ac, 49–63][compiler] |
| Autotools | `build.sh` ruft `libtoolize` beziehungsweise unter Darwin `glibtoolize` auf, danach `autoreconf`, `autoheader`, `automake` und `autoconf`. | [build.sh][bootstrap] |
| `make`, Archivierungsprogramm, `pkg-config` | Werden während der Konfiguration erkannt; `pkg-config` hilft beim Auffinden externer Bibliotheken. | [configure.ac, 49–63][compiler], [Bibliothekserkennung][find-lib] |
| Flex und Bison | Werden benötigt, wenn der Parser mit `--enable-parser-generation=yes` neu erzeugt wird. Diese Option ist standardmäßig deaktiviert; bereits erzeugte Parserdateien sind im Repository enthalten. | [configure.ac, 284–310][parser-option], [src/Makefile.am, 17–28][src-make] |
| Git-Submodule | `configure.ac` prüft auf bestimmte Dateien aus libinjection und dem verschachtelten mbedTLS-Quellbaum. Fehlende Dateien brechen die Konfiguration ab. | [configure.ac, 65–107][submodule-checks] |

`build.sh` verändert erzeugte Dateien: Unter anderem entfernt es
Autotools-Zwischendateien und erzeugt `src/headers.mk` neu. Prüfe vorhandene
Änderungen vor dem Aufruf mit `git status --short`. Dies ist eine Arbeitsregel
dieses Leitfadens; die Dateioperationen selbst stehen in
[build.sh][bootstrap].

## 2. Submodule und externe Bibliotheken

Das Hauptrepository deklariert vier Submodule:

| Pfad | Aus dem Quellbaum belegbare Rolle |
| --- | --- |
| `others/libinjection` | Eingebundene libinjection-Quellen; die Konfiguration benötigt `src/libinjection_html5.c`. |
| `others/mbedtls` | Eingebundene Mbed-TLS-Quellen; die Konfiguration benötigt ausdrücklich `tf-psa-crypto/utilities/base64.c` innerhalb eines verschachtelten Quellbaums. |
| `test/test-cases/secrules-language-tests` | Zusätzliche gemeinsame SecLang-Testfälle; die Standardpfade des Unit-Testprogramms und die Testsuite verweisen auf diesen Ort. |
| `bindings/python` | Eigenständiges Submodul mit Python-Anbindungen; es steht nicht in der `SUBDIRS`-Liste des Bibliotheks-Builds auf oberster Ebene. |

Quellen: [.gitmodules][submodules], [Konfigurationsprüfungen][submodule-checks],
[Standardpfad der Unit-Tests][unit-default], [Testliste][test-list],
[Build-Unterverzeichnisse][root-make].

Im Stammverzeichnis des Repositorys:

```sh
git submodule update --init --recursive
git submodule status --recursive
```

Die Upstream-README verlangt eine rekursive Initialisierung. Ein führendes `-`
in der Statusausgabe kennzeichnet ein nicht initialisiertes Submodul. Auch die
Statusabfrage rekursiv auszuführen, ist eine hier gewählte Vorgehensweise, um
verschachtelte Abhängigkeiten sichtbar zu machen.
Quelle: [README: Vorbereitung eines Git-Checkouts][readme-build].

### Welche Bibliothek wird wofür benötigt?

Die Tabelle unterscheidet die tatsächliche Build-Logik von der vorgesehenen
Testumgebung. „Optional“ bedeutet hier, dass die Unix-Konfiguration die
Bibliothek nicht immer voraussetzt; daraus folgt keine Zusage unveränderter
Funktionalität bei fehlender Bibliothek.

| Bibliothek | Status im untersuchten Quellstand | Beleg |
| --- | --- | --- |
| PCRE2 oder ausdrücklich gewähltes älteres PCRE | Der Standardpfad sucht nach PCRE2; `--with-pcre` wählt den anderen Pfad. Ohne `WITH_PCRE` bindet `regex.h` unmittelbar `pcre2.h` ein. Ein nutzbarer Build benötigt daher die gewählte Regex-Abhängigkeit. | [configure.ac, 147–157][regex-selection], [regex.h, 15–20][regex-header] |
| YAJL | Die README bezeichnet YAJL als verpflichtend. Die Unix-Makros lassen die automatische Erkennung bei fehlendem YAJL jedoch weiterlaufen; `TEST_UTILITIES` wird nur bei `YAJL_FOUND=1` aktiviert. Fordere es für den hier beschriebenen Test-Build ausdrücklich an. | [README: Abhängigkeiten][readme-deps], [YAJL-Makro][yajl], [allgemeine Erkennung][find-lib], [Testaktivierung][test-enable] |
| libcurl | Die Konfiguration definiert `MSC_WITH_CURL`, wenn curl erkannt wird; Upstream ordnet curl `SecRemoteRules` zu. | [configure.ac, 130–138][curl-config], [README: Operatorabhängigkeiten][readme-deps] |
| libxml2 | Optionaler Build-Bestandteil für die XML-Verarbeitung; die XML-Implementierung und der Transaktionspfad hängen von `WITH_LIBXML2` ab. | [configure.ac, 141–144][xml-config], [XML-Implementierung][xml-code] |
| LMDB | Steuert das Collection-Backend: LMDB mit `WITH_LMDB`, andernfalls `InMemoryPerProcess`. | [ModSecurity-Konstruktor][collections] |
| Lua, SSDEEP, GeoIP/MaxMind | Werden als eigenständige Fähigkeiten erkannt. Tests können abhängig von den einkompilierten Fähigkeiten übersprungen werden; die Testprogramme prüfen `resource` anhand ihrer eigenen Ressourcenlisten. | [configure.ac, 115–128][optional-config], [Ressourcen der Regressionstests][regression-resources] |

**Praktische Folge der YAJL-Abweichung:** Eine erfolgreiche Konfiguration allein
belegt nicht, dass die Programme für Unit- und Regressionstests gebaut wurden.
Prüfe bei einem Test-Build, ob die Zusammenfassung `Test Utilities ... enabled`
meldet, und kontrolliere die tatsächlichen Testzahlen. Bei deaktiviertem
Debug-Logging besitzt `regression_tests` außerdem einen Pfad, der eine Meldung
ausgibt und mit `0` endet, ohne die Regressionstests auszuführen. Quellen:
[Testaktivierung][test-enable], [Konfigurationszusammenfassung][configure-summary],
[NO_LOGS-Pfad][regression-main], [Behandlung übersprungener Tests][test-driver].

## 3. Ein Entwicklungs-Build unter Unix

Paketnamen hängen vom Betriebssystem ab. Dieses Dokument gibt keinen
universellen Befehl zur Paketinstallation vor. Die untersuchten GitHub-Workflows
enthalten konkrete Beispiele für Ubuntu und macOS. Sie bauen mit GCC oder
Clang, aktivieren Assertions und führen `make check` aus. Die Workflow-Dateien
belegen, welche Prüfungen konfiguriert sind, nicht deren Erfolg auf einem
beliebigen Rechner oder einen hier durchgeführten CI-Lauf.
Quellen: [Linux-CI][linux-ci], [weitere Linux-/macOS-CI][new-ci].

Führe nach der Installation geeigneter Build-Werkzeuge und
Entwicklungsbibliotheken diese Befehle im Stammverzeichnis des Checkouts aus:

```sh
git status --short
git submodule update --init --recursive
git submodule status --recursive
./build.sh
./configure --with-yajl=yes --with-pcre2=yes --enable-assertions=yes
make -j2
make check
```

Die Reihenfolge `build.sh`, `configure`, `make` und `make check` stammt aus der
Upstream-README und der CI. Die ausdrücklichen YAJL-/PCRE2-Flags und `-j2` sind
die hier gewählte Entwicklungsvariante: Fehlende Bibliotheken sollen während
der Konfiguration erkannt werden, und die Zahl paralleler Compilerprozesse
bleibt überschaubar. `MSC_CHECK_LIB` behandelt `yes` als zwingende Anforderung.
Quellen: [Build-Abfolge der README][readme-build], [Linux-CI][linux-ci],
[Bibliotheksoptionen][find-lib], [PCRE2-Makro][pcre2].

Für die meisten Arbeiten am Quellcode ist zunächst keine Installation nötig:
Die Testprogramme werden gegen den Build unter `src/.libs` gelinkt. Wenn eine
lokale Installation benötigt wird, setze bei der Konfiguration den
Installationspräfix auf einen beschreibbaren Pfad, zum Beispiel:

```sh
./configure --prefix="$HOME/.local/modsecurity" \
  --with-yajl=yes --with-pcre2=yes --enable-assertions=yes
make -j2
make check
make install
```

Dieser lokale Präfix ist eine Entscheidung dieses Leitfadens. Der
Standardpräfix im Quellcode lautet `/usr/local/modsecurity`; `make install`
installiert Build-Ergebnisse und ist kein Prüfschritt.
Quellen: [Standardpräfix][prefix], [Linkerpfade der Tests][test-make],
[installierbare Bibliothek und Header][src-make],
[Upstream-Installationsabfolge][readme-build].

### Debug-Builds und Arbeiten am Parser

Setze für die hier abgeleitete Debug-Variante bei der Konfiguration sowohl
C- als auch C++-Flags:

```sh
CFLAGS='-g -O0' CXXFLAGS='-g -O0' ./configure \
  --with-yajl=yes --with-pcre2=yes --enable-assertions=yes
make -j2
```

Die Assertion-Option ist implementiert: Sie setzt `-UNDEBUG` anstelle von
`-DNDEBUG`. Die zusätzlichen C++-Debug-Flags sind eine hier gewählte
Vorgehensweise. Prüfe den tatsächlichen Compileraufruf, um festzustellen, ob
ein bestimmter Build weitere Flags ergänzt.
Quelle: [Assertions und globale Flags][assertions].

Bearbeite für Änderungen an Grammatik oder Scanner die Eingabedateien
`src/parser/seclang-parser.yy` und `src/parser/seclang-scanner.ll`.
Die Erzeugung wird mit `--enable-parser-generation=yes` aktiviert. Das
Parser-Makefile enthält außerdem eine Nachbearbeitung des erzeugten Headers;
ein beliebiger eigenständiger Bison-Aufruf darf daher nicht als Ersatz für den
Projekt-Build angenommen werden.
Quellen: [Parseroption und Werkzeugerkennung][parser-option],
[Parser-Makefile][parser-make].

## 4. Gezielte Tests auswählen

Nach einem erfolgreichen Build mit aktiviertem `TEST_UTILITIES`, wie in
Abschnitt 3 mit ausdrücklich angefordertem YAJL, enthält `test` die Programme
`unit_tests` und `regression_tests`. Ihr gemeinsamer Argumentparser akzeptiert
unter anderem einen Dateipfad; Regressionstests unterstützen zusätzlich die
Auswahlform `file.json:N`. Regressionen werden nach ihren geladenen Schlüsseln
sortiert, und das Testprogramm zählt `N` ab `1`.
Quellen: [Programmnamen][test-make], [Argumentparser][test-cli],
[Auswahl von Regressionen][regression-main].

Die Upstream-README verwendet im allgemeinen Kompilierungsabschnitt einmal
`tests/`. Die tatsächlichen Build-Definitionen und Dateien verwenden `test/`,
ohne abschließendes `s`; Befehle müssen diesen tatsächlichen Pfad nutzen.
Quellen: [abweichende README-Stelle][readme-test-path],
[Testeinbindung][test-integration].

So führst du die gesamte konfigurierte Testsuite im Stammverzeichnis des
Repositorys aus:

```sh
make check
```

Wechsle für die folgenden direkten Aufrufe zuerst nach `test`. Das
Arbeitsverzeichnis ist relevant, weil Testdaten und Standardpfade relativ
dazu sind; der Shell-Testtreiber wechselt ebenfalls in dieses Verzeichnis.
Quelle: [test/test-suite.sh][test-driver].

```sh
cd test
./unit_tests test-cases/unit/transformation-hash-base64.json
./regression_tests test-cases/regression/secruleengine.json
./regression_tests test-cases/regression/secruleengine.json:1
```

Das erste Beispiel wählt eine vorhandene lokale Datei mit Transformationstests.
Das zweite prüft die vorhandenen Fälle der Regel-Engine; das dritte wählt einen
Eintrag aus dieser Datei aus. Dies sind dokumentierte Aufrufe, kein Protokoll
einer Testausführung.
Quellen: [vorhandene Unit-Testdatei][hash-test],
[vorhandene Engine-Regressionen][engine-test], [Argumentbehandlung][test-cli].

Ohne Dateiargument verwendet `unit_tests` standardmäßig Operatorfälle unter
`test-cases/secrules-language-tests/operators` und lädt bei diesem Standardziel
zusätzlich `transformations`. Weitere lokale Dateien unter `test-cases/unit`
werden von diesem direkten Standardaufruf daher nicht automatisch erfasst.
Den Umfang der vollständigen vom Projekt aufgeführten Suite bestimmen
`make check` und `test/test-suite.in`.
Quellen: [Standardpfad der Unit-Tests][unit-default],
[zusätzlicher Ladepfad][unit-main], [Testliste][test-list],
[Testeinbindung im Makefile][test-integration].

### Was ein Testergebnis belegt

- Halte Exitcode, Zahl der ausgeführten Fälle sowie übersprungene und
  deaktivierte Fälle gemeinsam fest. Die Programme nehmen übersprungene Fälle
  von den Fehlerzahlen aus; Exitcode `0` belegt daher nicht, dass alle möglichen
  Fähigkeiten geprüft wurden. [Ergebnisberechnung der Unit-Tests][unit-main],
  [Ergebnisberechnung der Regressionstests][regression-main].
- Prüfe beim Hinzufügen eines Regressionstests auch, ob er in
  `test/test-suite.in` aufgeführt ist. Die Suite wird über diese Datei
  eingebunden. [Makefile-Einbindung][test-integration], [Testliste][test-list].
- `format` ist ein schreibender Modus des Regressionstestprogramms: Er öffnet
  JSON-Dateien zur Ausgabe. Er ist kein alternativer, nur lesender Testbefehl.
  [Formatierungszweig][regression-main].
- Eine vorhandene JSON-Erwartung definiert einen Testvertrag; erst eine
  dokumentierte Ausführung belegt, ob der aktuelle Build ihn erfüllt.
  Diese Unterscheidung ist eine Arbeitsregel für dieses Repository.

## 5. Regeldateien laden und statische Prüfungen ausführen

Der Unix-Build definiert `tools/rules-check/modsec-rules-check`.
Das Programm behandelt ein vorhandenes Dateisystemobjekt als Eingabedatei
(`RulesSet::loadFromUri`); andernfalls behandelt es das Argument als SecLang-Text
(`RulesSet::load`). Ein negatives Ladeergebnis wird als Fehler gemeldet.
Quellen: [Programmziel][rules-check-make], [Implementierung][rules-check].

Beispiel aus dem Stammverzeichnis des Repositorys nach dem Build:

```sh
./tools/rules-check/modsec-rules-check 'SecRuleEngine DetectionOnly'
```

Das Prüfprogramm lädt Regeln und meldet Ladefehler; seine `main`-Funktion
simuliert keine HTTP-Transaktion. Erfolgreiches Laden ersetzt daher keinen
Regressionstest mit passenden Anfrage- und Antwortdaten.
Quelle: [vollständiger Kontrollfluss des Prüfprogramms][rules-check].

`make check-static` ruft `cppcheck` auf; Optionen und Unterdrückungen stehen
im Makefile auf oberster Ebene. `make check-style` ruft `check-coding-style`
auf, verwendet `cpplint.py` und schreibt `coding-style.txt`. Für diese
zusätzlichen Prüfungen müssen die jeweiligen Werkzeuge installiert sein.
Quelle: [statische Analyse und Stilprüfungen][static-checks].

## 6. Windows verwendet einen eigenen Build-Weg

Der untersuchte Windows-Weg verwendet Visual Studio 2022 Build Tools, Conan
und CMake. `vcbuild.bat` führt `conan install` mit `compiler.cppstd=17` aus,
erzeugt ein Projekt mit `Visual Studio 17 2022` und baut die gewählte
Konfiguration. `build/win32/CMakeLists.txt` verlangt mindestens CMake 3.24
und C++17.
Quellen: [Windows-README][windows-readme], [vcbuild.bat][vcbuild],
[Windows-CMake][windows-cmake].

Aus den Quellen belegte Beispielaufrufe in einer passend vorbereiteten
Windows-Umgebung, im Stammverzeichnis des Repositorys:

```bat
vcbuild.bat Release x86_64 NO_ASAN
vcbuild.bat Debug x86_64 USE_ASAN
```

Laut Windows-Anleitung liegen die Build-Ergebnisse unter
`build\win32\build\<configuration>`. Zusätzliche CMake-Optionen werden ab dem
vierten Argument weitergereicht; die CI verwendet zum Beispiel
`-DWITH_LMDB=ON`.
Quellen: [vcbuild.bat][vcbuild], [Windows-README][windows-readme],
[Windows-CI][windows-ci].

**Prüfe den Quellstand, statt die Anleitung zu verallgemeinern:** Die
Windows-README führt LMDB unter den standardmäßig aktivierten Funktionen auf.
Die tatsächliche CMake-Option `WITH_LMDB` steht auf `OFF`, während Lua, libxml2,
MaxMind und curl auf `ON` stehen. Für diesen Quellstand legen die
CMake-Optionen diese Standards fest. Die Conan-Abhängigkeiten haben im
Repository feste Versionen; ob sie auflösbar sind, wurde beim Erstellen dieser
Dokumentation nicht geprüft.
Quellen: [CMake-Optionen][windows-cmake],
[widersprüchliche README-Stelle][windows-readme], [conanfile.txt][conan].

Vor `ctest -C Release --output-on-failure` bereitet die Windows-CI unter anderem
DLLs, Testprogramme und Testdaten vor und deaktiviert mehrere benannte Fälle.
Den CTest-Befehl allein zu übernehmen, belegt keine Gleichwertigkeit mit der
vollständigen Linux-Prüfung.
Quelle: [Vorbereitung der Windows-Tests][windows-ci].

## 7. Fehler für die nächste Sitzung festhalten

Speichere für reproduzierbare weitere Arbeit diese Angaben:

1. Commit, Branch, Betriebssystem und Compiler.
2. Ausgabe von `git submodule status --recursive`.
3. Exakter Aufruf von `configure` oder `vcbuild.bat` und erkannte Fähigkeiten.
4. Erster fehlgeschlagener Befehl, Exitcode und relevante Fehlermeldung.
5. Konkrete Testdatei, ausgewählte Fälle, Fallzahlen sowie Zahlen
   übersprungener und deaktivierter Fälle.
6. Bestätigte Ursache oder eine ausdrücklich ungeklärte Hypothese.

Diese Liste definiert die hier eingeführte Vorgehensweise. Sie behauptet nicht,
dass bereits Builds ausgeführt wurden. Ihre Befehle und technischen Angaben
lassen sich anhand der oben verlinkten Build- und Testquellen prüfen.

## Quellen
[root-make]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/Makefile.am#L1-L17
[src-make]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/Makefile.am#L1-L67
[readme-connectors]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/README.md#L35-L49
[compiler]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L49-L63
[bootstrap]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/build.sh#L1-L33
[find-lib]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/build/msc_find_lib.m4#L21-L99
[parser-option]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L284-L310
[submodule-checks]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L65-L107
[submodules]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/.gitmodules#L1-L12
[unit-default]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/test/unit/unit.cc#L52-L53
[test-list]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/test/test-suite.in
[readme-build]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/README.md#L58-L95
[regex-selection]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L147-L157
[regex-header]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/utils/regex.h#L15-L20
[readme-deps]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/README.md#L101-L134
[yajl]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/build/yajl.m4#L10-L11
[test-enable]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L313-L335
[curl-config]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L130-L138
[xml-config]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L141-L144
[xml-code]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/request_body_processor/xml.cc
[collections]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/modsecurity.cc#L62-L78
[optional-config]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L115-L128
[regression-resources]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/test/regression/regression.cc#L160-L185
[configure-summary]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L479-L498
[regression-main]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/test/regression/regression.cc#L395-L561
[test-driver]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/test/test-suite.sh#L1-L40
[linux-ci]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/.github/workflows/ci.yml#L8-L106
[new-ci]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/.github/workflows/ci_new.yml#L75-L171
[pcre2]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/build/pcre2.m4#L11-L13
[prefix]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L40-L42
[test-make]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/test/Makefile.am#L29-L145
[assertions]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L350-L358
[parser-make]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/Makefile.am#L1-L43
[test-cli]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/test/common/modsecurity_test.cc#L95-L173
[hash-test]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/test/test-cases/unit/transformation-hash-base64.json
[engine-test]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/test/test-cases/regression/secruleengine.json#L1-L123
[unit-main]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/test/unit/unit.cc#L307-L379
[test-integration]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/Makefile.am#L88-L96
[readme-test-path]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/README.md#L53-L54
[rules-check-make]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/tools/rules-check/Makefile.am#L3-L9
[rules-check]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/tools/rules-check/rules-check.cc#L32-L93
[static-checks]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/Makefile.am#L58-L85
[windows-readme]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/build/win32/README.md#L3-L83
[vcbuild]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/vcbuild.bat#L1-L28
[windows-cmake]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/build/win32/CMakeLists.txt#L1-L149
[windows-ci]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/.github/workflows/ci_new.yml#L173-L234
[conan]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/build/win32/conanfile.txt#L1-L14
