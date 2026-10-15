# So verarbeitet ModSecurity einen JSON-Request

Diese Analyse betrifft unveränderten Produktionscode des offiziellen
Repositories `owasp-modsecurity/ModSecurity` auf genau diesen Referenzen:

| Linie | Originalbranch | Originalcommit |
| --- | --- | --- |
| v3 | `v3/master` | `8271c9d3bc0abcbd1189f5414c196c336c959047` |
| v2 | `v2/master` | `151074aca78a416764b12ba1221e61a17a6c714d` |

**Belegarten:** „statisch“ bedeutet am festgelegten Originalcode festgestellt.
Das beweist keinen ausgeführten Test. Die
[versionsgetrennten Laufberichte](JSON-YAJL-BASELINE.md#lesen-und-reproduzieren)
und Testmatrizen nennen die tatsächlich ausgeführten Fälle,
Erwartungen und Ergebnisse. Die Versionslinien haben unterschiedliche
Variablennamen und Tiefenzählung; eine Bibliotheksablösung ist nicht Bestandteil
dieser Arbeit. YAJL bleibt in beiden Linien unverändert.

## 1. Einstieg ohne C- oder C++-Kenntnisse

Der **Body** ist die Folge von Bytes hinter den HTTP-Headern. Ein **Parser**
erkennt darin JSON-Strukturen und Werte. YAJL meldet erkannte Bestandteile über
**Callbacks**, also von ModSecurity bereitgestellte Rückruffunktionen. Dabei
baut ModSecurity keine vollständige Kopie des JSON-Dokuments als Objektbaum
(DOM) auf: Es bildet Namen für die skalaren Werte, also Strings, Zahlen,
Booleans und null, und speichert diese Werte als Argumente.

**ARGS** ist die Sammlung, auf die Regeln zugreifen. Sie enthält auch Argumente
aus dem Query-String der URL. Eine **Regelphase** legt fest, wann die Engine
Regeln prüft: Phase 1 verarbeitet Header, Phase 2 den Request-Body.
Ein **Limit** begrenzt beispielsweise Body-Größe, Verschachtelung oder Anzahl
der Argumente. Der **Parserabschluss** meldet das Ende der Eingabe an YAJL.
Erst dann ist beispielsweise erkennbar, dass hinter `1` keine weitere Ziffer
mehr folgt oder dass eine geöffnete Struktur unvollständig geblieben ist.

Der Verarbeitungspfad lautet vereinfacht:

```text
Webserver / Connector liefert Header und Body-Bytes
  -> Phase-1-Regel wählt JSON-Prozessor
  -> Body speichern und an YAJL übergeben (v2) bzw. erst sammeln (v3)
  -> YAJL-Callbacks bilden Namen und speichern skalare Werte in ARGS
  -> YAJL-Abschluss erkennt verbleibende / unvollständige Eingabe
  -> ModSecurity setzt Fehlerzustand
  -> Phase-2-Regeln lesen ARGS und REQBODY_ERROR
  -> Webserver / Connector setzt eine angeforderte Intervention um
```

`Content-Type: application/json` allein schaltet im Enginecode den
JSON-Prozessor nicht automatisch ein. Die mitgelieferte Beispielkonfiguration
enthält dafür eine Phase-1-Regel mit `ctl:requestBodyProcessor=JSON`.
Diese Regel ist Konfiguration, kein eingebautes Erkennungsverfahren.
[v3: Auswahl in `addRequestHeader`, Zeilen 557–589][v3-header-selection],
[v3: Beispielkonfiguration, Zeilen 19–37][v3-example-config],
[v2: automatische Auswahl, Zeilen 404–439][v2-selection].

### Ein kleines Objekt

Für `{"user":"Ada","enabled":true,"count":1.2300,"optional":null}`
geschieht Folgendes. Die vier Einträge sind zusätzlich in beiden Versionslinien durch `K-intro-object` zur Laufzeit bestätigt:

1. `{` eröffnet den Objektkontext.
2. `user` wird der aktuelle Schlüssel; der String-Callback liefert `Ada`.
3. Der Boolean-Callback liefert den Text `true`.
4. Der Zahlen-Callback liefert genau `1.2300`, ohne Umwandlung in eine
   Gleitkommazahl.
5. Der null-Callback erzeugt einen vorhandenen Wert mit Länge null.
6. `}` schließt den Objektkontext; der Abschluss bestätigt die vollständige
   Eingabe.

| Schlüssel im JSON | v2-Name in ARGS | v3-Name in ARGS | gespeicherter Wert |
| --- | --- | --- | --- |
| `user` | `user` | `json.user` | `Ada` |
| `enabled` | `enabled` | `json.enabled` | `true` |
| `count` | `count` | `json.count` | `1.2300` |
| `optional` | `optional` | `json.optional` | leerer String, 0 Bytes |

Ein vorhandenes null-Argument ist deshalb anhand seines Werts allein nicht
von einem vorhandenen leeren JSON-String unterscheidbar. Ein fehlendes
Argument unterscheidet sich durch seine Abwesenheit.
[v3: `addArgument` und skalare Callbacks][v3-flatten],
[v2: `json_add_argument` und skalare Callbacks][v2-scalars].

### Ein verschachteltes Array

Beim Body `{"items":[["A","B"],"C"],"tail":"D"}` eröffnen die eckigen
Klammern Arraykontexte. Im v3-Code besitzen diese eigene Elementzähler.
Nach dem inneren Array geht die Verarbeitung zum äußeren Array zurück;
nach dessen Ende folgt das Geschwisterfeld `tail`. v2 besitzt dagegen
keine numerischen Arrayindizes.

Die genaue Namensbildung für verschachtelte und leere Container wird in den
versionsgetrennten Tests charakterisiert. Diese Analyse vereinheitlicht sie
nicht zu JSONPath oder JSON Pointer. Der v3-Test `D-walkthrough` prüft das
obige verschachtelte Beispiel bytegenau; v2 `D-walkthrough` prüft es
ebenfalls im Apache-Lauf. Beide Läufe wurden bestätigt. Die
Versionsberichte enthalten die Ergebnisse und die weiteren D-Fälle. Für die kleineren
Referenzbeispiele gilt statisch:

| Body | v2: Einträge als Liste | v3: Einträge als Liste |
| --- | --- | --- |
| `["A","B"]` | `[("array.array","A"),("array.array","B")]` | `[("json.array_0","A"),("json.array_1","B")]` |
| `{"items":["A","B"]}` | `[("items.items","A"),("items.items","B")]` | `[("json.items.array_0","A"),("json.items.array_1","B")]` |


Für das verschachtelte Beispiel lauten die Einträge:

| Schritt | v2-Name | v3-Name | Wert |
| --- | --- | --- | --- |
| erster Skalar im inneren Array | `items.items.items` | `json.items.array_0.array_0` | `A` |
| zweiter Skalar im inneren Array | `items.items.items` | `json.items.array_0.array_1` | `B` |
| inneres Array beendet; nächster äußerer Skalar | `items.items` | `json.items.array_1` | `C` |
| äußeres Array beendet; Geschwister im Rootobjekt | `tail` | `json.tail` | `D` |

In beiden Versionslinien wurde dies im Fall `D-walkthrough` im Lauf
bestätigt (v3 Transaktions-API, v2 Apache-Integration). Die statische
Herleitung steht in den Arraycallbacks beider Linien; die
[Versionsberichte](JSON-YAJL-BASELINE.md#lesen-und-reproduzieren) dokumentieren
die Ausführungen.

Die v2-Namen stammen aus `prefix + "." + current_key`; das erste Array
ohne Schlüssel setzt beide auf `array`. Die Listen sind absichtlich keine
Wörterbücher: Ein Wörterbuch könnte doppelte Namen verlieren.
[v2: Arraycallbacks, Zeilen 171–232][v2-arrays],
[v3: Pfadbildung, Zeilen 129–162][v3-flatten].

### Fehlerzeitpunkt, Limits und HTTP-Entscheidung

Bei `{"ok":"yes","broken":` kann der erste Wert bereits gespeichert sein,
bevor der Abschluss einen unvollständigen Text meldet. Der Parser rollt
gespeicherte Argumente nicht zurück. Der Fehlerzeitpunkt ist dabei
entscheidend: In v3 setzt ein Syntaxfehler die Fehlerflags; eine passende
Phase-2-Regel kann anschließend etwa mit Status 400 ablehnen. Der
Apache-Pfad in v2 übersetzt dagegen einen erst beim Abschluss erkannten
Fehler in HTTP 500 und erreicht die normale Phase 2 nicht. Bei einem schon
im Einleseaufruf erkannten Fehler bleibt in v2 ein anderer Pfad mit
Fehlerflag und anschließender Regelprüfung. Ein Parserstatus allein
beweist daher keinen versionsunabhängigen HTTP-Status.
[v3: Parseraufrufe und Fehlerflags][v3-body],
[v2: Einlesen][v2-reqbody-json], [v2: Abschluss][v2-reqbody-end],
[v2: Übersetzung in Apache-Status][v2-completion-status].

Bei `SecArgumentsLimit 2` und drei skalaren Objektwerten behält der Code die
ersten zwei Argumente und verwirft den überzähligen Wert. Der gemeinsame
Zähler umfasst bereits vorhandene Query-Argumente. Ein mit `return 0`
abgebrochener Callback bewirkt eine YAJL-Fehlermeldung; die Callbacks für
Boolean und null geben diesen Rückgabewert in beiden Linien weiter.
[v3: Argumentaufnahme][v3-argument], [v3: Boolean/null][v3-flatten],
[v2: Argumentaufnahme][v2-scalars].

## 2. Technischer Pfad in v3

### Eingang, Auswahl und Regelphasen

Die C- und C++-API erlaubt `appendRequestBody` mit Zeiger und Byte-Länge;
ein Connector kann sie mehrfach aufrufen. `requestBodyFromFile` liest eine
Datei in einen String und reicht sie ebenfalls an `appendRequestBody`
weiter. Beide Wege landen im transaktionseigenen `std::ostringstream
m_requestBody`. Das bedeutet keinen auf Datei ausgelagerten Parserbetrieb.
[API: `transaction.h`, Zeilen 385–387 und 705–709][v3-api],
[Implementierung: Zeilen 899–970][v3-buffer].

Die eingebaute Headerauswahl erkennt Multipart und URLENCODED, nicht JSON.
`RequestBodyProcessorJSON::evaluate` setzt ausdrücklich
`m_requestBodyProcessor = JSONRequestBody` und `REQBODY_PROCESSOR = JSON`.
In `processRequestBody` führt `SecRuleEngine Off` vor dem Parser zu einem
frühen Return. Bei aktivem JSON-Zweig erfolgt ein `processChunk` für den
gesammelten Body und danach `complete`. Ein Array von Body-Zeilen im
Regressionstest ist kein Mehrchunk-Test: Der Runner fügt sie zuerst zusammen
und ruft `appendRequestBody` einmal auf.
[Auswahlaktion][v3-ctl], [Transaktionspfad][v3-body],
[Runner: Zeilen 289–301][v3-runner-body],
[Body-Zusammenbau: `yajl_array_to_str`][v3-test-body].

Dieser Zusammenbau verwendet C-Strings (`stringstream << const char *`).
Ein eingebettetes NUL aus einer äußeren JSON-Testdefinition kann dabei
abgeschnitten werden. Ungültige UTF-8-Bytes und NUL-Werte benötigen deshalb
einen bytegenauen Zusatzharness; ein `\u0000` im äußeren Testformat ist
nicht automatisch ein unveränderter Rohbody. Der Originalrunner berechnet
Content-Length nach dem Zusammenbau aus `request_body.size()`.
[`RegressionTest`, Zusammenbau Zeilen 52–59][v3-test-body],
[Längenberechnung Zeilen 268–269][v3-test-length].

Wichtig für API-Tests: Die Prüfungen für `SecRequestBodyAccess` und
`ctl:requestBodyAccess` stehen erst **nach** den Prozessorzweigen in
Zeilen 827–843. Bereits über die API zugeführte Bytes können daher schon
JSON-Callbacks auslösen, bevor der Guard Phase 2 überspringt. Ob ein realer
Connector bei ausgeschaltetem Body-Zugriff überhaupt Bytes zuführt, ist
eine zusätzliche Connectorfrage. Der Schluss „Off bedeutet unter allen
API-Aufrufen kein Parsen“ wäre statisch falsch.
[`processRequestBody`, Zeilen 736–843][v3-body].

### Lebensdauer und Speicher

Pro `Transaction` wird ein neues `JSON`-Objekt erzeugt; sein Konstruktor
legt den YAJL-Handle an. `JSON::init` selbst ist ein No-op und setzt keinen
bestehenden Parser zurück. Eine neue HTTP-Transaktion erfordert einen neuen
Transaktionskontext. Der Destruktor löscht verbleibende Container und ruft
`yajl_free`; die Transaktion löscht wiederum ihren JSON-Prozessor.
[Transaktion: Zeilen 119–180][v3-lifetime],
[`JSON`-Konstruktor/-Destruktor und `init`, Zeilen 29–87][v3-parser].

Gespeichert werden der Rohbody, temporäre `str()`-Kopien beim Aufruf,
YAJL-Lexer-/Tokenzustand, der Stapel offener Container mit deren Namen,
der aktuelle Schlüssel und separate ARGS-Namen/-Werte. Die durch
`str().c_str()` übergebene temporäre Stringkopie lebt bis zum Ende des
Funktionsaufrufs; ModSecurity speichert deren Zeiger nicht als ARGS-Wert.
String- und Zahlen-Callbacks kopieren Bytes anhand der von YAJL gemeldeten
Länge in `std::string`. `VariableValue` kopiert Namen und Wert nochmals in
eigene Strings; die Sammlung besitzt diese Einträge bis zu ihrer Freigabe.
[Callbacks][v3-flatten], [Sammlung: `set`/`unset`, Zeilen 40–67][v3-collection],
[`VariableValue`, Zeilen 42–56][v3-value].

Nach der Verarbeitung baut die Transaktion außerdem `FULL_REQUEST` auf und
setzt `REQUEST_BODY` und `REQUEST_BODY_LENGTH`. Das sind zusätzliche
Speicherpfade neben dem Parser. Ein vollständig gepufferter Rohbody belegt
deshalb nicht, dass eine zusätzliche DOM-Darstellung kostenlos wäre.
[Zeilen 845–870][v3-phase2].

### Namen, Collections und Datentypen

`getCurrentKey` setzt den Wurzelnamen auf `json`. Ein leerer Objektschlüssel
wird in den betreffenden Kontexten zu `empty-key`. Der Containerstapel hängt
Objektnamen mit Punkt und Arraynamen mit `.array_N` an. Ein Skalar erhöht
den aktuellen Arrayzähler; auch das Schließen eines Kindcontainers erhöht
den Zähler des übergeordneten Arrays. Damit verbrauchen leere Container zwar
keinen ARGS-Eintrag, können aber einen Arrayindex verbrauchen.
[Header: `getCurrentKey`, Zeilen 91–103][v3-key],
[Callbacks und Zähler: Zeilen 129–311][v3-flatten].

Es gibt keine Maskierung von Punkten als Pfadtrenner. Ein ursprünglicher
Schlüssel `a.b` kann mit dem Pfad des verschachtelten Objekts `a`/`b`
kollidieren. Schlüssel und Werte werden in v3 mit expliziten Byte-Längen
kopiert; ein escaped NUL ist von einer Ausgabe über C-Strings oder Logs zu
unterscheiden. Die Sammlung ist ein `unordered_multimap`: gleiche Namen
bleiben als mehrere Einträge erhalten; eine feste Reihenfolge verschiedener
Namen ist daraus nicht abzuleiten.
[Pfadbildung][v3-flatten], [Sammlung und `emplace`][v3-collection],
[Sammlungstyp][v3-collection-type].

`Transaction::addArgument` prüft vor dem Einfügen
`m_variableArgs.size() >= limit`. Der überzählige Eintrag wird nicht
aufgenommen. `origin == "JSON"` fügt in `ARGS`, aber nicht in `ARGS_POST`
ein; nur `origin == "POST"` befüllt letztere Sammlung. `ARGS_NAMES` ist die
Namenssicht auf ARGS; der kombinierte Größenwert zählt Namen und Werte.
Query-Argumente kommen vorher über `extractArguments("GET", ...)` in
dieselbe Sammlung. JSON-Bodywerte gehören hier also nicht automatisch zu
`ARGS_POST` oder `ARGS_POST_NAMES`.
[`addArgument`, Zeilen 254–310][v3-argument],
[Proxy-Sammlungen: `transaction.h`, Zeilen 213–215][v3-proxies].

### Grenzen, Abschluss und Fehler

Root-Objekte und Root-Arrays erhöhen in v3 beide `m_current_depth` von
null auf eins. Nach dem Anlegen des Containerobjekts prüft der Callback
`depth > max_depth`. Genau die Grenze ist zulässig. Der interne
Parserdefault ist 10000; die Beispielkonfiguration setzt 512. Diese Werte
sind unterschiedliche Ebenen. `setMaxDepth` übernimmt den konfigurierten
Wert.
[`json.cc`, Zeilen 29, 236–283][v3-parser],
[`setMaxDepth`][v3-key], [Beispielkonfiguration][v3-example-config].

Der Konfigurationsscanner akzeptiert für Tiefe und Argumentlimit eine Folge
von Dezimalziffern. Der Parser verwendet anschließend `atoi` und einen
unsigned-Konfigurationswert. Anders als v2 ist dort keine ausdrückliche
Prüfung `> 0` vorhanden; null kann daher als Limitwert auftreten. Negative
oder syntaktisch ungültige Konfigurationen sind Konfigurationstests,
keine JSON-Laufzeitfälle. Überlauf großer Konfigurationszahlen ist eine
gesonderte Validierungsfrage und hier kein behaupteter Runtimevertrag.
[Scanner: Zeilen 412, 804–805][v3-scanner],
[Konfigurationsparser: Zeilen 1628–1636][v3-config].

`processChunk` und `complete` geben jeweils Erfolg oder Fehler zurück.
Der Transaktionscode wertet stattdessen den gemeinsamen Fehlerstring aus;
`complete` wird auch nach einem fehlerhaften Chunk aufgerufen. Beide
Methoden holen YAJL-Fehler ab und geben deren Puffer wieder frei.
Nichtleere Bodies mit Fehler setzen sowohl `REQBODY_ERROR` als auch
`REQBODY_PROCESSOR_ERROR` und beide Meldungsvariablen. Ein Body mit 0 Bytes
erhält dagegen Fehlerflag null, selbst wenn der Parserabschluss eine leere
Eingabe beanstandet. Das ist eine ModSecurity-Sonderbehandlung, kein
allgemeiner YAJL-Erfolg für leeren Text.
[Parser: Zeilen 91–125][v3-parser],
[Transaktion: Zeilen 748–764][v3-body].

Ein Syntax-/Callbackfehler löscht keine zuvor gespeicherten ARGS.
HTTP-Ablehnung für solche Fehler entsteht durch Regeln, etwa die
Beispielregel auf `REQBODY_ERROR`. Die Body-Größenlimits haben zusätzlich
eigene Enginepfade: `appendRequestBody` vergleicht `current_size + len >
limit`. Bei `ProcessPartial` schreibt es den noch erlaubten Präfix und
liefert false; bei `Reject` setzt es im Modus On eine disruptive
Intervention mit Status 403. Ein Returnwert allein ist deshalb keine
HTTP-Antwort. `SecRequestBodyNoFilesLimit` kann schon vor JSON den Parser
überspringen und `REQBODY_ERROR`/`INBOUND_DATA_ERROR` setzen.
[Bodylimit: Zeilen 929–970][v3-buffer],
[No-files-Grenze: Zeilen 697–709][v3-body].

## 3. Technischer Pfad in v2

### Apache-Eingang und Prozessorauswahl

`read_request_body` prüft zuerst, ob ein Body zu erwarten ist und ob
Body-Zugriff erlaubt ist. Nur dann initialisiert es den Prozessor. Apache
liefert über `ap_get_brigade` bereits vom HTTP-Transport bearbeitete
Bucket-Brigades. Nach `apr_bucket_read` werden nichtleere Datenstücke über
`modsecurity_request_body_store` übergeben; am Ende folgt
`modsecurity_request_body_end`. HTTP-Chunkgrenzen, Apache-Buckets und
YAJL-Token-/Parsergrenzen sind verschiedene Dinge.
[`apache2_io.c`, Zeilen 181–345][v2-apache-input].

Die automatische Auswahl setzt URLENCODED oder MULTIPART.
`ctl:requestBodyProcessor` kann JSON ausdrücklich auswählen. Query-Argumente
liegen bereits in `msr->arguments`, bevor Bodywerte hinzugefügt werden.
Die Beispielkonfiguration enthält dafür wie v3 eine Headerregel.
[Auswahl und Argumentinitialisierung: Zeilen 409–449][v2-selection],
[ctl-Aktion: `re_actions.c`, Zeilen 1179–1186][v2-ctl].

```text
Apache read_request_body
  -> modsecurity_request_body_start -> json_init -> yajl_alloc
  -> pro nichtleerem Bucket:
       modsecurity_request_body_store -> json_process_chunk -> yajl_parse
       -> Body aufbewahren (Memory-Chunks / Datei)
  -> modsecurity_request_body_end -> json_complete -> yajl_complete_parse
  -> modsecurity_process_phase(PHASE_REQUEST_BODY)
  -> perform_interception bei passender Regelentscheidung
  -> Transaktionsbereinigung -> json_cleanup -> yajl_free
```

[Initialisierung][v2-reqbody-init], [Chunkweitergabe][v2-reqbody-json],
[Abschluss][v2-reqbody-end],
[Phase 2 im Apache-Modul, Zeilen 1034–1060][v2-phase2],
[Bereinigung: `modsecurity.c`, Zeilen 324–328][v2-cleanup].

### Rohbody, Kopien und Besitz

Der JSON-Prozessor arbeitet in v2 tatsächlich inkrementell. Die gleiche
Eingabe wird anschließend gespeichert, solange kein entsprechender Fehler-
oder Limitpfad dies verhindert. Der Rohbody liegt in malloc-Bytepuffern mit
APR-Verwaltungsstrukturen oder in einer temporären Datei. Beim Überschreiten
des In-memory-Limits kann der Speicherpfad auf Disk wechseln.
YAJL verwaltet Lexer und unvollständige Tokens zusätzlich.
`json_add_argument` kopiert jeden Wert mit `apr_pstrmemdup` in den
Transaktionspool `msr->mp`; Schlüssel und neue Präfixe werden ebenfalls
poolbasiert angelegt. Ein neuer Präfix ersetzt nur den Zeiger, nicht die
bereits im Pool belegten Bytes. Die Argumentwerte bleiben bis zum Ende
der Transaktion gültig, unabhängig vom kurzlebigen Apache-Bucket.
[Speicherinitialisierung und Spill: `msc_reqbody.c`, Zeilen 44–77,
188–267][v2-storage], [JSON-Kopien][v2-scalars].

Für URLENCODED ist das bestehende Präzedenzbeispiel korrekt:
Der Chunkzweig zählt lediglich Bytes. Am Ende baut
`modsecurity_request_body_end_raw` einen zusammenhängenden malloc-Puffer,
kopiert die gespeicherten Speicherchunks hinein und gibt alte Chunks frei;
anschließend ruft `end_urlencoded` `parse_arguments` auf.
Diese Implementierung ist ein Präzedenzfall für spätes Parsen, aber kein
Beweis, dass der JSON-Speicher- oder Ressourcenvertrag identisch wäre.
[Chunkpfad und Rohpuffer: Zeilen 392–396, 550–640][v2-urlencoded].

### Pfadbildung, Werte und Collections

Der v2-Parser hält `prefix` und `current_key` als APR-Strings. Für Objekt-
Schlüssel werden YAJL-Bytes zunächst mit Länge kopiert, spätere
Pfadoperationen verwenden jedoch `%s`, `strlen` und `strrchr('.')`.
Escaped NUL und Punkte im Schlüssel erfordern deshalb eigene
Charakterisierung; v3-Verhalten darf nicht übertragen werden.
`json_add_argument` verbindet Präfix und aktuellen Schlüssel mit einem
Punkt und setzt die Herkunft auf `JSON`. Der oberste Skalar erhält einen
leeren Namen. Der Root-Array-Sonderpfad verwendet `array.array`.
Ein numerischer Elementzähler existiert in dieser v2-Implementierung nicht.
[`msc_json.c`, Zeilen 21–114 und 171–310][v2-json-all].

Zahlen werden über `yajl_number`, nicht `yajl_integer` oder
`yajl_double`, übernommen; ihr Text wird mit der angegebenen Länge kopiert.
Booleans ergeben `true`/`false`, null einen leeren Wert. Doppelte Namen
werden mit `apr_table_addn`, nicht `set`, in die Argumenttabelle aufgenommen.
Die Regelvariable `ARGS` und die Namensvariable iterieren über diese Tabelle
mit Mehrfachvorkommen. `ARGS_POST` filtert dagegen Herkunft `BODY`,
wodurch JSON-Herkunft ausgeschlossen wird. Gleiches gilt für
`ARGS_POST_NAMES`. Die Größe zählt Namen und Werte; nicht nur JSON-Bytes.
[Skalare und Tabelle][v2-scalars],
[`ARGS`/`ARGS_NAMES` und Größenberechnung, Zeilen 118–223][v2-args],
[`ARGS_POST`/`ARGS_POST_NAMES`, Zeilen 334–430][v2-post].

### Tiefe, Argumentzahl und Fehler

v2 startet bei Tiefe null. Der Root-Objektcallback kehrt ohne aktuellen
Schlüssel sofort zurück und zählt den Root nicht. Der Root-Arraycallback
erhöht dagegen die Tiefe. Weitere Objekt-/Arraykontexte erhöhen die Tiefe;
der Abbruchvergleich ist `>` nach der Erhöhung. Daher ist dieselbe
JSON-Verschachtelung unter derselben Zahl nicht pauschal gleich zu v3.
Die Defaults sind Tiefe 10000 und Argumentanzahl 1000. Die Apache-
Konfigurationsfunktionen verwenden `strtol` und lehnen Werte `<= 0`
sowie `LONG_MIN`/`LONG_MAX` ab. Sie prüfen keinen vollständigen Verbrauch
des Konfigurationstexts; Aussagen über strikte Konfigurationssyntax müssen
daher separat getestet werden.
[Tiefe: Array-/Objektcallbacks][v2-json-all],
[Defaults: `modsecurity.h`, Zeilen 98–99][v2-defaults],
[Konfiguration: Zeilen 2098–2143][v2-config].

Das Argumentlimit zählt alle schon vorhandenen `msr->arguments`, also
Query-Werte, doppelte Namen und skalare JSON-Werte. Leere Container erzeugen
keine Argumente. `json_add_argument` prüft `nelts >= arguments_limit` vor
`apr_table_addn`; der überzählige Wert ist zwar schon poolbasiert kopiert,
wird aber nicht aufgenommen. Der Code setzt `msc_reqbody_error` und einen
JSON-Limittext und gibt null zurück. Alle vier skalaren Callbacktypen
reichen dieses Abbruchsignal weiter.
[`json_add_argument`, Zeilen 35–77 und Callbacks 123–168][v2-scalars].

`json_process_chunk` meldet Fehler über Rückgabewert und Meldungszeiger.
Der umgebende Bodypfad setzt `msc_reqbody_error` und dessen Meldung.
`json_complete` ruft den YAJL-Abschluss auf; der Body-Endpfad ignoriert
dessen Fehler bei Bodylänge null. Ein Body ohne Content-Length bzw.
Transfer-Encoding kann schon vor Parserinitialisierung übersprungen
werden. Leerer Transportbody, Body mit 0 Bytes und YAJL mit leerem Text sind
somit verschiedene Beobachtungen.
[Fehlerpfade: Zeilen 373–423][v2-complete],
[Body-Endpfad: Zeilen 724–739][v2-reqbody-end],
[Apache-Guard: Zeilen 191–208][v2-apache-input].

In dieser v2-Linie sind `REQBODY_ERROR` und `REQBODY_ERROR_MSG` die
registrierten einschlägigen Regelvariablen; die v3-Namen
`REQBODY_PROCESSOR_ERROR` und `REQBODY_PROCESSOR_ERROR_MSG` sind keine
übertragbare Voraussetzung. Parserfehler löschen die zuvor erzeugten
Argumente nicht. Bei Fehlern während `yajl_parse` setzt der Storepfad
das Fehlerflag; wegen dieses Flags überspringt der Body-Endpfad weitere
Prozessorabschlüsse. Phase 2 kann dann die verbliebenen Argumente und das
Fehlerflag auswerten. Entdeckt erst `json_complete` einen Fehler, liefert
`modsecurity_request_body_end` dagegen -1. `read_request_body` übersetzt
dies unmittelbar in HTTP 500; das Apache-Modul kehrt vor der normalen
Phase 2 zurück. Diese vorbestehende Unterscheidung muss eine spätere
Änderung ausdrücklich bewerten. Body-Größenlimits können unabhängig davon direkt Apache
Status 413 liefern; `Reject`, `ProcessPartial` und `DetectionOnly` haben
eigene Zweige und dürfen nicht aus dem HTTP-Status eines einzelnen Tests
verallgemeinert werden.
[Variablenregistrierung: Zeilen 4192–4212][v2-error-vars],
[Abschlussguard: Zeilen 685–686][v2-end-guard],
[Apache-Abschlussstatus: Zeilen 345–358][v2-completion-status],
[Apache-Bodylimit: Zeilen 997–1024][v2-phase2],
[laufender Bucket-Grenztest: Zeilen 261–332][v2-apache-input].

## 4. Lesen und Schreiben von JSON sind getrennte Abhängigkeiten

### Audit-JSON in v3

`Transaction::toJSON` verwendet einen eigenen `yajl_gen`-Handle,
erzeugt den Auditdatensatz, kopiert den fertigen Puffer in einen String,
hängt eine Newline an und gibt den Generator frei. Der Generator wird
nicht als Requestparser benutzt. Ports und HTTP-Code werden als Zahlen,
`is_interrupted` als Boolean geschrieben. Regel-ID, Severity und mehrere
weitere Metadaten werden dagegen als Strings geschrieben. Ein Writer-Test
muss diese Typen bewusst prüfen. Für Requestbody, Requestheader und
bestimmte Meldungsdaten wird außerdem `toHexIfNeeded` vorgeschaltet;
der Audittext ist daher nicht grundsätzlich identisch mit ursprünglichen
Requestbytes.
[`toJSON`, Zeilen 1567–1762][v3-audit].

`src/modsecurity.cc` enthält ebenfalls Generatoraufrufe, aber diese gehören
zu `ModSecurity::processContentOffset`: Die öffentliche C++-Funktion baut
eine JSON-Beschreibung von Treffer- und Transformationspositionen. Sie
ist nicht die zweite Audit-Writerimplementierung. Ein mitgeliefertes
Beispiel ruft sie auf. Ein späterer Ersatz muss daher auch diesen
eigenständigen JSON-Erzeuger berücksichtigen.
[`processContentOffset`, Zeilen 215–385][v3-offset-writer],
[öffentliche Deklaration][v3-offset-api],
[Beispielaufruf, Zeilen 18–30][v3-offset-example].

Die `LOGFY_ADD*`-Makros stehen tatsächlich im installierten öffentlichen
`transaction.h` und expandieren zu YAJL-Generatoraufrufen. Die Suche im
untersuchten Quellbaum findet deren Verwendung nur in `transaction.cc`.
Das beweist weder fehlende externe Nutzer noch, dass das Entfernen aus
einem öffentlichen Header ohne Kompatibilitätsentscheidung zulässig wäre.
Die Makros selbst stellen zudem keine Typdeklaration von `yajl_gen` dar;
sie setzen Generatornamen und einschlägige Deklarationen am Aufrufort
voraus.
[Makros, Zeilen 83–99][v3-macros],
[Installation der Header: `src/Makefile.am`, Zeilen 39–57][v3-build].

### Audit-JSON, Herkunft und Sanitization in v2

`sec_audit_logger_json` in `msc_logging.c` verwendet `yajl_gen` und die
Makrohilfen aus `msc_logging_json.h`. Diese Hilfen sind direkte Makros,
keine von YAJL unabhängige abstrakte Writerschnittstelle. Beispielsweise
arbeitet `yajl_string` mit `strlen`; eingebettete Nullbytes erfordern
deshalb eine eigene Writerbeobachtung. Der JSON-Body im Auditlog wird als
Array gespeicherter Body-Chunks geschrieben, nicht als erneut geparstes
JSON-Objekt.
[`msc_logging_json.h`, Zeilen 1–13][v2-writer-helpers],
[`sec_audit_logger_json`, Zeilen 656–678, 932–1020][v2-audit].

Sanitization bedeutet hier das Maskieren ausgewählter Werte vor dem
Schreiben des Auditlogs. Der Writer sortiert markierte BODY-/JSON-Argumente
anhand `value_origin_offset`, liest den gespeicherten Rohbody sequenziell
und überschreibt die bezeichneten Bereiche mit Sternchen.
`json_add_argument` setzt die Herkunftslänge jedoch auf die vom Callback
gelieferte Wertlänge und berechnet den Offset aus Callbackzeiger minus
`base_offset`; `base_offset` wird pro Chunk gesetzt. String-Decodierung,
Tokens über Chunkgrenzen und synthetische Boolean-/Nullwerte benötigen
deshalb eigene Ende-zu-Ende-Tests, bevor korrektes Mapping auf den
ursprünglichen Body behauptet werden kann. Eine in-situ-Bibliothek würde
allein weder absolute Herkunft über mehrere Chunks noch die Länge der
ursprünglichen Escape-Schreibweise beweisen. Diese Analyse macht daraus
keine veröffentlichte Sicherheitsbewertung und behauptet keinen
nachgewiesenen Datenabfluss.
[Offset-/Längenzuweisung: `msc_json.c`, Zeilen 19–55 und 378][v2-json-all],
[Verwendung im Writer: Zeilen 879–1006][v2-audit].

### Build und Connector

v3 sucht YAJL über `build/yajl.m4` mit den Paketnamen `yajl2 yajl`, Header
`yajl/yajl_parse.h`, Linkbibliothek `yajl` und Definition `WITH_YAJL`.
Parser und Generator sind von diesem Buildmerkmal abhängig. v2 besitzt
`build/find_yajl.m4`, das YAJL-Header und Linkflags ermittelt; das
v2-Testprogramm wird ausdrücklich ebenfalls gegen YAJL gebaut.
[v3-Buildprüfung][v3-yajl-build], [v2-Buildprüfung][v2-yajl-build],
[v2-Test-Linkflags, Zeilen 35–75][v2-test-build].

Das separate offizielle Connectorrepository wurde nur lesend auf Commit
`b7cf44e57bc386bc5c15e5c9319e6b6b719357f2` betrachtet. Dessen `config`
setzt `YAJL_EXTRA="-L$YAJL_LIB -lyajl"` **nur bei nichtleerem `YAJL_LIB`**
und übernimmt es in den expliziten ModSecurity-Pfad. Die pauschale Aussage
„der Connector linkt immer direkt -lyajl“ ist zu stark. Hier wurde kein
Connector gebaut und keine Aussage über sämtliche Connectoren getroffen.
[Connector: `config`, Zeilen 8–48][nginx-config].

## 5. Bestandsaufnahme der Originaltests

Die Zahlen sind eine statische Bestandsaufnahme an den festgelegten
Originalcommits, keine Anzahl erfolgreicher Läufe. Als JSON-Integrationstest
wird hier ein Fall gezählt, dessen aktive Regeln den JSON-Prozessor
ausdrücklich auswählen und dessen Body/Assertions den Requestpfad betreffen.

| Linie / Originaldatei | Fälle | Was die vorhandenen Assertions absichern |
| --- | ---: | --- |
| v3 `request-body-parser-json.json` | 6 | ein flaches und ein verschachteltes Feld; leerer Body ohne Fehlerflag; ungültiger Text mit Fehlerflag; Objekttiefe unter/über Grenze |
| v3 `issue-1576.json` | 3 | ausgewählte Namen/Werte in Root-/verschachtelten Arrays, keine vollständige Eintragsliste |
| v3 `secargumentslimit.json` | 2 | ein Wert unter der Grenze; über Grenze Logmeldung/400 durch Fehlerregel |
| v3 `config-body_limits.json` | 2 mit JSON-Auswahl, 16 insgesamt | No-files-Limit unter/über Grenze; keine vollständige JSON-Wertematrix |
| v3 `transformations.json` | 3 mit JSON-Auswahl | nachgelagerte Transformationen auf JSON-Werten, keine allgemeine Parsercharakterisierung |
| **v3 gesamt nach dieser Definition** | **16** | unterschiedliche Assertionstiefe; nicht mit 16 vollständig geprüften JSON-Verträgen gleichzusetzen |
| v2 `rule/15-json.t` | 7 | Objektwert, drei Array-/Pfadfälle, Tiefe unter/über Grenze, Root-Zahl `25` |
| v2 `action/00-transformations.t` | 1 | HTML-Entity-Transformation eines JSON-Werts |
| **v2 gesamt nach dieser Definition** | **8** | die ersten vier Fälle in `15-json.t` akzeptieren alternativ fehlenden JSON-Support |

[v3 Hauptdatei][v3-original-tests], [v3 Arrays][v3-array-tests],
[v3 Argumentlimit][v3-args-tests], [v3 Bodylimits][v3-body-tests],
[v3 Transformationen][v3-transform-tests], [v2 Hauptdatei][v2-original-tests],
[v2 Transformation][v2-transform-tests].

Der v3-Automake-Testkatalog registriert die genannten Regressiondateien;
der Runner ruft den Transaktionspfad auf. Die v2-Automake-Regel
`test-regression` startet den vorhandenen Perl-Runner; normales `make test`
ist dort nicht derselbe HTTP-Regressionslauf.
[v3 Katalog][v3-suite], [v3 Runner][v3-runner-body],
[v2 Testziele, Zeilen 77–83][v2-test-build].

Weitere Originalbestände wurden auf ARGS, Body, Limits und Writerbezug
durchsucht: v3 unter anderem `variable-ARGS*.json`,
[`variable-REQBODY_PROCESSOR_ERROR.json`](https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/test/test-cases/regression/variable-REQBODY_PROCESSOR_ERROR.json), [`variable-REQUEST_BODY.json`](https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/test/test-cases/regression/variable-REQUEST_BODY.json),
[`action-ctl_request_body_access.json`](https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/test/test-cases/regression/action-ctl_request_body_access.json) und [`action-ctl_request_body_processor.json`](https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/test/test-cases/regression/action-ctl_request_body_processor.json);
v2 insbesondere [`target/00-targets.t`](https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/tests/regression/target/00-targets.t), [`config/10-request-directives.t`](https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/tests/regression/config/10-request-directives.t),
[`config/10-audit-directives.t`](https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/tests/regression/config/10-audit-directives.t) und Loggingtests. Ihre Existenz beweist
keine JSON-spezifische Absicherung. So sind die v2-Argumentlimitfälle in
`10-request-directives.t` URLENCODED-/Query-Fälle; eine JSON-Auswahlregel
innerhalb eines fuzzyHash-**Requestbodys** ist nur Testdateninhalt und wird
nicht als aktive Parserregel gezählt.

v3 `auditlog.json` enthält 15 Fälle, davon sechs mit
`SecAuditLogFormat JSON`: Grundform, binäre Meldungsdaten, Body, Header und
zwei `is_interrupted`-Zustände. Das sind Writerfälle. Ein Writer-Regex
prüft noch nicht die vollständige syntaktische und strukturelle Gültigkeit
eines Datensatzes.
[v3 Original-Writerfälle][v3-audit-tests].

| Pflichtgruppe | Originalstand v3 | Originalstand v2 |
| --- | --- | --- |
| A leerer Body | teilweise: Body mit 0 Bytes/Flag vorhanden | in den JSON-Fällen nicht vorhanden |
| B besondere Schlüssel | keine vollständige Abdeckung gefunden | keine vollständige Abdeckung gefunden |
| C Root-Skalare | nicht in den genannten JSON-Fällen | teilweise: Root-Zahl `25` |
| D Arrays/Namen | teilweise: drei gezielte historische Fälle | teilweise: drei gezielte historische Fälle |
| E Tiefe | teilweise: Objekt-Unter-/Überschreitung | teilweise: Objekt-Unter-/Überschreitung |
| F Argumentlimit | teilweise: zwei JSON-Fälle | allgemeine Grenzfälle vorhanden, keine JSON-Matrix |
| G UTF-8/Escapes | Writer-/Transformationstests ersetzen keine Parser-Bytematrix | keine JSON-Bytematrix gefunden |
| H Zahlentext | keine geforderte Grenz-/Lexemmatrix | nur Root-Zahl, keine Grenz-/Lexemmatrix |
| I doppelte Schlüssel | keine gezielte JSON-Duplikatmatrix gefunden | keine gezielte JSON-Duplikatmatrix gefunden |
| J Abschluss/Restinhalt | ungültiges `a`, keine vollständige Abschlussmatrix | keine gezielte Abschlussmatrix gefunden |

Der zusätzliche, nur als Suchkontext betrachtete Originalbestand enthält:

| Linie | Datei | Fallobjekte in der Datei | Einordnung |
| --- | --- | ---: | --- |
| v3 | [`action-ctl_request_body_access.json`](https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/test/test-cases/regression/action-ctl_request_body_access.json) | 3 | Body-Zugriff; nicht automatisch JSON-Abdeckung |
| v3 | [`action-ctl_request_body_processor.json`](https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/test/test-cases/regression/action-ctl_request_body_processor.json) | 3 | Prozessorauswahl; nicht automatisch JSON-Abdeckung |
| v3 | [`variable-ARGS.json`](https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/test/test-cases/regression/variable-ARGS.json) / [`variable-ARGS_NAMES.json`](https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/test/test-cases/regression/variable-ARGS_NAMES.json) | 7 / 5 | allgemeine Argumente und Namen |
| v3 | [`variable-ARGS_POST.json`](https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/test/test-cases/regression/variable-ARGS_POST.json) / [`variable-ARGS_POST_NAMES.json`](https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/test/test-cases/regression/variable-ARGS_POST_NAMES.json) | 3 / 4 | POST-Herkunft, separat zu JSON |
| v3 | [`variable-REQBODY_PROCESSOR_ERROR.json`](https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/test/test-cases/regression/variable-REQBODY_PROCESSOR_ERROR.json) | 2 | allgemeine Prozessorfehler |
| v3 | [`variable-REQUEST_BODY.json`](https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/test/test-cases/regression/variable-REQUEST_BODY.json) | 1 | Rohbodyvariable |
| v2 | [`target/00-targets.t`](https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/tests/regression/target/00-targets.t) | 19 | allgemeine Regelziele/Collections |
| v2 | [`config/10-request-directives.t`](https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/tests/regression/config/10-request-directives.t) | 25 | Zugriff, Body- und Argumentgrenzen |
| v2 | [`config/10-audit-directives.t`](https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/tests/regression/config/10-audit-directives.t) | 10 | Auditkonfiguration |
| v2 | [`action/10-logging.t`](https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/tests/regression/action/10-logging.t) | 24 | Logging/Sanitization, keine vollständige JSON-Writersuite |
| v2 | [`rule/10-xml.t`](https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/tests/regression/rule/10-xml.t) | 12 | XML-/gemeinsame Argumentgrenzen als Vergleich |
| v2 | [`misc/00-phases.t`](https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/tests/regression/misc/00-phases.t) | 5 | Phasenverhalten als Vergleich |

Diese Größen zählen definierte Fallobjekte, keine ausgeführten oder
bestandenen Fälle. Die v2-Zählung beruht auf den `type =>`-Fallobjekten des
Originalrunners, der jede `.t` als Liste auswertet; sie wurde nicht durch
eine Ausführung der gesamten aufgezählten Dateien bestätigt.
[Perl-Runner: Dateisuche und Auswertung, Zeilen 150–189][v2-runner-inventory].

Die neuen Tests und ihre Laufberichte ergänzen diese Lücken. Ein Test, der
gegenwärtiges Verhalten festschreibt, erklärt eine Auffälligkeit nicht
automatisch zur erwünschten Spezifikation.

## 6. Prüfung der Aussagen aus fzipis Kommentar

Der [konkrete Kommentar vom 14. Oktober 2026][comment] wurde selbst gelesen.
Die Tabelle unterscheidet Beleg und Schlussfolgerung. In der Laufzeitspalte
verweisen Gruppen-IDs auf die expliziten Erwartungen der
[Versionsberichte](JSON-YAJL-BASELINE.md#lesen-und-reproduzieren), nicht auf
zusätzliche pauschale PASS-Behauptungen. Bibliotheksvergleich,
Paketverfügbarkeit, Lizenz-/Performancebehauptungen und konkrete Migration
sind außerhalb dieses Auftrags und wurden nicht überprüft.

| Aussage | Ergebnis der Prüfung | Originalcodebeleg | Laufzeitbeleg / Grenze |
| --- | --- | --- | --- |
| v3 übergibt den gesamten Body in einem `processChunk` plus `complete` | bestätigt für `Transaction::processRequestBody` | [736–764][v3-body] | v3 `L-parser-*` prüft echte Chunks; `K-connector-chunks` prüft mehrere API-Zuführungen. Das sind unterschiedliche Ebenen |
| Deshalb habe SAX in v3 keinen Speichervorteil | nicht belegt | [Bodypuffer][v3-buffer], [Container/ARGS][v3-flatten] | kein vergleichender Peak-RSS-/Allocatorversuch; DOM-Knoten, Kopien und frühere Abbrüche sind zusätzlich zu betrachten |
| v2 parst Chunks und bewahrt den Body auf | im normalen Pfad bestätigt; Grenzen können diesen Pfad vorher abbrechen | [Store][v2-storage], [JSON-Weitergabe][v2-reqbody-json] | HTTP-Transportchunks sind nicht gleich Parserchunks; Disk-Spill gesondert prüfen |
| v2 URLENCODED parst erst am Ende | bestätigt | [550–640][v2-urlencoded] | Präzedenzfall, keine Gleichheit aller Ressourcen-/Fehlerverträge |
| Beide zählen Arrayelemente | für numerische Benennung in v2 widerlegt | [v2 171–232][v2-arrays], [v3 129–162][v3-flatten] | `D-root`, `D-multidim`, `D-walkthrough`: v2 wiederholt Namen, v3 erzeugt `array_N` |
| Beide flachen skalare Werte in ARGS ab | bestätigt, mit unterschiedlicher Benennung und Collectionherkunft | [v2][v2-scalars], [v3][v3-flatten] | Gruppen C/D/F: leere Container liefern keine Argumente; null wird leerer String |
| Beide wenden Tiefen- und Argumentlimits an | bestätigt, aber Zählung/Defaults/Validierung unterscheiden sich | [v2 Tiefe][v2-json-all], [v3 Tiefe][v3-parser], [v2 Konfig][v2-config], [v3 Konfig][v3-config] | Gruppen E/F prüfen Container-/Skalarabbruch; späteres DOM-Wandern hätte einen anderen Zeitpunkt |
| Zahlen werden als Text übernommen | bestätigt | [v2 159–168/324–335][v2-json-all], [v3 47–58/225–228][v3-parser] | Gruppe H vergleicht Textbytes; numerische Regeloperatoren allein genügen nicht |
| DOM benötigt nur Body plus festen Aufwand pro Wert | für ModSecurity nicht nachgewiesen | [gespeicherte Pfad-/Wertkopien][v3-flatten], [v2 APR-Kopien][v2-scalars] | keine neue Bibliothek/Allocatoranalyse; Namen-/Tokenlängen, Tiefe und Zeitpunkt von Limits fehlen in dieser Näherung |
| Writerseite sei die größere spätere Änderung | noch nicht als Änderungsumfang überprüfbar; keine Migration implementiert | [v2 Writer][v2-audit], [v3 Writer][v3-audit], [öffentliche Schnittstelle][v3-offset-api] | keine hypothetischen Diffgrößen oder Aufwandsgleichheit behauptet |
| Writer nutzt YAJL in v2/v3 | bestätigt | [v2 Writer][v2-audit], [v3 Writer][v3-audit] | eigene `AUDIT-*`-Fälle; Writer und Requestparser werden getrennt geprüft |
| `modsecurity.cc` sei Teil des Auditwriters | präzisieren: eigener öffentlicher Offset-JSON-Erzeuger | [215–385][v3-offset-writer] | Beispiel nutzt die öffentliche Funktion; externe Nutzung nicht vollständig ermittelbar |
| öffentliche Header enthalten YAJL-Makros | bestätigt | [83–99][v3-macros] | nur interne Nutzung im untersuchten Baum gefunden; externe Nichtnutzung nicht bewiesen |
| ModSecurity-nginx linkt `-lyajl` | bedingt bestätigt | [Connector 20–23][nginx-config] | nur bei gesetztem `YAJL_LIB`; Connectorbuild nicht ausgeführt |
| sechs v3 / sieben v2 Parserfälle | richtig für die zwei genannten Dateien, zu klein als Repositoryinventar | [Testbestände oben](#5-bestandsaufnahme-der-originaltests) | 16 v3 / 8 v2 aktive JSON-Integrationsfälle nach offengelegter Definition |
| leerer Body und Root-Skalare hätten auf beiden Linien keine Tests | pauschal widerlegt | [v3 Leerbody Fall 3][v3-original-tests], [v2 Rootzahl Fall 7][v2-original-tests] | jeweilige Varianten bleiben nur teilweise abgedeckt |
| in-situ ersetze den Parser unter Erhalt der Ursprungsoffsets | offen; kein gesicherter Vertrag | [Offsetberechnung][v2-json-all], [Maskierung][v2-audit] | Escape-Längen, Mehrchunk-Ursprung und tatsächlicher Auditdatensatz benötigen Ende-zu-Ende-Belege |

## 7. RFC 8259, YAJL und künftige Kompatibilität

[RFC 8259 §2][rfc2] erlaubt jeden JSON-Wert an der Wurzel; Root-Skalare
sind daher nicht automatisch ungültig. §4 empfiehlt eindeutige Objektnamen
mit SHOULD, verbietet Duplikate aber nicht in der Grammatik. §6 trennt
Zahlensyntax von Implementierungsgrenzen: `1e400` oder ein Integer jenseits
von uint64 ist nicht allein aufgrund seines Betrags syntaktisch ungültig.
Erhaltenes Zahlentextverhalten ist ein ModSecurity-Vertrag über die
Weitergabe an Regeln, nicht eine Forderung nach numerischer Interpretation.
[RFC §4][rfc4], [RFC §6][rfc6].

RFC §7 verlangt Escapes für rohe Steuerzeichen; §8.1 fordert UTF-8 beim
Austausch zwischen offenen Systemen. §8.2 behandelt einzelne Surrogate
ausdrücklich als problematischen Interoperabilitätsfall, den die ABNF
zulassen kann. Solche Unicodefälle sind von fehlerhafter Escape-Syntax und
von ungültigen UTF-8-Bytefolgen getrennt zu beurteilen. §9 erlaubt
Implementierungslimits, etwa für Größe, Tiefe und Zahlenpräzision;
§10 verlangt syntaktisch gültige Generatorausgabe.
[RFC §7][rfc7], [RFC §8][rfc8], [RFC §9/10][rfc9].

Die YAJL-2.1.0-API beschreibt Callbackabbruch durch Rückgabe null als
`yajl_status_client_canceled`. Standardmäßig sind die permissiven Optionen
für Kommentare, mehrere Dokumente, Restinhalt und Teilwerte ausgeschaltet.
Beide ModSecurity-Linien setzen `yajl_allow_partial_values` ausdrücklich
auf null. Der Abschluss bleibt daher notwendig. Die YAJL-Dokumentation
beschreibt außerdem, dass ein Zahlen-Callback statt Integer-/Double-
Konvertierung den Zahlentext erhalten kann. Die konkret verwendete
Distributionsversion und Patches gehören zusätzlich ins Herkunftsmanifest;
Webdokumentation ersetzt deren Erfassung nicht.
[YAJL Parser-API][yajl-api], [YAJL Callbacks][yajl-callbacks].

Für eine spätere Bibliotheksentscheidung ergeben sich folgende getrennte
Kompatibilitätsfragen:

* Bewahren: Zahlentext, Länge und Bytes der dekodierten Werte;
  Mehrfachvorkommen; belegte Namenskonventionen je Versionslinie;
  ARGS-/Namenssicht und Herkunft; Fehlerflags; vollständiger Abschluss.
* Bewusst entscheiden: Kollisionen durch Punkte, Behandlung leerer/NUL-
  Schlüssel, ungewöhnliche Arraypfade und unterschiedliche Tiefenzählung.
  Charakterisierung bedeutet hier keine Zustimmung zum Ist-Verhalten.
* Ressourcen erhalten oder bewerten: Früher Abbruch an Tiefe/Argumentlimit,
  zusätzliche Body-/Token-/Pfad-/Wertkopien, Bodylimitaktionen und
  Speicher-/Dateipfade. Ein späterer DOM-Aufbau vor dem Limit ist keine
  automatisch gleichwertige Umsetzung.
* Writer separat erhalten: strukturelle JSON-Gültigkeit, Feldtypen,
  Escaping/Hexdarstellung, Multipart-/Chunkdarstellung, Metadata und die
  öffentliche Offset-JSON-Funktion. Parsererfolg beweist Writerkorrektheit
  nicht.

Nicht im Lauf belegte Ressourcen-, Offset-, Nebenläufigkeits- oder
Connectorfragen bleiben offene Prüfpunkte. Diese Dokumentation ändert
keinen Produktionspfad und wählt keine Ersatzbibliothek aus.

<!-- Immutable official-source references. -->
[comment]: https://github.com/owasp-modsecurity/ModSecurity/issues/3308#issuecomment-6166461842
[v3-header-selection]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/src/transaction.cc#L557-L589
[v3-example-config]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/modsecurity.conf-recommended#L19-L71
[v3-ctl]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/src/actions/ctl/request_body_processor_json.cc#L28-L34
[v3-api]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/headers/modsecurity/transaction.h#L385-L387
[v3-buffer]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/src/transaction.cc#L899-L970
[v3-body]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/src/transaction.cc#L674-L843
[v3-phase2]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/src/transaction.cc#L845-L871
[v3-lifetime]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/src/transaction.cc#L119-L180
[v3-parser]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/src/request_body_processor/json.cc#L29-L283
[v3-flatten]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/src/request_body_processor/json.cc#L129-L311
[v3-key]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/src/request_body_processor/json.h#L91-L108
[v3-argument]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/src/transaction.cc#L254-L310
[v3-proxies]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/headers/modsecurity/transaction.h#L213-L215
[v3-collection-type]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/headers/modsecurity/anchored_set_variable.h#L71-L72
[v3-collection]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/src/anchored_set_variable.cc#L40-L109
[v3-value]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/headers/modsecurity/variable_value.h#L42-L56
[v3-scanner]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/src/parser/seclang-scanner.ll#L804-L805
[v3-config]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/src/parser/seclang-parser.yy#L1628-L1636
[v3-audit]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/src/transaction.cc#L1567-L1762
[v3-offset-writer]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/src/modsecurity.cc#L215-L385
[v3-offset-api]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/headers/modsecurity/modsecurity.h#L299-L300
[v3-offset-example]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/examples/reading_logs_with_offset/read.cc#L18-L30
[v3-macros]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/headers/modsecurity/transaction.h#L83-L99
[v3-build]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/src/Makefile.am#L39-L57
[v3-yajl-build]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/build/yajl.m4#L10-L31
[v3-runner-body]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/test/regression/regression.cc#L289-L301
[v3-test-length]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/test/regression/regression_test.cc#L268-L269
[v3-test-body]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/test/regression/regression_test.cc#L52-L68
[v3-suite]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/test/test-suite.in
[v3-original-tests]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/test/test-cases/regression/request-body-parser-json.json
[v3-array-tests]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/test/test-cases/regression/issue-1576.json
[v3-args-tests]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/test/test-cases/regression/secargumentslimit.json
[v3-body-tests]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/test/test-cases/regression/config-body_limits.json
[v3-transform-tests]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/test/test-cases/regression/transformations.json
[v3-audit-tests]: https://github.com/owasp-modsecurity/ModSecurity/blob/8271c9d3bc0abcbd1189f5414c196c336c959047/test/test-cases/regression/auditlog.json
[v2-selection]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/modsecurity.c#L404-L449
[v2-ctl]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/re_actions.c#L1179-L1186
[v2-apache-input]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/apache2_io.c#L181-L345
[v2-phase2]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/mod_security2.c#L997-L1060
[v2-json-all]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/msc_json.c#L19-L435
[v2-scalars]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/msc_json.c#L21-L168
[v2-arrays]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/msc_json.c#L171-L232
[v2-completion-status]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/apache2_io.c#L345-L358
[v2-end-guard]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/msc_reqbody.c#L685-L686
[v2-reqbody-init]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/msc_reqbody.c#L100-L150
[v2-reqbody-end]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/msc_reqbody.c#L724-L739
[v2-reqbody-json]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/msc_reqbody.c#L373-L396
[v2-storage]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/msc_reqbody.c#L44-L267
[v2-urlencoded]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/msc_reqbody.c#L550-L640
[v2-cleanup]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/modsecurity.c#L324-L328
[v2-complete]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/msc_json.c#L373-L435
[v2-args]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/re_variables.c#L118-L223
[v2-post]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/re_variables.c#L334-L430
[v2-defaults]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/modsecurity.h#L98-L99
[v2-config]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/apache2_config.c#L2098-L2143
[v2-error-vars]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/re_variables.c#L4192-L4212
[v2-audit]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/msc_logging.c#L656-L1020
[v2-writer-helpers]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/msc_logging_json.h#L1-L13
[v2-yajl-build]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/build/find_yajl.m4#L21-L153
[v2-runner-inventory]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/tests/run-regression-tests.pl.in#L150-L189
[v2-test-build]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/tests/Makefile.am#L35-L83
[v2-original-tests]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/tests/regression/rule/15-json.t
[v2-transform-tests]: https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/tests/regression/action/00-transformations.t
[nginx-config]: https://github.com/owasp-modsecurity/ModSecurity-nginx/blob/b7cf44e57bc386bc5c15e5c9319e6b6b719357f2/config#L8-L48
[rfc2]: https://www.rfc-editor.org/rfc/rfc8259.html#section-2
[rfc4]: https://www.rfc-editor.org/rfc/rfc8259.html#section-4
[rfc6]: https://www.rfc-editor.org/rfc/rfc8259.html#section-6
[rfc7]: https://www.rfc-editor.org/rfc/rfc8259.html#section-7
[rfc8]: https://www.rfc-editor.org/rfc/rfc8259.html#section-8
[rfc9]: https://www.rfc-editor.org/rfc/rfc8259.html#section-9
[yajl-api]: https://lloyd.github.io/yajl/yajl-2.1.0/yajl__parse_8h.html
[yajl-callbacks]: https://lloyd.github.io/yajl/yajl-2.1.0/structyajl__callbacks.html
