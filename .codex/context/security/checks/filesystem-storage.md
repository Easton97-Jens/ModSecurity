# Fragen zu Datei-, Pfad- und Speichergrenzen

Nur für tatsächliche Datei- und Speicheroperationen verwenden, die der ausgewählte Kernpfad erreicht, etwa Laden von Regeln, Audit-Ausgabe, Kollektionsspeicherung oder Umgang mit temporären Dateien. Lies vor der Fallauswahl die konkrete Implementierung und ihren konfigurierten Pfad.

- Wer liefert den Pfad, relativ zu welchem Verzeichnis wird er aufgelöst und mit wessen Berechtigungen wird die Operation ausgeführt?
- Werden Erstellen, Öffnen, Lesen, Schreiben, Umbenennen, Löschen und Freigabe auf das beabsichtigte Objekt angewendet?
- Welche Prüfungen für Pfadbegrenzung, Berechtigungen, symbolische Verknüpfungen beziehungsweise Austausch, Längen und Fehler bestehen am Verwendungsort?
- Ist die Datenmenge begrenzt, und was geschieht bei unvollständigem Lesen oder Schreiben beziehungsweise nicht verfügbarem oder vollem Speicher?
- Werden temporäre Ressourcen freigegeben, ohne Dateien einer anderen Aufgabe oder eines anderen Aufrufers zu löschen?

Eine erkannte Konfigurationsdirektive beweist nicht, dass eine Pfadeinstellung angewendet wird. Übernimm keine Annahmen zu privaten UDS oder Gegenstellen-Berechtigungsnachweisen aus Connector-Diensten in einen Bibliothekspfad, der diesen Dienst nicht nutzt.

Halte für jeden ausgeführten Test tatsächliche Dateiidentität, relevante Metadaten, begrenzte Nachweise und Bereinigung fest. Einstiegspunkte: [Regeln und Parser](../components/rules-parser.md), [Verarbeitung der Nachrichtenkörper](../components/body-processors.md) und [Protokollierung und Speicherung](../components/logging-storage.md).
