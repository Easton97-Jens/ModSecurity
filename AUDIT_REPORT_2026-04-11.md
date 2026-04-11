# Audit-Bericht

## 1. Verwendete Dokumentationsquellen
- README.md

## 2. Verwendete Codequellen
- configure.ac
- headers/modsecurity/modsecurity.h
- headers/modsecurity/rules_set.h
- headers/modsecurity/transaction.h
- headers/modsecurity/anchored_variable.h
- src/modsecurity.cc
- src/transaction.cc
- test/benchmark/benchmark.cc
- test/Makefile.am
- examples/simple_example_using_c/test.c

## 3. Bestätigte Übereinstimmungen zwischen Doku und Code

- Aussage aus der Doku:
  - Das Projekt verwendet C++17.
- Befund im Code:
  - Das Build-System erzwingt C++17 via `AX_CXX_COMPILE_STDCXX(17, noext, mandatory)`.
- Evidenz:
  - configure.ac:63
- Bewertung: Bestätigt

- Aussage aus der Doku:
  - Es existieren C- und C++-Schnittstellen.
- Befund im Code:
  - C++-Klasse `ModSecurity` ist definiert, zusätzlich sind C-API-Symbole (`msc_init`, `msc_set_log_cb`, `msc_new_transaction`, `msc_process_*`) deklariert und implementiert.
- Evidenz:
  - headers/modsecurity/modsecurity.h:276-313,323-331
  - headers/modsecurity/transaction.h:639-715
  - src/modsecurity.cc:383-415
  - src/transaction.cc:1739-1930,2166-2180
- Bewertung: Bestätigt

- Aussage aus der Doku:
  - Es gibt Unit-/Regressionstests sowie ein Benchmark-Tool unter `test/benchmark/`.
- Befund im Code:
  - Test-Binaries `unit_tests`, `regression_tests` und Subdir `benchmark` sind im Build eingetragen; Benchmark-Quelle ist vorhanden.
- Evidenz:
  - test/Makefile.am:7-9,31-35,92-96
  - test/benchmark/benchmark.cc:47-176
- Bewertung: Bestätigt

## 4. Widersprüche oder Lücken

- Aussage aus der Doku:
  - „test utilities are located under the subfolder ‘tests’“.
- Problem im Code:
  - Das Repository nutzt den Pfad `test/` (singular), nicht `tests/`.
- Evidenz:
  - README.md:56
  - test/Makefile.am:7-9
- Risiko:
  - Dokumentationsirrtum bei Build-/Test-Automatisierung.
- Bewertung: Widerspruch

- Aussage aus der Doku:
  - YAJL sei eine „mandatory dependency“ für JSON.
- Problem im Code:
  - Konfigurierter JSON-Backend-Pfad ist `simdjson` oder `jsoncons`; YAJL wird in `configure.ac` nicht als mandatory dependency geprüft.
- Evidenz:
  - README.md:89
  - configure.ac:65-77,123-167
- Risiko:
  - Falsche Dependency-Annahmen in Build-Dokumentation.
- Bewertung: Widerspruch

- Aussage aus der Doku:
  - Falls libinjection fehlt, werde ohne `@detectXSS`/`SecRemoteRules` kompiliert.
- Problem im Code:
  - Fehlendes libinjection führt in `configure.ac` zu `AC_MSG_ERROR` (Konfiguration bricht ab), also nicht „degradiert weiterbauen“.
- Evidenz:
  - README.md:93-97
  - configure.ac:82-98
- Risiko:
  - CI/Deploy-Pipelines können statt Feature-Degradierung hart fehlschlagen.
- Bewertung: Widerspruch

- Aussage aus der Doku:
  - Benchmark-Tool rufe die Logging-Phase nicht auf.
- Problem im Code:
  - `benchmark.cc` ruft `modsecTransaction->processLogging();` in jeder Iteration auf.
- Evidenz:
  - README.md:322
  - test/benchmark/benchmark.cc:167-169
- Risiko:
  - Benchmark-Interpretation verfälscht, falls Nutzer Logging-kosten als „nicht enthalten“ annehmen.
- Bewertung: Widerspruch

- Aussage aus der Doku:
  - C-Beispiel zeigt `msc_rules_add_file(rules, main_rule_uri);`.
- Problem im Code:
  - Tatsächliche API-Signatur verlangt `const char **error` als drittes Argument.
- Evidenz:
  - README.md:155
  - headers/modsecurity/rules_set.h:104
  - examples/simple_example_using_c/test.c:41-45
- Risiko:
  - README-Beispiel ist mit aktueller API nicht kompilierbar.
- Bewertung: Widerspruch

## 5. Korrektheitsrisiken

- Befund:
  - C-Wrapper dereferenzieren Eingabepointer direkt ohne Null-Check (z.B. `transaction->processRequestBody()`).
- Warum riskant:
  - Bei fehlerhafter API-Nutzung kann ein Nullpointer-Dereferenz/Crash entstehen, statt sauberem Fehlercode.
- Evidenz:
  - src/transaction.cc:1769-1772,1798-1801,1820-1822,1844-1846,1903-1906,1928-1930,2179-2180
- Schweregrad: Mittel

- Befund:
  - `processRequestBody()` führt Verarbeitungsschritte aus, bevor `SecRequestBodyAccess`-Deaktivierung geprüft wird.
- Warum riskant:
  - Semantisch kann bereits Parsing/State-Mutation erfolgen, obwohl body access später als deaktiviert erkannt wird.
