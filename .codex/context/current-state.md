# Aktueller Arbeitsstand

Festgehalten am **25.10.2026** (Europe/Berlin). Prüfe bei Wiederaufnahme den tatsächlichen
Branch, Commit und Remote-Zustand erneut; Branch-Namen und PR-Status können sich ändern.

## Aktueller Auftrag: deutsche Variante

Übersetze die Kontextdateien aus **`Easton97-Jens/ModSecurity:codex/kontext-v3`**,
Commit `a35e8c3a1116a763f286f965f718f36d5f8876cc`, vollständig ins Deutsche und lade
sie im selben bestehenden Fork unter **`codex/kontext-v3-de`** hoch.
**Keinen Pull Request erstellen und nicht mergen.** Der englische Branch bleibt erhalten.

Die Variante umfasst die wiederverwendbare `AGENTS.md`, `CODEX_START.md` und `.codex/`
mit technischen Leitfäden, Arbeitsrichtlinien, optionalen Prüfprofilen,
Komponenten-/Prüfleitfäden, Manifesten, Prüfprogrammen, Prompts und Übergaben.
Die Dateien dieser Variante werden auf Deutsch gepflegt. Eine Chatsprache wird nicht
vorgeschrieben; jede Person entscheidet selbst. Konkrete ModSecurity-Vorgaben und
Startprüfungen stehen in [repository-context.md](repository-context.md); die
Arbeitsvereinbarung im Hauptverzeichnis bleibt über Repositories hinweg wiederverwendbar.

Der [Index](index.md) ist der Einstiegspunkt. Der [Anpassungsnachweis](template-adaptation.md)
erklärt die Vorlagenzuordnung und ausgelassene referenzspezifische Annahmen.
Technische Bezeichner, Quellenverweise, Quellcode und maschinenlesbare Schlüssel bleiben
erhalten. Aktuelle Branch-Verweise und Navigationsbefehle nennen den deutschen Branch;
historische Angaben zur englischen Fassung bleiben als solche gekennzeichnet.

## Inhalt und Ursprung der englischen Grundlage

Die ursprüngliche Aufgabe ergänzte englische `AGENTS.md`- und ausführliche `.codex`-Dateien
im bestehenden Fork. Sie erklären Repository, zentrale Funktionen, C++-/C-Schnittstellen,
Konfiguration, Builds und Tests anhand offizieller v3-Quellen. Der Aufbau folgt dem vom
Eigentümer gewählten Connector-Repository und ist auf die Kernbibliothek angepasst.

Auf Wunsch des Eigentümers dokumentiert [rtk.md](rtk.md) optionale RTK-Ausgabezusammenfassungen.
Die wiederverwendbare Hauptanweisung bevorzugt unterstützte Zusammenfassungen bei verfügbarem
RTK und native Ausgaben für genaue Nachweise und abschließende Prüfungen. Offizielle
OpenAI-Dokumentation bestimmt die Codex-Integration; festgehaltene RTK-Quellen bestimmen
die Beispiele. Diese Ergänzung installiert RTK nicht, führt dessen Initialisierer nicht aus,
aktiviert keine Hooks/Proxys und verändert keine persönlichen Einstellungen.
Token- oder Abrechnungseinsparungen zur Laufzeit wurden nicht gemessen.

## Überprüfte Quellgrenzen

| Gegenstand | Festgehaltener Stand oder Umfang |
| --- | --- |
| Offizielle Produktquelle | `owasp-modsecurity/ModSecurity`, `v3/master`, `effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729` |
| Fork-Ziel, letzter festgehaltener Vergleich der Grundlagenarbeit | `Easton97-Jens/ModSecurity:v3/master`, derselbe Commit `effb6c0...` |
| Strukturvorlage | `Easton97-Jens/ModSecurity-conector:master`, `42fd5a8e895aca3a36b09eb09fe5c7cf9c3399e3` |
| Hauptquelleninventar | 850 Einträge: 846 Blobs und 4 Gitlinks |
| Referenzinventar | 72 versionierte `.codex`-Dateien sowie `AGENTS.md`, `README.md`, `.gitignore` und `Makefile` im Hauptverzeichnis |
| Optionale RTK-Referenz | `rtk-ai/rtk`, Tag `v1.0.1`, Commit `4ef8730cf3a0dea11c8d64a9b90bc0616fe52100`; von ModSecurity-Quellen getrennt |
| Referenz der Codex-Integration | Offizielle OpenAI-Dokumentation zu AGENTS.md und Hooks, am 25.10.2026 gelesen; siehe [Quellenherkunft](provenance.md) |
| Englische Übersetzungsgrundlage | Commit `a35e8c3a1116a763f286f965f718f36d5f8876cc`, Baum `05186c8c673a1a86ff35abe4f414376c22704ab5` |
| Produktänderungen | Keine außerhalb von `AGENTS.md`, `CODEX_START.md` und `.codex/`; durch strenge Kontextprüfung bestätigt |

