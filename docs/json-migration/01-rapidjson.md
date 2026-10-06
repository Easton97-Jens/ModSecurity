# RAPIDJSON

## 1. Kurzfazit
- **Grundsätzlich technisch geeignet** (SAX + DOM + Writer vorhanden).
- **Realistischer YAJL-Ersatz:** ja, API-Mapping ist machbar.
- **Konzept-Fit für dieses Repo:** mittel – C++-lastig, aber ModSecurity-Kern ist C++17.
- **Hauptproblem:** Release-/Maintenance-Signal (siehe Abschnitt 2).

## 2. Projektstatus und Wartbarkeit
- Offizielles Repo: <https://github.com/Tencent/rapidjson>
- Offizielle Doku: <https://rapidjson.org/>
- Beobachtung (GitHub API, 2026-04-07): letzte Release **v1.1.0 (2016-08-25)**, obwohl Commits vorhanden.
- Risiko: fehlende Releases erschweren Security-/Packaging-Vertrauen (insb. Enterprise/Distro).

## 3. Technisches Modell
- Sprache: C++ (header-only, template-heavy).
- Modelle: DOM, SAX (`Reader`/`Handler`), Writer (`Writer`, `PrettyWriter`).
- Incremental: SAX kann streamartig gefüttert werden, aber Adapter-Design nötig.
- Error Handling: ParseResult + ErrorOffset.
- Memory: eigener Allocator-Ansatz; DOM kann allocator-sensitive sein.
- Threading: reentrant pro Dokument/Reader-Instanz.

## 4. Build-/Integration
- Systempaket: je Distribution unterschiedlich gut gepflegt.
- Bundled: sehr einfach (header-only), aber Third-party Governance nötig.
- pkg-config: meist nein; CMake/Include-Only Integration üblich.

## 5. Passung zum konkreten Repo
Zu ersetzen wären:
- `src/request_body_processor/json.cc`: YAJL-callbacks → RapidJSON SAX `Handler`.
- `src/transaction.cc`, `src/modsecurity.cc`, `headers/modsecurity/transaction.h`: `yajl_gen_*` → `Writer` API.
- Tests (`test/common`, `test/unit`, `test/regression`): `yajl_tree_parse`/`yajl_val` → RapidJSON DOM.

Direkt übertragbar:
- SAX-Eventfluss (map start/end, key, string, number, bool, null).

Nicht direkt:
- YAJL-spezifische Error-Strings, some numeric/raw access patterns.

## 6. Modul vs. Systempaket
- Empfehlung: **beides optional**, aber Standard eher **system**, falls gepflegte Pakete vorhanden.
- Für deterministische CI kann vendored snapshot sinnvoll sein.
- Wegen alter Release-Signale sollte vendoring nur mit aktivem eigener Patch-/Tracking-Policy erfolgen.

## 7. Integrationsstrategie (Skizze)
```cpp
class RapidJsonStreamAdapter : public JsonStreamParser {
  // map events -> existing addArgument/path logic
};
class RapidJsonWriterAdapter : public JsonWriter {
  // Object(), Key(), String(), Int64(), EndObject() ...
};
```
Configure-Flag:
```text
--with-json-parser=rapidjson --with-json-backend-mode=system|bundled
```

## 8. Migrationsaufwand
- **hoch** (Parser + Writer + Test-DOM).

## 9. Empfehlung
- **nur optional / experimentell**, solange Maintenance-Strategie (Release-Stagnation) nicht organisatorisch abgesichert ist.
- Priorisierung: **nur mit Abstraktionsschicht**.
