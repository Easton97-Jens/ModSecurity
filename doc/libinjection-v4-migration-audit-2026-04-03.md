# libinjection v4.0 migration audit (beweisnahe Fassung, 2026-04-03)

## 1) Executive summary

- **OBSERVED:** Die direkten libinjection-Aufrufer für SQLi/XSS nutzen `injection_result_t` und behandeln `LIBINJECTION_RESULT_TRUE`, `LIBINJECTION_RESULT_FALSE`, `LIBINJECTION_RESULT_ERROR` explizit per `switch`. `ERROR` wird lokal fail-safe als Match behandelt.  
- **OBSERVED:** Die Operator-Engine propagiert Ergebnisse nur als `bool` (Match/No-Match), nicht als Tri-State.  
- **INFERRED:** Dadurch ist die Migration sicherheitslogisch (Enforcement) robust, aber nicht vollständig tri-state-konsistent entlang der gesamten Call-Chain/Architektur.

**Endurteil:** `PARTIALLY MIGRATED`

---

## 2) Migration scope discovered

### 2.1 Direkte libinjection-Nutzung

- **OBSERVED:** Adapter-Signaturen und direkte Calls:
  - `runLibinjectionSQLi(...) -> libinjection_sqli(...)`
  - `runLibinjectionXSS(...) -> libinjection_xss(...)`
- **OBSERVED:** Signaturen verwenden `injection_result_t` (keine int/bool-Rückgabe im Adapter).

### 2.2 Result-/Error-Typen und Utilities

- **OBSERVED:** `libinjection_error.h` wird eingebunden in Adapter/Operator-Utilities/Detectors.
- **OBSERVED:** `isMaliciousLibinjectionResult(...)` mappt `TRUE || ERROR` auf „malicious“ (fail-safe).

### 2.3 Security-Entry-Points im Produkt

- **OBSERVED:** `DetectSQLi::evaluate(...)` und `DetectXSS::evaluate(...)` sind die produktiven Operator-Entry-Points.
- **OBSERVED:** Parser/Factory binden `@detectSQLi` und `@detectXSS` auf diese Operatoren.

### 2.4 Testrelevante Wrapper/Overrides

- **OBSERVED:** Test-Hooks erlauben erzwungenes `LIBINJECTION_RESULT_ERROR` via thread-local Override.
- **OBSERVED:** Unit-Test-Fälle prüfen `ret=1` + Capture für erzwungene ERROR-Pfade.

### 2.5 Nicht gefundene APIs

- **OBSERVED:** Keine Verwendungen von `libinjection_is_xss` oder `libinjection_h5_next` im Repository-Codepfad (`src/`, `test/`).
- **UNCERTAIN:** Ob diese APIs in externen Modulen/Build-Varianten außerhalb dieses Repos genutzt werden, ist nicht beweisbar.

---

## 3) Call-chain analysis

## 3.1 SQLi-Kette (konkret)

1. **OBSERVED (Tri-State vorhanden):** `DetectSQLi::evaluate` hält libinjection-Ergebnis als `const injection_result_t sqli_result`.
2. **OBSERVED (Tri-State verarbeitet):** `switch (sqli_result)` hat Cases für `TRUE`, `ERROR`, `FALSE`.
3. **OBSERVED (Tri-State -> bool):** Rückgabe erfolgt über `return isMaliciousLibinjectionResult(sqli_result);`.
4. **OBSERVED (nur bool propagiert):** `Operator::evaluateInternal(...)` arbeitet mit `bool res` und gibt bool zurück.
5. **OBSERVED (Enforcement bool-basiert):** `RuleWithOperator::executeOperatorAt` und `RuleWithOperator::evaluate` nutzen nur `bool ret` zur Match/No-Match-Entscheidung.

## 3.2 XSS-Kette (konkret)

1. **OBSERVED (Tri-State vorhanden):** `DetectXSS::evaluate` hält `const injection_result_t xss_result`.
2. **OBSERVED (Tri-State verarbeitet):** `switch (xss_result)` mit Cases `TRUE`, `ERROR`, `FALSE`.
3. **OBSERVED (Tri-State -> bool):** Rückgabe über `isMaliciousLibinjectionResult(xss_result)`.
4. **OBSERVED (nur bool propagiert):** Weiter oben identische bool-Engine.

