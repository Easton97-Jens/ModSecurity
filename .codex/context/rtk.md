# RTK: optionale Verringerung von Befehlsausgaben

Verwende diesen Leitfaden, wenn RTK verfügbar ist oder seine Einrichtung zur
Aufgabe gehört. Er ergänzt
[command-execution-policy.md](command-execution-policy.md) und muss nicht bei
jedem Start gelesen werden. Die wiederverwendbare Vereinbarung im
Stammverzeichnis bevorzugt unterstützte RTK-Zusammenfassungen und erlaubt
native Werkzeuge, wenn RTK fehlt oder die vollständige Ausgabe benötigt wird.

## Quellen und Geltungsbereich

Untersucht am **2026-10-25**: RTK-Tag `v1.0.1`, Commit
[`4ef8730cf3a0dea11c8d64a9b90bc0616fe52100`][rtk-tree]. Dieser Referenzstand
ist weder eine Mindestversionsvorgabe noch eine installierte Abhängigkeit.
Prüfe vor Verwendung dieser Beispiele die tatsächlich installierte Version
und ihre Hilfe.

RTK stellt Befehle bereit, die die einem Assistenten gezeigte Terminalausgabe
verringern. Seine [README][rtk-readme] erklärt die beabsichtigte Reduktion.
Verwende für das Verhalten von Codex OpenAIs offizielle
[AGENTS.md-Anleitung][codex-agents] und [Hook-Referenz][codex-hooks]. RTK ist ein
externes Werkzeug; diese OpenAI-Quellen belegen keine Empfehlung oder Garantie
von OpenAI für RTK. Keine dieser Quellen ändert die offizielle
ModSecurity-Produktbasis in [upstream-lock.json](upstream-lock.json).

## Manuelle Nutzung einer verfügbaren ausführbaren Datei

Prüfe die Verfügbarkeit in der aktuellen Umgebung und anschließend die
installierte CLI. Diese Beispiele sind bewusst auszuführende Befehle,
keine aufgezeichneten Testergebnisse:

```sh
rtk --version
rtk --help
rtk git --help
```

Für die übliche Erkundung dieses Checkouts implementiert der untersuchte
Release Folgendes:

| Befehl | Nützliche Ausgabe und Grenze | Beleg |
| --- | --- | --- |
| `rtk git status` | Übersicht über den Arbeitsbaum; den nativen Status verwenden, wenn exakte Einträge oder ein maschinenlesbares Format benötigt werden. | [`run_status`][rtk-status] |
| `rtk git diff -- src/transaction.cc` | Gezielte Änderungsübersicht. Der Standardpfad verdichtet den Diff und kann Inhalte auslassen. Vor dem Freigeben von Änderungen den vollständigen nativen Diff prüfen. | [`run_diff`][rtk-diff] |
| `rtk git log -5` | Übersicht der letzten Commits mit ausdrücklich angegebener Anzahl; einen Commit vollständig prüfen, wenn sein Beschreibungstext oder exakte Metadaten relevant sind. | [Argumentbehandlung in `run_log`][rtk-log] |
| `rtk gain --project --text` | Statische Kennzahlen zu Befehlsausgaben für den Bereich des aktuellen Verzeichnisses; diese Zahlen sind Schätzungen. Ein gesonderter Proxy-Abschnitt kann einen anderen Geltungsbereich haben. | [Analyseleitfaden][rtk-gain], [Bereichsflag][rtk-gain-cli] |
| `rtk gain --project --history` | Zuletzt erfasste Befehle in diesem Bereich; frühere Aufgaben können weiterhin enthalten sein. | [Analyseleitfaden][rtk-gain], [Bereichsbehandlung][rtk-gain-scope] |

Beginne mit eingegrenzten Suchen und begrenzten Dateiausschnitten. Lade nicht
jedes Kontextkapitel und sammle keine wiederholten Logs, nur weil ein Filter
verfügbar ist. Verwende natives `rg` und genaue Quellbereiche, um Codeverhalten
zu belegen. In diesem Release verwendet `rtk read` standardmäßig `--level none`;
seine bloße Verwendung bedeutet daher noch keine verdichtete Quelltextausgabe.
Siehe [CLI-Definition][rtk-read].

## Exakte Belege verfügbar halten

RTK-Zusammenfassungen dienen der Orientierung. Verwende für Schlussfolgerungen
vollständigen Quellcode, relevante unveränderte Diagnostik, vollständige Diffs
und tatsächliche Exitcodes. Leite keine gefilterte Zusammenfassung in einen
Parser weiter, erzeuge daraus keinen Patch und werte ein fehlendes Detail nicht
als Beleg dafür, dass es in der ursprünglichen Ausgabe nicht vorkam.

Führe bei der manuellen Einrichtung dieses Repositorys den nativen Befehl direkt
aus. Falls eine bestehende RTK-Integration ihn umschreibt, stellt der untersuchte
Release außerdem einen Befehl zur ungefilterten Ausführung bereit:

```sh
git --no-pager diff -- src/transaction.cc
rtk bypass git --no-pager diff -- src/transaction.cc
```

