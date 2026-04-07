# Verbesserung von Test- und Check-Frameworks mit Fuzz Testing und Sanitizern

## Einleitung: Warum klassische Tests allein nicht reichen

In sicherheitskritischen C/C++-Systemen sind Unit- und Integrationstests unverzichtbar, aber sie decken naturgemäß nur bekannte oder antizipierte Verhaltensmuster ab. Gerade bei Parsern, Protokoll-Stacks, Serialisierungscode, RegEx-Engines oder Komponenten mit komplexem Zustandsraum entstehen Fehler häufig in Randbereichen, die man manuell kaum vollständig modellieren kann.

Typische Risiken in nativen Toolchains:

- Speicherfehler (Out-of-Bounds Read/Write, Use-After-Free, Double-Free)
- Undefined Behavior (Integer Overflow in kritischen Pfaden, invalid shifts, misaligned access)
- Datenrennen in nebenläufigem Code
- Logikfehler, die nur unter ungewöhnlichen Input-Sequenzen sichtbar werden

Fuzz Testing (insbesondere coverage-guided mit libFuzzer) und Sanitizer (ASan/UBSan/TSan) erweitern deshalb klassische Teststrategien um eine automatisierte, tiefere Fehlersuche.

---

## Technischer Hintergrund

### Klassische Test-Frameworks und ihre Grenzen

Klassische Frameworks (z. B. GoogleTest, Catch2, CTest-basierte Pipelines) arbeiten überwiegend mit *beispielbasierten* Testfällen:

1. Arrange: definierter Zustand
2. Act: Funktionsaufruf
3. Assert: erwartetes Ergebnis

Das ist stark bei regressionssicheren, fachlich klaren Szenarien. Grenzen treten auf bei:

- **Input-Dimensionalität**: Millionen relevanter Kombinationsmöglichkeiten sind praktisch nicht manuell abdeckbar.
- **Unbekannte Unbekannte**: Fehler, deren Trigger nicht vorhergesehen wurde, werden selten getestet.
- **Parser-/Binary-Formate**: Kleine Mutationen können tief im Codepfad neue Zustände aktivieren.
- **Nebenläufigkeit**: Nichtdeterministische Interleavings sind mit klassischen Tests schwer reproduzierbar.

### Was ist Fuzz Testing?

Fuzz Testing führt ein Zielprogramm wiederholt mit vielen automatisch erzeugten oder mutierten Inputs aus, um Crashes, Hangs und Security-relevante Fehlzustände zu finden.

#### Random Fuzzing vs. Coverage-Guided Fuzzing

**Random Fuzzing**

- erzeugt Inputs zufällig (oder mit einfachen Heuristiken)
- einfach einzurichten, aber oft geringe Codeabdeckung
- „verbraucht“ viel CPU in bereits bekannten Pfaden

**Coverage-Guided Fuzzing (CGF)**

- instrumentiert das Zielprogramm zur Erfassung der Codeabdeckung
- bevorzugt Inputs, die neue Basic Blocks/Kanten erreichen
- lernt schrittweise, komplexere Zustände zu erreichen
- deutlich effizienter beim Finden tiefer Bugs

### Funktionsweise von libFuzzer

libFuzzer ist ein in-process, coverage-guided Fuzzer für LLVM/Clang.

Kernprinzip:

- Ein Fuzz-Target implementiert `LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)`.
- libFuzzer ruft diese Funktion in einer Schleife mit mutierten Inputs auf.
- LLVM-Instrumentierung liefert Coverage-Signale.
- Inputs, die neue Coverage erzeugen, werden im Corpus behalten.

Minimales Lebenszyklusmodell:

1. Starte mit Seeds (optional)
2. Mutiere Seed/Input
3. Führe Target aus
4. Sammle Coverage + Crash-Information
5. Behalte „interessante“ Inputs
6. Wiederhole

Wichtig in der Praxis:

- Die Target-Funktion muss deterministisch sein (soweit möglich).
- Kein globaler leakender Zustand zwischen Iterationen.
- Möglichst keine teuren I/O-Operationen im Hot Path.

### Welche Probleme lassen sich finden?

1. **Speicherfehler**
   - Heap/Stack Buffer Overflow
   - Use-After-Free
   - Double Free
2. **Undefined Behavior**
   - Signed Integer Overflow
   - Null Pointer Dereference
   - invalid enum/value conversions
3. **Logik- und Robustheitsfehler**
   - unerwartete Parserzustände
   - unvollständig validierte Input-Pfade
   - algorithmische Pathologien (extreme Laufzeiten)
4. **Nebenläufigkeitsprobleme** (mit TSan in passender Teststrategie)
   - Datenrennen
   - inkonsistente Synchronisationsannahmen

### Rolle der Sanitizer

#### AddressSanitizer (ASan)

- erkennt Speicherzugriffsfehler zur Laufzeit
- nutzt Shadow Memory und Red Zones
- liefert sehr präzise Stacktraces für Heap/Stack OOB, UAF etc.

#### UndefinedBehaviorSanitizer (UBSan)

- detektiert UB-Kategorien, die sonst häufig still bleiben
- besonders wertvoll für sicherheitsrelevante C/C++-Pfadvalidierung

#### ThreadSanitizer (TSan)

- erkennt Datenrennen und problematische Synchronisationsmuster
- hoher Laufzeit-/Speicher-Overhead, aber unverzichtbar für parallelisierten Code

### Zusammenspiel: Fuzzing + Sanitizer

Coverage-guided Fuzzing maximiert Pfadexploration; Sanitizer wandeln subtile Laufzeitfehler in harte, diagnostizierbare Signale um. Genau diese Kombination ist in Sicherheitskontexten besonders effektiv: Der Fuzzer findet den Trigger, der Sanitizer liefert die evidenzbasierte Diagnose.

