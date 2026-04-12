# simdjson evaluation

## 1. Locations
- **File:** `src/request_body_processor/json_backend_simdjson.cc`
  **Function:** `parseDocumentWithSimdjson`, `JsonBackendWalker::*`
  **Purpose:** Full simdjson backend path for request-body JSON parsing and event emission.
- **File:** `src/request_body_processor/json_adapter.cc`
  **Function:** `JSONAdapter::parse`
  **Purpose:** Dispatches to simdjson when the simdjson backend is selected.
- **File:** `src/Makefile.am`, `configure.ac`, `build/win32/CMakeLists.txt`
  **Function:** Build/backend selection
  **Purpose:** Includes simdjson backend and sources when selected.

## 2. Is the implementation correct?
- **Location:** `ondemand::parser` + `iterate(padded_string)` usage
  - **Rating:** KORREKT
  - **Short reason:** This matches the documented simdjson On-Demand usage with padded input.
  - **Evidence:** `src/request_body_processor/json_backend_simdjson.cc` (`parseDocumentWithSimdjson`); `others/simdjson/doc/basics.md`.

- **Location:** Thread-local parser reuse
  - **Rating:** KORREKT
  - **Short reason:** simdjson documents thread-local parser usage (one active document per thread at a time).
  - **Evidence:** `src/request_body_processor/json_backend_simdjson.cc` (`getReusableSimdjsonParser`); `others/simdjson/doc/basics.md`.

- **Location:** Error mapping (UTF-8 / parse / internal)
  - **Rating:** KORREKT
  - **Short reason:** Backend maps simdjson error classes to internal parse statuses in a consistent way.
  - **Evidence:** `src/request_body_processor/json_backend_simdjson.cc` (`fromSimdjsonError`).

- **Location:** Concrete functional errors
  - **Rating:** NICHT BEWIESEN
  - **Short reason:** No clear, reproducible functional API misuse is visible in the inspected code.
  - **Evidence:** Inspected full backend flow in `json_backend_simdjson.cc`.

## 3. Performance
- **Location:** `src/request_body_processor/json_backend_simdjson.cc:447-453`
- **Problem:** Each parse creates `simdjson::padded_string padded(input)` (extra copy + allocation).
- **Evidence:** Explicit copy path in code; simdjson docs describe copy-avoidance paths with guaranteed padding.
- **Rating:** PROBLEM

## 4. Memory / memory management
- **Location:** `src/request_body_processor/json_backend_simdjson.cc:101-115`
- **Problem:** `thread_local` parser persists for thread lifetime; capacity can remain high after large inputs.
- **Evidence:** Persistent `thread_local std::unique_ptr<...>`; simdjson docs note On-Demand capacity does not auto-shrink in long-running processes.
- **Rating:** PROBLEM

## 5. Simple simdjson conclusion
Overall the simdjson implementation is correct. The most important issue is the forced padded-string copy on every parse. The most important optimization is adding a safe zero-copy/padded-view path with guaranteed padding and lifetime. Parser reuse is correct, but it can keep memory capacity over time.

# jsoncons evaluation

## 1. Locations
- **File:** `src/request_body_processor/json_backend_jsoncons.cc`
  **Function:** `parseDocumentWithJsoncons`, `emitEvent`, `RawJsonTokenCursor`
  **Purpose:** Full jsoncons backend path for request-body JSON parsing and event emission.
- **File:** `src/request_body_processor/json_adapter.cc`
  **Function:** `JSONAdapter::parse`
  **Purpose:** Dispatches to jsoncons when the jsoncons backend is selected.
- **File:** `src/Makefile.am`, `configure.ac`, `build/win32/CMakeLists.txt`
  **Function:** Build/backend selection
  **Purpose:** Includes jsoncons backend and headers when selected.
- **File:** `test/common/json.h`
  **Function:** Test JSON type alias
  **Purpose:** Test helpers use jsoncons (`jsoncons::ojson`).

## 2. Is the implementation correct?
- **Location:** `json_string_cursor` event loop (`current()/next()/done()`)
  - **Rating:** KORREKT
  - **Short reason:** This matches documented jsoncons pull-parsing usage.
  - **Evidence:** `src/request_body_processor/json_backend_jsoncons.cc` (`parseDocumentWithJsoncons`); `others/jsoncons/README.md`.

- **Location:** Options (`max_nesting_depth`, `lossless_number`, `lossless_bignum`)
  - **Rating:** KORREKT
  - **Short reason:** Setters and option semantics are documented and used consistently.
  - **Evidence:** `src/request_body_processor/json_backend_jsoncons.cc`; `others/jsoncons/doc/ref/corelib/basic_json_options.md`.

- **Location:** Position-based numeric token reconstruction (`begin_position/end_position` + fallback scan)
  - **Rating:** KORREKT
  - **Short reason:** Uses documented context position APIs with guarded fallback paths.
  - **Evidence:** `src/request_body_processor/json_backend_jsoncons.cc` (`rawNumberFromContext`, `RawJsonTokenCursor`); `others/jsoncons/doc/ref/corelib/ser_context.md`.

- **Location:** Concrete functional errors
  - **Rating:** NICHT BEWIESEN
  - **Short reason:** No clear, reproducible functional API misuse is visible in the inspected code.
  - **Evidence:** Inspected full backend flow in `json_backend_jsoncons.cc`.

## 3. Performance
- **Location:** `src/request_body_processor/json_backend_jsoncons.cc:688-695` and `242-539`
- **Problem:** For numeric events, code performs an additional raw token scan over input alongside cursor processing.
- **Evidence:** `consumeNextNumberToken` / `skipToNextNumberToken` run in addition to event loop.
- **Rating:** PROBLEM

## 4. Memory / memory management
- **Location:** Whole jsoncons backend path
- **Problem:** No concrete memory-management fault is proven in visible code.
- **Evidence:** No persistent global parser state; parse-local object lifetimes only.
- **Rating:** KORREKT

## 5. Simple jsoncons conclusion
Overall the jsoncons implementation is correct. The most important issue is the extra numeric token synchronization scan. The most important optimization is reducing this extra scan when context positions are sufficient. In visible code, memory management is clean.

# End comparison

- **Proven difference:** simdjson path has explicit per-parse padded copy; jsoncons path has explicit extra numeric token synchronization scan.
- **NOT PROVEN:** That one backend is always faster than the other across all real workloads.
