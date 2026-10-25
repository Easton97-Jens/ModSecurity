# Fragen zur HTTP-Darstellung und Normalisierung

Beginne mit den tatsächlich an die Bibliothek gelieferten Bytes und Längen. Verfolge im ausgewählten Pfad rohe und dekodierte URI- beziehungsweise Abfragewerte, Header-Namen und -Werte, Körperwerte, Transformationen und gemeldete Trefferursprünge.

- Welche Darstellung erhält jede Variable oder jeder Operator?
- Was geschieht im anwendbaren Pfad mit Prozentkodierung, wiederholter Dekodierung, Trennzeichen, doppelten Argumenten, Steuerzeichen, eingebetteten NULs und fehlerhaftem Text?
- Bleiben explizite Längen, Versätze und abgeleitete Werte konsistent, nachdem eine Transformation die Daten verändert hat?
- Ist ein Parserfehler oder ein nur teilweise verarbeiteter Körper in der vom Fall verwendeten Konfiguration beziehungsweise den Regeln sichtbar?
- Nutzt die nachgelagerte Anwendung dieselbe Darstellung, oder ist das eine ungeprüfte externe Grenze?

Ein Unterschied ist nur dann relevant, wenn seine erreichbare Folge nachgewiesen ist. Halte Eingabedarstellung, abgeleiteten Wert, verantwortliche Funktion, wirksame Regel, erwartetes und beobachtetes Ergebnis, zulässigen Kontrollfall und verbleibende Unsicherheit fest.

Einstiegspunkte: [API-Grenze](../components/api-boundary.md), [Verarbeitung der Nachrichtenkörper](../components/body-processors.md), [Regeln und Parser](../components/rules-parser.md) und [Datendarstellung an Integrationsgrenzen](integration-representation.md).
