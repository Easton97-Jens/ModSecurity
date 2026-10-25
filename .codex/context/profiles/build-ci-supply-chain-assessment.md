# Bewertung von Build, CI und Lieferkette

## Anlass und Umfang

Nutze dieses Profil, wenn der Nutzer Build-Reproduzierbarkeit, Identität optionaler Funktionen, Abhängigkeiten, CI oder Artefaktherkunft bewerten lassen möchte. Grenze vor der Auswahl von Befehlen die tatsächlichen Plattformen, Workflows, Werkzeugketten und Quellstände ab.

## Zu lesen

[Build und Tests](../build-and-test.md), [Komponente Build und Abhängigkeiten](../security/components/build-dependencies.md), [Lieferkette](../security/checks/supply-chain.md), [Konfigurations- und Eingabeinjektion](../security/checks/config-injection.md), [Offenlegung](../security/checks/logging-disclosure.md) und [Nachweisintegrität](../security/checks/evidence-integrity.md). Nutze die [Recherche zu Sicherheitshinweisen](../security/checks/advisory-research.md) nur, wenn sie ausgewählt wurde.

## Fragen

- Welche Quellen, Gitlinks, generierten Dateien, Header, Bibliothek und Testprogramme gehören zum Ergebnis?
- Welche Abhängigkeiten und Funktionsschalter wurden tatsächlich erkannt, und welche Ziele existieren infolgedessen?
- Erhalten Workflow-Ereignisse, Berechtigungen, Eingaben, Caches, Artefakte, Actions beziehungsweise Images und Downloads die vorgesehene Vertrauensgrenze?
- Entspricht eine ausgeführte lokale Prüfung dem behaupteten externen Job und seiner Revision oder nur einem Teil davon?
- Sind fehlende Tests, übersprungene Testsammlungen, abgefangene Fehler und veraltete Artefakte sichtbar?

Untersuche die tatsächlichen Repository-Definitionen; erfinde weder Matrix, Paketbindung, Python-Anforderung, Scanner noch Qualitätskriterium. Eine rein lesende Prüfung externer CI erlaubt weder das Auslösen einer Bereitstellung noch Änderungen an Zugangsdaten oder Aktualisierungen von Abhängigkeiten.

## Ergebnis

Berichte das Quellen-, Werkzeug- und Abhängigkeitsinventar, ausgewählte Ziele, tatsächliche Befehlsergebnisse, die Identität optionaler Funktionen, relevante Workflow-Beobachtungen und offene Lücken. Halte Ergebnisse für Unix, Windows, normale Builds, Diagnose-Builds und externe Jobs getrennt.
