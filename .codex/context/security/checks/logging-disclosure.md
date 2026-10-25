# Fragen zu Protokollierung und Offenlegung

Wähle den tatsächlichen Pfad für Callback, Debug-, Audit- oder Fehlerausgabe, temporäres Artefakt oder Bericht. Ermittle vor dem Sammeln von Nachweisen, welche Felder extern kontrollierte oder sensible Werte enthalten können.

- Können Nachrichtenkörper, Zugangsdaten, Cookies, Kennungen, Regelinhalte, interne Pfade oder Werte früherer Transaktionen diese Ausgabe erreichen?
- Sind Maskierung und Serialisierung für das Ausgabeformat korrekt, einschließlich Zeilenumbrüchen und Steuerzeichen?
- Welche Konfiguration, Audit-Relevanzentscheidung, welcher Schreiber, welche Callback-Eigenschaft und Berechtigungen gelten?
- Kann ein Aufrufer Daten über die Lebensdauer eines Callbacks hinaus aufbewahren oder Transaktionskontexte vermischen?
- Wie beeinflussen ein fehlgeschlagener Schreibvorgang oder ein nicht verfügbares Ziel das beobachtete Ergebnis?

Bewahre bereinigte, begrenzte Nachweise auf, soweit möglich anhand von Längen, Kategorien und minimalen Auszügen. Ein Callback-Ereignis, eine Audit-Entscheidung und ein dauerhaft gespeicherter Datensatz sind unterschiedliche Beobachtungen.

Einstiegspunkte: [Protokollierung und Speicherung](../components/logging-storage.md), [Datei- und Speichergrenzen](filesystem-storage.md) und [Speicherlebenszyklus](memory-lifecycle.md).
