# JSONCPP

## 1. Summary
- Stable C++ JSON library.
- Good for DOM/writer usage; weaker direct match for YAJL incremental callbacks.

## 2. Project status
- Repo: <https://github.com/open-source-parsers/jsoncpp>
- Active releases (e.g., 1.9.7 in 2026-03).

## 3. Technical model
- C++ DOM (`Json::Value`) + parser/writer builders.

## 4. Build/integration
- Good system package coverage.
- Bundled and CMake integration are straightforward.

## 5. Fit to repository
- Good candidate for test parsing and writer abstraction.
- Request-body parser still needs dedicated adapter/redesign.

## 6. Module vs package
- Prefer system packages; bundled optional for reproducibility.

## 7. Integration strategy
- Adopt for DOM/writer backend path; separate streaming backend if needed.

## 8. Migration effort
- **Medium**.

## 9. Recommendation
- **Optional C++ backend with abstraction layer**.
