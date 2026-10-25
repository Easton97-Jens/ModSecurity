# Optionale Ergänzung zur strikten Vollständigkeit der Ausführung

## Anlass

Nutze dieses Profil nur, wenn der Nutzer ausdrücklich verlangt, dass jedes anwendbare verpflichtende Ziel einer erklärten Bewertung ein ausgeführtes Ergebnis erreicht. Diese Ergänzung erlaubt weder das Überschreiten von Berechtigungen noch das Abschwächen von Kontrollen oder eine stillschweigende Ausweitung des Umfangs.

## Ziele erfassen

Lege Zielkennungen und Anwendbarkeit vor der Ausführung fest. Ein Ziel ist durch ein tatsächliches Ergebnis `passed`/`failed` abgedeckt oder durch eine technisch begründete Entscheidung `not_applicable` ausgeschlossen. Fehlende Werkzeuge, Zugriff, Zeit oder Umgebung lassen eine ungelöste Abdeckungslücke zurück.

Ein Versuch, eine Voraussetzung herzustellen, hat ein eigenes Ergebnis. Scheitert er vor Beginn des Ziels, zähle das Ziel nicht als ausgeführt und entferne es nicht aus der Grundgesamtheit für die Vollständigkeitsberechnung. Erhalte die Beschreibungen `blocked`/`not_run`, soweit hilfreich, und halte die Lücke ausdrücklich sichtbar.

## Lücken bearbeiten

Untersuche den ersten relevanten Fehler einer Voraussetzung, nutze vorhandene unterstützte Einrichtungswege, stelle fehlende aufgabenlokale Voraussetzungen bei vorhandener Freigabe her, führe das ursprüngliche Ziel nach sinnvollen Korrekturen erneut aus und setze unabhängige Ziele fort. Erhalte frühere Versuche und die jeweils erreichte Stufe.

Ändere keine Prüfaussagen, lösche keine Fälle, umgehe keine Isolation und erfinde keinen Erfolg, um die Aufstellung zu erfüllen. Beende wiederholte Versuche, wenn sie das konkrete Hindernis innerhalb der verfügbaren Befugnisse und Umgebung nicht lösen können; berichte diese Grenze.

## Abschluss

Verbleibende anwendbare, ungelöste Ziele machen die strikte Ausführung unvollständig, selbst wenn der Bericht selbst fertig ist. Ein ausgeführter fehlgeschlagener Produkttest kann dennoch vollständig erfasst sein. Halte Vollständigkeit der Ausführung, Produkterfolg und Sicherheitsschlussfolgerungen getrennt.
