# YAJL-Charakterisierung: ModSecurity v2

Die untersuchte Originalbasis ist **151074aca78a416764b12ba1221e61a17a6c714d** aus dem offiziellen `v2/master`. Produktionsdateien und YAJL bleiben unverändert. Der neue Fork-Branch `test/json-yajl-baseline-v2-20261015` enthält ausschließlich Tests, Beobachtungshilfen und Dokumentation. Arbeitsort: `/workspace/work/json-yajl-baseline/v2`. Herkunft, Fork-Vergleich und Veröffentlichung stehen im [zentralen Ergebnisbericht](JSON-YAJL-BASELINE.md); die gemeinsame [Codeanalyse](json-yajl-code-analysis.md) erklärt den vollständigen Pfad und bewertet fzipis Aussagen. Der dort beschriebene ursprüngliche Kommentar wurde anhand des aktuellen Originalcodes geprüft.

## Gemessene Ergebnisse

| Ebene | PASS | FAIL | SKIP | BLOCKED | XFAIL |
|---|---:|---:|---:|---:|---:|
| Unveränderte Originaldatei `15-json.t`, vor eigenen Änderungen | 7 | 0 | 0 | 0 | 0 |
| Originale allgemeine Unit-Suite `make check` | 4245 | 1 | 0 | 0 | 0 |
| Erweiterte native `15-json.t` | 11 | 0 | 0 | 0 | 0 |
| Neue Apache-/Regelintegration, einschließlich YAJL-Kontrollfall | 145 | 0 | 0 | 0 | 0 |
| Echte Aufteilung am ModSecurity-Parser, ergänzende Ebene | 611 | 0 | 0 | 0 | 0 |
| Apache-Konfigurationsprüfungen | 14 | 0 | 0 | 0 | 0 |
| ASan + UBSan am Parser-Helfer und mitkompilierten Originaldateien | 611 | 0 | 0 | 0 | 0 |
| Getrennte Audit-Writer-Integration | 5 | 0 | 0 | 0 | 0 |
| LeakSanitizer-Erkennung | 0 | 0 | 0 | 1 | 0 |

Die Unit-Suite hatte bereits vor eigenen Änderungen genau einen Fehler: `op/ipMatch.t`, Fall 17 mit `10.0.0.0/100`, führt nach einem abgewiesenen Rule-Aufbau zu einer bestehenden Assertion in `msre_op_ipmatch_execute`. Das ist kein durch diese Aufgabe eingeführter Produktionsfehler. `make check` meldet deshalb Exit 2, der untergeordnete Test-Runner Exit 1. Der erste Lauf ohne Loopback-Berechtigung konnte Apache nicht starten; nach freigegebenem lokalen Netzwerkzugriff bestand die Original-JSON-Suite vollständig. Auch `httpd -t` benötigt hier diese Berechtigung.

LeakSanitizer meldete ausdrücklich „does not work under ptrace“. Diese Infrastrukturprüfung ist **BLOCKED**. Die anschließenden 611 ASan-/UBSan-Fälle liefen mit deaktivierter Leakerkennung. Instrumentiert sind `processor.c`, der unveränderte `msc_json.c` und die eingebundenen Funktionen aus `msc_util.c`; Apache, APR und die installierte YAJL-Binärbibliothek sind damit nicht vollständig instrumentiert.

Die Maschinenberichte [json-yajl-v2-results.json](json-yajl-v2-results.json), [json-yajl-v2-processor-results.json](json-yajl-v2-processor-results.json) und [json-yajl-v2-config-results.json](json-yajl-v2-config-results.json) enthalten stabile Fall-IDs und Ergebnisse. Ergänzend sind der [Audit-Bericht](json-yajl-v2-audit-results.json) und der [begrenzte ASan-/UBSan-Bericht](json-yajl-v2-sanitizer-results.json) versioniert. Der Integrationsbericht enthält außerdem Körperlänge, SHA-256, Hexbytes, explizite erwartete Argumentlisten, tatsächliche Argumentbytes, Fehlerflag, HTTP-Status und beobachtete Parserinitialisierung. Das [Fixture-Modul](../tests/json-yajl-baseline/cases.py) liefert die konkreten Konfigurationsoptionen und die maßgeblichen Eingabebytes. Seine Erwartungen sind fest eingecheckt und werden beim Testlauf nicht aus der Implementierung erzeugt.

