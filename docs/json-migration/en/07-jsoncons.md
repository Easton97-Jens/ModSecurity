# JSONCONS

## 1. Summary
- Modern feature-rich C++ library.
- Technically adaptable, but less natural for C-like YAJL replacement points.

## 2. Project status
- Repo: <https://github.com/danielaparker/jsoncons>
- Docs: <https://danielaparker.github.io/jsoncons/>
- Active maintenance and releases.

## 3. Technical model
- C++-oriented, largely header-only.
- Supports DOM/events/cursor patterns.

## 4. Build/integration
- Vendoring is easy.
- System package presence is less uniform than json-c/jsoncpp.

## 5. Fit to repository
- Strong for C++ modernization tracks.
- Requires adapter for existing YAJL-shaped paths.

## 6. Module vs package
- Often better as bundled optional dependency; system mode where available.

## 7. Integration strategy
- Optional C++ backend for DOM/writer/cursor use cases.

## 8. Migration effort
- **Medium to high**.

## 9. Recommendation
- **Optional only (C++-focused)**.