Die erste Fork-Beobachtung lag 22 Commits hinter Upstream; die spätere Zielbeobachtung
entsprach der Grundlage. Das sind separat erhaltene historische Fakten und kein
Produktunterschied von 22 Commits bei der Kontextbereitstellung. Siehe
[provenance.md](provenance.md) und [upstream-lock.json](upstream-lock.json).

## Historische Prüfungen der englischen Ausgangsversion

Die folgenden Ergebnisse wurden vor der Übersetzung festgehalten. Sie sind keine
Behauptung, dass diese Prüfungen für die deutsche Fassung erneut ausgeführt wurden.
Die damaligen Befehle liefen vom Repository-Hauptverzeichnis aus mit Python 3.12.14
und Git mit Unterstützung für `--no-lazy-fetch`. Dokumentzahlen werden vom Prüfprogramm
gemeldet, weil spätere Übergaben Markdown-Dateien und Links ergänzen können.

| Prüfung | Damals beobachtetes Ergebnis |
| --- | --- |
| `python3 .codex/bin/verify-context.py --strict-source` | Exit 0; festgehaltener Baum, vollständiges Quelleninventar, lokale Dokumentziele, 487 eindeutige gebundene ModSecurity-Quellverweise und Zeilenbereiche sowie unveränderte Produktquellgrenze |
| `python3 .codex/bin/validate-structure.py` | Exit 0; 95 zugeordnete Dateien, erforderlicher Aufbau, Identität und Manifestkonsistenz |
| `python3 .codex/bin/validate-codex-inheritance.py --check` | Exit 0; 23 lokale Richtlinien-Prüfsummen, keine aktive Vererbung an untergeordnete Repositories |
| `git diff --check` | Exit 0; keine Whitespace-Fehler in der RTK-Dokumentationsergänzung |
| RTK-Quellverweisprüfung | Tag ergab den gespeicherten Commit; 19 zitierte Quellpfade/Zeilenbereiche gegen den lokalen Referenzklon geprüft, getrennt vom ModSecurity-Prüfer |
| Prüfung gebundener Vorlagenverweise | Gültige lokale Git-Objekte und Zeilenbereiche am Referenz-Commit |
| Unabhängiges Dokumentations-/Quellenreview | Grundlagen- und Portabilitätsreviews abgeschlossen; RTK-Beispiele/Quellenzuordnung und offizielle Codex-Dokumentation geprüft. Der getrennte Geltungsbereich von Proxy-Statistiken wurde nach Review präzisiert. |
| Unabhängige Hilfsprogrammproben in isolierter Kopie | Fehlende/ungültige/außerhalb liegende Links, veraltete Prüfsummen, doppelte IDs und ungültige Pfade abgewiesen; historische Nachweise und Codebeispiele wie dokumentiert behandelt |
| Nachprüfung der Hilfsprogrammkorrekturen | Datierte Sitzungsaufzeichnungen benötigen keine aktive Zuordnung; kollidierende abgeleitete Richtlinien-IDs werden vor Schreibzugriffen abgewiesen; Katalog-Symlinks werden ohne Änderung einer externen Kontroll-Datei abgewiesen |

Daraus folgt keine Garantie semantischer Vollständigkeit oder Sicherheit.
Die [Grundlagenübergabe](../audit/sessions/2026-10-25-context-foundation.md) hält
Umfang und damalige Prüfdetails fest.

## Prüfstand der deutschen Variante

Die deutsche Variante wurde gegen den englischen Ausgangscommit abgeglichen.
Alle 90 Markdown-Dokumente wurden übersetzt. Insgesamt wurden 98 der 100 Kontextdateien
geändert; die beiden maschinenlesbaren Quelleninventare sind bytegleich erhalten.
Die Prüfungen liefen vom Repository-Hauptverzeichnis mit Python 3.12.14 aus.

| Frische Prüfung | Tatsächliches Ergebnis |
| --- | --- |
| `python3 .codex/bin/verify-context.py --strict-source` | Exit 0; 850 Quelleneinträge, 90 Markdown-Dateien, 499 lokale Linkziele, 487 eindeutige gebundene ModSecurity-Verweise; keine Produktquellabweichungen |
| `python3 .codex/bin/validate-structure.py` | Exit 0; 95 zugeordnete Dateien konsistent |
| `python3 .codex/bin/validate-codex-inheritance.py --refresh` und anschließend `--check` | Jeweils Exit 0; 23 Prüfsummen bewusst erneuert und geprüft, keine aktive Vererbung |
| `git diff --check` | Exit 0; keine Whitespace-Fehler |
| Übersetzungsabgleich | Alle 789 Vorkommen von 509 unterschiedlichen gebundenen ModSecurity-, RTK- und Vorlagenquelllinks erhalten; ausführbare Beispiele unverändert bis auf die beauftragte Branch-Navigation |
| Python-/Manifestprüfung | Alle drei Prüfprogramme syntaktisch gültig und mit unveränderter AST-Struktur außerhalb von Textliteralen; unabhängig überprüft: nur Docstrings, Hilfe, eigene Diagnosen und generierte Kommentare geändert. TOML-Daten außer Prüfsummen unverändert; JSON-Erklärungen übersetzt und Herkunft der Übersetzung ergänzt. |
| Unabhängige sprachliche Gegenprüfung | Alle 90 Markdown-Dateien gegen Englisch gelesen. Ein Schreibfehler und die Übersetzung von „signedness change“ wurden korrigiert; keine offenen Befunde. |

