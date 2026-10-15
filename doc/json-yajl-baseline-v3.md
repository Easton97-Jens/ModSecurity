# JSON/YAJL: reproduzierbare Charakterisierung von ModSecurity v3

Diese Untersuchung verwendet ausschließlich den offiziellen Produktionsstand
[`8271c9d3bc0abcbd1189f5414c196c336c959047`](https://github.com/owasp-modsecurity/ModSecurity/commit/8271c9d3bc0abcbd1189f5414c196c336c959047).
Die Tests und diese Dokumentation liegen auf dem neuen Fork-Branch
`test/json-yajl-baseline-v3-20261015`, lokal unter
`/workspace/work/json-yajl-baseline/v3`. Der Produktionscode wurde nicht geändert.
Originalreferenz, Arbeitsbranch und spätere Veröffentlichungs-SHA sind verschiedene
Dinge; die nach dem Push überprüfte SHA steht im Abschlussbericht, damit dieses
Dokument keine sich selbst referenzierende Commitfolge benötigt.

Der [gemeinsame Einstieg und Ergebnisindex](JSON-YAJL-BASELINE.md), das
[Herkunftsmanifest](json-yajl-provenance.json) und die
[Codeanalyse einschließlich Aussagenprüfung](json-yajl-code-analysis.md) ergänzen
hier die ausführbare v3-Matrix. Ein bestandener Charakterisierungstest bedeutet
„das ist hier tatsächlich geschehen“, nicht „dieses Verhalten soll für immer
beibehalten werden“.

## Testebenen und belastbare Beobachtung

1. Die vorhandenen Regressionstests benutzen den unveränderten Projekt-Runner.
   Die Datei `request-body-parser-json.json` wurde um drei passende Regeln mit
   exakten Aussagen über Zahlentext, doppelte Argumente, ARGS_NAMES, ARGS_POST,
   Tiefe und Fehlerflags erweitert. Die ursprünglichen sechs Fälle bleiben erhalten.
2. `test/json-baseline/probe.cc` benutzt die tatsächliche öffentliche
   `Transaction`-Schnittstelle: Verbindung, URI/Query, Header, Phase 1,
   `appendRequestBody`, `processRequestBody`, Phase 2 und Interventionen. Die
   Testhilfe liest bestehende Variablenstrukturen aus; sie fügt keine öffentliche
   Produktionsschnittstelle hinzu. Die Auswertung benutzt Listen mit Bytepaaren
   und einen Multimengenvergleich: Duplikate und ihre Häufigkeit bleiben erhalten,
   die Iterationsreihenfolge einer `unordered_multimap` wird nicht spezifiziert.
3. Für echte Parser-Chunks wird die schon vorhandene interne ModSecurity-Klasse
   `RequestBodyProcessor::JSON` mit einer echten Transaction benutzt. Das testet
   die ModSecurity-Callbacks, nicht bloß direkte YAJL-Aufrufe. Es ersetzt keine
   HTTP-Connector-Prüfung. Ein Array von Textzeilen im Projekt-Testformat ist
   ausdrücklich kein Nachweis mehrerer Parseraufrufe.
4. `audit_probe.cc` durchläuft den Request-/Response-Lebenszyklus und ruft
   `Transaction::toJSON` auf. `run_audit.py` prüft den erzeugten Datensatz
   strukturell. Das ist ein eigener Test der schreibenden Seite. Die vorhandenen
   15 Audit-Tests prüfen ergänzend den bestehenden Audit-Writer/Dateipfad.

Der Beobachter enthält vor jeder eigentlichen Probe eine echte JSON-Canary:
`{"canary":"active"}` muss als `json.canary=active` ankommen, ohne Parserfehler.
Fehlender YAJL-Support führt zu Exit 77/BLOCKED. Das `whoAmI`-Banner allein
enthält keinen YAJL-Fähigkeitsnachweis. Die tatsächlich geladene Bibliothek wurde
mit `ldd` auf `v3/src/.libs/libmodsecurity.so.3` geprüft. Das ist bei der hier
vorhandenen älteren vorinstallierten Fork-Bibliothek besonders wichtig.

Eingaben gelangen als Hextext in den Beobachter und werden dort mit expliziten
Längen zu Bytes. Keine zusätzliche Newline wird dem Body angefügt. Content-Length
wird aus `body.size()` berechnet. JSON des Testberichts ist nur der Container für
Hexbytes, kein Parser, der Payload-Zahlen oder Duplikate umschreibt. Namen und
Werte einschließlich NUL werden als Hexbytes verglichen. ARGS, ARGS_NAMES,
ARGS_GET und ARGS_POST werden separat geprüft. Native Regeln und Fall K-rule-value
belegen zusätzlich den Regelzugriff mit `t:none` und gegebenenfalls `t:hexEncode`.

Timeouts, Abstürze und Protokollfehler werden als FAIL mit Fehlergrund gemeldet.
Fehlende ausführbare Datei oder die explizite YAJL-Canary-Sperre sind BLOCKED.
Gezielte Syntaxablehnung ist ein erfolgreicher Negativtest, wenn beide erwarteten
Fehlerflags, Fehlerklasse und die exakte verbliebene Argumentliste stimmen.
Ein leerer Testfilter liefert Exit 2. Vier bewusst falsche Erwartungen müssen
scheitern: NUL-Schlüssel, ursprünglicher Zahlentext, Duplikate und Abschlussfehler.

## Die zehn Pflichtgruppen

Jede ID ist stabil. Exakte Bytes, Regeln, Header, Konfiguration, Erwartungen und
Status jedes Falls stehen in [run.py](../test/json-baseline/run.py) sowie im
[maschinenlesbaren Laufbericht](../test/json-baseline/results/v3-request-results.json).
Die Erwartungen sind im Quelltext festgeschrieben und werden beim Testlauf nicht
aus der Ausgabe erzeugt. Der Bericht enthält außerdem Länge, SHA-256 und Hexbytes
jeder Payload. Große synthetische Werte sind auf 4 KiB, Schlüssel auf 256 Bytes,
das größere Array auf 128 skalare Elemente beschränkt.

| Gruppe | IDs / Fälle | Konkret geprüfte Eigenschaften |
|---|---|---|
| A | A-* / 6 | Kein append-Aufruf, append mit null Bytes, vier erlaubte Whitespace-Zeichen, `{}`, `[]`, `""`; Auswahl, Argumente und Flags. |
| B | B-* / 9 | Leere Schlüssel, leere Geschwister, verschachtelt, Punkt-/Pfadkollision, Backslash, Unicode und NUL mit voller Länge. |
| C | C-* / 14 | String/leerer String, Integer, Dezimal, true, false, null; jeweils auch Whitespace. |
| D | D-* / 9 | Root-Array, Arrays/Objekte ineinander, mehrere Dimensionen, leere Slots, Geschwister und nachvollziehbares Beispiel. |
| E | E-* / 16 | Objekt/Array/Mischform bei L=3: L−1/L/L+1; Root zählt als 1; Teilergebnis; 0/SKalar; Default; drei ungültige Konfigurationen separat. |
| F | F-* / 16 | L−1/L/L+1 bei Objekten und Arrays, Callback-Abbruch für alle vier Skalartypen, leere Container, Duplikate, Query+JSON, Limit 0. |
| G | G-* / 20 | UTF-8 mit 2/3/4 Bytes, Escapes/Surrogatpaar, acht echte Binärfälle in Keys/Werten, Escape-Syntax/Steuerzeichen, einzelne Surrogate und NUL. |
| H | H-* / 27 | Alle angeforderten 21 Zahlentokens einschließlich 96-stelligem Integer unverändert; sechs echte Syntax-Gegenproben. |
| I | I-* / 5 | Gleiche/verschiedene Duplikatwerte, leere und verschachtelte Namen sowie Groß-/Kleinschreibung; Wechselwirkung mit Limit unter F. |
| J | J-* / 8 | Nachfolgender Whitespace, Müll, zwei Dokumente und abgeschnittene fünf Token-/Strukturarten; Teilargumente und Flags. |

Die Zusatzgruppen K (22 Fälle) und L (7 Fälle) prüfen Prozessorwahl,
Content-Type-Varianten, BodyAccess/Regelmodi, explizite Ablehnung, Regelzugriff,
Body-Limits an/unter/über der Grenze, NoFilesLimit, längere Daten,
Connector-Übergaben und Transaktionsisolation. Der Isolationsfall verwendet zuerst
einen Syntaxfehler und dann einen erfolgreichen Array-Request mit derselben
Engine und denselben Regeln. Beide Transaktionen werden einzeln auf Argumente,
Pfad/Indizes, Fehlermeldung und Fehlerflags geprüft.

L kontrolliert echte Parser-Chunks an Zahl-, UTF-8- und Escape-Grenzen, Fehler in
`processChunk` gegenüber Fehlern in `complete` und den zunächst zurückgehaltenen
Root-Zahlentoken `1`: vor complete kein Argument, danach `json=1`. Der leere
Parserlauf hat erfolgreichen Zwischenaufruf und fehlerhaften Abschluss. Die
Transaction meldet beim leeren Body trotzdem 0, weil ihr Code die Fehlermeldung
bei Bodylänge 0 ausdrücklich unterdrückt.

## Wichtige konkrete Ergebnisse

- Ein kleines Objekt `{"a":1,"b":false,"c":null}` führt zu `json.a="1"`,
  `json.b="false"`, `json.c=""`. JSON füllt in v3 ARGS und ARGS_NAMES,
  **nicht ARGS_POST**. Query-Argumente bleiben ohne `json.`-Präfix in ARGS_GET.
- `{"items":[["A","B"],"C"],"tail":"D"}` ergibt
  `json.items.array_0.array_0=A`, `json.items.array_0.array_1=B`,
  `json.items.array_1=C`, `json.tail=D` (D-walkthrough). Leere Container verbrauchen
  eine Arrayposition, erzeugen aber selbst kein Argument.
- `{"a.b":1}` und `{"a":{"b":1}}` ergeben denselben Namen `json.a.b`.
  Werden beide Formen kombiniert, bleiben beide Werte erhalten. Ein leerer
  Objektschlüssel erhält `empty-key`; das ist ebenfalls potenziell kollidierend.
- Alle Zahlentokens bleiben unverändert, auch `1E+02`, `1.2300`, große Integer,
  `1e400` und `1e-400`. Diese werden nicht über Integer/Double zurückformatiert.
- Beim Argumentlimit wird vor dem Einfügen `size >= limit` geprüft. Query und
  JSON teilen die Grenze; der überzählige Wert wird nicht gespeichert. Auch
  Boolean- und Null-Callbacks propagieren im aktuellen Original den Abbruch.
- Bereits erzeugte Argumente bleiben bei Syntax- oder Limitfehlern erhalten.
  `{"a":1,` hat `json.a=1`, aber beide Parserfehlerflags sind 1. Eine Regel muss
  entscheiden, ob sie das ablehnt; K-explicit-deny zeigt explizit Status 422.
- `SecRequestBodyAccess Off` verhindert bei über die API bereits gelieferten
  Bytes den JSON-Parse hier nicht: Die Guard steht nach dem Parserblock. Phase 2
  und REQUEST_BODY-Aufbereitung werden übersprungen. Dies ist eine Abgrenzung zur
  üblichen Connector-Entscheidung, deaktivierte Bytes gar nicht zu liefern; der
  Test behauptet kein solches Verhalten eines konkreten Webservers.
- `SecRequestBodyNoFilesLimit` setzt hier REQBODY_ERROR und INBOUND_DATA_ERROR auf
  1, lässt REQBODY_PROCESSOR_ERROR jedoch leer und überspringt den Parser. Dieser
  Pfad unterscheidet sich ausdrücklich von einer JSON-Syntaxablehnung.
- Das Body-Limit Reject erzeugt die Intervention beim append-Aufruf. Ein
  Connector muss sie dort abfragen. ProcessPartial lässt die passende Präfixlänge
  zurück; bei einem um die schließende Klammer gekürzten Dokument kann `json.x=1`
  bereits vorliegen, obwohl der Abschluss fehlschlägt.

### Zeichenverarbeitung ist keine bereits beschlossene Zielspezifikation

YAJL 2.1.0 lehnt alleinstehende Fortsetzungsbytes und abgeschnittene Sequenzen ab.
Es akzeptiert in dieser Distribution aber `c0 af` (überlange Kodierung) und
`f4 90 80 80` (oberhalb U+10FFFF) in Namen und Werten. Diese Bytes bleiben erhalten.
Ein isoliertes `\ud800` wird zu `?`, ein isoliertes `\udc00` zu `ed b0 80`.
Diese Ergebnisse sind explizite Charakterisierungen und **keine gewünschte
UTF-8-/Unicode-Anforderung**. RFC 8259 §8.1 verlangt UTF-8 für den offenen
Datenaustausch; §8.2 erläutert Probleme mit einzelnen Surrogaten. Ein späterer
Wechsel oder YAJL-Fix muss diese Fälle bewusst entscheiden, statt sie durch einen
lockeren Alternativvergleich unsichtbar zu machen. Ein nutzbarer Angriff oder
zusätzlicher Sicherheitsimpact wurde hier nicht nachgewiesen.

Für einen Parserwechsel gut begründete Kompatibilitätsforderungen sind dagegen
exakter Zahlentext, erhaltene Mehrfachwerte, klare Namensbildung, unverlorene
Callback-Abbrüche, früh wirksame Limits, definierte Teilargumente/Fehlerflags und
saubere Transaktionsgrenzen. Punkt-/empty-key-Kollisionen, Unicode-Nichtkonformität,
BodyAccess-Reihenfolge und die unterschiedlichen Fehlerflags benötigen eine
bewusste Produktentscheidung. Sie werden nicht im Rahmen dieser Tests repariert.

### Audit-JSON getrennt betrachten

Die zwölf Audit-Prüfungen bestehen aus zehn Datensätzen und zwei absichtlichen
Fehlermutationen. Request-Body und Request-Header werden vor YAJL mit `\xhh`
dargestellt, wenn Bytes nicht druckbares ASCII sind. Response-Body/-Header werden
anders behandelt: Nach JSON-Dekodierung bleiben die Unicode-Zeichen und
Steuerzeichen erhalten. Das ist nicht dieselbe Darstellung wie ARGS. Die Suite
prüft Quotes, Backslashes, NUL/Steuerzeichen, Unicode, leere Inhalte, doppelte
Header, Part-Auswahl und boolesche/numerische Feldtypen. Sie erhält doppelte
JSON-Objektmember beim strukturellen Lesen als Paarlisten. Nur tatsächlich
variable Zeit-/Serverkennung wird typisiert statt wortgleich verglichen.
Synthetische Steuerzeichen in Headern testen die Connector-API, nicht die
Headerakzeptanz eines HTTP-Servers. Ungültiges Response-UTF-8 ist nicht abgedeckt.

## Ausgeführte Prüfungen und Grenzen

Die Original-Baseline wurde vor dem Einfügen eigener Fälle ausgeführt. Dabei waren
Originalcode und Originaltests unverändert. Die Binär-/Audit-Hilfen wurden erst
danach in den Aufgabenbranch kopiert. Ergänzend ausgeführte originale
Body-Limit-/Transformationstests lasen ebenfalls ihre unveränderten Dateien.

| Stufe | PASS | FAIL | SKIP | BLOCKED | disabled / XFAIL |
|---|---:|---:|---:|---:|---:|
| Originaltests: JSON 6, issue-1576 3, ArgumentsLimit 2, Audit 15, BodyLimits 16, Transformationen 8 | 50 | 0 | 0 | 0 | 0 |
| Erweiterte native JSON-Datei: ursprüngliche 6 + neue 3 | 9 | 0 | 0 | 0 | 0 |
| Neue Request-Matrix: 159 Fälle + 4 Erwartungsmutationen | 163 | 0 | 0 | 0 | 0 |
| Separate Audit-Matrix: 10 Fälle + 2 Erwartungsmutationen | 12 | 0 | 0 | 0 | 0 |
| Begrenztes ASan/UBSan: nur ursprüngliche JSON-Klasse und Testbeobachter, 7 Chunkfälle | 7 | 0 | 0 | 0 | 0 |
| LeakSanitizer-Versuch: nach Diagnose durch Sandbox/ptrace blockiert | 0 | 0 | 0 | 6 | 0 |

Die sechs LeakSanitizer-Prozesse endeten tatsächlich mit Nonzero-Exit; der
allgemeine Test-Runner meldete deshalb zunächst FAIL 6. Die eindeutige Meldung
„LeakSanitizer does not work under ptrace“ begründet die abschließende Einordnung
BLOCKED 6 für die Leakprüfung. Das ist weder ein Nachweis von sechs Speicherlecks
noch ein bestandener Leaktest. Der anschließende Lauf mit `detect_leaks=0`
prüfte ASan/UBSan erfolgreich. Der abschließende ASan/UBSan-Lauf umfasst auch den Root-Zahlentoken; der
vorherige LeakSanitizer-Versuch umfasste sechs Fälle.

Original50 und erweiterte native9 dürfen nicht als 59 verschiedene Anforderungen
gezählt werden: Sechs Fälle werden absichtlich vorher/nachher ausgeführt. Ebenso
sind die Sanitizerfälle Wiederholungen. Die Ergebnisse sind kein vollständiger
Repository-Testlauf und kein Performancebenchmark. Das Instrumentieren der
Originaldatei `src/request_body_processor/json.cc` bedeutet **nicht**, dass die
übrige Engine, YAJL oder Systembibliotheken instrumentiert waren.

Die Ergebnisse der Vorläufe dienten dazu, falsche Testannahmen zu korrigieren:
unter anderem war `whoAmI` kein YAJL-Featurebanner, ein Objektbeispiel zunächst
noch nicht tief genug für den beabsichtigten Tiefenabbruch, und Unicode-Grenzfälle
wurden von YAJL anders behandelt als zunächst vermutet. Das waren Testentwurf-
oder Erkenntniskorrekturen, keine neu verursachten Produktionsregressionen.
Die endgültigen Erwartungen sind explizit festgeschrieben und frisch geprüft.

Die [Zusammenfassung](../test/json-baseline/results/v3-summary.json),
[Request-Ergebnisse](../test/json-baseline/results/v3-request-results.json),
[Audit-Ergebnisse](../test/json-baseline/results/v3-audit-results.json) und
[Sanitizer-Ergebnisse](../test/json-baseline/results/v3-sanitizer-results.json)
werden mit den Tests versioniert. Alle Payloads sind synthetisch. Rohe Buildlogs
liegen lokal außerhalb der Quellen unter `/workspace/work/json-yajl-baseline/`:
`v3-bootstrap.log`, `v3-configure.log`, `v3-build.log`,
`v3-original-relevant.log`, `v3-original-{issue-1576,secargumentslimit,auditlog,config-body_limits,transformations}.log`,
`v3-native-expanded.log`, `v3-request-results.log`, `v3-audit-results.log`,
`v3-sanitizer-results.log` und `v3-sanitizer-no-leak-results.log`.
Sie sind keine erforderlichen Testfixtures und werden nicht als Build-Artefakte
mitveröffentlicht.

## Reproduzieren

Eine übliche Debian-Bauumgebung benötigt `git`, `build-essential`, `autoconf`,
`automake`, `libtool`, `pkg-config`, `bison`, `flex`, `libyajl-dev`, `libpcre2-dev`,
`libcurl4-openssl-dev`, `libxml2-dev`, `liblua5.4-dev`, `liblmdb-dev`,
`libmaxminddb-dev` und `libfuzzy-dev`. Die hier tatsächlich verwendete
Abhängigkeit war YAJL **2.1.0-5+b2**, g++ **14.2.0-19**, Python **3.12.14** und
Linux **6.18.44 x86_64**. Die Distributionspaket-/Patchangaben stehen im gemeinsamen
Manifest. Andere Versionen sind neue Vergleichsumgebungen, kein ungeprüfter
Beweis identischen Verhaltens. Ein Apache-/nginx-Dienst ist für diese v3-Tests
nicht nötig.

Das sichere Beschaffen und Festhalten beider Originalcommits ist im gemeinsamen
Index beschrieben. Nach dem Checkout des tatsächlich veröffentlichten
v3-Aufgabencommits in einen neuen Fork-Klon genügen mit systemweit installierten
Abhängigkeiten folgende zum Projekt passende Befehle. Das Arbeitsverzeichnis ist
dabei die Repositorywurzel; `results_dir` wird als absoluter, freier Pfad gewählt:

```sh
git submodule update --init --recursive others/libinjection others/mbedtls test/test-cases/secrules-language-tests
sh -e ./build.sh
./configure --prefix="$PWD/../v3-install" \
  --enable-assertions=yes --enable-parser-generation=no --without-geoip \
  --with-yajl --with-pcre2 --with-curl --with-libxml --with-lua --with-lmdb \
  --with-maxmind --with-ssdeep=/usr CXXFLAGS='-O2 -g'
make -j2
results_dir="$PWD/../v3-results"
mkdir -p "$results_dir"
export LD_LIBRARY_PATH="$PWD/src/.libs${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"
# Der Projekt-Runner wertet nur den ersten Dateiparameter aus!
for suite in request-body-parser-json issue-1576 secargumentslimit auditlog config-body_limits transformations; do
  ./test/regression_tests "test/test-cases/regression/$suite.json" > "$results_dir/$suite.log" 2>&1 || exit $?
done
sh test/json-baseline/build-probes.sh "$results_dir/probes"
ldd "$results_dir/probes/probe"
python3 test/json-baseline/run.py --probe "$results_dir/probes/probe" \
  --output "$results_dir/request.json" --sensitivity
python3 test/json-baseline/run_audit.py "$results_dir/probes/audit-probe" \
  --report "$results_dir/audit.json"
```

Der vorhandene Originalrunner kann bei einem falschen Dateipfad „0 tests passed“
ausgeben. Deshalb müssen seine Zusammenfassungen die dokumentierten Fallzahlen
zeigen. Die neue Matrix blockiert eine leere Fallauswahl selbst. Die drei neuen
nativen Fälle werden beim üblichen Projekttest automatisch mitgelesen; die
zusätzliche Byte-/Chunk-Matrix wird ausdrücklich mit den obigen Befehlen gestartet.
`test/Makefile.am` nimmt ihre Quelldateien in die Distribution auf.

In der tatsächlich verwendeten Sitzung wurden Abhängigkeiten ohne Systeminstall
unter `/workspace/modsecurity-env/deps/usr` bereitgestellt. Vor dem Build wurde
`/workspace/modsecurity-env/env.sh` eingelesen, das nur Tool-/Bibliothekspfade,
keine Zugangsdaten setzt. Der tatsächliche configure-Aufruf unterschied sich von
oben nur durch diese relocatierten Optionen:

```sh
./configure --prefix=/workspace/work/json-yajl-baseline/v3-install \
  --enable-assertions=yes --enable-parser-generation=no --without-geoip \
  --with-yajl --with-pcre2 --with-curl --with-libxml --with-lua --with-lmdb \
  --with-maxmind --with-ssdeep=/workspace/modsecurity-env/deps/usr \
  CXXFLAGS='-O2 -g' CPPFLAGS='-I/workspace/modsecurity-env/deps/usr/include' \
  LDFLAGS='-L/workspace/modsecurity-env/deps/usr/lib/x86_64-linux-gnu -Wl,-rpath,/workspace/modsecurity-env/deps/usr/lib/x86_64-linux-gnu'
make -j3
```

Die Reproduktion setzt diese lokale env-Datei nicht voraus. Bei einer relocatierten
YAJL-Installation akzeptiert `build-probes.sh` normales `CPPFLAGS`/`LDFLAGS` und
`PKG_CONFIG_PATH`; es berücksichtigt zusätzlich das übergeordnete Verzeichnis des
von YAJL gemeldeten `includedir`, weil die ModSecurity-Header `<yajl/...>` nutzen.
Die aktuelle Quellbibliothek muss beim Test vor älteren Installationen in
`LD_LIBRARY_PATH` stehen.

Der begrenzte Instrumentierungslauf wurde so gebaut und ausgeführt:

```sh
c++ -std=c++17 -DWITH_YAJL -Iheaders -I. \
  -I/workspace/modsecurity-env/deps/usr/include \
  -fsanitize=address,undefined -fno-omit-frame-pointer -g -O1 \
  test/json-baseline/probe.cc src/request_body_processor/json.cc \
  -Lsrc/.libs -L/workspace/modsecurity-env/deps/usr/lib/x86_64-linux-gnu \
  -Wl,-rpath,"$PWD/src/.libs" -lmodsecurity -lyajl \
  -o "$results_dir/probes/probe-sanitized"
ASAN_OPTIONS=detect_leaks=0:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 \
  python3 test/json-baseline/run.py --probe "$results_dir/probes/probe-sanitized" \
  --filter L-parser --output "$results_dir/sanitizer.json"
```

Der abschließende Lauf des endgültigen Stands enthält sieben Parserfälle.
Unter Systemabhängigkeiten werden die beiden lokalen `-I`/`-L`-
Pfade weggelassen. Für einen echten Original-Vorherlauf muss ein separater
Worktree direkt am festen Originalcommit benutzt werden; die bereits erweiterte
Datei im Aufgabenbranch ist keine unveränderte Baseline.

## Dateien und ausstehende Ebenen

v3 ändert `test/test-cases/regression/request-body-parser-json.json` und
`test/Makefile.am`. Neu sind `test/json-baseline/{probe.cc,run.py,build-probes.sh,audit_probe.cc,run_audit.py}`,
die vier Ergebnisdateien unter `test/json-baseline/results/` und diese Dokumentation.
Die gemeinsamen Dokumente werden zusätzlich vom koordinierenden Aufgabenstand
bereitgestellt. Erforderliche Binärfixtures entstehen deterministisch aus den
bytegenauen Python-Literalen; es gibt keine externe Fixture-Abhängigkeit.
Buildprodukte, temporäre Diagnoseberichte und Python-Caches gehören nicht zum
Aufgabencommit. Die Gitlinks bleiben auf ihren Originalständen.

Nicht geprüft sind ein vollständiger externer Webserver-Connector-Test,
plattformübergreifende Zahlen-/UTF-8-Unterschiede, vollständige Heapmessungen,
Out-of-memory-Pfade, unbeschränkte Dokumentgrößen sowie ein durchgängig
instrumentierter Engine-/YAJL-Build. Eine spätere Ersatzbibliothek wird weder
gewählt noch eingebaut. Ein Performance- oder Speichergleichheitsversprechen
zwischen SAX und DOM lässt sich aus diesen Ergebnissen nicht ableiten.
