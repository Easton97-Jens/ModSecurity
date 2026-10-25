# Aufgabenablauf und Fortsetzung über Chats hinweg

Nutze gespeicherten Kontext für einen effizienten Wiedereinstieg und prüfe anschließend den
für die neue Aufgabe relevanten Code und Arbeitsstand. Die Dateien sind ein dauerhafter
Ausgangspunkt, keine Zusicherung, dass jeder neue Chat das Repository oder frühere Gespräche
automatisch gelesen hat.

## Von der tatsächlichen Aufgabe ausgehen

1. Lies die anwendbaren Anweisungen, den [Index](index.md),
   den [aktuellen Stand](current-state.md) und relevante Einträge in den
   [Entscheidungen](decisions.md).
2. Ermittle das beabsichtigte Nutzerergebnis, Vorgaben und frühere, weiterhin geltende Freigaben.
   Behandle neue Präzisierungen als Aktualisierungen der laufenden Aufgabe, sofern der Nutzer
   sie nicht ersetzt oder abbricht.
3. Prüfe Arbeits-Repository, Branch, Commit und vorhandene Änderungen. Unterscheide den
   Arbeitscommit vom Quellcode-Ausgangsstand in
   [upstream-lock.json](upstream-lock.json).
4. Wähle die relevanten technischen Kapitel und Richtlinien. Lies betroffenen Quellcode,
   Aufrufer, Build-Bedingungen und Tests, statt dich nur auf Zusammenfassungen zu verlassen.
5. Lege ein praktikables Abnahmekriterium und die nächste sinnvolle Handlung fest.

Nutze den [Projektkontext](project-context.md), um die Bibliothek kennenzulernen,
die [Repository-Übersicht](repository-map.md), um Code zu finden, und
[Architektur und Grenzen](architecture-and-boundaries.md), um die Zuständigkeit zu klären.
Nutze die [Machbarkeitsrichtlinie](feasibility-policy.md), wenn Voraussetzungen oder
externe Handlungen die Ausführung wesentlich beeinflussen.

Hat ein Chat keinen Checkout, nutze den tatsächlich verfügbaren Repository-Zugriff. Bitte nur
dann um eine bestimmte nicht zugängliche Datei, wenn sie benötigt wird; behaupte nicht,
nicht verfügbare Dateien gelesen zu haben oder dich an nicht verfügbare Gespräche zu erinnern.

## Dem Arbeitsumfang entsprechend planen

Für eine kleine reversible Änderung genügen eine kurze Aufgabenbeschreibung und eine gezielte Prüfung.
Halte für umfangreiche Arbeit einen knappen Plan mit den hilfreichen Einzelheiten fest:

- Ziel und Ergebnisse;
- Repository- und Dateizuständigkeit;
- wesentliche Vorgaben, Annahmen und offene Entscheidungen;
- Abnahmekriterien und Prüfung;
- Umsetzungsschritte und Bereitstellungsstand.

Verfolge jede wesentliche Anforderung, bis sie erfüllt, ausdrücklich ersetzt oder als blockiert
benannt ist. Eine Tabelle ist optional. Lasse keinen angefragten Teil stillschweigend aus und
weite keine Aufgabe auf sachfremden Code, Abhängigkeiten, Scans oder Integration aus.

## Fragen, Entscheidungen und Freigaben

Untersuche verfügbare Nachweise, bevor du fragst. Triff angemessene Umsetzungsentscheidungen,
wenn sie innerhalb des Umfangs liegen und reversibel sind. Frage, wenn eine offene Entscheidung
Verhalten, Kompatibilität, Zuständigkeit, Kosten, Zugriff oder eine destruktive Handlung wesentlich
ändert und das bisherige Gespräch sie nicht klärt.

Die Nutzerfreigabe gilt während der laufenden Aufgabe weiter. Eine neue Phase oder ein anderes
Werkzeug verlangt für sich genommen keine erneute Freigabe. Bereite konkrete, prüfbare Arbeit vor,
bevor du um eine tatsächlich fehlende abschließende Freigabe bittest. Ist ein Teil blockiert,
schließe sinnvolle unabhängige Teile ab und erkläre die genaue verbleibende Lücke.

## Arbeit und Prüfung

Wende den [Änderungsablauf](change-workflow.md) auf die Umsetzung und
[Dokumentation und Nachvollziehbarkeit](documentation-and-traceability.md) auf Erklärungen an.
Erhalte sachfremde Änderungen und weise bei Delegation überschneidungsfreie Dateien zu.
Verknüpfe delegierte Befunde mit Nachweisen und prüfe das zusammengeführte Ergebnis;
Delegation verifiziert für sich genommen keine Aussage.

Nutze Prüfungen, die das angefragte Ergebnis belegen. Die Einstiegspunkte der Kontextprüfung,
ausgeführt von der Repository-Wurzel, sind:

```sh
python3 .codex/bin/verify-context.py
python3 .codex/bin/validate-structure.py
```

Diese Prüfungen betreffen den gepflegten Kontext. Nutze für die Produktprüfung
[Build und Tests](build-and-test.md) sowie die tatsächlichen Build-Definitionen.
Beschreibe eine Strukturprüfung nicht als ausgeführten C++-Test und wiederhole keine
breiten Tests, nachdem die notwendigen Nachweise ausreichen.

## Mit einem fortsetzbaren Stand abschließen

Wende die [Abschlusskriterien](definition-of-done.md) an und gleiche wesentliche Anforderungen
mit dem tatsächlichen Ergebnis ab. Aktualisiere den [aktuellen Stand](current-state.md)
mit Arbeitsstand, tatsächlicher Prüfung und nächstem konkreten Schritt. Aktualisiere
[Entscheidungen](decisions.md) und [offene Aufgaben](backlog.md), wenn sich Entscheidungen
oder ausstehende Arbeit geändert haben.

Speichere für umfangreiche Arbeit eine knappe Aufzeichnung in den
[Sitzungsprotokollen](../audit/sessions/) mithilfe der
[Übergabevorlage](handoff-template.md). Nenne Quellstand, Dateien, aussagekräftige Prüfungen,
offene Fragen und tatsächliche Bereitstellung. Ordne historische Ergebnisse ihrer tatsächlichen
Revision zu und vermeide das Duplizieren vollständiger technischer Kapitel oder privater Gesprächsabschriften.

Schließe mit einem eigenständig verständlichen Bericht darüber ab, was erreicht wurde,
wie es geprüft wurde, was unsicher bleibt und wo die Arbeit steht. Beachte die
[Git-Richtlinie](git-policy.md) für freigegebene Bereitstellung und
[Upstream-Synchronisierung](upstream-sync.md), wenn die Aufgabe absichtlich den offiziellen
Quellcode-Ausgangsstand ändert.
