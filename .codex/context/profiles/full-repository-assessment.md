# Optionale Bewertung des gesamten Repositorys

## Anlass und Umfang

Nutze dieses Profil, wenn der Nutzer ausdrücklich eine repository-weite Bewertung verlangt. Lege fest, ob damit Quellcodeprüfung, Build- und Testausführung, Sicherheitsanalyse, Wartbarkeit, Nachweisintegrität oder eine genannte Kombination gemeint ist. Dieses Profil ergänzt nicht stillschweigend jeden möglichen Scan oder jede externe Integration.

Die [Repository-Übersicht](../repository-map.md) ist ein Einstiegspunkt in das Inventar. Ermittle den tatsächlichen versionierten Dateibaum und die Revisionen; unterscheide Code des Haupt-Repositorys, generierte Dateien, Submodule, Dokumentation und externe Projekte. Inventarabdeckung ist keine inhaltliche Prüfung.

## Die Arbeit zusammenstellen

- Nutze die [Abdeckung aller Komponenten](all-components-assessment.md) für die sechs Kernbereiche.
- Ergänze die [Sicherheit aller Komponenten](all-components-security-assessment.md), wenn eine vertiefte Sicherheitsanalyse ausdrücklich zur Aufgabe gehört.
- Nutze die [Bewertung von Build, CI und Lieferkette](build-ci-supply-chain-assessment.md) für ausgewählte Build- und Bereitstellungsdefinitionen.
- Nutze die [Nachweisintegrität](evidence-integrity-assessment.md) für bestehende und neu entstandene Aussagen.
- Ergänze [externe Analyse](../security/analysis-services-policy.md) oder [Qualitätsbewertung](../security/quality-scoring-policy.md) nur auf Anfrage.
- Nutze die [strikte Vollständigkeit](strict-completeness-assessment.md) nur, wenn der Nutzer diese Ausführungsanforderung ausdrücklich auswählt.

## Abdeckung dokumentieren

Halte ausgewählte Verzeichnisse und Komponenten, Build- und Konfigurationsvarianten, relevante Tests, erwartete Beobachtungen, tatsächliche Befehle, erreichte Stufe, Ergebnisse sowie ungeprüfte beziehungsweise unausgeführte Bereiche fest. Beziehe Dokumentation, Testinfrastruktur und Werkzeuge ein, wenn sie zum erklärten Umfang gehören.

Eine fehlgeschlagene Voraussetzung ist ein Einrichtungsergebnis und eine Lücke beim unausgeführten Ziel. Fehlender Zugriff oder eine fehlende Umgebung machen ein relevantes Ziel nicht technisch unanwendbar. Setze unabhängige freigegebene Arbeit fort und erhalte dabei die Lücke.

Verhalten externer Connectoren, externe Regelwerke, Submodulinterna und laufende Bereitstellungen benötigen jeweils eigene ausgewählte Quellen und Nachweise. Übernimm nicht die Host-Familien- oder CRS/MRTS-Matrix des Referenz-Repositorys.

## Ergebnis und Abschluss

Erstelle einen hilfreichen Bericht statt leerer Platzhalterdateien: Umfang und Inventar, Abdeckungstabelle, Quellbeobachtungen, ausgeführte Ergebnisse, Kandidateneinstufungen, Grenzen und verbleibende Schritte. Dokumentiere zur Aufgabe gehörende Artefakte und ihre Bereinigung, wenn die Ausführung solche erzeugt hat.

Abschluss bedeutet, dass der erklärte Bewertungsumfang vollständig erfasst wurde. Er bedeutet nicht, dass alle Produktprüfungen bestanden wurden, alle Sicherheitslücken ausgeschlossen sind oder die Produktionsnutzung freigegeben ist.
