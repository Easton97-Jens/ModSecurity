# Prüfung von Engine und Regelausführung

## Einstiegspunkte im Quellcode

Lies `src/rules_set.cc`, `src/rule_with_operator.cc`, `src/rule_with_actions.cc` und die relevanten Transaktionsphasenmethoden. `RulesSet::evaluate()` wählt eine Phasensammlung und behandelt Kontrollfluss wie Überspringen, Zulassen und unterbrechende Interventionen; unterbrechende Aktionen in `executeAction()` hängen von `EnabledRuleEngine` ab. Quellen: [Phasenschleife](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/rules_set.cc#L121-L232), [Regelauswertung](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/rule_with_operator.cc#L199-L353), [Aktionsbedingung](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/rule_with_actions.cc#L299-L324).

## Prüffragen

- Welche Variablenwerte, Ausschlüsse, Transformationen, Operatoren und Kettenglieder kann die ausgewählte Eingabe erreichen?
- Was geschieht vor einem vollständigen Kettentreffer, nach einem Treffer, nach einer fehlgeschlagenen Bedingung oder nach einer Intervention?
- Welcher Engine-Zustand gilt, und unterscheidet die vorgeschlagene Erwartung Treffer, Aktion und Intervention?
- Sind transaktionsbezogene Variablen und gemeinsamer Kollektionszustand entlang des tatsächlichen Pfads getrennt?
- Erhält eine Ausnahme, vorzeitige Rückgabe, übersprungene Regel oder ein Grenzwert den dokumentierten Zustand und einen zulässigen Folgefall?

Leite Regelausführung nicht aus der erfolgreichen Rückgabe einer Phasenmethode ab. `processRequestHeaders()` kehrt bei deaktivierter Engine erfolgreich zurück; auch Antwortheader und Protokollierung haben Schutzprüfungen für eine deaktivierte Engine. Quellen: [Anfrageheader](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L461-L471), [Antwortheader](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L992-L1097), [Protokollierung](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L1304-L1346).

## Relevante Prüfungen auswählen

[Fragen zur Umgehung von Regeln und Kontrollen](../checks/modsecurity-bypass.md), [API-Aufrufreihenfolge](../checks/api-sequencing.md), [Speicherlebenszyklus](../checks/memory-lifecycle.md), [Nebenläufigkeit](../checks/concurrency-races.md), [Ressourcengrenzen](../checks/resource-exhaustion.md) und [Befundvalidierung](../checks/finding-validation.md).

Halte die genauen Regeln, Engine-Zustand, Phase, Eingabe, erwartete Kontrolle, beobachtetes Ergebnis und Grenzen fest. Ein Ergebnis der Kernbibliothek beweist keine Durchsetzung durch den Connector.
