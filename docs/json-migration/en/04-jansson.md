# JANSSON

## 1. Summary
- Solid and mature C library.
- Good for DOM/writer; less direct for YAJL-style incremental SAX workflows.

## 2. Project status
- Repo: <https://github.com/akheron/jansson>
- Docs: <https://jansson.readthedocs.io/>
- Active; release v2.15.0 (2026-01-24).

## 3. Technical model
- C DOM model (`json_t*`) with load/dump APIs.
- Rich error object (`json_error_t`).

## 4. Build/integration
- Good system packaging.
- Bundled mode possible.

## 5. Fit to repository
- Straightforward for tests and writer.
- Incremental request-body semantics require adapter or staged parsing design.

## 6. Module vs package
- Prefer system package; bundled optional.

## 7. Integration strategy
- Use for DOM/writer first; consider separate streaming backend for parser role.

## 8. Migration effort
- **Medium to high**.

## 9. Recommendation
- **Strong candidate** in a multi-backend architecture.