Wähle eine passende Form; die beiden Zeilen sind Alternativen. `rtk bypass`
erfasst weiterhin die Nutzung und unterliegt den normalen Ausführungsrechten.
In diesem Release ist `rtk proxy` ein veralteter Alias dafür; die eigenständige
Funktion `llm-proxy` hat einen anderen Zweck. Siehe
[CLI-Definition][rtk-bypass]. Bewahre bei einem Befehl mit Nebenwirkungen seine
ursprüngliche Ausgabe auf, statt ihn nur für ein längeres Log erneut
auszuführen. Nimm nicht an, dass jeder Filter eine vollständig
wiederherstellbare Ausgabe speichert.

Verwende für ModSecurity-Builds/-Tests und die drei Kontextprüfungen weiterhin
die dokumentierten nativen Aufrufe in [build-and-test.md](build-and-test.md) und
[bin/README.md](../bin/README.md). Erfinde keinen Befehl `rtk make` und ersetze
die Aufrufe nicht durch `rtk test make check`: Der untersuchte allgemeine
Test-Runner stuft diesen Testbefehl als unbekannt ein und fällt auf eine
begrenzte Ausgabe zurück. Siehe [Test-Runner-Klassifizierung][rtk-test] und
[Ersatzausgabe][rtk-test-fallback]. RTK ergänzt keine ModSecurity-Testabdeckung
und belegt nicht, dass ein Test ausgeführt wurde.

## Codex-Integration folgt der offiziellen OpenAI-Dokumentation

Dieses Repository verwendet ausdrückliche Anweisungen in `AGENTS.md`; Details
werden über den Kontextindex erschlossen. OpenAI dokumentiert die Reihenfolge,
in der Anweisungsdateien gefunden werden; beliebige Markdown-Dateien unter
`.codex` sind nicht alle automatisch Startanweisungen. Verlasse dich nicht auf
einen undokumentierten `@file`-Import. Siehe
[offizielle Anleitung][codex-agents].

Optionales automatisches Umschreiben ist eine gesonderte
Einrichtungsentscheidung. OpenAI dokumentiert das Umschreiben von Eingaben über
`PreToolUse`, die Hook-Konfiguration sowie Prüfung und Vertrauensfreigabe über
`/hooks` für unterstützte Codex-Arbeitsabläufe. Projekt-Hooks benötigen eine als
vertrauenswürdig eingestufte Projektebene. Lokale Befehls-Hooks werden bei
Cloud-Orchestrierung nicht unterstützt, selbst wenn Werkzeuge lokal ausgeführt
werden. Prüfe den tatsächlichen Client und die aktuelle
[Hook-Dokumentation][codex-hooks].

RTKs eigene [Codex-Einrichtungsanleitung][rtk-codex] beschreibt seinen
Initialisierer. Eine bewusste Einrichtungsprüfung kann nach dem Lesen der
lokalen Hilfe mit `rtk init --codex --dry-run` beginnen. Der tatsächliche
Initialisierer schreibt `RTK.md`, passt `AGENTS.md` an und konfiguriert
`.codex/hooks.json`; der globale Modus richtet sich stattdessen an die
persönliche Codex-Konfiguration. Das sind Konfigurationsänderungen, keine
Voraussetzungen für einen manuellen Aufruf. Siehe [Zielpfade][rtk-init],
[Schreiboperationen][rtk-init-writes] und [dry-run-Option][rtk-init-cli].

Diese Ergänzung führt den Initialisierer nicht aus, installiert keine Hooks,
aktiviert keinen Proxy und ändert keine persönliche Konfiguration. Bei einer
späteren Einführung von Projekt-Hooks müssen deren erzeugte Dateien und die
Kontextgrenze geprüft werden: Eine neue `RTK.md` im Stammverzeichnis liegt
außerhalb der aktuellen `--strict-source`-Ergänzungen. Prüfe die tatsächlich
geladenen Anweisungen und Hooks im jeweiligen Client, bevor du eine
automatische Nutzung behauptest. Die Codex-Berechtigungen bleiben wirksam.

## Verringerte Ausgaben messen, ohne Einsparungen zu erfinden

Die untersuchte Implementierung schätzt Tokens als UTF-8-Bytelänge geteilt
durch vier, aufgerundet; sie ist kein Modell-Tokenizer. Der
[Schätzer][rtk-estimator] und die [Erfassungsaufrufe][rtk-tracking] belegen diese
Berechnung. Die Kennzahlen zu Befehlsausgaben in `rtk gain` verwenden die von
RTK erfassten Schätzungen; eine Ausgabeverdichtung lässt sich nicht unmittelbar
in denselben Prozentsatz an gesamten Unterhaltungstokens, Abonnementnutzung
oder Kosten übersetzen.

Ein vorhandener LLM Proxy kann `rtk gain` um einen gesonderten Abschnitt
ergänzen. In diesem Quellstand wird dessen Zusammenfassung unabhängig vom
Projektfilter für Befehlsausgaben geladen und fasst Proxy-Datensätze ohne diese
Projektbeschränkung zusammen. Ordne seine Werte nicht allein deshalb diesem
Checkout zu, weil `--project` verwendet wurde. Siehe
[Laden der Zusammenfassung][rtk-gain-proxy] und
[Proxy-Zusammenfassung][rtk-proxy-stats].

