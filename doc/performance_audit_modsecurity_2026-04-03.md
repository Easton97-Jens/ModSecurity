# OWASP ModSecurity v3 – Technischer Performance-Audit (Code-fokussiert)

Dieses Dokument fasst eine tiefgehende, codebasierte Performance-Analyse von libmodsecurity zusammen, mit Fokus auf Hot Paths, Regel-Engine, Speicherverhalten und Skalierung.

## Scope

Analysierte Kernpfade:
- `Transaction` Request/Response Lifecycle
- `RulesSet` + `RuleWithOperator` Evaluierung
- Request-Body-Prozessoren (URLENCODED/JSON/XML/MULTIPART)
- Regex- und Pattern-Matching Operatoren (`@rx`, `@pm`)
- Collection-Backends und Locking
- Audit-Logging und Serialisierung

## Wichtigste Erkenntnisse (Kurzfassung)

1. **Dominanter CPU-Pfad**: `RulesSet::evaluate()` → `RuleWithOperator::evaluate()` → Transformationen → Operator (`@rx`, `@pm`, …).
2. **Regex ist Hauptkostentreiber** bei CRS-lastigen Regelsets; Match-Limits sind vorhanden, aber nur Schadensbegrenzung.
3. **Signifikante String-/Copy-Kosten** in Request-Body- und Logging-Pfaden (`stringstream::str()`, Header-Konkatenation, JSON/Audit-Serialisierung).
4. **Multipart-Parsing** arbeitet byteweise mit hohem Branching- und State-Machine-Aufwand.
5. **Concurrency**: transaktionslokal weitgehend lock-frei; globale Collections nutzen `shared_mutex` und können bei write-lastigen Workloads kontendieren.

## Potenzielle Optimierungen (Top)

- Request-Body intern von `stringstream` auf chunk-/span-basierte Buffering-Strategie umstellen.
- `FULL_REQUEST` lazy oder feature-gated berechnen (TODO im Code vorhanden).
- Regex-basierte Regeln per Vorfilter (Literal prefilter, cheap guards) reduzieren.
- Transformation-Pipeline für häufige Kombinationen cachen/fusen.
- Audit-Logging asynchron und selektiv (Parts minimieren, JSON nur wenn nötig).

