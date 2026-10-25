# Ressourcen und Speicher

## Geltungsbereich

Diese Richtlinie regelt Build-, Analyse- und temporäre Daten, die zur Aufgabe
gehören. Sie setzt weder ein bestimmtes Rechnerverzeichnis noch eine
exportierte Umgebungsvariable, einen Speicherbudget-Dienst oder ein
numerisches Kontingent voraus. Ermittle die tatsächlichen Berechtigungen und
Kapazitäten des Arbeitsbereichs, wenn die Aufgabe erhebliche Ressourcen
benötigt.

Halte gepflegten Quellcode und Dokumentation im Repository. Lege vorübergehende
Downloads, umfangreiche Logs, Kompilierungsdatenbanken, Scanner-Ausgaben und
Laufzeitdaten an einem geeigneten, der Aufgabe zugeordneten Ort ab, den die
aktive Umgebung unterstützt. Halte Ort und Aufbewahrungsbedarf fest, ohne
rechnerspezifische Pfade in die allgemeinen Anweisungen des Repositorys
aufzunehmen.

## Die tatsächliche Build-Struktur beachten

Die versionierte `.gitignore` führt übliche Autotools-Ausgaben, Bibliotheken,
Testprogramme und das Windows-Build-Verzeichnis auf. `build.sh` schreibt
`src/headers.mk` neu und erzeugt beziehungsweise entfernt Build-Zwischendateien.
Diese konkreten Auswirkungen innerhalb des Checkouts sind bei der
Build-Vorbereitung zu berücksichtigen.
Quellen: [ignorierte Build-Ergebnisse][ignore],
[Operationen der Build-Vorbereitung][bootstrap].

Ein ignorierter Pfad beweist nicht, dass sein Inhalt entbehrlich ist oder zur
Aufgabe gehört. Prüfe ihn vor dem Aufräumen. Behaupte keine Unterstützung eines
beliebig verlagerten Build-Verzeichnisses, ohne die tatsächlichen
Build-Anweisungen und Pfade zu prüfen. Verwende bei sinnvoller Isolation einen
aufgabeneigenen Checkout oder Worktree, der die unterstützte Quellstruktur
erhält.

Beachte für Installationspräfixe [build-and-test.md](build-and-test.md).
Vermeide systemweite Installationen bei gewöhnlicher Quelluntersuchung, wenn
ein lokaler Build oder ein beschreibbarer aufgabenspezifischer Präfix die
Aufgabe erfüllt.

## Kapazität und Parallelität

Prüfe vor einem großen Build, Download oder einer Laufzeitbewertung freien
Plattenplatz und verfügbaren Arbeitsspeicher. Schätze Ausgabeumfang und
aufzubewahrende Belege ab, wähle eine vernünftige Anzahl paralleler Jobs und
vermeide Überlastung von Speicher oder Datenträger. Nutze nicht jede CPU allein
deshalb, weil sie verfügbar ist, und setze kein globales `MAKEFLAGS`, das
andere Arbeiten verändert.

Compilerparallelität, mehrere Builds, Hintergrundprozesse und
Assistentenagenten beanspruchen unterschiedliche Ressourcen. Berücksichtige
ihre Gesamtlast. Überschreitet die Arbeit die verfügbare Kapazität, reduziere
die Parallelität oder beende sie geordnet und erhalte dabei nützliche Ergebnisse.

Führe unabhängige Jobs nur parallel aus, wenn ihre Build-Dateien, erzeugte
Konfiguration, exklusiv benötigte Caches, temporäre Dateien, Logs und sonstige
veränderliche Ausgaben nicht kollidieren. Denselben Checkout für einen Build
neu zu konfigurieren, während ein anderer ihn kompiliert, ist keine
unabhängige Arbeit.

Lösche keine Dateien einer anderen Aufgabe, verlagere Daten nicht
stillschweigend in ein begrenztes Dateisystem und installiere keine Werkzeuge
global, um eine Ressourcenbeschränkung zu umgehen.

## Submodulgrenzen

Das Core-Repository deklariert `others/libinjection`, `others/mbedtls`,
`test/test-cases/secrules-language-tests` und `bindings/python` als Submodule.
Behandle ihre Arbeitsverzeichnisse, Commits und erzeugten Daten als getrennte
Zuständigkeitsbereiche. Die mbedTLS-Voraussetzungsprüfung verweist auf den
verschachtelten Pfad `tf-psa-crypto/utilities/base64.c`.
Quellen: [Submoduldeklarationen][submodules],
[Abhängigkeitsprüfungen][dependencies].

Ersetze diese tatsächlichen Abhängigkeiten nicht durch die Framework-/MRTS-Struktur
oder Speichervariablen des Connector-Referenzrepositorys. Initialisierung und
Aktualisierung von Abhängigkeiten richten sich nach dem Aufgabenumfang;
eine Ressourcenrichtlinie erlaubt weder eine Gitlink-Änderung noch eine nicht
beauftragte Aktualisierung von Abhängigkeiten.

## Aufbewahren und aufräumen

Halte fest, welche temporären Verzeichnisse, Prozesse und Belege zu dieser
Aufgabe gehören. Bewahre ausreichend Belege auf, um das Ergebnis zu erklären,
und entferne nur nicht mehr benötigte Daten mit klarer Zugehörigkeit.
Für Worktrees und den Repository-Zustand gilt
[cleanup-and-restoration.md](cleanup-and-restoration.md). Ist ein
aufbewahrtes Artefakt in der nächsten Sitzung nicht verfügbar, benenne das,
statt einen lokalen Pfad als dauerhaft gemeinsamen Beleg zu behandeln.

[ignore]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/.gitignore
[bootstrap]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/build.sh#L1-L33
[submodules]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/.gitmodules#L1-L12
[dependencies]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/configure.ac#L65-L107
