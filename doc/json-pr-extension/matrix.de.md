# Testmatrix: zusätzliche ModSecurity-v3-Prüfungen aus PR #4/#5 und den Kommentaren

[English version](matrix.en.md) · [Überblick und Reproduktion](README.de.md)

Diese Matrix betrifft den **unveränderten offiziellen Produktionscommit
`8271c9d3bc0abcbd1189f5414c196c336c959047`**, mit aktivem YAJL 2.1.0. Neue Tests
prüfen ModSecurity selbst: die öffentliche Transaktions-API, den tatsächlichen
internen JSON-Request-Prozessor und davon getrennte JSON-Writer. Die v2-Analyse
bleibt im zweisprachigen Archiv enthalten; diese Erweiterung wurde nur für v3
implementiert und ausgeführt.

Der [vollständige Katalog](catalog.json) ist der maßgebliche Fallindex. Er und die
unten verlinkten maschinenlesbaren Berichte enthalten die vollständigen Bytes,
Längen/Hashes, Header, Regeln, Konfiguration, expliziten Erwartungen und
beobachteten Ergebnisse. Die Tabellen hier sind eine lesbare Zuordnung und
ersetzen diese exakten Angaben nicht. Insbesondere sind Argumente Listen von
Bytepaaren, keine Dictionaries; doppelte Namen, NUL und Zahlenschreibweisen
bleiben erhalten. Erwartungen wurden im Testcode festgelegt und werden beim
Testlauf nicht aus der Ausgabe des untersuchten Parsers neu erzeugt.

## 1. Quellen, Testarten und Zählweise