Benenne bei Messberichten RTK-Version, Befehle, Geltungsbereich und Zeitfenster
sowie, ob die Belege Ausgabebytes, geschätzte Tokens oder tatsächliche
Anbieternutzung messen. Halte Ergebnisse anderer Projekte oder früherer Läufe
getrennt. Führe aufwendige oder zustandsändernde Befehle nicht allein zum
Größenvergleich zweimal aus. Bei der Erstellung dieses Leitfadens wurden weder
eine RTK-Ausführung noch Token- oder Kosteneinsparungen gemessen. Die ausgewählten
Quellen und Dokumentationen wurden untersucht; Laufzeitintegration und
Einsparungen in einer tatsächlichen Entwicklungssitzung bleiben ungeprüft.

[rtk-tree]: https://github.com/rtk-ai/rtk/tree/4ef8730cf3a0dea11c8d64a9b90bc0616fe52100
[rtk-readme]: https://github.com/rtk-ai/rtk/blob/4ef8730cf3a0dea11c8d64a9b90bc0616fe52100/README.md#L45-L76
[rtk-status]: https://github.com/rtk-ai/rtk/blob/4ef8730cf3a0dea11c8d64a9b90bc0616fe52100/src/cmds/git/git_cmd.rs#L2290-L2374
[rtk-diff]: https://github.com/rtk-ai/rtk/blob/4ef8730cf3a0dea11c8d64a9b90bc0616fe52100/src/cmds/git/git_cmd.rs#L261-L387
[rtk-log]: https://github.com/rtk-ai/rtk/blob/4ef8730cf3a0dea11c8d64a9b90bc0616fe52100/src/cmds/git/git_cmd.rs#L1711-L1759
[rtk-gain]: https://github.com/rtk-ai/rtk/blob/4ef8730cf3a0dea11c8d64a9b90bc0616fe52100/docs/guide/analytics/gain.md#L8-L125
[rtk-gain-cli]: https://github.com/rtk-ai/rtk/blob/4ef8730cf3a0dea11c8d64a9b90bc0616fe52100/src/lib.rs#L106-L119
[rtk-gain-scope]: https://github.com/rtk-ai/rtk/blob/4ef8730cf3a0dea11c8d64a9b90bc0616fe52100/src/analytics/gain.rs#L1454-L1462
[rtk-gain-proxy]: https://github.com/rtk-ai/rtk/blob/4ef8730cf3a0dea11c8d64a9b90bc0616fe52100/src/analytics/gain.rs#L675-L688
[rtk-proxy-stats]: https://github.com/rtk-ai/rtk/blob/4ef8730cf3a0dea11c8d64a9b90bc0616fe52100/src/llm_proxy.rs#L283-L312
[rtk-read]: https://github.com/rtk-ai/rtk/blob/4ef8730cf3a0dea11c8d64a9b90bc0616fe52100/src/lib.rs#L197-L218
[rtk-bypass]: https://github.com/rtk-ai/rtk/blob/4ef8730cf3a0dea11c8d64a9b90bc0616fe52100/src/lib.rs#L812-L821
[rtk-test]: https://github.com/rtk-ai/rtk/blob/4ef8730cf3a0dea11c8d64a9b90bc0616fe52100/src/core/runner.rs#L767-L799
[rtk-test-fallback]: https://github.com/rtk-ai/rtk/blob/4ef8730cf3a0dea11c8d64a9b90bc0616fe52100/src/core/runner.rs#L1170-L1224
[rtk-codex]: https://github.com/rtk-ai/rtk/blob/4ef8730cf3a0dea11c8d64a9b90bc0616fe52100/docs/guide/getting-started/supported-agents.md#L249-L270
[rtk-init]: https://github.com/rtk-ai/rtk/blob/4ef8730cf3a0dea11c8d64a9b90bc0616fe52100/src/hooks/init/codex.rs#L541-L572
[rtk-init-writes]: https://github.com/rtk-ai/rtk/blob/4ef8730cf3a0dea11c8d64a9b90bc0616fe52100/src/hooks/init/codex.rs#L607-L650
[rtk-init-cli]: https://github.com/rtk-ai/rtk/blob/4ef8730cf3a0dea11c8d64a9b90bc0616fe52100/src/lib.rs#L535-L544
[rtk-estimator]: https://github.com/rtk-ai/rtk/blob/4ef8730cf3a0dea11c8d64a9b90bc0616fe52100/text/src/lib.rs#L75-L85
[rtk-tracking]: https://github.com/rtk-ai/rtk/blob/4ef8730cf3a0dea11c8d64a9b90bc0616fe52100/src/core/tracking.rs#L2148-L2163
[codex-agents]: https://learn.chatgpt.com/docs/agent-configuration/agents-md
[codex-hooks]: https://learn.chatgpt.com/docs/hooks