Die Quelleninventare, Gitlinks, technischen Bezeichner und Schnittstellen bleiben erhalten.
Die Prüfungen belegen Übersetzungs- und Kontextkonsistenz; sie ersetzen keine erneute
vollständige Untersuchung aller Produktimplementierungen oder einen Laufzeittest.

**Nicht ausgeführt:** C++-Kompilierung, ModSecurity-Unit-/Regressionstests, Windows-Build,
Parser-Neugenerierung, Bibliotheksinstallation, Connector-Integration und Sicherheitsscan.
Die Aufgabe betrifft Kontext und lokale Dokumentationshilfen. Submodulimplementierungen
wurden nicht initialisiert oder umfassend geprüft. In der englischen Grundlagenarbeit
waren RTK- und Codex-Programme in der Aufgaben-Umgebung nicht verfügbar; Beispielbefehle,
Initialisierer, Hook-Verhalten und tatsächliche Einsparungen wurden dort nicht getestet.
Die deutsche Übersetzung führt diese Integrationen ebenfalls nicht aus. Quellen- und
Dokumentationslektüre beweist keine Laufzeitintegration.

## Bereitstellungsstand

Die deutsche Variante ist im bestehenden öffentlichen Fork unter `codex/kontext-v3-de`
hochgeladen. Der erste veröffentlichte Übersetzungs-Commit ist
`b3db8941fada504caf3b88da9357da307c6c8132` mit Baum
`8188488bd10cb5fa4e26e4f5a5740d52a2fd3ce8`. Dieser Baum stimmt exakt mit dem lokal
geprüften Dateibaum überein. Der Branch wurde aus dem Remote abgerufen, sein Commit,
Eltern-Commit und Baum wurden geprüft. Der GitHub-Vergleich bestätigte 98 geänderte
Kontextdateien und keine zusätzlichen Produktänderungen. Die anschließende Aktualisierung
dieses Arbeitsstands und des Backlogs hält diese beobachtete Bereitstellung fest.
Für die deutsche Variante wurde kein Pull Request erstellt und kein Merge durchgeführt.

Beim Start der Übersetzung wurde der englische Remote-Branch `codex/kontext-v3` auf
`a35e8c3a1116a763f286f965f718f36d5f8876cc` bestätigt; der deutsche Remote-Branch
war noch nicht vorhanden. Die englische Fassung wurde bei der Bereitstellung nicht geändert.

**Historie der englischen Fassung:** Die englische Anpassung wurde im bestehenden Fork
mit [Entwurfs-PR #120](https://github.com/Easton97-Jens/ModSecurity/pull/120) gegen
`v3/master` veröffentlicht. Einschließlich RTK-Ergänzung umfasst sie 100 ergänzte Dateien
und keine Produktquelländerungen. Bei der damaligen Prüfung war kein Merge erfolgt.
Der Anpassungs-Commit `bad6cd355fd78ffcfa6393b1c3ada8d734feb098` hatte den Baum
`9a884e6e02525a469bfa531e8aaa767cf915e856`, der dem geprüften lokalen Baum entsprach.
Er wurde separat aus dem Fork geklont; alle drei Prüfer waren erfolgreich und der
Checkout sauber. Spätere reine Kontextänderungen hielten Bereitstellungsfakten,
Portabilität und optionale RTK-Hinweise fest. Die 487 eindeutigen gebundenen
ModSecurity-Quellverweise blieben dabei erhalten. Die damalige PR-Workflow-Abfrage
lieferte für den Anpassungs-Commit keine Läufe. Das ist kein positives CI-Ergebnis
und kein Beleg dafür, dass später keine Workflows laufen können.

Nutze `git rev-parse HEAD` und den tatsächlichen Remote-Branch für den neuesten
Arbeits-Commit dieser deutschen Variante. Der Head des englischen PR bezeichnet einen anderen Branch.

## Nächster konkreter Schritt

Für den nächsten Chat `codex/kontext-v3-de` auswählen und den
[Startprompt](prompts/start-new-chat.md) verwenden. Danach die konkrete nächste Aufgabe benennen. Bei Wiederaufnahme immer den tatsächlichen Remote-Stand prüfen.
Andere Repositories und globale Nutzerkonfiguration wurden nicht geändert.
Für eine spätere Produktaufgabe relevante Leitfäden und tatsächlichen Quellcode lesen,
gewünschtes Verhalten und Umgebung bestimmen und angemessene Prüfungen auswählen.
Der erste tatsächliche C++-Build bleibt ein Nachweis für eine spätere Aufgabe.
