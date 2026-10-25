# Teilagenten und Hintergrundarbeit

## Verantwortung und Umfang

Diese Arbeitsrichtlinie gilt, wenn die aktive Umgebung Teilagenten oder
Hintergrundausführung bereitstellt. Sie setzt keine dieser Fähigkeiten voraus.
Der koordinierende Agent bleibt für Aufgabenumfang, Nachweise, Zusammenführung,
Prüfung, Bereitstellungsentscheidungen und den Abschlussbericht verantwortlich.

Delegiere unabhängige, begrenzte Arbeit, wenn der Gewinn an Geschwindigkeit oder Prüfqualität
den Koordinationsaufwand rechtfertigt. Rein lesende Quellcodeuntersuchungen und unabhängige
Reviews eignen sich dafür. Ein Helfer erhält keine weitergehenden Befugnisse als die Hauptaufgabe,
und Delegation überträgt keine wesentliche Nutzerentscheidung auf einen Agenten.

## Aufträge festlegen

Jeder Auftrag soll Folgendes benennen:

- Repository-Wurzel und relevanten Quellstand.
- Konkrete Frage oder Ergebnis sowie Abschlusskriterien.
- Rein lesenden Modus oder die genauen Dateien, die der Agent bearbeiten darf.
- Relevante vorhandene Anweisungen und Anforderungen an Quellen und Nachweise.
- Zuständigkeit für temporäre Ausgaben, falls temporäre Dateien notwendig sind.
- Verbotene Handlungen, einschließlich aufgabenspezifischer Grenzen für Builds,
  Netzwerknutzung, Änderungen an Abhängigkeiten, Commits, Pushes oder externe Nachrichten.

Weise parallel Schreibenden überschneidungsfreie Dateien zu. Bestimme eine einzige zuständige
Person oder Instanz für die Zusammenführung gemeinsamer Indizes, Manifeste, Zustandsaufzeichnungen
und abschließender Konsistenzprüfungen. Überschreibe keine Änderungen anderer Agenten und mache
keine sachfremden Änderungen rückgängig, um ein lokales Ergebnis zu vereinfachen.

## Grenzen der Kernbibliothek und Submodule

Die deklarierten Submodule sind `others/libinjection`, `others/mbedtls`,
`test/test-cases/secrules-language-tests` und `bindings/python`.
Eine delegierte Untersuchung eines dieser Pfade muss dessen Repository und festgelegten Stand
vom Kern-Repository unterscheiden. Das Lesen einer Abhängigkeit erlaubt weder Änderungen
an ihr noch am Gitlink im Kern-Repository.
Quelle: [Submoduldeklarationen][submodules].

Diese Richtlinie führt kein Framework/MRTS-Auftragsmodell ein. Nutze die tatsächlichen Grenzen
der Kernbibliothek und die Anweisungen der betreffenden Abhängigkeit, wenn ihr Quellcode
zum Aufgabenumfang gehört. Die technischen Leitfäden
[C++-Leitfaden](cpp-guide.md), [Build und Tests](build-and-test.md) und
[Konfiguration und Regeln](configuration-and-rules.md) bieten Lesewege.

## Hintergrundbefehle

Halte vor dem Start einer länger laufenden Aufgabe Zweck, Arbeitsverzeichnis,
Prozess- oder Sitzungsreferenz, relevanten Ausgabeort, erwartete Dauer oder Zeitlimit
und Abschluss- beziehungsweise Abbruchbedingung fest. Nutze die von der Umgebung unterstützte
Prozesssteuerung; entkopple keinen Prozess so, dass sein Status oder seine Zuständigkeit verloren geht.

Prüfe [Ressourcen und Speicher](resources-and-storage.md) hinsichtlich Kapazität und
getrennter Ausgaben. Parallele Builds oder Tests dürfen nicht denselben Checkout neu konfigurieren,
gemeinsame Protokolle oder Testdaten überschreiben oder um einen Laufzeitport oder veränderlichen
Dienstzustand konkurrieren. Rein lesende Suchen lassen sich gewöhnlich unabhängig ausführen,
ohne einen Hintergrunddienst einzurichten.

Beobachte den Fortschritt und erfasse den endgültigen Exit-Status. Stoppe oder passe Arbeit an,
die ihren Umfang überschreitet, mit einem anderen Schreibenden kollidiert, unerwartet wächst
oder sich nicht beobachten lässt. Halte den Nutzer während längerer Arbeit auf dem Laufenden.

## Zusammenführung und Abschluss

Prüfe jedes Helferergebnis und die dazugehörigen Nachweise. Eine Zusammenfassung eines anderen
Agenten beweist für sich genommen nicht die Richtigkeit einer Quellcodeaussage oder eines
Testergebnisses; überprüfe wesentliche Befunde anhand der betroffenen Dateien oder aufgezeichneten Ausführung.

Hole vor der Abschlussmeldung die erforderlichen Ergebnisse ein, beende oder stoppe zur Aufgabe
gehörende Helfer und Befehle und bestätige, dass kein unbeabsichtigter Prozess verbleibt.
Bewahre notwendige Übergabenachweise auf und entferne nur dafür vorgesehene, zur Aufgabe
gehörende temporäre Dateien gemäß [Bereinigung und Wiederherstellung](cleanup-and-restoration.md).

Ein gesondert freigegebener dauerhafter Dienst oder Überwachungsauftrag benötigt eine ausdrückliche
Zuständigkeit und einen Lebenszyklus; er darf nicht als undokumentierte Nebenwirkung gewöhnlicher
Repository-Arbeit zurückbleiben.

[submodules]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/.gitmodules#L1-L12
