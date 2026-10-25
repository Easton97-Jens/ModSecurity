# Optionaler Vergleich der Darstellung zwischen Aufrufer und Bibliothek

Verwenden, wenn die ausgewählte Aufgabe einen tatsächlichen Aufrufer oder Connector einbezieht und fragt, ob die Bibliothek die relevanten Anwendungsdaten untersucht. Eine reine Prüfung des Kernquellcodes belegt weder das Parsen des Übertragungsformats noch Verhalten bei Request Smuggling.

- Welche Aufruferrevision parst, dekodiert, kombiniert, kürzt oder transformiert Daten vor API-Aufrufen?
- Welche genauen Bytes, Längen, Header und Transaktionsgrenzen erreichen libmodsecurity?
- Nutzt die tatsächliche nachgelagerte Anwendung dieselbe Darstellung und Anfrageidentität?
- Welche beabsichtigte Kontrolle ändert sich bei einer vermuteten Abweichung, und welcher zulässige Kontrollfall bleibt gültig?
- Welche Beobachtung gilt nur für die Bibliothek, und welche hat zugeordnete Nachweise vom Aufrufer und nachgelagerten System?

Übertragungseinrahmung, doppelte Längenheader, Trailer, HTTP/2- oder HTTP/3-Übersetzung, Wiederverwendung von Verbindungen und Stream-Resets sind nur relevant, wenn eine tatsächliche Integration innerhalb des Umfangs diese Pfade bereitstellt. Behaupte keine Abdeckung aufgrund einer HTTP-Versionszeichenfolge oder eines Kern-API-Tests.

Einstiegspunkte: [API-Grenze](../components/api-boundary.md), [Normalisierung](http-normalization.md) und [Protokollgrenzen](../../protocol-and-hardening.md).
