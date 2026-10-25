# Werkzeuge und Sprache

## Geltungsbereich

Dies ist eine Arbeitsrichtlinie für dieses Core-Repository. Sie legt die Wahl
geeigneter Werkzeuge und Prüfungen fest;
[command-execution-policy.md](command-execution-policy.md) regelt ihre
Ausführung. Die angeführten Quellen beschreiben den festgelegten
Upstream-Stand, während die folgenden Anweisungen die Pflege dieses
Arbeitsrepositorys bestimmen.

## Den tatsächlichen Core-Build verwenden

Lies [build-and-test.md](build-and-test.md), bevor du Build- oder Testbefehle
wählst. Der Unix-Build verwendet Autotools und enthält `others`, `src`, `doc`,
`tools` sowie bedingt `examples` und `test`. Windows hat einen eigenen Weg über
`vcbuild.bat`/CMake. Wähle den Weg, der zur Aufgabe und Umgebung passt.
Quellen: [Build-Unterverzeichnisse][subdirs],
[Vorbereitungsskript][bootstrap], [Windows-Build-Skript][windows].

Bevorzuge vorhandene Build-Definitionen und Skripte gegenüber einem neuen
parallelen Build-System. Prüfe Werkzeugverfügbarkeit und Version, wenn die
Aufgabe davon abhängt. Ein aus der CI kopierter Befehl beschreibt die Umgebung
dieses Workflows; er belegt nicht, dass dieselben Pakete, Berechtigungen oder
Programme lokal vorhanden sind.

## C und C++

`configure.ac` prüft C- und C++-Compiler und verlangt über
`AX_CXX_COMPILE_STDCXX(17, noext, mandatory)` ausdrücklich C++17. Auch die
Windows-CMake-Konfiguration verlangt C++17. Verwende für Kompilierung und
Analyse die tatsächlichen Komponenten-Flags; übernimm keine C17-Anforderung
eines anderen Repositorys und erhöhe nicht stillschweigend die zugrunde gelegte
Sprachversion. Quellen: [Compilerprüfungen][compilers], [Windows-CMake][cmake].

Lies [cpp-guide.md](cpp-guide.md) zu öffentlichen Schnittstellen,
Besitzverhältnissen und Lebensdauer. Untersuche Deklarationen, Implementierungen
und relevante Aufrufer gemeinsam. Diagnose-Builds, Sanitizer und ein neuerer
Compilermodus sind gesonderte Konfigurationen; berichte ihre tatsächlichen
Flags und Ergebnisse, ohne sie als normalen Build darzustellen.

Leite eine Kompilierungsdatenbank bei Bedarf aus einem tatsächlichen,
anwendbaren Build ab. Halte Quellrevision und Konfiguration fest und nimm
vorübergehende Analyseartefakte nicht in Commits auf. Eine Meldung eines
Editors oder Analysewerkzeugs ist ein Ansatzpunkt für eine Untersuchung;
sie ist keine erfolgreiche Kompilierung oder Laufzeitprüfung.

## Parsereingaben und Quelltextstil

Die Parsererzeugung ist eine standardmäßig deaktivierte Build-Option. Bei
Aktivierung prüft die Konfiguration Flex und Bison; das Parser-Makefile verwendet
die `.ll`-/`.yy`-Eingaben und führt eine zusätzliche Header-Nachbearbeitung aus.
Stimme Änderungen an diesen Eingaben mit den erzeugten Dateien ab, statt die
erzeugte Ausgabe als alleinige Sprachdefinition zu behandeln.
Quellen: [Erzeugungsoption][parser-option], [Parser-Makefile][parser-make].

Beachte die vorhandene [EditorConfig][editorconfig] und den Stil der umgebenden
Datei. Prüfe Formatiererausgaben vor der Anwendung und halte nicht zugehörige
Umformatierungen aus einer gezielten Änderung heraus. Lies
[configuration-and-rules.md](configuration-and-rules.md), wenn eine Änderung
SecLang betrifft und nicht nur die C++-Implementierung.

## Skripte, Anbindungen und optionale Hilfsprogramme

Für Python-Arbeit gilt [python-policy.md](python-policy.md). Die
Python-Anbindungen sind ein eigenständiges Submodul, und die
Unterverzeichnisliste des Builds auf oberster Ebene enthält diesen Pfad nicht.
Prüfe vor Arbeiten dort die eigene Revision und die Anweisungen der Anbindungen.
Quellen: [deklarierte Submodule][submodules], [Unterverzeichnisse][subdirs].

Verwende verfügbare Suchwerkzeuge wie `rg` zur Navigation. Setze Shell-Linting,
Formatierer oder weitere Analysewerkzeuge nur ein, wenn sie für die geänderten
Dateien relevant sind. Das Core-Makefile definiert `check-static` über cppcheck
und `check-style` über `check-coding-style`; diese benötigen ihre tatsächlichen
Werkzeuge und ersetzen keine Verhaltenstests.
Quelle: [Analyse- und Stilziele][analysis].

Installiere oder aktualisiere Compiler, Interpreter, Analyse- oder
Assistentenwerkzeuge nicht als beiläufige Aufräumarbeit. Eine notwendige
Werkzeug- oder Abhängigkeitsänderung gehört mit dokumentierter Kompatibilität,
Herkunft und Validierung in den Aufgabenumfang.

[subdirs]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/Makefile.am#L2-L17
[bootstrap]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/build.sh#L1-L33
[windows]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/vcbuild.bat
[compilers]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L49-L63
[cmake]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/build/win32/CMakeLists.txt
[parser-option]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L284-L310
[parser-make]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/Makefile.am#L1-L43
[editorconfig]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/.editorconfig#L1-L25
[submodules]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/.gitmodules#L1-L12
[analysis]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/Makefile.am#L58-L85
