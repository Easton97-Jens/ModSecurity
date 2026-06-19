# GLAZE

## 1. Kurzfazit
- Moderne High-Performance C++ Reflection/Serialization-Library.
- Für YAJL-Ersatz im aktuellen Repo **konzeptionell nur begrenzt passend**.

## 2. Projektstatus und Wartbarkeit
- Repo: <https://github.com/stephenberry/glaze>
- Doku: Repo/docs.
- Stand 2026-04-07: sehr aktiv, Release v7.3.0 (2026-04-07).

## 3. Technisches Modell
- Sprache: C++20+ Fokus (wichtiger Punkt!).
- Modell: typed binding/reflection-first, serialisieren in structs.
- Nicht primär YAJL-artiges C-SAX callback parsing.

## 4. Build-/Integration
- Header-only orientiert, meist bundled/CMake.
- C++ Sprachstandard-Anforderungen können mit aktuellem C++17-Projekt kollidieren.

## 5. Passung zum Repo
- Aktuelles ModSecurity nutzt dynamische key/value und eventbasierte Pfade.
- Glaze spielt Stärke bei statisch typisierten DTOs aus; passt schlechter zu frei geformtem JSON der Request Bodies/Testfälle.

## 6. Modul vs. Systempaket
- Eher bundled; system packaging weniger etabliert.

## 7. Integrationsstrategie
- Wenn überhaupt: isoliert in optionalen Tools oder neuen C++20 Subkomponenten.
- Nicht als Primary Backend.

## 8. Migrationsaufwand
- **sehr hoch** bei Versuch als universeller YAJL-Ersatz.

## 9. Empfehlung
- **nicht empfehlenswert als Primärersatz**; höchstens experimentell in C++-Spezialpfaden.
