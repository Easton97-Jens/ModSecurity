# C++ und C im ModSecurity-Quellcode verstehen

## Grundlage und Zweck

Dieser Leitfaden erklärt die tatsächlich im Repository verwendeten Sprachmerkmale und API-Grenzen auf Grundlage von `owasp-modsecurity/ModSecurity` am Commit **`effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729`**, Branch `v3/master`. Er dient dem Lesen und Ändern des Codes; er ist weder ein vollständiger C++-Kurs noch ein Nachweis für einen erfolgreichen Build. Alle verlinkten Implementierungen sind auf denselben Quellstand festgelegt.

Lies für den Verarbeitungsablauf der Bibliothek zuerst [architecture.md](architecture.md). Dieser Leitfaden konzentriert sich anschließend darauf, Funktionssignaturen, Datenbesitz, Rückgabewerte und vorhandene Codemuster richtig zu verstehen.

## 1. Welche Sprache wo verwendet wird

| Bereich | Belegte Verwendung | Bedeutung für die Arbeit |
| --- | --- | --- |
| C++17 | `configure.ac` verlangt `AX_CXX_COMPILE_STDCXX(17, noext, mandatory)`; der Windows-CMake-Build setzt `CMAKE_CXX_STANDARD 17`. | C++17 ist die belegte Grundlage für Sprachmerkmale und Compilerfehler. [Autotools][cxx17], [Windows][cxx17-win] |
| C++-Klassen und `.cc`-Dateien | `ModSecurity`, `RulesSet` und `Transaction` werden in öffentlichen Headern deklariert und in `src/` implementiert. | Header zeigen den Aufrufvertrag; Implementierungen zeigen das tatsächliche Verhalten. [Engine-Header][engine-api], [Ruleset-Header][rules-api], [Transaktions-Header][transaction-api] |
| C-Schnittstelle | `msc_*`-Funktionen werden mit `extern "C"` definiert; C-Aufrufer erhalten opake Typen. | C-Aufrufer greifen über Funktionen auf die C++-Objekte zu. [Opake Typen][c-types], [C-Wrapper][c-construct] |
| SecRules-Konfiguration | `modsecurity.conf-recommended` enthält unter anderem `SecRuleEngine`, `SecRule` und Aktionslisten. | Dies ist die Eingabesprache der Engine; eine Regeländerung ist nicht automatisch eine C++-Änderung. [Konfiguration][recommended] |
| Scanner und Parser | `src/parser/Makefile.am` führt `seclang-parser.yy` und `seclang-scanner.ll` als Quellen auf und erzeugt die entsprechenden Dateien. | Eine Sprachänderung beginnt bei der Grammatik oder dem Scanner und den daraus entstehenden Objekten. [Parser-Build][parser-build], [Driver][driver-rules] |

Die C++-API verwendet den Namensraum **`modsecurity`**, etwa in `modsecurity::RulesSet`. `::` qualifiziert den Typ mit seinem Namensraum. Ein README-Abschnitt enthält ältere Bezeichner wie `ModSecurity::Rules`; die aktuellen Header und ausführbaren Beispielquellen verwenden `modsecurity::RulesSet`. Übernimm diese README-Beispiele nicht ungeprüft als aktuelle Signaturen. Quellen: [aktuelle Deklarationen][rules-api], [aktuelles Beispiel][example-setup], [abweichendes README-Beispiel][readme-example].

## 2. Header, Implementierungen und öffentliche Schnittstelle

Eine Header-Deklaration beschreibt Namen, Parametertypen und Rückgabetyp. Der zugehörige Methodenkörper beschreibt, was bei einem Aufruf tatsächlich geschieht. Bei `RulesSet::loadFromUri(const char *uri)` steht die Deklaration in `headers/modsecurity/rules_set.h`; der Funktionskörper in `src/rules_set.cc` verwendet `Parser::Driver`, gibt Parsefehler weiter und vereinigt die geladenen Daten. Ein Kommentar kann einen Einstieg liefern, ersetzt diesen Vergleich aber nicht. Quellen: [Deklaration][rules-api], [Ladeimplementierung][load].

