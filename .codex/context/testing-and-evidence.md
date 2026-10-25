# Tests und Nachweise

## Prüfungen aus dem geänderten Verhalten ableiten

Dies ist die Validierungsrichtlinie des Arbeitsrepositorys. Sie behauptet
nicht, dass bereits eine Prüfung ausgeführt wurde. Wähle Nachweise, die die
konkreten Abnahmekriterien der Aufgabe klären; Befehle und Voraussetzungen
stehen in [build-and-test.md](build-and-test.md).

Die Core-Testsuite wird über das Makefile auf oberster Ebene,
`test/test-suite.in`, das Test-Makefile und die tatsächlichen Testprogramme
definiert. Erhalte diesen Umfang, wenn du eine vollständige Suite beschreibst.
Diese Richtlinie überträgt weder das Inventar mit 100 Fällen noch die frühere
Qualifizierungskampagne oder Host-Matrix des Connector-Referenzrepositorys auf
den Core. Quellen: [Suite-Einbindung][suite], [Testprogramme][programs].

## Änderungen an Dokumentation und Richtlinien

Lies bei einer reinen Dokumentationsänderung die betroffenen Quellen, prüfe
Verweise und Diff und führe die Dokumentationsprüfungen des Repositorys aus,
wenn sie verfügbar sind:

```sh
python3 .codex/bin/verify-context.py --strict-source
python3 .codex/bin/validate-structure.py
git diff --check
```

Führe sie im Stammverzeichnis des Repositorys aus. Dies sind Befehle für den
Dokumentationsablauf, keine C++-Tests von Upstream. Prüfe ihre tatsächliche
Ausgabe und ihren Umfang. Ihr Bestehen beweist weder die inhaltliche
Richtigkeit jeder Erklärung noch den Zugriff auf externe Websites oder eine
Kompilierung von ModSecurity.

Ein C++-Build ist nicht allein deshalb erforderlich, weil erklärendes Markdown
geändert wurde. Eine Änderung an einem ausführbaren Hilfsprogramm benötigt bei
wesentlicher Bedeutung eine Prüfung seines eigenen Verhaltens: etwa den
Nachweis, dass ein Referenzprüfer die konkrete ungültige Eingabe zurückweist,
die er zurückweisen soll, mit aufgabeneigenen temporären Testdaten.

`--strict-source` ist passend, wenn die Aufgabe unveränderte Upstream-Quellen
zusagt. Weicht eine autorisierte Produktänderung bewusst von der festgelegten
Basis ab, berichte diese Abweichung und prüfe den betroffenen Kontext. Ändere
die Quellbindung nicht nur, um eine fehlgeschlagene Abweichungsprüfung in ein
bestandenes Ergebnis zu verwandeln.

## Produkt- und Build-Änderungen

Wähle gezielte Tests für das betroffene Verhalten und einen passenden Grenz-
oder Fehlerpfad, wenn dieser der Grund für die Änderung ist. Ergänze eine
Regression, wenn sie einen beobachtbaren Fehler oder eine Anforderung erfasst.
Vermeide Tests, die nur die Implementierung wiederholen, sowie nicht zugehörige
große Testkampagnen.

Prüfe bei Build-, Parser-, öffentlichen API- oder Abhängigkeitsänderungen
zusätzlich die betroffene Konfiguration und Aufrufer. Erweitere die
Validierung nur, um ein konkretes verbleibendes Risiko zu klären oder eine
anwendbare bestehende Prüfvorgabe zu erfüllen. Sobald die Abnahmekriterien der
Aufgabe ausreichend belegt sind, schließe sie ab und benenne verbleibende Grenzen.

Deaktiviere, kürze oder ändere keine Tests, um ein bestandenes Ergebnis zu
erzielen. Eine umfassende oder plattformspezifische Prüfung, die nicht laufen
kann, behält ihren tatsächlichen Status. Ein automatisierter CI-Job ist nicht
von wahrheitsgemäßer Berichterstattung ausgenommen, nur weil die lokale Aufgabe
kleiner war.

## Core-spezifische Fallstricke bei Nachweisen