---

## Praktischer Teil (Hands-on)

### Beispiel 1: Einfaches libFuzzer-Target (C++)

```cpp
// fuzz_parse.cpp
#include <cstdint>
#include <cstddef>
#include <cstring>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < 4) return 0;

    // Simuliere Parser-Magic
    if (data[0] == 'M' && data[1] == 'O' && data[2] == 'D' && data[3] == 'S') {
        // Intentional bug: out-of-bounds read, wenn size == 4
        volatile uint8_t x = data[10];
        (void)x;
    }

    return 0;
}
```

Build mit Clang + libFuzzer + ASan:

```bash
clang++ -g -O1 -fno-omit-frame-pointer \
  -fsanitize=fuzzer,address \
  fuzz_parse.cpp -o fuzz_parse
```

Start:

```bash
./fuzz_parse -runs=0
```

Effekt:

- libFuzzer exploriert Inputs und entwickelt mutierte Varianten
- bei Treffer auf das Trigger-Muster meldet ASan einen präzisen OOB-Report

### Beispiel 2: UBSan ergänzen

```bash
clang++ -g -O1 -fno-omit-frame-pointer \
  -fsanitize=fuzzer,undefined,address \
  fuzz_parse.cpp -o fuzz_parse_ubsan
```

Damit werden zusätzlich UB-Klassen sichtbar, die ohne Instrumentierung oft unbemerkt bleiben.

### Beispiel 3: Typischer Bug – Buffer Overflow

```cpp
#include <cstdint>
#include <cstddef>
#include <cstring>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    char buf[8];
    if (size > 0) {
        // Klassischer Fehler: ungeprüfte Kopierlänge
        memcpy(buf, data, size); // ASan triggert bei size > 8
    }
    return 0;
}
```

Dieser Fehler kann in normalem Testbetrieb „zufällig“ unentdeckt bleiben, wird unter Fuzzing + ASan jedoch schnell reproduzierbar.

---

## CI/CD-Integration

Ein praxistaugliches Setup trennt schnelle Gate-Checks von längeren Fuzz-Kampagnen:

1. **PR-Pipeline (kurz, deterministisch)**
   - Build der wichtigsten Fuzz-Targets
   - Smoke-Fuzzing pro Target (z. B. 30–120 Sekunden)
   - Artefakte: Crash-Inputs, Logs

2. **Nightly/Continuous Fuzzing (langlaufend)**
   - mehrere Stunden Laufzeit
   - Corpus-Pflege (Minimierung, Dedup)
   - Trendmetriken: Coverage-Wachstum, Unique Crashes

3. **Crash-Handling-Prozess**
   - Auto-Ticketing mit reproduzierbarem Input
   - Priorisierung nach Ausnutzbarkeit/Reachability
   - Regressionstest nach Fix (Crash-Seed als Testfall)

Beispielhafte CI-Schritte:

```bash
# 1) Build
clang++ -g -O1 -fno-omit-frame-pointer -fsanitize=fuzzer,address fuzz_parse.cpp -o fuzz_parse

# 2) Kurzer Lauf im CI (z. B. 60s)
./fuzz_parse -max_total_time=60 -artifact_prefix=./artifacts/

# 3) Reproduktion eines Crashes
./fuzz_parse ./artifacts/crash-123456
```

---

## Bewertung & Best Practices

### Vorteile

- Hohe Bug-Fundrate in input-getriebenen Komponenten
- Sehr gute Kombination aus Exploration (Fuzzer) und Diagnose (Sanitizer)
- Frühes Auffinden sicherheitskritischer Defekte
- Automatisierbar und gut in DevSecOps integrierbar

### Nachteile

- Setup-Komplexität (Targets, Build-Flags, Corpus-Strategie)
- Laufzeit-/Ressourcenkosten durch Instrumentierung
- Fuzzing findet primär das, was erreichbar ist (Reachability-Limit)
- Triage-Aufwand bei vielen ähnlichen Crashes

### Wann lohnt sich Fuzzing besonders?

- Parser, Decoder, Protokoll-Handler, Datei-/Netzwerk-Input
- Legacy-C/C++-Code mit unklarer Input-Historie
- Security-kritische Libraries (Krypto-Parsing, Auth-Token-Parsing etc.)
- Systeme mit hoher Exploit-Relevanz

### Best Practices

1. **Klein starten, systematisch ausbauen**
   - zuerst „hot“ Angriffsflächen targeten
2. **Always-on Sanitizer in Test-Builds**
   - ASan/UBSan als Standard in non-release Profilen
3. **Corpus-Management etablieren**
   - deduplizieren, minimieren, seeden
4. **Determinismus priorisieren**
   - vermeidet Flaky Findings
5. **Unit Tests + Fuzzing kombinieren**
   - Unit Tests sichern spezifizierte Regeln
   - Fuzzing entdeckt unbekannte Trigger
6. **Fixes in Regression überführen**
   - jeder reproduzierbare Crash wird ein permanenter Testfall

---

## Fazit

Fuzz Testing mit libFuzzer und Sanitizern verschiebt Testqualität von „wir prüfen bekannte Fälle“ zu „wir suchen systematisch unbekannte Fehlzustände“. Für C/C++-Codebasen mit Security-Anforderungen ist diese Kombination heute kein Nice-to-have mehr, sondern ein zentraler Baustein für robuste Softwarequalität.

## Referenzen

- Android Open Source Project: Fuzz with libFuzzer — https://source.android.com/docs/security/test/libfuzzer
- LLVM Project: libFuzzer documentation — https://llvm.org/docs/LibFuzzer.html
