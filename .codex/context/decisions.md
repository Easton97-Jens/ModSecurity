# Entscheidungen

Diese Entscheidungen beschreiben die am 25.10.2026 geschaffene Arbeitsgrundlage.
Eine Historie aus früheren Gesprächen wird nicht vorausgesetzt. Künftige Einträge
sollen Datum, Entscheidung, Grund, betroffenen Stand und Folgen nennen.
Historische Sprach- und Bereitstellungsentscheidungen bleiben erkennbar; D012
beschreibt die ausdrücklich angeforderte deutsche Variante.

## D001 — Den offiziellen v3/master-Quellcode als technische Grundlage verwenden

**Entscheidung:** Die anfängliche Dokumentation beschreibt den offiziellen Commit
`effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729`.

**Grund:** Der Eigentümer verlangt Fakten aus dem offiziellen ModSecurity-v3-Quellcode.
Beim ersten Vergleich lag der Fork 22 Commits zurück. Eine spätere Prüfung vor dem
Öffnen des Pull Requests stellte fest, dass sein Zielbranch bereits mit der
dokumentierten Upstream-Grundlage übereinstimmte. Beide Beobachtungen stehen in
[provenance.md](provenance.md).

**Folge:** Bei einer späteren Quellenaktualisierung müssen Erklärungen und Verweise
erneut geprüft werden. Quellenvergleich und Dokumentationsänderungen sind von einer
tatsächlichen Aktualisierung des Zielbranches im Fork zu unterscheiden.

## D002 — Startanweisungen knapp halten und Fachwissen auslagern

**Entscheidung:** `AGENTS.md` definiert Arbeitsregeln und Lesereihenfolge.
Ausführliche Erklärungen liegen in `.codex/` und werden passend zur Aufgabe gelesen.

**Grund:** Wiederverwendbarer Kontext benötigt klare Navigation. Eine einzige lange
Startanweisung würde Arbeitsregeln und technische Details vermischen.
Der [Index](index.md) führt zu den passenden Kapiteln.

**Folge:** Neue Sitzungen lesen den aktuellen Stand und öffnen für Detailfragen erneut
den Code. Automatisches Laden aller Dokumente oder Erinnern jedes früheren Gesprächs
wird nicht versprochen.

## D003 — Die ursprünglichen Ergänzungen auf Englisch pflegen

**Entscheidung:** Dokumentation, Assistentenanweisungen, Prompts, Übergaben und Meldungen
der Prüfprogramme werden gemäß der damaligen ausdrücklichen Präzisierung des Eigentümers
auf Englisch gepflegt. Tatsächliche API-Namen, Pfade, Befehle, Branch-Bezeichner und
zitierte Quelldiagnosen behalten ihre ursprüngliche Schreibweise.

**Grund:** Der Eigentümer verlangte die gesamte Ergänzung auf Englisch.
Unveränderte Quellbezeichner halten Erklärungen direkt im Code durchsuchbar.

**Geltung:** Diese Entscheidung beschreibt die englische Grundlage. Für den später
beauftragten Branch `codex/kontext-v3-de` gilt die Sprachentscheidung D012.

## D004 — Quellnachweise von Empfehlungen unterscheiden

**Entscheidung:** ModSecurity-Erklärungen zitieren konkrete Quellstellen am
festgehaltenen Commit. Vorgeschlagene Abläufe und Lernwege sind als Projektkonventionen
gekennzeichnet. Bekannte Widersprüche stehen in [fact-check.md](fact-check.md).

**Grund:** Der Eigentümer verlangt ausdrücklich, dass Informationen nicht erfunden
oder ohne Prüfung behauptet werden. Eine plausible README-Zusammenfassung allein genügt nicht.

## D005 — Den Originalquellcode bei der Dokumentationsaufgabe erhalten

**Entscheidung:** Die Aufgabe ergänzt `AGENTS.md`, `CODEX_START.md` und `.codex/`.
Vorhandene Implementierung, Build-Definitionen, Tests und Originaldokumentation bleiben
gegenüber der festgehaltenen Grundlage unverändert.

**Grund:** Gefragt ist eine wiederverwendbare, verständliche Arbeitsgrundlage.
Änderungen am Produktverhalten erfordern eigene Anforderungen und Prüfungen.

