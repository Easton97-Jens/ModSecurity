# Mit dem bestehenden ModSecurity-Fork arbeiten

Dieser Arbeitskontext gehört zum bestehenden Fork
[`Easton97-Jens/ModSecurity`](https://github.com/Easton97-Jens/ModSecurity).
Die deutschen Kontextdateien liegen im Arbeitsbranch `codex/kontext-v3-de`.
Die englische Grundlage ist `codex/kontext-v3`, Commit
`a35e8c3a1116a763f286f965f718f36d5f8876cc`.
[Entwurfs-PR #120](https://github.com/Easton97-Jens/ModSecurity/pull/120) gehört zur
englischen Fassung. Für die deutsche Variante ist kein neuer Pull Request beauftragt.
Der tatsächliche Arbeits- und Bereitstellungsstand steht in [current-state.md](current-state.md).

## 1. Den richtigen Branch auswählen

Für einen neuen lokalen Checkout:

```sh
git clone --branch codex/kontext-v3-de https://github.com/Easton97-Jens/ModSecurity.git ModSecurity
cd ModSecurity
git status --short
git log -1 --oneline
python3 .codex/bin/verify-context.py --strict-source
```

Prüfe bei einer vorhandenen Arbeitskopie zuerst Änderungen und Remotes.
Wenn `origin` tatsächlich auf diesen Fork zeigt und der Arbeitsbaum einen
Branch-Wechsel erlaubt:

```sh
git fetch origin
git switch codex/kontext-v3-de
```

Diese Schritte setzen voraus, dass der Remote-Arbeitsbranch bereits vorhanden ist.
Die Kontextprüfung erfordert Python 3 und Git mit `--no-lazy-fetch`.
Lies danach [CODEX_START.md](../../CODEX_START.md) und nutze den
[Startprompt](prompts/start-new-chat.md) für einen neuen Chat.

## 2. Quellgrundlage und Branches verstehen

Die technische Dokumentation beschreibt den offiziellen v3-Commit
`effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729`. Bei der ersten Beobachtung lag
`v3/master` des Forks auf `2e08f23dd50e7c0aa7c969b244b9d3a6dce4d641`,
22 Upstream-Commits zurück. Der Kontext-Arbeitsbranch enthält die dokumentierte
offizielle Grundlage und die ergänzten Kontextdateien.

Vor dem Öffnen des damaligen englischen Pull Requests wurde der Zielbranch erneut
geprüft: `v3/master` hatte inzwischen denselben dokumentierten offiziellen Commit
erreicht. Der Pull Request gegen diese Grundlage enthielt deshalb nur Kontextergänzungen.
[provenance.md](provenance.md) unterscheidet den ersten vom erneuten Vergleich.
Produktcode-Änderungen gehören nicht zu den Kontextergänzungen oder ihrer Übersetzung.

Wähle für die deutsche Fassung ausdrücklich `codex/kontext-v3-de`, damit ein neuer
Chat die übersetzten `AGENTS.md`- und `.codex/`-Dateien liest. Die frühere englische
Integration und ihr PR betreffen einen anderen Branch. Ein Link auf den Fork allein
garantiert nicht, dass ein Chat den richtigen Branch geöffnet hat; prüfe Branch und
Commit ausdrücklich. Ein später beauftragter Merge könnte Dateien in einem Zielbranch
verfügbar machen, gehört aber nicht zur deutschen Übersetzungsaufgabe.

## 3. Build und Submodule

Stelle vor einem Build die für die tatsächliche Plattform in
[build-and-test.md](build-and-test.md) beschriebenen Werkzeuge und Bibliotheken bereit.
Initialisiere die vom Hauptrepository festgehaltenen Submodule mit
`git submodule update --init --recursive`. Dieser Schritt ist im offiziellen
[Git-Checkout-Ablauf](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/README.md#L65-L98)
beschrieben. Die genauen Gitlinks stehen in [upstream-lock.json](upstream-lock.json).

Die enthaltenen Build-Anweisungen wurden aus den Quellen abgeleitet. Tatsächliche
Build- und Testergebnisse für die Zielumgebung müssen als neue Prüfnachweise
festgehalten werden. Das Vorhandensein dieses Branches beweist keinen ausgeführten C++-Build.

## 4. Neue Chats und spätere Änderungen

Benenne oder liefere zu Beginn eines Chats den gewählten Branch, `AGENTS.md`, den Index
und `current-state.md`. Nenne anschließend die konkrete Aufgabe. Speichere nach einer
Sitzung den tatsächlichen Arbeitsstand, Entscheidungen und offene Fragen.
[session-workflow.md](session-workflow.md) beschreibt den Ablauf;
[upstream-sync.md](upstream-sync.md) gilt für die Aktualisierung der Upstream-Grundlage.
