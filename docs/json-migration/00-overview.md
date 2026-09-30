# JSON-Migration Overview (YAJL → austauschbare Backends)

Stand: **2026-04-07**.

## 1) Ist-Zustand im Repository

## 1.1 YAJL ist aktuell tief integriert (nicht nur ein einzelner Parser-Aufruf)

YAJL wird im aktuellen Code in **drei Rollen** genutzt:

1. **Streaming/SAX Parsing für Request-Body JSON** (`src/request_body_processor/json.cc/.h`)  
   - `yajl_alloc`, `yajl_parse`, `yajl_complete_parse` mit Callback-Tabelle (`yajl_callbacks`).
   - ModSecurity baut daraus Pfad-basierte ARGS-Namen (`json.foo.bar.array_0...`) und speichert Werte als Strings.
   - Parser ist chunkbasiert (inkrementell) via `processChunk()` + `complete()`.

2. **JSON-Serialization / Writer für Runtime-Ausgaben**  
   - `src/transaction.cc::Transaction::toJSON()` (Audit-Log JSON).  
   - `src/modsecurity.cc::ModSecurity::processContentOffset()` (JSON-Output für Offsets).  
   - Extensive Nutzung von `yajl_gen_*` APIs.

3. **DOM/Tree Parsing + teilweise Writer in Testframework**  
   - `test/common/modsecurity_test.cc` (`yajl_tree_parse`).
   - `test/unit/*`, `test/regression/*`: starkes `yajl_val`/`YAJL_GET_*` Coupling.
   - `test/regression/regression_test.cc` nutzt zusätzlich `yajl_gen_*` für Formatierung.

**Wichtiger Punkt:** YAJL ist nicht nur Build-Dependency, sondern prägt Modell/Typen/Fehlerpfade über viele Schichten.

## 1.2 Betroffene Build-Systeme

- **Autotools (primär):** `configure.ac`, `build/yajl.m4`, `build/msc_find_lib.m4`, mehrere `Makefile.am`.  
  Detection erfolgt über `MSC_CHECK_LIB([YAJL], [yajl2 yajl], [yajl/yajl_parse.h], [yajl], [-DWITH_YAJL])`.
- **Windows/CMake:** `build/win32/CMakeLists.txt`. Dort `HAVE_YAJL` derzeit auf 1 gesetzt (als mandatory gedacht) und `find_package(yajl REQUIRED)`.
- **pkg-config Export:** `modsecurity.pc.in` enthält `@YAJL_LDADD@` in `Libs.private`.
- **Conan (win32):** `build/win32/conanfile.txt` pinnt `yajl/2.1.0`.

## 1.3 Aktuelle Parser-/Writer-Eigenschaften (funktional)

- **SAX/Event-basiert:** ja (RequestBody JSON).
- **DOM/Tree:** ja (Tests via `yajl_tree_parse` / `yajl_val`).
- **Inkrementelles Parsing:** ja (chunk feeding).
- **Validierung:** syntaktisch (JSON-Parse), keine JSON-Schema-Validierung.
- **Writer/Serialization:** stark genutzt (`yajl_gen_*`).
- **Number Handling:** im Request-Body-Parser als String übernommen (`yajl_number`), in Tests teilweise `YAJL_GET_INTEGER`/`YAJL_GET_NUMBER`.
- **UTF-8/Unicode:** YAJL-validiert standardmäßig; TODO-Hinweis im Code, dass UTF8-Validierung optional werden sollte.
- **Error Reporting:** YAJL-Fehlerstring via `yajl_get_error`; zusätzlicher eigener Depth-Limit-Hinweis.

## 1.4 Sprache / API / ABI / Packaging

- Kernprojekt ist C++17 (`configure.ac` via `AX_CXX_COMPILE_STDCXX(17, ...)`, README nennt C++17).
- Es gibt C-API-Oberfläche, aber JSON intern ist derzeit in C++-Implementierung mit C-Lib YAJL.
- YAJL wirkt sich auf Build-Optionen, test utilities und teils Featureverfügbarkeit aus.
- Für Distributionen ist YAJL heute ein erwarteter Systembaustein (README nennt YAJL mandatory).

## 1.5 Bruchstellen bei direktem Wechsel

Bei 1:1-Tausch ohne Abstraktion brechen sicher:

- `src/request_body_processor/json.{h,cc}` (Callbacksignaturen + Parsing-Lifecycle).
- `headers/modsecurity/transaction.h` (LOGFY-Makros hängen direkt an `yajl_gen_*`).
- `src/transaction.cc`, `src/modsecurity.cc` (Writer-Calls massiv hardcoded).
- `test/common`, `test/unit`, `test/regression` (direkte `yajl_val`-Datenstrukturen).
- Build-Makros in `configure.ac`/`build/*.m4` + Windows CMake + conanfile.

