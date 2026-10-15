# Originale JSON-Verarbeitung mit YAJL: Ergebnisübersicht

Diese Untersuchung hält das Verhalten des offiziellen ModSecurity fest, bevor über eine Ersatzbibliothek entschieden wird. YAJL und sämtliche Produktionsdateien bleiben unverändert. Die neuen Tests und diese Dokumentation gehören ausschließlich zu zwei neuen Branches im Fork [Easton97-Jens/ModSecurity](https://github.com/Easton97-Jens/ModSecurity).

## Lesen und reproduzieren

1. [So verarbeitet ModSecurity einen JSON-Request; technische Analyse und Prüfung von fzipis Aussagen](json-yajl-code-analysis.md).
2. Versionsbericht mit ausgeführten Befehlen, Test-IDs, Erwartungen, Ergebnissen und Grenzen: [v3](json-yajl-baseline-v3.md). Der Bericht für v2 steht unter `doc/json-yajl-baseline-v2.md` auf `test/json-yajl-baseline-v2-20261015`. Jeder Branch enthält seine eigenen Tests; der abschließende Übergabebericht verlinkt beide veröffentlichten Stände.
3. [Herkunftsmanifest](json-yajl-provenance.json): vollständige Original-SHAs, bestehende Fork-Referenzen, Vergleiche, Submodule, Arbeitsverzeichnisse und Umgebung.

| Linie | Originalbranch und festgeschriebener Commit | Neuer Aufgabenbranch | Lokales Arbeitsverzeichnis |
|---|---|---|---|
| v2 | `v2/master`, `151074aca78a416764b12ba1221e61a17a6c714d` | `test/json-yajl-baseline-v2-20261015` | `/workspace/work/json-yajl-baseline/v2` |
| v3 | `v3/master`, `8271c9d3bc0abcbd1189f5414c196c336c959047` | `test/json-yajl-baseline-v3-20261015` | `/workspace/work/json-yajl-baseline/v3` |

Die Originalcommits wurden am 15. Oktober 2026 abgerufen; Abschluss des expliziten Abrufs: `2026-10-15T12:22:19+00:00` (14:22:19 MESZ). Danach wurde die Basis nicht aktualisiert. Der [Ausgangskommentar von fzipi](https://github.com/owasp-modsecurity/ModSecurity/issues/3308#issuecomment-6166461842) wurde selbst gelesen. Bibliotheksauswahl, Parserwechsel, Produktionsfixes, Pull Requests und Tags gehören nicht zu dieser Arbeit.

## Was die Ergebnisse bedeuten

Ein Request-Body ist die Folge von Bytes hinter den HTTP-Headern. ModSecurity reicht sie an einen Parser weiter, sobald die Konfiguration den JSON-Prozessor auswählt. YAJL ruft bei Schlüsseln, Werten und Containergrenzen Funktionen von ModSecurity auf. Diese **Callbacks** bauen keinen vollständigen JSON-Baum auf, sondern legen skalare Werte als Argumente an. Eine Regel kann diese über Collections wie `ARGS` lesen. Welche Collections genau betroffen sind, wann Regeln laufen und wie Fehler sichtbar werden, erklärt die technische Analyse.

Drei Ebenen bleiben getrennt: ein Parser kann einen Syntaxfehler melden; ModSecurity setzt daraufhin einen Fehlerzustand; eine Regel, ein Engine-Limitpfad oder der Webserver entscheidet über die HTTP-Antwort. Beispielsweise kann v2 einen erst beim Parserabschluss erkannten Fehler als HTTP 500 vor Phase 2 zurückgeben; ein Fehler während der Eingabe kann dagegen für eine Phase-2-Regel sichtbar bleiben. Bereits angelegte Argumente können auch bei einem späteren Fehler bestehen bleiben. Deshalb prüfen die neuen Tests Argumente und Fehlerzustand gemeinsam.

Die Namen von v2 und v3 unterscheiden sich. Zahlen werden über den Text-Callback übernommen: `1.2300` und `1E+02` dürfen im Test nicht durch eine numerische Konvertierung mit `1.23` und `100` gleichgesetzt werden. Doppelte Schlüssel erfordern eine Liste von Argumenteinträgen; ein Dictionary würde Mehrfachvorkommen verlieren. Details und Grenzfälle sind in den versionsgetrennten Erwartungslisten festgelegt.

Ein bestandener Charakterisierungstest bedeutet: **So verhält sich diese Originalversion in dieser Umgebung.** Er entscheidet nicht automatisch, dass jedes beobachtete Detail erwünscht ist. Die technische Analyse und Versionsberichte unterscheiden belegtes Verhalten, mögliche Fehler, Kompatibilitätsanforderungen und offene Entscheidungen. Ein künftiger Fix muss betroffene Erwartungen bewusst ändern.

## Bestehende Arbeit und Herkunft

Der unveränderte Ausgangsklon `/workspace/ModSecurity` blieb auf seinem bestehenden Branch `work` bei `48e561b2ba44d35eec799176608720129c678777`. Er war zu Beginn sauber; vorhandene ignorierte Build-Artefakte und die Umgebung `/workspace/modsecurity-env` wurden nicht überschrieben. Es wurden keine vorhandenen lokalen Branches ausgecheckt, zurückgesetzt, gestasht, gemergt oder rebased. Die Aufgabenbranches wurden mit `git worktree add --no-track -b ... <vollständiger Original-SHA>` erzeugt.

`origin` ist der Fork, `upstream` das offizielle Original. Letzteres wurde nur gelesen. Das vollständige vorherige Fork-Refverzeichnis steht im Manifest. Die direkten Vergleiche zeigen:

- Fork `v3/master` liegt vier Originalcommits zurück; Unterschiede betreffen README, Sicherheitsrichtlinie und den Zeitstempeltyp im öffentlichen Transaktionsheader. Der JSON-Prozessor ist gleich.
- `v3/master-json` enthält im direkten Vergleich gleiche JSON-Prozessordateien, aber andere Build- und Transaktionsstände; daraus wurde nichts übernommen.
- `v3/master-json2` enthält zusätzliche Backends und deutliche Parseränderungen. Diese sind ausdrücklich keine Originalreferenz dieser Untersuchung.
- Ein Fork-Branch `v2/master` fehlt. Der vorhandene Branch `Modsecurity/v2` enthält trotz seines Namens einen v3-Quellbaum (Commitnachricht v3.0.16); er wurde nicht als v2-Quelle verwendet.

Der erste Fetch älterer Fork-Referenzen stieß beim automatisch rekursiven Abruf historischer, nicht mehr verfügbarer Python-/Sprachtest-Submodule auf Fehler. Die Elternreferenzen waren vorhanden; der explizite Fetch mit `fetch.recurseSubmodules=false` wurde erfolgreich wiederholt. Die festgeschriebenen Original-Submodule und tatsächlich verwendeten Stände stehen im Manifest und Versionsbericht. Keine fremden Submodule wurden auf deren aktuelle Branchspitzen aktualisiert.

## Originaltests und zusätzliche Tests

Die ursprünglichen Tests werden vor eigenen Teständerungen ausgeführt; die Rohprotokolle liegen außerhalb der Quellen unter `/workspace/work/json-yajl-baseline/`. Danach laufen dieselben Produktionsquellen mit erweiterten Tests. Ein Fehler aus einem alten Runner, fehlenden Abhängigkeiten, Parserablehnung und ein Absturz sind unterschiedliche Ergebnisse.

Die beiden im Kommentar genannten Dateien enthalten tatsächlich sechs v3- beziehungsweise sieben v2-Fälle. Das ist nicht die gesamte JSON-Abdeckung: weitere Tests stehen unter anderem bei Argumentlimits, Body-Limits, Transformationen und Audit-Logs. Zudem enthält bereits die genannte v3-Datei einen Leerbody-Fall und die v2-Datei eine Zahl auf oberster Ebene. Einige v2-Erwartungen erlauben ausdrücklich fehlenden JSON-Support; ihr Erfolg allein belegt daher keine funktionierende JSON-Verarbeitung. Die neuen Suiten weisen aktives YAJL nach und verwenden eindeutige Erwartungen.

Die zehn Pflichtgruppen werden jeweils über stabile IDs verfolgt: A leerer Body, B Schlüssel, C Werte auf oberster Ebene, D Arrays/Namen, E Tiefe, F Argumentlimit, G UTF-8/Unicode/Escapes, H Zahlentext, I Duplikate, J nachfolgender Inhalt/Abschluss. Die konkreten Varianten, Bytefolgen, erwarteten Argumentlisten und Ergebniszustände stehen bei den versionsbezogenen Tests. Kontrollierte Chunk-Aufteilungen sind gesonderte Parser-API-Tests; Aufteilungen in HTTP- oder Testformat-Zeilen belegen diese Eigenschaft nicht.

## Umgebung und Aussagegrenzen

Beide Builds verwenden Debian 13 auf x86-64, GCC/G++ 14.2.0 und YAJL aus Debian `2.1.0-5+b2` (Quellpaket `2.1.0-5`). Der Distributions-Changelog nennt Patches für CVE-2017-16516, CVE-2022-24795 und den nachgebesserten Fix für CVE-2023-33460. Das ist kein ungepatchtes YAJL-2.1.0. Paket- und Bibliotheks-Hashes sowie genaue Optionen gehören in die Versionsberichte beziehungsweise das Manifest.

Die Untersuchung verspricht keine Gleichheit des Speicherverbrauchs von SAX und DOM. Ein gepufferter Rohbody, Parserzustand, dekodierte Tokens, Pfadkopien und dauerhaft gespeicherte ARGS sind verschiedene Speicheranteile. Ein späteres Parsen mit anschließendem Baumdurchlauf kann ein Limit zu einem anderen Zeitpunkt anwenden. Gemessene Performance und eine Auswahl der Ersatzbibliothek sind nicht Gegenstand dieser Tests.

## Sichere Wiederherstellung

Die Veröffentlichungs-SHAs werden im abschließenden Übergabebericht außerhalb des jeweiligen Commits festgehalten. So entsteht keine endlose Folge von Commits, die nur ihre eigene SHA nachtragen. Zum Wiederherstellen genau diese verifizierte SHA aus dem Fork auschecken, anschließend die im Versionsbericht genannten Submodule auf den im Baum gespeicherten Ständen initialisieren und die dokumentierten Build-/Testbefehle ausführen.

```sh
git clone --no-checkout https://github.com/Easton97-Jens/ModSecurity.git modsecurity-json-baseline
cd modsecurity-json-baseline
git remote add upstream https://github.com/owasp-modsecurity/ModSecurity.git
git -c fetch.recurseSubmodules=false fetch --no-tags upstream \
  refs/heads/v2/master:refs/remotes/upstream/v2/master \
  refs/heads/v3/master:refs/remotes/upstream/v3/master
# Danach die im Übergabebericht verifizierte Aufgaben-SHA verwenden:
git switch --detach <VERIFIZIERTE-AUFGABEN-SHA>
git diff --stat <ORIGINAL-SHA> HEAD
```

Der Fetch oben beschafft die aktuell angebotenen Branchreferenzen; ihre Spitze ersetzt **nicht** die festgeschriebenen SHAs dieser Untersuchung. Prüfe mit `git cat-file -e <ORIGINAL-SHA>^{commit}`, ob der konkrete Originalcommit vorliegt, und beschaffe ihn bei Bedarf explizit. Es wird kein `git pull` benötigt. Der getestete Arbeitsstand wird aus dem Aufgabencommit einschließlich aller neuen Dateien rekonstruiert; ein älterer Fork-Branch genügt dafür nicht.

## Veröffentlichte Tests und abschließende Validierung

Beide neuen Aufgabenbranches wurden normal auf `origin` gepusht und anschließend per `git ls-remote` mit dem lokalen erwarteten SHA verglichen. Die folgenden Testcommits sind tatsächlich veröffentlicht und verifiziert; die späteren Dokumentationsspitzen stehen mit vollständigen SHAs im abschließenden Übergabebericht. Es wurden keine PRs, Releases oder Tags erstellt.

| Linie | Verifizierter Testcommit | Wichtigste ausführbare Tests |
|---|---|---|
| v2 | [a37f9b531d9ba8dd2d60f17eb4f3984668b34c50](https://github.com/Easton97-Jens/ModSecurity/commit/a37f9b531d9ba8dd2d60f17eb4f3984668b34c50) | [tests/json-yajl-baseline/cases.py](https://github.com/Easton97-Jens/ModSecurity/blob/a37f9b531d9ba8dd2d60f17eb4f3984668b34c50/tests/json-yajl-baseline/cases.py) |
| v3 | [57a42a31b4be6f6754dbb0fde662fbee9114cf9e](https://github.com/Easton97-Jens/ModSecurity/commit/57a42a31b4be6f6754dbb0fde662fbee9114cf9e) | [test/json-baseline/run.py](https://github.com/Easton97-Jens/ModSecurity/blob/57a42a31b4be6f6754dbb0fde662fbee9114cf9e/test/json-baseline/run.py) |

| Ausgeführte Ebene | v2 PASS / FAIL | v3 PASS / FAIL |
|---|---:|---:|
| Relevante Originalregressionen | 7 / 0 | 50 / 0 |
| Erweiterte native JSON-Datei | 11 / 0 | 9 / 0 |
| Neue Request-/Regelmatrix | 145 / 0 | 163 / 0 |
| Ergänzende echte Parser-Chunks | 611 / 0 | in obiger Matrix enthalten (7) |
| Zusätzliche Konfigurationstests | 14 / 0 | in obiger Matrix enthalten |
| Separate Audit-Matrix | 5 / 0 | 12 / 0 |
| Begrenztes ASan/UBSan | 611 / 0 | 7 / 0 |

Die neue v2-Matrix enthält 144 Fixtures plus einen YAJL-Kontrollfall; v3 enthält 159 Fälle plus vier absichtlich falsche Erwartungen als Erkennungskontrollen. Die zwölf v3-Auditprüfungen sind zehn Datensätze plus zwei solche Kontrollen. Wiederholungen unter Sanitizern und vorher/nachher ausgeführte Originalfälle sind keine zusätzlichen eindeutigen Anforderungen. Bei diesen abschließenden Funktionsläufen sind SKIP, BLOCKED und XFAIL jeweils null.

Gesondert: Die unveränderte allgemeine v2-Unit-Suite hatte **4245 PASS / 1 FAIL** (`ipMatch`, ungültiger CIDR-Präfix, bestehende Assertion). LeakSanitizer war durch die ptrace-Umgebung blockiert: **v2 1 BLOCKED, v3 6 BLOCKED**; die übrigen Statuszahlen dieser abschließend eingeordneten Leakprüfungen sind null. Beim v3-Versuch meldete der allgemeine Runner ursprünglich sechs FAIL-Exits; Rohzählung und Ursachenklassifikation bleiben im Versionsbericht nebeneinander sichtbar. Nur Parser/Testhilfe wurden für ASan/UBSan instrumentiert, die übrigen Bibliotheken nicht vollständig.

Die direkte Schlussprüfung wiederholte die betroffenen Tests nach der letzten Fixtureänderung. Leere Objekte und Arrays stehen nun ausdrücklich auch **nach** dem letzten skalaren Arrayelement. Die Gruppen A–J haben konkrete bytegetreue Erwartungen auf beiden Linien; die Tabellen der Versionsberichte nennen die Varianten und Grenzen. Nicht nachgewiesen bleiben insbesondere Speicherknappheit, vollständige Thread-/Offset-Sanitization-Verträge und einige Rohkörperdatei-/Audit-Grenzen. Die jeweiligen Versionsberichte weisen diese Lücken einzeln aus.

Die vollständigen versionsgetrennten Listen neuer und geänderter Dateien stehen im Manifest unter `deliverable_files` (v2: 17 Dateien; v3: 15 Dateien). Lokale generierte Dateien sind unter `local_only_untracked_artifacts` aufgeführt; ignorierte Build-Ausgaben und rohe Logs bleiben ebenfalls lokal. Sämtliche erforderlichen synthetischen Fixtures, Generatoren und Testhilfen sind versioniert. Separate Ersatzpatches sind nicht nötig, weil beide Linien eigene veröffentlichte Branches besitzen.
