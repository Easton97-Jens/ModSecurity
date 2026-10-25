# Konfiguration und SecRules anhand des Codes verstehen

## Geltungsbereich

Diese Erklärung gilt für den untersuchten offiziellen `v3/master`-Quellstand
`effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729`.
Sie dient der Orientierung in vorhandenen Implementierungen und Testfällen.
Sie ist weder eine vollständige Direktivenreferenz noch eine Aussage über
einen erfolgreich bereitgestellten Webserver.

## 1. Drei Ebenen unterscheiden

| Ebene | Zweck in diesem Repository | Einstieg in die Quellen |
| --- | --- | --- |
| Bibliothekskonfiguration | Direktiven setzen Eigenschaften des Regelsatzes, etwa Engine-Modus, Zugriff auf Bodies und Größenlimits. | [Parseraktionen für Engine-Modus und Body-Zugriff][engine-grammar], [Limits][limits-grammar] |
| SecRules | Regeln verbinden eine Variablenauswahl, einen Operator und Aktionen. Der Parser erzeugt dafür `RuleWithOperator`-Objekte. | [SecRule-Grammatik][rule-grammar] |
| Aufrufende Anwendung oder Connector | Liefert HTTP-Daten an die Bibliothek und entscheidet anhand ihrer Interventionen über die weitere Verarbeitung. Das v3-Hauptrepository enthält die Bibliothek; Connectors sind eigenständige Projekte. | [README: Connectors][connectors], [Transaktions-API][transaction-header] |

SecLang ist die Regelsprache, die dieses Projekt einliest. C++ ist die Sprache
der hier beschriebenen Engine-Implementierung. Eine textuelle Regel wird nicht
in ein eigenständiges C++-Programm übersetzt: Die Grammatik erzeugt Variablen-,
Operator- und Aktionsobjekte, die später anhand einer `Transaction` ausgewertet
werden. Quellen: [Objektaufbau im Parser][rule-grammar],
[RuleWithOperator::evaluate][rule-evaluation].

## 2. Der Engine-Modus beeinflusst das tatsächliche Verhalten

`modsecurity.conf-recommended` beginnt mit `SecRuleEngine DetectionOnly`.
Dies ist der Inhalt dieser Beispielkonfiguration, kein Beleg dafür, welche
Konfiguration eine Anwendung tatsächlich geladen hat. Der Parser unterscheidet
`Off`, `On` und `DetectionOnly`; `RuleWithActions::executeAction` führt disruptive
Aktionen nur in `EnabledRuleEngine` aus.
Quellen: [Beispielkonfiguration][recommended-engine],
[Engine-Grammatik][engine-grammar], [Aktionsausführung][actions].

| Wert | Was der Quellcode hier belegt |
| --- | --- |
| `On` | Der Parser setzt `EnabledRuleEngine`; die Prüfung des Engine-Zustands lässt disruptive Regelaktionen zu. |
| `Off` | Der Parser setzt `DisabledRuleEngine`; vorhandene Engine-Regressionen erwarten in ihrem Szenario mit deaktivierter Engine keine Blockierung. |
| `DetectionOnly` | Der Parser setzt `DetectionOnlyRuleEngine`; die zentrale Aktionsprüfung führt keine disruptiven Regelaktionen aus. |

Quellen: [Parserwerte][engine-grammar], [Aktionsprüfung][actions],
[Engine-Regressionen][engine-tests]. Die JSON-Erwartungen sind vorhandene
Testfälle; dieses Dokument belegt nicht, dass sie gerade ausgeführt wurden.

Die Aktion `ctl:ruleEngine=...` kann einen transaktionsspezifischen Zustand
setzen. `Transaction::getRuleEngineState()` verwendet diesen Zustand, wenn er
gesetzt ist, andernfalls den Zustand des Regelsatzes. Um eine konkrete Anfrage
zu erklären, kann es daher nötig sein, mehr als eine einzelne globale Direktive
zu prüfen.
Quellen: [ctl-Implementierung][ctl-engine],
[getRuleEngineState][engine-state].

## 3. Eine vorhandene Regel vollständig lesen

Die folgende Regel steht genau so in `modsecurity.conf-recommended`:

```apache
SecRule REQUEST_HEADERS:Content-Type "^application/json" \
     "id:'200001',phase:1,t:none,t:lowercase,pass,nolog,ctl:requestBodyProcessor=JSON"
```

Quelle: [JSON-Regel der Beispielkonfiguration][recommended-json].

