# Recommendation Matrix

| Library | Sprache | SAX/Streaming | Incremental | DOM | Writer | Maintenance (2026-04) | YAJL-Ersatz | Empfehlung |
|---|---|---:|---:|---:|---:|---|---|---|
| RapidJSON | C++ | Ja | Teilweise | Ja | Ja | Commits aktiv, Releases alt (2016) | technisch gut, governance risk | nur optional |
| nlohmann/json | C++ | Begrenzt/Adapter | begrenzt | Ja | Ja | sehr aktiv | gut für DOM/Tests | C++ optional |
| json-c | C | mittel | Ja (tokener) | Ja | Ja | aktiv | sehr guter C-Kandidat | starker Kandidat |
| jansson | C | eher nein | begrenzt | Ja | Ja | aktiv | gut mit Architekturhilfe | starker Kandidat |
| cJSON | C | nein | nein/gering | Ja | Ja | aktiv | nur teilweiser Ersatz | experimentell |
| jsoncpp | C++ | gering | gering | Ja | Ja | aktiv | gut für C++ DOM/Writer | optional |
| jsoncons | C++ | ja (events/cursor) | möglich | Ja | Ja | aktiv | machbar, C++-orientiert | optional |
| simdjson | C++ | on-demand statt SAX | begrenzt | Ja | nein | sehr aktiv | kein 1:1 Ersatz | spezialfall |
| yyjson | C | reader/writer | teilweise | Ja | Ja | aktiv | sehr guter moderner C-Kandidat | starker Kandidat |
| glaze | C++20 | nein (klassisch) | n/a | typed binding | typed ser | sehr aktiv | konzeptionell fern | nicht empfohlen |

## Priorisierte Kandidaten
1. **json-c**
2. **jansson**
3. **yyjson**
4. (separat für Tests/C++): **nlohmann/json** oder **jsoncpp**