**Folge:** Die strenge Kontextprüfung kontrolliert diese Grenze. Eine dokumentierte
Upstream-Abweichung wird dadurch nicht automatisch im Produkt behoben.

## D006 — Keine erfolgreichen Prüfungen erfinden

**Entscheidung:** Quellen-/Dokumentprüfungen und ModSecurity-Builds/-Tests werden
getrennt berichtet. Nur tatsächlich ausgeführte Befehle werden als ausgeführt festgehalten.
Nicht durchgeführte Prüfungen bleiben sichtbar.

**Grund:** Das Lesen von Quellcode belegt konkrete Code-Fakten, keine erfolgreiche
Ausführung auf einer späteren Zielplattform.

## D007 — Den bestehenden Fork verwenden

**Entscheidung:** Nach ausdrücklicher Präzisierung des Eigentümers gehören die Dateien
in `Easton97-Jens/ModSecurity`. Der damalige Arbeitsbranch ist `codex/kontext-v3`,
mit einem Pull Request gegen `v3/master` dieses Forks.

**Grund:** Der Eigentümer möchte den vorhandenen Fork als Arbeitsort. Ein erneuter
Vergleich vor dem Pull Request zeigte, dass dessen `v3/master` bereits zur dokumentierten
offiziellen Grundlage passte. Der Pull Request enthält deshalb nur Kontextergänzungen;
der frühere Rückstand bleibt als historische Beobachtung erhalten.

**Folge:** Vor einer Integration nach `v3/master` wird auf dem Kontextbranch gelesen
und gearbeitet. Mergen ist eine gesonderte Aktion. Tatsächliche Branch- und PR-Stände
stehen in [current-state.md](current-state.md). D012 ergänzt den später beauftragten
deutschen Branch ohne neuen Pull Request.

## D008 — Den gewählten Connector-Kontextaufbau anpassen

**Datum:** 25.10.2026.

**Entscheidung:** Folge der am Connector-Vorlagen-Commit
`42fd5a8e895aca3a36b09eb09fe5c7cf9c3399e3` untersuchten Struktur mit `context/`,
zugeordneten Richtlinien und Profilen, Sicherheitsprüfungen, `bin/`, `audit/` und
ausdrücklichen Manifesten. Ordne ausführliche Kernleitfäden und dauerhaften Sitzungskontext dort ein.

**Grund:** Der Eigentümer wählte die bestehende Connector-Einrichtung ausdrücklich
als Strukturvorlage. Kernkomponenten ersetzen Connector-Host-Unterschiede. Keine
Framework-/MRTS-Topologie, Maschinenpfade, verpflichtenden nicht verfügbaren Wrapper,
historischen Connector-Ergebnisse oder Zweisprachigkeitsvorgaben werden übernommen.

**Folge:** Der [Anpassungsnachweis](template-adaptation.md) bezeichnet Umfang und Änderungen.
Der lokale Katalog prüft Prüfsummen der primären Richtlinien; er aktiviert keine
Richtlinienvererbung in Submodulen. ModSecurity-Produktfakten stammen weiterhin
ausschließlich aus der festgehaltenen offiziellen Kernquelle.

## D009 — Die Arbeitsvereinbarung für beliebige Repositories wiederverwendbar machen

**Datum:** 25.10.2026.

**Entscheidung:** Halte `AGENTS.md` unabhängig von einem bestimmten Projekt, Konto,
Branch, einer Implementierungssprache, einem Build-System oder installierten Hilfsprogramm.
Ermittle vorhandenen Projektkontext bedingt. Lege konkrete Vorgaben und den Startablauf
dieses Forks in [repository-context.md](repository-context.md) ab, auf den der Index verweist.
Die `.codex`-Leitfäden und Prüfprogramme bleiben auf diesen Kern zugeschnitten.

**Grund:** Der Eigentümer stellte klar, dass die Vereinbarung für alle Repositories
gelten soll. Eine wiederverwendbare Hauptanweisung muss auch ohne diesen `.codex`-Aufbau,
Python-Hilfsprogramme oder Git-Checkout verwendbar sein.