Viele Header unter `headers/modsecurity/` werden laut `src/Makefile.am` installiert. Änderungen dort können daher Nutzer der Bibliothek betreffen. Die ABI ist die binäre Schnittstelle, die bereits kompilierte Nutzer verwenden; an einigen Stellen bewahrt der Code aus ABI-Gründen ausdrücklich alte Signaturen, etwa bei `AnchoredSetVariable::resolveRegularExpression()`. **Arbeitsregel für neue Änderungen:** Behandle Signaturen, Klassenlayout und Besitzverhältnisse in diesen Headern nicht als rein lokale Implementierungsdetails. Quellen: [installierte Header][installed-headers], [ABI-Kommentar][abi].

## 3. Klassen, Konstruktoren und Lebensdauer

`ModSecurity` und `Transaction` besitzen Konstruktoren und Destruktoren. Ein Konstruktor stellt den Anfangszustand her; ein Destruktor gibt verwaltete Ressourcen frei. Beide Klassen löschen Kopieroperationen ausdrücklich mit `= delete`. Dadurch wird beispielsweise verhindert, dass eine normale Kopie eine Transaktion mit denselben Ressourcenzeigern dupliziert. Quellen: [ModSecurity][engine-api], [Transaction][transaction-api].

Ein vorhandenes Beispiel für automatische Objektlebensdauer ist `test/common/modsecurity_test_context.h`: Der Kontext enthält `ModSecurity` und `RulesSet` als direkte Member; `create_transaction()` erzeugt eine Transaktion mit Zeigern auf diese Member. Die Methode gibt ein neu konstruiertes Objekt direkt zurück. Lies diesen Code im Zusammenhang mit der verlangten C++17-Version; ein gelöschter Kopierkonstruktor ist kein Grund, zur Fehlerbehebung eine Kopieroperation hinzuzufügen, ohne zuerst die Ursache zu untersuchen. Quellen: [Testkontext][test-context], [C++17-Anforderung][cxx17].

### Wem gehört welche Ressource?

Die folgende Tabelle unterscheidet tatsächliche Besitzverwaltung von bloßen Verweisen. Die Anforderungen an die Lebensdauer geliehener Zeiger werden aus Konstruktoren, Destruktoren und Aufrufern abgeleitet.

| Ressource | Erzeugung oder Übergabe | Freigabe oder Lebensdauer |
| --- | --- | --- |
| Engine-Collections | `ModSecurity` erzeugt Collection-Backends mit `new`. | `ModSecurity::~ModSecurity()` löscht sie. [Quelle][engine-life] |
| `Transaction::m_ms`, `m_rules` | Der Aufrufer liefert Engine und Ruleset; der Konstruktor speichert die Zeiger. | Die Transaktion löscht sie nicht. Beide müssen während ihrer Verwendung gültig bleiben. [Quelle][transaction-life] |
| JSON-/XML-Prozessoren der Transaktion | Der Konstruktor erzeugt sie abhängig von Build-Makros. | Der Destruktor löscht sie unter denselben Makros. [Quelle][transaction-life] |
| Transaktions-Collection `TX` | `Collections` erzeugt `InMemoryPerProcess("TX")`. | `Collections::~Collections()` löscht sie; die übrigen Collection-Zeiger werden von außen übergeben. [Quelle][collections] |
| Regeln in `Rules` | `std::vector<std::shared_ptr<Rule>>`. | Die Smart Pointer verwalten die von ihnen gehaltenen Regelobjekte. [Quelle][rules-storage] |
| Neues Regelobjekt des Parsers | `Driver::addSecRule(std::unique_ptr<RuleWithActions>)`. | Der Besitz geht für die Speicherung in einer Phase oder als Kettenkind in `std::shared_ptr` über. [Driver][driver-rules], [Kettenfelder][chain-fields] |
| Operator in einem `RuleWithOperator` | Der Konstruktor übernimmt einen `Operator *`. | `RuleWithOperator::~RuleWithOperator()` löscht ihn und die übernommenen Variablenobjekte. [Quelle][operator-life] |
| Transformations- und Aktionsobjekte | `RuleWithActions` hält mehrere Zeiger und Zeigercontainer. | Der Destruktor löscht die dort verwalteten Objekte. [Quelle][actions-life] |

