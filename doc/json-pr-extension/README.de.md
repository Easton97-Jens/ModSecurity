# Zusätzliche v3-JSON-Tests aus den PRs und der ModSecurity-Diskussion

Diese Übergabe erweitert die YAJL-Bestandsaufnahme um eigenständige ModSecurity-Tests. Beide vorbereiteten Repositories enthalten dieselben Tests und **die vollständige bisherige Dokumentation auf Deutsch und Englisch**. Nur die Einstiegssprache ihrer Haupt-README unterscheidet sich. YAJL, der Parser und alle übrigen Produktionsdateien bleiben unverändert.

## Vollständige Dokumentation

| Inhalt | Deutsch | English |
|---|---|---|
| Bisherige Ergebnisübersicht | [Original](../JSON-YAJL-BASELINE.md) | [Full translation](../en/JSON-YAJL-BASELINE.md) |
| Bisheriger v3-Bericht mit Befehlen und Grenzen | [Original](../json-yajl-baseline-v3.md) | [Full translation](../en/json-yajl-baseline-v3.md) |
| Ablauf, technische Analyse, v2/v3-Unterschiede, fzipis Aussagen | [Original](../json-yajl-code-analysis.md) | [Full translation](../en/json-yajl-code-analysis.md) |
| Neue Test- und Anforderungsmatrix | [Matrix](matrix.de.md) | [Matrix](matrix.en.md) |
| Neue Ergebnisse und Reproduktion | diese Datei | [English](README.en.md) |
| Ursprüngliche Projekt-README | [Vollständige Übersetzung](../reference/de/ModSecurity-README.md) | [Full archived original](../reference/en/ModSecurity-README.md) |

Das [bisherige Herkunftsmanifest](../json-yajl-provenance.json) und sämtliche vier bisherigen maschinenlesbaren v3-Testberichte sind unverändert enthalten. Das [neue Manifest](provenance.json), die [neue Ergebnisübersicht](results-summary.json) und der [Fallkatalog](catalog.json) sind sprachneutrale JSON-Dateien; die beiden Textfassungen erläutern sie. Doxygen-Konfiguration, Logo und ursprüngliche Dokumentations-Builddatei stehen unverändert unter `doc/reference/`; ihre Inhalte sind archivierte Werkzeuge, keine zweite ausführbare Source-Distribution.

Die bisherigen Berichte beschreiben den damaligen vollständigen ModSecurity-Checkout und dessen veröffentlichte Branches. Ihre historischen Zahlen bleiben erhalten. Befehle mit `src/`, `headers/`, `./configure` oder `test/regression_tests` benötigen diesen vollständigen Quellbaum. Für die eigenständigen Test-Repositories gelten die folgenden Befehle.

## Quellen und untersuchter Stand

