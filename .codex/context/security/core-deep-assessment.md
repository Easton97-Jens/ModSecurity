# Gemeinsamer Ablauf für eine optionale vertiefte Kernprüfung

## Anlass und Umfang

Nutze diesen Ablauf, wenn der Nutzer eine vertiefte Sicherheitsprüfung der Kernbibliothek oder ausgewählter Komponenten verlangt. Der standardmäßige Bewertungsmodus ist Analyse: Quellcode lesen, begrenzte Prüfungen und Nachweise. Produktkorrekturen, Änderungen an Abhängigkeiten und externe Integrationsarbeit richten sich nach dem tatsächlichen Nutzerauftrag und werden durch dieses Dokument nicht vorausgesetzt.

Halte Arbeitscommit, festgelegten Upstream-Ausgangsstand, vorhandene nicht committete Änderungen, ausgewählte [Komponenten](components/README.md), relevante Build-Funktionen und ausgeschlossene Bereiche fest. Submodul-Gitlinks bezeichnen getrennte Revisionen; behaupte nicht, ihre Inhalte seien geprüft worden, nur weil das Haupt-Repository einen Stand festhält.

## Empfohlener Ablauf

1. Benenne die Prüffrage und beabsichtigte Invariante; ermittle vom Aufrufer kontrollierte Eingaben, Schutzgüter, Berechtigungen und Vertrauensgrenzen.
2. Lies öffentliche Einstiegspunkte, konkrete Implementierung, Aufrufer, Fehlerpfade und relevante Testdefinitionen.
3. Halte anwendbare Prüfmodule und einen begrenzten Validierungsplan fest. Prüfe tatsächliche Werkzeuge und Voraussetzungen, bevor du Ausführung zusagst.
4. Untersuche Kandidatenpfade einschließlich zulässiger Kontrollfälle und der stärksten Gegenbelege.
5. Nutze die aussagekräftigste geeignete Validierung, die innerhalb der Aufgabe verfügbar ist: gezielte vorhandene Tests, begrenzte temporäre Testgerüste oder ausdrücklich ausgewählte Diagnosen.
6. Gleiche Kandidatenstatus, Abdeckung, Konfigurationsvarianten und nicht ausgeführte Prüfungen ab. Halte vorgeschlagene Härtung von implementierten oder validierten Korrekturen getrennt.
7. Halte Befehle, Ergebnisse, Grenzen, zur Aufgabe gehörende Artefakte und abschließende Quelländerungen fest; übergib die verbleibende konkrete Arbeit.

## Kandidatenaufzeichnung

| Feld | Erforderliche Bedeutung für einen hilfreichen Kandidaten |
| --- | --- |
| Identität | Stabile lokale Kennung, Komponente, Quellstand, Datei und Symbol. |
| Kontrolle | Wer kontrolliert die Eingabe, mit welchen Berechtigungen und Voraussetzungen? |
| Pfad | Erreichbarer Einstiegspunkt, relevante Transformationen, erwartete Kontrolle und Zielstelle. |
| Folge | Beobachtbare Auswirkung und ihre Grenze; Schlussfolgerung von Ausführung unterscheiden. |
| Gegenbelege | Schutzprüfungen, Konfiguration, unerreichbare Zweige oder Aufruferverträge, die die Hypothese entkräften können. |
| Validierung | Tatsächliche Methode, zulässiger Kontrollfall, Ergebnis, Werkzeug- und Konfigurationsidentität sowie Nachweisort. |
| Einstufung | Kandidat, validiert, widerlegt oder ungeklärt, mit Begründung und Vertrauen in die Einstufung. |

Nutze die [Befundvalidierung](checks/finding-validation.md). Ein Scanner- oder KI-Ergebnis beginnt als Kandidat. Fasse gemeinsame Grundursachen zusammen und erhalte dabei unabhängig erreichbare Vorkommen, die getrennte Validierung oder Korrekturen benötigen.

## Bibliotheksspezifische Fragen

- Sind die Lebensdauern von Engine, Regelwerk, Transaktion, Variablen und Callbacks mit dem tatsächlichen Aufrufer vereinbar?
- Entsprechen die von Regeln geprüften Bytes und Längen den von diesem Aufrufer gelieferten Eingaben?
- Welcher Engine-Zustand, welche Körperzugriffseinstellung, MIME-Auswahl, Grenzwertaktion und welcher optionale Prozessor gelten?
- Ist ein Ergebnis eine Methodenrückgabe, ein Regeltreffer, eine Intervention, ein Callback oder ein dauerhaft gespeicherter Audit-Datensatz?
- Welches gemeinsame Backend, welcher Dateisystemort, Parser-Pfad und welche Abhängigkeitsvariante sind für die ausgewählte Eingabe erreichbar?

Dies sind Prüffragen, keine Fehlerbehauptungen. Die [Architektur](../architecture.md) und der [C++-Leitfaden](../cpp-guide.md) bieten mit Quellen verknüpfte Einstiegspunkte.

## Nachweise und Abschluss

Ein knapper Bericht kann Umfang und Inventar, Abdeckungstabelle, Kandidaten, Befehlsprotokoll, Grenzen und vorgeschlagene nächste Schritte enthalten. Teile Dateien nur auf, wenn es hilfreich ist; erstelle keine leeren Berichtssammlungen und setze keine konfigurierten Variablen für externen Speicher voraus.

Nutze tatsächliche, zur Aufgabe gehörende Ausgabeorte und dokumentiere die Bereinigung. Halte sensible Nutzdaten und Zugangsdaten aus versioniertem Kontext heraus. Fehlende Werkzeuge sind eine Abdeckungslücke, kein Beweis für Produktversagen oder Sicherheit.

Vollständig bedeutet, dass der erklärte Prüfumfang abgeglichen wurde; es bedeutet nicht, dass jede Prüfung bestanden wurde oder jedes mögliche Problem ausgeschlossen ist. Benenne ausdrücklich, dass die Bewertung keine Produktionsfreigabe darstellt.
