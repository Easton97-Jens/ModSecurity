# Ablauf für Änderungen

Diese Richtlinie gilt für Funktionen, Fehlerkorrekturen, Refactorings, Konfiguration,
Build-Änderungen und umfangreiche Dokumentationsänderungen an der Kernbibliothek.
Planung und Prüfung sollen dem Umfang der gewünschten Änderung entsprechen.

## Die Änderung klären

1. Lies die aktuelle Aufgabe und den relevanten gespeicherten Kontext gemäß
   [Aufgabenablauf](task-workflow.md).
2. Ermittle das aktuelle Verhalten anhand des betroffenen Quellcodes, seiner Aufrufer und Tests.
   Trenne das vom Nutzer gewünschte Verhalten vom derzeitigen Verhalten des Codes.
3. Kläre die Zuständigkeit anhand von
   [Architektur und Grenzen](architecture-and-boundaries.md).
4. Ermittle wesentlich betroffene Schnittstellen, Regelsyntax, Standardwerte, Datenverarbeitung,
   Fehlerpfade, Build-Optionen, Kompatibilität und Ressourcennutzung.
5. Lege vor der Bearbeitung ein beobachtbares Abnahmekriterium und eine angemessene Prüfung fest.
   Für eine kleine Änderung genügt eine kurze Erklärung.

Ermittle vorhandene Testprogramme und tatsächliche Befehle anhand des aktuellen Quellcodes und
[Build und Tests](build-and-test.md). Erfinde kein Ziel und übernimm keinen Befehl aus
einem anderen Repository allein deshalb, weil die Aufgabe ähnlich aussieht.

## Funktionen und Fehlerkorrekturen

Erkläre bei einer Funktion relevante gültige, ungültige und ausgelassene Eingaben,
Standardwerte, Fehlerverhalten und Folgen für die Kompatibilität. Implementiere das
vereinbarte Verhalten vollständig. Verberge keinen unfertigen Pfad hinter einer
Erfolgsrückgabe oder einem undokumentierten Ersatzverhalten.

Trenne bei einem gemeldeten Fehler den Bericht von der überprüften Beobachtung.
Reproduziere ihn, soweit praktikabel, ermittle die verantwortliche Implementierung und
prüfe eine zulässige Eingabe, die weiterhin funktionieren muss. Ein gezielter Regressionstest,
der vor der Korrektur fehlschlägt und danach besteht, ist hilfreich, wenn er den tatsächlichen
Fehler auslöst. Ist keine Ausführung möglich, liefere die bestmögliche quellcodegestützte
Analyse und benenne, was ungeprüft bleibt.

Dokumentationskorrekturen benötigen gewöhnlich Quellen- und Verweisprüfungen, keinen neuen
C++-Test. Ergänze keine Tests, die lediglich die Implementierung wiedergeben, und weite eine
kleine Änderung nicht zu einer sachfremden Testkampagne aus.

## Implementierungsprüfungen für die Kernbibliothek

| Änderungsbereich | Am betroffenen Code zu klärende Fragen |
| --- | --- |
| C++-/C-Schnittstelle oder Objektzustand | Wem gehören die einzelnen Objekte und Puffer? Welche Aufrufer, Freigabepfade, exportierten Deklarationen oder Speicherlayouts sind betroffen? |
| Transaktionsverarbeitung | Welche Phase, Konfiguration, Einstellung für den Zugriff auf den Nachrichtenkörper, welcher Fehlerpfad und welcher Interventionspfad werden tatsächlich ausgeführt? |
| Regelsyntax, Aktionen, Operatoren oder Variablen | Akzeptiert der Parser die vorgesehene Form? Wo sind Standardwerte und Auswertungssemantik definiert? Welche vorhandenen Tests decken sie ab? |
| Parser-Eingaben | Wie bleiben die Änderung an `.ll`/`.yy` und die versionierten generierten Dateien im Erzeugungsprozess des Repositorys konsistent? |
| Optionale Abhängigkeiten oder Build-Optionen | Was geschieht bei aktivierter und deaktivierter Funktion? Welche Tests werden in dieser Konfiguration gebaut oder übersprungen? |
| Protokollierung und aufbewahrte Daten | Beeinflusst die Änderung Inhalt, Eigentümerschaft, Lebensdauer, Ausgabefehler oder unbeabsichtigte Offenlegung? |

Die öffentlichen Schnittstellen der Kernbibliothek stehen in
[modsecurity.h](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/modsecurity.h),
[rules_set.h](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/rules_set.h) und
[transaction.h](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/transaction.h).
Der Ausgangsstand verlangt C++17 in
[configure.ac](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L63).
Die Beziehungen zwischen Parser-Quellen und generierten Dateien stehen in
[src/Makefile.am](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/Makefile.am#L17-L28).
Das sind Einstiegspunkte für die Untersuchung, keine allgemeinen Garantien zur Eigentümerschaft oder Sicherheit.

Beachte [`.editorconfig`](../../.editorconfig) und den umgebenden Code. Halte die
Änderung zusammenhängend, erhalte sachfremde Arbeit und vermeide beiläufige Neuformatierung
oder Aktualisierungen von Abhängigkeiten. Schwäche Prüfungen nicht ab und verberge keine Fehler,
um einen erfolgreichen Durchlauf zu erhalten.

## Prüfen und übergeben

Führe die engsten aussagekräftigen Prüfungen für das betroffene Verhalten sowie alle
anwendbaren verbindlichen Repository-Prüfungen aus. Beziehe bei einer Fehlerkorrektur nach
Möglichkeit die ursprüngliche Reproduktion und die relevante Kontrollprüfung ein. Erweitere
Tests nur zur Klärung eines konkreten verbleibenden Risikos oder zur Erfüllung einer geltenden Vorgabe.

Halte die tatsächlich ausgeführten Prüfungen, ihre Ergebnisse, relevante Konfiguration und
fehlende Abdeckung fest. Trenne Quellcodeanalyse, Laufzeitnachweise und Schlussfolgerungen.
Aktualisiere die betroffenen Erklärungen und den dauerhaften Aufgabenstand gemäß
[Dokumentation und Nachvollziehbarkeit](documentation-and-traceability.md).
Wende vor dem Ergebnisbericht die [Abschlusskriterien](definition-of-done.md) an.
