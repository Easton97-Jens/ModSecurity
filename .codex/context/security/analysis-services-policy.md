# Optionale externe Analysedienste

Nutze diesen Leitfaden nur, wenn externe Analyse angefragt wird oder bereits zur freigegebenen Aufgabe gehört. Diese Datei stellt nicht fest, dass ein Dienst, Plugin, eine CLI, ein Endpunkt, Konto oder eine Lizenz verfügbar ist.

## Vor der Nutzung

Ermittle den tatsächlich konfigurierten Zugangsweg, die unterstützte Operation, den ausgewählten Quellstand und freigegebenen Umfang. Nutze bei Bedarf aktuelle offizielle Diensthinweise. Erfinde weder TAC-Daemon, Scannerbefehl, Laufkennung, SonarQube-Projekt noch Qualitätskriterium. Lade keinen Code und keine Daten außerhalb des freigegebenen Aufgabenumfangs hoch.

Halte Zugangsdaten, private Anfragekörper, unverarbeitete Kundenprotokolle und sachfremde Dateien aus Analyseeingaben und aufbewahrten Ausgaben heraus. Nutze den kleinsten relevanten Quellumfang, den die angefragte Analyse benötigt.

## Aufzeichnen und abgleichen

Halte für jeden tatsächlich genutzten Dienst seine Identität, den Aufruf, die Zuordnung zu Revision und Branch, analysierte Dateien, zurückgegebene Lauf- oder Anfragekennungen, soweit verfügbar, Kandidatenergebnisse und Grenzen fest. Ein erfolgreicher Upload oder Authentifizierungsschritt ist keine abgeschlossene Analyse.

Halte Dienstbefunde, Compilerdiagnosen, Quellcodebegründungen, Tests und Laufzeitbeobachtungen als getrennte Nachweisquellen fest. Validiere Kandidaten anhand der [Befundvalidierung](checks/finding-validation.md); fasse Duplikate erst zusammen, nachdem Herkunft und unabhängig erreichbare Vorkommen erhalten wurden.

Ein unauffälliges Ergebnis widerlegt kein unabhängig nachgewiesenes Problem. Ein Widerspruch verlangt die Untersuchung des zugrunde liegenden Quellpfads und seiner Bedingungen. Externe Analyse ersetzt weder einen erforderlichen Build oder Test noch ein gesondert abgegrenztes Connector-Ergebnis.

Kann ein angefragter Dienst nicht ausgeführt werden, halte die genaue Zugriffs- oder Umgebungslücke fest und setze die weiterhin freigegebene unabhängige Arbeit fort. Behaupte nicht stillschweigend, ein anderer Dienst habe gleichwertige Abdeckung geliefert.
