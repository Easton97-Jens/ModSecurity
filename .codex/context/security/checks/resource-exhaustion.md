# Fragen zu Ressourcengrenzen und Wiederherstellung

Inventarisiere Ressourcen, die die ausgewählte Eingabe tatsächlich erreicht: gepufferte Nachrichtenkörper, Header- und Argumentwerte, Parser-Verschachtelung, Regex- und Transformationsarbeit, Regelanzahlen, Kollektionseinträge, Allokationen, Dateien oder Protokollausgabe. Beziehe eine Ressource nur ein, wenn der Quellpfad sie belegt.

Frage für jede Ressource, wem sie gehört, wer ihre Größe beziehungsweise Arbeitsmenge beeinflusst, welche Konfiguration oder Implementierung sie begrenzt, wo die Erfassung erfolgt, was an der Grenze geschieht und wie sie freigegeben wird.

Erhält Teilverarbeitung die beabsichtigte Regelsemantik? Lässt ein abgelehnter oder fehlgeschlagener Fall eine nutzbare zulässige Folgetransaktion zu? Ist die vermutete Verstärkung unter dem tatsächlichen Aufrufer und Build erreichbar?

Nutze bei ausgewählter Ausführung begrenzte lokale Fälle mit erklärten Limits. Verwandle eine Prüfung nicht in unkontrollierte Last und leite ohne Auswirkungspfad aus einer isolierten Allokation keine dienstweite Dienstverweigerung ab.

Einstiegspunkte: [Verarbeitung der Nachrichtenkörper](../components/body-processors.md), [Regeln und Parser](../components/rules-parser.md), [Protokollierung und Speicherung](../components/logging-storage.md) und [Befundvalidierung](finding-validation.md).