`std::unique_ptr` drückt exklusiven Besitz aus; `std::shared_ptr` erlaubt gemeinsamen Besitz. Ein roher Zeiger allein sagt nicht, wer das Objekt freigeben muss. Das Repository verwendet alle drei Formen. Eine mechanische Umstellung sämtlicher roher Zeiger würde daher die tatsächlichen Besitzgrenzen verfehlen. Lies zuerst Erzeugung, Speicherung, Übergabe und Freigabe im Zusammenhang. Die Tabelle liefert belegte Beispiele; diese Empfehlung ist eine daraus abgeleitete Arbeitsweise.

### `const` bedeutet nicht „muss nie freigegeben werden“

`AnchoredSetVariable::resolve()` allokiert Kopien mit `new VariableValue(...)` und legt Zeiger darauf in einem `std::vector<const VariableValue *>` ab. Die Regelverarbeitung löscht diese Elemente nach der Auswertung ausdrücklich. Hier beschränkt `const` die Änderung des Objekts über diesen Zeiger; es beschreibt nicht den Ressourcenbesitz. `resolveFirst()` hat einen anderen Vertrag und liefert einen `std::unique_ptr<std::string>`. Quellen: [Allokation in resolve()][resolve], [Verwendung und Löschen][evaluate], [resolveFirst()][resolve-first].

`const std::string&` ist eine Referenz auf eine vorhandene Zeichenkette. Beispielsweise liefern `VariableValue::getValue()` und `getKey()` Referenzen auf Member zurück. Eine solche Referenz darf nach dem Löschen des `VariableValue` nicht mehr verwendet werden. Beachte diese Lebensdauer bei Änderungen an der Auswertungsschleife. Quellen: [Getter und Member][variable-value], [Löschstellen][evaluate].

## 4. Zeichenketten, Bytes und Längen

Die API unterscheidet mehrere Formen von Texteingaben. `processURI()` nimmt nullterminierte C-Zeichenketten entgegen. Die Header-Überladung ohne Längen berechnet die Größen mit `strlen()`. Die Überladung mit `key_n` und `value_n` übergibt dagegen genau diese Längen an `std::string::assign()`. Für C-Aufrufer besteht dieselbe Unterscheidung zwischen `msc_add_request_header()` und `msc_add_n_request_header()` sowie den entsprechenden Funktionen für Antwortheader. Quellen: [URI-Verarbeitung][uri], [Header-Überladungen][header-lengths], [C-Deklarationen][c-trans-api].

| Eingabeform | Verhalten im Code | Konsequenz für einen Aufrufer |
| --- | --- | --- |
| `const char *` für URI, Methode und HTTP-Version | Wird als C-Zeichenkette verwendet. | Gültige Nullterminierung und Lebensdauer für den Aufruf sicherstellen. [Quelle][uri] |
| Header-Zeiger ohne Länge | Verwendet `strlen()`. | Nicht für einen beliebigen, nicht nullterminierten Ausschnitt aus Netzwerkdaten verwenden. [Quelle][header-lengths] |
| Header-Zeiger mit `size_t`-Längen | Erzeugt Zeichenkettenwerte mit expliziten Längen. | Die tatsächlich gültigen Puffergrenzen übergeben. [Quelle][header-lengths] |
| Body-Zeiger mit Länge | `appendRequestBody()` schreibt `len` Bytes oder die verbleibende Kapazität innerhalb der Begrenzung in den Stream. | Die übergebene Byteanzahl bestimmt die Eingabe; Rückgabewert und Intervention getrennt prüfen. [Quelle][request-append] |

`reinterpret_cast<const char *>(buf)` im Append-Pfad ändert den Zeigertyp für den Stream-Aufruf; es führt keine URL-, Unicode- oder Zeichensatzdekodierung durch. Die URI-Dekodierung erfolgt ausdrücklich durch `utils::uri_decode()` in `processURI()`. Erkläre Änderungen der Eingabeinterpretation anhand der jeweiligen Verarbeitung und nicht allein anhand eines Casts. Quellen: [Append-Pfad][request-append], [URI-Dekodierung][uri].

