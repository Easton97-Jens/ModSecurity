# Optionale Sicherheitsbewertung über die Kernkomponenten hinweg

## Anlass

Nutze dieses Profil nur, wenn der Nutzer eine breite oder vertiefte Sicherheitsabdeckung der Kernbibliothek verlangt. Es wird durch eine Änderung an gemeinsamem Code nicht automatisch aktiviert.

Wende die [Komponentensicherheitsbewertung](component-security-assessment.md) auf Engine, Regeln und Parser, Verarbeitung der Nachrichtenkörper, Protokollierung und Speicherung, API-Grenze sowie Build und Abhängigkeiten aus dem [Komponentenindex](../security/components/README.md) an.

## Abdeckung und gemeinsame Ursachen

Halte für jede Komponente Einstiegspunkte, Kontrolle über Eingaben, Konfiguration, Kandidatenkennungen, Validierung und Lücken sichtbar. Verfolge komponentenübergreifende Pfade wie Parser zur Regelausführung oder Nachrichtenkörper über Variablen zur Aktionsverarbeitung, wenn der tatsächliche Quellcode dies erfordert.

Gemeinsamer Code kann gemeinsame Nachweise nur dann stützen, wenn der Bericht den gemeinsamen Pfad und seine Bedingungen erklärt. Erhalte unabhängig erreichbare Vorkommen und unterschiedliche Annahmen zu Build, Backend und Aufrufer.

Prüfe externe Abhängigkeiten oder Connectoren nur, wenn sie ausdrücklich zum Umfang gehören. Ein Gitlink, eine öffentliche API oder die Erwähnung eines Connectors belegt keine Prüfung dieses externen Codes.

## Ausführung und Ergebnis

Nutze begrenzte Prüfungen und unabhängige, zur Aufgabe gehörende Ausgaben. Fehlende Voraussetzungen bleiben Abdeckungslücken; schwäche keine Prüfaussagen ab, um einen bestandenen Status zu erreichen.

Erstelle eine Tabelle zur Komponentenabdeckung, eine Zusammenfassung komponentenübergreifender Kandidaten, eine Befehls- und Nachweisaufzeichnung, Grenzen und vorgeschlagene nächste Schritte. Aus diesem Profil folgen weder eine feste Scanzahl noch eine allgemeine Vollständigkeitsbehauptung oder Produktionsfreigabe.
