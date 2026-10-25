# Fragen zur Umgehung von Regeln und Kontrollen

Verwenden, wenn die Aufgabe fragt, ob eine bestimmte beabsichtigte Kontrolle umgangen werden kann. Definiere zuerst diese Kontrolle, wirksame Regeln, Engine-Zustand und vertrauenswürdige beziehungsweise vom Aufrufer kontrollierte Eingaben.

- Stellen Variablen und Transformationen die Bytes dar, die für die beabsichtigte Entscheidung relevant sind?
- Verändern Kettenverhalten, Negation, Ausschlüsse, `ctl`, Regelentfernung oder Phasenauswahl die erreichbare Kontrolle?
- Was geschieht bei leerer, aufgeteilter oder gekürzter Eingabe, Parserfehlern, Körpergrenzen und optionalen Prozessoren?
- Unterscheidet die Erwartung das Verhalten von DetectionOnly von der Ausführung unterbrechender Aktionen?
- Kann eine erfolgreiche Methodenrückgabe mit einer abgeschlossenen Phase oder einer Zulassungsentscheidung verwechselt werden?

Konfigurierte Ausschlüsse, reiner Erkennungsbetrieb oder falsche Nutzung durch den Aufrufer sind nicht automatisch Bibliothekssicherheitslücken. Kläre Erreichbarkeit, beabsichtigte Richtlinie, Gegenbelege und einen zulässigen Kontrollfall, bevor du eine Umgehung behauptest.

Halte Regelauswertung, Treffer, Interventionsanforderung, Aufruferhandlung und Client-Ergebnis getrennt. Einstiegspunkte: [Engine](../components/engine.md), [Regeln und Parser](../components/rules-parser.md), [Verarbeitung der Nachrichtenkörper](../components/body-processors.md) und [Befundvalidierung](finding-validation.md).
