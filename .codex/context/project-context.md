# Projektkontext: ModSecurity v3 verstehen und damit arbeiten

## Verlässliche Grundlage

Diese Einführung beschreibt den offiziellen ModSecurity-Quellstand **`effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729`** auf `v3/master`. Die Quellen am Ende sind auf diesen Commit festgelegt. Die Erklärung bietet einen Einstieg; lies für eine konkrete Änderung anschließend die betroffenen Implementierungen.

`AGENTS.md`, `CODEX_START.md` und `.codex/` sind Arbeitskonventionen, die diesem Repository hinzugefügt wurden. Sie erklären den Code und halten Arbeitsergebnisse fest. Sie sind keine Funktionen der Upstream-Bibliothek.

## 1. Wozu ModSecurity dient

ModSecurity v3 stellt **libmodsecurity** bereit, eine Bibliothek zur regelbasierten Prüfung von HTTP-Daten. Der Code kann Regeln laden, Anfrage- und Antwortdaten entgegennehmen, Bedingungen auswerten sowie Meldungen und einen Interventionszustand erzeugen. Das zentrale Build-Ziel ist `libmodsecurity.la`. Quellen: [Projektbeschreibung][scope], [Bibliotheksziel][build], [Transaktions-API][transaction].

Unterscheide drei Komponenten:

| Komponente | Rolle | Wo diese Unterscheidung sichtbar wird |
| --- | --- | --- |
| **Bibliothek** | Verarbeitet übergebene Daten entsprechend dem geladenen Ruleset. | `Transaction`, `RulesSet` und ihre Implementierungen. [Quellen][transaction] [Regelauswertung][rules-evaluate] |
| **Connector oder aufrufende Anwendung** | Liefert der Bibliothek HTTP-Daten und reagiert im jeweiligen Server- oder Anwendungskontext auf ihre Ergebnisse. | Die API nimmt Daten als Parameter entgegen; Upstream beschreibt Connectors als separate Projekte. [Quellen][scope] [API-Beispiel][example] |
| **Ruleset und Konfiguration** | Bestimmt die zu verwendenden Variablen, Bedingungen, Aktionen, Phasen und Einstellungen. | SecLang-Grammatik und `modsecurity.conf-recommended`. [Quellen][grammar] [Konfiguration][config] |

Eine Bibliotheksänderung, eine neue Regel und eine Änderung an einem Webserver-Connector sind daher unterschiedliche Aufgaben. Auch die Beispielkonfiguration des Repositorys und ein externes Ruleset müssen getrennt betrachtet werden; die hier beschriebenen Quellen belegen den Inhalt dieser Bibliothek und ihrer mitgelieferten Dateien. Weiterführend: [Architektur](architecture.md) und [Regeln und Konfiguration](configuration-and-rules.md).

## 2. Welche Sprachen hier wichtig sind

Die Bibliothek wird mit **C++17** gebaut: `configure.ac` verlangt diesen Standard ausdrücklich, und der Windows-CMake-Build setzt ihn ebenfalls. Klassen wie `modsecurity::Transaction` und `modsecurity::RulesSet` werden in Headern unter `headers/modsecurity/` deklariert; ihre Implementierungen liegen überwiegend unter `src/`. Quellen: [C++17 mit Autotools][cxx17], [C++17 unter Windows][cxx17-win], [öffentliche API][transaction].

Zusätzlich gibt es eine **C-API** mit Funktionen wie `msc_init()`, `msc_new_transaction()` und `msc_process_request_body()`. Diese Funktionen delegieren an C++-Objekte. Ein Programm kann die C-Schnittstelle nutzen, ohne die C++-Klassen direkt aufzurufen. Daraus folgt keine vollständige Gleichwertigkeit sämtlicher Möglichkeiten beider Schnittstellen. Quellen: [C-Einstiegspunkt][c-engine], [Transaktions-Wrapper][c-transaction], [C-Deklarationen][c-api].

