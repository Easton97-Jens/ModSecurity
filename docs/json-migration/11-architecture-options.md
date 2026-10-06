# Architekturvarianten für austauschbare JSON-Backends

## Option A: Direkter YAJL-zu-X Austausch (nicht empfohlen)
- Niedriger initialer Designaufwand.
- Hoher Lock-in auf neue Library.
- Erneuter Migrationsschmerz bei nächstem Wechsel.

## Option B: Interne Backend-Schnittstelle (empfohlen)

## B.1 Interface-Skizze
```cpp
struct JsonError { std::string message; size_t offset{0}; int line{-1}; int column{-1}; };

class IJsonStreamEvents {
 public:
  virtual bool on_null() = 0;
  virtual bool on_bool(bool) = 0;
  virtual bool on_number(std::string_view raw) = 0;
  virtual bool on_string(std::string_view) = 0;
  virtual bool on_key(std::string_view) = 0;
  virtual bool on_start_object() = 0;
  virtual bool on_end_object() = 0;
  virtual bool on_start_array() = 0;
  virtual bool on_end_array() = 0;
};

class IJsonStreamParser {
 public:
  virtual bool feed(const char* data, size_t len, JsonError*) = 0;
  virtual bool finish(JsonError*) = 0;
};

class IJsonWriter {
 public:
  virtual bool begin_object() = 0; /* ... */
  virtual std::string take() = 0;
};
```

## B.2 Capability-Maske
```text
HAS_STREAMING | HAS_INCREMENTAL_PARSE | HAS_DOM | HAS_WRITER |
HAS_TYPED_BINDING | HAS_ZERO_COPY_DOM
```

## B.3 Schichten im Repo
1. `src/json_backend/*` (neue Abstraktion)
2. `src/request_body_processor/json.cc` nutzt nur `IJsonStreamParser`
3. `src/transaction.cc` + `src/modsecurity.cc` nutzen nur `IJsonWriter`
4. Tests nutzen `IJsonDom` oder test-local parser facade

## Option C: Getrennte Backends pro Use Case
- Parser-Backend und Writer-Backend getrennt auswählbar.
- Praktisch, weil nicht jede Library alle Rollen gleich gut erfüllt.
- Erhöht Konfigurationsmatrix, aber reduziert Zwangskompatibilität.

## Empfehlung
- **Option B + C kombiniert**:
  - einheitliche interne API,
  - aber getrennte Backendwahl für parser/writer/dom.
