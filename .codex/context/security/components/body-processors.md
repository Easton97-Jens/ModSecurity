# Prüfung der Verarbeitung von Nachrichtenkörpern

## Einstiegspunkte im Quellcode

`appendRequestBody()` puffert Bytes und wendet das Anfragelimit an; `processRequestBody()` übernimmt das Formatparsen und die spätere Entscheidung über Körperzugriff und Phase. Multipart- und URL-kodierte Typen werden bei der Header-Verarbeitung erkannt, während JSON/XML durch Prozessoraktionen ausgewählt werden. Quellen: [Anhängen](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L929-L971), [Körperverarbeitung](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L674-L871), [Header-Auswahl](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L576-L589), [JSON-Auswahl](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/actions/ctl/request_body_processor_json.cc#L28-L35), [XML-Auswahl](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/actions/ctl/request_body_processor_xml.cc#L28-L35).

JSON- und XML-Verarbeitung hängen in diesem Pfad von `WITH_YAJL` und `WITH_LIBXML2` ab. Die Auswertung des Antwortkörpers hat eigene Bedingungen für Zugriff, Grenzwerte und MIME. Quellen: [Anfragekörper](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L674-L871), [Antwortkörper](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L1117-L1230).

## Prüffragen

- Welche genauen Bytes und expliziten Längen erreichen jeden Aufruf zum Anhängen, und wie werden leere oder aufgeteilte Eingaben behandelt?
- Ist der Prozessor tatsächlich ausgewählt und einkompiliert? Welche Variablen und Fehler ergeben sich aus fehlerhaften Eingaben?
- Wann werden Engine-Zustand, Körperzugriff, Tiefe, Gesamtgröße oder Größe ohne Dateiinhalte geprüft?
- Verändert Teilverarbeitung, welche Bytes eine Regel sieht, und prüft der Aufrufer den Interventionszustand gesondert?
- Deckt der ausgewählte Fall Parser-Abschluss, Freigabe bei Fehlern, temporäre Dateien und eine folgende zulässige Transaktion ab?
- Beschränkt sich ein behauptetes Antwortergebnis auf untersuchte Daten, oder wird es durch einen gesondert abgegrenzten Connector-Test gestützt?

Die Rückgabe eines Aufrufs zum Anhängen allein reicht nicht: Ein Ablehnungszweig kann eine Intervention setzen und dabei `true` zurückgeben. Quelle: [Zweige beim Anhängen von Anfragedaten](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L929-L971).

## Relevante Prüfungen auswählen

[Normalisierung](../checks/http-normalization.md), [Ressourcengrenzen](../checks/resource-exhaustion.md), [Speicherlebenszyklus](../checks/memory-lifecycle.md), [Fragen zur Umgehung von Regeln und Kontrollen](../checks/modsecurity-bypass.md), [Antwortintervention](../checks/response-intervention.md), [Fehler und Wiederherstellung](../checks/runtime-failure-recovery.md) und [Datei- und Speichergrenzen](../checks/filesystem-storage.md).
