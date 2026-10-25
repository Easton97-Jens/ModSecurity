# 25.10.2026 — Englische Kontextgrundlage und Vorlagenanpassung

Dies ist die deutsche Übersetzung der historischen Aufzeichnung zur englischen
Grundlagenarbeit. Die unten genannten Stände und Prüfungen gehören zu dieser
früheren Arbeit; der aktuelle deutsche Branch wird in
[current-state.md](../../context/current-state.md) beschrieben.

## Auftrag und Umfang

`AGENTS.md` und ausführlichen wiederverwendbaren Kontext im bestehenden Fork
`Easton97-Jens/ModSecurity` erstellen. Die damalige Vorgabe war durchgängig Englisch.
ModSecurity-Erklärungen auf den offiziellen `v3/master`-Code stützen. Die `.codex`-Organisation
aus `Easton97-Jens/ModSecurity-conector:master` anpassen, damit spätere Chats anhand
gespeicherter Fakten, Entscheidungen und tatsächlich erfolgter Prüfungen fortsetzen können.

Quellgrundlage: `effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729`.
Vorlagengrundlage: `42fd5a8e895aca3a36b09eb09fe5c7cf9c3399e3`.
Die [Quellenherkunft](../../context/provenance.md) und der
[Anpassungsnachweis](../../context/template-adaptation.md) unterscheiden ihre Rollen.

## Bereitgestellte Inhalte und Review

Die Ergänzungen behandeln Repository-Übersicht und Sprachen, Architektur, C++17/C-API,
Besitzverhältnisse und Callbacks, Phasen und Interventionen, Konfiguration/Regeln/Funktionen,
Builds/Tests, sachliche Einschränkungen und Upstream-Pflege. Zugeordnete Richtlinien und
Profile folgen der Vorlagenorganisation mit tatsächlichen Kernkomponenten.
Aktueller Stand, Entscheidungen, Backlog, Prompts und dieser Sitzungsmechanismus schaffen
ausdrücklich gespeicherte Kontinuität. Automatisches Gedächtnis oder automatische Vererbung wird nicht behauptet.

Die technischen Kapitel wurden mit gezielten Quellenprüfungen unabhängig gegengeprüft.
In den endgültigen Leitfäden erhaltene Korrekturen betreffen unter anderem Prüfbedingungen
bei deaktivierter Engine, bedingt gebaute YAJL-Testprogramme, veraltete Beispiele und
die Tatsache, dass ein erfolgreicher Methodenaufruf nicht dasselbe ist wie eine Intervention
oder ein Connector-Ergebnis. Eine veraltete aktuelle Angabe von 25 Dateien wurde nach
Erweiterung der Struktur entfernt.

Alle 72 versionierten `.codex`-Referenzdateien wurden in der parallelen Prüfung gelesen,
zusammen mit den Anweisungen, der README, den Ignore-Regeln und dem relevanten Makefile-Inventar
im Hauptverzeichnis. Das ist keine Prüfung aller 1.714 Einträge des Referenzrepositorys
oder seiner externen Module. Die Submodulinhalte der Hauptbibliothek wurden nicht
umfassend geprüft; ihre genauen Gitlink-Bindungen stehen im Inventar.

## Prüfungen und Grenzen der Nachweise

Folgende Prüfungen waren in der endgültigen lokalen Anpassung erfolgreich:

```sh
python3 .codex/bin/verify-context.py --strict-source
python3 .codex/bin/validate-structure.py
python3 .codex/bin/validate-codex-inheritance.py --check
```

Jede lieferte Exit-Code 0. Der Quellenprüfer prüfte 850 Hauptbaumeinträge und 487
eindeutige, an einen Commit gebundene ModSecurity-Links. Der Strukturprüfer prüfte
93 Zuordnungen; der Katalogprüfer 23 Prüfsummen primärer Richtlinien. Python war 3.12.14.
Gebundene Quellenverweise und Zeilenbereiche der Connector-Vorlage wurden separat
gegen lokale Git-Objekte der Referenz geprüft.

Unabhängige Proben mit temporären Testdaten belegten:

- Fehlende lokale Links, ungültige Quellbereiche und Links außerhalb des Hauptverzeichnisses schlagen fehl.
- Veraltete Katalogprüfsummen und doppelte tatsächliche IDs schlagen fehl.
- Kollisionen abgeleiteter IDs schlagen fehl, bevor `--refresh` einen Katalog schreibt.
- Katalog-Symlinks schlagen fehl, während eine externe Kontroll-Datei erhalten bleibt.
- Historische Quellverweise werden an den neuen Entscheidungs-/Sitzungsorten akzeptiert
  und in aktiven Leitfäden sowie am veralteten Sitzungsort abgewiesen.
- Eingezäunte Codebeispiele erzeugen keine falschen Markdown-Verweisfehler.
- Eine neue datierte Sitzungsaufzeichnung besteht ohne Aufnahme in die normale Richtlinienzuordnung.

Die temporären Proben blieben außerhalb des ausgelieferten Baums. Die drei im Review
erkannten Hilfsprogrammfehler wurden korrigiert und die zugehörigen Fälle erneut ausgeführt.
Kein C++-Build, keine Produkttestsuite, kein Sicherheitsscan und keine Connector-Laufzeit
wurden ausgeführt. Eine quellenverknüpfte Erklärung und strukturelle Prüfungen sind nicht diese Ausführungsergebnisse.

## Repository-Grenze und Fortsetzung

Der ursprüngliche Hauptquellbaum und alle Gitlinks blieben gegenüber der festgehaltenen
Grundlage unverändert. Nur `AGENTS.md`, `CODEX_START.md` und `.codex/` wurden ergänzt.
Der damalige Bereitstellungsbranch war `codex/kontext-v3` im bestehenden Fork.
[Entwurfs-PR #120](https://github.com/Easton97-Jens/ModSecurity/pull/120) war gegen
`v3/master` geöffnet und nicht gemergt. Die veröffentlichte Anpassung an
`bad6cd355fd78ffcfa6393b1c3ada8d734feb098` wurde aus dem Fork geklont; alle drei
Prüfer liefen dort erfolgreich. Ihr Baum `9a884e6e02525a469bfa531e8aaa767cf915e856`
entsprach dem geprüften lokalen Baum. Der beobachtete Vergleich enthielt 98 ergänzte
Kontextdateien und keine Produktquelländerungen. Die Abfrage lieferte für diesen Commit
keine PR-ausgelösten Workflow-Läufe; dies wurde nicht als positive CI-Prüfung berichtet.
Spätere reine Kontextaktualisierungen sind im [aktuellen Stand](../../context/current-state.md)
und im tatsächlichen Head des englischen PR nachvollziehbar.

Beginne einen späteren Chat mit dem [Startprompt](../../context/prompts/start-new-chat.md).
Lies die tatsächlichen Quellen und ausgewählten Richtlinien für die jeweilige Aufgabe.
Aktualisiere den gespeicherten Stand mit tatsächlich erhaltenen Ergebnissen; leite aus
dieser Übergabe keinen abgeschlossenen Build oder Merge ab.
