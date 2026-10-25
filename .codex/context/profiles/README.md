# Optionale Bewertungsprofile

Wähle ein Profil nur, wenn es zur angefragten Nutzeraufgabe passt. Diese Dateien sind wiederverwendbare Pläne, keine abgeschlossenen Prüfungen, verpflichtenden Startscans oder Aussagen darüber, dass Werkzeuge und Laufzeitumgebungen verfügbar sind.

| Profil | Auswählen, wenn das angefragte Ziel Folgendes ist |
| --- | --- |
| [Kernbibliothek](core-library-assessment.md) | Ein begrenztes Bibliotheksverhalten anhand tatsächlicher Build-, Test- oder API-Nachweise prüfen. |
| [Alle Komponenten](all-components-assessment.md) | Die Abdeckung der sechs Kernbereiche prüfen, ohne einen tiefgehenden Sicherheitsscan zu unterstellen. |
| [Komponentensicherheit](component-security-assessment.md) | Sicherheitsfragen in einer ausgewählten Komponente und ihren erreichbaren Abhängigkeiten untersuchen. |
| [Sicherheit aller Komponenten](all-components-security-assessment.md) | Eine angefragte vertiefte Prüfung über die Kernbereiche hinweg durchführen. |
| [Build, CI und Lieferkette](build-ci-supply-chain-assessment.md) | Build-Identität, Abhängigkeiten, Workflows und ihre Nachweise prüfen. |
| [Nachweisintegrität](evidence-integrity-assessment.md) | Feststellen, ob bestehende Aussagen belegt und korrekt zugeordnet sind. |
| [Gesamtes Repository](full-repository-assessment.md) | Den erklärten repository-weiten Umfang bewerten und Abdeckungslücken aufzeigen. |
| [Antwortphasen](response-phase-assessment.md) | Antwortprüfung im Kern, Phasenbedingungen und Interventionssemantik prüfen. |
| [Strikte Vollständigkeit](strict-completeness-assessment.md) | Eine ausdrücklich verlangte Ergänzung zur vollständigen Ausführung auf ein anderes Profil anwenden. |

Nutze den [Komponentenindex](../security/components/README.md) für Einstiegspunkte im Quellcode und gezielte Prüfungen. Der Bewertungsumfang erlaubt für sich genommen weder Quellcodekorrekturen noch Änderungen an Abhängigkeiten, externe Veröffentlichung oder Vorgänge an externen Connectoren.

Lies die [Sicherheitsrichtlinie](../security-policy.md) zum Umgang mit Kandidaten und Ergebnissen sowie die [Protokoll- und Build-Grenzen](../protocol-and-hardening.md), bevor du Aussagen zu Integration oder Diagnose-Builds machst.