## Beobachteter Verarbeitungspfad

Apache übergibt gelesene Bytes an `read_request_body`. Die Engine initialisiert den ausgewählten Prozessor und führt `modsecurity_request_body_store` für Body-Abschnitte aus. Dort läuft JSON bereits während des Empfangs; anschließend wird der Rohkörper in Speicherstücken oder einer Datei gespeichert. Beim Abschluss ruft `modsecurity_request_body_end` `json_complete` auf. Erst danach erreichen normale Requests die Request-Body-Regelphase, also Phase 2.

```text
Apache read_request_body
  -> modsecurity_request_body_start -> json_init -> yajl_alloc
  -> modsecurity_request_body_store -> json_process_chunk -> yajl_parse
        -> Callbacks -> json_add_argument -> APR-Argumenttabelle
        -> Rohkörper speichern: Speicherstücke oder Datei
  -> modsecurity_request_body_end -> json_complete -> yajl_complete_parse
  -> Phase 2 -> ARGS / ARGS_NAMES / ARGS_GET / Fehlerflag
  -> Bereinigung -> json_cleanup -> yajl_free; APR-Pool freigeben
```

Belege: [msc_reqbody.c, Initialisierung und Übergabe](https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/msc_reqbody.c#L85), [Store vor Speicherung](https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/msc_reqbody.c#L318), [Abschluss](https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/msc_reqbody.c#L724), [Apache-Abschlussentscheidung](https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/apache2_io.c#L345).

`application/json` allein aktiviert im Kern diesen Prozessor nicht. Die mitgelieferte Beispielkonfiguration verwendet eine explizite Phase-1-Regel. Der neue Runner wählt JSON bewusst über eine globale Phase-1-Regel mit `ctl:requestBodyProcessor=JSON`; er setzt keinen nicht belegten Automatismus voraus. Auswahl unabhängig vom Header wird mit `text/plain`, `application/problem+json` und einem Charset-Parameter geprüft. Eine deaktivierte Body-Verarbeitung oder fehlende Auswahl überspringt den Parser. Ohne Content-Length und Body erfolgt ebenfalls kein Parseraufruf. Mit explizitem `Content-Length: 0` initialisiert Apache den Parser, unterdrückt aber dessen Fehler beim Abschluss einer leeren Eingabe. Die direkte Prozessorebene zeigt den eigentlichen leeren JSON-Text als unvollständig. `A-no-body` und `A-zero-bytes` unterscheiden das ausdrücklich.

Der YAJL-Zahlen-Callback erhält Text und kopiert ihn ohne numerische Umwandlung. `1E+02`, `1.2300`, `-0`, sehr große Ganzzahlen sowie extreme Exponenten bleiben bytegleich. Strings werden dekodiert, `true`/`false` werden entsprechende Textwerte, `null` ein leerer Wert. Der APR-Pool besitzt Pfadkopien, Argumentstrukturen und Argumentbytes bis zum Transaktionsende. YAJL besitzt seinen Parserzustand und gegebenenfalls Tokenpuffer. Der Rohkörper wird zusätzlich gespeichert; daraus folgt keine Gleichheit von SAX- und DOM-Speicherbedarf.

[msc_json.c, `json_add_argument`](https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/msc_json.c#L21) trägt JSON-Werte mit Ursprung `JSON` in dieselbe Tabelle wie Query-Argumente ein. `ARGS` und `ARGS_NAMES` sehen sie, `ARGS_POST` ist für diese JSON-Einträge leer, weil es Ursprung `BODY` filtert. `ARGS_GET` enthält allein Query-Argumente. Die neue Suite prüft alle vier Collections und `&ARGS`. `REQBODY_PROCESSOR_ERROR` existiert in dieser v2-Variablenregistrierung nicht; geprüft wird `REQBODY_ERROR` und die dazugehörige Meldung. [Collection-Filter in `re_variables.c`](https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/re_variables.c#L334).

## Einfache Beispiele und Fehlerentscheidungen

`K-intro-object` bestätigt für `{"user":"Ada","enabled":true,"count":1.2300,"optional":null}` die vier geordneten Einträge `user=Ada`, `enabled=true`, `count=1.2300`, `optional=`. Alle treten auch mit Namen und Anzahl in Regeln auf.

`D-walkthrough` bestätigt für `{"items":[["A","B"],"C"],"tail":"D"}` nacheinander `items.items.items=A`, `items.items.items=B`, `items.items=C`, `tail=D`. v2 verwendet wiederholte Arraynamen ohne numerischen Index. Das zu v3 unterschiedliche Verhalten darf eine spätere Bibliothek nicht stillschweigend vereinheitlichen.

`J-garbage` (`{"a":1}x`) bestätigt einen Fehler schon beim Einlesen: `a=1` bleibt erhalten, `REQBODY_ERROR=1`, HTTP 200 ohne ablehnende Regel. `K-error-deny` zeigt mit exakt derselben Eingabe die ausdrückliche Fehlerregel und HTTP 403. Parserfehler und HTTP-Entscheidung sind somit getrennt.

`J-truncated-object` (`{"a":1`) bestätigt dagegen einen Fehler erst im Abschluss. Der direkte Prozessor behält `a=1`, aber Apache beantwortet den Request vor Phase 2 mit HTTP 500. Daher sind die Regelvariablen in dieser Integrationsebene nicht beobachtbar; der Bericht weist die erwartete fehlende Phase 2 aus. Auch reine Whitespace-Eingaben und unvollständige Strings, Zahlen und Literale zeigen diesen Weg. Die Parser-API-Suite prüft getrennt, dass `parse` erfolgreich war und `complete` scheitert. Ein erfolgreicher Zwischenaufruf zählt nicht als Gesamtparse-Erfolg.

Beim Argumentlimit prüft `json_add_argument` vor dem Tabelleneintrag `Anzahl >= Limit`. Bereits vorhandene Query-Argumente zählen mit; der überzählige Eintrag fehlt. Alle fünf Skalartypvarianten sowie Duplikate und leere Container werden geprüft. Boolean-/Null-Callbacks geben den Abbruch in dieser Originalversion weiter. Vorherige Argumente bleiben erhalten. Beispiel `F-callback-false`: Limit 1, `{"a":1,"b":false}` -> nur `a=1`, Fehlerflag 1.

Die Tiefe erhöht sich bei Arrayanfängen, ein initiales Root-Objekt wird dagegen nicht gezählt. Abbruch erfolgt bei `current_depth > Limit`; die Suite prüft diese Ungleichheit und gemischte Strukturen. Ein Limit von 3 lässt vier direkt verschachtelte Objekte mit Skalar zu, aber nur drei Root-Arrayebenen. Der Default 10000 ist [statisch definiert](https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/modsecurity.h#L98); `E-default-depth-64` lässt die Direktive tatsächlich weg und prüft 64 Ebenen als begrenzte Gegenprobe, nicht die volle Defaultgrenze.

Die [Konfigurationsfunktionen](https://github.com/owasp-modsecurity/ModSecurity/blob/151074aca78a416764b12ba1221e61a17a6c714d/apache2/apache2_config.c#L2098) verwenden `strtol` ohne Endzeigerprüfung. Laufzeitbelegt: `0`, `-1`, nichtnumerischer Text und `LONG_MAX` werden abgewiesen; `1`, `3` und `3trailing` werden akzeptiert. Der letzte Fall charakterisiert die vorhandene lockere Konfigurationsvalidierung und ist keine gewünschte neue Anforderung.

## Abdeckung und Kompatibilitätsfragen

| Pflichtgruppe | Stabile neue IDs / Varianten | Konkrete geprüfte Eigenschaften |
|---|---|---|
| A – leer | `A-no-body`, `A-zero-bytes`, `A-whitespace`, `A-object`, `A-array`, `A-empty-string` | Auswahl, tatsächliche Initialisierung, leere Collections, Fehlerflag oder belegte vorzeitige HTTP-Entscheidung; direkte leere Texte separat |
| B – Schlüssel | `B-empty*`, `B-nested-empty`, `B-dot-*`, `B-backslash-unicode`, `B-nul-key` | Exakte Namen, leere Duplikate, Punktkollision, Unicode/Backslash und NUL-Abschneidung im Namen |
| C – Root-Skalare | `C-string/empty/int/decimal/true/false/null`, jeweils auch `-ws` | Annahme, leerer Argumentname, bytegenauer Wert, Anzahl, Collections |
| D – Arrays | `D-root-array`, `D-object-array`, `D-objects`, `D-multidimensional`, `D-mixed-empty`, `D-walkthrough` | Wiederholte Arraynamen, vollständige geordnete Liste, leere Container vor/zwischen/nach Werten, Pfadrückkehr |
| E – Tiefe | `E-object/array/mixed-2/3/4`, `E-object-over`, `E-partial`, `E-limit-one-*`, `E-default-depth-64`; `CFG-*JsonDepthLimit*` | L−1/L/L+1 bei L=3 einschließlich unterschiedlicher Rootzählung, Teilargumente, Limit1, ausgelassene Defaultdirektive, Konfigurationsannahme |
| F – Argumentlimit | `F-object/array-2/3/4`, `F-callback-*`, `F-empty-containers`, `F-duplicates`, `F-query-below/at/json`; `CFG-*ArgumentsLimit*` | L−1/L/L+1, Typen, gemeinsames Query-/JSON-Limit, fehlender überzähliger Eintrag, Fehlerklasse, Callbackabbruch |
| G – Unicode | `G-utf8`, `G-escapes`, `G-control-escapes`, `G-nul-value`, `G-invalid-key/value-*`, `G-invalid-*`, `G-high/low-surrogate` | Gültige 2/3/4 Bytes, Surrogatpaar, NUL, echte ungültige Bytes in Schlüssel/Wert, Escape-Syntax und unterschiedliche Interoperabilitätsfälle |
| H – Zahlen | `H-number-00` bis `H-number-20`, `H-invalid-00` bis `H-invalid-05` | Alle beauftragten Grenzzahlen und Schreibweisen, 96-stellige Zahl, unveränderter Text, echte Syntaxfehler einschließlich bereits extrahiertem `0` bei `01` |
| I – Duplikate | `I-same`, `I-different`, `I-empty`, `I-nested`, `I-collision`, zusätzlich F | Geordnete Mehrfacheinträge, Anzahl und bytegenaue Regelwerte; kein Dictionary als Erwartung |
| J – Abschluss | `J-follow-ws`, `J-garbage`, `J-documents`, `J-truncated-*` | Nachfolgende Daten, zwei Dokumente, fünf Abschneidungsarten, parse/complete-Stufe, Teilargumente und HTTP-/Regelunterschied |
| Ergänzungen | `K-access-off/no-selection/type-*/after-error/long/large/error-deny/intro-object`, `K-body-*` | Auswahlschalter, Content-Type, Fehler danach Erfolg, Schlüssel256/Wert2048 Bytes, 256 Skalare, explizite Ablehnung, Bodylimit8 bei7/8/9 Bytes in beiden Regelmodi und beiden Aktionen |

Die ursprünglichen sieben nativen Tests deckten Grundobjekte, Arrays/Pfadbeispiele, zwei Tiefenvarianten und Root-Zahl 25 ab. Sie prüften meist Status und Logmuster. Die ersten vier akzeptierten ausdrücklich auch „JSON support was not enabled“ und sicherten somit keine aktive JSON-Unterstützung ab. Die vier neuen Fälle erweitern dieselbe Datei um unveränderten Zahlentext, Duplikate, Boolean-Limitabbruch und NUL. Die ergänzende Byte-Suite übernimmt die Eigenschaften, die das ursprüngliche Format nicht bequem vollständig beobachtet.

Ein bestätigter Charakterisierungstest beschließt keine Soll-Spezifikation. Insbesondere sind folgende Befunde bewusst offen:

- `{"a.b":1}` und `{"a":{"b":1}}` kollidieren als `a.b`. Wiederholte Schlüssel bleiben erhalten. Escaped NUL im Schlüssel kürzt den Namen am C-String-Ende, während Werte NUL samt Länge behalten. Eine Änderung erfordert eine bewusste Regelkompatibilitätsentscheidung.
- YAJL 2.1.0 in dieser Distribution akzeptiert überlange UTF-8-Sequenzen und Kodierungen oberhalb U+10FFFF. Einzelne Fortsetzungsbytes und abgeschnittene Sequenzen werden abgewiesen. Ein isoliertes hohes Surrogat wird `?`, ein isoliertes niedriges Surrogat wird als drei ungültige UTF-8-Bytes erhalten. RFC 8259 §8.1 verlangt UTF-8 im offenen Austausch; §8.2 beschreibt Probleme einzelner Surrogate. Die Tests dokumentieren Ist-Verhalten, keine Empfehlung zum Akzeptieren solcher Daten.
- Abschlussfehler bewirken in der Apache-Anbindung HTTP 500, während Einlesefehler ohne Regel nicht automatisch ablehnen. Ein möglicher separater Fix müsste Fehlerweitergabe und HTTP-Policy ausdrücklich entscheiden und diese Erwartungen anpassen.
- Unterschiedliche Arraynamen, Zahlentext, Duplikate, gemeinsame Argumentanzahl, Abbruch während des Parsens und erhaltene Teilargumente sind belegte Kompatibilitätspunkte. Eine Baumverarbeitung erst nach vollständig aufgebautem Dokument hätte eine andere Wirkung auf Ressourcen vor dem Limit.
- Der globale `base_offset` und Herkunftsoffsets werden aus Callbackzeigern berechnet. Dekodierte Tokens, Literale und über Chunkgrenzen zusammengesetzte Tokens können außerhalb des aktuellen Rohkörperpuffers liegen. Diese Offsets sind kein belegter korrekter Bezug zum ursprünglichen Körper. Sanitization-Offsettreue, Parallelzugriffe und deren Auswirkungen sind nicht dynamisch nachgewiesen; ein separater Änderungsvorschlag sollte echte rohe Bytebereiche ohne Zeigersubtraktion unzusammengehöriger Objekte modellieren. Die JSON-Writer-Tests beweisen diese Eigenschaft nicht.

Keine neue Bibliothek wurde gewählt, keine Produktion repariert und keine Performancegleichheit behauptet.

## Wie die Tests falschen Erfolg verhindern

Jeder Apache-Lauf startet mit `CANARY-json-enabled`, der `canary=enabled` tatsächlich in `ARGS` und `ARGS_NAMES` nachweist. Auch ein Filter für deaktivierte Verarbeitung kann deshalb nicht gegen einen Build ohne YAJL grün werden. Reguläre Tests verwenden erfolglose `@rx ^z`-Regeln nach `t:none,t:hexEncode`. Hex enthält kein `z`; dadurch prüft die Engine **alle** Collectioneinträge einschließlich Duplikaten und protokolliert deren tatsächliche Bytes. Der Runner prüft Anwesenheit, Abwesenheit, Reihenfolge, Anzahl, Prozessor und Fehlerklasse. `Content-Length` wird aus `len(bytes)` gebildet; ungültige Bytes stammen direkt aus Python-Byte-Literalen. Kein Fixture wird geparst und erneut serialisiert.

Für `K-long` ist das normale Debugprotokoll zu kurz. Hier wird ausdrücklich **kein** erwarteter Wert als tatsächlich beobachteter Byte-Dump eingesetzt: Die Liste `args` bleibt leer, `exact_value_rule_matched=true` dokumentiert stattdessen den Erfolg einer unabhängigen `@streq`-Regel über den vollständigen Hexwert. Die vollständigen Namensbytes aus `ARGS_NAMES` und `&ARGS=1` sichern Namen und fehlende zusätzliche Argumente. Drei kontrolliert falsche Erwartungen (`value`, `name`, `count`) erzeugten jeweils genau 1 FAIL bei weiterhin bestandenem Kontrollfall und Exit 1. Damit ist diese Beobachtungsgrenze überprüfbar.

Die direkte Prozessorebene ruft die unveränderten ModSecurity-Funktionen auf, keine isolierte YAJL-Spielimplementierung. Sie füttert jede geeignete Eingabe in Größen 1, 2, 3, 7 und der gesamten Eingabelänge; doppelte Größen werden nur einmal ausgeführt. Das sind echte `json_process_chunk`-Aufrufe über Token-, Escape- und UTF-8-Grenzen. HTTP-Übertragungsstücke werden damit nicht verwechselt. Pro Prozess wird sauber initialisiert und freigegeben. Die Apache-Suite verarbeitet dagegen viele aufeinanderfolgende Transaktionen im selben Server einschließlich Fehlern und anschließendem Erfolg.

## Reproduktion

Normale Debian-Buildabhängigkeiten: Compiler, make, pkg-config, autoconf, automake, libtool, apache2-bin, apache2-dev, libapr1-dev, libaprutil1-dev, libxml2-dev, libpcre2-dev, libyajl-dev, perl und libwww-perl. Python 3 verwendet nur die Standardbibliothek. Es ist kein externer Dienst nötig; die Tests starten einen eigenen Apache-Prozess auf einem zufällig gewählten Loopback-Port. Als unprivilegierter Benutzer ausführen, keine bestehende Apache-Konfiguration ändern.

Auf einem System mit regulär installierten Abhängigkeiten, aus dem festgehaltenen v2-Arbeitsbaum:

```sh
git submodule update --init apache2/others/libinjection
./autogen.sh
./configure --with-apxs="$(command -v apxs)" --without-lua --without-curl --enable-assertions
make -j2
make check -j2
perl tests/run-regression-tests.pl -a /usr/sbin/apache2 -S "$PWD/tests/regression/server_root" -p 18088 -v tests/regression/rule/15-json.t
python3 tests/json-yajl-baseline/run.py --httpd /usr/sbin/apache2 --modules /usr/lib/apache2/modules --module apache2/.libs/mod_security2.so --output /tmp/modsec-v2-integration-new
python3 tests/json-yajl-baseline/run-config.py --httpd /usr/sbin/apache2 --modules /usr/lib/apache2/modules --module apache2/.libs/mod_security2.so --output /tmp/modsec-v2-config-new
sh tests/json-yajl-baseline/build-processor.sh /tmp/modsec-v2-processor
python3 tests/json-yajl-baseline/run-processor.py --processor /tmp/modsec-v2-processor --output /tmp/modsec-v2-processor-results.json
python3 tests/json-yajl-audit-v2.py --httpd /usr/sbin/apache2 --modules /usr/lib/apache2/modules --module apache2/.libs/mod_security2.so --output /tmp/modsec-v2-audit-new
```

Jedes Apache-Ausgabeverzeichnis muss neu und unbenutzt sein. `--filter B-` oder `--filter K-long` begrenzt die neue Integration; der Kontrollfall bleibt aktiv. Der originale Runner akzeptiert Dateiname und optional Fallnummer, keine erfundenen Filter. Seine Outputs liegen standardmäßig unter `tests/regression/server_root/logs`; vor der Übergabe wurden sie außerhalb der Quelldateien gesichert. Native Tests bauen unveränderte Produktion erneut nur dann, wenn nötig.

Die hier tatsächlich verwendete isolierte Umgebung installiert nichts global. Vorhandene v3-Buildwerkzeuge und YAJL stammen aus `/workspace/modsecurity-env`; zusätzlich wurden Debian-Pakete mit APT aus signierter Paketmetadatenliste heruntergeladen und mit `dpkg-deb -x` nach `/workspace/work/json-yajl-baseline/v2-env/deps` entpackt. Die Paketliste lautet: apache2-bin/apache2-dev/apache2-data 2.4.68-1~deb13u1, libapr1-dev/libapr1t64 1.7.5-1, libaprutil1-dev/libaprutil1t64 1.6.3-3+deb13u1, libldap-dev/libldap2, libsctp-dev/libsctp1, libapache2-mod-perl2/-dev sowie libwww-perl und seine Perl-Modulabhängigkeiten. Das unveränderte YAJL ist das vorhandene Debian-Paket 2.1.0-5+b2, APR/APU dienen nur der isolierten v2-Anbindung. Der zusätzliche mod_perl-Baustein wurde nicht als Apache-Modul geladen.

Der Compiler war GCC 14.2.0 (`Debian 14.2.0-19`), der Kernel Linux 6.18.44 x86_64. Das festgeschriebene Submodul `apache2/others/libinjection` stand unverändert auf `211782219663f889f471650150df12b623c5766e`.

Die entpackten Hilfsskripte `apxs`, `apr-1-config`, `apu-1-config` und `config_vars.mk` mussten auf diesen Präfix umgebogen werden; das ändert keine ModSecurity-Quelle. `apxs` darf seinen automatisch ermittelten Destdir nicht nochmals vor bereits absolute Präfixpfade setzen. Die relevanten Konfigurationsvariablen `prefix`, Apache include-/module-/sbin-Pfade, `APR_CONFIG`, `APU_CONFIG` und `LIBTOOL` verweisen auf die isolierten Dateien. Wegen des vorhandenen yajl.pc-Includepfads war zusätzlich ein Include des Präfixwurzelverzeichnisses nötig. Die tatsächlich erfolgreichen Befehle:

```sh
. /workspace/modsecurity-env/env.sh
export PATH=/workspace/work/json-yajl-baseline/v2-env/deps/usr/bin:$PATH
export LD_LIBRARY_PATH=/workspace/work/json-yajl-baseline/v2-env/deps/usr/lib/x86_64-linux-gnu:$LD_LIBRARY_PATH
export PERL5LIB=/workspace/work/json-yajl-baseline/v2-env/deps/usr/share/perl5:/workspace/work/json-yajl-baseline/v2-env/deps/usr/lib/x86_64-linux-gnu/perl5/5.40:$PERL5LIB
./autogen.sh
./configure --with-apxs=/workspace/work/json-yajl-baseline/v2-env/deps/usr/bin/apxs --with-apr=/workspace/work/json-yajl-baseline/v2-env/deps/usr/bin/apr-1-config --with-apu=/workspace/work/json-yajl-baseline/v2-env/deps/usr/bin/apu-1-config --with-yajl=/workspace/modsecurity-env/deps/usr --with-libxml=/workspace/modsecurity-env/deps/usr/bin/xml2-config --without-lua --without-curl --enable-assertions CPPFLAGS=-I/workspace/modsecurity-env/deps/usr/include
make -j2
make check -j2
perl tests/run-regression-tests.pl -a /workspace/work/json-yajl-baseline/v2-env/deps/usr/sbin/apache2 -S "$PWD/tests/regression/server_root" -p 18088 -v tests/regression/rule/15-json.t
python3 tests/json-yajl-baseline/run.py --httpd /workspace/work/json-yajl-baseline/v2-env/deps/usr/sbin/apache2 --modules /workspace/work/json-yajl-baseline/v2-env/deps/usr/lib/apache2/modules --module apache2/.libs/mod_security2.so --output /workspace/work/json-yajl-baseline/v2-final-verified
python3 tests/json-yajl-baseline/run-config.py --httpd /workspace/work/json-yajl-baseline/v2-env/deps/usr/sbin/apache2 --modules /workspace/work/json-yajl-baseline/v2-env/deps/usr/lib/apache2/modules --module apache2/.libs/mod_security2.so --output /workspace/work/json-yajl-baseline/v2-config-final
EXTRA_CFLAGS=-I/workspace/modsecurity-env/deps/usr/include sh tests/json-yajl-baseline/build-processor.sh /workspace/work/json-yajl-baseline/v2-processor
python3 tests/json-yajl-baseline/run-processor.py --processor /workspace/work/json-yajl-baseline/v2-processor --output /workspace/work/json-yajl-baseline/v2-processor-verified.json
EXTRA_CFLAGS='-I/workspace/modsecurity-env/deps/usr/include -fsanitize=address,undefined -fno-omit-frame-pointer' EXTRA_LDFLAGS=-fsanitize=address,undefined sh tests/json-yajl-baseline/build-processor.sh /workspace/work/json-yajl-baseline/v2-processor-sanitized
ASAN_OPTIONS=detect_leaks=0:abort_on_error=1 UBSAN_OPTIONS=halt_on_error=1 python3 tests/json-yajl-baseline/run-processor.py --processor /workspace/work/json-yajl-baseline/v2-processor-sanitized --output /workspace/work/json-yajl-baseline/v2-sanitizer-verified.json
python3 tests/json-yajl-audit-v2.py --httpd /workspace/work/json-yajl-baseline/v2-env/deps/usr/sbin/apache2 --modules /workspace/work/json-yajl-baseline/v2-env/deps/usr/lib/apache2/modules --module apache2/.libs/mod_security2.so --output /workspace/work/json-yajl-baseline/v2-audit-verified
```

Die abschließende koordinierte Prüfung wiederholte native11 (Port18093), Apache145, Prozessor611, Konfiguration14, Audit5 und ASan/UBSan611 nach dem Ergänzen nachfolgender leerer Container. Ihre Befehle entsprechen den obigen Aufrufen mit Ausgabepfaden unter `/workspace/work/json-yajl-baseline/root-verification/`; dort liegen auch die frischen Rohprotokolle.

Rawlogs: `/workspace/work/json-yajl-baseline/v2-original-regression.log`, `v2-original-check.log`, `v2-extended-native.log`, `v2-build.log`, `v2-configure.log`; pro Integrationsausgabe `httpd.conf`, `debug.log`, `error.log`, `startup.log`, eine Logdatei pro Fall und `results.json`. Sanitizer- und Prozessorlogs stehen neben den benannten Ergebnisdateien. Nur bereinigte Ergebnisse mit synthetischen Fixturebytes werden eingecheckt, keine Buildprodukte, Tokens, Paketarchive oder allgemeinen Debuglogs.

## Geänderte Dateien und verbleibende Lücken

Eigene v2-Arbeit: Erweiterung von `tests/regression/rule/15-json.t`; neue Dateien `tests/json-yajl-baseline/cases.py`, `run.py`, `run-config.py`, `processor.c`, `build-processor.sh`, `run-processor.py`; getrennter Audit-Runner `tests/json-yajl-audit-v2.py`; dieser Bericht und die fünf Ergebnisdateien. Hinzu kommen die vom Koordinator verwaltete zentrale Übersicht, gemeinsame Quellcodeanalyse und das Herkunftsmanifest. Die vollständige Git-Dateiliste steht im zentralen Abschlussbericht.

Nicht dynamisch nachgewiesen: Datenherkunftsoffsets/Sanitization bei dekodierten Tokens, Konkurrenz mehrerer Parser in Threads, exakte Defaultgrenze10000, Speicherknappheit/Allocatorfehler, NoFiles-Bodylimit als eigene Laufzeitmatrix, Rohkörperdatei-Umschaltgrenze sowie Audit-Writer mit ungültigem UTF-8, NUL oder mehreren gespeicherten Body-Chunks. Diese Lücken werden nicht als bestandene Prüfungen gezählt. Die begrenzten 256-Element-/96-Ziffern-Fälle sind keine Lastmessung. Die Resultate tragen die zehn Pflichtgruppen und die genannten konkreten Varianten, keine pauschale Behauptung erschöpfender Abdeckung.

Normative Einordnung: [RFC 8259](https://www.rfc-editor.org/rfc/rfc8259.html), insbesondere §§4,6,7,8,9; die tatsächlich installierten YAJL-2.1.0-Header dokumentieren `yajl_parse`, `yajl_complete_parse`, Längen und Callback-Rückgaben. Eine statische Quellenaussage, ein bestandener Lauf und eine noch offene Policyfrage sind im Text jeweils ausdrücklich getrennt.
