# Mit Codex und ChatGPT an ModSecurity arbeiten

Hier beginnt die zusätzliche deutsche Projektdokumentation.
Die Übersetzung liegt im Branch `codex/kontext-v3-de` und basiert auf dem englischen
Branch `codex/kontext-v3`, Commit `a35e8c3a1116a763f286f965f718f36d5f8876cc`.

**Technische Grundlage:** das offizielle
[Repository `owasp-modsecurity/ModSecurity`, `v3/master`](https://github.com/owasp-modsecurity/ModSecurity/tree/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729),
Commit `effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729`, untersucht am 25.10.2026.
Die Originaldateien dieses Stands bilden die Grundlage der Erklärungen.
Arbeitsanweisungen und Kontextdateien sind als Ergänzungen für dieses Projekt gekennzeichnet.

## Enthaltene Dokumentation

| Einstiegspunkt | Inhalt |
| --- | --- |
| [AGENTS.md](AGENTS.md) | Wiederverwendbare, vom Repository unabhängige Arbeitsvereinbarung |
| [Repository-Kontext](.codex/context/repository-context.md) | Identität dieses Forks, Quellenvorgaben, Prüfungen und Sprache der gepflegten Dateien |
| [.codex/context/index.md](.codex/context/index.md) | Technische Anleitungen, zuständige Richtlinien, Aufgabenprofile und Lesepfade |
| [Projektkontext](.codex/context/project-context.md) | Leistungen von libmodsecurity und Umfang dieses Repositorys |
| [Repository-Übersicht](.codex/context/repository-map.md) | Verzeichnisse, Sprachen, Build-Dateien und hilfreiche Einstiegspunkte |
| [Architektur](.codex/context/architecture.md) | Datenfluss von HTTP-Transaktionen, zentrale Klassen und Verarbeitung |
| [C++-Leitfaden](.codex/context/cpp-guide.md) | C++17, die C-Schnittstelle, Besitzverhältnisse und das Lesen des vorhandenen Codes |
| [Funktionsübersicht](.codex/context/feature-map.md) | Orte der Implementierung von Funktionen und zugehöriger Testdefinitionen |
| [Konfiguration und Regeln](.codex/context/configuration-and-rules.md) | SecRules, Konfiguration, Operatoren und tatsächliche Einschränkungen |
| [Build- und Testanleitung](.codex/context/build-and-test.md) | Quellenbelegte Vorbereitung, Build-Wege und aussagekräftige Prüfungen |
| [RTK-Leitfaden](.codex/context/rtk.md) | Optionale Verringerung von Befehlsausgaben, anhand der Quellen geprüfte Beispiele und Grenzen der Codex-Integration |
| [Aktueller Stand](.codex/context/current-state.md) | Abgeschlossene Arbeit, Prüfungen, offene Fragen und Hindernisse |
| [Quellenherkunft](.codex/context/provenance.md) | Upstream-Revision, Fork-Vergleich, Submodule und Regeln für Nachweise |

## An das Connector-Repository angepasster Aufbau

Der Aufbau folgt der untersuchten `.codex`-Struktur von
[`Easton97-Jens/ModSecurity-conector`](https://github.com/Easton97-Jens/ModSecurity-conector/tree/42fd5a8e895aca3a36b09eb09fe5c7cf9c3399e3/.codex):
`context/`, `context/profiles/`, `context/security/`, `bin/`, `audit/` und
ausdrückliche Manifestdateien. Leitfäden zu Kernkomponenten ersetzen Connector-spezifische Leitfäden.
Der [Anpassungsnachweis](.codex/context/template-adaptation.md) erklärt die Zuordnung
und unterscheidet wiederverwendbare Abläufe von maschinen- oder Connector-spezifischen Regeln.

## Dieselbe AGENTS.md in anderen Repositories verwenden

Die `AGENTS.md` im Hauptverzeichnis ist bewusst unabhängig vom Repository. Sie enthält keinen
festen Projektnamen, Branch, Sprachstandard, Build-Befehl und kein zwingend benötigtes Python-Hilfsprogramm.
Ihre Verweise auf `.codex` gelten bedingt: Ein Repository ohne diese Struktur nutzt seine
eigenen verfügbaren Anweisungen und Dokumente. Git-spezifische Prüfungen setzen voraus,
dass Git und ein tatsächlicher Checkout vorhanden sind.

Du kannst diese Datei unverändert in einem anderen Repository verwenden. Jedes Repository behält
seine eigene Identität, Befehle, Versionen, technische Anleitungen und gespeicherten Arbeitsstände.
Für diesen Fork beginnen diese Angaben in
[repository-context.md](.codex/context/repository-context.md). Das Kopieren der Datei im Hauptverzeichnis
verteilt sie nicht automatisch, aktiviert keine globale Einstellung und übernimmt diesen
ModSecurity-spezifischen Kontext nicht in andere Projekte.

Die gepflegten Kontextdateien dieser Variante sind Deutsch. Die Dateien schreiben keine Chatsprache
vor; jede Person entscheidet selbst über die Sprache ihres Gesprächs.

## Wozu AGENTS.md und .codex?

`AGENTS.md` liefert knappe Anweisungen am Einstiegspunkt des Repositorys. Codex
verwendet solche Dateien entlang seines Pfads für Projektanweisungen. Die längeren Kapitel
liegen in `.codex/`; die Startanweisungen führen den Assistenten zu den relevanten Dateien.
Die offizielle
[Anleitung zu AGENTS.md](https://learn.chatgpt.com/docs/agent-configuration/agents-md)
beschreibt, wie Anweisungen gefunden werden.

In diesem Repository ist `.codex/` ein versioniertes Dokumentationsverzeichnis. Namen wie
`current-state.md` sind Projektkonventionen. Sie versprechen nicht, dass jedes Produkt
jede Datei automatisch lädt. Diese Einrichtung wählt kein Modell aus und überschreibt
keine persönliche Codex-Konfiguration.

Ein neuer Chat kann anhand dieser Dateien fortfahren, wenn er Zugriff darauf hat und sie liest.
Stelle bei gewöhnlicher ChatGPT-Nutzung Repository-Zugriff oder die aktuellen Dateien bereit.
Entscheidungen, die nur in einem früheren Gespräch stehen, sind in diesem System nicht enthalten.
Den gespeicherten Arbeitsstand zu aktualisieren, gehört deshalb zum Abschluss einer Sitzung.

## Routinemäßige Befehlsausgaben mit RTK reduzieren

Die Arbeitsvereinbarung bevorzugt unterstützte RTK-Zusammenfassungen, wenn das Werkzeug verfügbar ist.
Native Befehle stehen bereit, wenn es fehlt oder genaue Ausgaben benötigt werden.
Lies den [RTK-Leitfaden](.codex/context/rtk.md), wenn du RTK verwendest oder einrichtest.
Diese Ergänzung liefert Anweisungen und Beispiele; sie installiert RTK nicht und aktiviert
keine Hooks. Codex-Verhalten wurde anhand der offiziellen OpenAI-Dokumentation geprüft,
RTK-Verhalten anhand des eigenen festgehaltenen Quellstands. Für diese Dokumentationsänderung
wurden keine Einsparungen bei Sitzungstokens oder Abrechnung gemessen.

## Einen neuen Chat beginnen

Öffne das Repository in deiner Arbeitsumgebung oder stelle die relevanten Dateien
im Chat bereit. Verwende dann beispielsweise folgenden Prompt:

> Arbeite an diesem ModSecurity-Repository im Branch codex/kontext-v3-de.
> Lies zuerst AGENTS.md, .codex/context/index.md,
> .codex/context/repository-context.md, .codex/context/current-state.md
> und .codex/context/decisions.md.
> Vergleiche den tatsächlichen Checkout mit .codex/context/upstream-lock.json.
> Fasse den überprüften Arbeitsstand und offene Fragen zusammen. Erfinde keine
> fehlenden Informationen. Pflege die Kontextdateien dieser Variante auf Deutsch.
> Meine nächste Aufgabe lautet: ...

Ein längerer Prompt zum Kopieren steht in
[start-new-chat.md](.codex/context/prompts/start-new-chat.md). Nutze
[end-session.md](.codex/context/prompts/end-session.md) für die Übergabe.

Diese Dateien gehören in den bestehenden Fork `Easton97-Jens/ModSecurity`.
[using-this-fork.md](.codex/context/using-this-fork.md) erklärt, wie du den
Arbeitsbranch `codex/kontext-v3-de` öffnest und in neuen Chats verwendest.
Für diese deutsche Variante ist kein Pull Request beauftragt.

## Konsistenz des Kontexts prüfen

Vom Hauptverzeichnis des Repositorys aus:

```sh
python3 .codex/bin/verify-context.py
python3 .codex/bin/validate-structure.py
python3 .codex/bin/validate-codex-inheritance.py --check
```

Wenn der Upstream-Quellcode unverändert sein soll, zusätzlich:

```sh
python3 .codex/bin/verify-context.py --strict-source
```

Die Struktur- und Katalogprüfer benötigen Python 3.11 oder neuer. Der Quellenprüfer
benötigt Python 3 und Git mit `--no-lazy-fetch`, das automatische Objektdownloads
in partiellen Klonen verhindert. Er prüft den festgehaltenen Commit, das Quelleninventar,
lokale Dokumentationsverweise sowie Existenz und Zeilenbereiche der an einen Commit
gebundenen ModSecurity-Verweise. Er meldet Abweichungen im Arbeitsbaum.
Er ersetzt weder das Lesen des Codes noch Builds oder Tests.
Die [Anleitung zu den Prüfprogrammen](.codex/bin/README.md) beschreibt Umfang und Pflege der Richtlinienprüfsummen.

Diese Dokumentation erklärt den Aufbau und zentrales Verhalten des Repositorys ausführlich.
Sie ist keine überprüfte Beschreibung jeder C++-Funktion und kein Nachweis dafür,
dass eine bestimmte Installation sicher ist.
