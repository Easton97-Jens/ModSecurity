# Richtlinie für Befunde und Behebung

Nutze dauerhafte Befundaufzeichnungen für wesentliche Fehler, ungelöste Hindernisse oder
Nachweislücken, die eine spätere Sitzung verstehen muss. Eine routinemäßige kleine Korrektur
kann im Aufgabenprotokoll erklärt werden, ohne ein eigenes Befundsystem einzurichten.
Diese Richtlinie macht nicht jede Aufgabe zu einer Sicherheitsbewertung.

## Beobachtungen von Schlussfolgerungen trennen

Unterscheide ein gemeldetes Symptom, eine Quellcodebeobachtung, einen reproduzierbaren Fehler,
eine ungeprüfte Hypothese und ein fehlendes Testergebnis. Eine statische Warnung oder ein
verdächtiger Name ist ein Untersuchungsansatz, nicht automatisch ein bestätigter Fehler oder
eine Sicherheitslücke.

Halte für jede unabhängig behebbare Ursache die jeweils hilfreichen Angaben fest:

- eine stabile lokale Kennung und eine knappe Beschreibung;
- betroffenes Repository, Quellstand, Pfade und Symbole;
- erwartetes Verhalten und beobachtete Abweichung;
- Konfiguration, Eingaben, Reproduktion oder Quellcodebegründung;
- Auswirkungen und Vertrauen in den Befund, mit gesondert genannter Unsicherheit;
- Zuständigkeit, Status und ein beobachtbares Abnahmekriterium;
- den vorgeschlagenen nächsten Schritt und die zum Abschluss erforderliche Prüfung.

Nutze die [offenen Aufgaben](backlog.md) für ausstehende Punkte und verlinke bei umfangreichen
Details eine begrenzte Aufzeichnung in den [Sitzungsprotokollen](../audit/sessions/).
Nutze den [Faktenabgleich](fact-check.md) für Widersprüche zwischen Dokumentation und Quellcode.
Erfinde keine Befundkennung, die den Eindruck erweckt, aus einem externen Ticketsystem zu stammen.

## Nachweise und Zuständigkeit

Verknüpfe Beobachtungen mit dem genauen Quellstand und tatsächlich ausgeführten Befehlen.
Bewahre nur die Diagnoseinhalte auf, die zur Reproduktion oder Erklärung erforderlich sind,
und entferne Geheimnisse sowie sensible Nutzdaten. Vermeide vollständige Umgebungsabbilder
oder ungeprüfte große Artefakte.

Trenne Nachweise zur Kernbibliothek, zu einem Submodul und zu einem externen Connector
gemäß [Architektur und Grenzen](architecture-and-boundaries.md). Ein über einen Connector
beobachteter Fehler verortet die Ursache für sich genommen nicht in dieser Bibliothek.
Ein reiner Kerntest belegt nicht das durchgängige Verhalten des Connectors.

## Zustände der Behebung

Eine zweckmäßige Aufzeichnung kann von **offen** über **untersucht** und **behoben** zu
**verifiziert** wechseln; **blockiert**, **nicht reproduziert** oder **außerhalb des Umfangs**
werden verwendet, wenn diese Beschreibungen zutreffen. Das sind Beschreibungen der Nachweislage,
keine verpflichtenden formalen Schritte.

- **Behoben** bedeutet, dass die relevante Änderung vorhanden ist; nenne ihre Dateien oder Revision.
- **Verifiziert** bedeutet, dass das Abnahmekriterium mit geeigneten Nachweisen geprüft wurde,
  soweit möglich einschließlich des ursprünglichen Fehlers und zulässigen Verhaltens.
- **Nicht reproduziert** bedeutet, dass der Reproduktionsversuch den gemeldeten Fehler unter
  den angegebenen Bedingungen nicht belegt hat. Das beweist nicht seine Abwesenheit.
- **Akzeptiertes Risiko** setzt eine ausdrückliche Entscheidung des verantwortlichen Nutzers oder
  Eigentümers voraus, auch wenn sie bereits in der aktuellen Aufgabe festgehalten ist; ein Assistent
  darf eine solche Zustimmung nicht stellvertretend erfinden.

Ein Commit, Push, PR, Merge oder eine sachfremde erfolgreiche Prüfung schließt einen Befund
nicht automatisch. Stützt die Quellcodeprüfung eine Korrektur, während die Ausführung noch fehlt,
benenne diesen Unterschied, statt die Laufzeitprüfung als abgeschlossen darzustellen.

## Umfang weiterer Arbeit

Behebe zur Aufgabe gehörende Befunde innerhalb des freigegebenen Umfangs gemäß dem
[Änderungsablauf](change-workflow.md). Halte sachfremde Bedenken fest, ohne die Aufgabe
stillschweigend auszuweiten oder eine Abhängigkeit zu ändern. Erweitere die Untersuchung nur,
wenn dies zum Nachweis des gewünschten Ergebnisses nötig ist oder der Nutzer es verlangt.

Externe Issues, Antworten auf Reviews, Sicherheitsberichte oder andere Nachrichten benötigen
die Nutzerfreigabe für diese Kommunikation. Erhalte bereits erteilte Freigaben; veröffentliche
sensible Befunde nicht als routinemäßige Nebenwirkung der Pflege lokalen Kontexts.
