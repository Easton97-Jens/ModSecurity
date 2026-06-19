# Build System Integration

## 1) Autotools

Proposed flags:
```m4
--with-json-parser=auto|yajl|json-c|jansson|yyjson|rapidjson|...
--with-json-writer=auto|yajl|json-c|yyjson|rapidjson|jsoncpp|...
--with-json-dom=auto|yajl|json-c|jansson|nlohmannjson|jsoncpp|...
--with-json-backend-mode=auto|system|bundled|none
```

Implementation notes:
- Add per-library detection macros/variables (`*_FOUND`, `*_CFLAGS`, `*_LDADD`).
- Generate explicit `config.h` backend defines.
- Gate backend sources via `AM_CONDITIONAL` in `Makefile.am`.

## 2) Windows CMake

Replace hardcoded YAJL mandatory assumption with cache variables:
```cmake
JSON_PARSER_BACKEND
JSON_WRITER_BACKEND
JSON_DOM_BACKEND
JSON_BACKEND_MODE
```

## 3) pkg-config export
- Stop exporting YAJL-specific libs unconditionally.
- Export only active backend private link flags.

## 4) CI matrix
Minimum:
- YAJL legacy baseline
- json-c backend
- jansson backend
- optional yyjson experimental job
