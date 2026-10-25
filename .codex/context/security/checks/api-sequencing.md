# API-Aufrufreihenfolge und Transaktionszustand

Für den tatsächlichen C- oder C++-Aufrufer beziehungsweise das Testgerüst verwenden. Lies dessen Einrichtung, Phasenaufrufe, Interventionsbehandlung, Protokollierung und Freigabe gemeinsam mit der Implementierung.

- Wann werden Verbindung, URI, Header und Körperabschnitte im Verhältnis zur Phasenauswertung geliefert?
- Welche vorzeitigen Rückgaben und Bedingungen für Engine und Körperzugriff gelten?
- Unterscheidet der Aufrufer unvollständige Eingaben, Grenzen beim Anhängen, Verarbeitungsergebnisse und Interventionszustand?
- Sind wiederholte Aufrufe, fehlende Schritte, Fehlerpfade und Freigabe durch den dokumentierten beziehungsweise implementierten Vertrag unterstützt oder lediglich Annahmen des Testgerüsts?
- Werden transaktionsbezogene Kennungen und Nutzerkontexte von Callbacks über die tatsächlichen Aufruferlebensdauern hinweg getrennt gehalten?

Halte die genaue Reihenfolge und den an jedem relevanten Schritt beobachteten Zustand fest. Ein Robustheitstest einer nicht unterstützten Aufrufreihenfolge darf nicht als gewöhnliches unterstütztes Verhalten berichtet werden.

Übernimm weder SPOE, gRPC, UDS-Frames, begleitende Antwortkomponenten noch den Lebenszyklus von Host-Streams in diese Bibliotheksprüfliste. Sie benötigen eigene Implementierungen und einen eigenen Umfang.

Einstiegspunkte: [API-Grenze](../components/api-boundary.md), [Engine](../components/engine.md) und [Architektur](../../architecture.md).
