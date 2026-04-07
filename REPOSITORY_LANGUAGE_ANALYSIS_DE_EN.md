# ModSecurity v3 Repository-Sprachanalyse / Repository Language Analysis

## Deutsch

**Hauptsprache:**  
C++

**Weitere Sprachen:**  
C (vor allem Beispiele/C-API-Nutzung), Shell (Build- und Test-Automation), M4/Autotools, CMake, JSON/Lua (primär Testdaten/-skripte)

**Analyse:**

### 1) Repository-Struktur
- **Kern-Implementierung:** `src/`, `headers/`
- **Build-System:** `configure.ac`, `Makefile.am`, `build.sh`, `build/win32/CMakeLists.txt`
- **Tests:** `test/` (C++-Test-Binaries plus viele JSON/Lua-Testfälle)
- **Skripte/Tools/Beispiele:** `tools/`, `examples/`, diverse `.sh`

### 2) Dateitypen (gesamt, aus Repository-Scan)
- Sehr häufig: `.h`, `.cc`
- Außerdem: viele `.json` (hauptsächlich in `test/test-cases`), einige `.lua`, `.sh`, `.m4`, `.cmake`, wenige `.c`

### 3) Syntax-Merkmale
- **C++-typisch:** `#include`, `namespace`, Klassenmethoden (`ModSecurity::ModSecurity`), `std::string`
- **C-Anteile:** vorhanden, aber v. a. in Beispielen (`examples/*/*.c`) bzw. C-API-Verwendung
- **Shell:** Shebangs wie `#!/bin/sh` / `#!/usr/bin/env bash` in Build/Test-Skripten
- **Testdaten:** JSON-Dateien mit Test-Szenarien; Lua-Dateien für Skript-/Regeltests

### 4) Build-System-Einordnung
- Autotools konfiguriert explizit C++ und C++17 (`AC_PROG_CXX`, `AX_CXX_COMPILE_STDCXX(17, ...)`)
- Windows-CMake-Projekt definiert `LANGUAGES CXX` und baut Bibliotheksquellen aus `src/*.cc`
- Build-Sprache ist **Hinweisgeber**, aber nicht die Hauptsprache des Produkts selbst

### 5) Gewichtung
- Die eigentliche Engine/Bibliothek (`libmodsecurity`) liegt in C++-Quellen unter `src/` und API-Headern in `headers/`
- JSON/Lua dominieren nur im Testdatenbereich numerisch, nicht als Kern-Implementierung
- C ist ergänzend (Beispiele/Interop), nicht dominierend im Kern

**Begründetes Fazit:**  
Die Hauptsprache des bereitgestellten ModSecurity-v3-Repositories ist **C++**. Das zeigen die Struktur der Kernverzeichnisse, die Quelllisten im Build, die sichtbare C++-Syntax in zentralen Dateien und die Build-Konfiguration auf C++17.

**Sicherheitsgrad der Einschätzung:**  
**hoch**

---

## English

**Primary language:**  
C++

**Other languages:**  
C (mainly examples/C API usage), Shell (build and test automation), M4/Autotools, CMake, JSON/Lua (primarily test data/scripts)

**Analysis:**

### 1) Repository structure
- **Core implementation:** `src/`, `headers/`
- **Build system:** `configure.ac`, `Makefile.am`, `build.sh`, `build/win32/CMakeLists.txt`
- **Tests:** `test/` (C++ test binaries plus many JSON/Lua test cases)
- **Scripts/tools/examples:** `tools/`, `examples/`, multiple `.sh` files

### 2) File types (overall, from repository scan)
- Most frequent: `.h`, `.cc`
- Also present: many `.json` (mostly in `test/test-cases`), some `.lua`, `.sh`, `.m4`, `.cmake`, and few `.c`

### 3) Syntax signals
- **C++ indicators:** `#include`, `namespace`, class methods (`ModSecurity::ModSecurity`), `std::string`
- **C usage:** present, but mostly in examples (`examples/*/*.c`) and C API usage
- **Shell:** shebangs like `#!/bin/sh` / `#!/usr/bin/env bash` in build/test scripts
- **Test data:** JSON scenario files; Lua scripts for rule/script testing

### 4) Build-system context
- Autotools explicitly configures C++ and C++17 (`AC_PROG_CXX`, `AX_CXX_COMPILE_STDCXX(17, ...)`)
- Windows CMake defines `LANGUAGES CXX` and builds library sources from `src/*.cc`
- Build language is a **signal**, but not itself the application’s primary implementation language

### 5) Weighting
- The actual engine/library (`libmodsecurity`) is in C++ sources under `src/` and API headers under `headers/`
- JSON/Lua are numerically heavy mainly in test data, not in core implementation
- C is supplementary (examples/interop), not dominant in the core

**Reasoned conclusion:**  
The primary language of this ModSecurity v3 repository is **C++**. This is supported by core directory layout, build source lists, visible C++ syntax in central files, and C++17-oriented build configuration.

**Confidence level:**  
**high**
