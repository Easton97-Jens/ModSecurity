# Nachweisintegrität und Zuordnung von Aussagen

Halte für jede Aussage Komponente, Quellstand, Aufrufer beziehungsweise Testgerüst, wirksame Regeln und Einstellungen, Identität optionaler Funktionen und Abhängigkeiten, Werkzeugversionen, Befehl und Arbeitsverzeichnis, beobachtetes Ergebnis und Nachweisort fest.

- Stammt das Artefakt aus der berichteten Revision und Konfiguration oder aus einem früheren beziehungsweise vermischten Lauf?
- Wurden Tests tatsächlich gebaut, gefunden und mit aussagekräftigen Fällen und Prüfaussagen ausgeführt?
- Verbirgt ein Exit-Code ein Überspringen, einen leeren Lauf, einen abgefangenen Fehler oder eine fehlgeschlagene Voraussetzung?
- Bewahrt ein generierter Bericht die ursprünglichen Beobachtungen oder führt er eine stärkere Aussage ein?
- Wird eine Quellcodeaussage zu einem Laufzeitergebnis oder ein Kernergebnis zur Durchsetzung durch den Connector hochgestuft?
- Decken gemeinsame Nachweise tatsächlich jede berichtete Komponente und Konfiguration ab?

Halte `passed`, `failed`, `blocked`, `not_run` und begründetes `not_applicable` auseinander. Eine fehlgeschlagene Voraussetzung lässt das Ziel unausgeführt; sie ist kein Fehler des Zieltests.

Einstiegspunkte: [Build und Abhängigkeiten](../components/build-dependencies.md), [Nachweisprofil](../../profiles/evidence-integrity-assessment.md) und [bekannte Quellwidersprüche](../../fact-check.md).