## 3.3 Zentrale These „Tri-State wird auf bool reduziert“ (beweisnah)

- **Letzte Stelle mit echtem Tri-State:**
  - `DetectSQLi::evaluate`: lokale Variable `sqli_result` und `switch`.
  - `DetectXSS::evaluate`: lokale Variable `xss_result` und `switch`.
- **Stelle der Reduktion auf bool:**
  - `return isMaliciousLibinjectionResult(...);` in beiden evaluate-Methoden.
- **Stelle, an der nur Match/No-Match propagiert wird:**
  - `Operator::evaluateInternal` (`bool res`), `RuleWithOperator::executeOperatorAt` (`bool ret`), `RuleWithOperator::evaluate` (`if (ret == true) ...`).
- **Konkrete Folgen:**
  - **OBSERVED:** Enforcement bleibt fail-safe, weil `ERROR` als `true` (match) in die bool-Engine läuft.
  - **INFERRED:** Logging/Reasoning oberhalb der Detector-Ebene kann den Zustand „parser error vs. attack detected“ nicht als getrennten Rückgabekanal nutzen; Differenzierung passiert nur lokal über Debug-Logs in den Detectoren.

---

## 4) Correctly migrated locations

### C-01: Adapter-Interface auf v4-Resulttypen

- **OBSERVED:** `DetectSQLiFn`, `DetectXSSFn`, `runLibinjectionSQLi`, `runLibinjectionXSS` verwenden `injection_result_t`.
- **Mechanismus:** Interface-Signatur migriert, kein implizites int/bool am Adapter-Rand.

### C-02: Expliziter Tri-State-Switch in SQLi-Operator

- **OBSERVED:** `switch (sqli_result)` mit separaten Cases `TRUE`, `ERROR`, `FALSE`.
- **Mechanismus:** explizite Zustandsbehandlung statt truthy/falsy.
- **OBSERVED:** `ERROR`-Case loggt parser error und behandelt den Fall als Match (fail-safe) via finalem Mapping.
- **OBSERVED:** Capture-Verhalten unterscheidet TRUE (Fingerprint) vs ERROR (Input), d.h. semantisch bewusst.

### C-03: Expliziter Tri-State-Switch in XSS-Operator

- **OBSERVED:** `switch (xss_result)` mit Cases `TRUE`, `ERROR`, `FALSE`.
- **Mechanismus:** explizite Zustandsbehandlung.
- **OBSERVED:** `ERROR`-Case loggt parser error und behandelt als Match (fail-safe) via finalem Mapping.
- **OBSERVED:** Capture bei TRUE und ERROR vorhanden.

### C-04: Zentrales fail-safe Mapping klar dokumentiert

- **OBSERVED:** `isMaliciousLibinjectionResult` mappt `TRUE || ERROR` auf `true`.
- **Mechanismus:** explizites fail-safe Mapping in Utility-Funktion.

### C-05: Tests für ERROR-Pfad vorhanden

- **OBSERVED:** Test-Override-Funktionen geben `LIBINJECTION_RESULT_ERROR` zurück.
- **OBSERVED:** JSON-Testfälle erwarten Match (`ret: 1`) bei Override `error` für beide Operatoren.

---

## 5) Findings

### F-01
- **Severity:** medium
- **Titel:** Tri-State-Semantik endet an Detector-Grenze; Engine bleibt bi-state
- **Kategorie:** **architektonisch**
- **Exakte Code-Stellen:**
  - Tri-State zuletzt lokal: `detect_sqli.cc` (`sqli_result`, `switch`), `detect_xss.cc` (`xss_result`, `switch`)
  - Reduktion auf bool: `return isMaliciousLibinjectionResult(...)` in beiden Dateien
  - Nur bool-Propagation: `operator.cc` (`bool res`), `rule_with_operator.cc` (`bool ret`, match-Entscheidungen)
