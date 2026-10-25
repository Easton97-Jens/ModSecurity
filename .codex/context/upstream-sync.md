# Die offizielle v3-Grundlage bewusst aktualisieren

Dies ist der gewählte Pflegeablauf für die Quellgrundlage dieser Dokumentation.
Sie aktualisiert sich nicht selbst. Der zuerst untersuchte Stand ist in
[upstream-lock.json](upstream-lock.json) festgehalten; historische Herkunft und
Branch-Vergleich stehen in [provenance.md](provenance.md).

## 1. Zuerst den vorhandenen Zustand bewahren und verstehen

Wenn eine Upstream-Aktualisierung beauftragt wird, lies zuerst `AGENTS.md`,
`current-state.md` und die festgehaltenen Entscheidungen. Prüfe anschließend
Arbeitsbaum und Remotes:

```sh
git status --short
git branch --show-current
git remote -v
git rev-parse HEAD
python3 .codex/bin/verify-context.py
```

Erhalte vorhandene Änderungen. Setze nicht automatisch auf eine neue Branch-Spitze zurück.
Prüfe, ob `upstream` tatsächlich auf das offizielle Repository zeigt.
Setze die Bedeutung des Remote-Namens nicht voraus, nur weil diese Anleitung ihn verwendet.

## 2. Eine neue offizielle Grundlage bestimmen

Nach dem Prüfen des Remotes ist der folgende Fetch eine bewusste Netzwerkaktion:

```sh
git fetch upstream
git rev-parse upstream/v3/master
```

Nutze die bisherige Commit-ID aus `upstream-lock.json` und die tatsächlich
zurückgegebene neue Commit-ID, um `git log` und `git diff` zwischen genau diesen
Revisionen zu lesen. Prüfe zuerst, ob der alte Stand ein Vorfahr des neuen ist.
Eine andere Beziehung erfordert eine Erklärung und darf nicht als gewöhnliche
Vorwärtsaktualisierung behandelt werden.

Der Name `v3/master` ist ein veränderlicher Verweis. Die neue vollständige Commit-ID
wird erst nach ihrer Untersuchung zur Dokumentationsgrundlage. Ein Versionsmakro in
einem Header oder eine README-Aussage allein beweist weder ein Release noch
getestete Kompatibilität.

## 3. Code-Aktualisierung und Dokumentation gemeinsam prüfen

Wie der Code in den Arbeitsbranch übernommen wird, hängt vom beauftragten Git-Ablauf
und vorhandenen Änderungen ab. Danach:

1. Lies geänderte öffentliche Header, Implementierungen, Grammatik, Build-Dateien,
   Submodulbindungen und Tests.
2. Überarbeite betroffene technische Kapitel anhand des neuen Codes. Achte besonders
   auf Aussagen zu Phasen, Besitzverhältnissen, optionalen Funktionen und Tests.
3. Richte Quellverweise auf überprüfte Zeilen des neuen Commits. Ersetze nicht einfach
   die alte SHA-Zeichenfolge in der gesamten Dokumentation.
4. Berücksichtige gelöschte oder umbenannte Dateien und geänderte Submodule ausdrücklich
   im Inventar und den Erklärungen.
5. Behalte für historische Sitzungsnotizen deren ursprüngliche Grundlage bei.
   Verweise für aktuelle Fakten auf die überarbeiteten technischen Kapitel.

## 4. Herkunftsdaten neu erzeugen

`upstream-lock.json` enthält Commit, Baum, Beobachtungsdatum, Inventarzahlen und
Submodulbindungen. Diese Werte stammen aus der tatsächlichen Git-Historie, nicht
aus einer geschätzten Versionsnummer. `source-index.tsv` hat die Spalten
`mode`, `type`, `object`, `path` und enthält jeden mit `git ls-tree -r` gelesenen
Eintrag im neuen Basis-Commit.

Die folgende Python-Sequenz erzeugt **nur das Inventar** neu, nachdem der neue
überprüfte Commit bereits bewusst in `upstream-lock.json` eingetragen wurde.
Sie aktualisiert Erklärungen, Metadaten oder Submodule nicht automatisch.

```sh
python3 - <<'PY'
import json
import subprocess
from pathlib import Path

lock = json.loads(Path('.codex/context/upstream-lock.json').read_text(encoding='utf-8'))
commit = lock['upstream']['commit']
raw = subprocess.check_output(['git', '--no-lazy-fetch', 'ls-tree', '-r', '-z', commit])
rows = ['mode\ttype\tobject\tpath\n']
for item in raw.split(b'\0'):
    if item:
        metadata, path = item.decode('utf-8').split('\t', 1)
        mode, kind, oid = metadata.split()
        rows.append(f'{mode}\t{kind}\t{oid}\t{path}\n')
Path('.codex/audit/source-index.tsv').write_text(''.join(rows), encoding='utf-8')
PY
```

Dieser Pflegebefehl schreibt die angegebene Datei. Ein gewöhnlicher Lauf von
`verify-context.py` liest ausschließlich Dateien.

## 5. Abschließende Prüfungen ausführen und Ergebnis übergeben

Die Kontextprüfung muss mit den aktualisierten Daten erfolgreich sein. Sind
Produktänderungen beabsichtigt, meldet der normale Lauf die Unterschiede;
`--strict-source` ist nur passend, wenn alles außerhalb der Kontextdateien exakt
mit der festgehaltenen offiziellen Quelle übereinstimmen soll.

Führe anschließend die für die Code-Änderung geeigneten Builds und Regressionstests
aus [build-and-test.md](build-and-test.md) tatsächlich aus und halte ihren Umfang fest.
Aktualisiere `current-state.md`, Entscheidungen und offene Aufgaben. Stelle eine
neue Quellgrundlage nicht anhand von Testergebnissen einer älteren Revision als
frisch überprüft dar.
