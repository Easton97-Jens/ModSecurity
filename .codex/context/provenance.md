# Herkunft, Quellgrundlage und Nachweise

## 1. Festgehaltener Quellstand

| Feld | Überprüfter Wert |
| --- | --- |
| Offizielles Repository | `https://github.com/owasp-modsecurity/ModSecurity` |
| Untersuchter Entwicklungsbranch | `v3/master` |
| Festgehaltener Commit | `effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729` |
| Git-Baum | `d626de814f96ceea7fd717d6176b30a17a86f08e` |
| Commit-Zeit laut Git | `2026-10-24T18:06:05+02:00` |
| Untersuchung | 25. Oktober 2026, Datum in Europe/Berlin |
| Inventar des Hauptrepositorys | 850 Einträge: 846 Blobs und 4 Gitlinks |

Der [Commit](https://github.com/owasp-modsecurity/ModSecurity/commit/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729)
und sein [Quellbaum](https://github.com/owasp-modsecurity/ModSecurity/tree/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729)
bilden die Grundlage. Die Werte wurden mit `git show`, `git rev-parse` und
`git ls-tree -r` gelesen. Das vollständige maschinenlesbare Inventar liegt in
[source-index.tsv](../audit/source-index.tsv), die Metadaten in
[upstream-lock.json](upstream-lock.json).

„850 Einträge“ bezieht sich auf den Git-Baum, nicht auf 850 einzeln erklärte
Funktionen oder ein umfassendes Code-Review. Ein Blob kann beispielsweise auch
eine Bilddatei sein. Ein Gitlink verweist auf einen Commit in einem separaten Repository.

## 2. Beziehung zum angegebenen Nutzerfork

Beim ersten Vergleich am Untersuchungstag zeigte `Easton97-Jens/ModSecurity`,
ebenfalls im Branch `v3/master`, auf `2e08f23dd50e7c0aa7c969b244b9d3a6dce4d641`.
Dieser Commit ist ein Vorfahr der oben genannten offiziellen Grundlage. Der Vergleich
zeigte **22 zusätzliche Upstream-Commits und 0 Commits, die nur im verglichenen Fork-Branch
vorhanden waren**. Dies beschreibt diese beiden Branch-Spitzen zu diesem Zeitpunkt,
nicht alle Branches oder den künftigen Zustand des Forks.

Nachweise: [Fork-Commit](https://github.com/Easton97-Jens/ModSecurity/commit/2e08f23dd50e7c0aa7c969b244b9d3a6dce4d641),
[Vergleich der beiden Commits](https://github.com/owasp-modsecurity/ModSecurity/compare/2e08f23dd50e7c0aa7c969b244b9d3a6dce4d641...effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729).
Nach dem Abrufen der v3-Historie wurden zusätzlich diese Prüfungen lokal ausgeführt:

```sh
git merge-base --is-ancestor \
  2e08f23dd50e7c0aa7c969b244b9d3a6dce4d641 \
  effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729
git rev-list --count \
  2e08f23dd50e7c0aa7c969b244b9d3a6dce4d641..effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729
git rev-list --count \
  effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729..2e08f23dd50e7c0aa7c969b244b9d3a6dce4d641
```

Ergebnisse in dieser Reihenfolge: Exit-Code `0`, Anzahl `22`, Anzahl `0`.
**Erneuter Vergleich vor dem damaligen Pull Request:** Der Branch `v3/master` des Forks
hatte inzwischen ebenfalls `effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729` erreicht.
Der Vergleich mit dem ersten Kontext-Commit `fbf883634454ba39e4c49aa2f87483b9c4610902`
zeigte einen zusätzlichen Dokumentations-Commit und 25 geänderte Dateien.
Die Branch-Spitzen wurden auch mit `git ls-remote` gelesen. Hier wird nicht angenommen,
wer den Zielbranch aktualisiert hat.

Der damalige Pull Request gegen diese aktualisierte Fork-Grundlage enthielt daher
nur Kontextergänzungen. Der ursprüngliche Unterschied von 22 Commits ist historische
Herkunftsinformation und kein zusätzlicher Produktcode-Diff dieses Pull Requests.
Beide Beobachtungen bleiben getrennt in der Lockdatei erhalten.

Die deutsche Variante übersetzt den englischen Kontext aus `codex/kontext-v3`, Commit
`a35e8c3a1116a763f286f965f718f36d5f8876cc`, in `codex/kontext-v3-de`.
Der Auftrag umfasst das Hochladen dieses Branches im selben Fork ohne neuen Pull Request.
Er ändert die oben festgehaltene Produktgrundlage nicht.

## 3. Submodule sind getrennte Quellen

Die vier direkten Gitlinks im Haupt-Commit sind:

| Pfad | Festgehaltener Submodul-Commit |
| --- | --- |
| `bindings/python` | `bc625d5bb0bac6a64bcce8dc9902208612399348` |
| `others/libinjection` | `211782219663f889f471650150df12b623c5766e` |
| `others/mbedtls` | `0fe989b6b514192783c469039edd325fd0989806` |
| `test/test-cases/secrules-language-tests` | `f73c73027ef49ccf99c7911744929a71d15ff419` |

Quelle für Zuordnungen und URLs: [.gitmodules](https://github.com/owasp-modsecurity/ModSecurity/blob/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729/.gitmodules).
Quelle für Commit-IDs: Gitlinks im
[festgehaltenen Quellbaum](https://github.com/owasp-modsecurity/ModSecurity/tree/effb6c0fa031a7bddb3253e0b7ab9ca9a18b5729),
reproduzierbar mit `git ls-tree -r` und im Inventar festgehalten.

Die internen Implementierungen dieser Submodule wurden für diese Dokumentation nicht
vollständig untersucht. Aussagen über ihre Verwendung beruhen auf Aufrufern und
Build-Definitionen im Hauptrepository. Eine Änderung innerhalb eines Submoduls erfordert,
dessen tatsächlichen Commit separat zu lesen und zu prüfen. Dies gilt auch für
verschachtelte Submodule.

## 4. Welche Quelle beantwortet welche Frage?

| Frage | Bevorzugter Nachweis |
| --- | --- |
| Öffentliche Signatur | Deklaration im relevanten Header und Funktionsdefinition |
| Tatsächliches Verhalten | Implementierung, Aufrufer, Zustands- und Fehlerpfade |
| Akzeptierte Regelsyntax | Scanner, Parser-Grammatik und konkrete Parser-Aktionen |
| Aktivierte oder deaktivierte Funktion | Build-Definitionen, Präprozessorschalter und Laufzeitpfad |
| Erwartetes Verhalten | Vorhandene Tests und nachvollziehbare Anforderungen |
| In dieser Umgebung erfolgreich? | Ein tatsächlich aufgezeichneter Build- oder Testlauf |
| Historischer Kontext | Git-Commit und Diff, klar von aktuellem Code unterschieden |

README, Kommentare und Doxygen helfen bei der Orientierung, können aber von der
Implementierung abweichen. [fact-check.md](fact-check.md) nennt konkrete Beispiele.
Ein registrierter Test und ein erfolgreicher Testlauf liefern unterschiedliche Nachweise.

## 5. Eigene Ergänzungen und externe Produktdokumentation

`AGENTS.md`, `CODEX_START.md` und `.codex/` wurden für diese Aufgabe ergänzt.
Arbeitsabläufe, Übergabevorlagen und Kontextprüfer sind Konventionen dieses
Arbeitsrepositorys. Sie werden nicht als vorhandene Upstream-Funktionen oder
Anweisungen der ModSecurity-Verantwortlichen dargestellt.

Die `.codex`-Organisation wurde aus dem vom Eigentümer gewählten Connector-Repository
an dem in [template-adaptation.md](template-adaptation.md) festgehaltenen Stand angepasst.
Dessen Richtlinien liefern eine Strukturvorlage, keine Nachweise für libmodsecurity-Verhalten.
Die lokalen Manifestschemata und Prüfprogramme sind ausdrücklich ergänzte Arbeitskonventionen.

Die offizielle OpenAI-[Anleitung zu AGENTS.md](https://learn.chatgpt.com/docs/agent-configuration/agents-md)
und [Hook-Referenz](https://learn.chatgpt.com/docs/hooks) wurden am 25.10.2026 für das
Auffinden von Codex-Anweisungen und unterstütztes Integrationsverhalten gelesen.
Sie betreffen Codex und belegen keine Empfehlung von RTK durch OpenAI.

Der optionale [RTK-Leitfaden](rtk.md) verwendet separat `rtk-ai/rtk`, Tag `v1.0.1`,
Commit `4ef8730cf3a0dea11c8d64a9b90bc0616fe52100`. Dessen Dokumentation und ausgewählte
Implementierungen für Befehle, Erfassung und Codex-Initialisierung wurden untersucht.
Dies ist ein Referenzstand, keine installierte Abhängigkeit oder umfassende RTK-Prüfung.
Die Beispielbefehle wurden in dieser Umgebung nicht mit RTK ausgeführt.
**Keine dieser externen Quellen ist maßgeblich für das Verhalten von ModSecurity.**

Dieser Dokumentationsansatz erfordert keine bestimmte Modellversion, keinen
API-Schlüssel und keine Übernahme persönlicher Einstellungen. Die Dateien müssen
für den verwendenden Chat oder Programmierassistenten tatsächlich zugänglich sein.

## 6. Grenzen der Konsistenzprüfung

`verify-context.py` prüft Git-Objekte, Inventar, lokale Verweisziele und an einen
Commit gebundene Quellpfade einschließlich Zeilenbereichen. Aktuelle technische
Kapitel müssen zur festgehaltenen Grundlage passen. Historische Sitzungs- und
Entscheidungsnachweise dürfen ihre eigene vollständige Commit-ID verwenden und
werden gegen die lokal verfügbaren Git-Objekte dieses Commits geprüft.

Das Prüfprogramm lädt nichts aus dem Netzwerk und führt keine Build-Programme aus.
Ein fehlendes erforderliches Git-Objekt wird als Fehler gemeldet. Die Prüfung erfordert
eine Git-Version mit `--no-lazy-fetch`. Sie prüft nicht die Erreichbarkeit externer
Websites oder ob jeder Satz durch seinen Link inhaltlich gestützt wird. Letzteres
bleibt Aufgabe eines Code- und Dokumentationsreviews.

Ausgangsstand und tatsächlich ausgeführte Prüfungen sind in
[current-state.md](current-state.md) festgehalten. [upstream-sync.md](upstream-sync.md)
beschreibt die Pflege beim Übernehmen einer neuen Upstream-Version.