- **Relevanter Codepfad:**
  - `libinjection_*` -> `runLibinjection*` -> `Detect*::evaluate` (tri-state) -> `isMalicious...` (bool) -> `Operator::evaluateInternal` (bool) -> `RuleWithOperator::*` (bool)
- **Warum Information dort verloren geht:**
  - Beim `return bool` aus `Detect*::evaluate` wird `ERROR` in denselben Rückgabewert wie „Attacke erkannt“ gefaltet; danach existiert kein separater Zustand mehr im Interface.
- **OBSERVED:** bool-Verträge in `Operator`/`RuleWithOperator` sind binär.
- **INFERRED:** Kein zentraler, maschinenlesbarer Kanal für „parser-error“ bis zur Rule-Engine (außer lokale Logs/Capture-Details).
- **Warum gegen FULL MIGRATION:**
  - Full Migration entlang aller Call-Chains würde `ERROR` als eigenständigen Zustand bis zu den relevanten Systemgrenzen erhalten oder explizit typisieren.
- **Auswirkung:**
  - **Security correctness:** fail-safe bleibt erhalten (kein offensichtliches fail-open in diesem Pfad).
  - **Migration completeness:** tri-state nicht end-to-end.
- **ERROR-Verhalten:** fail-closed/fail-safe bei Enforcement, aber architektonisch zusammengefaltet.

### F-02
- **Severity:** low
- **Titel:** Fehlender expliziter Umgang mit unbekannten zukünftigen Result-Werten in Detector-Switches
- **Kategorie:** **lokal**
- **Exakte Code-Stellen:** `detect_sqli.cc` und `detect_xss.cc` `switch` ohne `default`.
- **Relevanter Codepfad:** `Detect*::evaluate -> switch(result) -> return isMalicious...`
- **Warum Information dort verloren geht:**
  - Unbekannte Enum-Werte würden nicht geloggt/behandelt im `switch`; final entscheidet nur `isMalicious...` (aktuell nur TRUE/ERROR=true).
- **OBSERVED:** `libinjectionResultToString` hat Fallback `"unexpected-result"`, aber wird nur in ERROR-Case verwendet.
- **INFERRED:** Bei API-Erweiterung könnte neues Ergebnis nicht explizit auffallen.
- **Warum gegen FULL MIGRATION:**
  - Strenge Migration fordert robuste, explizite Ergebnisbehandlung; fehlender Guard verschlechtert Zukunftssicherheit.
- **Auswirkung:** derzeit gering, potenziell semantische Drift bei zukünftigen libinjection-Enums.
- **ERROR-Verhalten:** unverändert fail-safe; Finding betrifft Zukunfts-/Robustheitsaspekt.

---

## 6) Test coverage assessment

- **OBSERVED:** Es existieren Unit-Tests für erzwungenes `LIBINJECTION_RESULT_ERROR` bei detectXSS/detectSQLi (Override + erwartetes Match + Capture).
- **OBSERVED:** Test-Harness unterstützt den Override über `libinjection_override`.
- **INFERRED:** Diese Tests beweisen lokale fail-safe Behandlung, aber nicht architekturelle Tri-State-Erhaltung (weil Interfaces bool sind).
- **UNCERTAIN:** Vollständige Regression-Abdeckung aller produktiven Regeln/Integrationspfade kann ohne Build/Execution aller Suiten nicht abschließend nachgewiesen werden.

---

## 7) Recommended fixes

### 7.1 Priorisierte Minimaländerungen (ohne große Architekturänderung)

1. **Lokal robuster machen:** In `detect_sqli.cc` und `detect_xss.cc` `default`-Case ergänzen, unbekannte Werte explizit loggen und fail-safe behandeln.
2. **Beobachtbarkeit erhöhen:** Bei ERROR einen expliziten, strukturierten Marker in `RuleMessage`/Transaction setzen (nicht nur Debug-Text), damit Downstream-Policy parser-error erkennen kann.

### 7.2 Saubere Architekturänderungen für „FULLY MIGRATED“

