# Begrenzte Bewertung des Verhaltens der Kernbibliothek

## Anlass

Nutze dieses Profil, wenn der Nutzer Bibliotheksverhalten, einen Build oder einen API-Pfad prüfen lassen möchte, ohne einen breiten Sicherheitsscan anzufordern. Wähle Komponente, Quellstand, Regeln beziehungsweise Konfiguration, Funktionsvarianten und beobachtbare Erwartung.

## Zu lesen

[Architektur](../architecture.md), [C++-/C-Leitfaden](../cpp-guide.md), [Build und Tests](../build-and-test.md), der relevante [Komponentenleitfaden](../security/components/README.md) und [Nachweisintegrität](../security/checks/evidence-integrity.md). Ergänze weitere Prüfungen nur, wenn die ausgewählte Frage sie erfordert.

## Vorgeschlagener Ablauf

1. Ermittle Quellen sowie tatsächliche Werkzeug- und Abhängigkeitsidentität; untersuche den bestehenden Test beziehungsweise das Testgerüst und seine Prüfaussagen.
2. Bereite nur die Ziele und Funktionsvarianten vor und baue sie, die für das angefragte Verhalten benötigt werden.
3. Bestätige, dass die vorgesehenen Fälle gefunden und ausgeführt wurden.
4. Verfolge relevante Konfigurations- und API-Eingaben, Phasenbedingungen, Ausgaben und Freigabe; beziehe bei Bedarf einen zulässigen Kontrollfall und einen relevanten Fehlerfall ein.
5. Halte die erreichte Stufe, das tatsächliche Ergebnis, Befehl und Arbeitsverzeichnis, Nachweise und Grenzen fest.

Nutze zur Aufgabe gehörende Ausgaben und erhalte sachfremde Änderungen. Leite aus einem erfolgreichen Build kein Bibliotheksergebnis und aus einer Bibliotheksintervention keine Durchsetzung durch den Connector ab. Ein Fehler vor Beginn eines Zieltests lässt diesen Test unausgeführt.

## Ergebnis

Eine knappe Tabelle mit Fallkennung, Komponente und Konfiguration, erwarteter Beobachtung, Befehl, tatsächlichem Ergebnis und verbleibender Lücke. Halte Builds, Tests, Diagnosen und externe Integrationen getrennt.
