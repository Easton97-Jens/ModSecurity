# JSONCONS

## 1. Kurzfazit
- Moderne C++-Library mit breitem Feature-Set.
- YAJL-Ersatz: technisch möglich, aber für C-nahe Stellen eher indirekt.

## 2. Projektstatus und Wartbarkeit
- Repo: <https://github.com/danielaparker/jsoncons>
- Doku: <https://danielaparker.github.io/jsoncons/>
- Stand 2026-04-07: aktiv, Release v1.6.0 (2026-03-23).

## 3. Technisches Modell
- Sprache: C++ (header-only Schwerpunkt).
- Modelle: DOM, events, cursor/pull, serialization.
- Gute Flexibilität für Adapterdesign.

## 4. Build-/Integration
- Bundled sehr einfach (header-only).
- Systempaket weniger standardisiert als bei json-c/jsoncpp.

## 5. Passung zum Repo
- Für C++-Architekturmodernisierung stark.
- Für YAJL-nahe C-ähnliche Pfade nur via Adapter sinnvoll.

## 6. Modul vs. Systempaket
- Aufgrund Packaging-Lage häufig eher **bundled** praktikabel; system optional.

## 7. Integrationsstrategie
- Einsatz als C++-backend für DOM/Writer/optional cursor.
- Nicht als alleinige Pflichtdependency für alle Plattformen.

## 8. Migrationsaufwand
- **mittel bis hoch**.

## 9. Empfehlung
- **nur optional / C++-builds**, mit klarer Abstraktionsschicht.