| Bestandteil | Bedeutung in dieser Regel | Beleg im Code |
| --- | --- | --- |
| `SecRule` | Erzeugt eine Regel mit Variablen und einem Operator. | [Grammatikzweig][rule-grammar] |
| `REQUEST_HEADERS:Content-Type` | Wählt den benannten Eintrag aus der Collection der Anfrage-Header. | [Header-Variablen in der Grammatik][header-variables] |
| `"^application/json"` | Regulärer Ausdruck aus der Beispielkonfiguration. Operatorauflösung und Regex-Ausführung sind im Parser und in `Rx` implementiert. | [Operatorgrammatik][operators-grammar], [Rx][rx] |
| `id:'200001'` | Regelkennung. Bei Regeln, die nicht als Kettenglieder angehängt werden, prüft `Driver::addSecRule` auf fehlende IDs und doppelte IDs von Operatorregeln. | [Driver::addSecRule][driver-rules] |
| `phase:1` | Ordnet die Regel der Phase für Anfrage-Header zu. | [Phase::init][phases] |
| `t:none,t:lowercase` | Unterdrückt frühere Transformationen für diese Auswertung und führt danach die angegebene Umwandlung in Kleinbuchstaben aus. | [Transformationsablauf][transformations], [LowerCase-Implementierung][lowercase] |
| `pass` | Die Aktionsimplementierung gibt vorhandene Interventionsinhalte frei und setzt die Intervention zurück. Ihre Ausführung unterliegt der Prüfung für disruptive Aktionen. | [Pass][pass], [Aktionsprüfung][actions] |
| `nolog` | Setzt `ruleMessage.m_saveMessage` auf `false`. | [NoLog][nolog] |
| `ctl:requestBodyProcessor=JSON` | Setzt den angeforderten Body-Prozessor für die Transaktion. | [Body-Prozessor-Aktion][body-ctl] |

Bestimme zuerst die ausgewählte Eingabe, dann die Transformationen, den Operator
und schließlich die Aktionen und den Engine-Modus. Das entspricht
`RuleWithOperator::evaluate`: Variablen auswerten, Transformationen anwenden,
den Operator ausführen und bei Treffern weitere Aktionen ausführen.
Quelle: [RuleWithOperator::evaluate][rule-evaluation].

### Variablenlisten und Collections

Die Grammatik enthält Variablenlisten mit `PIPE` sowie Ausschluss- und
Zählmodifikatoren. Sie unterscheidet außerdem Formen wie `ARGS`,
`ARGS:<Element>` und die Auswahl per regulärem Ausdruck. Diese Stellen sind
Einstiegspunkte für eine Regel, die mehrere Eingaben untersucht, bestimmte
Ziele ausschließt oder eine Anzahl vergleicht. Ein vorhandenes Zählbeispiel
ist `SecRule &ARGS "@ge 1000"` in der empfohlenen Konfiguration.
Quellen: [Variablengrammatik][variable-grammar],
[Regel zur Argumentanzahl][arguments-rule].

`TX` besitzt für jede `Collections`-Instanz ein eigenes
`InMemoryPerProcess`-Objekt. Die anderen Collections `GLOBAL`, `IP`, `SESSION`,
`USER` und `RESOURCE` werden dem Collections-Objekt der Transaktion übergeben;
der `ModSecurity`-Konstruktor wählt für sie abhängig von `WITH_LMDB` das LMDB-
oder In-Memory-Backend. Nimm nicht an, dass alle Variablen dauerhaft gespeichert
oder prozessübergreifend geteilt werden.
Quellen: [Collections-Konstruktor][tx-collection],
[Backend-Auswahl][collections].

## 4. Phasen und Verfügbarkeit der Daten

Die Regelsprache ordnet die üblichen numerischen Phasenwerte wie folgt zu:

| SecRule-Einstellung | Vom Parser gewählter Phasenname |
| --- | --- |
| `phase:1` | `RequestHeadersPhase` |
| `phase:2` | `RequestBodyPhase` |
| `phase:3` | `ResponseHeadersPhase` |
| `phase:4` | `ResponseBodyPhase` |
| `phase:5` | `LoggingPhase` |

Quelle: [Zuordnung in Phase::init][phases]. Dieselbe Funktion akzeptiert auch
`request`, `response` und `logging` und ordnet sie den Regelsprache-Phasen 2, 4
und 5 zu. Zusätzlich erkennt sie `phase:0` als `ConnectionPhase`; das allein
belegt nicht, dass diese Phase bei einem bestimmten Connector-Aufruf ausgeführt
wird. Prüfe die Zuordnung, bevor du interne Enum-Werte mit den Nummern der
Regelsprache gleichsetzt. Die API-Abfolge steht in
[architecture.md](architecture.md).

