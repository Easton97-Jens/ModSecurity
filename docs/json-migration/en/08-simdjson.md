# SIMDJSON

## 1. Summary
- Excellent performance library, but not a direct YAJL model replacement.

## 2. Project status
- Repo: <https://github.com/simdjson/simdjson>
- Docs: <https://simdjson.org/>
- Very active; release v4.6.1 (2026-04-03).

## 3. Technical model
- C++ library focused on ondemand/DOM parsing speed.
- Writer/generator is not its primary role.

## 4. Build/integration
- Can be packaged or vendored; packaging versions may lag.

## 5. Fit to repository
- Good for high-throughput parsing scenarios.
- Weak direct fit for current writer-heavy and callback-centric YAJL usage.

## 6. Module vs package
- Both are possible; bundled often used for performance pinning.

## 7. Integration strategy
- Use as optional performance backend with dedicated capability flags.

## 8. Migration effort
- **High** for full replacement.

## 9. Recommendation
- **Specialized/experimental**, not primary replacement path.
