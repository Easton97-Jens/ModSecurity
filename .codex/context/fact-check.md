# Faktenprüfung: bekannte Stolperfallen und Quellenabweichungen

## Geltungsbereich und Verwendung

Untersuchter Quellstand: `owasp-modsecurity/ModSecurity`, `v3/master`, **`effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729`**.
Diese Übersicht beschreibt Unterschiede zwischen Kommentaren, Beispielen, Build-Regeln und Implementierung. Sie behauptet weder Sicherheitslücken noch ausgeführte Laufzeittests.
Die Absätze **Prüfung** enthalten eigene Empfehlungen für neue Chats. Die Absätze **Befund** beruhen auf den verlinkten Quellen an diesem Commit.

## 1. Der README-Auszug entspricht nicht vollständig der aktuellen C++-API

**Befund:** Das README-Beispiel verwendet `ModSecurity::Rules`, einen Transaktionskonstruktor mit zwei Argumenten, `processConnection()` mit nur einer Adresse und `intervention()` ohne Ausgabeparameter. Die aktuellen Header deklarieren `modsecurity::RulesSet`, Konstruktoren mit `logCbData`, vier Verbindungsparameter und `intervention(ModSecurityIntervention *)`. Quellen: [README-Beispiel][readme-api], [RulesSet][rules-api], [Transaktionskonstruktor und Verbindung][transaction-args], [Intervention][intervention-api].

**Prüfung:** Wurde die Signatur direkt in Header und Implementierung geprüft oder nur aus der README übernommen? Ziehe für C++-Beispiele die tatsächlichen Quellen unter `examples/` und [cpp-guide.md](cpp-guide.md) heran.

## 2. Die Testprogramme liegen unter `test/`

**Befund:** Der Kompilierungsabschnitt der README nennt `tests/`. Das oberste Makefile nimmt tatsächlich `test` in den Build auf; das Makefile dieses Verzeichnisses definiert `unit_tests` und `regression_tests`. Quellen: [Schreibweise der README][readme-tests], [Build-Unterverzeichnisse][root-make], [Unit-Test-Ziel][unit-target], [Regressionstest-Ziel][regression-target].

**Prüfung:** Stimmen Programmname, Testdatei und Arbeitsverzeichnis? Die direkten Beispielbefehle in [build-and-test.md](build-and-test.md) beginnen innerhalb von `test/`.

## 3. Der belegte Sprachstandard ist C++17

**Befund:** `configure.ac` verlangt `AX_CXX_COMPILE_STDCXX(17, noext, mandatory)`. Auch der Windows-Build verlangt C++17. Der Standard wird durch die Build-Definitionen belegt und nicht aus dem Projektalter oder einzelnen Syntaxmerkmalen geschätzt. Quellen: [Unix-Anforderung][cpp-standard], [Windows-Anforderung][cpp-standard-win].

**Prüfung:** Ist ein vorgeschlagenes Sprachkonstrukt mit diesem Quellstand und dem tatsächlich verwendeten Compiler kompatibel? Lies bei einem anderen Commit dessen Build-Definitionen erneut.

## 4. YAJL: Die README-Anforderung weicht von der Unix-Build-Logik ab

**Befund:** Die README bezeichnet YAJL als zwingend erforderlich. Das YAJL-Makro verwendet dagegen eine allgemeine Bibliothekserkennung, deren automatischer Suchpfad bei fehlender Bibliothek fortfährt. Nur eine ausdrücklich verpflichtende Anforderung führt dort zu einem Konfigurationsfehler. `TEST_UTILITIES` wird nur bei `YAJL_FOUND=1` aktiviert. Quellen: [README][readme-dependencies], [YAJL-Makro][yajl-macro], [Erkennungslogik][find-library], [Testbedingung][test-condition].

**Prüfung:** Wurde YAJL erkannt, und wurden die Testprogramme tatsächlich gebaut? Für einen vorgesehenen Test-Build ist `--with-yajl=yes` die in dieser Arbeitsdokumentation gewählte Variante. Ein erfolgreicher Bibliotheksbuild allein belegt nicht, dass Testprogramme vorhanden sind.

## 5. Exit-Code 0 kann ohne ausgeführte Regressionstests auftreten

**Befund:** Mit `NO_LOGS` gibt `regression_tests` eine Meldung über fehlende Logging-Unterstützung aus und kehrt sofort mit `0` zurück. Dieser Pfad liegt vor der Argumentverarbeitung und dem Laden der Tests. Quelle: [NO_LOGS-Zweig][no-logs].

**Prüfung:** Welche Fälle liefen tatsächlich? Halte neben dem Exit-Code auch Fallzahlen, übersprungene und deaktivierte Fälle sowie Meldungen des Testprogramms fest. Dieser Befund bedeutet nicht, dass andere Build-Varianten ihre Tests bestehen.

