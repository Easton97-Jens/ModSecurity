# JSON-C (json-c)

## 1. Summary
- Strong C-based candidate.
- Realistic replacement path for this repository.

## 2. Project status
- Repo: <https://github.com/json-c/json-c>
- Docs: <https://json-c.github.io/json-c/>
- Active maintenance (latest tag line includes `json-c-0.18-20240915`).

## 3. Technical model
- C library.
- DOM via `json_object` and incremental tokener parsing (`json_tokener_parse_ex`).
- Writer via `json_object_to_json_string_ext`.

## 4. Build/integration
- Strong distro availability.
- Works as system package; can also be bundled.

## 5. Fit to repository
- Good path for request-body incremental parsing (with adapter semantics).
- Good replacement candidate for writer and test DOM.

## 6. Module vs package
- Prefer system package; keep optional bundled fallback.

## 7. Integration strategy
- Add `jsonc_stream_parser`, `jsonc_writer`, `jsonc_dom` backends.

## 8. Migration effort
- **Medium**.

## 9. Recommendation
- **Strong candidate (recommended PoC #1)**.
