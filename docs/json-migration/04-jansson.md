# JANSSON

## 1. Kurzfazit
- Sehr solide C-Library mit klarer API.
- YAJL-Ersatz: gut für DOM/Writer; SAX/incremental weniger direkt.

## 2. Projektstatus und Wartbarkeit
- Repo: <https://github.com/akheron/jansson>
- Doku: <https://jansson.readthedocs.io/>
- GitHub API Stand 2026-04-07: aktiv, Release v2.15.0 (2026-01-24).

## 3. Technisches Modell
- Sprache: C.
- Modell: DOM-zentriert (`json_t*`), dump/load APIs.
- Incremental: primär buffer/file parse; YAJL-artiges Callback-SAX nicht Kernmodell.
- Fehler: `json_error_t` mit Zeile/Spalte/Text.
- Memory: refcounting.

## 4. Build-/Integration
- Systempaket: gut verfügbar.
- Bundled: möglich, aber weniger nötig als bei Nischenlibs.
- pkg-config vorhanden.

## 5. Passung zum Repo
- Testmigration relativ geradlinig.
- Writer-Migration machbar.
- RequestBody-Chunkparser erfordert Architekturänderung (z. B. sammeln + parse am Ende oder eigener incremental Adapter).

## 6. Modul vs. Systempaket
- **Systempaket bevorzugt**; optional bundled.

## 7. Integrationsstrategie
- Zuerst DOM+Writer backend.
- Für Request-Body ggf. getrenntes Streaming-Backend (json-c/yyjson) statt alles mit jansson zu erzwingen.

## 8. Migrationsaufwand
- **mittel bis hoch** (abhängig vom Umgang mit incremental parsing).

## 9. Empfehlung
- **ja, aber ideal als DOM/Writer-Backend** oder in Multi-Backend-Architektur.
