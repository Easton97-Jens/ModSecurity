# Prüfung von Regeln und Parser

## Einstiegspunkte im Quellcode

`RulesSet::loadFromUri()` verwendet `Parser::Driver` und führt geparste Regeln und Eigenschaften zusammen. `Driver::addSecRule()` ordnet Regeln zu und prüft Ketten- und Kennungsbedingungen. Quellen: [Laden](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/rules_set.cc#L50-L109), [Parsen im Treiber](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/driver.cc#L128-L198), [Regelerzeugung](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/driver.cc#L76-L124).

Lies die relevante `.ll`/`.yy`-Eingabe und konkrete Aktions- oder Operatorimplementierung. Das Parser-Makefile benennt generierte Dateien; ein Token- oder Klassenname allein belegt kein akzeptiertes und implementiertes Verhalten. Quellen: [Parser-Build](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/Makefile.am#L1-L43), [Beispiel ausdrücklich nicht unterstützter Grammatik](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/seclang-parser.yy#L2726-L2729).

## Prüffragen

- Wer kann in diesem Szenario Regeltext, Include-Pfad, entfernte Quelle oder Konfigurationswert bereitstellen?
- Führt die Tokenerkennung zur Ablehnung, einer gespeicherten Eigenschaft, Regelerzeugung oder einer leeren Aktion?
- Werden Anführungszeichen, Maskierungen, Makros, Längen, Zahlenbereiche, doppelte Kennungen und Kettenbedingungen konsistent behandelt?
- Welcher Zustand bleibt nach fehlgeschlagenem Laden oder Zusammenführen erhalten, und was macht der Aufrufer mit der Rückgabe beziehungsweise dem Fehlertext?
- Erreicht eine akzeptierte Einstellung den Laufzeitzweig, dessen Verhalten behauptet wird?

Unterscheide SecRules-Nummern von internen Enum-Werten: `phase:1` wird `RequestHeadersPhase` mit dem internen Wert `2` zugeordnet. `processURI()` befüllt Daten, ohne `UriPhase` auszuwerten. Quellen: [Phasenaktion](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/actions/phase.cc#L25-L72), [Enum](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/modsecurity.h#L97-L170), [URI-Verarbeitung](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L337-L443).

## Relevante Prüfungen auswählen

[Konfigurations- und Eingabeinjektion](../checks/config-injection.md), [Speicherlebenszyklus](../checks/memory-lifecycle.md), [Ressourcengrenzen](../checks/resource-exhaustion.md), [Fragen zur Umgehung von Regeln und Kontrollen](../checks/modsecurity-bypass.md), [Datei- und Speichergrenzen](../checks/filesystem-storage.md) und [Befundvalidierung](../checks/finding-validation.md).

Nutze passende Fälle für Akzeptanz und Ablehnung und prüfe deren tatsächliche Ausführung. Erhalte die Unterscheidung zwischen den Berechtigungen eines Konfigurationsautors und den Einflussmöglichkeiten eines nicht authentifizierten HTTP-Clients.
