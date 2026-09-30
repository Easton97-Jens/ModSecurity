# Build-System-Integration

## 1) Autotools (configure.ac / m4)

## Vorschlag für Optionen
```m4
AC_ARG_WITH([json-parser], [AS_HELP_STRING([--with-json-parser=BACKEND], [auto|yajl|json-c|jansson|yyjson|rapidjson|...])])
AC_ARG_WITH([json-writer], [AS_HELP_STRING([--with-json-writer=BACKEND], [auto|yajl|json-c|yyjson|rapidjson|jsoncpp|...])])
AC_ARG_WITH([json-dom],    [AS_HELP_STRING([--with-json-dom=BACKEND],    [auto|yajl|json-c|jansson|nlohmannjson|jsoncpp|...])])
AC_ARG_WITH([json-backend-mode], [AS_HELP_STRING([--with-json-backend-mode=MODE], [auto|system|bundled|none])])
```

## Umsetzungshinweise
- Neue `build/json-<lib>.m4` oder Erweiterung `msc_find_lib.m4` pro Library.
- Pro Backend eigene `*_FOUND`, `*_CFLAGS`, `*_LDADD` Variablen.
- In `config.h` eindeutige Makros: `WITH_JSON_BACKEND_JSONC`, etc.
- `Makefile.am`: backend-spezifische Quellen per `if`/`AM_CONDITIONAL`.

## 2) Windows CMake

Aktuell ist YAJL hart als mandatory codiert (`HAVE_YAJL 1`). Das muss zu echter Option werden.

Vorschlag:
```cmake
set(JSON_PARSER_BACKEND "auto" CACHE STRING "auto;yajl;json-c;jansson;yyjson;rapidjson")
set(JSON_WRITER_BACKEND "auto" CACHE STRING "auto;yajl;json-c;yyjson;rapidjson;jsoncpp")
set(JSON_DOM_BACKEND    "auto" CACHE STRING "auto;yajl;json-c;jansson;nlohmannjson;jsoncpp")
set(JSON_BACKEND_MODE   "system" CACHE STRING "auto;system;bundled;none")
```

## 3) pkg-config / modsecurity.pc.in
- `Libs.private` darf nicht starr YAJL referenzieren.
- Nur tatsächlich aktive JSON-Libs exportieren.

## 4) CI-Matrix
Mindestens:
- `json-parser=yajl` (legacy)
- `json-parser=json-c`
- `json-parser=jansson`
- optional `yyjson` experimentell

Und kombinierte Writer/DOM-Matrix in nightly builds.