`processURI()` hält sowohl rohe als auch abgeleitete URI-Daten vor und berechnet Offsets. `VariableValue` kann Herkunftsangaben enthalten, die später in Trefferreferenzen eingehen. Ein scheinbar einfacher Wechsel von einer rohen zu einer dekodierten Zeichenkette kann daher gleichzeitig Vergleichswerte, Variablennamen und Diagnosepositionen beeinflussen. Dies ist eine anhand des Datenflusses zu untersuchende Frage, keine Behauptung über einen konkreten Fehler. Quellen: [URI][uri], [VariableValue][variable-value], [Trefferverarbeitung][evaluate].

## 5. Vererbung und virtuelle Aufrufe

`Rule` definiert virtuelle `evaluate()`-Methoden und einen virtuellen Destruktor. `RuleWithActions` erweitert diesen Typ; `RuleWithOperator` ergänzt Operator und Zielvariablen. So kann `RulesSet::evaluate()` die konkrete Auswertung über einen allgemeinen Regelzeiger aufrufen. Der Code verwendet außerdem `dynamic_cast`, beispielsweise um zu prüfen, ob eine Regel Aktionsmetadaten besitzt. Quellen: [Basisklasse Rule][rule-base], [RuleWithActions][chain-fields], [konkreter Operator][operator-life], [Phasenschleife][rules-evaluate].

Ein neuer Regeltyp wirft deshalb mehrere getrennte Fragen auf: Wie erzeugt der Parser das Objekt? Welcher Phase wird es zugeordnet? Welche virtuelle Auswertung läuft? Wem gehören die beteiligten Objekte? Ein neuer Name in der Grammatik allein beantwortet diese Fragen nicht. Die vorhandenen Aufrufstellen sind der Einstiegspunkt für eine solche Änderung. Quellen: [Driver][driver-rules], [Phasenschleife][rules-evaluate], [Operatorauswertung][evaluate].

## 6. Rückgabewerten nicht pauschal dieselbe Bedeutung zuweisen

Das Repository hat keinen einheitlichen universellen Vertrag für `int`-Rückgabewerte. Lies die Bedeutung für jede Funktion einzeln nach. Insbesondere belegt ein erfolgreicher API-Aufruf weder, dass die Anfrage erlaubt ist, noch, dass eine bestimmte Regelphase tatsächlich ausgeführt wurde.

| Funktion/Pfad | Belegtes Ergebnis | Häufige Fehlinterpretation |
| --- | --- | --- |
| `RulesSet::loadFromUri()`, `load()` | `-1` bei den dort behandelten Fehlern; andernfalls die Anzahl der übernommenen Regeln. [Quelle][load] | `0` in jedem Fall als Fehler behandeln oder die Anzahl mit einer Entscheidung verwechseln, eine Anfrage zu erlauben. |
| `processRequestHeaders()` | Gibt auch bei deaktivierter Engine `true`/`1` zurück. [Quelle][request-phase] | Aus `1` schließen, dass Regeln ausgeführt wurden. |
| `appendRequestBody()` | Der Zweig für Teilverarbeitung gibt `false` zurück; ein Ablehnungszweig kann eine Intervention setzen und trotzdem `true` zurückgeben. [Quelle][request-append] | Nur den Rückgabewert prüfen und die Intervention übersehen. |
| `Transaction::intervention()` | Liefert den eingreifenden Zustand; bei `false` schreibt der Funktionskörper nicht in die Ausgabestruktur. [Quelle][intervention] | Rückgabewert und Struktur unabhängig voneinander interpretieren. |
| `msc_rules_add_file()` | Reicht das Ladeergebnis durch; bei einem negativen Ergebnis liefert die Funktion eine mit `strdup` allokierte Fehlermeldung. [Quelle][c-rules] | Einen Fehlerpuffer wie eine statische Zeichenkette behandeln. |

Lies auch Konstruktor-Wrapper sorgfältig: Im untersuchten Code verwenden `msc_init()`, `msc_create_rules_set()` und `msc_new_transaction()` schlicht `new`. Der Transaktionskommentar beschreibt einen möglichen Rückgabewert `NULL`; der Wrapper enthält jedoch weder `new (std::nothrow)` noch einen `catch`-Block. Dokumentiere hier keine allgemeine Garantie, dass alle C++-Fehler automatisch in einen C-Fehlercode übersetzt werden. Quellen: [Engine-Erzeugung][c-engine], [Ruleset-Erzeugung][c-rules], [Transaktionskommentar und Wrapper][c-construct].

