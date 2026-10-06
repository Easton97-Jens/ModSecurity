# YYJSON

## 1. Summary
- Strong modern C candidate.
- Good potential for YAJL replacement with backend abstraction.

## 2. Project status
- Repo: <https://github.com/ibireme/yyjson>
- Docs: <https://ibireme.github.io/yyjson/>
- Active project; release 0.12.0 (2025-08-18).

## 3. Technical model
- C API with immutable/mutable DOM and reader/writer APIs.
- Performance-oriented design with allocator controls.

## 4. Build/integration
- Easy to bundle.
- System package availability varies by distribution.

## 5. Fit to repository
- Good for writer and test DOM migration.
- Incremental callback parity should be validated in PoC.

## 6. Module vs package
- Recommend dual mode (system + bundled fallback).

## 7. Integration strategy
- Evaluate early after json-c/jansson PoCs.

## 8. Migration effort
- **Medium**.

## 9. Recommendation
- **Strong candidate**.
