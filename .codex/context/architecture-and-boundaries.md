# Architektur und Repository-Grenzen

Diese Richtlinie ordnet Arbeit der richtigen Quell- und Nachweisgrenze zu. Sie ist
eine Arbeitskonvention für diesen Fork. Lies [project-context.md](project-context.md),
[architecture.md](architecture.md) und [repository-map.md](repository-map.md)
für die durch Quellen belegte technische Erklärung.

## Identität des Repositorys

Das Arbeitsrepository ist `Easton97-Jens/ModSecurity`. Sein deutschsprachiger Kontextbranch ist
`codex/kontext-v3-de`, auf Grundlage des englischen Kontextbranchs `codex/kontext-v3`.
Der Auftrag für diese Variante umfasst Übersetzung und Hochladen des Branchs, ohne Pull Request oder Merge.
Die offizielle Quelle ist `owasp-modsecurity/ModSecurity`, Branch `v3/master`.
Verwende [upstream-lock.json](upstream-lock.json) für den dokumentierten Quellstand
und [current-state.md](current-state.md) für den beobachteten Arbeitsstand.
Prüfe vor einer Aktion den tatsächlichen Branch und Commit; Branchnamen sind veränderliche Referenzen.

Die Kernbibliothek verarbeitet HTTP-Daten, die eine Anwendung oder ein Connector
bereitstellt, und wertet konfigurierte Regeln aus. Webserver-Connectors sind separate Projekte.
Die [Upstream-Übersicht](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/README.md)
und die [Transaction-Schnittstelle](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/headers/modsecurity/transaction.h)
belegen diese Grenze. Schreibe Host-Hooks, Transportverarbeitung oder
Bereitstellungsverhalten eines Connectors nicht dieser Bibliothek zu, ohne den
separat identifizierten Quellstand des Connectors zu untersuchen.

## Zuständigkeit für die Arbeit

| Bereich | Was hierher gehört | Zu wahrende Grenze |
| --- | --- | --- |
| `headers/modsecurity/` und zugehörige Definitionen in `src/` | Kerntypen, C++- und C-Schnittstellen, Bibliotheksverhalten | Deklarationen, Definitionen und Aufrufer zusammen untersuchen; ein installierter Header verspricht keine ABI-Stabilität |
| `src/parser/` | Scanner der Regelsprache, Grammatik, Parser-Driver und erzeugte Parserdateien | `.ll`-/`.yy`-Eingaben von erzeugten Ausgaben unterscheiden |
| Andere Bereiche von `src/` | Regelauswertung, Aktionen, Transformationen, Operatoren, Variablen, Body-Verarbeitung, Collections, Logs und Hilfsfunktionen | Der betroffenen Implementierung und den Build-Bedingungen folgen, statt Unterstützung aus einem Dateinamen abzuleiten |
| `configure.ac`, `Makefile.am`, Build-Dateien der Komponenten und `build/` | Compiler-/Abhängigkeitserkennung und plattformbezogene Build-Definitionen | Build-Anweisungen hängen von der tatsächlichen Plattform und den konfigurierten Funktionen ab |
| `test/` | Tests, Testprogramme und deren Daten im Hauptrepository | Das separat festgelegte Submodul des Testkorpus getrennt halten |
| `examples/` und `tools/` | Bibliotheksbeispiele und unterstützende Werkzeuge | Ein Beispiel oder Werkzeugergebnis belegt nur sein gezeigtes Verhalten |
| `AGENTS.md`, `CODEX_START.md` und `.codex/` | Assistentenanweisungen, technische Erklärungen, Arbeitsabläufe und gespeicherter Kontext | Dies sind lokale Ergänzungen, keine Laufzeitfunktionen von Upstream |

Die Quellzuordnungen und die Liste der installierten Header stehen in
[src/Makefile.am](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/Makefile.am).
Das vollständige Verzeichnisinventar und weitere Belege stehen in der
[Repository-Übersicht](repository-map.md).

## Submodule und externe Projekte

Der Hauptquellstand verzeichnet vier direkte Submodulpfade:
`bindings/python`, `others/libinjection`, `others/mbedtls` und
`test/test-cases/secrules-language-tests`. Die Einträge stehen in
[.gitmodules](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/.gitmodules);
ihre tatsächlich festgelegten Commits sind in [provenance.md](provenance.md) dokumentiert.

Behandle Submodulquellen und deren Git-Historie als schreibgeschützt, sofern die
Nutzeraufgabe dort keine Änderungen umfasst. Das Lesen relevanter festgelegter Quellen
oder das Initialisieren festgelegter Abhängigkeiten für einen autorisierten Build erlaubt weder
das Bearbeiten dieser Quellen noch das Fortschreiben ihres Branchs oder die Änderung
des Gitlinks im Hauptrepository. Vermeide `git submodule update --remote` bei der gewöhnlichen Einrichtung.
Wenn eine Korrektur in ein Submodul gehört, kläre vor der Änderung den erforderlichen Umfang
und halte dessen Belege und Veröffentlichungsstand
getrennt.

Externe Connectors, unabhängig bereitgestellte Rulesets wie das Core Rule Set
und ModSecurity v2 sind getrennte Untersuchungsbereiche. Halte ihre Quellstände fest,
wenn sie relevant sind. Eine Änderung der Kernbibliothek belegt weder deren
Kompatibilität noch Laufzeitverhalten oder Testergebnisse.

## Nachweisgrenze

Unterscheide das Lesen von Quellen, erfolgreiche Builds, Testdefinitionen,
ausgeführte Tests und Ergebnisse der Connector-Integration. Keines davon belegt automatisch die anderen.
Binde Befunde an Quellstand, Build-Optionen, Abhängigkeiten, Eingaben
und Umgebung, die tatsächlich untersucht oder ausgeführt wurden.

Beginne bei einem bereichsübergreifenden Symptom mit der beobachtbaren Eingabe und den relevanten
Kernaufrufen. Eine gezielte Reproduktion auf Bibliotheksebene kann den Fehler genauer eingrenzen;
wenn er sich allein im Kern nicht reproduzieren lässt, ist das kein Beweis für einen Defekt
einer externen Komponente. Halte diese Unsicherheit gemäß [findings-policy.md](findings-policy.md) fest.