**SecLang beziehungsweise SecRules** ist die Eingabesprache für Regeln und Direktiven. Ihre Scanner- und Grammatikquellen sind `.ll`- und `.yy`-Dateien, die vom C++-Implementierungscode zu unterscheiden sind. Shell, JSON, Python, Lua und Build-Formate kommen in anderen Bereichen vor. Die genaue Zuordnung steht in der [Repository-Übersicht](repository-map.md). Quellen zur Regelsprache: [Parser-Build][parser-build], [Grammatik][grammar].

Der öffentliche Header enthält die Versionsmakros `3`, `0` und `17`, zusammen `3.0.17`. Dies ist eine Angabe im untersuchten Quellcode. Für sich genommen belegt sie weder eine veröffentlichte Release-Version dieses Commits noch die Version einer installierten Bibliothek. Quelle: [Versionsmakros][version].

## 3. Die drei Kernklassen

| Klasse | Einfache Erklärung | Konkrete Aufgaben im Code |
| --- | --- | --- |
| `ModSecurity` | Gemeinsam genutzter Bibliothekskontext. | Erzeugt Collection-Backends und hält Connector-Informationen sowie den Logging-Callback vor. [Konstruktor][engine], [API][engine-api] |
| `RulesSet` | Geladene Regeln und ihre Einstellungen. | `loadFromUri()` lädt eine Datei; `load()` lädt Regeltext; `evaluate()` wertet die Regeln einer Phase aus. [Laden][load], [Auswertung][rules-evaluate] |
| `Transaction` | Zustand einer einzelnen HTTP-Transaktion. | Nimmt Verbindungsdaten, URI, Header und Bodys entgegen; hält Variablen, Meldungen und Interventionen vor. [API][transaction], [Lebensdauer][lifetime] |

Der Aufrufer übergibt Engine und Ruleset an die Transaktion. Die Transaktion speichert die Zeiger, löscht die beiden Objekte aber nicht selbst. Diese Objekte müssen während ihrer Verwendung gültig bleiben. Das vorhandene Beispiel mit stückweise übergebenen Bodys zerstört zuerst die Transaktion, dann das Ruleset und zuletzt die Engine. Quellen: [Konstruktor und Destruktor][lifetime], [Beispiel][example]. Der [C++/C-Leitfaden](cpp-guide.md) erklärt die genauen Besitz- und Freigaberegeln.

## 4. Eingaben und Ergebnisse der Bibliothek

| Eingabe | Typischer API-Einstiegspunkt | Bedeutung |
| --- | --- | --- |
| Regeln und Direktiven | `RulesSet::loadFromUri()` oder `load()` | Baut das Ruleset und seine Einstellungen auf. [Quelle][load] |
| Verbindung und Anfragezeile | `processConnection()` und `processURI()` | Adressen, Ports, URI, HTTP-Methode und Protokollversion. [Quelle][transaction] |
| Anfrage-/Antwortheader | `addRequestHeader()` und `addResponseHeader()` | Übergibt Header-Namen und -Werte. [Quelle][transaction] |
| Anfrage-/Antwortbody | `appendRequestBody()` und `appendResponseBody()` | Übergibt Datenpuffer mit expliziten Längen; Größenbegrenzungen können bereits an dieser Stelle greifen. [Anfrage][append], [Antwort][response-body] |

| Ergebnis | Was es aussagt | Grenze dieser Aussage |
| --- | --- | --- |
| Ladeergebnis und Parserfehler | Ob der Ladepfad Regeln übernehmen konnte und welche Fehler er meldet. | Es prüft keine spätere HTTP-Transaktion. [Quelle][load] |
| Transaktionsvariablen und Trefferdaten | Welche Daten und Treffer die Auswertung ermittelt oder speichert. | Ihre Bedeutung hängt von der Phase und dem Auswertungspfad ab. [Quelle][rule-evaluate] |
| Intervention | Ein Zustand, den der Aufrufer abfragen muss und der unter anderem Status, URL, Logtext und ein Flag für den eingreifenden Zustand enthält. | Der Aufrufer muss die tatsächliche HTTP-Reaktion auf diesen Zustand umsetzen. [Quelle][intervention] |
| Regelmeldungen, Debug-Ausgabe und Audit-Ausgabe | Informationen über Treffer, Verarbeitung oder ausgewählte Transaktionen. | Die Ausgabepfade haben unterschiedliche Konfigurationen und Bedingungen. [Logging][logging], [Callback][callback] |

