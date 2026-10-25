# Optionale nachweisgestützte Qualitätsbewertung mit Punkten

Nur verwenden, wenn der Nutzer eine Punktbewertung verlangt. Dies ist eine vorgeschlagene Bewertungskonvention, kein Upstream-Qualitätsstandard und keine abgeschlossene Bewertung.

## Die Bewertung zuerst festlegen

Halte vor der Ausführung Umfang und Revision, Kriterien, beobachtbare Abnahmebedingungen, Gewichtungen, erforderliche Nachweise und begründete Ausschlüsse fest. Entferne danach keine fehlgeschlagenen oder unbelegten Bedingungen, um die Punktzahl zu verbessern.

Ein mögliches 100-Punkte-Schema nutzt vier Bereiche mit jeweils 25 Punkten. Wähle pro Bereich fünf konkrete Kriterien mit jeweils bis zu fünf Punkten:

| Bereich | Für die ausgewählte Aufgabe zu konkretisierende Kriterienvorschläge |
| --- | --- |
| API und Lebenszyklus | Kompatibilität öffentlicher Schnittstellen; Eigentümerschaft; Längen und Grenzen; Fehler- und Freigabeverträge; Verhalten von Callbacks und gemeinsamem Zustand. |
| Regeln und Datenverarbeitung | Parser-Akzeptanz; Phasen- und Aktionssemantik; Transformationen; Bedingungen der Körperverarbeitung; konfigurierte Grenzwert- und Fehlerbehandlung. |
| Build, CI und Abhängigkeiten | Normaler Build; Identität optionaler Funktionen; Abhängigkeitsherkunft; Grenzen von Workflow-Eingaben und Berechtigungen; Artefaktidentität. |
| Nachweise und Wartbarkeit | Aussagekräftige Fälle und Kontrollfälle; Revisionszuordnung; erklärte Abdeckung; reproduzierbare Befehle; erklärte offene Arbeit. |

Dies sind Planungskategorien, keine Behauptungen, dass jede bereits bewertet wurde. Definiere vor der Punktevergabe messbare Bedingungen, die zum angefragten Umfang passen.

## Berechnung

Nutze für Kriterium `i` die Formel `points_i = weight_i * satisfied_conditions / applicable_conditions`. Fehlende Nachweise für eine anwendbare Bedingung ergeben keine Punkte; sie belegen nicht automatisch einen Produktfehler.

Berechne `score = 100 * earned_points / applicable_max_points`. Entferne eine Bedingung nur mit einer technischen Begründung `not_applicable` aus dem Nenner. Fehlende Werkzeuge, Zugriff oder Zeit sind Abdeckungslücken. Gibt es keinen anwendbaren Nenner, berichte keinen Prozentwert.

Wird ein Punktrechner implementiert, prüfe leere und doppelte Kriterien, vollständig erfüllte, vollständig unerfüllte und teilweise erfüllte Bedingungen, begründete Ausschlüsse, fehlende Nachweise sowie Rundung nahe 100 %. Veröffentliche eine Berechnung nicht als ausgeführt, wenn sie nicht tatsächlich ausgeführt wurde.

## Getrennt berichten

Halte Bewertungspunktzahl, Anzahl ausgeführter Tests, Testerfolg, gemessene Codeabdeckung, Scanner- beziehungsweise Qualitätsergebnisse, Befunde und Integrationsnachweise getrennt. Eine Punktzahl von 100 bedeutet nur, dass die vorab festgelegten anwendbaren Bedingungen belegt sind; sie bedeutet weder allgemeine Sicherheit noch Produktionsreife.