## 6. LMDB ist im Windows-CMake-Build standardmäßig deaktiviert

**Befund:** Die Windows-README führt LMDB unter den standardmäßig aktivierten Funktionen auf. `build/win32/CMakeLists.txt` setzt `WITH_LMDB` jedoch auf `OFF`; Lua, libxml2, MaxMind und curl werden auf `ON` gesetzt. Quellen: [Funktionsliste der README][windows-readme], [CMake-Optionen][windows-options].

**Prüfung:** Welche CMake-Optionen wurden tatsächlich gesetzt? Eine Conan-Abhängigkeit oder die allgemeine README-Liste belegt nicht, dass die entsprechende Funktion in der Binärdatei aktiv ist.

## 7. Erkannt, abgewiesen und wirkungslos sind unterschiedliche Zustände

**Befund:** Die Grammatik behandelt die folgenden bekannten Direktiven unterschiedlich. Die Tabelle beschreibt die jeweiligen Parserzweige; sie ist keine vollständige v2/v3-Kompatibilitätsliste.

| Direktive | Belegtes Verhalten im Grammatikzweig |
| --- | --- |
| `SecRequestBodyInMemoryLimit` | Meldet fehlende Unterstützung und löst `YYERROR` aus. [Quelle][memory-limit] |
| `SecPcreMatchLimitRecursion` | Hat dort keine ausführende Aktion; die Fehlermeldung ist auskommentiert. Das benachbarte `SecPcreMatchLimit` setzt dagegen eine Eigenschaft. [Quelle][recursion-limit] |
| `SecTmpDir`, `SecDataDir`, `SecStatusEngine` | Die gezeigten Zweige nehmen keine entsprechende Wertzuweisung vor; Fehlermeldungen sind auskommentiert. [Quelle][compat-directives] |

**Prüfung:** Was passiert nach dem Erkennen des Tokens? Verfolge Grammatikaktion, gespeicherte Eigenschaft und Laufzeitzugriff. Weder „jede nicht implementierte Wirkung erzeugt einen Fehler“ noch „erfolgreiches Parsen bedeutet wirksames Verhalten“ ist hier eine verlässliche Schlussfolgerung.

## 8. Eine Operatordatei belegt keine Verfügbarkeit in SecRules

**Befund:** `Operator::instantiate()` erkennt `gsblookup`, `rsub` und `validatehash`. Die SecRules-Grammatik weist `@gsbLookup`, `@rsub` und `@validateHash` jedoch ausdrücklich mit `OPERATOR_NOT_SUPPORTED` ab; das Makro meldet einen Fehler und führt `YYERROR` aus. Die entsprechenden `evaluate()`-Implementierungen enthalten TODOs und geben sofort `true` zurück. Quellen: [Factory][operator-factory], [Grammatikzweige][operator-grammar], [Fehlermakro][unsupported-macro], [GsbLookup][gsb-stub], [Rsub][rsub-stub], [ValidateHash][hash-stub].

**Prüfung:** Wurde der normale SecRules-Ladepfad untersucht? Die bloße Existenz einer Klasse oder eines Factory-Eintrags rechtfertigt nicht die Aussage, der Operator könne in Regeln verwendet werden. Auch eine ausnutzbare Sicherheitswirkung wird daraus hier nicht abgeleitet.

## 9. Interne Phasenwerte sind keine SecRules-Phasennummern

**Befund:** Das Enum enthält `ConnectionPhase`, `UriPhase`, `RequestHeadersPhase` und weitere Phasen. `Phase::init()` übersetzt `phase:1` in `RequestHeadersPhase`, deren interner Wert `2` ist. Es übersetzt `phase:0` in `ConnectionPhase`. `processURI()` befüllt Variablen und ruft im untersuchten Funktionskörper keine Auswertung von `UriPhase` auf. Quellen: [Enum][phase-enum], [Zuordnung][phase-action], [URI-Funktion][uri].

**Prüfung:** Stammt eine Zahl aus SecRules, einem internen Enum, einer API oder einer Logdarstellung? Die vollständige Zuordnung steht in [architecture.md](architecture.md).

## 10. Dieses Repository beschreibt die Bibliothek

**Befund:** Die Upstream-README unterscheidet libmodsecurity von den separat gepflegten Webserver-Connectors. Die Bibliothek verarbeitet HTTP-Daten, die ihr Aufrufer liefert. Die Beispiele und öffentlichen Transaktionsmethoden zeigen die API-Verwendung, belegen aber keine vollständige Integration mit einer bestimmten Nginx-, Apache- oder IIS-Version. Quellen: [Projektumfang][scope], [Transaktions-API][transaction-public].