Die Beispielkonfiguration wählt den JSON-Prozessor in Phase 1 aus.
`processRequestBody()` verarbeitet später abhängig von der Auswahl und den
einkompilierten Bibliotheken XML-, JSON- oder Multipart-Daten und setzt dabei
unter anderem Fehlervariablen. Eine Body-Regel kann nur die Daten beurteilen,
die dieser Verarbeitungspfad tatsächlich geliefert hat.
Quellen: [Auswahl des JSON-Prozessors][recommended-json],
[Body-Verarbeitung][body-processing].

## 5. Transformationen, Aktionen und Verkettung

### Transformationen

`executeTransformations()` beginnt mit einer Kopie des untersuchten Werts.
Die Funktion berücksichtigt Regelsatzvorgaben, die Transformationen der Regel
und Regelaktualisierungen. `t:none` beeinflusst, welche früheren Transformationen
ausgeführt werden. Mit `multiMatch` werden auch Zwischenwerte für
Operatorprüfungen gesammelt. Ohne `multiMatch` wird der endgültige Ergebniswert
am Ende hinzugefügt. Eine Erklärung muss daher die Reihenfolge der
Transformationen berücksichtigen.
Quelle: [executeTransformations][transformations].

### Aktionen und `SecDefaultAction`

Nicht alle Aktionen warten auf dieselbe Bedingung. Der Code unterscheidet unter
anderem Aktionen, die unabhängig vom Ergebnis einer nachfolgenden verketteten
Regel laufen, und Aktionen, die nach einem vollständigen Treffer ausgeführt
werden. `SetVar` gehört im untersuchten Ablauf zur ersten Gruppe. Die endgültige
disruptive Aktion wird an anderer Stelle behandelt.
Quellen: [executeActionsIndependentOfChainedRuleResult und executeActionsAfterFullMatch][actions],
[Aufrufreihenfolge im Regelauswerter][rule-evaluation].

`SecDefaultAction` ist im Parser kein beliebiger Textanhang. Neben weiteren
Prüfungen verlangt der Parser eine disruptive Aktion, verbietet `t:none`
innerhalb der Standardaktionen und lässt pro Phase und Konfigurationskontext
nur einen solchen Standardaktionssatz zu. Diese Einschränkungen sind für die
Diagnose von Ladefehlern relevant.
Quelle: [SecDefaultAction-Grammatik][default-actions].

### `chain`

Beim Anhängen einer verketteten Regel übernimmt der Treiber die Phase der
vorhergehenden Regel und weist disruptive Aktionen im angehängten Kettenglied
zurück. Zur Laufzeit ruft `RuleWithOperator::evaluate` die untergeordnete Regel
rekursiv auf; im gezeigten Pfad werden Aktionen nach einem vollständigen
Treffer nur erreicht, wenn auch dieser weitere Treffer gelingt. Daraus folgt
nicht, dass zuvor keine andere Aktion ausgeführt worden sein kann:
Unabhängige Aktionen werden vor der Kettenprüfung aufgerufen.
Quellen: [Verkettung im Treiber][driver-rules],
[Auswertungsreihenfolge][rule-evaluation], [Aktionsgruppen][actions].

## 6. Bodies, Limits und Fehlerzustände

Die Werte in `modsecurity.conf-recommended` sind konkrete Beispiele:

| Einstellung | Wert in der Datei |
| --- | --- |
| `SecRequestBodyAccess` | `On` |
| `SecRequestBodyLimit` | `13107200` |
| `SecRequestBodyNoFilesLimit` | `131072` |
| `SecRequestBodyLimitAction` | `Reject` |
| `SecRequestBodyJsonDepthLimit` | `512` |
| `SecArgumentsLimit` | `1000` |
| `SecResponseBodyAccess` | `On` |
| `SecResponseBodyMimeType` | `text/plain text/html text/xml` |
| `SecResponseBodyLimit` | `524288` |
| `SecResponseBodyLimitAction` | `ProcessPartial` |

Quellen: [Anfrageeinstellungen][recommended-request],
[Antworteinstellungen][recommended-response]. Diese Tabelle erfasst den Inhalt
der Datei. Sie empfiehlt keine Größen für deine Anwendung und beschreibt nicht
jeden internen Standardwert.

Bei Überschreitung eines Anfrage-Body-Limits setzt `appendRequestBody()`
`INBOUND_DATA_ERROR`. Für die konfigurierte Aktion `ProcessPartial` schreibt
der gezeigte Zweig nur so viel, wie noch Platz verfügbar ist, und gibt `false`
zurück.
Bei `Reject` setzt er nur bei aktivierter Regel-Engine eine disruptive
Intervention mit Status 403. Der Antwortpfad prüft entsprechend Größe und
Aktion und setzt `OUTBOUND_DATA_ERROR`.
Quellen: [Pufferung der Anfrage][request-limit],
[Pufferung der Antwort][response-limit].

