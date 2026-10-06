# Migrationsplan (phasenweise)

## Phase 0 – Baseline erfassen
- Bestehende YAJL-abhängige Tests identifizieren und „golden output“ sichern.
- JSON-Ausgabe von `Transaction::toJSON()` und `processContentOffset()` snapshotten.

## Phase 1 – Abstraktionsschicht einziehen (ohne Verhaltensänderung)
- Neue Interfaces in `src/json_backend/`.
- YAJL-Adapter implementieren (`yajl_stream_parser`, `yajl_writer`, optional `yajl_dom`).
- Bestehender Code nutzt nur noch Abstraktion.

Ergebnis: Keine Featureänderung, aber Austauschbarkeit geschaffen.

## Phase 2 – Test-DOM entkoppeln
- `test/common/modsecurity_test.cc`, `test/unit`, `test/regression` auf `IJsonDom` oder parser-facade umstellen.
- YAJL-Typen aus Testcode entfernen.

## Phase 3 – 1. alternatives C-Backend (json-c)
- json-c Adapter implementieren.
- CI-Job `--with-json-parser=json-c --with-json-writer=json-c` hinzufügen.
- Regressionen gegen YAJL-baseline vergleichen.

## Phase 4 – 2./3. Backend (jansson, yyjson)
- Gleiches Verfahren; Fokus auf Fehlertexte/Number-/Unicode-Semantik.

## Phase 5 – C++ optionale Backends
- nlohmann/json oder jsoncpp für testdomäne/Tools.
- simdjson/glaze nur als experimentelle targets.

## Technische Risiken / Prüfungen pro Phase
- **Semantikdrift bei Number/String** (`1`, `1.0`, exponent notation).
- **UTF-8 Validierung** konsistent halten.
- **Error-Position/Text** kompatibel genug für Tests/Logs.
- **Writer Key-Order** (falls Tests implizit darauf basieren).

## First PoC Empfehlung
1. Abstraktionsschicht + YAJL-Backend (no-op migration)
2. json-c als zweites Backend
3. Regression suite + audit-log JSON diff