**Prüfung:** Gehört das untersuchte Verhalten zur Bibliothek oder zum Connector? Fordere für einen externen Connector dessen eigene Quellen und Version an oder untersuche sie innerhalb des autorisierten Umfangs. Setze eine Bibliotheksintervention ohne weitere Belege nicht mit einer tatsächlich gesendeten HTTP-Antwort gleich.

## 11. Submodulversionen stammen aus Gitlinks

**Befund:** `.gitmodules` führt Pfade und URLs auf. Die folgenden festgelegten Commits wurden aus dem Git-Baum des festgelegten Hauptrepository-Commits gelesen, nicht aus Branchnamen oder README-Beispielausgaben. Quellen: [.gitmodules][gitmodules], [festgelegter Upstream-Baum][source-tree].

| Submodulpfad | Gitlink-Commit |
| --- | --- |
| `bindings/python` | `bc625d5bb0bac6a64bcce8dc9902208612399348` |
| `others/libinjection` | `211782219663f889f471650150df12b623c5766e` |
| `others/mbedtls` | `0fe989b6b514192783c469039edd325fd0989806` |
| `test/test-cases/secrules-language-tests` | `f73c73027ef49ccf99c7911744929a71d15ff419` |

**Prüfung:** Stimmen Checkout und [upstream-lock.json](upstream-lock.json) mit diesen Gitlinks überein? Die Zuordnung lässt sich mit `git ls-tree -r effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729` nachvollziehen. Eine Commit-Festlegung allein belegt weder die Initialisierung der Submodule noch die Prüfung ihrer Dateiinhalte.

## 12. Das Scanner-Image erzwingt keinen erfolgreichen Testlauf

**Befund:** `.oss-scanner/Dockerfile` bricht bei Fehlern in seiner Build-Kette ab, behandelt `make check` anschließend aber mit `|| echo "WARNING: make check reported failures"`. Ein Fehler dieses Testbefehls muss deshalb den Image-Build nicht fehlschlagen lassen. Auch der Kommentar vor der Testzeile beschreibt dieses Verhalten. Quelle: [Build- und Testschritte][scanner-docker].

**Prüfung:** Belegen die Ergebnisse „Image gebaut“ oder „Tests bestanden“? Werte Testausgabe und ursprünglichen Teststatus getrennt aus; halte einen erfolgreichen Docker-Build nicht als bestandene Testsuite fest.

## Pflege dieser Liste

**Unsere Empfehlung:** Lies bei einem Wechsel zu einem neueren Quellstand die betroffenen Implementierungen erneut. Ändere oder entferne Einträge nur mit nachvollziehbaren Quellen; übertrage behobene Abweichungen nicht in neue Chats. Halte bei zusätzlichen Beobachtungen immer Befund, Quelle, praktische Bedeutung und verbleibende Unsicherheit fest.

## Quellen am festgelegten Commit

[readme-api]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/README.md#L233-L255
[rules-api]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/rules_set.h#L38-L77
[transaction-args]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/transaction.h#L335-L350
[intervention-api]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/transaction.h#L398-L403
[readme-tests]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/README.md#L53-L54
[root-make]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/Makefile.am#L1-L17
[unit-target]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/test/Makefile.am#L29-L35
[regression-target]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/test/Makefile.am#L93-L99
[cpp-standard]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L49-L63
[cpp-standard-win]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/build/win32/CMakeLists.txt#L102-L104
[readme-dependencies]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/README.md#L101-L106
[yajl-macro]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/build/yajl.m4#L10-L11
[find-library]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/build/msc_find_lib.m4#L48-L97
[test-condition]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L313-L324
[no-logs]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/test/regression/regression.cc#L437-L442
[windows-readme]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/build/win32/README.md#L64-L74
[windows-options]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/build/win32/CMakeLists.txt#L1-L9
[memory-limit]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/seclang-parser.yy#L1655-L1663
[recursion-limit]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/seclang-parser.yy#L1696-L1707
[compat-directives]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/seclang-parser.yy#L1748-L1785
[operator-factory]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/operators/operator.cc#L148-L181
[operator-grammar]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/seclang-parser.yy#L992-L1026
[unsupported-macro]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/seclang-parser.yy#L298-L300
[gsb-stub]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/operators/gsblookup.cc#L26-L32
[rsub-stub]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/operators/rsub.cc#L26-L32
[hash-stub]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/operators/validate_hash.cc#L25-L31
[phase-enum]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/modsecurity.h#L108-L170
[phase-action]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/actions/phase.cc#L25-L66
[uri]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L337-L443
[scope]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/README.md#L21-L46
[transaction-public]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/transaction.h#L335-L419
[gitmodules]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/.gitmodules#L1-L12
[source-tree]: https://github.com/owasp-modsecurity/ModSecurity/tree/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729
[scanner-docker]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/.oss-scanner/Dockerfile#L29-L37
