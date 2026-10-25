# Bewertung der Abdeckung aller Komponenten

## Anlass

Nutze dieses Profil, wenn der Nutzer eine Abdeckung der Hauptbereiche der Kernbibliothek verlangt. Es ordnet den Umfang; es fordert nicht automatisch einen tiefgehenden Sicherheitsscan oder jede mögliche Plattform- und Funktionskombination an.

Wende die [begrenzte Bewertung der Kernbibliothek](core-library-assessment.md) auf jede ausgewählte Zeile an. Ermittle komponentenübergreifende Pfade und begründe jede Wiederverwendung von Nachweisen.

| Komponente | Frage zur Abdeckung |
| --- | --- |
| [Engine](../security/components/engine.md) | Welches Phasen-, Aktions- und Transaktionszustandsverhalten wurde untersucht? |
| [Regeln und Parser](../security/components/rules-parser.md) | Welche akzeptierten beziehungsweise abgewiesenen Einstellungen und Erzeugungspfade wurden untersucht? |
| [Verarbeitung der Nachrichtenkörper](../security/components/body-processors.md) | Welche Format-, Grenzwert-, Zugriffs- und Fehlervarianten wurden untersucht? |
| [Protokollierung und Speicherung](../security/components/logging-storage.md) | Welche Callback-, Audit-, Kollektions- und Dateipfade wurden untersucht? |
| [API-Grenze](../security/components/api-boundary.md) | Welche C-/C++-Signaturen und Verträge zu Eigentümerschaft, Längen und Freigabe wurden untersucht? |
| [Build und Abhängigkeiten](../security/components/build-dependencies.md) | Welche Werkzeugkette, optionalen Funktionen, Herkunftsnachweise und Tests wurden untersucht? |

Halte je Zeile Quellcodeprüfung, verfügbare Tests, tatsächliche Ausführung, Konfiguration, Befunde beziehungsweise Fragen und Lücken getrennt fest. Sechs aufgeführte Zeilen beweisen keine Abdeckung jeder Datei, Funktion, Eingabe oder jedes Backends.

Parallelisiere nur unabhängige Arbeit, deren Ausgabepfade und Ressourcen nicht miteinander kollidieren. Erstelle für eine reine Kernaufgabe keine Connector- oder Transportmatrix.

Berichte `not_run` und `blocked` mit Begründung; nutze `not_applicable` nur für eine technisch ausgeschlossene Bedingung, nicht für eine unbequeme Voraussetzung.