**Kommentare und Implementierung unterscheiden:** Ein Kommentar in der
Beispielkonfiguration beschreibt einen automatischen Wechsel zu `ProcessPartial`
in `DetectionOnly`. Der zitierte Zweig in `appendRequestBody()` belegt jedoch
unmittelbar nur, dass er die konfigurierte Limit-Aktion prüft und außerhalb
von `EnabledRuleEngine` eine Zurückweisung unterdrückt. Diese Dokumentation
behauptet keinen allgemeinen automatischen Wechsel, der an dieser Stelle nicht
implementiert ist.
Quellen: [Konfigurationskommentar][recommended-request],
[implementierter Zweig][request-limit].

Bei XML- oder JSON-Fehlern setzt die Body-Verarbeitung Variablen wie
`REQBODY_ERROR`, `REQBODY_ERROR_MSG` und `REQBODY_PROCESSOR_ERROR`.
Die Beispielkonfiguration enthält ausdrückliche Regeln für diese Zustände.
Untersuche solche Fehlervariablen gemeinsam mit den geladenen Regeln; ein
fehlgeschlagener Parser-Vorgang ist nicht automatisch dasselbe Ereignis wie
eine vom Connector durchgeführte HTTP-Zurückweisung.
Quellen: [Fehlerbehandlung der Body-Prozessoren][body-processing],
[Regeln für Parserfehler][recommended-errors],
[Prüfung disruptiver Aktionen][actions].

## 7. Regex-Verhalten ist auch eine Build-Entscheidung

`Rx::init()` erzeugt für ein statisches Muster ein `Regex`; Parameter mit einem
Makro ergeben in `Rx::evaluate()` ein transaktionsspezifisches Muster. Ein
ungültiges statisches Muster kann daher schon beim Laden der Regel erkannt
werden. Die Auswertung berücksichtigt ein konfiguriertes `m_pcreMatchLimit`
und meldet Regex-Fehler über Transaktionsvariablen.
Quelle: [Rx-Implementierung][rx].

`kRxMultiLine` steht standardmäßig auf `true` und wird durch
`MODSEC_REGEX_DOLLAR_ENDONLY` auf `false` gesetzt. Der `Regex`-Konstruktor
verwendet dann für PCRE2 oder PCRE entweder `DOTALL | MULTILINE` oder
`DOTALL | DOLLAR_ENDONLY`. Die Build-Option
`--enable-regex-dollar-endonly=yes` setzt das entsprechende Makro.
Regressionen für diesen konkreten Build verwenden die Ressource
`regex-dollar-endonly`; ein in einem anderen Build übersprungener Fall
bestätigt dessen Semantik nicht.
Quellen: [Rx-Schalter][rx], [Regex-Konstruktor][regex-constructor],
[configure-Schalter][regex-option], [vorhandene Regressionen][regex-test].

## 8. Einige bekannte Direktiven sind nicht implementiert

Dass ein Direktivenname in Beispielen vorkommt, belegt nicht, dass sein
Verhalten vollständig implementiert ist. Die untersuchte Grammatik enthält
folgende Unterschiede:

| Direktive | Was die Grammatik tatsächlich tut | Quelle |
| --- | --- | --- |
| `SecRequestBodyInMemoryLimit` | Löst einen Parserfehler aus, der auf die Webserver-Konfiguration verweist. | [Grammatik, 1655–1662][limits-grammar] |
| `SecPcreMatchLimitRecursion` | Akzeptiert den Zweig ohne Wirkung im gezeigten Parsercode; die Meldung zur nicht unterstützten Funktion ist auskommentiert. Nicht mit dem implementierten `SecPcreMatchLimit` gleichsetzen. | [Grammatik, 1696–1707][limits-grammar] |
| `SecTmpDir` | Der gezeigte Zweig setzt keinen Wert; die Fehlermeldung zur nicht unterstützten Funktion ist auskommentiert. | [Grammatik, 1748–1758][compatibility] |
| `SecDataDir` | Auch hier ist keine Zuweisung implementiert; ein Kommentar verweist auf das Verhalten des Backends. | [Grammatik, 1759–1767][compatibility] |
| `SecStatusEngine` | Im gezeigten Grammatikzweig ist keine Aktion implementiert; die Fehlermeldung ist auskommentiert. | [Grammatik, 1781–1785][compatibility] |

