# Optionale Leitfäden zur Sicherheitsprüfung der Kernbibliothek

Diese Dokumente sind wiederverwendbare Prüfanweisungen, keine Befunde oder abgeschlossenen Scans. Nutze sie nur für die vom Nutzer ausgewählte Aufgabe. Sie führen keine standardmäßige Pflicht zu Sicherheitsscans ein.

Die Gliederung ist aus dem [Sicherheitsindex der Connector-Referenz](https://github.com/Easton97-Jens/ModSecurity-conector/blob/42fd5a8e895aca3a36b09eb09fe5c7cf9c3399e3/.codex/context/security/README.md#L1-L23) angepasst. Dieses Repository liefert das Gestaltungsmuster; das Verhalten der Kernbibliothek muss aus den eigenen festgelegten Quellen dieses Repositorys ermittelt werden.

## Lesereihenfolge

1. Wähle ein [Bewertungsprofil](../profiles/README.md) und halte seinen Umfang fest.
2. Lies für eine angefragte vertiefte Prüfung den [gemeinsamen Bewertungsablauf für den Kern](core-deep-assessment.md).
3. Wähle die relevanten [Komponentenleitfäden](components/README.md).
4. Lade nur die Prüfmodule, auf die die ausgewählten Komponenten und Fragen verweisen.
5. Nutze die [Sicherheitsrichtlinie](../security-policy.md) und den [Leitfaden zu Protokoll- und Build-Grenzen](../protocol-and-hardening.md), um Nachweise und Umfang ausdrücklich festzuhalten.

## Gemeinsame Hinweise

| Leitfaden | Verwendung |
| --- | --- |
| [Vertiefte Bewertung des Kerns](core-deep-assessment.md) | Eine quellcodegestützte Untersuchung planen und Kandidaten validieren. |
| [Analysedienste](analysis-services-policy.md) | Tatsächlich verfügbare externe Analyse auf Anfrage abgleichen. |
| [Qualitätsbewertung](quality-scoring-policy.md) | Eine optionale nachweisgestützte Punktbewertung vor der bewerteten Arbeit festlegen. |
| [Komponenten](components/README.md) | Bibliotheksgrenzen und relevante Prüfungen bestimmen. |

## Nachweisgrenze

Ein Ergebnis gehört zu seiner Komponente, seinem Quellstand, Aufrufer, seiner Konfiguration, den gewählten Abhängigkeiten beziehungsweise Backends und ausgeführten Fällen. Nutze Nachweise nur dann komponentenübergreifend, wenn der Bericht die gemeinsame Implementierung benennt und erklärt, warum dasselbe Ergebnis jede Aussage abdeckt. Ein Bibliotheks-Build, eine Testdefinition, ein generierter Bericht oder ein Regeltreffer belegt kein Ergebnis eines laufenden Connectors.

Die Komponentenauswahl dient der Navigation und beweist nicht, dass jede Datei oder mögliche Eingabe geprüft wurde. Fehlende Nachweise bleiben sichtbar.