1. **Interface-Anpassung:** Operator-Rückgabevertrag von `bool` auf Tri-State/Statusobjekt erweitern (z. B. `OperatorEvalResult` mit Feldern `{matched, parser_error, detector}` oder direkt `injection_result_t` + Mapping-Layer).
2. **Engine-Anpassung:** `Operator::evaluateInternal`, `RuleWithOperator::executeOperatorAt`, `RuleWithOperator::evaluate` auf neuen Resulttyp umstellen.
3. **Policy-Anpassung:** Explizite Policy-Regeln für `ERROR` definieren (block/log/metric), statt impliziter bool-Faltung.
4. **Logging/Reasoning:** Match-Begründung trennen: `attack-detected` vs `parser-error-fail-safe`.

### 7.3 Zusätzliche Tests für FULL MIGRATION

1. Unit-Tests für Tri-State-End-to-End durch Engine (inkl. Negation/chaining).
2. Regressionstests, die unterscheiden zwischen TRUE und ERROR in Logging, RuleMessage, Actions.
3. Kompatibilitätstests für unbekannte Enum-Werte (default/guard behavior).

---

## 8) What would be required for FULLY MIGRATED

### 8.1 Minimale Anforderungen

- Alle libinjection-Aufrufer behalten Tri-State bis mindestens zur zentralen Rule-Entscheidung.
- Kein obligatorischer Informationsverlust an Operator-Grenze.
- Explizite Behandlung von TRUE/FALSE/ERROR (plus defensiv unbekannte Werte).

### 8.2 Architekturelle Anforderungen

- Bool-zentrierte Operator-Interfaces müssen erweitert oder durch typisierte Ergebnisse ergänzt werden.
- Rule-Engine muss parser errors separat tragen können (nicht nur als Match=true).
- Observability- und Enforcement-Kanäle müssen denselben Zustand konsistent verwenden.

### 8.3 Konkret betroffene Interfaces

- `Operator::evaluateInternal(...)`
- `RuleWithOperator::executeOperatorAt(...)`
- `RuleWithOperator::evaluate(...)`
- ggf. `RuleMessage`/Transaction-Metadaten für strukturierten Fehlerstatus

### 8.4 Notwendige zusätzliche Tests

- End-to-End Tri-State-Tests durch Operator -> Rule -> Action.
- Separate Assertions für TRUE vs ERROR in Audit-Output und Policy-Entscheidung.
- Chained-rule- und Negationsfälle mit ERROR.

---

## 9) Counterarguments and why they do or do not change the verdict

1. **„ERROR wird fail-safe behandelt, also reicht das.“**
   - **OBSERVED:** korrekt für Security-Enforcement im aktuellen Pfad.
   - **INFERRED:** reicht für *Security correctness*, aber nicht für *Full migration completeness* (Tri-State endet an bool-Interface).
   - **Bewertung:** ändert Urteil nicht zu FULLY MIGRATED.

2. **„Die Engine ist bewusst bool-basiert.“**
   - **OBSERVED:** ja, Engine ist binär.
   - **INFERRED:** Genau diese Architektur verhindert aber end-to-end Tri-State-Migration.
   - **Bewertung:** erklärt den Zustand, rechtfertigt aber nicht das Label FULLY MIGRATED.

3. **„Für Security reicht match=true bei ERROR.“**
   - **OBSERVED:** stimmt für fail-safe Blockierbarkeit.
   - **INFERRED:** Vollständigkeit der Migration umfasst mehr als Block/Allow; sie umfasst konsistente Ergebnissemantik entlang der Call-Chain.
   - **Bewertung:** verbessert Sicherheitsbewertung, aber nicht Vollständigkeitsbewertung.

---

## 10) Final verdict

`PARTIALLY MIGRATED`

- **OBSERVED:** Tri-State korrekt in den Detectoren und fail-safe Mapping auf Match.
- **OBSERVED:** Architekturelle bool-Grenze in Operator/Rule-Engine.
- **INFERRED:** Deshalb sicherheitslogisch solide, aber nicht vollständig als Full Migration im Sinne end-to-end Ergebnissemantik.

---

## 11) Decision Memo

- Security correctness: **PASS**
- Full migration completeness: **FAIL**
- Architectural consistency: **FAIL**
- Test adequacy: **FAIL**
- Final verdict: **PARTIALLY MIGRATED**