Ein erfolgreicher Methodenaufruf ist daher nicht automatisch eine Entscheidung, eine Anfrage zu erlauben. Beispielsweise kann `appendRequestBody()` im Ablehnungszweig eine Intervention setzen und trotzdem `true` zurückgeben. Interpretiere Rückgabewert und Intervention nach ihren jeweiligen Verträgen. Quelle: [Begrenzungspfad des Anfragebodys][append].

## 5. Typischer Transaktionsablauf

Das vorhandene Beispiel mit stückweise übergebenen Bodys zeigt einen vollständigen API-Ablauf. Die folgende Reihenfolge fasst ihn in einfachen Worten zusammen; ein tatsächlicher Connector kann die weitere HTTP-Verarbeitung nach einer Intervention beenden. Quelle: [Beispielprogramm][example].

1. Bibliothekskontext und Ruleset erzeugen; Regeln laden und auf Ladefehler prüfen.
2. Mit diesen Objekten eine Transaktion erzeugen; Verbindungsdaten, URI und Anfrageheader übergeben.
3. Die Anfrageheader-Phase ausführen; den Interventionszustand prüfen.
4. Daten des Anfragebodys übergeben; Body-Verarbeitung und Anfragebody-Phase aufrufen; auf eine Intervention prüfen.
5. Die entsprechenden Schritte für Antwortheader und Antwortbody ausführen, solange die Anwendung die Verarbeitung fortsetzt.
6. Logging abschließen und Transaktionsressourcen freigeben.

Die Methoden enthalten Bedingungen und vorzeitige Rückkehr. Beispielsweise überspringt `processLogging()` bei deaktivierter Engine sowohl die Logging-Phase als auch die Audit-Auswahl. Anfrage- und Antwortbodys haben eigene Bedingungen für Zugriff, Format und Begrenzungen. Quellen: [Logging][logging], [Anfrageverarbeitung][request-body], [Antwortverarbeitung][response-body].

Auch die Nummerierung erfordert Aufmerksamkeit: SecRules-Phasen und das interne C++-Enum stimmen nicht überein, weil das Enum zusätzlich `UriPhase` enthält. Die Aktionsklasse `Phase` übersetzt die Regelangabe; `processURI()` befüllt Daten, ohne `RulesSet::evaluate(UriPhase, ...)` aufzurufen. Die vollständige Tabelle steht in [architecture.md](architecture.md). Quellen: [Enum][phases], [Übersetzung][phase-action], [URI-Verarbeitung][uri].

## 6. Was „eine Regel auswerten“ im Code bedeutet

Eine Regel kann Daten über Variablen auswählen, diese Daten transformieren, eine Bedingung mit einem Operator prüfen und Aktionen ausführen. `RuleWithOperator::evaluate()` verbindet diese Schritte. Die Methode berücksichtigt außerdem Ausnahmen und Regelketten. Quellen: [Regelauswertung][rule-evaluate], [Transformationen][transformations].

Ein Operator prüft die konkrete Bedingung; eine Transformation ändert den Vergleichswert. Aktionen können beispielsweise eine Meldung ergänzen, einen Wert setzen oder eine Intervention anfordern. Bei Regelketten müssen Aktionen vor und nach einem vollständigen Kettentreffer getrennt betrachtet werden. Quellen: [Regelauswertung][rule-evaluate], [Aktionssteuerung][actions].

Die mitgelieferte Konfiguration beginnt mit `SecRuleEngine DetectionOnly`. In `executeAction()` laufen eingreifende Aktionen nur bei `EnabledRuleEngine`. Ein Treffer oder das bloße Vorhandensein einer `deny`-Aktion beweist daher nicht, dass eine Anfrage tatsächlich blockiert wurde. Quellen: [Konfiguration][config], [Engine-Bedingung für Aktionen][actions].

## 7. Sinnvoll lernen und Änderungen vorbereiten

