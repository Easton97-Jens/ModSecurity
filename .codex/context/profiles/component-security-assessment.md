# Optionale Sicherheitsbewertung einer Komponente

## Anlass und Modus

Nutze dieses Profil für eine vom Nutzer angefragte Sicherheitsprüfung einer Kernkomponente. Die Bewertung selbst ist eine Analyse; implementiere Änderungen nur, wenn dies ebenfalls zur freigegebenen Aufgabe gehört.

## Zu lesen

1. [Gemeinsamer Ablauf für vertiefte Prüfungen](../security/core-deep-assessment.md).
2. Der ausgewählte [Komponentenleitfaden](../security/components/README.md).
3. Nur seine relevanten Prüfmodule sowie jedes zusätzliche Modul, das der tatsächliche Daten- oder Kontrollpfad erfordert.
4. [Sicherheitsrichtlinie](../security-policy.md), [Protokoll- und Build-Grenzen](../protocol-and-hardening.md) und die betroffene Implementierung, Aufrufer und Tests.

## Umfang

Beginne mit einer Komponente und verfolge ihre konkreten Abhängigkeiten. Halte fest, warum gemeinsamer Engine-, Parser-, API-, Speicher- oder Build-Code einbezogen wird. Submodulinterna und externe Aufrufer bleiben getrennte Quellgrenzen; behaupte keine Prüfung nicht gelesenen Materials.

Ermittle kontrollierte Eingaben, Berechtigungen, vorgesehene Kontrollen, zulässiges Verhalten und die stärksten Gegenbelege. Validiere plausible Kandidaten mit der am besten geeigneten verfügbaren Methode; kein Scanner wird als installiert oder verpflichtend vorausgesetzt.

## Ergebnis

Berichte den erklärten Umfang, die genaue Revision und Konfiguration, geprüfte Pfade, ausgeführte Prüfungen, Kandidaten und ihre Einstufungen, Nachweisgrenzen und verbleibende Arbeit. Trenne Empfehlungen von tatsächlichen Korrekturen und verifizierten Ergebnissen.

Der Abschluss beschreibt den abgeglichenen Bewertungsumfang, keinen Beweis für Fehlerfreiheit der Komponente oder eine Produktionsfreigabe.
