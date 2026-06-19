# Recommendation Matrix

| Library | Lang | Streaming | Incremental | DOM | Writer | Maintenance | Fit as YAJL replacement | Recommendation |
|---|---|---:|---:|---:|---:|---|---|---|
| RapidJSON | C++ | Yes | Partial | Yes | Yes | Active commits, old releases | Good technically, governance risk | Optional |
| nlohmann/json | C++ | Limited | Limited | Yes | Yes | Very active | Good for DOM/tests | C++ optional |
| json-c | C | Medium | Yes | Yes | Yes | Active | Strong C candidate | Strong |
| jansson | C | Limited | Limited | Yes | Yes | Active | Good with abstraction | Strong |
| cJSON | C | No | Low | Yes | Yes | Active | Partial only | Experimental |
| jsoncpp | C++ | Low | Low | Yes | Yes | Active | Good for C++ DOM/writer | Optional |
| jsoncons | C++ | Good | Possible | Yes | Yes | Active | Feasible for C++ tracks | Optional |
| simdjson | C++ | Different model | Limited | Yes | No | Very active | Not 1:1 | Specialized |
| yyjson | C | Good | Partial | Yes | Yes | Active | Strong modern C option | Strong |
| glaze | C++20 | No | n/a | Typed | Typed | Very active | Conceptual mismatch | Not recommended |

Priority shortlist:
1. json-c
2. jansson
3. yyjson
4. nlohmann/json or jsoncpp (C++/tests)
