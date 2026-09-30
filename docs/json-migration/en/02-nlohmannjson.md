# NLOHMANN/JSON

## 1. Summary
- Highly maintained and widely adopted.
- Strong for DOM/test use cases.
- Less natural as a direct replacement for YAJL-style incremental SAX parsing.

## 2. Project status
- Repo: <https://github.com/nlohmann/json>
- Docs: <https://json.nlohmann.me/>
- Active project; release v3.12.0 (2025-04-11).

## 3. Technical model
- C++ header-only.
- DOM-first model; includes SAX interface and many utility features.
- Incremental streaming behavior differs from YAJL callback lifecycle.

## 4. Build/integration
- Excellent system package availability and easy vendoring.

## 5. Fit to repository
- Very good for replacing test-side `yajl_tree_parse` usage.
- Usable for writer abstraction.
- Request-body incremental parser needs adapter or redesign.

## 6. Module vs package
- Prefer system packages by default; allow vendored fallback.

## 7. Integration strategy
- Introduce first in test DOM layer, then optionally in writer.

## 8. Migration effort
- **Medium to high**.

## 9. Recommendation
- **Recommended for C++ DOM/test paths**, not first pick for request-body SAX backend.
