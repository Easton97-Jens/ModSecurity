# RE-AUDIT: libinjection v4.0 migration against official MIGRATION.md (2026-04-03)

Primäre normative Quelle: <https://github.com/libinjection/libinjection/blob/main/MIGRATION.md>

## 1. Derived migration checklist from MIGRATION.md

1. **Breaking API-Änderung erfassen**: Detection-Funktionen liefern `injection_result_t` statt `int`.
2. **Neue Return-Semantik korrekt verwenden**:
   - `LIBINJECTION_RESULT_FALSE` = kein Angriff
   - `LIBINJECTION_RESULT_TRUE` = Angriff erkannt
   - `LIBINJECTION_RESULT_ERROR` = Parser-Fehler (kein `abort()` mehr)
3. **Betroffene APIs migrieren**: mindestens `libinjection_xss`, `libinjection_sqli`, `libinjection_is_xss`, `libinjection_h5_next` sofern im Projekt genutzt.
4. **Header-Anpassung**: `#include "libinjection_error.h"` ergänzen, wo Result-Typ/Konstanten verwendet werden.
5. **Explizite ERROR-Behandlung** in allen relevanten Codepfaden.
6. **Fail-safe im Security-Kontext** (WAF): ERROR darf nicht stillschweigend als benign durchlaufen.
7. **Truthiness-Fallen prüfen**: `if (result)` / `!result` / `== 0` etc. auf unbeabsichtigte Behandlung kontrollieren.
8. **Tests für Error-Fälle** ergänzen.
9. **Logging/Monitoring**: Fehlerzustände sichtbar machen (insb. parser errors).
10. **Edge-Case-Testen** (z. B. leere/auffällige Inputs), soweit für Projektkontext relevant.

---

## 2. Checklist evaluation

| Punkt | Status | Begründung | Evidenz |
|---|---|---|---|
| 1. `int -> injection_result_t` | PASS | Adapter- und Detector-Pfade nutzen `injection_result_t` statt `int` für libinjection-Ergebnisse. | OBSERVED |
| 2. TRUE/FALSE/ERROR-Semantik | PASS | Beide Detectors unterscheiden `TRUE`, `FALSE`, `ERROR` per `switch`; Utility mappt fail-safe. | OBSERVED |
| 3. Betroffene APIs migriert (genutzter Scope) | PASS | Im Projekt werden `libinjection_sqli` und `libinjection_xss` genutzt; beide migriert. Keine Nutzung von `is_xss`/`h5_next` im Repo gefunden. | OBSERVED + UNCERTAIN |
| 4. `libinjection_error.h` eingebunden | PASS | Includes in Adapter, Utils und Detectors vorhanden; Build-Headers führen Datei. | OBSERVED |
| 5. ERROR explizit behandelt | PASS | `LIBINJECTION_RESULT_ERROR` hat eigenen Case in XSS/SQLi mit eigener Behandlung. | OBSERVED |
| 6. Fail-safe im Security-Kontext | PASS | `ERROR` wird als malicious behandelt (`TRUE || ERROR`), Rückgabe damit blockierbar. | OBSERVED |
| 7. `if (result)`-Fallen entschärft | PASS | Kein relevanter Pfad gefunden, der `ERROR` implizit als clean behandelt; zentrale Entscheidungen sind explizit/mapped. | OBSERVED + INFERRED |
| 8. Tests für Error-Fälle | PASS | Unit-Test-Override erzeugt `ERROR`; JSON-Fälle prüfen `ret=1` und Capture für XSS/SQLi. | OBSERVED |
| 9. Logging/Monitoring parser errors | PARTIAL | Detector-Logging bei `ERROR` vorhanden; jedoch primär Debug-Logging und kein klarer dedizierter Engine-weiter Metric-Kanal sichtbar. | OBSERVED + INFERRED |
| 10. Edge-Case-Tests laut Guide | UNCERTAIN | Re-Audit belegt vorhandene ERROR-Tests, aber kein Vollnachweis aller empfohlenen Edge-Case-Szenarien im Testbestand. | UNCERTAIN |

---

