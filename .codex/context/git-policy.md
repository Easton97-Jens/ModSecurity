# Git-Richtlinie für den ModSecurity-Kern-Fork

Diese Richtlinie unterstützt überprüfbare Arbeit im bestehenden Fork. Sie ersetzt
weder den Nutzerauftrag noch bereits im Gespräch erteilte Freigaben.
Nutze [master-integration.md](master-integration.md) für einen beauftragten Merge
in den Zielbranch. Für die deutsche Übersetzung ist kein Pull Request oder Merge beauftragt.

## Identität von Repository und Branch

| Rolle | Repository | Branch dieser Kontextaufgabe |
| --- | --- | --- |
| Offizielle Quelle | `owasp-modsecurity/ModSecurity` | `v3/master` |
| Arbeitsfork des Nutzers, deutsche Variante | `Easton97-Jens/ModSecurity` | `codex/kontext-v3-de` |
| Englische Übersetzungsgrundlage | `Easton97-Jens/ModSecurity` | `codex/kontext-v3` |
| Zielbranch für eine gegebenenfalls später beauftragte Integration im Fork | `Easton97-Jens/ModSecurity` | `v3/master` |

Der Quellcode-Ausgangsstand ist separat in [upstream-lock.json](upstream-lock.json)
festgehalten. Ein Branch-Name ist kein unveränderlicher Quellstand.
[using-this-fork.md](using-this-fork.md) und [provenance.md](provenance.md) erläutern den Kontext.

Prüfe vor Änderungen oder Bereitstellung den relevanten Repository-Zustand:

```sh
git status --short
git branch --show-current
git rev-parse HEAD
git remote -v
```

Nutze den festgelegten Arbeitsbranch für diese Aufgabe. Wähle für spätere unabhängige
Arbeit einen passenden Aufgabenbranch oder isolierten Arbeitsbaum, wenn dies andere
Arbeit schützt. Mache einen neuen Arbeitsbaum oder Branch nicht zu einem verpflichtenden
Ritual. Wechsle, setze zurück oder erstelle den aktiven Nutzerbranch nicht neu,
nur weil eine allgemeine Anleitung einen anderen Ausgangspunkt vorschlägt.

## Arbeit und Umfang bewahren

Prüfe Änderungen innerhalb und außerhalb des Git-Index vor dem Staging oder Commit.
Erhalte vorhandene versionierte und unversionierte Arbeit. Nimm ausdrücklich benannte,
zur Aufgabe gehörende Pfade auf, beispielsweise `git add -- <task-files>` mit den
tatsächlich vorgesehenen Pfaden. Vermeide pauschales Staging in einem gemischten Arbeitsbaum.

Destruktives Aufräumen, das Verwerfen von Änderungen, Umschreiben veröffentlichter Historie,
Force-Pushes und Löschen von Branches sind keine Routine. Solche Aktionen müssen Teil
einer klar freigegebenen Aufgabe sein und Repository-Schutzregeln beachten.
Löse gewöhnliche Konflikte innerhalb einer freigegebenen Änderung, wenn das gewünschte
Ergebnis klar ist; frage nach, wenn ein Konflikt eine offene Produktentscheidung aufzeigt.

Quellcode, Historie und Gitlinks von Submodulen bleiben durch
[architecture-and-boundaries.md](architecture-and-boundaries.md) abgegrenzt.
Ein Commit im Kernrepository erlaubt nicht, Änderungen im Repository einer Abhängigkeit zu veröffentlichen.

## Bereitstellung im Remote-Repository

Wenn Commit, Push oder PR-Arbeit zum Auftrag gehören, behalte diese Freigabe über die
Aufgabe hinweg bei. Prüfe vor einem Remote-Schreibzugriff die tatsächliche Repository-Identität,
Fetch- und wirksamen Push-Ziele, den ausgewählten Branch und gegebenenfalls die PR-Basis.
Setze nicht voraus, dass ein Remote namens `origin` auf diesen Fork zeigt oder ein
Remote namens `upstream` die erwartete Bedeutung hat.

Eine Zielabweichung muss untersucht werden. Wähle nicht stillschweigend ein anderes
Repository, erstelle kein Ersatzrepository und ändere keine Remotes, um die Abweichung
zu verdecken. Schließe die überprüfbare lokale Arbeit ab, während eine tatsächlich
fehlende Zielentscheidung geklärt wird.

Halte Commits zusammenhängend und ihre Nachrichten sachlich. Prüfe nach einem
freigegebenen Push, dass der vorgesehene Remote-Branch und, falls vorhanden und beauftragt,
der PR-Head den ausgelieferten Commit bezeichnen. Ein lokaler Commit oder ein erfolgreich
aufgerufener Befehl allein beweist den erreichten Remote-Zustand nicht.

Ein erstellter PR erlaubt für sich genommen keinen Merge. Umgehe keine Branch-Schutzregeln,
pushe nicht zur Abkürzung direkt nach `v3/master` und behaupte keinen Merge, bevor er
beobachtet wurde. Halte tatsächliche Bereitstellungsfakten in
[current-state.md](current-state.md) oder der relevanten [Sitzungsaufzeichnung](../audit/sessions/) fest.
