# Codex- und ChatGPT-Kontext für ModSecurity v3

Beginne mit der wiederverwendbaren [AGENTS.md](../AGENTS.md), danach mit dem
[Kontextverzeichnis](context/index.md) und dem
[Repository-spezifischen Kontext](context/repository-context.md). Die ausführlichen
Leitfäden, Arbeitsrichtlinien, Profile und der aktuelle Stand liegen unter `context/`.
Der Aufbau folgt der gewünschten Vorlage aus dem Connector-Repository des Eigentümers.

| Bereich | Zweck |
| --- | --- |
| [context/index.md](context/index.md) | Lesepfade, zuständige Richtlinien, technische Leitfäden und Aufgabenprofile |
| [context/current-state.md](context/current-state.md) | Tatsächliche Arbeit, Prüfungen und nächste Schritte |
| [context/template-adaptation.md](context/template-adaptation.md) | Genaue Revision der Vorlage und Anpassungen an die Kernbibliothek |
| [context/manifest.toml](context/manifest.toml) | Maschinenlesbare Dateizuordnung |
| [audit/README.md](audit/README.md) | Quelleninventar und datierte Übergaben mit Nachweisen |
| [bin/README.md](bin/README.md) | Nur lesende Konsistenzprüfer und ihre Grenzen |
| [inheritance-templates/README.md](inheritance-templates/README.md) | Ausdrückliche, optionale Wiederverwendung; keine automatische Vererbung an Submodule |

Die Arbeitsvereinbarung im Hauptverzeichnis kann unverändert in anderen Repositories
verwendet werden; diese `.codex`-Dateien bleiben auf den Fork zugeschnitten.
Die gepflegten Dateien dieser Variante sind Deutsch. Die Einrichtung schreibt keine Chatsprache vor.

Diese Dateien sind versionierter Projektkontext. Sie ändern kein Bibliotheksverhalten,
wählen kein Modell, konfigurieren kein Konto und machen alte Chats nicht zugänglich.
Neue Sitzungen müssen die relevanten Dateien tatsächlich lesen. Die deutsche Variante
liegt in `codex/kontext-v3-de`; die englische Grundlage bleibt in `codex/kontext-v3`.