- [jsonbench PR #4](https://github.com/digitalwave/jsonbench/pull/4) und
  [PR #5](https://github.com/digitalwave/jsonbench/pull/5) liefern Anregungen zu
  Tokenarten, Namensbildung, Byteverarbeitung, Grenzen, Abschluss und einer
  vorgeschlagenen atomaren Argumentübernahme. Die Tests wurden unabhängig für
  ModSecurity geschrieben; Benchmarkerwartungen werden nicht ungeprüft zu dessen
  Vertrag erklärt. Die geprüften PR-Stände stehen in [provenance.json](provenance.json).
- [Fzipis Kommentar](https://github.com/owasp-modsecurity/ModSecurity/issues/3308#issuecomment-6166461842)
  liefert die zehn Pflichtgruppen A–J sowie zusätzliche Fragen zu Streaming,
  Ressourcen, Limits und der schreibenden Seite. A–J bleiben vollständig als
  benannte Gruppen vorhanden, auch wenn einzelne Gruppen keine neuen Fälle
  benötigen. Das behauptet keine vollständige Prüfung aller denkbaren Varianten.
- [Airweens Kommentar](https://github.com/owasp-modsecurity/ModSecurity/issues/3308#issuecomment-6142615388)
  ist insbesondere mit leerer Eingabe, großen Ganzzahlen und ungültigem UTF-8
  verknüpft. Diese drei Themen werden unten ausdrücklich zugeordnet. Eine
  Bibliothekswahl, ein SAX-Neuentwurf oder JSON-Schema-Fähigkeiten sind hier keine
  implementierten Eigenschaften: YAJL bleibt unverändert; Schema-Validierung
  wird weder ergänzt noch als getestete ModSecurity-Funktion ausgegeben.

**Charakterisierung** bedeutet Prüfung des beobachteten Ist-Verhaltens.
Ein PASS erklärt problematische Unicode-Verarbeitung oder fehlerhafte
Writer-Ausgabe nicht zur gewünschten Spezifikation. **Anforderungstests**
prüfen davon getrennt Garantien und ausdrücklich markierte Policyvorschläge.
Nicht erfüllte Erwartungen bleiben echte FAIL mit Nonzero-Exit, keine XFAIL.
**Kontrollen** prüfen, ob absichtlich falsche Erwartungen tatsächlich scheitern,
oder ob eine zulässige alternative Ausgabe akzeptiert wird.

| Bericht / Ebene, jeweils v3 | Produktfälle | Zusätzliche Kontrollen | PASS | FAIL | SKIP / BLOCKED / XFAIL |
| --- | ---: | ---: | ---: | ---: | --- |
| [Bisherige Request-Baseline](../../test/json-baseline/results/v3-request-results.json), Transaktion / interner Parser / Konfiguration | 159 | 4 | 163 | 0 | 0 / 0 / 0 |
| [Neue Request-Charakterisierung](../../test/json-baseline/results/extended-request-results.json), 60 Transaktionsfälle, 4 Transaktionsfolgen, 124 interne Parserfälle | 188 | 4 | 192 | 0 | 0 / 0 / 0 |
| [Request-Anforderungen](../../test/json-baseline/results/requirements-results.json), Transaktions-API | 22 | 0 | 6 | 16 | 0 / 0 / 0 |
| [Erweiterte Writer](../../test/json-baseline/results/extended-audit-results.json), Audit / öffentliche Offset-API | 23 | 2 | 21 | 4 | 0 / 0 / 0 |

Diese vier Berichte enthalten zusammen **382 PASS und 20 FAIL**, einschließlich
zehn Kontrollen. Zusätzlich enthält der abschließende Gesamtstand die
beibehaltenen Auditprüfungen (12 PASS) und die native JSON-Regressiondatei
(11 PASS). Damit ergeben sich gemäß [results-summary.json](results-summary.json)
**399 PASS im Charakterisierungsprofil**, **6 PASS / 20 FAIL im
Anforderungsprofil**, zusammen **405 PASS / 20 FAIL**; SKIP, BLOCKED und XFAIL
jeweils 0. Die begrenzte [Sanitizer-Wiederholung](../../test/json-baseline/results/extended-sanitizer-results.json)
mit 124 PASS ist getrennt ausgewiesen und keine zusätzliche einzigartige
Fallabdeckung. Historische Baselinestatistiken werden dadurch nicht umgeschrieben.

## 2. Pflichtgruppen A–J: nachprüfbare Zuordnung

„Baseline“ zählt hier die **primären ID-Präfixe** A–J. Die 29 weiteren
Baselinefälle K/L und vier Kontrollen stehen zusätzlich im Bericht. „Neu“
zählt nach dem konkret geprüften Gegenstand, nicht pauschal nach allen groben
`area`-Tags des Generators. Beispielsweise werden UTF-8-Chunkfälle nicht als
Zahlenfälle gezählt. Ein neuer Fall kann mehrere Gruppen prüfen; die Spalte
ist daher **überlappend und nicht addierbar**. Es sind 233 Gruppenzuordnungen
für 184 verschiedene neue Fälle; vier weitere Fälle prüfen ausschließlich
ASCII-Wertlängen als Ressourcenthema. Die vollständigen neuen IDs und ihre
konkrete Zuordnung stehen in Abschnitt 5.

| Gruppe / Gegenstand | Baselinefälle | Neue Charakterisierungsfälle, überlappend | Zugeordnete zusätzliche Request-Anforderungen | Geprüfte Erwartung und Grenze |
| --- | ---: | ---: | --- | --- |
| A: leerer Body | 6 | 0 | keine | Fehlender Append-Aufruf und explizite 0 Bytes: keine ARGS, Fehlerflag 0; nur JSON-Whitespace: Fehlerflag 1; `{}`/`[]`: keine ARGS; `""`: ein leerer Rootwert. `L-parser-empty` unterscheidet davon echten Parserabschluss mit leerer Eingabe |
| B: leere/besondere Schlüssel | 9 | 17 | UTF-8-Schlüssel separat unter G | Exakte `json.*`-Namen, `empty-key`, Punkte/Pfadkollisionen, Backslash, Unicode und NUL mit Länge; neue Fälle prüfen Rückkehr zu Geschwistern, verschachteltes NUL und lange vollständige Pfade |
| C: Werte auf oberster Ebene | 14 | 7 | keine | String, leerer String, Integer, Dezimalzahl, true/false/null jeweils mit/ohne Whitespace unter Namen `json`; neue echte Chunkfälle für `1E+02` prüfen leere Vorab-ARGS und Übernahme erst beim Abschluss |
| D: Arrays und Namensbildung | 9 | 13 | keine | `json.array_N`, verschachtelte Pfade, leere Container verbrauchen einen Index ohne ARGS; neue Fälle kombinieren Tokenarten, gepunktete Namen, Geschwister und Array-Argumentlimits |
| E: Tiefe | 16 | 1 | `REQ-POLICY-rollback-depth` | Limit 3 mit Tiefen 2/3/4 für Objekt/Array/Mischform; Root zählt als 1, genaues Limit erlaubt; Limit 0, Default-Gegenprobe und drei abgewiesene Konfigurationen separat. Neue Folge: Tiefenfehler, dann Erfolg ohne Restzustand |
| F: Argumentlimit | 16 | 11 | `REQ-POLICY-rollback-arguments` | Gemeinsame Zählung einschließlich Query/Duplikaten; überzähliger Wert fehlt; leere Container zählen nicht; alle skalaren Callbacks einschließlich true/false/null brechen wirksam ab. Neue Arrays bei Limit 2: zwei Werte erfolgreich, dritter verworfen und Fehlerflags 1 |
| G: UTF-8, Unicode, Escapes | 20 | 63 | 12 `REQ-UTF8-*`, 6 `REQ-POLICY-surrogate-*` | Dekodierte Bytes und Längen in Namen/Werten, echte ungültige Bytes, Escape-Syntax, Surrogate, NUL und Grenzen innerhalb von UTF-8-/Escapesequenzen; Akzeptanz fehlerhafter Unicodefolgen bleibt nur Charakterisierung |
| H: Zahlen und Zahlentext | 27 | 71 | keine | Exakte Lexeme statt numerischer Gleichheit; 64-Bit-/IEEE-754-Grenzen, lange Integer, Exponenten und Syntax-Gegenproben; neue Tokenartvergleiche, 512 Ziffern, lange Exponenten sowie alle ausgewählten Chunkaufteilungen |
| I: doppelte Schlüssel | 5 | 0 | keine | Gleiche/verschiedene Werte bleiben als Mehrfachvorkommen erhalten; leere/verschachtelte Schlüssel, Schreibweise und Regelvariablen; `F-duplicates` prüft zusätzlich das gemeinsame Limit. Bestehende Tests werden beibehalten statt dupliziert |
| J: Restinhalt und unvollständige Eingaben | 8 | 50 | `REQ-POLICY-rollback-syntax`, `REQ-POLICY-rollback-eof` | Whitespace erlaubt, Restzeichen/zwei Dokumente abgewiesen; abgeschnittene Container/Strings/Zahlen/Literale; neue Separator-/Kommentarfehler, NUL, lange unvollständige Tokens, Fehlerzeitpunkt im Chunk oder erst bei complete und erhaltene Teilargumente |

Baseline-IDs je Gruppe (sämtliche Einträge im [Baselinebericht](../../test/json-baseline/results/v3-request-results.json)):

- **A**: `A-absent`, `A-zero`, `A-whitespace`, `A-object`, `A-array`, `A-string`.
- **B**: `B-empty`, `B-nested-empty`, `B-siblings`, `B-dot-flat`, `B-dot-nested`, `B-dot-collision`, `B-backslash`, `B-unicode`, `B-nul`.
- **C**: `C-string`, `C-string-ws`, `C-empty`, `C-empty-ws`, `C-integer`, `C-integer-ws`, `C-decimal`, `C-decimal-ws`, `C-true`, `C-true-ws`, `C-false`, `C-false-ws`, `C-null`, `C-null-ws`.
- **D**: `D-object-walkthrough`, `D-root`, `D-object-array`, `D-array-object`, `D-multidim`, `D-empty-slots`, `D-mixed`, `D-walkthrough`, `D-tail`.
- **E**: `E-object-2`, `E-object-3`, `E-object-4`, `E-array-2`, `E-array-3`, `E-array-4`, `E-mixed-2`, `E-mixed-3`, `E-mixed-4`, `E-partial`, `E-zero-object`, `E-zero-scalar`, `E-default-depth`, `E-config--1`, `E-config-nope`, `E-config-1.5`.
- **F**: `F-object-2`, `F-object-3`, `F-object-4`, `F-array-2`, `F-array-3`, `F-array-4`, `F-callback-string`, `F-callback-number`, `F-callback-boolean`, `F-callback-boolean-true`, `F-callback-null`, `F-empty-containers`, `F-duplicates`, `F-query`, `F-query-full`, `F-zero`.
- **G**: `G-utf2`, `G-utf3`, `G-utf4`, `G-escapes`, `G-common-escapes`, `G-nul-value`, `G-invalid-value-continuation`, `G-invalid-key-continuation`, `G-invalid-value-overlong`, `G-invalid-key-overlong`, `G-invalid-value-truncated`, `G-invalid-key-truncated`, `G-invalid-value-outside`, `G-invalid-key-outside`, `G-bad-escape`, `G-short-unicode`, `G-nonhex`, `G-raw-control`, `G-high-surrogate`, `G-low-surrogate`.
- **H**: `H-number-00`, `H-number-01`, `H-number-02`, `H-number-03`, `H-number-04`, `H-number-05`, `H-number-06`, `H-number-07`, `H-number-08`, `H-number-09`, `H-number-10`, `H-number-11`, `H-number-12`, `H-number-13`, `H-number-14`, `H-number-15`, `H-number-16`, `H-number-17`, `H-number-18`, `H-number-19`, `H-number-20`, `H-invalid-01`, `H-invalid-+1`, `H-invalid-1.`, `H-invalid-1e`, `H-invalid-NaN`, `H-invalid-Infinity`.
- **I**: `I-same`, `I-different`, `I-empty`, `I-nested`, `I-case`.
- **J**: `J-trailing-ws`, `J-trailing-data`, `J-two-docs`, `J-incomplete-object`, `J-incomplete-array`, `J-incomplete-string`, `J-incomplete-number`, `J-incomplete-literal`.

## 3. Zusätzliche Integration, Ressourcen und echte Chunks

Die bestehenden `K-*`-Fälle prüfen Prozessorauswahl (einschließlich fehlender
automatischer JSON-Auswahl anhand Content-Type), explizite Vendor-/Schreibweise-
Varianten, Bodyzugriff/Regelmodus, ARGS-Regelzugriff, Bodylimits unmittelbar um
die Grenze mit Reject/ProcessPartial/DetectionOnly, No-files-Limit und
Transaktionszustand. Die sieben `L-parser-*`-Fälle rufen den tatsächlichen
internen ModSecurity-Prozessor auf. Ein Body-Zeilenarray oder mehrere
Connector-`appendRequestBody`-Aufrufe beweisen dagegen keine mehrfachen
`JSON::processChunk`-Aufrufe: v3 sammelt den Body für den Transaktionspfad.

| Neue Fallfamilie | Anzahl eindeutiger Fälle | Ebene / festgelegte Erwartung |
| --- | ---: | --- |
| `PR-L-raw-value-*`, `PR-L-raw-key-*`, `PR-L-escaped-value`, `PR-L-escaped-key`, `PR-L-complete-path`, `PR-L-incomplete-string` | 11 | Transaktion: Roh-/dekodierte Längen um 8192 und bis 16384, kompletter langer Pfad und Geschwister, unvollständiger String mit erhaltenem `json.a=1`. 8192 ist keine ModSecurity-Grenze; ein Benchmarkerfassungspuffer wird nicht als Produktionslimit übernommen |
| `PR-S-sequence-*` | 4 | Zwei neue Transaktionen auf derselben Engine und demselben RulesSet: Syntax-/Tiefen-/Argument-/Abschlussfehler, dann `{"ok":1}` mit genau `json.ok=1`, Fehlerflags 0 und leerer Meldung |
| `PR-F-array-*` | 10 | Transaktion: je String, Zahl, true, false, null mit zwei/drei Elementen bei Limit 2; genau zwei Einträge, Fehlerflags 0 bzw. 1, beim dritten Element Callbackabbruch |
| `PR-C-utf8-*`, `PR-C-escape-*`, `PR-C-number-*`, `PR-C-root-number-*`, `PR-C-incomplete-number-*`, `PR-C-incomplete-string-*` | 111 | Interner ModSecurity-JSON-Prozessor: jede mögliche Zweiteilung einschließlich leerem Anfang/Ende plus byteweise Zuführung; alle Einleseaufrufe erfolgreich, vollständige Tokens complete=1, unvollständige complete=0 |
| `PR-C-trailing-split-*` | 9 | Interner Prozessor: Restzeichen im Chunk abgewiesen, complete=0; je nach Aufteilung `[0]` oder `[1,0]`, schon extrahiertes `json.a=1` bleibt |
| `PR-C-16k-*` | 4 | Echte Parsergrenze bei Byte 16384 in UTF-8, Surrogat-Escape, Zahl oder unvollständigem langem String; letzteren Fall erkennen erst complete=0 und Fehlerklasse, nicht die zwei erfolgreichen Chunkaufrufe |

Die drei Chunkfamilien zusammen sind die 124 neuen internen Parserfälle. Sie
verwenden **keine direkten YAJL-only-Tests** und verändern Produktionsschnittstellen
nicht. Bytewerte werden vor dem Aufruf hexkodiert transportiert und im Probe
mit expliziter Länge rekonstruiert. Lange Fixtures sind CI-begrenzt: die neuen
Requestfälle erreichen höchstens 49166 Bodybytes; dies ist kein Test von
Speicherknappheit oder unbegrenzt großen Dokumenten. Ein erfolgreicher
begrenzter Lauf beweist weder Speichergleichheit von SAX und DOM noch
Thread-Sicherheit. Keine Schema-Validierung und keine SAX-/DOM-Migration wurden
implementiert oder als bestanden erklärt.

## 4. Anforderungen und Policyvorschläge bleiben sichtbar rot

Alle 22 folgenden Requestfälle erwarten **keine JSON-Argumente**,
`REQBODY_ERROR=1`, `REQBODY_PROCESSOR_ERROR=1`, `INBOUND_DATA_ERROR=0`,
Phase-2-Marker `1` und keine disruptive Intervention (Beobachterstatus 200).
Die Meldungsklasse ist `lexical`, außer bei den vier Rollbackfällen (`parse`,
`parse`, `depth`, `cancel`). Ein Fehlerflag allein erzwingt keinen HTTP-Fehler;
die minimalen Regeln lehnen hier nicht ab. Exakte Bytes und Konfigurationen
stehen im [Anforderungsbericht](../../test/json-baseline/results/requirements-results.json)
und im Katalog. Fehlerhafte Bytes werden nicht durch den JSON-Fixtureloader repariert.

| Kategorie | Anzahl | PASS / FAIL | Einordnung |
| --- | ---: | --- | --- |
| `REQ-UTF8-*` | 12 | 6 / 6 | Ablehnung tatsächlich fehlerhafter UTF-8-Bytes in Schlüssel und Wert. RFC 8259 §8.1 verlangt UTF-8 für Austausch zwischen offenen Systemen; Akzeptanz überlanger, außerhalb des Unicodebereichs oder als UTF-8 kodierter Surrogatfolgen wird nicht als Sollverhalten übernommen |
| `REQ-POLICY-surrogate-*` | 6 | 0 / 6 | Eigenständiger Vorschlag, ungepaarte/wiederholte Surrogat-Escapes abzulehnen. RFC §8.2 beschreibt Interoperabilitätsprobleme; nicht jede solche Escape-Schreibweise ist durch die JSON-ABNF verboten |
| `REQ-POLICY-rollback-*` | 4 | 0 / 4 | Offene atomare Extraktionspolicy aus PR #5: bei Fehler alle zuvor extrahierten JSON-Argumente verwerfen. Der aktuelle Originalcode behält `json.a=1`; der Vorschlag ist kein beschlossener Kompatibilitätsvertrag |

| ID | Bodybytes (Hex) | Meldungsklasse | Einordnung | Ergebnis |
| --- | --- | --- | --- | --- |
| `REQ-UTF8-key-continuation` | `7b2280223a2276227d` | `lexical` | UTF-8-Anforderung | **PASS** |
| `REQ-UTF8-value-continuation` | `7b2278223a2280227d` | `lexical` | UTF-8-Anforderung | **PASS** |
| `REQ-UTF8-key-overlong` | `7b22c0af223a2276227d` | `lexical` | UTF-8-Anforderung | **FAIL** |
| `REQ-UTF8-value-overlong` | `7b2278223a22c0af227d` | `lexical` | UTF-8-Anforderung | **FAIL** |
| `REQ-UTF8-key-truncated` | `7b22e282223a2276227d` | `lexical` | UTF-8-Anforderung | **PASS** |
| `REQ-UTF8-value-truncated` | `7b2278223a22e282227d` | `lexical` | UTF-8-Anforderung | **PASS** |
| `REQ-UTF8-key-above-unicode` | `7b22f4908080223a2276227d` | `lexical` | UTF-8-Anforderung | **FAIL** |
| `REQ-UTF8-value-above-unicode` | `7b2278223a22f4908080227d` | `lexical` | UTF-8-Anforderung | **FAIL** |
| `REQ-UTF8-key-surrogate` | `7b22eda080223a2276227d` | `lexical` | UTF-8-Anforderung | **FAIL** |
| `REQ-UTF8-value-surrogate` | `7b2278223a22eda080227d` | `lexical` | UTF-8-Anforderung | **FAIL** |
| `REQ-UTF8-key-ff` | `7b22ff223a2276227d` | `lexical` | UTF-8-Anforderung | **PASS** |
| `REQ-UTF8-value-ff` | `7b2278223a22ff227d` | `lexical` | UTF-8-Anforderung | **PASS** |
| `REQ-POLICY-surrogate-key-high` | `7b225c7564383030223a2276227d` | `lexical` | offener Unicode-Policyvorschlag | **FAIL** |
| `REQ-POLICY-surrogate-value-high` | `7b2278223a225c7564383030227d` | `lexical` | offener Unicode-Policyvorschlag | **FAIL** |
| `REQ-POLICY-surrogate-key-low` | `7b225c7564633030223a2276227d` | `lexical` | offener Unicode-Policyvorschlag | **FAIL** |
| `REQ-POLICY-surrogate-value-low` | `7b2278223a225c7564633030227d` | `lexical` | offener Unicode-Policyvorschlag | **FAIL** |
| `REQ-POLICY-surrogate-key-high-high` | `7b225c75643830305c7564383031223a2276227d` | `lexical` | offener Unicode-Policyvorschlag | **FAIL** |
| `REQ-POLICY-surrogate-value-high-high` | `7b2278223a225c75643830305c7564383031227d` | `lexical` | offener Unicode-Policyvorschlag | **FAIL** |
| `REQ-POLICY-rollback-syntax` | `7b2261223a31202262223a327d` | `parse` | offener Rollback-Policyvorschlag | **FAIL** |
| `REQ-POLICY-rollback-eof` | `7b2261223a312c` | `parse` | offener Rollback-Policyvorschlag | **FAIL** |
| `REQ-POLICY-rollback-depth` | `7b2261223a312c2278223a7b2279223a7b227a223a327d7d7d` | `depth` | offener Rollback-Policyvorschlag | **FAIL** |
| `REQ-POLICY-rollback-arguments` | `7b2261223a312c2262223a6e756c6c7d` | `cancel` | offener Rollback-Policyvorschlag | **FAIL** |

### Getrennte Writer-Anforderungen und Auditprüfungen

Writerfälle ersetzen keine Requestparserfälle. Der
[Writerbericht](../../test/json-baseline/results/extended-audit-results.json) und
[`run_extended_audit.py`](../../test/json-baseline/run_extended_audit.py) definieren
exakte Feldmengen und -typen, Inhalte und Abwesenheiten. Auditdatensätze werden
streng als UTF-8 dekodiert und strukturell als JSON geprüft; doppelte Header
bleiben als Mitgliederliste erhalten. Variable Zeit-/Server-Metadaten werden
gezielt auf ihren Typ und nichtleeren Inhalt geprüft.

- `AUDX-C01`–`AUDX-C07` prüfen alle 32 Steuerbytes, Text mit JSON-Anschein,
  eingebettetes NUL, Unicode, lange Werte, Leere und die bestehende
  ASCII-Hexdarstellung binärer Requestdaten.
- `AUDX-P01`–`AUDX-P07` prüfen Auditparts einzeln, einschließlich erwarteter
  Abwesenheit; `AUDX-L01` prüft drei identische Serialisierungen, `AUDX-L02` eine
  neue Transaktion ohne übernommene Header/Bodydaten.
- `AUDX-R01` erwartet gültiges UTF-8-JSON bei ungültigen Responsebytes. Der Fall
  scheitert am strengen UTF-8-Decoding. Die künftige Ersatzdarstellung
  (z. B. Escape/Hex oder Ablehnung) bleibt offen; es gibt keine Abschwächung auf
  einen beliebigen Stringvergleich.
- `OFFSET-C01` hält das gegenwärtig fehlerhafte Resultat der öffentlichen
  `ModSecurity::processContentOffset`-API fest; das ist kein gewünschtes
  Ausgabeformat. `OFFSET-C02/C03` prüfen Bereichsfehler.
  `OFFSET-R01/R02/R03` erwarten gültiges JSON sowie beim Einzelbereich und der
  Transformation konkrete Struktur und Werte. Alle drei scheitern bereits an
  der JSON-Syntax. Das belegt keine erfolgreiche Prüfung der nachfolgenden
  Wertassertions; diese bleiben für einen späteren Fix im Test erhalten.
- `AUDX-M01` verlangt Ablehnung einer falschen Erwartung. `AUDX-M02` verlangt,
  dass eine synthetische zulässige Alternativkodierung akzeptiert wird. Beide
  sind Harnesskontrollen, keine zusätzlichen Produktfähigkeiten.

| ID | Ebene | Festgelegte Erwartung / Zweck | Ergebnis |
| --- | --- | --- | --- |
| `AUDX-C01` | Audit-Writer | Alle Steuerbytes; genaue Unterscheidung Request/Response | **PASS** |
| `AUDX-C02` | Audit-Writer | Text mit JSON-Anschein bleibt String | **PASS** |
| `AUDX-C03` | Audit-Writer | Eingebettetes NUL bleibt mit Länge erhalten | **PASS** |
| `AUDX-C04` | Audit-Writer | Unicode-Headernamen und Zeilentrennerwerte | **PASS** |
| `AUDX-C05` | Audit-Writer | Begrenzte lange Werte mit Escapes | **PASS** |
| `AUDX-C06` | Audit-Writer | Leerer Request gegenüber Response mit Präfix | **PASS** |
| `AUDX-P01` | Audit-Writer | A allein: obligatorische Metadaten | **PASS** |
| `AUDX-P02` | Audit-Writer | B: nur Requestheader zusätzlich | **PASS** |
| `AUDX-P03` | Audit-Writer | C: nur Requestbody zusätzlich | **PASS** |
| `AUDX-P04` | Audit-Writer | E: nur Responsebody zusätzlich | **PASS** |
| `AUDX-P05` | Audit-Writer | F: nur Responseheader zusätzlich | **PASS** |
| `AUDX-P06` | Audit-Writer | H: Producer und leere Messages | **PASS** |
| `AUDX-P07` | Audit-Writer | Andere Parts ergänzen keine JSON-Felder | **PASS** |
| `AUDX-L01` | Audit-Writer | Drei Serialisierungen erhalten denselben Zustand | **PASS** |
| `AUDX-L02` | Audit-Writer | Neue Transaktion ohne alte Header/Bodydaten | **PASS** |
| `AUDX-C07` | Audit-Writer | Binäre Requestdaten als ASCII-Hexdarstellung | **PASS** |
| `AUDX-R01` | Audit-Writer | Responseausgabe ist gültiges UTF-8-JSON | **FAIL** |
| `OFFSET-C01` | öffentliche Offset-API | Einzelne Offsets fehlen gegenwärtig; exakte fehlerhafte Bytes | **PASS** |
| `OFFSET-C02` | öffentliche Offset-API | Start an Bytelänge wird abgewiesen | **PASS** |
| `OFFSET-C03` | öffentliche Offset-API | Start jenseits Bytelänge wird abgewiesen | **PASS** |
| `OFFSET-R01` | öffentliche Offset-API | Leerer Match ergibt gültiges JSON; Darstellung sonst offen | **FAIL** |
| `OFFSET-R02` | öffentliche Offset-API | Einzelner Variablen-/Operatorbereich: Struktur und Werte | **FAIL** |
| `OFFSET-R03` | öffentliche Offset-API | Transformationsausgabe: gültiges JSON und konkrete Kette | **FAIL** |
| `AUDX-M01` | Harnesskontrolle | Falsche Erwartung wird abgewiesen | **PASS** |
| `AUDX-M02` | Harnesskontrolle | Gültige alternative Kodierung wird akzeptiert | **PASS** |

## 5. Alle 188 neuen Request-Charakterisierungsfälle

Jede folgende Zeile bezeichnet genau einen Fall aus dem
[Erweiterungsbericht](../../test/json-baseline/results/extended-request-results.json).
`ARGS` ist die erwartete Eintragszahl der ersten/alleinigen Transaktion; exakte
Namen, Werte und Mehrfachvorkommen stehen unter `expected.args_hex` im Bericht
und im [Katalog](catalog.json). Bei Folgen enthält `expected.second_transaction`
zusätzlich die vollständige zweite Erwartung. `T` bezeichnet die Transaktions-API,
`S` zwei Transaktionen, `P` den echten internen ModSecurity-JSON-Prozessor.
Bei `T/S` bedeutet Zustand `E/P/I/Phase/Status` die Erwartungen für
REQBODY_ERROR, REQBODY_PROCESSOR_ERROR, INBOUND_DATA_ERROR, Phase-2-Marker und
Interventionsstatus. Bei `P` stehen stattdessen `complete` und der Ort eines
beabsichtigten Fehlers; `expected.chunk_results`, `expected.pre_args_hex` und
`chunks` enthalten die exakten Zwischenzustände. Gruppen `K` bezeichnen hier
zusätzliche Ressourcen-/Lebenszyklusprüfungen. Alle Zustände sind feste
Erwartungen; PASS verlangt den vollständigen Vergleich, nicht nur die Eintragszahl.

| ID | Gruppen | Ebene | Bodybytes | ARGS | Erwarteter Zustand | Ergebnis |
| --- | --- | --- | ---: | ---: | --- | --- |
| `PR-N-token-kinds` | H | T | 92 | 6 | `0/0/0/1/200` | PASS |
| `PR-N-array-token-kinds` | D,H | T | 43 | 8 | `0/0/0/1/200` | PASS |
| `PR-B-dotted-parent-sibling` | B,D | T | 35 | 3 | `0/0/0/1/200` | PASS |
| `PR-B-dotted-array-sibling` | B,D | T | 33 | 3 | `0/0/0/1/200` | PASS |
| `PR-B-nested-nul` | B,G | T | 45 | 2 | `0/0/0/1/200` | PASS |
| `PR-G-literal-backslash-surrogate` | B,G | T | 35 | 2 | `0/0/0/1/200` | PASS |
| `PR-G-high-ordinary` | G | T | 15 | 1 | `0/0/0/1/200` | PASS |
| `PR-G-key-high-ordinary` | B,G | T | 15 | 1 | `0/0/0/1/200` | PASS |
| `PR-G-high-high` | G | T | 20 | 1 | `0/0/0/1/200` | PASS |
| `PR-G-key-high-high` | B,G | T | 20 | 1 | `0/0/0/1/200` | PASS |
| `PR-G-low-high` | G | T | 20 | 1 | `0/0/0/1/200` | PASS |
| `PR-G-key-low-high` | B,G | T | 20 | 1 | `0/0/0/1/200` | PASS |
| `PR-G-high-end` | G | T | 14 | 1 | `0/0/0/1/200` | PASS |
| `PR-G-key-high-end` | B,G | T | 14 | 1 | `0/0/0/1/200` | PASS |
| `PR-G-low-end` | G | T | 14 | 1 | `0/0/0/1/200` | PASS |
| `PR-G-key-low-end` | B,G | T | 14 | 1 | `0/0/0/1/200` | PASS |
| `PR-G-raw-surrogate-value` | G | T | 11 | 1 | `0/0/0/1/200` | PASS |
| `PR-G-raw-surrogate-key` | B,G | T | 11 | 1 | `0/0/0/1/200` | PASS |
| `PR-G-ff-key` | B,G | T | 9 | 0 | `1/1/0/1/200 (lexical)` | PASS |
| `PR-G-raw-nul-key` | B,G | T | 11 | 0 | `1/1/0/1/200 (lexical)` | PASS |
| `PR-G-raw-nul-value` | G | T | 11 | 0 | `1/1/0/1/200 (lexical)` | PASS |
| `PR-J-trailing-raw-nul` | J | T | 8 | 1 | `1/1/0/1/200 (parse)` | PASS |
| `PR-J-syntax-colon` | J | T | 7 | 0 | `1/1/0/1/200 (parse)` | PASS |
| `PR-J-syntax-comma` | J | T | 13 | 1 | `1/1/0/1/200 (parse)` | PASS |
| `PR-J-syntax-equals` | J | T | 7 | 0 | `1/1/0/1/200 (lexical)` | PASS |
| `PR-J-syntax-object-comma` | J | T | 8 | 1 | `1/1/0/1/200 (parse)` | PASS |
| `PR-J-syntax-array-comma` | J | T | 4 | 1 | `1/1/0/1/200 (parse)` | PASS |
| `PR-J-syntax-block-comment` | J | T | 20 | 0 | `1/1/0/1/200 (lexical)` | PASS |
| `PR-J-syntax-line-comment` | J | T | 18 | 0 | `1/1/0/1/200 (lexical)` | PASS |
| `PR-H-invalid-dot-one` | H,J | T | 8 | 0 | `1/1/0/1/200 (lexical)` | PASS |
| `PR-H-invalid-exponent-plus` | H,J | T | 9 | 0 | `1/1/0/1/200 (lexical)` | PASS |
| `PR-H-invalid-double-minus` | H,J | T | 9 | 0 | `1/1/0/1/200 (lexical)` | PASS |
| `PR-H-decimal30` | H | T | 38 | 1 | `0/0/0/1/200` | PASS |
| `PR-H-negative-zero` | H | T | 10 | 1 | `0/0/0/1/200` | PASS |
| `PR-H-huge-exponent` | H | T | 14 | 1 | `0/0/0/1/200` | PASS |
| `PR-H-tiny-exponent` | H | T | 14 | 1 | `0/0/0/1/200` | PASS |
| `PR-H-exponent-zeroes` | H | T | 13 | 1 | `0/0/0/1/200` | PASS |
| `PR-H-long-integer` | H | T | 518 | 1 | `0/0/0/1/200` | PASS |
| `PR-H-long-incomplete-exponent` | H,J | T | 519 | 0 | `1/1/0/1/200 (lexical)` | PASS |
| `PR-F-array-string-2` | D,F | T | 9 | 2 | `0/0/0/1/200` | PASS |
| `PR-F-array-string-3` | D,F | T | 13 | 2 | `1/1/0/1/200 (cancel)` | PASS |
| `PR-F-array-number-2` | D,F | T | 13 | 2 | `0/0/0/1/200` | PASS |
| `PR-F-array-number-3` | D,F | T | 19 | 2 | `1/1/0/1/200 (cancel)` | PASS |
| `PR-F-array-true-2` | D,F | T | 11 | 2 | `0/0/0/1/200` | PASS |
| `PR-F-array-true-3` | D,F | T | 16 | 2 | `1/1/0/1/200 (cancel)` | PASS |
| `PR-F-array-false-2` | D,F | T | 13 | 2 | `0/0/0/1/200` | PASS |
| `PR-F-array-false-3` | D,F | T | 19 | 2 | `1/1/0/1/200 (cancel)` | PASS |
| `PR-F-array-null-2` | D,F | T | 11 | 2 | `0/0/0/1/200` | PASS |
| `PR-F-array-null-3` | D,F | T | 16 | 2 | `1/1/0/1/200 (cancel)` | PASS |
| `PR-S-sequence-syntax` | J,K | S | 13 | 1 | `1/1/0/1/200 (parse)` | PASS |
| `PR-S-sequence-depth` | E,J,K | S | 25 | 1 | `1/1/0/1/200 (depth)` | PASS |
| `PR-S-sequence-arguments` | F,J,K | S | 16 | 1 | `1/1/0/1/200 (cancel)` | PASS |
| `PR-S-sequence-eof` | J,K | S | 7 | 1 | `1/1/0/1/200 (parse)` | PASS |
| `PR-L-raw-value-8191` | K | T | 8199 | 1 | `0/0/0/1/200` | PASS |
| `PR-L-raw-value-8192` | K | T | 8200 | 1 | `0/0/0/1/200` | PASS |
| `PR-L-raw-value-8193` | K | T | 8201 | 1 | `0/0/0/1/200` | PASS |
| `PR-L-raw-value-16384` | K | T | 16392 | 1 | `0/0/0/1/200` | PASS |
| `PR-L-raw-key-8191` | B,K | T | 8202 | 1 | `0/0/0/1/200` | PASS |
| `PR-L-raw-key-8192` | B,K | T | 8203 | 1 | `0/0/0/1/200` | PASS |
| `PR-L-raw-key-8193` | B,K | T | 8204 | 1 | `0/0/0/1/200` | PASS |
| `PR-L-escaped-value` | G,K | T | 49166 | 1 | `0/0/0/1/200` | PASS |
| `PR-L-escaped-key` | B,G,K | T | 49166 | 1 | `0/0/0/1/200` | PASS |
| `PR-L-complete-path` | B,K | T | 8726 | 2 | `0/0/0/1/200` | PASS |
| `PR-L-incomplete-string` | J,K | T | 16397 | 1 | `1/1/0/1/200 (parse)` | PASS |
| `PR-C-16k-incomplete-string` | J,K | P | 16397 | 1 | `complete=0; Fehler bei complete` | PASS |
| `PR-C-utf8-split-000` | G,K | P | 16 | 1 | `complete=1` | PASS |
| `PR-C-utf8-split-001` | G,K | P | 16 | 1 | `complete=1` | PASS |
| `PR-C-utf8-split-002` | G,K | P | 16 | 1 | `complete=1` | PASS |
| `PR-C-utf8-split-003` | G,K | P | 16 | 1 | `complete=1` | PASS |
| `PR-C-utf8-split-004` | G,K | P | 16 | 1 | `complete=1` | PASS |
| `PR-C-utf8-split-005` | G,K | P | 16 | 1 | `complete=1` | PASS |
| `PR-C-utf8-split-006` | G,K | P | 16 | 1 | `complete=1` | PASS |
| `PR-C-utf8-split-007` | G,K | P | 16 | 1 | `complete=1` | PASS |
| `PR-C-utf8-split-008` | G,K | P | 16 | 1 | `complete=1` | PASS |
| `PR-C-utf8-split-009` | G,K | P | 16 | 1 | `complete=1` | PASS |
| `PR-C-utf8-split-010` | G,K | P | 16 | 1 | `complete=1` | PASS |
| `PR-C-utf8-split-011` | G,K | P | 16 | 1 | `complete=1` | PASS |
| `PR-C-utf8-split-012` | G,K | P | 16 | 1 | `complete=1` | PASS |
| `PR-C-utf8-split-013` | G,K | P | 16 | 1 | `complete=1` | PASS |
| `PR-C-utf8-split-014` | G,K | P | 16 | 1 | `complete=1` | PASS |
| `PR-C-utf8-split-015` | G,K | P | 16 | 1 | `complete=1` | PASS |
| `PR-C-utf8-split-016` | G,K | P | 16 | 1 | `complete=1` | PASS |
| `PR-C-utf8-bytewise` | G,K | P | 16 | 1 | `complete=1` | PASS |
| `PR-C-escape-split-000` | G,K | P | 22 | 1 | `complete=1` | PASS |
| `PR-C-escape-split-001` | G,K | P | 22 | 1 | `complete=1` | PASS |
| `PR-C-escape-split-002` | G,K | P | 22 | 1 | `complete=1` | PASS |
| `PR-C-escape-split-003` | G,K | P | 22 | 1 | `complete=1` | PASS |
| `PR-C-escape-split-004` | G,K | P | 22 | 1 | `complete=1` | PASS |
| `PR-C-escape-split-005` | G,K | P | 22 | 1 | `complete=1` | PASS |
| `PR-C-escape-split-006` | G,K | P | 22 | 1 | `complete=1` | PASS |
| `PR-C-escape-split-007` | G,K | P | 22 | 1 | `complete=1` | PASS |
| `PR-C-escape-split-008` | G,K | P | 22 | 1 | `complete=1` | PASS |
| `PR-C-escape-split-009` | G,K | P | 22 | 1 | `complete=1` | PASS |
| `PR-C-escape-split-010` | G,K | P | 22 | 1 | `complete=1` | PASS |
| `PR-C-escape-split-011` | G,K | P | 22 | 1 | `complete=1` | PASS |
| `PR-C-escape-split-012` | G,K | P | 22 | 1 | `complete=1` | PASS |
| `PR-C-escape-split-013` | G,K | P | 22 | 1 | `complete=1` | PASS |
| `PR-C-escape-split-014` | G,K | P | 22 | 1 | `complete=1` | PASS |
| `PR-C-escape-split-015` | G,K | P | 22 | 1 | `complete=1` | PASS |
| `PR-C-escape-split-016` | G,K | P | 22 | 1 | `complete=1` | PASS |
| `PR-C-escape-split-017` | G,K | P | 22 | 1 | `complete=1` | PASS |
| `PR-C-escape-split-018` | G,K | P | 22 | 1 | `complete=1` | PASS |
| `PR-C-escape-split-019` | G,K | P | 22 | 1 | `complete=1` | PASS |
| `PR-C-escape-split-020` | G,K | P | 22 | 1 | `complete=1` | PASS |
| `PR-C-escape-split-021` | G,K | P | 22 | 1 | `complete=1` | PASS |
| `PR-C-escape-split-022` | G,K | P | 22 | 1 | `complete=1` | PASS |
| `PR-C-escape-bytewise` | G,K | P | 22 | 1 | `complete=1` | PASS |
| `PR-C-number-split-000` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-001` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-002` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-003` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-004` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-005` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-006` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-007` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-008` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-009` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-010` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-011` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-012` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-013` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-014` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-015` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-016` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-017` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-018` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-019` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-020` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-021` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-022` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-023` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-024` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-025` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-026` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-027` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-028` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-029` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-030` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-031` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-032` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-033` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-034` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-035` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-036` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-037` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-038` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-039` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-040` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-041` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-042` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-043` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-split-044` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-number-bytewise` | H,K | P | 44 | 1 | `complete=1` | PASS |
| `PR-C-root-number-split-000` | C,H,J,K | P | 5 | 1 | `complete=1` | PASS |
| `PR-C-root-number-split-001` | C,H,J,K | P | 5 | 1 | `complete=1` | PASS |
| `PR-C-root-number-split-002` | C,H,J,K | P | 5 | 1 | `complete=1` | PASS |
| `PR-C-root-number-split-003` | C,H,J,K | P | 5 | 1 | `complete=1` | PASS |
| `PR-C-root-number-split-004` | C,H,J,K | P | 5 | 1 | `complete=1` | PASS |
| `PR-C-root-number-split-005` | C,H,J,K | P | 5 | 1 | `complete=1` | PASS |
| `PR-C-root-number-bytewise` | C,H,J,K | P | 5 | 1 | `complete=1` | PASS |
| `PR-C-incomplete-number-split-000` | H,J,K | P | 3 | 0 | `complete=0; Fehler bei complete` | PASS |
| `PR-C-incomplete-number-split-001` | H,J,K | P | 3 | 0 | `complete=0; Fehler bei complete` | PASS |
| `PR-C-incomplete-number-split-002` | H,J,K | P | 3 | 0 | `complete=0; Fehler bei complete` | PASS |
| `PR-C-incomplete-number-split-003` | H,J,K | P | 3 | 0 | `complete=0; Fehler bei complete` | PASS |
| `PR-C-incomplete-number-bytewise` | H,J,K | P | 3 | 0 | `complete=0; Fehler bei complete` | PASS |
| `PR-C-incomplete-string-split-000` | J,K | P | 9 | 0 | `complete=0; Fehler bei complete` | PASS |
| `PR-C-incomplete-string-split-001` | J,K | P | 9 | 0 | `complete=0; Fehler bei complete` | PASS |
| `PR-C-incomplete-string-split-002` | J,K | P | 9 | 0 | `complete=0; Fehler bei complete` | PASS |
| `PR-C-incomplete-string-split-003` | J,K | P | 9 | 0 | `complete=0; Fehler bei complete` | PASS |
| `PR-C-incomplete-string-split-004` | J,K | P | 9 | 0 | `complete=0; Fehler bei complete` | PASS |
| `PR-C-incomplete-string-split-005` | J,K | P | 9 | 0 | `complete=0; Fehler bei complete` | PASS |
| `PR-C-incomplete-string-split-006` | J,K | P | 9 | 0 | `complete=0; Fehler bei complete` | PASS |
| `PR-C-incomplete-string-split-007` | J,K | P | 9 | 0 | `complete=0; Fehler bei complete` | PASS |
| `PR-C-incomplete-string-split-008` | J,K | P | 9 | 0 | `complete=0; Fehler bei complete` | PASS |
| `PR-C-incomplete-string-split-009` | J,K | P | 9 | 0 | `complete=0; Fehler bei complete` | PASS |
| `PR-C-incomplete-string-bytewise` | J,K | P | 9 | 0 | `complete=0; Fehler bei complete` | PASS |
| `PR-C-trailing-split-0` | J,K | P | 8 | 1 | `complete=0; Fehler bei chunk` | PASS |
| `PR-C-trailing-split-1` | J,K | P | 8 | 1 | `complete=0; Fehler bei chunk` | PASS |
| `PR-C-trailing-split-2` | J,K | P | 8 | 1 | `complete=0; Fehler bei chunk` | PASS |
| `PR-C-trailing-split-3` | J,K | P | 8 | 1 | `complete=0; Fehler bei chunk` | PASS |
| `PR-C-trailing-split-4` | J,K | P | 8 | 1 | `complete=0; Fehler bei chunk` | PASS |
| `PR-C-trailing-split-5` | J,K | P | 8 | 1 | `complete=0; Fehler bei chunk` | PASS |
| `PR-C-trailing-split-6` | J,K | P | 8 | 1 | `complete=0; Fehler bei chunk` | PASS |
| `PR-C-trailing-split-7` | J,K | P | 8 | 1 | `complete=0; Fehler bei chunk` | PASS |
| `PR-C-trailing-split-8` | J,K | P | 8 | 1 | `complete=0; Fehler bei chunk` | PASS |
| `PR-C-16k-utf8` | G,K | P | 16389 | 1 | `complete=1` | PASS |
| `PR-C-16k-escape` | G,K | P | 16397 | 1 | `complete=1` | PASS |
| `PR-C-16k-number` | H,K | P | 16389 | 2 | `complete=1` | PASS |

Die vier zusätzlichen Requestkontrollen sind `PR-N-token-kinds`,
`PR-B-nested-nul`, `PR-C-root-number-split-000` und `PR-J-syntax-colon`
unter `sensitivity[]` im Bericht. Jeweils wird nur die Erwartung absichtlich
verfälscht; PASS bedeutet, dass der Checker diese falsche Erwartung ablehnt.
Sie sind nicht nochmals in den 188 Produktfällen gezählt.

Die 21 gültigen numerischen Baselineliterale (`H-number-*`) sind für die
Nachvollziehbarkeit nochmals explizit zugeordnet:

| Baseline-ID | Erwarteter unveränderter Zahlentext |
| --- | --- |
| `H-number-00` | `0` |
| `H-number-01` | `-0` |
| `H-number-02` | `1` |
| `H-number-03` | `-1` |
| `H-number-04` | `-9223372036854775808` |
| `H-number-05` | `9223372036854775807` |
| `H-number-06` | `-9223372036854775809` |
| `H-number-07` | `9223372036854775808` |
| `H-number-08` | `18446744073709551615` |
| `H-number-09` | `18446744073709551616` |
| `H-number-10` | `9007199254740991` |
| `H-number-11` | `9007199254740992` |
| `H-number-12` | `9007199254740993` |
| `H-number-13` | `1.0` |
| `H-number-14` | `1.2300` |
| `H-number-15` | `1e2` |
| `H-number-16` | `1E+02` |
| `H-number-17` | `1e-2` |
| `H-number-18` | `1e400` |
| `H-number-19` | `1e-400` |
| `H-number-20` | `999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999` |

## 6. Was diese Matrix ausdrücklich nicht beweist

Die zehn Pflichtgruppen sind durch benannte Fälle und festgelegte Erwartungen
verknüpft; eine hohe Zahl von Chunkaufteilungen beweist nicht Vollständigkeit
aller Datenformen. Neue Fälle ergänzen die vorhandene Abdeckung; sie ersetzen
weder die offiziellen Regressionen noch einen realen externen Connectorlauf.

Nicht implementiert/geprüft sind Bibliotheksmigration, JSON-Schema-Validierung,
ein neuer SAX-/DOM-Entwurf, ein Performancevergleich, vollständige
Allocator-/Fehlerinjektion, unbegrenzte Belastung und vollständige
Offset-/Sanitization-Ende-zu-Ende-Verträge. Serielle Transaktionsfolgen beweisen
keine Nebenläufigkeitssicherheit. Ein interner Parserfall setzt die
transaktionsbezogenen Fehlerflags nicht selbst; seine Leerfelder dürfen nicht
als „Transaktion fehlerfrei“ gelesen werden. Die normale Transaktion prüft
zusätzlich Parserauswahl, ARGS/ARGS_NAMES/ARGS_GET/ARGS_POST, Fehlerzustände,
Regelphase und Intervention.

Die [deutsche Originalanalyse](../json-yajl-code-analysis.md) und ihre
[vollständige englische Übersetzung](../en/json-yajl-code-analysis.md) liefern
commitfeste Codebelege zu diesen Unterscheidungen. Die
[Reproduktionsanleitung](README.de.md) beschreibt die tatsächlich verfügbaren
Runner. Veröffentlichung und Teststatus sind getrennte Nachweise: ein
vorbereitetes Repository ist noch kein neu veröffentlichtes GitHub-Repository;
ein erfolgreicher Push würde umgekehrt keinen fehlgeschlagenen Anforderungstest
bestehen lassen.
