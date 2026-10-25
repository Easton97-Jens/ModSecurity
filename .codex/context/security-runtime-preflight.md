# Vorprüfung für Sicherheit und Laufzeit

## Wann diese Richtlinie gilt

Verwende diese Arbeitsrichtlinie, bevor eine Aufgabe sicherheitsbezogene Tests,
Reproduktionen, Regelkonfigurationen mit externen Auswirkungen oder
Laufzeitintegration ausführt. Sie verlangt keine Laufzeiteinrichtung für eine
gewöhnliche reine Dokumentationsänderung. Wähle Prüfungen, die der tatsächlichen
Aufgabe dienen, statt eine vollständige Host-Qualifizierungskampagne zu übernehmen.

Dieses v3-Upstream-Repository enthält den Bibliotheksanteil;
Webserver-Connectors sind eigenständige Projekte. Ein Core-API- oder
Regressionsergebnis belegt nicht das Verhalten eines konkreten NGINX-,
Apache-, Envoy- oder anderen Hosts.
Quelle: [Upstream-Grenze zwischen Core und Connector][connectors].

## Feststellen, was ausgeführt wird

Halte Core-Commit, wesentliche lokale Änderungen, relevante Submodulrevisionen,
Build-Flags/-Fähigkeiten, ausgewählten Test oder Reproduktionsfall,
Regelkonfiguration und erwartete Beobachtung fest. Lies die betroffenen
Quellen und Testdaten vor der Ausführung. Verwende
[build-and-test.md](build-and-test.md),
[configuration-and-rules.md](configuration-and-rules.md) und
[cpp-guide.md](cpp-guide.md), um die tatsächlich beteiligten Pfade zu ermitteln.

Einige Core-Operationen wirken über die Untersuchung einer Zeichenkette im
Arbeitsspeicher hinaus:

| Quellpfad oder Symbol | Zu berücksichtigendes beobachtetes Verhalten |
| --- | --- |
| `RulesSet::loadRemote` und der Scanner-Zweig `SecRemoteRules` | Rufen beim Beschaffen von Regeltext den HTTP-Client auf. |
| `HttpsClient::download` | Verwendet libcurl, wenn diese Fähigkeit einkompiliert ist; ruft `curl_easy_perform` auf und gibt dessen Ergebnisstatus zurück. |
| `audit_log::writer::Https::write` | Serialisiert eine Transaktion nach JSON und übergibt sie dem HTTP-Client für das konfigurierte Audit-Ziel. |
| `InspectFile::evaluate` | Führt ein kompatibles Lua-Skript aus oder startet einen plattformspezifischen Hilfsprozess. |

Quellen: [Remote-Rules-API][remote-api], [Remote-Rules-Scanner][remote-scanner],
[HTTP-Client][http], [HTTPS-Audit-Writer][audit],
[inspectFile-Implementierung][inspect].

Beschreibe deshalb beliebiges Regelladen oder die Ausführung von Regressionen
nicht als frei von Nebenwirkungen. Prüfe die gewählte Konfiguration auf
Dateipfade, entfernte Ziele, Skripte, Hilfsprogramme, Logging und dauerhaften
Zustand.

## Eine begrenzte Umgebung herstellen

Verwende für eine lokale Reproduktion synthetische Eingaben und aufgabeneigene
Testdaten, sofern die Aufgabe nicht ausdrücklich andere Daten verlangt.
Halte sensible Anfragen, Zugangsdaten und private Logs aus versionierten Tests
und gemeinsam genutzten Berichten heraus.

Wähle beschreibbare, aufgabeneigene Ausgabepfade und prüfe ihre Identität, bevor
du sie beschreibst, entfernst oder an einen Hilfsprozess übergibst. Arbeite mit
den tatsächlich benötigten Rechten; installiere keinen Systemdienst, ändere
keine rechnerweiten Besitzverhältnisse und starte keinen öffentlich erreichbaren
Listener allein zur Prüfung von Bibliotheksverhalten.

Benötigt ein Test Netzwerkzugriff, benenne das Ziel und warum es zur
autorisierten Aufgabe gehört. Verwende für eine lokale Server-Testumgebung
einen isolierten Endpunkt, vermeide kollidierende Ports oder Sockets und lege
Start, Bereitschaft, Zeitlimit, Beendigung und Ausgabezugehörigkeit vor dem
Start fest.

Prüfe ausführbare Hilfsprogramme und Skripte, die durch die Testdaten ausgewählt
werden. Ein erwarteter Fehler oder eine bösartig wirkende Eingabe in einer
Regression ist Testmaterial, keine Erlaubnis für einen sachfremden Befehl.
Beachte [command-execution-policy.md](command-execution-policy.md) und
[resources-and-storage.md](resources-and-storage.md).

## Aussagen auf die beobachtete Ebene begrenzen

Unterscheide Konfigurationsladen, Bibliotheksauswertung, eine angeforderte
Intervention und deren tatsächliche Durchsetzung durch den Host. Halte
Engine-Modus, durchlaufene Phasen/API-Aufrufe und beobachtetes Ergebnis fest.
Fehlende Tests, deaktivierte Fähigkeiten oder nicht erfüllte Voraussetzungen
sind Einschränkungen, keine Belege für sicheres Verhalten.

Der HTTPS-Audit-Writer ruft `download` auf und gibt `true` zurück, ohne dessen
Rückgabewert zu verwenden. Diese Rückgabe allein belegt daher keinen
erfolgreichen entfernten Empfang. Prüfe die für die Aufgabe erforderliche
Beobachtung auf der passenden Ebene.
Quelle: [Https::write][audit].

Gehört externe Host-Integration ausdrücklich zum Auftrag, lies den eigenen
Quellcode, die Revision, Anweisungen und Laufzeitvoraussetzungen des Connectors.
Übertrage weder die NGINX-Worker-Identitätsregeln, Envoy-Socket-Struktur oder
Framework-/MRTS-Orchestrierung des Referenzrepositorys noch erfundene
Host-Befehle in den Core.

## Abschluss

Erfasse das Endergebnis und wesentliche Ausgaben, beende aufgabeneigene
Laufzeitprozesse und stelle nur tatsächlich veränderten und sicher
wiederherstellbaren Zustand wieder her. Dokumentiere fehlgeschlagene,
übersprungene, blockierte und nicht ausgeführte Prüfungen gemäß
[testing-and-evidence.md](testing-and-evidence.md). Entferne temporäre Testdaten,
die gelöscht werden können, gemäß
[cleanup-and-restoration.md](cleanup-and-restoration.md).

Quellprüfung, eine erfolgreiche Reproduktion und eine begrenzte Zahl
bestandener Regressionen stützen konkrete Aussagen. Sie sind keine allgemeine
Sicherheitsgarantie, vollständige Schwachstellenbewertung oder
Produktionszertifizierung.

[connectors]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/README.md#L40-L46
[remote-api]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/rules_set.cc#L86-L96
[remote-scanner]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/seclang-scanner.ll#L1344-L1378
[http]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/utils/https_client.cc#L66-L142
[audit]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/audit_log/writer/https.cc#L50-L58
[inspect]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/operators/inspect_file.cc#L124-L345
