# cJSON

## 1. Kurzfazit
- Leichtgewichtig und populär, aber funktional schmaler.
- YAJL-Ersatz nur eingeschränkt; für umfassenden Backend-Anspruch eher schwach.

## 2. Projektstatus und Wartbarkeit
- Repo: <https://github.com/DaveGamble/cJSON>
- Doku: im Repo (`README.md`) / CMake docs.
- Stand 2026-04-07: aktiv, Release v1.7.19 (2025-09-09).

## 3. Technisches Modell
- Sprache: C.
- Modell: DOM (`cJSON*`), print/parse API.
- Kein robustes YAJL-äquivalentes SAX/incremental Callback-Modell im Zentrum.
- Fehler: globaler Fehlerzeiger (`cJSON_GetErrorPtr`) etc.

## 4. Build-/Integration
- Systempaket: oft verfügbar, aber Versionen variieren.
- Bundled: einfach (kleiner Footprint).

## 5. Passung zum Repo
- Tests/DOM und einfache Writer-Fälle möglich.
- Request-body chunk parser schwierig ohne Umbaumaßnahmen.
- Numeric/typing Verhalten muss sorgfältig geprüft werden.

## 6. Modul vs. Systempaket
- Eher **bundled optional** vertretbar wegen Kleinheit; für Distros trotzdem system bevorzugbar.

## 7. Integrationsstrategie
- Nur als optionales minimalistisches Backend für nicht-kritische Pfade.

## 8. Migrationsaufwand
- **hoch** für vollständigen YAJL-Ersatz (insb. streaming/incremental).

## 9. Empfehlung
- **nur optional / experimentell**.
