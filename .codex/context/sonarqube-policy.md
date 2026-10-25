# Anwendbarkeit von SonarQube und Nachweise

## Bedingter Geltungsbereich

Dies ist eine Arbeitsrichtlinie für Aufgaben, die SonarQube tatsächlich verwenden. Sie
behauptet nicht, dass für diesen Fork ein SonarQube-Server, Projektschlüssel, Scanner,
verbundenes Konto, Wrapper, eine CI-Analyse oder ein verbindliches Qualitätskriterium konfiguriert ist.

Kläre die Anwendbarkeit anhand der aktuellen Aufgabe und der tatsächlichen Repository-, PR-,
Workflow- oder Dienstkonfiguration. Ist SonarQube für eine reine Dokumentationsänderung
nicht relevant, richte keine neue Analyse allein zur Erfüllung dieser Richtlinie ein.

Das Makefile des festgelegten Kernstands bietet ein auf cppcheck beruhendes Ziel `check-static`.
Diese quellcodebelegte Prüfung ist von jeder SonarQube-Integration getrennt.
Quelle: [Definition der statischen Analyse][static]. Nutze für den Kern-Build und weitere
verfügbare Prüfungen [Build und Tests](build-and-test.md).

## Zugriffs- und Befehlsgrenzen

Nutze die unterstützte Schnittstelle, die in der aktiven Umgebung verfügbar ist, etwa einen
konfigurierten Connector, eine CLI, einen Scanner oder einen authentifizierten Dienst zum
Abrufen der Ergebnisse. Ermittle ihre tatsächlichen Fähigkeiten, bevor du einen Befehl
zusammenstellst. Eine Abfrageschnittstelle ermöglicht nicht automatisch Scans, und ein
Scanner ermöglicht nicht automatisch administrativen Zugriff.

Beachte die [Richtlinie zur Befehlsausführung](command-execution-policy.md). Erfinde weder
festen Wrapper-Pfad, Authentifizierungsvariable, Serveradresse, Projektkennung noch CLI-Flag.
Verlangt die aktuelle Umgebung einen Authentifizierungs-Wrapper, lies dessen dokumentierte
Schnittstelle und nutze sie, ohne diese Grenze zu umgehen.

Nutze den konfigurierten Authentifizierungsmechanismus. Gib niemals Tokens aus, erstelle
keine vollständigen Umgebungsabbilder, speichere keine Zugangsdaten in versionierter Konfiguration
und übernimm keine geheimen Werte in Argumente, Protokolle oder Nachweise. Fehlender Zugriff ist
eine konkrete Einschränkung; er erlaubt keine Suche nach Zugangsdaten in sachfremden Dateien.

Kläre vor dem Hochladen einer Analyse das vorgesehene Projekt beziehungsweise Ziel und ob das
Senden der ausgewählten Quellen oder Analysedaten dorthin zum Aufgabenumfang gehört.
Eine rein lesende Befundprüfung erlaubt für sich genommen weder Änderungen an Servereinstellungen
noch die Veröffentlichung eines neuen Scans.

## Ergebnisse dem Code zuordnen

Nutze für eine PR-Entscheidung Ergebnisse, die zur aktuellen Head-SHA des PR gehören.
Halte bei einer lokalen oder eigenständigen Analyse den analysierten Commit, wesentliche
nicht committete Änderungen, die Build-Konfiguration sowie einbezogene und ausgeschlossene
Bereiche fest. Eine Analyse eines älteren Commits bleibt ein Nachweis für diese ältere Eingabe.

Ein erfolgreich beendeter Scanner-Prozess oder ein hochgeladener Bericht beweist weder den
Abschluss der Serververarbeitung noch das Bestehen eines Qualitätskriteriums. Hängt die
Aufgabe davon ab, rufe die endgültigen Analyse- und Qualitätsergebnisse ab.
Kennzeichne ausstehende, fehlgeschlagene, abgebrochene, nicht zugängliche oder unvollständige
Ergebnisabrufe entsprechend. Beschreibe eine einzelne Seite von Befunden nicht als vollständige Ergebnismenge.

## Befunde und Änderungen an Qualitätskriterien

Behandle gemeldete Probleme als Nachweise, die eine Untersuchung des tatsächlichen Codes und
Ausführungspfads erfordern. Unterscheide Zuverlässigkeitsprobleme, Sicherheitsbefunde,
prüfbedürftige Sicherheitsstellen, Wartbarkeit, Duplikation, Abdeckung, Analysefehler und
mögliche Fehlalarme.

Kläre vor Codeänderungen, ob ein Befund diese Aufgabe und Revision betrifft. Unterdrücke
keine Regel, erweitere keinen Ausschluss, senke keinen Schwellenwert, ändere kein
Qualitätskriterium und stufe kein Problem als sicher ein, nur um ein grünes Ergebnis zu erhalten.
Eine begründete Konfigurations- oder Einstufungsänderung ist eine eigenständige, prüfbare
Handlung innerhalb der für die Aufgabe geltenden Befugnisse.

Ein anwendbares verbindliches Qualitätskriterium bleibt Teil der Bereitstellung. Ein Kriterium,
dessen Anwendbarkeit nicht festgestellt wurde, darf nicht erfunden werden. Beachte
[Bereitstellung und CI](delivery-and-ci.md) für die bereitgestellte Revision und den abschließenden Status.

## Aufzubewahrende Nachweise

Halte den verwendeten Mechanismus, Repository- und Projektidentität, den genauen analysierten
Stand und Umfang, die Analysekennung, soweit verfügbar, den Exit-Status des Scanners,
den Serververarbeitungsstatus, das Qualitätsergebnis, relevante Befunde und Grenzen fest.
Bewahre nur Ausgaben ohne geheime Inhalte auf, die zum Belegen der Aussage erforderlich sind.

Kann die Analyse nicht fortgesetzt werden, nenne die konkrete Ursache, etwa nicht verfügbare
Werkzeuge, fehlenden konfigurierten Zugriff, einen nicht erreichbaren Dienst, eine fehlgeschlagene
Analyse, einen veralteten Stand oder einen unvollständigen Ergebnisabruf. Setze unabhängige
Arbeit innerhalb des Umfangs fort, ohne fehlende Nachweise durch einen angenommenen Erfolg zu ersetzen.

[static]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/Makefile.am#L58-L75