Die Produktionsreferenz bleibt der offizielle Commit [`8271c9d3bc0abcbd1189f5414c196c336c959047`](https://github.com/owasp-modsecurity/ModSecurity/commit/8271c9d3bc0abcbd1189f5414c196c336c959047). Ausgangspunkt der Erweiterung ist der bereits veröffentlichte Fork-Stand [`2bfb2180d8c0fa35ae7a3f2665e999600607d896`](https://github.com/Easton97-Jens/ModSecurity/commit/2bfb2180d8c0fa35ae7a3f2665e999600607d896). Dessen Produktionsdateien entsprechen der Originalreferenz. Die neue Arbeit liegt ausschließlich auf `test/json-yajl-pr-coverage-v3-20261015`; die früheren v2-/v3-Branches werden nicht geändert.

Beim erneuten Leseabruf am 15.10.2026, 14:14 UTC, stand das offizielle `v3/master` bereits bei [`2e08f23dd50e7c0aa7c969b244b9d3a6dce4d641`](https://github.com/owasp-modsecurity/ModSecurity/commit/2e08f23dd50e7c0aa7c969b244b9d3a6dce4d641). Der direkte Vergleich betrifft Upload-Dateimodus/Multipart und zugehörige Tests, nicht JSON-Parser, Transaktion oder JSON-Schreibpfade. Die getestete Referenz wurde nicht verschoben; die Ergebnisse sind **keine vollständige Validierung dieses neueren Commits**.

Gelesen wurden [PR 4](https://github.com/digitalwave/jsonbench/pull/4), [PR 5](https://github.com/digitalwave/jsonbench/pull/5), [fzipis Kommentar](https://github.com/owasp-modsecurity/ModSecurity/issues/3308#issuecomment-6166461842) und [airweens Kommentar](https://github.com/owasp-modsecurity/ModSecurity/issues/3308#issuecomment-6142615388). PR 5 enthält den Stand von PR 4 vollständig; seine Tests werden nicht doppelt gezählt. Die festen PR-Commits stehen im Manifest. Es wurden Testideen unabhängig umgesetzt, kein GPL-Adaptercode aus jsonbench übernommen. Dessen Benchmark-Ergebnisse sind kein Laufzeitbeleg für ModSecurity.

## Tatsächliche Ergebnisse

| Frisch ausgeführtes Profil | PASS | FAIL | SKIP | BLOCKED | XFAIL | Exit |
|---|---:|---:|---:|---:|---:|---:|
| Charakterisierung des vorhandenen Verhaltens | 399 | 0 | 0 | 0 | 0 | 0 |
| Zusätzliche Anforderungen und ausdrücklich offene Vorschläge | 6 | 20 | 0 | 0 | 0 | 1 |
| Beide Profile zusammen | 405 | 20 | 0 | 0 | 0 | 1 |
| Begrenzte ASan/UBSan-Wiederholung | 124 | 0 | 0 | 0 | 0 | 0 |

Die 399 Prüfungen bestehen aus 163 bisherigen Request-Prüfungen, 192 zusätzlichen Request-/Parser-Prüfungen, 12 bisherigen Audit-Prüfungen, 21 zusätzlichen Schreib-/Kontrollprüfungen und 11 nativen JSON-Regressionsfällen. Sensitivitätskontrollen sind enthalten: Sie müssen absichtlich falsche Erwartungen zurückweisen. Die Sanitizer-Zeile wiederholt 124 dieser Fälle; sie ist kein zusätzlicher Funktionsumfang. Instrumentiert wurden nur Testhilfe und unveränderte `json.cc`, nicht die gesamte Engine oder YAJL. Leak-Prüfungen sind deaktiviert (`detect_leaks=0`); die bisher dokumentierte ptrace-Sperre bleibt eine Grenze.

Die 20 Fehlschläge bleiben gewöhnliche **FAILs**, mit Einzelbelegen und Exit 1:

- Sechs UTF-8-Anforderungen: bestimmte überlange, außerhalb des Unicode-Bereichs liegende oder Surrogat-Bytekodierungen werden angenommen. Sechs andere ungültige Bytefälle werden bereits abgelehnt und bestehen.
- Sechs zusätzliche Surrogat-Anforderungen: eine strengere Behandlung isolierter oder wiederholter Surrogate ist eine **offene Policy-Entscheidung**. RFC 8259 unterscheidet erlaubte Escape-Grammatik und Unicode-Interoperabilität; diese Fälle sind nicht sämtlich derselbe Syntaxfehler.
- Vier Vorschläge zum vollständigen Verwerfen bereits extrahierter Argumente nach Syntax-, Abschluss-, Tiefen- oder Argumentlimitfehlern. Das heutige Verhalten behält Teilargumente. Ein Rollback ist **kein bereits beschlossener Kompatibilitätsvertrag**.
- Eine Audit-Anforderung: ein erfolgreicher Writer soll strukturell gültiges UTF-8-JSON liefern. Ein synthetischer ungültiger Response-String führt derzeit zu ungültigen Ausgabebytes. Die konkrete Ersatz-/Kodierungsstrategie bleibt offen.
- Drei Anforderungen an die öffentliche C++-Methode `ModSecurity::processContentOffset`: die Ausgabe soll gültiges JSON sein und einen geprüften Bereich erhalten. Die Methode liefert in den Reprodukern Erfolg, aber unvollständige JSON-Strukturen. Das ist eine eigene Schreibschnittstelle, kein Request-Parser und kein Audit-Writer.

Bei den Offset-Fällen zeigen [Aufbau und Abschluss](https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/src/modsecurity.cc#L215-L380) den untersuchten Originalpfad. Die Bedingung `pending > 3` und die Generator-Abschlussfolge sind getrennte Änderungskandidaten. Es wurden keine Produktionsfixes eingespielt und keine Ausnutzbarkeit behauptet. Ein künftiger Fix benötigt einen eigenen Review und bewusst angepasste Charakterisierungserwartungen.

## Was zusätzlich geprüft wird

Zahlenähnliche Schlüssel und Strings werden von echten Zahlentokens unterschieden. Exponenten, `-0.0`, lange Dezimalzahlen, Werte jenseits von uint64 und 512-stellige Integer behalten ihre Bytes. Punkte und NUL in Pfaden, Geschwister nach verschachtelten Containern und Duplikate werden als Eintragslisten geprüft. Die Regeln der beiden neuen nativen Fälle sichern Werte, Namenanzahl, Abwesenheit in `ARGS_POST` und beide Fehlerflags ab; HTTP 418 ist hier nur das sichtbare Signal einer vollständig erfolgreichen Assertion-Kette.

Echte Aufrufe des ModSecurity-JSON-Prozessors teilen kurze Payloads an jeder Byteposition sowie byteweise auf. Weitere Fälle schneiden bei 16 KiB, innerhalb UTF-8/Escapes/Zahlen und vor dem Parserabschluss. Das sind keine Tests von HTTP-Transfer-Encoding. Fehler gefolgt von Erfolg verwenden dieselbe Engine und dasselbe Regelwerk mit neuen Transaktionen. Schlüssel und Werte um 8192 Bytes prüfen die Beobachtbarkeit; die Puffergröße eines jsonbench-Helfers ist kein ModSecurity-Limit.

Die getrennten Writer-Tests prüfen Steuerzeichen, Quotes, Backslashes, Unicode, leere und lange Werte, Feldtypen, doppelte Header, Logteile, Wiederholung und saubere Folgetransaktionen. Ein strukturell gültiger synthetischer Gegenbeleg beweist, dass die Audit-Anforderung eine zulässige künftige Lösung akzeptieren kann. Sie scheitert nicht lediglich daran, dass der Test selbst eine bekannte ungültige Fixture dekodiert.

Die [Matrix](matrix.de.md) führt jede der zehn Gruppen A–J auf und nennt Grenzen. JSON Schema aus airweens Vergleich, eine jsoncons-SAX-Neufassung, Bibliotheksbündelung, Connector-Abhängigkeitsabbau, Benchmarks und eine Ersatzbibliothekswahl werden **nicht** durch diese Tests erledigt. Ungeprüft bleiben unter anderem vollständige Speicherknappheit, Threads, fremde Connector-Implementierungen sowie API-Missbrauch nach einem abgeschlossenen Parse.

## Eigenständig reproduzieren

Voraussetzungen: Linux/Debian mit Git, Python 3, C++17-Compiler, Autoconf/Automake/Libtool, pkg-config und den ModSecurity-Entwicklungsabhängigkeiten. Der mitgelieferte [CI-Entwurf](../../test/json-baseline/repository-template/.github/workflows/json-tests.yml) nennt konkrete Debian/Ubuntu-Pakete und baut mit YAJL. Kein Apache- oder Nginx-Dienst ist für diese v3-API-Tests nötig. Die bisherige vollständige Build-Konfiguration und Debian-YAJL-Patches stehen im alten Herkunftsmanifest.

Aus dem Root eines vorbereiteten Test-Repositories, mit frischen, bislang nicht existierenden Zielverzeichnissen:

```sh
python3 test/json-baseline/tools/prepare_source.py --destination ../modsecurity-original --build
python3 test/json-baseline/tools/build.py --source ../modsecurity-original --output ../json-probes
python3 test/json-baseline/tools/run_all.py --probes ../json-probes --output ../json-characterization --profile characterization
python3 test/json-baseline/tools/run_all.py --probes ../json-probes --output ../json-requirements --profile requirements
```

Der letzte Befehl liefert beim untersuchten Stand absichtlich Exit 1. Für einen gemeinsamen Bericht dient `--profile all` mit einem neuen Ausgabeordner. Kein `continue-on-error`, kein XFAIL und keine beliebige Fehlerregex macht diese Anforderungen grün. Im CI-Entwurf laufen Charakterisierung und Anforderungen als getrennte Jobs; der Anforderungsjob wird rot, solange Erwartungen unerfüllt sind.

`prepare_source.py` klont ausschließlich in ein neues Verzeichnis aus dem Fork, liest den festen Commit aus dem offiziellen Repository und initialisiert die festgeschriebenen Submodule. Es ändert keine bestehende Arbeitskopie. `build.py` akzeptiert auch einen bereits gebauten vollständigen Checkout, wenn dessen tatsächliche Produktionsdateien und benötigte Submodule dem festen Original entsprechen. Bibliothekspfad, Hashes, Probe-Quellen, Compiler und eine aktive JSON-Kontrolle werden protokolliert. `run_all.py` prüft diese Nachweise erneut; eine andere installierte libModSecurity wird nicht stillschweigend akzeptiert.

Die tatsächlich ausgeführte Wiederverwendung des bestehenden Original-Builds und die Läufe aus beiden Export-Repositories stehen mit vollständigen Befehlen im neuen Manifest. Frischer Netzwerkabruf und vollständiger Neubau werden dort separat als ausgeführt oder nicht ausgeführt ausgewiesen. Die ursprünglichen Hilfs-Shellskripte sind aus Vollständigkeitsgründen archiviert und benötigen den vollständigen Source-Checkout; im eigenständigen Repository ist `tools/build.py` der passende Einstieg.

Jeder Lauf schreibt `summary.json`, Einzelberichte und getrennte stdout/stderr-Dateien in den expliziten Ausgabeordner. Fehlende Abhängigkeiten sind BLOCKED, Assertion-Fehler FAIL; ein leerer Filter oder fehlender Parsernachweis ist kein Erfolg. Body-Bytes werden hexadezimal übergeben; die Berichte enthalten Länge, Hexdarstellung und SHA-256. Erwartete Werte stehen fest im Code und werden nicht bei jedem Lauf aus der beobachteten Ausgabe erzeugt.

## Übergabe und Veröffentlichung

Die neuen GitHub-Repositories sind **vorbereitet**, nicht automatisch angelegt. Für `ModSecurity-JSON-Tests-DE` und `ModSecurity-JSON-Tests-EN` gibt es jeweils einen vollständigen lokalen Git-Stand, ein ZIP und ein geprüftes Git-Bundle. Die Pakete enthalten beide Sprachen; die Haupt-README startet jeweils mit Deutsch beziehungsweise Englisch. Code und Dokumentationsdateien sind ansonsten identisch. Die Archiv-/Bundle-Hashes stehen bei den Paketen im Exportmanifest.

Die ausdrückliche bisherige Push-Freigabe gilt für neue Aufgabenbranches im vorhandenen ModSecurity-Fork. Deshalb werden die geprüften Quelländerungen und Übergabepakete dort veröffentlicht; das Anlegen neuer GitHub-Repositories ist ein gesonderter Schritt. Der Abschlussbericht nennt den tatsächlich zurückgelesenen Fork-SHA und funktionierende Links. Weder bestehende Fork-Branches noch das offizielle Repository werden verändert.


Die ZIPs enthalten alle versionierten Dateien einschließlich Binärlogo und Fixtures.
Ein Bundle enthält zusätzlich die vollständige eigenständige Git-Historie dieser
Übergabe. Es lässt sich ohne Zugriff auf eine neue GitHub-Repositoryadresse prüfen:

```sh
git clone ModSecurity-JSON-Tests-DE.bundle ModSecurity-JSON-Tests-DE
git -C ModSecurity-JSON-Tests-DE bundle verify ../ModSecurity-JSON-Tests-DE.bundle
```

Für EN entsprechend den Dateinamen ersetzen. Nur ein neues Zielverzeichnis verwenden.
Das Exportmanifest verzeichnet beide Git-SHAs, alle Datei-Hashes und die überprüfte
Inhaltsgleichheit bis auf die Haupt-README. `export_repositories.py` ist ein Helfer
für den vollständigen Fork mit dessen Historie; im eigenständigen Test-Repository
werden nur `prepare_source.py`, `build.py` und `run_all.py` benötigt. Der optionale
Patch `patches/native-regressions.patch` gilt gegen den festen Originalcommit und
enthält native JSON-Fälle samt Test-Distributionsliste. Vor seiner Anwendung im
vollständigen Checkout das gesamte Verzeichnis `test/json-baseline/` aus dem Paket
übernehmen. Für die eigenständigen API-Läufe ist der Patch nicht erforderlich.


Der frische Beschaffungsweg ohne `--build` wurde vollständig ausgeführt: Fork-Klon, fester offizieller Commit und sieben rekursive Submodulstände sind verifiziert. Ein zweiter vollständiger Engine-Neubau und ein GitHub-Actions-Lauf wurden nicht ausgeführt; die Laufzeittests verwenden den vorhandenen, originalgleichen Build. Unveränderte Archivkopien behalten auch ihre ursprünglichen Leerzeichen; der Whitespace-Check der neuen übrigen Dateien besteht.