| Ziel | Empfohlener Einstieg | Was anschließend im Code zu lesen ist |
| --- | --- | --- |
| Überblick gewinnen | [Repository-Übersicht](repository-map.md) | Build-Dateien im Wurzelverzeichnis, öffentliche Header und relevante Unterverzeichnisse. |
| HTTP-Datenfluss verstehen | [Architektur](architecture.md) | `src/transaction.cc` und ein vorhandenes API-Beispiel. |
| C++ und C richtig zuordnen | [C++/C-Leitfaden](cpp-guide.md) | Signatur, Implementierung, Aufrufer und Freigabepfad im Zusammenhang. |
| Eine Regel oder Direktive verstehen | [Konfiguration und Regeln](configuration-and-rules.md) | Scanner, Grammatik und konkrete Operator-/Aktionsimplementierung. |
| Einen Fehler oder eine Funktion finden | [Funktionsübersicht](feature-map.md) | Die dort zugeordneten Implementierungen und vorhandenen Tests. |
| Eigenen Build verifizieren | [Build und Tests](build-and-test.md) | `configure.ac`, der Plattform-Build und tatsächliche Testausgaben. |

Diese Lesereihenfolge ist eine Empfehlung für dieses Arbeitsrepository. Halte vor einer Codeänderung das gewünschte beobachtbare Verhalten fest und ermittle anschließend die relevanten Dateien und vorhandenen Prüfungen. Das Auffinden eines Symbols oder eines vorhandenen Testfalls ersetzt keine ausgeführte Prüfung des Verhaltens.

## 8. Wie neue Chats bestehende Arbeit fortsetzen

Die Arbeitskonvention besteht darin, zuerst `AGENTS.md`, den [Dokumentationsindex](../README.md) und den festgehaltenen aktuellen Arbeitsstand zu lesen und anschließend die für die Aufgabe relevanten Kapitel und Quellen zu öffnen. Übergib am Ende Ergebnisse, Belege, tatsächlich ausgeführte Prüfungen und verbleibende Schritte in nachvollziehbarer Form. Der [Sitzungsablauf](session-workflow.md) und die [Übergabevorlage](handoff-template.md) beschreiben diesen Prozess.

Diese Dateien stellen bewusst lesbaren Projektkontext bereit. Sie garantieren weder automatisches Erinnern über Chats hinweg noch Zugriff auf Dateien, die in einer neuen Sitzung nicht verfügbar sind. Unbekannte Tatsachen bleiben ungeklärt, bis die relevanten Quellen gelesen oder Prüfungen tatsächlich durchgeführt wurden.

## Quellen am festgelegten Commit

[scope]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/README.md#L21-L46
[build]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/Makefile.am#L11-L14
[transaction]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/transaction.h#L335-L419
[rules-evaluate]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/rules_set.cc#L121-L234
[example]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/examples/using_bodies_in_chunks/simple_request.cc#L95-L282
[grammar]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/seclang-parser.yy#L934-L1218
[config]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/modsecurity.conf-recommended#L1-L76
[cxx17]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L49-L63
[cxx17-win]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/build/win32/CMakeLists.txt#L102-L103
[c-engine]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/modsecurity.cc#L462-L482
[c-transaction]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L1804-L1904
[c-api]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/transaction.h#L677-L767
[parser-build]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/Makefile.am#L4-L43
[version]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/modsecurity.h#L191-L201
[engine-api]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/modsecurity.h#L275-L313
[engine]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/modsecurity.cc#L62-L109
[load]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/rules_set.cc#L50-L110
[lifetime]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L113-L182
[append]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L929-L971
[rule-evaluate]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/rule_with_operator.cc#L199-L360
[intervention]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L1350-L1390
[logging]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L1304-L1346
[callback]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/modsecurity.cc#L193-L212
[request-body]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L674-L870
[response-body]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L1117-L1230
[phases]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/modsecurity.h#L100-L171
[phase-action]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/actions/phase.cc#L25-L72
[uri]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L337-L443
[transformations]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/rule_with_actions.cc#L329-L441
[actions]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/rule_with_actions.cc#L202-L325
