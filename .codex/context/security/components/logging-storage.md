# Prüfung von Protokollierung und Speicherung

## Einstiegspunkte im Quellcode

`ModSecurity::serverLog()` liefert entweder Callback-Text oder eine `RuleMessage`. Der Textpfad nutzt während des Callbacks `c_str()` einer lokalen Zeichenfolge. `processLogging()` kehrt bei deaktivierter Engine vorzeitig zurück; andernfalls wertet es die Protokollierungsphase aus und prüft die Audit-Speicherung über `saveIfRelevant()`. Quellen: [Callback](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/modsecurity.cc#L193-L212), [Protokollierung](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L1304-L1346), [Audit-Entscheidung](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/audit_log/audit_log.cc#L278-L325).

Die Engine erzeugt unter `WITH_LMDB` gemeinsame Kollektions-Backends mit LMDB, andernfalls `InMemoryPerProcess`; Transaktionskollektionen erzeugen ein getrenntes speicherinternes `TX`-Backend. Quellen: [Engine-Backends](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/modsecurity.cc#L62-L109), [Transaktionskollektionen](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/collection/collections.cc#L37-L56).

## Prüffragen

- Welche Felder können sensiblen oder vom Angreifer kontrollierten Text enthalten, und welcher Maskierungs- beziehungsweise Serialisierungspfad schreibt sie?
- Welche Protokollierungs- und Audit-Einstellungen, Callback-Eigenschaften und Backend-Funktionsmakros sind wirksam?
- Kopiert der Callback Daten, die nach seiner Rückkehr benötigt werden, und überdauert sein Nutzerkontext jeden Aufruf?
- Wem gehören die an der ausgewählten Operation beteiligten Kollektionen, Schreiber, Pfade, Dateien und Synchronisationsmittel?
- Was geschieht bei einem fehlenden Verzeichnis, fehlgeschlagenem Schreibvorgang, vollem Speicher, fehlerhaftem Wert oder Freigabepfad?
- Belegt der Nachweis einen Callback, eine Audit-Auswahlentscheidung oder einen tatsächlich dauerhaft gespeicherten Datensatz?

Leite aus dem Vorhandensein eines Backends oder Callbacks weder allgemeine Persistenz noch Threadsicherheit oder erfolgreiche Audit-Ausgabe ab. Untersuche für diese Fragen den konkreten Schreiber beziehungsweise das Backend und den Aufrufer.

## Relevante Prüfungen auswählen

[Offenlegung](../checks/logging-disclosure.md), [Datei- und Speichergrenzen](../checks/filesystem-storage.md), [Nebenläufigkeit](../checks/concurrency-races.md), [Speicherlebenszyklus](../checks/memory-lifecycle.md), [Ressourcengrenzen](../checks/resource-exhaustion.md) und [Nachweisintegrität](../checks/evidence-integrity.md).
