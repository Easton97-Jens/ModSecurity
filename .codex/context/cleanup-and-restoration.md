# Aufräumen und Arbeitsbereich wiederherstellen

## Zugehörigkeit beachten

Diese Arbeitsrichtlinie gilt für das Aufräumen nach einer abgegrenzten Aufgabe.
Worktree, lokaler Branch, Remote-Branch, PR, externer Clone, Submodul-Checkout,
Gitlink und GitHub-Repository sind unterschiedliche Objekte. Die Erlaubnis,
eines davon zu verändern, erlaubt nicht das Löschen oder Ersetzen eines
anderen.

Erhalte den Hauptcheckout, vorhandene Benutzerarbeit, Änderungen anderer
Agenten und unbekannte Dateien. Ein ignorierter Dateiname oder ein Pfad namens
`tmp` reicht nicht aus, um dessen Zugehörigkeit zur Aufgabe festzustellen.
Verwende keine pauschale Löschung, `git clean`, keinen Hard-Reset, erzwungenen
Checkout oder automatischen Stash als gewöhnliche Aufräummaßnahme.

## Temporäre Daten und erzeugte Dateien

Führe ein zur Arbeit passendes Verzeichnis der temporären Testdatenpfade,
Build- oder Laufzeitverzeichnisse, Prozessreferenzen, aufbewahrten Logs und
gegebenenfalls für die Aufgabe erstellten Worktrees. Entferne nur eindeutig
zugeordnete Daten, nachdem Aufbewahrungsbedarf und aktive Nutzung geklärt sind.

Der Quellbaum enthält erzeugte und ignorierte Build-Ergebnisse; auch das
Build-Aufräumen hat jedoch konkrete Semantik. Das Makefile im Stammverzeichnis
führt maintainer-clean-Dateien auf; `CLEANFILES` des Parser-Makefiles enthält
erzeugte Scanner-/Parserdateien. Prüfe das jeweilige Ziel vor der Verwendung,
besonders wenn Parseränderungen oder erzeugte Ausgaben zur Aufgabe gehören.
Quellen: [ignorierte Dateien][ignore], [maintainer-clean-Liste][maintainer],
[Parser-Bereinigungsliste][parser-clean].

Lösche erzeugte Ausgaben nicht allein deshalb, weil sie normalerweise neu
erzeugt werden können, wenn sie ungeprüfte Arbeit enthalten oder der
benötigte Generator nicht verfügbar ist. Lass sie bestehen und melde den
Grund, wenn eine sichere Entfernung nicht festgestellt werden kann.

## Worktrees und Branches

Prüfe vor dem Entfernen eines aufgabeneigenen Worktrees dessen genauen Pfad
und Repository, versionierte, vorgemerkte und nicht versionierte Änderungen,
einzigartige oder unveröffentlichte Arbeit, aktive Prozesse sowie die
Abhängigkeit von Belegen oder anderen Aufgaben. Verwende den normalen
Git-Worktree-Mechanismus nur, wenn die Entfernung sicher ist und zum Auftrag
gehört. Lösche einen registrierten Worktree nicht manuell und erzwinge keine
Entfernung, um ungeklärten Zustand zu umgehen.

Ein lokaler Branch darf nur gelöscht werden, wenn er nicht mehr benötigt wird,
keine erforderliche einzigartige Arbeit enthält und nicht von einem anderen
Worktree verwendet wird. Bevorzuge sicheres Löschen ohne Erzwingen. Für einen
Remote-Branch oder PR braucht es eine eigene Festlegung im Auftrag; der bloße
Abschluss lokaler Bearbeitung erlaubt keine entfernte Löschung.

## Nur einen tatsächlich angeforderten Zustand wiederherstellen

Wechsle nicht allein deshalb zu einem Branch namens `master` oder `v3/master`,
damit der Arbeitsbereich abgeschlossen wirkt. Erhalte den für die Aufgabe
gewählten Branch, sofern Wiederherstellung oder Integration nicht zur Anfrage
gehören.

Ist die Rückkehr zu einem anderen Branch autorisiert, prüfe zuerst das
tatsächliche Stammverzeichnis, Branch, HEAD, Status und beabsichtigte Remote.
Wechsle ohne Erzwingen, erhalte nicht committete Arbeit und prüfe den
resultierenden Zustand. Eine Remote-Aktualisierung ist eine gesonderte
Netzwerkaktion; verwende sie nur innerhalb des Auftrags und vermeide das
Umschreiben nicht zugehöriger Historie.

## Submodule wiederherstellen

Das Core-Repository legt vier deklarierte Submodulpfade fest. Behandle Gitlink
und verschachtelten Checkout getrennt; ein verschachtelter Detached HEAD kann
einfach die festgehaltene Abhängigkeitsrevision abbilden. Wechsle
Abhängigkeiten beim Aufräumen nicht auf deren eigene Standardbranches und
aktualisiere sie nicht auf neuere Commits.
Quelle: [Submoduldeklarationen][submodules].

Hat die Aufgabenbearbeitung einen verschachtelten Checkout verändert, prüfe
dessen eigenen Status und erhalte erforderliche Arbeit, bevor du ihn auf die
vom übergeordneten Repository festgehaltene Revision zurücksetzt.
Die Initialisierung von Abhängigkeiten kann Netzwerkzugriff benötigen und
darf nicht in einem vermeintlich offline ausgeführten Aufräumschritt
versteckt werden.

Berichte zum Abschluss über verbliebenen temporären Zustand, noch nicht
abgeschlossene Prozessbereinigung oder blockierte Wiederherstellung mit dem
konkreten Grund. Für Prozesse gilt
[subagents-and-background.md](subagents-and-background.md), für Speicher
[resources-and-storage.md](resources-and-storage.md).

[ignore]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/.gitignore
[maintainer]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/Makefile.am#L26-L51
[parser-clean]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/src/parser/Makefile.am#L35-L43
[submodules]: https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/.gitmodules#L1-L12