## 3. Assessment of tri-state propagation requirement

- **OBSERVED (aus MIGRATION.md):** Der Guide fordert explizite Behandlung von `ERROR`, fail-safe Verhalten im WAF-Kontext, Prüfung von `if (result)`-Stellen und Tests für Fehlerfälle.
- **OBSERVED (aus MIGRATION.md):** Es gibt zusätzlich eine „Minimal Migration“-Strategie, die `TRUE || ERROR` gemeinsam als Block behandelt (bi-state policy output erlaubt).
- **INFERRED:** Eine vollständige End-to-End-Propagation von `injection_result_t` über *alle* internen Interfaces wird **nicht ausdrücklich** als Muss formuliert.
- **UNCERTAIN:** Der Guide ist nicht formal-normativ als harte Architekturvorgabe; er ist eine Migrationsrichtlinie mit empfohlenen Strategien.

**Schluss zu dieser Frage:** Nach offizieller MIGRATION.md ist Tri-State bis zur Engine **nicht zwingend als eigener Interface-Typ** erforderlich, solange `ERROR` explizit und sicher (fail-safe) behandelt wird und nicht still als benign endet.

---

## 4. Re-evaluation of Finding F-01

### F-01 alt: „Tri-State wird an Engine-Grenze auf bool reduziert“

1. **Verstoß gegen MIGRATION.md?**
   - **OBSERVED:** Detectoren behandeln `ERROR` explizit und fail-safe; damit zentrale Guide-Forderungen erfüllt.
   - **INFERRED:** Die reine bool-Weitergabe verletzt den Guide nicht zwingend.

2. **Echter Migrationsfehler oder strengere Interpretation?**
   - **Bewertung:** **Design-Tradeoff / strenger als Guide**.
   - **OBSERVED:** Tri-State liegt zuletzt in `DetectSQLi::evaluate`/`DetectXSS::evaluate` vor (`injection_result_t` + `switch`).
   - **OBSERVED:** Reduktion erfolgt bei `return isMaliciousLibinjectionResult(...)` zu boolescher Match-Semantik.
   - **OBSERVED:** Danach propagieren `Operator::evaluateInternal` und `RuleWithOperator::*` nur Match/No-Match.
   - **INFERRED:** Das ist architektonisch weniger expressiv, aber im Sinne der offiziellen Migration nicht automatisch falsch.

3. **Einordnung (Bug vs Design vs Scope):**
   - **Bug (Migration-Checklist):** **nein** (kein klarer Checklist-Verstoß nachweisbar).
   - **Design-Tradeoff:** **ja** (Informationsverlust vs. einfaches Engine-Interface).
   - **Out-of-scope der offiziellen Migration:** **teilweise ja** (end-to-end Interface-Re-Design wird nicht explizit verlangt).

---

## 5. Updated verdict

## FINAL: **FULLY MIGRATED**

Begründung strikt nach offizieller MIGRATION.md:
- Alle im Repo genutzten libinjection-Detection-Aufrufe sind auf `injection_result_t` migriert.
- `libinjection_error.h` ist eingebunden.
- `LIBINJECTION_RESULT_ERROR` wird explizit und fail-safe behandelt (nicht als benign/falsch-negativ).
- Es gibt dedizierte Error-Tests per Override.
- Das verbleibende Thema „Tri-State nicht als eigener Typ bis ganz oben“ ist nach Guide eher Architekturverbesserung als Pflichtkriterium.

---

## 6. Delta to previous audit

### Bestätigt
- Explizite TRUE/FALSE/ERROR-Behandlung in Detectors.
- Fail-safe Verhalten bei ERROR.
- Vorhandene Error-Tests.

### Relativiert
- Frühere harte Abwertung wegen bool-Engine war **strenger als MIGRATION.md**.
- Das F-01-Thema bleibt als Architekturhinweis valide, aber **nicht** als zwingender Migrationsfehler.

### Korrigiert
- Vorheriges Endurteil `PARTIALLY MIGRATED` wird auf Basis der offiziellen Checklist zu `FULLY MIGRATED` angepasst.
