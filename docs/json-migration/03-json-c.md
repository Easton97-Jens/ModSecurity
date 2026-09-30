# JSON-C (json-c)

## 1. Kurzfazit
- **Starker YAJL-Ersatzkandidat** für C-nahe Integrationen.
- Realistisch für dieses Repo: **ja**, besonders wegen C-API und guter Paketverfügbarkeit.

## 2. Projektstatus und Wartbarkeit
- Repo: <https://github.com/json-c/json-c>
- Doku: <https://json-c.github.io/json-c/>
- Stand 2026-04-07: aktive Commits, aktueller Tag `json-c-0.18-20240915`.

## 3. Technisches Modell
- Sprache: C.
- Modelle: DOM (`json_object`), Tokener (`json_tokener`) inkl. incremental parsing.
- Writer: `json_object_to_json_string_ext` etc.
- Fehler: Tokener-Error-Codes/Position.
- Memory: refcounted objects.

## 4. Build-/Integration
- Systempaket: sehr gut in Linux/BSD Distros etabliert.
- Bundled: möglich, aber compiled C-lib + ABI-Themen beachten.
- pkg-config/CMake: vorhanden.

## 5. Passung zum Repo
- RequestBodyProcessor: gut mapbar über `json_tokener_parse_ex` (incremental), Eventfluss müsste via DOM-walk oder custom tokener handling emuliert werden.
- Writer in `transaction.cc`/`modsecurity.cc`: gut ersetzbar.
- Tests: `yajl_tree_parse` → `json_tokener_parse_ex` + DOM access.

Zu beachten:
- YAJL-SAX callbacks sind direkter als json-c Standardpfad; ggf. eigener Event-Adapter nötig.

## 6. Modul vs. Systempaket
- Primär **Systempaket** empfehlen (Security updates, distro compatibility).
- Optional bundled für spezielle Plattformen.

## 7. Integrationsstrategie
- Backend `jsonc_stream_parser` + `jsonc_writer` + `jsonc_dom`.
- Start mit Tests + Writer, dann Request-Body incremental parser.

## 8. Migrationsaufwand
- **mittel** (gegenüber anderen Alternativen relativ günstig).

## 9. Empfehlung
- **ja (starker Kandidat)**, besonders als erster produktiver YAJL-Austausch-PoC.