## 2) Architekturfrage: direkte Ersetzung vs. Backend-Abstraktion

## 2.1 Empfehlung: interne Abstraktionsschicht ist sinnvoll

Ein direkter Ersatz (z. B. `sed yajl→json-c`) ist unrealistisch, weil YAJL drei unterschiedliche Modelle bedient (SAX, DOM, Writer).

**Empfohlenes Zielbild:**

- `json_backend/` als interne Schicht mit separaten Facetten:
  - `JsonStreamParser` (incremental callbacks)
  - `JsonWriter` (object/array/key/value)
  - `JsonDom` (nur für Tests oder optional Runtime)
- Capability-Flags pro Backend statt erzwungener Volluniformität.

## 2.2 Kleinste gemeinsame Schnittmenge

Realistisch vereinbar für alle Kandidaten:

- Parse whole buffer → success/error.
- Stream callback Ereignisse (bei manchen nur mit Adapter möglich).
- Writer: object/array open/close; string/number/bool/null schreiben.
- Error text + position (falls verfügbar).

Nicht sauber vereinbar über alle Libraries:

- Zero-copy On-Demand DOM (simdjson) vs mutable DOM (json-c/jansson).
- Typed Binding/Reflection (glaze, nlohmann/json) vs C-SAX.
- Vollwertiges incremental parsing ist je Library sehr unterschiedlich.

## 2.3 C- und C++-Backends trennen?

Ja, pragmatisch sinnvoll:

- **C-Backends** (json-c, jansson, cJSON, yyjson) passen besser zu heute YAJL-nahen Call-Flows und Packaging in Distros.
- **C++-Backends** (nlohmann/json, jsoncpp, jsoncons, simdjson, glaze, rapidjson) sind eher attraktiv für Tests/Tools oder C++-spezifische Pfade.

Empfohlene Buildoptionen:

```text
--with-json-parser=auto|yajl|json-c|jansson|yyjson|simdjson|rapidjson|...
--with-json-writer=auto|yajl|json-c|yyjson|rapidjson|jsoncpp
--with-json-dom=auto|yajl|json-c|jansson|nlohmannjson|jsoncpp|simdjson
--with-json-backend-mode=system|bundled|auto
```

## 2.4 Capability-Flags statt reine Einheits-API

Empfohlen:

- `HAS_STREAMING`
- `HAS_INCREMENTAL_PARSE`
- `HAS_DOM`
- `HAS_WRITER`
- `HAS_TYPED_BINDING`
- `HAS_ZERO_COPY_DOM`

Damit lassen sich Backend-spezifische Stärken nutzen, ohne künstlich alles auf kleinsten Nenner zu reduzieren.

## 3) C-Backends vs C++-Backends (Kurzvergleich)

- **Starke C-Kandidaten:** json-c, jansson, yyjson.
- **Eingeschränkt:** cJSON (schlank, aber weniger robust/featurevoll für SAX/incremental).
- **Starke C++-Kandidaten:** nlohmann/json (ergonomisch, DOM-zentriert), simdjson (Performance/On-Demand), jsoncpp (klassisch), jsoncons (modern/umfangreich).
- **Spezialfall RapidJSON:** technisch passend (SAX+DOM+Writer), aber Release-Stagnation (letzte Release 2016) = Governance-Risiko.
- **Spezialfall glaze:** starkes typed C++-Binding, aber konzeptionell weit weg von YAJL-Eventmodell.

## 4) Vendor/Bundled vs Systempaket

Grundsatz für ModSecurity:

- **Systempaket bevorzugen** für distro-freundliche Security-Updates und etablierte Packaging-Pfade.
- **Optional bundled** für CI-Reproduzierbarkeit / Plattformen ohne aktuelle Pakete.
- Bei C++ header-only libs (nlohmann/json, jsoncons, glaze) ist „bundled snapshot“ technisch einfach, erhöht aber Third-party Governance-Aufwand.

## 5) Priorisierung (empfohlen)

1. **PoC 1:** json-c (C, breit paketiert, aktiv)  
2. **PoC 2:** jansson (C, sauber API, aktiv)  
3. **PoC 3:** yyjson (C, schnell, modern)  
4. **PoC 4:** nlohmann/json oder jsoncpp für Tests/DOM-Schicht

Nur experimentell:
- simdjson (für Hochperformance-Pfade, nicht als direkter YAJL-Ersatz)
- glaze (typed-serialization Szenarien)
- RapidJSON (nur mit klarer Maintenance-Strategie)