- Evidenz:
  - src/transaction.cc:685-805 (Parsing/Payload-Verarbeitung)
  - src/transaction.cc:807-823 (Access-Gate erst danach)
- Schweregrad: Mittel

## 6. Performance-Optimierungen

- Stelle im Code:
  - `src/transaction.cc`, `Transaction::processRequestBody()`
- Aktuelles Verhalten:
  - Wiederholte Aufrufe von `m_requestBody.str()` (u.a. Größenprüfung, Parser-Übergabe, Variablenbelegung).
- Warum potenziell teuer:
  - `stringstream::str()` materialisiert jeweils eine `std::string`-Kopie; mehrfach pro Request-Body-Pfad.
- Konkrete Verbesserung:
  - Einmalige Materialisierung (`const std::string body = m_requestBody.str();`) und Wiederverwendung.
- Erwarteter Effekt:
  - Weniger temporäre Allokationen/Kopien im Request-Body-Pfad.
- Evidenzgrad: Stark belegt
- Evidenz:
  - src/transaction.cc:690,708-709,713,757,784,838,844-847

- Stelle im Code:
  - `src/transaction.cc`, Aufbau von `fullRequest`
- Aktuelles Verhalten:
  - Iterative String-Konkatenation in Schleife (`fullRequest = fullRequest + ...`).
- Warum potenziell teuer:
  - Potenziell wiederholte Reallokationen und Kopien bei wachsendem String.
- Konkrete Verbesserung:
  - `reserve()` (falls Länge abschätzbar) und `append()` statt wiederholter `operator+`.
- Erwarteter Effekt:
  - Reduzierte Reallokationen im REQUEST_BODY-Pfad.
- Evidenzgrad: Plausibel aber unbewiesen
- Evidenz:
  - src/transaction.cc:829-838

- Stelle im Code:
  - `src/transaction.cc`, Cookie-Key-Trim in `addRequestHeader`
- Aktuelles Verhalten:
  - Linkstrim via `erase(0, 1)` in Schleife.
- Warum potenziell teuer:
  - Wiederholtes Front-Erase ist O(n) pro Schritt.
- Konkrete Verbesserung:
  - Startindex per Scan ermitteln und einmalig `substr`/View nutzen.
- Erwarteter Effekt:
  - Weniger Datenverschiebungen bei langen/ungewöhnlichen Cookie-Keys.
- Evidenzgrad: Plausibel aber unbewiesen
- Evidenz:
  - src/transaction.cc:534-536

## 7. Speicherverbrauch-Optimierungen

- Stelle im Code:
  - `src/transaction.cc`, `processRequestBody()`
- Aktuelles Verhalten:
  - Request-Body wird mehrfach materialisiert/kopiert (u.a. `m_requestBody.str()`, `m_variableFullRequest`, `m_variableRequestBody`).
- Warum speicherineffizient:
  - Parallel existierende String-Kopien erhöhen Peak-Memory pro Transaktion.
- Konkrete Verbesserung:
  - Kopien reduzieren (eine Body-Repräsentation wiederverwenden; `FULL_REQUEST` nur bei Bedarf erzeugen, siehe vorhandenes FIXME).
- Erwarteter Effekt:
  - Geringerer temporärer Speicherverbrauch bei großen Bodies.
- Evidenzgrad: Stark belegt
- Evidenz:
  - src/transaction.cc:826-847
  - headers/modsecurity/anchored_variable.h:53-64

## 8. Fehlende Evidenz

- Welche Aussagen konntest du nicht verifizieren?
  - „Higher performance“ als generelle Produkteigenschaft: Keine im Repository abgelegten, reproduzierbaren Vergleichswerte zwischen v2 und v3 gefunden. Nicht verifizierbar.
  - Aussage zur praktischen Gleichwertigkeit von C- und C++-API („objective is to have both APIs providing the same functionality“): als Ziel formuliert, aber kein Vollständigkeitsnachweis im Repository. In der Doku behauptet, im Code an dieser Stelle nicht eindeutig nachweisbar.

- Welche Benchmarks oder Tests fehlen?
  - Kein versioniertes Benchmark-Ergebnis-Dataset im Repository (nur Tool/Anleitung vorhanden).
  - Kein im Repository abgelegter Leistungs-Regressionstest mit Schwellenwerten gefunden.

- Welche Punkte dürfen ausdrücklich nicht behauptet werden?
  - Keine belastbare Aussage über reale Laufzeit-/Speichergewinne der vorgeschlagenen Optimierungen ohne Messung.
  - Keine belastbare Aussage über Verhalten aller Connectoren (nicht Teil dieses Repositories).

## 9. Wichtigste 5 Maßnahmen

1. README-Benchmarktext korrigieren („logging phase wird nicht aufgerufen“) oder Benchmark-Code an Dokumentation anpassen.
2. README-API-Beispiel für `msc_rules_add_file` auf aktuelle 3-Argument-Signatur aktualisieren.
3. README-Dependencies aktualisieren (YAJL/libinjection-Aussagen an tatsächliches `configure.ac` anpassen).
4. `processRequestBody()` optimieren: `m_requestBody.str()` einmal materialisieren und wiederverwenden.
5. `processRequestBody()`-Reihenfolge prüfen: Access-Gate (`SecRequestBodyAccess`) früher auswerten, falls semantisch gewünscht.
