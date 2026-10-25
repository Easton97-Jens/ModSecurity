# Fragen zu Antwortprüfung und Intervention

Halte diese Beobachtungen getrennt: Antwortdaten geliefert; Phasenmethode erreicht; Regel ausgewertet; Regel getroffen; Intervention angefordert; Aufruferhandlung; tatsächliches externes HTTP-Ergebnis; Freigabe- und Protokollergebnis.

- Welcher Engine-Zustand, welcher Antwortkörperzugriff, welche MIME-Auswahl und welche Aktion bei Größenüberschreitung gelten?
- Wurde die Körperphase erreicht, oder trat eine dokumentierte vorzeitige Rückgabe ein?
- Prüft der Aufrufer Interventionen unabhängig von Rückgabewerten der Aufrufe zum Anhängen und Verarbeiten?
- Werden kopierte Interventionspuffer gemäß der tatsächlichen API initialisiert, genutzt und freigegeben?
- Falls Durchsetzung behauptet wird: Welche gesondert abgegrenzten Aufrufer- oder Host-Nachweise belegen sie?

Die SecRules-Antwortkörperphase `phase:4` wird `ResponseBodyPhase` zugeordnet, deren interner Enum-Wert `5` ist. Quellen: [Phasenzuordnung](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/actions/phase.cc#L25-L72), [Enum](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/modsecurity.h#L97-L170). Nutze die [Engine- und Regelleitfäden](../components/rules-parser.md), um Schutzprüfungen für eine deaktivierte Engine ebenso wie die normale Ausführung zu untersuchen.

Hier werden weder eine Connector-Zielrichtlinie `off`/`safe`/`strict` noch Abbruch nach bereits festgeschriebener Antwort, Reset oder Transportfähigkeit abgeleitet. Einstiegspunkte: [Verarbeitung der Nachrichtenkörper](../components/body-processors.md), [API-Grenze](../components/api-boundary.md), [Profil für Antwortphasen](../../profiles/response-phase-assessment.md) und [Protokollgrenzen](../../protocol-and-hardening.md).
