# Protokollgrenzen der Bibliothek und Diagnose-Builds

## Geltungsbereich

Dies ist ein optionaler Prüfungsleitfaden. Die Kernbibliothek und getrennt gepflegte Connectoren haben unterschiedliche Verantwortlichkeiten. Die öffentliche Transaktions-API erhält Verbindungsdaten, URI, Header und Puffer für Nachrichtenkörper von einem Aufrufer; die Upstream-README ordnet die Webserver-Integration externen Connectoren zu. Quellen: [API](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/transaction.h#L335-L419), [Projektgrenze](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/README.md#L21-L46).

Ein API-Test mit einer HTTP-Versionszeichenfolge weist weder HTTP/2, HTTP/3, ALPN, QUIC, die Übertragungseinrahmung, Stream-Resets noch Webserver-Routing nach. Solche Aussagen benötigen eine gesondert abgegrenzte Connector- und Transportumgebung mit eigenen Revisionen und beobachtetem Datenverkehr. Erweitere einen Testplan für die Kernbibliothek nicht zu einer erfundenen Host- oder CRS/MRTS-Matrix.

## Antwortprüfung und Intervention

`processResponseHeaders()` prüft vor der Auswertung seiner Phase, ob die Engine deaktiviert ist. Die Auswertung des Antwortkörpers prüft den Engine-Zustand, den Zugriff auf den Nachrichtenkörper und die MIME-Auswahl. Auch `processLogging()` kehrt bei deaktivierter Engine vorzeitig zurück. Quellen: [Antwortheader](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L992-L1097), [Antwortkörper](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L1117-L1230), [Protokollierung](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L1304-L1346).

`Transaction::intervention()` stellt eine Bibliotheksentscheidung bereit; daraus geht nicht hervor, ob ein Connector einen Status geändert, die Ausgabe gestoppt oder einen Stream zurückgesetzt hat. Halte Regelauswertung, Treffer, angeforderte Intervention, Handlung des Aufrufers und clientseitig sichtbares Ergebnis auseinander. Übernimm die Zielmodi `off`/`safe`/`strict` des Connector-Referenzprojekts nicht als Konfiguration der Kernbibliothek. Quelle: [Intervention](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/transaction.cc#L1350-L1389).

Nutze das [Profil für Antwortphasen](profiles/response-phase-assessment.md) und die [Prüfung von Antwortinterventionen](security/checks/response-intervention.md), wenn diese Grenzen relevant sind.

## Normale Builds und Diagnose-Builds

Kläre zuerst die Anforderungen des normalen Builds und den tatsächlichen Compiler. Der festgelegte Unix-Build verlangt C++17; auch der Windows-Build legt C++17 fest. Quellen: [Konfiguration](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L49-L63), [Windows-CMake](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/build/win32/CMakeLists.txt#L102-L104).

ASan, UBSan, TSan, Fuzzing, statische Analyse und Härtungskonfigurationen sind optionale, gesondert dokumentierte Prüfungen. Wähle Flags erst nach dem Lesen der tatsächlichen Build-Definitionen und der Prüfung von Compiler-Unterstützung, Einbindung der Abhängigkeiten und Artefakttyp. Schreibe einer Bibliothek keine nur für ausführbare Programme geeigneten Flags vor und verwandle einen normalen Build nicht stillschweigend in einen Diagnose-Build.

Nutze getrennte, zur Aufgabe gehörende Ausgabepfade, soweit der Build sie unterstützt. Halte Flags, Abhängigkeitsvarianten, Laufzeitvoraussetzungen und die tatsächlich durchgeführten Prüfungen fest. Ein Diagnoseergebnis ersetzt keinen erforderlichen normalen Build; ein erfolgreicher normaler Build belegt weder einen fehlerfreien Sanitizer-Lauf noch Transportverhalten.

Kläre für temporäre Testgerüste vor der Ausführung Arbeitsverzeichnis, beschreibbare Pfade, Eingabegrenzen, Zeit- und Ressourcenlimits sowie die Bereinigung. Ein Unittest der Kernbibliothek benötigt keine erfundene NGINX-Worker-Identität, keinen Envoy-Socket und keinen Connector-Dienst.