**Folge:** Dieselbe Hauptdatei kann andernorts unverändert wiederverwendet werden.
Diese Änderung verteilt sie nicht an andere Repositories und installiert keine globalen
Anweisungen. Projektspezifische Quellenautorität und Prüfungen bleiben im lokalen Kontext
erhalten, statt diese Annahmen in der portablen Vereinbarung festzuschreiben.

## D010 — Die Chatsprache jeder Person überlassen

**Datum:** 25.10.2026.

**Entscheidung:** In der ursprünglichen Variante bleiben gepflegte Repository-Dateien
Englisch. Arbeitsvereinbarung, lokaler Kontext und Startprompts schreiben keine Chatsprache
vor. Jede Person entscheidet selbst über die Sprache ihres Gesprächs.

**Grund:** Der Eigentümer stellte ausdrücklich klar, dass die Chatsprache eine individuelle
Wahl ist. Eine Sprache, die in einem Gespräch gewünscht wird, darf nicht zum Standard
für das gesamte Repository werden. D003 galt für die ursprünglichen gepflegten Dateien;
D012 legt ausschließlich die Dateisprache der deutschen Variante fest.

## D011 — Optionale RTK-Zusammenfassungen mit offizieller Codex-Anleitung verwenden

**Datum:** 25.10.2026.

**Entscheidung:** Bevorzuge unterstützte RTK-Befehle für routinemäßige Zusammenfassungen,
wenn das Werkzeug verfügbar ist, mit Rückgriff auf native Werkzeuge. Halte die Hauptregel
portabel und verweise für quellengeprüfte Beispiele und Einrichtungsgrenzen auf
[rtk.md](rtk.md). Codex-Mechanismen folgen offizieller OpenAI-Dokumentation; RTK-Befehle
folgen dem untersuchten RTK-Quellstand `v1.0.1` an
`4ef8730cf3a0dea11c8d64a9b90bc0616fe52100`.

**Grund:** Der Eigentümer wünschte RTK zur Reduzierung des Tokenverbrauchs und verlangte
ausdrücklich die Einhaltung offizieller Codex-Dokumentation. Weniger routinemäßige
Ausgabebytes zu lesen ist hilfreich; gefilterte Zusammenfassungen ersetzen jedoch keine genauen Nachweise.

**Folge:** Quellenuntersuchung, vollständige Diffs, Diagnosen, maschinenlesbare Daten
und abschließende Validierung verwenden geeignete ungefilterte Ausgaben.
ModSecurity-Build-/Testbefehle und die Implementierung der Kontextprüfer blieben bei
dieser RTK-Ergänzung unverändert. Die Aufgabe ergänzt Hinweise, ohne RTK zu installieren,
seinen Initialisierer auszuführen, Hooks/Proxys zu aktivieren oder persönliche
Codex-Einstellungen zu ändern. Tatsächliche Token- oder Abrechnungseinsparungen werden nicht behauptet.

## D012 — Eine vollständige deutsche Variante in einem eigenen Branch bereitstellen

**Datum:** 25.10.2026.

**Entscheidung:** Übersetze die Kontextdateien aus `codex/kontext-v3`, Commit
`a35e8c3a1116a763f286f965f718f36d5f8876cc`, und lade sie im bestehenden Fork
`Easton97-Jens/ModSecurity` unter `codex/kontext-v3-de` hoch. Erstelle dafür keinen
Pull Request. Die englische Fassung wird nicht verändert.

**Grund:** Der Eigentümer präzisierte ausdrücklich, dass die tatsächlichen Branch-Dateien
und nicht bloß der PR-Text übersetzt werden sollen.

**Folge:** Die ergänzten Anweisungen, Leitfäden, Tabellen, Prompts, Übergaben und eigenen
Prüfmeldungen dieser Variante sind Deutsch. Dateinamen, technische Bezeichner, Quellcode,
CLI-Optionen, Quellen-URLs, Git-IDs und maschinenlesbare Schlüssel bleiben erhalten.
Aktuelle Navigationsbefehle und Branch-Angaben verwenden den deutschen Branch; historische
Angaben zur englischen Arbeit bleiben als solche erkennbar. Das Quelleninventar und die
Produktdateien werden nicht übersetzt. Richtlinien-Prüfsummen werden nach Prüfung erneuert.
Die Chatsprache bleibt gemäß D010 frei wählbar. Upload ist beauftragt, PR und Merge sind es nicht.
