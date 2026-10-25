# Übersicht zur Prüfung der Kernkomponenten

Dies ist eine thematische Navigationsübersicht für optionale Prüfungen, keine Erklärung isolierter Upstream-Module oder abgeschlossener Abdeckung. Ein Datenpfad kann mehrere Zeilen durchlaufen; halte die gemeinsame Grenze fest, statt Ergebnisse zu duplizieren oder stillschweigend zu übertragen.

| Komponente | Hauptfragen |
| --- | --- |
| [Engine und Regelausführung](engine.md) | Phasenauswahl, Engine-Zustand, Regel- und Aktionskontrollfluss, Transaktionszustand. |
| [Regeln und Parser](rules-parser.md) | Eingabeakzeptanz, Eigenschaften, Regelerzeugung, Ketten und Phasenübersetzung. |
| [Verarbeitung der Nachrichtenkörper](body-processors.md) | Bytes, Formatauswahl, Pufferung, Grenzen, Fehler, optionale Prozessoren. |
| [Protokollierung und Speicherung](logging-storage.md) | Callback-Lebensdauer, Audit-Entscheidungen, Kollektionen, Pfade und Offenlegung. |
| [C++-/C-API-Grenze](api-boundary.md) | Eigentümerschaft, explizite Längen, Rückgaben, Freigabe, Callbacks und Aufruferverträge. |
| [Build und Abhängigkeiten](build-dependencies.md) | Funktionsidentität, generierte Quellen, Gitlinks, Tests, CI und Herkunft. |

Beginne mit dem [ausgewählten Profil](../../profiles/README.md). Der [gemeinsame Bewertungsablauf](../core-deep-assessment.md) gilt nur, wenn eine vertiefte Prüfung angefragt ist. Jede Komponente nennt vorgeschlagene Prüfmodule; wähle diejenigen, die zur tatsächlichen Frage passen.

Externe Connectoren, ihre host-spezifischen Routen, Übertragungsprotokolle und externe Regelwerke haben getrennte Quellen- und Nachweisgrenzen. Beziehe sie nur ein, wenn die Aufgabe diese Grenzen ausdrücklich erreicht.
