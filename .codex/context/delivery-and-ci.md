# Bereitstellung und kontinuierliche Integration

## Umfang und Freigaben

Diese Arbeitsrichtlinie unterscheidet Vorbereitung, Bereitstellung im Remote-Repository
und überprüfte Ergebnisse. Folge dem aktuellen Zielrepository und Branch des Nutzers.
Vorhandene Freigaben gelten weiter; eine rein lokale Aufgabe wird nicht automatisch
zu Commit, Push, Pull Request, Merge, Release oder Deployment.
Die deutsche Kontextvariante ist zum Hochladen nach `codex/kontext-v3-de` freigegeben;
ein Pull Request soll dafür nicht erstellt werden.

Bereite die gewünschten Änderungen vollständig vor und prüfe sie, bevor du nach einer
tatsächlich fehlenden abschließenden Freigabe fragst. Verlange keine wiederholte Bestätigung
für bereits beauftragte Aktionen. Umgekehrt erlaubt eine erfolgreiche Prüfung keinen
Schreibzugriff auf ein neues externes Ziel oder einen anderen Branch.

## Eine überprüfbare Änderung vorbereiten

Vor einem freigegebenen Commit oder Upload:

1. Prüfe vorgesehenes Repository, Branch, aktuellen HEAD und Remote-Identität.
2. Prüfe den vollständigen aufgabenbezogenen Diff einschließlich neuer Dateien und Submodulverweise.
3. Führe `git diff --check` und anwendbare Prüfungen aus
   [testing-and-evidence.md](testing-and-evidence.md) aus. Halte Fehlschläge und Grenzen fest.
4. Nimm nur vorgesehene Dateien in den Index auf. Schließe Zugangsdaten, lokale Caches,
   generierte Laufzeitprotokolle und andere sachfremde oder temporäre Artefakte aus.
5. Aktualisiere den deutschen Kontext und die Übergabe mit tatsächlichem Stand und nächstem Schritt.
6. Prüfe, ob aufgabenbezogene Hintergrundarbeit abgeschlossen oder beendet wurde.

Erstelle einen gezielten Commit, wenn dies zum Auftrag gehört. Erhalte sachfremde
Änderungen im Arbeitsbaum und Beiträge anderer. Führe keinen Force-Push, keine
Umschreibung gemeinsamer Historie, keinen Merge und keine Release-Veröffentlichung
als beiläufigen Bereitstellungsschritt aus.

## Was die festgehaltenen Workflow-Dateien belegen

Die folgenden Upstream-Dateien definieren CI-Abläufe am festgehaltenen Quellstand.
Sie beweisen nicht, dass Actions in diesem Fork aktiviert sind, ein Lauf erfolgreich war
oder ein bestimmter Job durch die aktuelle Branch-Schutzkonfiguration vorgeschrieben ist.

| Workflow | Durch die Quelle belegte Rolle |
| --- | --- |
| `.github/workflows/ci.yml` | Push-/PR-Workflows umfassen Linux-Build-/Testkonfigurationen und einen separaten Windows-Weg. |
| `.github/workflows/ci_new.yml` | Push-/PR-Workflows umfassen Linux-, macOS-, Windows- und cppcheck-Jobs. |
| `.github/workflows/release-windows.yml` | Definiert Windows-Build-/Paketierungsjobs und einen Veröffentlichungsjob, der an einen gepushten Tag gebunden ist; unterstützt auch manuelles Auslösen. |

Quellen: [CI-Definitionen][ci], [weitere CI-Definitionen][ci-new],
[Auslöser des Windows-Releases][release-trigger], [Veröffentlichungsbedingung][release-publish].

Nutze [build-and-test.md](build-and-test.md) für lokale Build-Details. Übernimm keine
Paketinstallationen aus Workflows, systemweiten Änderungen oder Release-Befehle in eine
Dokumentationsaufgabe. Ein Push kann konfigurierte Workflows auslösen; berücksichtige
das tatsächliche Verhalten des Ziels bei einer freigegebenen Bereitstellung.

## Remote-Ergebnisse der ausgelieferten Revision zuordnen

Prüfe nach einem freigegebenen Push, ob der Remote-Branch auf den vorgesehenen Commit zeigt.
Wenn ein PR zum Auftrag gehört, prüfe zusätzlich Zielbranch und Head-SHA. Beschreibe
im PR-Text Problem, resultierendes Verhalten, tatsächliche Validierung und wesentliche Grenzen.

Lies die anwendbaren Prüfergebnisse für genau diesen Head. Ein älterer erfolgreicher Lauf,
eine Workflow-Datei oder ein erfolgreicher Upload ist keine erfolgreiche Prüfung des
aktuellen Heads. Berichte ausstehende, fehlgeschlagene, abgebrochene, übersprungene,
nicht zugängliche und unbekannte Ergebnisse korrekt. Bearbeite erforderliche Review-Rückmeldungen
innerhalb des Umfangs und prüfe nach einem weiteren Push den neuen Head.

SonarQube ist nur relevant, wenn Aufgabe oder tatsächliche Repository-Konfiguration es
vorsehen; nutze [sonarqube-policy.md](sonarqube-policy.md). Erfinde keinen Server,
Projektbezeichner, verpflichtende Qualitätsprüfung oder erfolgreiche Analyse.

## Erreichten Stand berichten

Nutze genaue Bezeichnungen wie `local_complete`, `committed`, `pushed`, `pr_open`,
`checks_pending`, `remediation_required`, `blocked` oder `failed`.
Dies sind Berichtskonventionen und keine Behauptung, dass ein Bereitstellungsdienst existiert.

Nutze `verified_pr` nur, wenn ein PR besteht und alle anwendbaren Pflichtprüfungen sowie
aufgabenbezogenen Rückmeldungen für seinen aktuellen Head nachweislich geklärt sind.
Benenne Prüfungen, deren Anwendbarkeit nicht feststellbar war. Dieser Status erlaubt keinen
Merge oder Release. Bei einer auf lokale Vorbereitung begrenzten Aufgabe können ein
vollständiger überprüfbarer Diff und ein wahrheitsgemäßer Prüfbericht das Endergebnis sein.

[ci]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/.github/workflows/ci.yml#L1-L163
[ci-new]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/.github/workflows/ci_new.yml
[release-trigger]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/.github/workflows/release-windows.yml#L9-L31
[release-publish]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/.github/workflows/release-windows.yml#L136-L171