| Beobachtung | Erforderliche Einordnung | Quelle |
| --- | --- | --- |
| Die Unix-Konfiguration gelingt | Prüfen, ob `TEST_UTILITIES` aktiviert ist, bevor der Bau der Unit-/Regressionsprogramme behauptet wird; die Aktivierung hängt von `YAJL_FOUND=1` ab. | [Testaktivierung][activation], [bedingtes Unterverzeichnis][subdir]. |
| `regression_tests` endet mit `0` | Ausgabe und Fallzahlen prüfen: Der Zweig `NO_LOGS` gibt `0` zurück, ohne die Regressionen auszuführen. | [Programmzweig][nologs]. |
| Ein Fall wird übersprungen | Getrennt von ausgeführten, bestandenen Fällen erfassen; die Auswahl nach Fähigkeiten/Ressourcen ist relevant. | [Ressourcenprüfung der Regressionen][resources], [Suite-Treiber][driver]. |
| Eine Regeldatei lässt sich laden | Das belegt das Laden, keine vollständige HTTP-Transaktion. `modsec-rules-check` lädt über `RulesSet` und erzeugt keine Transaktion. | [Regelprüfprogramm][rules-check]. |
| Ein Test-JSON enthält ein erwartetes Ergebnis | Es beschreibt eine Testerwartung, keine aufgezeichnete Ausführung. | Arbeitsregel für Nachweise. |
| Eine Core-Regression besteht | Die Aussage auf diesen Build, Fall, diese Konfiguration und diesen API-Pfad begrenzen; externes Webserver-Verhalten benötigt Belege aus der gesonderten Integration. | [Core-/Connector-Grenze][connectors]. |

Das tatsächliche Testverzeichnis ist `test/`. Beispiele für direkte
Testprogrammaufrufe setzen dieses Arbeitsverzeichnis voraus;
`test/test-suite.sh` wechselt ausdrücklich dorthin. Lies den Build-Leitfaden,
bevor du einzelne Dateien oder Fälle über `file.json:N` auswählst.
Quelle: [Testtreiber][driver].

## Nachweisebenen und Statuswerte

Halte Quellprüfung, Dokumentationsvalidierung, Kompilierung,
Konfigurationsladen, Unit-Tests, Transaktionsregressionen, statische Analyse
und externe Host-/Laufzeittests auseinander. Ein Ergebnis auf einer Ebene
belegt keine andere. Verwende
[configuration-and-rules.md](configuration-and-rules.md) für Regelverhalten
und [cpp-guide.md](cpp-guide.md) für API- und Lebensdauerfragen.

Verwende diese Arbeitsstatus einheitlich:

| Status | Bedeutung |
| --- | --- |
| `passed` | Die angegebene Prüfung wurde ausgeführt und erfüllte ihr angegebenes Kriterium. |
| `failed` | Die Prüfung wurde ausgeführt und erfüllte dieses Kriterium nicht. |
| `blocked` | Eine erforderliche Voraussetzung oder Befugnis verhinderte die Prüfung. |
| `not_run` | Es fand keine Ausführung statt; den Grund nennen. |
| `not_applicable` | Die Prüfung ist auf diesen Umfang nicht anwendbar; den Grund erklären. |

Erhalte die vom Testprogramm gemeldeten Zahlen übersprungener und deaktivierter
Fälle als eigene Belege. Mache aus einer nicht verfügbaren Fähigkeit, fehlenden
Testdaten, einem abgebrochenen Job oder einem alten erfolgreichen Lauf kein
neues bestandenes Ergebnis.

## Übergabeprotokoll

Erfasse exakten Befehl und Verzeichnis, Core-Commit und relevante
Submodulrevisionen, lokale Änderungen, Build-Optionen, gegebenenfalls relevante
Werkzeugversionen, ausgewählten Fall/Datei, Exitcode, Fallzahlen, wichtige
Ausgabe und Einschränkungen. Halte Belegverweise zugänglich und entferne
Geheimnisse aus aufbewahrten Auszügen. Ein späterer Chat soll unterscheiden
können, was gelesen, abgeleitet, ausgeführt und offengelassen wurde, ohne ein
früheres Ergebnis zu erfinden.

[suite]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/Makefile.am#L88-L96
[programs]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/test/Makefile.am#L29-L99
[activation]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L313-L332
[subdir]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/Makefile.am#L2-L17
[nologs]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/test/regression/regression.cc#L437-L440
[resources]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/test/regression/regression.cc#L151-L183
[driver]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/test/test-suite.sh#L1-L40
[rules-check]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/tools/rules-check/rules-check.cc#L32-L93
[connectors]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/README.md#L40-L46
