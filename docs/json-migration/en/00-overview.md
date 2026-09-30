# JSON Migration Overview (YAJL → Replaceable Backends)

Date: **2026-04-07**.

## 1) Current state in this repository

YAJL is currently used in **three distinct roles**:

1. **Streaming/SAX parsing for request-body JSON** (`src/request_body_processor/json.cc/.h`)
   - Uses `yajl_alloc`, `yajl_parse`, `yajl_complete_parse` with `yajl_callbacks`.
   - Converts JSON paths to ModSecurity argument keys (`json.foo.bar.array_0...`) and stores values as strings.
   - Parsing is incremental (`processChunk()` + `complete()`).

2. **JSON serialization/writer for runtime output**
   - `src/transaction.cc::Transaction::toJSON()`.
   - `src/modsecurity.cc::ModSecurity::processContentOffset()`.
   - Extensive usage of `yajl_gen_*` APIs.

3. **DOM/tree parsing (and partial writer use) in tests**
   - `test/common/modsecurity_test.cc` (`yajl_tree_parse`).
   - `test/unit/*`, `test/regression/*` depend heavily on `yajl_val` and `YAJL_GET_*`.

This means YAJL is deeply embedded in behavior, data model, and error handling.

## 2) Build systems impacted

- **Autotools (primary)**: `configure.ac`, `build/yajl.m4`, `build/msc_find_lib.m4`, and multiple `Makefile.am` files.
- **Windows CMake**: `build/win32/CMakeLists.txt` currently treats YAJL as mandatory (`HAVE_YAJL` + `find_package(yajl REQUIRED)`).
- **pkg-config**: `modsecurity.pc.in` exports `@YAJL_LDADD@`.
- **Conan (win32)**: `build/win32/conanfile.txt` pins `yajl/2.1.0`.

## 3) Current parser/writer features in use

- SAX/event parsing: yes.
- DOM/tree parsing: yes (tests).
- Incremental parsing: yes.
- Serialization/writer: yes (heavy usage).
- Number handling: request-body parser currently stores numeric literals as strings.
- UTF-8 validation: YAJL-driven today; code includes TODO notes around optional behavior.
- Error reporting: YAJL error strings + custom depth-limit suffix.

## 4) Why direct 1:1 replacement is risky

A direct swap will break multiple layers:

- `src/request_body_processor/json.{h,cc}` (callback model + lifecycle).
- `headers/modsecurity/transaction.h` (LOGFY macros depend on `yajl_gen_*`).
- `src/transaction.cc`, `src/modsecurity.cc` (writer API calls).
- Test framework under `test/common`, `test/unit`, `test/regression` (YAJL tree types).
- Build macros and packaging definitions.

## 5) Architecture recommendation

Use an internal abstraction instead of direct replacement:

- `JsonStreamParser` (incremental streaming)
- `JsonWriter`
- `JsonDom` (primarily for tests)

Prefer capability flags over forced feature parity:

- `HAS_STREAMING`
- `HAS_INCREMENTAL_PARSE`
- `HAS_DOM`
- `HAS_WRITER`
- `HAS_TYPED_BINDING`
- `HAS_ZERO_COPY_DOM`

## 6) C backends vs C++ backends

- Strong C candidates: **json-c**, **jansson**, **yyjson**.
- Strong C++ candidates (domain-specific): **nlohmann/json**, **jsoncpp**, **jsoncons**.
- **RapidJSON**: technically strong, but release cadence risk.
- **simdjson/glaze**: useful for specific scenarios, not direct YAJL drop-ins.

## 7) Packaging model recommendation

- Prefer **system packages** by default (security updates, distro friendliness).
- Allow optional **bundled/vendor mode** for reproducible CI or missing platform packages.

## 8) Priority order (recommended)

1. json-c
2. jansson
3. yyjson
4. nlohmann/json or jsoncpp for C++/test-focused DOM paths