## 7. Zuordnung der C++-API zur C-API

Die C-Wrapper delegieren überwiegend an die C++-Objekte. Für C bleiben die Objektstrukturen verborgen; die `msc_*`-Funktionen ermöglichen den Zugriff über Zeiger. `extern "C"` legt für diese Funktionen die C-Sprachbindung fest; es bedeutet nicht, dass ihre Implementierung in C geschrieben ist. Quellen: [C-Typen][c-types], [Wrapper][c-engine], [Transaktions-Wrapper][c-construct].

| Aufgabe | C++-Einstiegspunkt | C-Einstiegspunkt |
| --- | --- | --- |
| Engine erzeugen und zerstören | `ModSecurity`, Destruktor | `msc_init()`, `msc_cleanup()` |
| Connector kennzeichnen | `setConnectorInformation()` | `msc_set_connector_info()` |
| Ruleset erzeugen und zerstören | `RulesSet`, Destruktor | `msc_create_rules_set()`, `msc_rules_cleanup()` |
| Regeldatei laden | `loadFromUri()` | `msc_rules_add_file()` |
| Regeln aus Text laden | `load()` | `msc_rules_add()` |
| Transaktion erzeugen | `Transaction(ms, rules, logCbData)` | `msc_new_transaction()` |
| Eigene Transaktions-ID übergeben | `Transaction(ms, rules, id, logCbData)` | `msc_new_transaction_with_id()` |
| Verbindungs- und URI-Daten übergeben | `processConnection()`, `processURI()` | `msc_process_connection()`, `msc_process_uri()` |
| Body-Daten anhängen | `appendRequestBody()`, `appendResponseBody()` | `msc_append_request_body()`, `msc_append_response_body()` |
| Regelphasen ausführen | `processRequestHeaders()`, `processRequestBody()`, `processResponseHeaders()`, `processResponseBody()` | Entsprechende `msc_process_*`-Funktionen mit C-Namensgebung |
| Intervention abfragen | `intervention()` | `msc_intervention()` |
| Logging abschließen | `processLogging()` | `msc_process_logging()` |
| Transaktion zerstören | Destruktor | `msc_transaction_cleanup()` |

Quellen für die Zuordnung: [Engine-Deklarationen][engine-api], [Ruleset-Deklarationen][rules-api], [C++-Transaktion][transaction-api], [C-Transaktion][c-trans-api]. Die Tabelle wählt zentrale Aufrufe aus; die Header enthalten weitere Funktionen. Sie behauptet keine vollständige funktionale Gleichwertigkeit der C- und C++-Schnittstellen.

### Die jeweils zugehörigen Freigabefunktionen verwenden

`msc_rules_error_cleanup()` verwendet `free()` für den von den Regel-Wrappern allokierten Fehlertext. `msc_intervention_cleanup()` gibt die Puffer einer Intervention frei und setzt ihre Felder zurück. Engine, Ruleset und Transaktion verwenden dagegen ihre jeweiligen Objekt-Freigabefunktionen, die intern `delete` ausführen. Ersetze `free()` nicht allein aufgrund des sichtbaren Zeigertyps durch `delete` oder umgekehrt. Quellen: [Regelfehler und Freigabe][c-rules], [Interventionsfreigabe][c-cleanup], [Engine-Freigabe][c-engine].

Ein weiteres Gegenbeispiel zur Annahme, `const char *` bedeute immer einen geliehenen Puffer: Im untersuchten Code gibt `Transaction::getResponseBody()` einen mit `strdup()` allokierten Puffer zurück, während `msc_who_am_i()` einen `c_str()`-Zeiger in die Zeichenkette der Engine liefert. Diese beiden Rückgabewerte haben unterschiedliche Speicherursprünge. Der Antwort-Getter wird in seinem Kommentar außerdem als ungenutztes Grundgerüst beschrieben; er darf nicht als Beleg für umfassendes Umschreiben von Antworten oder die Verarbeitung beliebiger Binärdaten dienen. Quellen: [Antwort-Getter][response-getter], [who_am_i][c-engine].

