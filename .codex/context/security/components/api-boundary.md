# Prüfung der C++-/C-API-Grenze

## Einstiegspunkte im Quellcode

Lies die öffentlichen Header gemeinsam mit den Wrappern und C++-Methodenimplementierungen. `Transaction` speichert Engine- und Regelwerkzeiger, ohne diese beiden Objekte in seinem Destruktor zu löschen; ihre Lebensdauer muss ihre Nutzung abdecken. Quellen: [öffentliche Transaktions-API](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/transaction.h#L335-L419), [C-Deklarationen](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/transaction.h#L677-L767), [Konstruktor und Destruktor](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L113-L182).

Überladungen für Header ohne Längenangaben nutzen `strlen()`, während Überladungen mit expliziten Längen die gelieferten Längen an Zeichenfolgenoperationen übergeben. Der Callback-Text liegt während des Aufrufs in einer lokalen Zeichenfolge. Quellen: [Header-Überladungen](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L618-L654), [Callback](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/modsecurity.cc#L193-L212).

## Prüffragen

- Welche Zeiger sind ausgeliehen, übertragen, neu allokiert oder Verweise in den Speicher eines anderen Objekts?
- Beschreiben Längen gültige zugängliche Puffer, und verlangt diese konkrete Überladung eine Nullterminierung?
- Beeinflusst eine Änderung öffentlicher Header Aufrufer, Klassenlayout, ABI, Freigabeverantwortung oder C-/C++-Verhalten?
- Unterscheidet der Aufrufer Ladeanzahlen, Methodenerfolg, Parserfehler und Interventionszustand?
- Sind Callback-Eigenschaften, Typ des Nutzerkontexts, Lebensdauer und eventuell aufbewahrte Daten vereinbar?
- Welche Fehler oder Ausnahmen überschreiten den tatsächlichen Wrapper, und wo stellt der Aufrufer einen brauchbaren Zustand wieder her?

Die C-Konstruktor-Wrapper verwenden `new`; setze keine allgemeine Übersetzung von Ausnahmen in Fehlercodes voraus. Interventionen kopieren Puffer und setzen internen Zustand zurück; nutze den entsprechenden Freigabepfad. Quellen: [Engine-Wrapper](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/modsecurity.cc#L411-L482), [Transaktions-Wrapper](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L1804-L1829), [Intervention](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L1350-L1389), [Freigabe](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L2153-L2195).

## Relevante Prüfungen auswählen

[Speicherlebenszyklus](../checks/memory-lifecycle.md), [API-Aufrufreihenfolge](../checks/api-sequencing.md), [Normalisierung](../checks/http-normalization.md), [Datendarstellung an Integrationsgrenzen](../checks/integration-representation.md), [Fehler und Wiederherstellung](../checks/runtime-failure-recovery.md) und [Befundvalidierung](../checks/finding-validation.md).

Eine Verletzung des Aufrufervertrags und ein erreichbarer Bibliotheksfehler sind unterschiedliche Hypothesen. Kläre, welche der tatsächliche Fall stützt.
