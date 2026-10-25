# Integration in v3/master des Forks

Der Dateiname folgt dem gewählten Richtlinienaufbau. Das hier beschriebene Ziel ist
`Easton97-Jens/ModSecurity:v3/master`, nicht ein angenommener Branch namens `master`.
Die deutsche Kontextarbeit liegt auf `codex/kontext-v3-de`; ihre englische Grundlage
liegt auf `codex/kontext-v3`. Der aktuelle Übersetzungsauftrag umfasst keinen PR oder Merge.
Prüfe vor einer später beauftragten Integration den tatsächlichen Branch- und PR-Stand.

## Freigabe

Ein Merge nach `v3/master` des Forks muss vom Nutzerauftrag gedeckt sein. Eine bereits
im Gespräch ausdrücklich erteilte Merge-Freigabe gilt für diesen Umfang weiter,
sofern der Nutzer sie nicht ändert oder zurücknimmt. Frage nicht in jeder Phase erneut
nach derselben Freigabe.

Ein Auftrag, Änderungen vorzubereiten, einen Branch hochzuladen oder einen PR zu erstellen,
fordert für sich genommen keinen Merge an. Ist eine Integration gewünscht, aber noch nicht
freigegeben, schließe zuerst konkrete Änderungen, Review und Validierung ab, damit der
Nutzer ein überprüfbares Ergebnis beurteilen kann. Benenne die verbleibende Aktion
und den Grund, warum eine Bestätigung erforderlich ist.

Eine Freigabe für diesen Fork erlaubt keine Schreibzugriffe oder Integration in das
offizielle Upstream-Repository oder ein Submodul-Repository.

## Vor einem freigegebenen Merge

Prüfe die Bedingungen, die für das tatsächliche Repository und den aktuellen PR gelten:

1. Repository, gewählter PR, Head-Branch und Ziel `v3/master` stimmen.
2. Abschließender Diff und PR-Beschreibung entsprechen dem beabsichtigten Umfang und aktuellen Code.
3. Erforderliche lokale und externe Prüfungen liefern aussagekräftige Ergebnisse für den
   aktuellen Head; erforderliche Fehlschläge oder fehlende Nachweise sind geklärt.
4. Erforderliche Reviews, Gespräche, Branch-Schutzregeln und Merge-Bedingungen sind erfüllt.
5. Wesentliche aufgabenbezogene Rückmeldungen sind bearbeitet; verbleibende Unsicherheit
   wird gemäß [pr-remediation.md](pr-remediation.md) berichtet.
6. Parallele aufgabenbezogene Arbeit wird den Branch während des Vorgangs nicht verändern.
7. Die Merge-Methode folgt der Nutzeranweisung und den tatsächlichen Repository-Einstellungen.

Ermittle Pflichtprüfungen und Dienste aus der realen Repository-Konfiguration.
Erfinde keinen vorgeschriebenen Scanner, keine externe Qualitätsprüfung, kein Deployment
und kein Review, das das Repository nicht verwendet. Deaktiviere keine tatsächliche
Pflichtprüfung und nutze keine administrative Umgehung, um einen Merge zu ermöglichen.

## Die Aktion an die geprüfte Revision binden

Lies die aktuelle Head-SHA unmittelbar vor dem Merge erneut. Nutze eine unterstützte
Prüfung auf den erwarteten Head, wenn API oder Werkzeug dies anbieten. Hat sich der
Head geändert, prüfe den Unterschied und wiederhole Prüfungen, deren Nachweise nicht
mehr gelten. Merge keine unbekannte Revision aufgrund eines älteren Reviews.

Hat sich die Basis verschoben oder wurde ein Konflikt gelöst, beurteile den resultierenden
Diff und erforderliche Prüfungen erneut. Verwende frühere Ergebnisse nicht als Nachweise
für Code, den sie nicht geprüft haben. Eine Aktualisierung des Zielbranches erlaubt
keine sachfremden Änderungen oder das Fortschreiben von Abhängigkeitsständen.

## Nach der Integration

Bestätige den maßgeblichen Merge-Status des PR und den resultierenden Commit im Zielbranch.
Prüfe anwendbare Workflows nach dem Merge, soweit sie zum beauftragten Ergebnis gehören,
und unterscheide abgeschlossene, ausstehende, fehlgeschlagene und nicht beobachtete Ergebnisse.

Halte beobachteten PR, Head- und Ziel-Commits, Prüfungen und verbleibende Arbeit in
[current-state.md](current-state.md) und einer hilfreichen [Sitzungsaufzeichnung](../audit/sessions/)
fest. Behaupte keine abgeschlossene Integration allein aufgrund eines Klicks oder einer
Werkzeugantwort, ohne das Ergebnis zu bestätigen.

Erhalte sachfremde lokale Arbeit. Behalte Aufgabenbranches oder Arbeitsbäume, sofern
Aufräumen nicht beauftragt oder bereits freigegeben ist; entferne nur identifizierte,
zur Aufgabe gehörende Objekte, wenn dies sicher möglich ist.