## 8. Callbacks: Typ und Lebensdauer gehören zusammen

`ModSecLogCb` hat die Form `void (*)(void *, const void *)`. Die Engine übergibt Benutzerdaten und entweder Text oder eine `RuleMessage`. Die C++-Überladung `setServerLogCb(cb, properties)` bestimmt die Darstellung. Die Überladung ohne Eigenschaften wählt `TextLogProperty`; `msc_set_log_cb()` verwendet genau diese Variante. Interpretiere einen `void *`-Zeiger nur entsprechend der tatsächlich ausgewählten Eigenschaft. Quellen: [Callback-Typ und Eigenschaften][callback-api], [Eigenschaftsauswahl und C-Wrapper][callback-setup].

Im Textpfad erzeugt `ModSecurity::serverLog()` eine lokale Zeichenkette und übergibt während des Callbacks deren `c_str()`. Im Objektpfad übergibt die Methode die Adresse einer `RuleMessage`; die Regelverarbeitung dokumentiert deren begrenzte Lebensdauer ausdrücklich und kopiert Meldungen für die spätere Audit-Verwendung. **Abgeleitete Regel für Aufrufer:** Kopiere Daten innerhalb des Callbacks, wenn sie nach dessen Rückkehr noch benötigt werden; bewahre diese Zeiger nicht auf, ohne ihre Lebensdauer zu prüfen. Quellen: [Callback-Ausführung][callback-call], [Lebensdauer von RuleMessage][message-life].

`logCbData` wird bei der Erzeugung der Transaktion übergeben. Der Testkontext verwendet die Adresse seines `std::stringstream`; der Callback wandelt sie in den passenden Typ zurück und schreibt die Meldung hinein. Dieses konkrete Beispiel zeigt, warum Callback-Funktion, Kontexttyp und Kontextlebensdauer zusammenpassen müssen. Quellen: [Testkontext][test-context], [Transaktionskonstruktor][transaction-life].

## 9. Präprozessor und Build-Varianten

Makros bestimmen, welche Implementierungen im Code enthalten sind. Beispiele sind `WITH_LMDB` für die Auswahl der Collection-Backends, `WITH_LIBXML2` und `WITH_YAJL` für Body-Prozessoren sowie `NO_LOGS` für Debug-Makros. Das erfolgreiche Lesen eines Quellpfads beweist nicht, dass dieser in einer bestimmten Binärdatei enthalten ist. Prüfe bei plattform- oder funktionsbezogenen Fragen die Build-Konfiguration und die tatsächlichen Compilerdefinitionen. Quellen: [Engine-Auswahl][engine-life], [Prozessorauswahl][transaction-life], [Debug-Makros][c-types].

Auch die SecRules-Grammatik ist mehr als eine Liste vorgesehener Funktionen. Sie enthält ausdrücklich nicht unterstützte Aktionen, etwa `ACTION_APPEND` mit `ACTION_NOT_SUPPORTED("Append", ...)`. Ein Token oder Name im Repository ist deshalb kein ausreichender Beleg dafür, dass die Funktion verfügbar ist. Quelle: [Grammatikfall][unsupported-action].

## 10. Vorhandene Beispiele sinnvoll nutzen

| Quelle | Hilfreich für | Zu beachten |
| --- | --- | --- |
| `examples/using_bodies_in_chunks/simple_request.cc` | API-Reihenfolge, Chunk-Eingabe, Interventionsprüfungen und Freigabereihenfolge. | Ein Demonstrationsprogramm, dessen Hilfsfunktion Ausgaben erzeugt; es belegt nicht die HTTP-Wirkung eines produktiven Connectors. [Quelle][example-flow] |
| `test/common/modsecurity_test_context.h` | Engine und Ruleset als Member, Transaktionserzeugung und einen Text-Callback. | Die zurückgegebene Transaktion verweist auf den Kontext. [Quelle][test-context] |
| `examples/simple_example_using_c/test.c` | C-Namen, Laden einer Regeldatei und Fehlertext. | Der gezeigte Freigabepfad enthält kein `msc_transaction_cleanup(transaction)`; übernimm ihn nicht als vollständiges Muster für die Ressourcenverwaltung. [Quelle][c-example] |
| C++-Auszug der README | Eine historische Einführung in das API-Konzept. | Namen und Argumentlisten mit den aktuellen Headern vergleichen; sie stimmen hier nicht vollständig überein. [Quelle][readme-example] |

