# RAPIDJSON

## 1. Summary
- Technically capable (SAX + DOM + writer available).
- Realistic YAJL replacement: yes, from an API-mapping perspective.
- Fit for this repository: medium (C++-centric, but project core is C++17).
- Main concern: maintenance/release signal.

## 2. Project status and maintainability
- Official repo: <https://github.com/Tencent/rapidjson>
- Official docs: <https://rapidjson.org/>
- Observation (GitHub API, 2026-04-07): latest release is **v1.1.0 (2016-08-25)** despite repository activity.
- Risk: old release line complicates security and packaging confidence.

## 3. Technical model
- Language: C++ (header-only).
- APIs: DOM, SAX (`Reader`/`Handler`), writer (`Writer`, `PrettyWriter`).
- Incremental parsing: feasible via SAX feeding, but requires adapter design.
- Error handling: ParseResult + error offset.

## 4. Build and integration effort
- System package availability varies by distro.
- Vendoring is easy (header-only), but governance/pinning is required.

## 5. Fit to this repository
- Replace points:
  - `src/request_body_processor/json.cc`: YAJL callbacks → RapidJSON SAX handler.
  - `src/transaction.cc`, `src/modsecurity.cc`, `headers/modsecurity/transaction.h`: `yajl_gen_*` → RapidJSON writer.
  - Tests: YAJL tree API → RapidJSON DOM.

## 6. Module vs system package
- Recommend both modes, but default to system where maintained packages exist.
- If vendored, enforce explicit update/security tracking policy.

## 7. Integration strategy
- Implement `RapidJsonStreamAdapter` and `RapidJsonWriterAdapter` behind an internal backend API.
- Build flag example: `--with-json-parser=rapidjson`.

## 8. Migration effort
- **High**.

## 9. Recommendation
- **Optional/experimental only**, unless maintenance concerns are organizationally mitigated.