Diese Tabelle beschreibt untersuchte Stellen. Sie ist keine vollständige Liste
der Unterschiede zu ModSecurity v2. Verfolge bei einer anderen Direktive zuerst
Scanner, Grammatikaktion, gespeicherte Eigenschaft und deren Verwendung zur
Laufzeit.

## 9. Konfiguration laden und Verhalten belegen

`Include` wird im Scanner behandelt: Der Code findet Ressourcen relativ zum
aktuellen Dateikontext, expandiert den aufgelösten Pfadausdruck und öffnet
weitere Scannerpuffer. Bei einem Fehler meldet er den betroffenen Dateipfad.
Ein fehlendes Include kann daher einen Ladefehler erklären, auch wenn eine
einzelne Regel syntaktisch korrekt erscheint.
Quelle: [Include-Behandlung][include].

Das Regelprüfprogramm lädt Dateien oder SecLang-Text über `RulesSet`, erzeugt
aber keine eigene HTTP-Transaktion. Erfolgreiches Laden belegt daher einen
anderen Teil des Verhaltens als ein Regressionstest. Vorhandene Regressionen
verbinden Regeln, Anfrage- und Antwortdaten sowie erwartete Ergebnisse;
`secruleengine.json` enthält zum Beispiel unterschiedliche Engine-Zustände und
erwartete HTTP-Codes.
Quellen: [Regelprüfprogramm][rules-check],
[Engine-Regressionen][engine-tests].

Daraus ergibt sich für Änderungen folgende Vorgehensweise:

1. Die betroffene Direktive oder Regel bis zu konkreten Implementierungen
   verfolgen.
2. Erforderliche Build-Fähigkeiten und den Engine-Modus festhalten.
3. Eine passende vorhandene Regression lesen; bei einer Verhaltensänderung
   den konkreten beobachtbaren Fall ergänzen.
4. Ladeprüfungen getrennt von Verhaltenstests berichten.
5. Ergebnisse und verbleibende Unsicherheiten im aktuellen Stand speichern.

Diese Schritte sind Arbeitsregeln des Projekts. Die zugehörigen, anhand des
Quellcodes geprüften Befehle stehen in
[build-and-test.md](build-and-test.md).

## Quellen
[engine-grammar]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/seclang-parser.yy#L1281-L1308
[limits-grammar]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/seclang-parser.yy#L1628-L1708
[rule-grammar]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/seclang-parser.yy#L1116-L1166
[connectors]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/README.md#L35-L49
[transaction-header]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/transaction.h
[rule-evaluation]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/rule_with_operator.cc#L199-L353
[recommended-engine]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/modsecurity.conf-recommended#L1-L7
[actions]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/rule_with_actions.cc#L202-L324
[engine-tests]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/test/test-cases/regression/secruleengine.json#L1-L123
[ctl-engine]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/actions/ctl/rule_engine.cc#L25-L59
[engine-state]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L1770-L1777
[recommended-json]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/modsecurity.conf-recommended#L25-L37
[header-variables]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/seclang-parser.yy#L2111-L2122
[operators-grammar]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/seclang-parser.yy#L914-L950
[rx]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/operators/rx.cc#L29-L100
[driver-rules]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/driver.cc#L76-L124
[phases]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/actions/phase.cc#L25-L72
[transformations]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/rule_with_actions.cc#L327-L441
[lowercase]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/actions/transformations/lower_case.cc
[pass]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/actions/disruptive/pass.cc#L32-L40
[nolog]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/actions/no_log.cc
[body-ctl]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/actions/ctl/request_body_processor_json.cc#L28-L35
[variable-grammar]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/seclang-parser.yy#L1854-L1978
[arguments-rule]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/modsecurity.conf-recommended#L60-L68
[tx-collection]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/collection/collections.cc#L37-L56
[collections]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/modsecurity.cc#L62-L78
[body-processing]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L694-L790
[default-actions]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/seclang-parser.yy#L1217-L1273
[recommended-request]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/modsecurity.conf-recommended#L10-L68
[recommended-response]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/modsecurity.conf-recommended#L163-L187
[request-limit]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L929-L971
[response-limit]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L1181-L1230
[recommended-errors]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/modsecurity.conf-recommended#L70-L98
[regex-constructor]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/utils/regex.cc#L90-L117
[regex-option]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L284-L292
[regex-test]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/test/test-cases/regression/operator-rx-dollar-endonly.json
[compatibility]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/seclang-parser.yy#L1748-L1785
[include]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/seclang-scanner.ll#L1285-L1342
[rules-check]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/tools/rules-check/rules-check.cc#L32-L93
