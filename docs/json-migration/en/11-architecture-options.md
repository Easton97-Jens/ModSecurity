# Architecture Options for Replaceable JSON Backends

## Option A: Direct YAJL-to-X replacement (not recommended)
- Low initial design effort.
- High lock-in and repeated migration pain later.

## Option B: Internal backend abstraction (recommended)

### Interface sketch
```cpp
struct JsonError { std::string message; size_t offset{0}; int line{-1}; int column{-1}; };
class IJsonStreamEvents { /* null, bool, number, string, key, obj/arr open/close */ };
class IJsonStreamParser { /* feed(), finish() */ };
class IJsonWriter { /* begin/end object/array, key/value, take() */ };
```

### Capability flags
- `HAS_STREAMING`
- `HAS_INCREMENTAL_PARSE`
- `HAS_DOM`
- `HAS_WRITER`
- `HAS_TYPED_BINDING`
- `HAS_ZERO_COPY_DOM`

## Option C: Split backends by use case
- Separate parser backend from writer backend from DOM backend.
- Better practical fit because libraries differ strongly by strengths.

## Recommendation
Adopt **Option B + C**:
1. Unified internal interfaces.
2. Independent backend selection per capability area.