## 11. Lesereihenfolge vor einer konkreten Änderung

Die folgende Reihenfolge ist eine Empfehlung für dieses Arbeitsrepository:

1. Finde die öffentliche Signatur und ihre tatsächlichen Aufrufer; vergleiche Namen und Parametertypen.
2. Lies die konkrete Implementierung einschließlich vorzeitiger Rückkehr, Build-Makros und Fehlerzweige.
3. Verfolge für jeden betroffenen Zeiger Erzeugung, Besitzübertragung und Freigabe.
4. Folge dem Datenfluss bis zur Regelphase, Intervention oder Logausgabe und beachte dabei die beiden Phasennummerierungen in [architecture.md](architecture.md).
5. Lies die relevanten vorhandenen Tests und Build-Anweisungen und halte den tatsächlich geprüften Umfang fest.
6. Halte ungeklärte Punkte als ungeklärt fest. Das Auffinden eines Symbolnamens, Kommentars oder Beispiels ersetzt keinen Beleg dafür, dass das Verhalten funktioniert.

Diese Empfehlung folgt aus den oben belegten Unterschieden zwischen Headern, Implementierungen und Beispielen. Sie behauptet weder ungeprüfte Sprachregeln noch unbelegte Thread-Sicherheit oder automatisch erfolgreiche Tests.

[cxx17]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L49-L63
[cxx17-win]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/build/win32/CMakeLists.txt#L102-L103
[engine-api]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/modsecurity.h#L275-L331
[rules-api]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/rules_set.h#L49-L107
[transaction-api]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/transaction.h#L335-L419
[c-types]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/transaction.h#L20-L80
[c-trans-api]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/transaction.h#L677-L767
[c-construct]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L1804-L1829
[c-engine]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/modsecurity.cc#L411-L482
[c-rules]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/rules_set.cc#L277-L345
[c-cleanup]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L2153-L2195
[load]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/rules_set.cc#L50-L109
[recommended]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/modsecurity.conf-recommended#L1-L76
[parser-build]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/Makefile.am#L1-L43
[driver-rules]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/driver.cc#L46-L124
[chain-fields]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/rule_with_actions.h#L38-L109
[readme-example]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/README.md#L233-L255
[example-setup]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/examples/using_bodies_in_chunks/simple_request.cc#L131-L170
[example-flow]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/examples/using_bodies_in_chunks/simple_request.cc#L95-L282
[installed-headers]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/Makefile.am#L37-L67
[abi]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/anchored_set_variable.cc#L124-L139
[engine-life]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/modsecurity.cc#L62-L109
[transaction-life]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L113-L182
[collections]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/collection/collections.cc#L37-L56
[test-context]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/test/common/modsecurity_test_context.h#L13-L39
[rules-storage]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/rules.h#L41-L87
[operator-life]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/rule_with_operator.cc#L54-L79
[actions-life]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/rule_with_actions.cc#L141-L178
[resolve]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/anchored_set_variable.cc#L70-L111
[resolve-first]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/anchored_set_variable.cc#L114-L122
[evaluate]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/rule_with_operator.cc#L199-L353
[variable-value]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/variable_value.h#L38-L125
[uri]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L337-L443
[header-lengths]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L618-L654
[request-append]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L929-L971
[rule-base]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/rule.h#L65-L105
[rules-evaluate]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/rules_set.cc#L121-L232
[request-phase]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L461-L471
[intervention]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L1350-L1389
[response-getter]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L1234-L1249
[callback-api]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/modsecurity.h#L205-L267
[callback-setup]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/modsecurity.cc#L389-L413
[callback-call]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/modsecurity.cc#L193-L212
[message-life]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/rule_with_actions.cc#L495-L525
[unsupported-action]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/seclang-parser.yy#L2726-L2729
[c-example]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/examples/simple_example_using_c/test.c#L26-L76
