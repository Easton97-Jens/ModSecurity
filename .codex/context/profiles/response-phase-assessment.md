# Bewertung von Antwortphasen und Interventionen im Kern

## Anlass

Nutze dieses Profil für eine angefragte Prüfung oder Änderung an Antwortheadern, Antwortkörpern, Phasenzuordnung, Engine- und Körpereinstellungen, Interventionsabfrage oder zugehöriger Protokollierung. Bewertet wird der aktuelle Code; ein Connector-Migrationsziel wird nicht vorausgesetzt.

## Quellcode-Ausgangsstand

SecRules `phase:3` wird `ResponseHeadersPhase` und `phase:4` wird `ResponseBodyPhase` zugeordnet; ihre internen Enum-Werte sind `4` und `5`. Quellen: [Zuordnung](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/actions/phase.cc#L25-L72), [Enum](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/modsecurity.h#L97-L170).

`processResponseHeaders()` verhindert die Phasenauswertung bei deaktivierter Engine. Die Verarbeitung des Antwortkörpers hat Bedingungen für Engine, Körperzugriff, MIME und Grenzwerte. Auch `processLogging()` kehrt bei deaktivierter Engine vorzeitig zurück. Quellen: [Header](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L992-L1097), [Nachrichtenkörper](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L1117-L1230), [Protokollierung](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L1304-L1346).

## Ausgewählte Prüfung

Lies [Verarbeitung der Nachrichtenkörper](../security/components/body-processors.md), [API-Grenze](../security/components/api-boundary.md), [Antwortintervention](../security/checks/response-intervention.md) und [Protokollgrenzen](../protocol-and-hardening.md).

Halte tatsächliche Einstellungen, übergebene Daten, Phasenmethode, erreichten Zweig, Regelergebnis, Intervention und Freigabe fest. Wähle anhand des konkreten Codes und Testumfangs anwendbare Normalfälle sowie Fälle mit deaktivierter Engine beziehungsweise reinem Erkennungsmodus, MIME- oder Körperzugriffsbedingungen, Teilverarbeitung beziehungsweise Grenzwerten, leeren Eingaben und Fehlern.

Ordne für jede Aussage die Nachweise als aktuellen Quellcode, ausgeführtes Kernergebnis, vorgeschlagenes Verhalten oder externe Integrationslücke ein. Behaupte ohne gesondert abgegrenzte Nachweise weder clientseitig sichtbares Blockieren, Abbruch nach bereits festgeschriebener Antwort, Reset noch die Connector-Semantik `off`/`safe`/`strict`.

## Ergebnis

Eine Tabelle mit Bedingung, Quellcodezweig, erwarteter Beobachtung, tatsächlicher Prüfung und Ergebnis sowie Grenze. Ein erfolgreicher Aufruf zum Anhängen oder Verarbeiten allein ersetzt keine Interventionsprüfung.
