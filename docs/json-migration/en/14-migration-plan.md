# Migration Plan (Phased)

## Phase 0 — Baseline capture
- Freeze YAJL-based outputs and regression references.

## Phase 1 — Introduce abstraction layer (no behavior change)
- Add `src/json_backend/` interfaces.
- Implement YAJL adapters first.
- Switch existing callers to abstraction.

## Phase 2 — Decouple tests from YAJL tree types
- Refactor `test/common`, `test/unit`, `test/regression` to backend-agnostic DOM/parser facade.

## Phase 3 — First alternative C backend (json-c)
- Implement json-c parser/writer/DOM adapters.
- Add CI jobs and compare with YAJL baseline.

## Phase 4 — Additional C backends (jansson, yyjson)
- Validate behavior around number, UTF-8, and error messages.

## Phase 5 — Optional C++ backends
- Introduce nlohmann/json or jsoncpp for C++ test/tooling paths.
- Keep simdjson/glaze as experimental/specialized options.

## Main technical risks
- Number text/typing drift.
- UTF-8 behavior differences.
- Error text/position differences affecting tests.
- Key ordering differences in generated JSON.

## Recommended first implementation sequence
1. Abstraction + YAJL adapter (compatibility phase).
2. json-c adapter.
3. Full regression diff against YAJL baseline.
