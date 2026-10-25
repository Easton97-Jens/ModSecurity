# Richtlinie für optionale Sicherheitsprüfungen

## Wann diese Unterlagen gelten

Dies ist eine Arbeitskonvention für eine ausdrücklich angeforderte Sicherheitsbewertung oder für begrenzte Sicherheitsfragen, die für eine freigegebene Änderung relevant sind. Sie verlangt nicht für jede Aufgabe einen Sicherheitsscan, setzt keinen installierten Scanner voraus und dokumentiert keine abgeschlossene Bewertung. Gewöhnliche Dokumentationsarbeit aktiviert nicht den vollständigen Sicherheitsablauf.

Wähle ein [Profil](profiles/README.md), ermittle den tatsächlich betroffenen Code und die Konfiguration und nutze nur die relevanten [Komponentenleitfäden](security/components/README.md) und Prüfmodule. Ein Bewertungsauftrag erlaubt für sich genommen weder Produktkorrekturen noch Aktualisierungen von Abhängigkeiten, Bereitstellungen oder Änderungen an externen Repositories. Ein bestehender Implementierungsauftrag behält seinen eigenen freigegebenen Umfang.

## Die Frage vor der Schlussfolgerung klären

Halte die beabsichtigte Invariante, die von einem Aufrufer oder Angreifer kontrollierte Eingabe, die dafür erforderlichen Berechtigungen, das betroffene Schutzgut, die Vertrauensgrenze und die beobachtbare Folge fest. Lies die Implementierung und ihre Aufrufer, bevor du entscheidest, ob eine Eingabe nicht vertrauenswürdig ist oder eine Kontrolle fehlt. Ein verdächtiges Muster, ein Sicherheitshinweis oder eine Scannermeldung ist ein Kandidat, keine bestätigte Sicherheitslücke.

Vergleiche für ausgewähltes Verhalten einen relevanten Negativfall mit einem zulässigen Kontrollfall. Trenne Quellcodebegründung, ausgeführte Ergebnisse, Grenzen und vorgeschlagene Härtung. Nutze die [Befundvalidierung](security/checks/finding-validation.md) für einen Kandidaten, der eine vertiefte Untersuchung rechtfertigt.

## Umfang der Bibliothek

libmodsecurity nimmt Daten über seine öffentliche API entgegen; Webserver-Connectoren sind eigenständige Projekte. Regeltreffer in der Bibliothek, eine Interventionsanforderung, die Durchsetzung durch den Connector und ein clientseitig sichtbares Ergebnis sind unterschiedliche Nachweisgrenzen. Quellen: [Upstream-Umfang](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/README.md#L21-L46), [Transaktions-API](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/transaction.h#L335-L419), [Implementierung der Intervention](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L1350-L1389).

Übertrage keine connector-spezifischen Host-Routen, Transportmodi, privaten IPC-Protokolle, Freigabekriterien für Releases oder Migrationsziele für Phase 4 auf das aktuelle Verhalten der Kernbibliothek. [Protokollgrenzen und Härtung](protocol-and-hardening.md) erläutert die Integrationsgrenze.

## Umgang mit Nachweisen und Ergebnissen

Ordne jede Beobachtung dem Quellstand, der Komponente, der wirksamen Konfiguration, optionalen Funktionen, der Werkzeugkette und dem tatsächlichen Befehl oder Quellpfad zu. Halte nicht ausgeführte Prüfungen als `not_run` und nicht verfügbare Voraussetzungen als `blocked` fest, jeweils mit Begründung. Eine fehlgeschlagene Voraussetzung ist kein ausgeführter Fehler des eigentlichen Tests. Verwende `not_applicable` nur mit einer technischen Begründung zum Umfang.

Bewahre begrenzte, bereinigte Nachweise auf. Versioniere keine Zugangsdaten, privaten Schlüssel, sensiblen Anfragekörper, ungefilterten Umgebungen oder privaten Kundenprotokolle. Benenne bei einem bestätigten Problem Auswirkungen, Erreichbarkeit, Gegenbelege, Vertrauen in den Befund und verbleibende Grenzen; erfinde weder Schweregrad, CVE, Exploit noch eine allgemeine Sicherheitsgarantie.

Schließe mit der tatsächlich erreichten Prüfungsabdeckung und offener Arbeit ab. Weder eine allgemeine Punktzahl noch eine abgeschlossene Prüfung oder eine unauffällige Scannerausgabe stellt eine Produktionsfreigabe dar.
