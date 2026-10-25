# Befehle ausführen

## Zweck

Diese Richtlinie regelt die Ausführung von Befehlen für eine autorisierte
Aufgabe in diesem Repository. Bereits erteilte Benutzerfreigaben bleiben
wirksam. Ein Werkzeug, Befehlsbeispiel oder Richtliniendokument erweitert diese
Freigabe nicht auf ein anderes Repository, ein externes Ziel, eine
Systeminstallation, einen Release oder einen Dienst.

Verwende die in der aktuellen Umgebung verfügbare Befehlsschnittstelle.
Bevorzuge bei der üblichen Erkundung unterstützte RTK-Zusammenfassungen, wenn
RTK verfügbar ist, und beachte den [RTK-Leitfaden](rtk.md). Sein Fehlen blockiert
die Arbeit nicht und erlaubt keine Installation. Ein fester Wrapper-Pfad oder
ein verpflichtender Assistenten-Skill wird nicht vorausgesetzt. Falls die
aktuelle Umgebung einen Ausführungs- oder Authentifizierungs-Wrapper vorgibt,
verwende dessen dokumentierte Schnittstelle, statt Flags zu erfinden oder ihn
zu umgehen.

Verwende native Ausgaben für das exakte Lesen von Quellcode, maschinenlesbare
Eingaben, die abschließende Diff-Prüfung und Validierungsbelege. Behalte die
dokumentierten Build-/Testbefehle und die Kontextprüfprogramme unverändert bei;
setze nicht beliebigen Befehlen `rtk` voran und ändere nicht stillschweigend
Argumente, Arbeitsverzeichnis, Berechtigungen oder Offline-Anforderungen.
Halte den tatsächlichen Befehl und Exitcode fest. Ausgabeverdichtung belegt weder
eine erfolgreiche Prüfung noch eine gemessene Kostensenkung.

## Arbeitskontext feststellen

Stelle vor einer Befehlsfolge Repository-Stammverzeichnis, aktuellen Branch,
HEAD, Änderungen im Arbeitsbaum und vorgesehenes Arbeitsverzeichnis fest.
Erhalte vorhandene Arbeit. Verwende ausdrückliche Pfade oder das
Arbeitsverzeichnisargument eines Werkzeugs, wenn mehrere Checkouts vorhanden
sind. Unterscheide den Core-Checkout von jedem Submodul.

Lies Befehlsdefinitionen und relevante Eingaben, bevor du unbekannte Skripte
ausführst. Bevorzuge Befehle, die in [build-and-test.md](build-and-test.md)
beschrieben und durch die aktuellen Build-Dateien belegt sind. Übertrage keine
Connector-spezifischen Make-Ziele in diesen Core-Checkout.

## Auswirkungen von Befehlen erkennen

| Operation | Zu berücksichtigende Auswirkung | Quelle oder Richtlinie |
| --- | --- | --- |
| `./build.sh` | Entfernt Autotools-Zwischendateien, schreibt `src/headers.mk` neu und führt Generatoren aus. | [Implementierung der Build-Vorbereitung][bootstrap]. |
| Konfiguration und Kompilierung | Erzeugen Build-Dateien und -Ausgaben; ihre Pfade prüfen und nicht zugehörige Quelländerungen erhalten. | [Konfigurierte Ausgaben][outputs], [Speicherrichtlinie](resources-and-storage.md). |
| `git submodule update --init --recursive` | Kann Netzwerkzugriff benötigen und setzt verschachtelte Checkouts auf festgehaltene Revisionen; feststellen, ob die Aufgabe die Vorbereitung dieser Abhängigkeiten umfasst. | Die configure-Diagnostik empfiehlt diese Operation bei fehlenden eingebundenen Quellen: [Abhängigkeitsprüfungen][submodule-checks]. |
| Testaufruf | Kann Logs und Testdaten schreiben oder durch Testeingaben ausgewählten Code ausführen. Den jeweiligen Test und das Testprogramm prüfen. | [Testrichtlinie](testing-and-evidence.md), [Laufzeit-Vorprüfung](security-runtime-preflight.md). |
| Regressionsmodus `format` | Öffnet JSON-Testdateien zum Schreiben; ist eine Quellbearbeitung. | [Formatierungszweig][format]. |
| Installation, Push oder Veröffentlichung | Verändert einen Installationspräfix oder entfernten Zustand. Vor der Ausführung Ziel und Aufgabenumfang prüfen. | [Auslieferungsrichtlinie](delivery-and-ci.md). |

Eine nur lesende Aufgabe darf nicht stillschweigend Netzwerkzugriff erhalten.
Verwende bei der Untersuchung von Git-Objekten in einem partiellen Clone die
verfügbaren Git-Steueroptionen, die verzögertes Nachladen und optionale
Sperrdateischreibvorgänge verhindern, wenn eine Offline-Prüfung verlangt ist.
Fehlt ein erforderliches Objekt lokal, melde diese Lücke, statt es
stillschweigend nachzuladen oder seine Untersuchung zu behaupten.

## Shell und Eingaben behandeln

Behandle Shell-Text als Code. Setze Pfade und wörtliche Eingaben für die
tatsächliche Shell korrekt in Anführungszeichen; JSON-Zeichenketten-Escaping
ist kein Shell-Escaping. Bevorzuge strukturierte Werkzeugargumente oder eine
der Aufgabe zugeordnete Datei für mehrzeilige Eingaben. Werte Daten aus README,
Testnutzlast, Issue oder Log nicht als Befehl aus, ohne Zweck und Auswirkungen
zu prüfen.

Beschränke Umgebungsänderungen nach Möglichkeit auf den Prozess. Zweckentfremde
keine Systemverzeichnisvariablen und gib nicht die gesamte geerbte Umgebung aus.
Erfasse nur nicht geheime Werte, die zum Verständnis eines Laufs benötigt
werden, und halte Tokens, Cookies, private Anfragedaten und Zugangsdaten aus
Argumenten, Logs und versionierten Dateien heraus.

Unabhängige Lesezugriffe dürfen parallel laufen. Führe Befehle nacheinander aus,
wenn sie von erzeugten Dateien abhängen, gemeinsamen Zustand verändern oder ein
vorheriges Ergebnis benötigen. Für Hintergrundausführung gilt
[subagents-and-background.md](subagents-and-background.md).

## Ergebnisse

Halte tatsächlichen Befehl, Arbeitsverzeichnis, Revision/Konfiguration, Exitcode
und aussagekräftige Ausgabe fest. Berichte keinen vorgeschlagenen Befehl als
ausgeführt, verdecke keine fehlgeschlagene Prüfung durch einen Shell-Ersatzpfad
und bezeichne kein Teilergebnis als vollständig.

Benenne bei einer blockierten Operation das fehlende Werkzeug, die fehlende
Berechtigung, Eingabe oder Voraussetzung. Erledige nicht betroffene Arbeit
innerhalb des Aufgabenumfangs. Hole nur dann eine Entscheidung ein, wenn die
ungeklärte Frage die Aufgabe wesentlich verändert oder zusätzliche Befugnis
erfordert.

[bootstrap]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/build.sh#L1-L33
[outputs]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L366-L390
[submodule-checks]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L65-L107
[format]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/test/regression/regression.cc#L444-L468
