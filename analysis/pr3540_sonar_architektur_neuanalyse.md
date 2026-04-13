# PR 3540 – Vollständige Neuanalyse (Sonar + Architektur)

## 1) Aktuellen Stand ermitteln
- PR-HEAD laut GitHub API: `3ec86cf63e095cddf4f22f6d9cfe3461bbbf4942`.
- Letzter Commit in der PR-Commitliste: `3ec86cf63e095cddf4f22f6d9cfe3461bbbf4942` (2026-04-13T18:16:29Z).
- Sonar PR-Eintrag referenziert Commit: `3ec86cf63e095cddf4f22f6d9cfe3461bbbf4942` (Analysis Date: 2026-04-13T18:17:07+0000).
- **Analyse basiert auf Commit: `3ec86cf63e095cddf4f22f6d9cfe3461bbbf4942`**.
- Neue Commits gegenüber Sonar-Analyse erkennbar: **Nein**.

## 2) Sonar-Daten aktuell abrufen
- Quality Gate abrufbar: **Ja**.
- Measures abrufbar: **Ja**.
- Security Hotspots abrufbar: **Ja** (Anzahl: 0).
- Issues abrufbar: **Ja** (API `total`: 213, vollständig paginiert).

## 3) Quality Gate Analyse
- Gate-Status: **OK**.
- Bedingungen:

| Metric | Status | Actual | Threshold | Comparator |
|---|---|---:|---:|---|
| new_reliability_rating | OK | 1 | 1 | GT |
| new_security_rating | OK | 1 | 1 | GT |
| new_maintainability_rating | OK | 1 | 1 | GT |
| new_duplicated_lines_density | OK | 0.0 | 3 | GT |
| new_security_hotspots_reviewed | OK | 100.0 | 100 | LT |
- Fehlbedingung: **Keine** (keine `ERROR`-Condition).

## 4) Tabelle: Alle Sonar-Befunde (vollständig enumeriert)

| ID | Kategorie | Regelcode | Severity | Status | Datei | Zeile | Nachricht | Codebereich | Direkt behebbar |
|---|---|---|---|---|---|---:|---|---|---|
| AZ2H7l4Bym_e-6l8FQml | Issue | cpp:S6009 | MINOR | OPEN | src/request_body_processor/json.cc | 39 | Replace this const reference to "std::string" by a "std::string_view". | Production | Teilweise |
| AZ2H7l3vym_e-6l8FQmj | Issue | cpp:S6024 | MINOR | OPEN | src/request_body_processor/json_adapter.cc | 56 | Prefer free functions over member functions when handling objects of generic type "InputType". | Production | Teilweise |
| AZ2H7l3vym_e-6l8FQmk | Issue | cpp:S995 | MINOR | OPEN | src/request_body_processor/json_adapter.cc | 57 | Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *". | Production | Teilweise |
| AZ2H7l3jym_e-6l8FQmi | Issue | cpp:S6004 | MINOR | OPEN | src/request_body_processor/json_backend_jsoncons.cc | 568 | Use the init-statement to declare "sync_detail" inside the if statement. | Production | Teilweise |
| AZ2H7l3jym_e-6l8FQmh | Issue | cpp:S1172 | MAJOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Remove the unused parameter "sink", make it unnamed, or declare it "[[maybe_unused]]". | Production | Nein |
| AZ2H7l0Nym_e-6l8FQmg | Issue | cpp:S6018 | MAJOR | OPEN | src/request_body_processor/json_instrumentation.cc | 13 | Use inline variables to define this global variable. | Production | Teilweise |
| AZ2HqeBsWym3B0O6okyS | Issue | cpp:S5952 | MINOR | OPEN | test/benchmark/json_benchmark.cc | 71 | Add a using-declaration to this derived class to inherit the constructors of "runtime_error", and remove the ones you manually duplicated. Note that this may add other constructors to your derived class. | Benchmark | Teilweise |
| AZ2HqeBsWym3B0O6okyT | Issue | cpp:S6004 | MINOR | OPEN | test/benchmark/json_benchmark.cc | 144 | Use the init-statement to declare "current" inside the if statement. | Benchmark | Teilweise |
| AZ2DWE24t-zbsGOGdN-_ | Issue | cpp:S3776 | CRITICAL | CLOSED | test/benchmark/json_benchmark.cc |  | Refactor this function to reduce its Cognitive Complexity from 33 to the 25 allowed. | Benchmark | Nein |
| AZ2DWE24t-zbsGOGdN_A | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ2DWE24t-zbsGOGdN_B | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ2DWE24t-zbsGOGdN_C | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ2DWE24t-zbsGOGdN_D | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ2DWE24t-zbsGOGdN_E | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ2DWE24t-zbsGOGdN_F | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ2DWE24t-zbsGOGdN_G | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ2DWE24t-zbsGOGdN_H | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ2DWE24t-zbsGOGdN_I | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ2DWE24t-zbsGOGdN_J | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ2DWE24t-zbsGOGdN_K | Issue | cpp:S4998 | MAJOR | OPEN | test/benchmark/json_benchmark.cc | 316 | Replace this use of "unique_ptr" by a raw pointer or a reference (possibly const). | Benchmark | Teilweise |
| AZ2DWE24t-zbsGOGdN_M | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ2DWE24t-zbsGOGdN_N | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ2DWE24t-zbsGOGdN_O | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ2DWE24t-zbsGOGdN_P | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ2DVDgODPiZK5yPV1-J | Issue | cpp:S1188 | MAJOR | OPEN | test/regression/regression_test.cc | 235 | This lambda has 23 lines, which is greater than the 20 lines authorized. Split it into several lambdas or functions, or make it a named function. | Test | Teilweise |
| AZ2DR4Fykud7vHWq_QVC | Issue | cpp:S6009 | MINOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Replace this const reference to "std::string" by a "std::string_view". | Production | Nein |
| AZ2DR4Fykud7vHWq_QVD | Issue | cpp:S995 | MINOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *". | Production | Nein |
| AZ2DR4Fykud7vHWq_QVE | Issue | cpp:S995 | MINOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Make the type of this parameter a reference-to-const. The current type of "input" is "std::string &". | Production | Nein |
| AZ2DK9KwXISY38E6wMPS | Issue | cpp:S995 | MINOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *". | Production | Nein |
| AZ2C_aavSTzC4JOHsQM1 | Issue | cpp:S1121 | MAJOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Extract the assignment from this expression. | Production | Nein |
| AZ2CwcldK0fgB4uOpVKy | Issue | cpp:S995 | MINOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Make the type of this parameter a reference-to-const. The current type of "input" is "std::string &". | Production | Nein |
| AZ2CwcldK0fgB4uOpVK0 | Issue | cpp:S995 | MINOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *". | Production | Nein |
| AZ2CwcldK0fgB4uOpVKz | Issue | cpp:S1172 | MAJOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Remove the unused parameter "options", make it unnamed, or declare it "[[maybe_unused]]". | Production | Nein |
| AZ2CwcnoK0fgB4uOpVK1 | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "tail" inside the if statement. | Production | Nein |
| AZ2CwcnoK0fgB4uOpVK2 | Issue | cpp:S1117 | MAJOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Declaration shadows a local variable "result" in the outer scope. | Production | Nein |
| AZ2CwcnoK0fgB4uOpVK3 | Issue | cpp:S1117 | MAJOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Declaration shadows a local variable "result" in the outer scope. | Production | Nein |
| AZ2CwcnoK0fgB4uOpVK4 | Issue | cpp:S5817 | MAJOR | OPEN | src/request_body_processor/json_backend_simdjson.cc | 401 | This function should be declared "const". | Production | Teilweise |
| AZ2CwcpOK0fgB4uOpVK5 | Issue | cpp:S6004 | MINOR | CLOSED | test/unit/json_backend_depth_tests.cc |  | Use the init-statement to declare "result" inside the if statement. | Test | Nein |
| AZ2CwcpOK0fgB4uOpVK6 | Issue | cpp:S6004 | MINOR | CLOSED | test/unit/json_backend_depth_tests.cc |  | Use the init-statement to declare "result" inside the if statement. | Test | Nein |
| AZ2CdKxRGCkM6OziHCww | Issue | cpp:S6004 | MINOR | CLOSED | test/unit/json_backend_depth_tests.cc |  | Use the init-statement to declare "result" inside the if statement. | Test | Nein |
| AZ2CdKxRGCkM6OziHCwx | Issue | cpp:S5945 | MAJOR | CLOSED | test/unit/json_backend_depth_tests.cc |  | Use "std::array" or "std::vector" instead of a C-style array. | Test | Nein |
| AZ2CdKxRGCkM6OziHCwy | Issue | cpp:S3628 | MINOR | CLOSED | test/unit/json_backend_depth_tests.cc |  | Convert this string literal to a raw string literal. | Test | Nein |
| AZ2CBI6Kkud7vHWq0tqj | Issue | cpp:S6022 | MAJOR | OPEN | src/operators/validate_byte_range.cc | 156 | Use "std::byte" for byte-oriented data manipulation. | Production | Teilweise |
| AZ2CA_0CGCkM6OziEPex | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nein |
| AZ2CA_xuGCkM6OziEPet | Issue | cpp:S5812 | MINOR | CLOSED | test/unit/json_backend_depth_tests.cc |  | Concatenate this namespace with the nested one. | Test | Nein |
| AZ2CA_xuGCkM6OziEPeu | Issue | cpp:S4144 | MAJOR | OPEN | test/unit/json_backend_depth_tests.cc | 50 | Update this method so that its implementation is not identical to on_key. | Test | Teilweise |
| AZ2CA_xuGCkM6OziEPev | Issue | cpp:S6004 | MINOR | CLOSED | test/unit/json_backend_depth_tests.cc |  | Use the init-statement to declare "result" inside the if statement. | Test | Nein |
| AZ2CA_xuGCkM6OziEPew | Issue | cpp:S6004 | MINOR | CLOSED | test/unit/json_backend_depth_tests.cc |  | Use the init-statement to declare "result" inside the if statement. | Test | Nein |
| AZ2BthMEO-njQfcv_7WG | Issue | cpp:S3776 | CRITICAL | OPEN | src/request_body_processor/json_backend_jsoncons.cc | 348 | Refactor this function to reduce its Cognitive Complexity from 41 to the 25 allowed. | Production | Teilweise |
| AZ2BthMEO-njQfcv_7WH | Issue | cpp:S1121 | MAJOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Extract the assignment from this expression. | Production | Nein |
| AZ2BthMEO-njQfcv_7WI | Issue | cpp:S1121 | MAJOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Extract the assignment from this expression. | Production | Nein |
| AZ2BthMEO-njQfcv_7WJ | Issue | cpp:S3776 | CRITICAL | OPEN | src/request_body_processor/json_backend_jsoncons.cc | 412 | Refactor this function to reduce its Cognitive Complexity from 43 to the 25 allowed. | Production | Teilweise |
| AZ2BthMEO-njQfcv_7WK | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Use the init-statement to declare "decoded_number" inside the if statement. | Production | Nein |
| AZ2BthMEO-njQfcv_7WL | Issue | cpp:S134 | CRITICAL | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements. | Production | Nein |
| AZ2BkKY5XISY38E6k-Ld | Issue | cpp:S1135 | INFO | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Complete the task associated to this "TODO" comment. | Production | Nein |
| AZ1-dn-nXISY38E6Txop | Issue | cpp:S5025 | CRITICAL | CLOSED | src/request_body_processor/json.cc |  | Rewrite the code so that you no longer need this "delete". | Production | Nein |
| AZ1-dn-nXISY38E6Txoq | Issue | cpp:S5827 | MAJOR | CLOSED | src/request_body_processor/json.cc |  | Replace the redundant type with "auto". | Production | Nein |
| AZ1-dn9hXISY38E6Txon | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json_instrumentation.cc |  | Concatenate this namespace with the nested one. | Production | Nein |
| AZ1-dn9hXISY38E6Txoo | Issue | cpp:S5421 | CRITICAL | CLOSED | src/request_body_processor/json_instrumentation.cc |  | Global variables should be const. | Production | Nein |
| AZ1-dn9QXISY38E6Txom | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json_instrumentation.h |  | Concatenate this namespace with the nested one. | Production | Nein |
| AZ1-dn4cXISY38E6Txoh | Issue | cpp:S4962 | CRITICAL | CLOSED | src/utils/msc_tree.cc |  | Use the "nullptr" literal. | Production | Nein |
| AZ1-doCWXISY38E6Txoz | Issue | cpp:S5421 | CRITICAL | CLOSED | test/benchmark/json_benchmark.cc |  | Global pointers should be const at every level. | Benchmark | Nein |
| AZ1-doCWXISY38E6Txo0 | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ1-doCWXISY38E6Txo1 | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ1-doCWXISY38E6Txo6 | Issue | cpp:S886 | MINOR | CLOSED | test/benchmark/json_benchmark.cc |  | Refactor this loop so that it is less error-prone. | Benchmark | Nein |
| AZ1-doCWXISY38E6Txo3 | Issue | cpp:S6004 | MINOR | CLOSED | test/benchmark/json_benchmark.cc |  | Use the init-statement to declare "output_format" inside the if statement. | Benchmark | Nein |
| AZ1-doCWXISY38E6Txo4 | Issue | cpp:S6004 | MINOR | CLOSED | test/benchmark/json_benchmark.cc |  | Use the init-statement to declare "is_invalid_scenario" inside the if statement. | Benchmark | Nein |
| AZ1-doCWXISY38E6TxpE | Issue | cpp:S5945 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Use "std::array" or "std::vector" instead of a C-style array. | Benchmark | Nein |
| AZ1-doCWXISY38E6TxpF | Issue | cpp:S5945 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Use "std::array" or "std::vector" instead of a C-style array. | Benchmark | Nein |
| AZ1-doCWXISY38E6TxpG | Issue | cpp:S7127 | CRITICAL | CLOSED | test/benchmark/json_benchmark.cc |  | Use "std::size" to get the size of this array. | Benchmark | Nein |
| AZ1-doCWXISY38E6TxpK | Issue | cpp:S6009 | MINOR | OPEN | test/benchmark/json_benchmark.cc | 321 | Replace this const reference to "std::string" by a "std::string_view". | Benchmark | Teilweise |
| AZ1-doCWXISY38E6TxpM | Issue | cpp:S7121 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Remove this redundant call to "c_str" when initializing a const "std::string" reference parameter. | Benchmark | Nein |
| AZ1-doCWXISY38E6TxpJ | Issue | cpp:S6004 | MINOR | CLOSED | test/benchmark/json_benchmark.cc |  | Use the init-statement to declare "parse_error" inside the if statement. | Benchmark | Nein |
| AZ1-doCWXISY38E6TxpS | Issue | cpp:S3628 | MINOR | CLOSED | test/benchmark/json_benchmark.cc |  | Convert this string literal to a raw string literal. | Benchmark | Nein |
| AZ1-doCWXISY38E6TxpT | Issue | cpp:S3628 | MINOR | CLOSED | test/benchmark/json_benchmark.cc |  | Convert this string literal to a raw string literal. | Benchmark | Nein |
| AZ1-doCWXISY38E6TxpR | Issue | cpp:S6009 | MINOR | OPEN | test/benchmark/json_benchmark.cc | 419 | Replace this const reference to "std::string" by a "std::string_view". | Benchmark | Teilweise |
| AZ1-doCWXISY38E6TxpV | Issue | cpp:S6004 | MINOR | CLOSED | test/benchmark/json_benchmark.cc |  | Use the init-statement to declare "rules_path" inside the if statement. | Benchmark | Nein |
| AZ1-doCWXISY38E6TxpU | Issue | cpp:S6009 | MINOR | OPEN | test/benchmark/json_benchmark.cc | 486 | Replace this const reference to "std::string" by a "std::string_view". | Benchmark | Teilweise |
| AZ1-doCWXISY38E6Txo2 | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ1-doCWXISY38E6Txo5 | Issue | cpp:S3776 | CRITICAL | CLOSED | test/benchmark/json_benchmark.cc |  | Refactor this function to reduce its Cognitive Complexity from 33 to the 25 allowed. | Benchmark | Nein |
| AZ1-doCWXISY38E6Txo7 | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ1-doCWXISY38E6Txo8 | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ1-doCWXISY38E6Txo9 | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ1-doCWXISY38E6Txo- | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ1-doCWXISY38E6Txo_ | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ1-doCWXISY38E6TxpA | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ1-doCWXISY38E6TxpB | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ1-doCWXISY38E6TxpC | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ1-doCWXISY38E6TxpD | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ1-doCWXISY38E6TxpH | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ1-doCWXISY38E6TxpI | Issue | cpp:S4998 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Replace this use of "unique_ptr" by a raw pointer or a reference (possibly const). | Benchmark | Nein |
| AZ1-doCWXISY38E6TxpN | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ1-doCWXISY38E6TxpO | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ1-doCWXISY38E6TxpP | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ1-doCWXISY38E6TxpQ | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Benchmark | Nein |
| AZ1-doCHXISY38E6Txor | Issue | shelldre:S7682 | MAJOR | CLOSED | test/benchmark/run-json-benchmarks.sh |  | Add an explicit return statement at the end of the function. | Benchmark | Nein |
| AZ1-doCHXISY38E6Txos | Issue | shelldre:S7688 | MAJOR | CLOSED | test/benchmark/run-json-benchmarks.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Benchmark | Nein |
| AZ1-doCHXISY38E6Txot | Issue | shelldre:S7688 | MAJOR | CLOSED | test/benchmark/run-json-benchmarks.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Benchmark | Nein |
| AZ1-doCHXISY38E6Txou | Issue | shelldre:S7688 | MAJOR | CLOSED | test/benchmark/run-json-benchmarks.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Benchmark | Nein |
| AZ1-doCHXISY38E6Txov | Issue | shelldre:S7688 | MAJOR | CLOSED | test/benchmark/run-json-benchmarks.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Benchmark | Nein |
| AZ1-doCHXISY38E6Txow | Issue | shelldre:S7688 | MAJOR | CLOSED | test/benchmark/run-json-benchmarks.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Benchmark | Nein |
| AZ1-doCHXISY38E6Txox | Issue | shelldre:S7688 | MAJOR | CLOSED | test/benchmark/run-json-benchmarks.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Benchmark | Nein |
| AZ1-doCHXISY38E6Txoy | Issue | shelldre:S7688 | MAJOR | CLOSED | test/benchmark/run-json-benchmarks.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Benchmark | Nein |
| AZ1-doDkXISY38E6TxpW | Issue | shelldre:S7682 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Add an explicit return statement at the end of the function. | Test | Nein |
| AZ1-doDkXISY38E6TxpX | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nein |
| AZ1-doDkXISY38E6TxpY | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nein |
| AZ1-doDkXISY38E6TxpZ | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nein |
| AZ1-doDkXISY38E6Txpa | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nein |
| AZ1-doDkXISY38E6Txpb | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nein |
| AZ1-doDkXISY38E6Txpc | Issue | shelldre:S7682 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Add an explicit return statement at the end of the function. | Test | Nein |
| AZ1-doDkXISY38E6Txpd | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nein |
| AZ1-doDkXISY38E6Txpe | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nein |
| AZ1-doDkXISY38E6Txpf | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nein |
| AZ1-doDkXISY38E6Txpg | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nein |
| AZ1-doDkXISY38E6Txph | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nein |
| AZ1-doDkXISY38E6Txpi | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nein |
| AZ1-doDkXISY38E6Txpj | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nein |
| AZ1-doDkXISY38E6Txpk | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nein |
| AZ1-doDkXISY38E6Txpl | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nein |
| AZ190QEMSTzC4JOHOn9q | Issue | cpp:S6022 | MAJOR | OPEN | src/operators/validate_byte_range.cc | 72 | Use "std::byte" for byte-oriented data manipulation. | Production | Teilweise |
| AZ190QEMSTzC4JOHOn9r | Issue | cpp:S6004 | MINOR | CLOSED | src/operators/validate_byte_range.cc |  | Use the init-statement to declare "token" inside the if statement. | Production | Nein |
| AZ190QGTSTzC4JOHOn-H | Issue | cpp:S3776 | CRITICAL | CLOSED | src/request_body_processor/json.cc |  | Refactor this function to reduce its Cognitive Complexity from 37 to the 25 allowed. | Production | Nein |
| AZ190QGTSTzC4JOHOn-D | Issue | cpp:S3230 | MAJOR | OPEN | src/request_body_processor/json.cc | 123 | Do not use the constructor's initializer list for data member "m_data". Use the in-class initializer instead. | Production | Teilweise |
| AZ190QGTSTzC4JOHOn-G | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json.cc |  | Use the init-statement to declare "result" inside the if statement. | Production | Nein |
| AZ190QGTSTzC4JOHOn-I | Issue | cpp:S134 | CRITICAL | CLOSED | src/request_body_processor/json.cc |  | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements. | Production | Nein |
| AZ190QGTSTzC4JOHOn-J | Issue | cpp:S134 | CRITICAL | CLOSED | src/request_body_processor/json.cc |  | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements. | Production | Nein |
| AZ190QGTSTzC4JOHOn-K | Issue | cpp:S134 | CRITICAL | CLOSED | src/request_body_processor/json.cc |  | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements. | Production | Nein |
| AZ190QGTSTzC4JOHOn-L | Issue | cpp:S134 | CRITICAL | CLOSED | src/request_body_processor/json.cc |  | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements. | Production | Nein |
| AZ190QGTSTzC4JOHOn-M | Issue | cpp:S134 | CRITICAL | CLOSED | src/request_body_processor/json.cc |  | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements. | Production | Nein |
| AZ190QGTSTzC4JOHOn-E | Issue | cpp:S4144 | MAJOR | CLOSED | src/request_body_processor/json.cc |  | Update this method so that its implementation is not identical to on_end_object. | Production | Nein |
| AZ190QGTSTzC4JOHOn-N | Issue | cpp:S1155 | MINOR | CLOSED | src/request_body_processor/json.cc |  | Use "empty()" to check whether the container is empty or not. | Production | Nein |
| AZ190QGTSTzC4JOHOn-O | Issue | cpp:S1155 | MINOR | CLOSED | src/request_body_processor/json.cc |  | Use "empty()" to check whether the container is empty or not. | Production | Nein |
| AZ190QEtSTzC4JOHOn9t | Issue | cpp:S3624 | CRITICAL | CLOSED | src/request_body_processor/json.h |  | Customize this class' copy constructor to participate in resource management. Customize or delete its copy assignment operator. Also consider whether move operations should be customized. | Production | Nein |
| AZ190QF1STzC4JOHOn9- | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Concatenate this namespace with the nested one. | Production | Nein |
| AZ190QF1STzC4JOHOn-A | Issue | cpp:S1172 | MAJOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Remove the unused parameter "options", make it unnamed, or declare it "[[maybe_unused]]". | Production | Nein |
| AZ190QF1STzC4JOHOn-B | Issue | cpp:S995 | MINOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *". | Production | Nein |
| AZ190QF1STzC4JOHOn9_ | Issue | cpp:S6009 | MINOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Replace this const reference to "std::string" by a "std::string_view". | Production | Nein |
| AZ190QF8STzC4JOHOn-C | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json_adapter.h |  | Concatenate this namespace with the nested one. | Production | Nein |
| AZ190QGbSTzC4JOHOn-P | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json_backend.h |  | Concatenate this namespace with the nested one. | Production | Nein |
| AZ190QFsSTzC4JOHOn9v | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Concatenate this namespace with the nested one. | Production | Nein |
| AZ190QFsSTzC4JOHOn9w | Issue | cpp:S3776 | CRITICAL | OPEN | src/request_body_processor/json_backend_jsoncons.cc | 92 | Refactor this function to reduce its Cognitive Complexity from 33 to the 25 allowed. | Production | Teilweise |
| AZ190QFsSTzC4JOHOn9u | Issue | cpp:S3562 | MAJOR | OPEN | src/request_body_processor/json_backend_jsoncons.cc | 188 | 4 enumeration values not handled in switch: 'int64_value', 'uint64_value', 'half_value'... | Production | Teilweise |
| AZ190QFsSTzC4JOHOn9z | Issue | cpp:S3776 | CRITICAL | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Refactor this function to reduce its Cognitive Complexity from 41 to the 25 allowed. | Production | Nein |
| AZ190QFsSTzC4JOHOn92 | Issue | cpp:S1121 | MAJOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Extract the assignment from this expression. | Production | Nein |
| AZ190QFsSTzC4JOHOn9x | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Use the init-statement to declare "current" inside the if statement. | Production | Nein |
| AZ190QFsSTzC4JOHOn93 | Issue | cpp:S1121 | MAJOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Extract the assignment from this expression. | Production | Nein |
| AZ190QFsSTzC4JOHOn94 | Issue | cpp:S3776 | CRITICAL | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Refactor this function to reduce its Cognitive Complexity from 43 to the 25 allowed. | Production | Nein |
| AZ190QFsSTzC4JOHOn90 | Issue | cpp:S134 | CRITICAL | OPEN | src/request_body_processor/json_backend_jsoncons.cc | 365 | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements. | Production | Teilweise |
| AZ190QFsSTzC4JOHOn91 | Issue | cpp:S134 | CRITICAL | OPEN | src/request_body_processor/json_backend_jsoncons.cc | 374 | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements. | Production | Teilweise |
| AZ190QFsSTzC4JOHOn9y | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Use the init-statement to declare "escaped" inside the if statement. | Production | Nein |
| AZ190QFsSTzC4JOHOn97 | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Use the init-statement to declare "sync_detail" inside the if statement. | Production | Nein |
| AZ190QFsSTzC4JOHOn96 | Issue | cpp:S6009 | MINOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Replace this const reference to "std::string" by a "std::string_view". | Production | Nein |
| AZ190QFsSTzC4JOHOn95 | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Use the init-statement to declare "end" inside the if statement. | Production | Nein |
| AZ190QFsSTzC4JOHOn98 | Issue | cpp:S3776 | CRITICAL | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Refactor this function to reduce its Cognitive Complexity from 48 to the 25 allowed. | Production | Nein |
| AZ190QFsSTzC4JOHOn99 | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Use the init-statement to declare "result" inside the if statement. | Production | Nein |
| AZ190QGkSTzC4JOHOn-Q | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Concatenate this namespace with the nested one. | Production | Nein |
| AZ190QGkSTzC4JOHOn-R | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "sink_status" inside the if statement. | Production | Nein |
| AZ190QGkSTzC4JOHOn-S | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "sink_status" inside the if statement. | Production | Nein |
| AZ190QGkSTzC4JOHOn-T | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "sink_status" inside the if statement. | Production | Nein |
| AZ190QGkSTzC4JOHOn-U | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "sink_status" inside the if statement. | Production | Nein |
| AZ190QGkSTzC4JOHOn-V | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "result" inside the if statement. | Production | Nein |
| AZ190QGkSTzC4JOHOn-W | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "sink_status" inside the if statement. | Production | Nein |
| AZ190QGkSTzC4JOHOn-X | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "result" inside the if statement. | Production | Nein |
| AZ190QGkSTzC4JOHOn-Y | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "sink_status" inside the if statement. | Production | Nein |
| AZ190QGkSTzC4JOHOn-Z | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "sink_status" inside the if statement. | Production | Nein |
| AZ190QGkSTzC4JOHOn-a | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "result" inside the if statement. | Production | Nein |
| AZ190QGkSTzC4JOHOn-b | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "sink_status" inside the if statement. | Production | Nein |
| AZ190QHkSTzC4JOHOn-c | Issue | cpp:S7121 | MAJOR | CLOSED | src/transaction.cc |  | Remove this redundant call to "c_str" when initializing a const "std::string" reference parameter. | Production | Nein |
| AZ190QA8STzC4JOHOn9j | Issue | cpp:S5812 | MINOR | CLOSED | src/utils/json_writer.cc |  | Concatenate this namespace with the nested one. | Production | Nein |
| AZ190QA8STzC4JOHOn9h | Issue | cpp:S3230 | MAJOR | CLOSED | src/utils/json_writer.cc |  | Remove this use of the constructor's initializer list for data member "m_output". It is redundant with default initialization behavior. | Production | Nein |
| AZ190QA8STzC4JOHOn9i | Issue | cpp:S3230 | MAJOR | CLOSED | src/utils/json_writer.cc |  | Remove this use of the constructor's initializer list for data member "m_stack". It is redundant with default initialization behavior. | Production | Nein |
| AZ190QA8STzC4JOHOn9k | Issue | cpp:S5945 | MAJOR | OPEN | src/utils/json_writer.cc | 155 | Use "std::string" instead of a C-style char array. | Production | Teilweise |
| AZ190QA8STzC4JOHOn9l | Issue | cpp:S3628 | MINOR | CLOSED | src/utils/json_writer.cc |  | Convert this string literal to a raw string literal. | Production | Nein |
| AZ190QA8STzC4JOHOn9m | Issue | cpp:S3628 | MINOR | CLOSED | src/utils/json_writer.cc |  | Convert this string literal to a raw string literal. | Production | Nein |
| AZ190QA8STzC4JOHOn9n | Issue | cpp:S6022 | MAJOR | OPEN | src/utils/json_writer.cc | 183 | Use "std::byte" for byte-oriented data manipulation. | Production | Teilweise |
| AZ190QA8STzC4JOHOn9o | Issue | cpp:S6022 | MAJOR | OPEN | src/utils/json_writer.cc | 184 | Use "std::byte" for byte-oriented data manipulation. | Production | Teilweise |
| AZ190QD4STzC4JOHOn9p | Issue | cpp:S5812 | MINOR | CLOSED | src/utils/json_writer.h |  | Concatenate this namespace with the nested one. | Production | Nein |
| AZ190QIVSTzC4JOHOn-f | Issue | cpp:S5812 | MINOR | CLOSED | test/common/json.h |  | Concatenate this namespace with the nested one. | Test | Nein |
| AZ190QIVSTzC4JOHOn-g | Issue | cpp:S2807 | MAJOR | OPEN | test/common/json.h | 78 | Make this member overloaded operator a hidden friend. | Test | Teilweise |
| AZ190QIVSTzC4JOHOn-h | Issue | cpp:S2807 | MAJOR | OPEN | test/common/json.h | 123 | Make this member overloaded operator a hidden friend. | Test | Teilweise |
| AZ190QIVSTzC4JOHOn-i | Issue | cpp:S1181 | MAJOR | OPEN | test/common/json.h | 219 | Catch a more specific exception instead of a generic one. | Test | Teilweise |
| AZ190QIVSTzC4JOHOn-j | Issue | cpp:S995 | MINOR | OPEN | test/common/json.h | 232 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Test | Teilweise |
| AZ190QIVSTzC4JOHOn-k | Issue | cpp:S995 | MINOR | OPEN | test/common/json.h | 242 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Test | Teilweise |
| AZ190QIVSTzC4JOHOn-l | Issue | cpp:S995 | MINOR | OPEN | test/common/json.h | 252 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Test | Teilweise |
| AZ190QIVSTzC4JOHOn-m | Issue | cpp:S995 | MINOR | OPEN | test/common/json.h | 262 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Test | Teilweise |
| AZ190QIVSTzC4JOHOn-n | Issue | cpp:S995 | MINOR | OPEN | test/common/json.h | 269 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Test | Teilweise |
| AZ190QIVSTzC4JOHOn-o | Issue | cpp:S995 | MINOR | OPEN | test/common/json.h | 279 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Test | Teilweise |
| AZ190QIVSTzC4JOHOn-p | Issue | cpp:S1181 | MAJOR | OPEN | test/common/json.h | 309 | Catch a more specific exception instead of a generic one. | Test | Teilweise |
| AZ190QIVSTzC4JOHOn-q | Issue | cpp:S1181 | MAJOR | OPEN | test/common/json.h | 321 | Catch a more specific exception instead of a generic one. | Test | Teilweise |
| AZ190QIVSTzC4JOHOn-r | Issue | cpp:S1181 | MAJOR | OPEN | test/common/json.h | 333 | Catch a more specific exception instead of a generic one. | Test | Teilweise |
| AZ190QI2STzC4JOHOn-s | Issue | cpp:S134 | CRITICAL | OPEN | test/common/modsecurity_test.cc | 89 | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements. | Test | Teilweise |
| AZ190QKvSTzC4JOHOn-x | Issue | cpp:S5415 | MAJOR | CLOSED | test/regression/regression_test.cc |  | The result of "std::move" should not be passed as a const reference. | Test | Nein |
| AZ190QKvSTzC4JOHOn-y | Issue | cpp:S5415 | MAJOR | CLOSED | test/regression/regression_test.cc |  | The result of "std::move" should not be passed as a const reference. | Test | Nein |
| AZ190QKvSTzC4JOHOn-z | Issue | cpp:S5415 | MAJOR | CLOSED | test/regression/regression_test.cc |  | The result of "std::move" should not be passed as a const reference. | Test | Nein |
| AZ190QKvSTzC4JOHOn-0 | Issue | cpp:S5415 | MAJOR | CLOSED | test/regression/regression_test.cc |  | The result of "std::move" should not be passed as a const reference. | Test | Nein |
| AZ190QKvSTzC4JOHOn-1 | Issue | cpp:S5415 | MAJOR | CLOSED | test/regression/regression_test.cc |  | The result of "std::move" should not be passed as a const reference. | Test | Nein |
| AZ190QKvSTzC4JOHOn-2 | Issue | cpp:S5415 | MAJOR | CLOSED | test/regression/regression_test.cc |  | The result of "std::move" should not be passed as a const reference. | Test | Nein |
| AZ190QKvSTzC4JOHOn-3 | Issue | cpp:S5415 | MAJOR | CLOSED | test/regression/regression_test.cc |  | The result of "std::move" should not be passed as a const reference. | Test | Nein |
| AZ190QKvSTzC4JOHOn-4 | Issue | cpp:S5817 | MAJOR | OPEN | test/regression/regression_test.cc | 431 | This function should be declared "const". | Test | Teilweise |
| AZ190QKvSTzC4JOHOn-v | Issue | cpp:S5274 | MAJOR | CLOSED | test/regression/regression_test.cc |  | moving a temporary object prevents copy elision | Test | Nein |
| AZ190QKvSTzC4JOHOn-w | Issue | cpp:S5274 | MAJOR | CLOSED | test/regression/regression_test.cc |  | moving a temporary object prevents copy elision | Test | Nein |
| AZ190QKvSTzC4JOHOn-5 | Issue | cpp:S1481 | MINOR | CLOSED | test/regression/regression_test.cc |  | Remove the unused lambda capture "writer". | Test | Nein |
| AZ190QJKSTzC4JOHOn-t | Issue | cpp:S5415 | MAJOR | CLOSED | test/unit/unit_test.cc |  | The result of "std::move" should not be passed as a const reference. | Test | Nein |
| AZ190QJYSTzC4JOHOn-u | Issue | cpp:S836 | MAJOR | CLOSED | test/unit/unit_test.h |  | Value assigned to field 'ret' in implicit constructor is garbage or undefined | Test | Nein |
| AZ190QIESTzC4JOHOn-d | Issue | cpp:S886 | MINOR | OPEN | src/modsecurity.cc | 232 | Refactor this loop so that it is less error-prone. | Production | Teilweise |
| AZ190QIESTzC4JOHOn-e | Issue | cpp:S886 | MINOR | OPEN | src/modsecurity.cc | 288 | Refactor this loop so that it is less error-prone. | Production | Teilweise |
| AZ190QGTSTzC4JOHOn-F | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json.cc |  | Concatenate this namespace with the nested one. | Production | Nein |
| AZ190QEtSTzC4JOHOn9s | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json.h |  | Concatenate this namespace with the nested one. | Production | Nein |
| AZ1-dn4cXISY38E6Txog | Issue | cpp:S4962 | CRITICAL | CLOSED | src/utils/msc_tree.cc |  | Use the "nullptr" literal. | Production | Nein |
| AZ1-dn4cXISY38E6Txoi | Issue | cpp:S4962 | CRITICAL | CLOSED | src/utils/msc_tree.cc |  | Use the "nullptr" literal. | Production | Nein |
| AZ1-dn4cXISY38E6Txoj | Issue | cpp:S4962 | CRITICAL | CLOSED | src/utils/msc_tree.cc |  | Use the "nullptr" literal. | Production | Nein |
| AZ1-dn4cXISY38E6Txok | Issue | cpp:S4962 | CRITICAL | CLOSED | src/utils/msc_tree.cc |  | Use the "nullptr" literal. | Production | Nein |
| AZ1-dn4cXISY38E6Txol | Issue | cpp:S4962 | CRITICAL | CLOSED | src/utils/msc_tree.cc |  | Use the "nullptr" literal. | Production | Nein |
| gate::new_reliability_rating | Gate Condition | new_reliability_rating | n/a | OK | n/a |  | actual=1, threshold=1, comparator=GT | n/a | Nein |
| gate::new_security_rating | Gate Condition | new_security_rating | n/a | OK | n/a |  | actual=1, threshold=1, comparator=GT | n/a | Nein |
| gate::new_maintainability_rating | Gate Condition | new_maintainability_rating | n/a | OK | n/a |  | actual=1, threshold=1, comparator=GT | n/a | Nein |
| gate::new_duplicated_lines_density | Gate Condition | new_duplicated_lines_density | n/a | OK | n/a |  | actual=0.0, threshold=3, comparator=GT | n/a | Nein |
| gate::new_security_hotspots_reviewed | Gate Condition | new_security_hotspots_reviewed | n/a | OK | n/a |  | actual=100.0, threshold=100, comparator=LT | n/a | Nein |

## 5) Tabelle: Nur offene Befunde (OPEN/TO_REVIEW/CONFIRMED/REOPENED)

| ID | Kategorie | Regelcode | Severity | Datei | Zeile | Kurzbeschreibung | Konkreter technischer Fix | Risiko der Änderung |
|---|---|---|---|---|---:|---|---|---|
| AZ2H7l4Bym_e-6l8FQml | Issue | cpp:S6009 | MINOR | src/request_body_processor/json.cc | 39 | Replace this const reference to "std::string" by a "std::string_view". | Grenzfallprüfung ergänzen (z. B. leere Eingaben, Null-/Range-Checks). | Mittel |
| AZ2H7l3vym_e-6l8FQmj | Issue | cpp:S6024 | MINOR | src/request_body_processor/json_adapter.cc | 56 | Prefer free functions over member functions when handling objects of generic type "InputType". | Unnötige Kopie/Move vermeiden (Referenz/const& bevorzugen). | Niedrig |
| AZ2H7l3vym_e-6l8FQmk | Issue | cpp:S995 | MINOR | src/request_body_processor/json_adapter.cc | 57 | Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *". | Ternären Ausdruck/Verzweigung vereinheitlichen und lesbarer machen. | Niedrig |
| AZ2H7l3jym_e-6l8FQmi | Issue | cpp:S6004 | MINOR | src/request_body_processor/json_backend_jsoncons.cc | 568 | Use the init-statement to declare "sync_detail" inside the if statement. | Funktion in kleinere, klar abgegrenzte Helfer zerlegen und Zweiglogik auslagern. | Mittel |
| AZ2H7l0Nym_e-6l8FQmg | Issue | cpp:S6018 | MAJOR | src/request_body_processor/json_instrumentation.cc | 13 | Use inline variables to define this global variable. | Statische Initialisierung vereinfachen oder klar kapseln, um Nebenwirkungen zu vermeiden. | Niedrig |
| AZ2HqeBsWym3B0O6okyS | Issue | cpp:S5952 | MINOR | test/benchmark/json_benchmark.cc | 71 | Add a using-declaration to this derived class to inherit the constructors of "runtime_error", and remove the ones you manually duplicated. Note that this may add other constructors to your derived class. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Niedrig |
| AZ2HqeBsWym3B0O6okyT | Issue | cpp:S6004 | MINOR | test/benchmark/json_benchmark.cc | 144 | Use the init-statement to declare "current" inside the if statement. | Funktion in kleinere, klar abgegrenzte Helfer zerlegen und Zweiglogik auslagern. | Mittel |
| AZ2DWE24t-zbsGOGdN_K | Issue | cpp:S4998 | MAJOR | test/benchmark/json_benchmark.cc | 316 | Replace this use of "unique_ptr" by a raw pointer or a reference (possibly const). | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Niedrig |
| AZ2DVDgODPiZK5yPV1-J | Issue | cpp:S1188 | MAJOR | test/regression/regression_test.cc | 235 | This lambda has 23 lines, which is greater than the 20 lines authorized. Split it into several lambdas or functions, or make it a named function. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Niedrig |
| AZ2CwcnoK0fgB4uOpVK4 | Issue | cpp:S5817 | MAJOR | src/request_body_processor/json_backend_simdjson.cc | 401 | This function should be declared "const". | Pointer/Array-Operation vor Nutzung explizit auf Grenzen prüfen. | Mittel |
| AZ2CBI6Kkud7vHWq0tqj | Issue | cpp:S6022 | MAJOR | src/operators/validate_byte_range.cc | 156 | Use "std::byte" for byte-oriented data manipulation. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Niedrig |
| AZ2CA_xuGCkM6OziEPeu | Issue | cpp:S4144 | MAJOR | test/unit/json_backend_depth_tests.cc | 50 | Update this method so that its implementation is not identical to on_key. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Niedrig |
| AZ2BthMEO-njQfcv_7WG | Issue | cpp:S3776 | CRITICAL | src/request_body_processor/json_backend_jsoncons.cc | 348 | Refactor this function to reduce its Cognitive Complexity from 41 to the 25 allowed. | Kognitive Komplexität reduzieren: frühe Returns und Teilfunktionen pro Event-Typ. | Mittel |
| AZ2BthMEO-njQfcv_7WJ | Issue | cpp:S3776 | CRITICAL | src/request_body_processor/json_backend_jsoncons.cc | 412 | Refactor this function to reduce its Cognitive Complexity from 43 to the 25 allowed. | Kognitive Komplexität reduzieren: frühe Returns und Teilfunktionen pro Event-Typ. | Mittel |
| AZ1-doCWXISY38E6TxpK | Issue | cpp:S6009 | MINOR | test/benchmark/json_benchmark.cc | 321 | Replace this const reference to "std::string" by a "std::string_view". | Grenzfallprüfung ergänzen (z. B. leere Eingaben, Null-/Range-Checks). | Mittel |
| AZ1-doCWXISY38E6TxpR | Issue | cpp:S6009 | MINOR | test/benchmark/json_benchmark.cc | 419 | Replace this const reference to "std::string" by a "std::string_view". | Grenzfallprüfung ergänzen (z. B. leere Eingaben, Null-/Range-Checks). | Mittel |
| AZ1-doCWXISY38E6TxpU | Issue | cpp:S6009 | MINOR | test/benchmark/json_benchmark.cc | 486 | Replace this const reference to "std::string" by a "std::string_view". | Grenzfallprüfung ergänzen (z. B. leere Eingaben, Null-/Range-Checks). | Mittel |
| AZ190QEMSTzC4JOHOn9q | Issue | cpp:S6022 | MAJOR | src/operators/validate_byte_range.cc | 72 | Use "std::byte" for byte-oriented data manipulation. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Niedrig |
| AZ190QGTSTzC4JOHOn-D | Issue | cpp:S3230 | MAJOR | src/request_body_processor/json.cc | 123 | Do not use the constructor's initializer list for data member "m_data". Use the in-class initializer instead. | Signatur/Überladung vereinheitlichen bzw. const-Korrektheit herstellen. | Niedrig |
| AZ190QFsSTzC4JOHOn9w | Issue | cpp:S3776 | CRITICAL | src/request_body_processor/json_backend_jsoncons.cc | 92 | Refactor this function to reduce its Cognitive Complexity from 33 to the 25 allowed. | Kognitive Komplexität reduzieren: frühe Returns und Teilfunktionen pro Event-Typ. | Mittel |
| AZ190QFsSTzC4JOHOn9u | Issue | cpp:S3562 | MAJOR | src/request_body_processor/json_backend_jsoncons.cc | 188 | 4 enumeration values not handled in switch: 'int64_value', 'uint64_value', 'half_value'... | String/Token-Synchronisation in dedizierte Utility-Funktion extrahieren. | Mittel |
| AZ190QFsSTzC4JOHOn90 | Issue | cpp:S134 | CRITICAL | src/request_body_processor/json_backend_jsoncons.cc | 365 | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements. | Verschachtelung reduzieren (Guard Clauses / frühe continue-return). | Mittel |
| AZ190QFsSTzC4JOHOn91 | Issue | cpp:S134 | CRITICAL | src/request_body_processor/json_backend_jsoncons.cc | 374 | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements. | Verschachtelung reduzieren (Guard Clauses / frühe continue-return). | Mittel |
| AZ190QA8STzC4JOHOn9k | Issue | cpp:S5945 | MAJOR | src/utils/json_writer.cc | 155 | Use "std::string" instead of a C-style char array. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Niedrig |
| AZ190QA8STzC4JOHOn9n | Issue | cpp:S6022 | MAJOR | src/utils/json_writer.cc | 183 | Use "std::byte" for byte-oriented data manipulation. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Niedrig |
| AZ190QA8STzC4JOHOn9o | Issue | cpp:S6022 | MAJOR | src/utils/json_writer.cc | 184 | Use "std::byte" for byte-oriented data manipulation. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Niedrig |
| AZ190QIVSTzC4JOHOn-g | Issue | cpp:S2807 | MAJOR | test/common/json.h | 78 | Make this member overloaded operator a hidden friend. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Niedrig |
| AZ190QIVSTzC4JOHOn-h | Issue | cpp:S2807 | MAJOR | test/common/json.h | 123 | Make this member overloaded operator a hidden friend. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Niedrig |
| AZ190QIVSTzC4JOHOn-i | Issue | cpp:S1181 | MAJOR | test/common/json.h | 219 | Catch a more specific exception instead of a generic one. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Niedrig |
| AZ190QIVSTzC4JOHOn-j | Issue | cpp:S995 | MINOR | test/common/json.h | 232 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Ternären Ausdruck/Verzweigung vereinheitlichen und lesbarer machen. | Niedrig |
| AZ190QIVSTzC4JOHOn-k | Issue | cpp:S995 | MINOR | test/common/json.h | 242 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Ternären Ausdruck/Verzweigung vereinheitlichen und lesbarer machen. | Niedrig |
| AZ190QIVSTzC4JOHOn-l | Issue | cpp:S995 | MINOR | test/common/json.h | 252 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Ternären Ausdruck/Verzweigung vereinheitlichen und lesbarer machen. | Niedrig |
| AZ190QIVSTzC4JOHOn-m | Issue | cpp:S995 | MINOR | test/common/json.h | 262 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Ternären Ausdruck/Verzweigung vereinheitlichen und lesbarer machen. | Niedrig |
| AZ190QIVSTzC4JOHOn-n | Issue | cpp:S995 | MINOR | test/common/json.h | 269 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Ternären Ausdruck/Verzweigung vereinheitlichen und lesbarer machen. | Niedrig |
| AZ190QIVSTzC4JOHOn-o | Issue | cpp:S995 | MINOR | test/common/json.h | 279 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Ternären Ausdruck/Verzweigung vereinheitlichen und lesbarer machen. | Niedrig |
| AZ190QIVSTzC4JOHOn-p | Issue | cpp:S1181 | MAJOR | test/common/json.h | 309 | Catch a more specific exception instead of a generic one. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Niedrig |
| AZ190QIVSTzC4JOHOn-q | Issue | cpp:S1181 | MAJOR | test/common/json.h | 321 | Catch a more specific exception instead of a generic one. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Niedrig |
| AZ190QIVSTzC4JOHOn-r | Issue | cpp:S1181 | MAJOR | test/common/json.h | 333 | Catch a more specific exception instead of a generic one. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Niedrig |
| AZ190QI2STzC4JOHOn-s | Issue | cpp:S134 | CRITICAL | test/common/modsecurity_test.cc | 89 | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements. | Verschachtelung reduzieren (Guard Clauses / frühe continue-return). | Mittel |
| AZ190QKvSTzC4JOHOn-4 | Issue | cpp:S5817 | MAJOR | test/regression/regression_test.cc | 431 | This function should be declared "const". | Pointer/Array-Operation vor Nutzung explizit auf Grenzen prüfen. | Mittel |
| AZ190QIESTzC4JOHOn-d | Issue | cpp:S886 | MINOR | src/modsecurity.cc | 232 | Refactor this loop so that it is less error-prone. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Niedrig |
| AZ190QIESTzC4JOHOn-e | Issue | cpp:S886 | MINOR | src/modsecurity.cc | 288 | Refactor this loop so that it is less error-prone. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Niedrig |

## 6) Summenübersicht
- Gesamtanzahl aller Befunde (Issues + Hotspots): **213**.
- Gesamtanzahl Issues: **213**.
- Anzahl offene Befunde (OPEN/TO_REVIEW/CONFIRMED/REOPENED): **42**.
- Anzahl geschlossene Befunde (CLOSED/RESOLVED/FIXED): **171**.
- Security Hotspots offen (TO_REVIEW): **0**.
- Security Hotspots reviewed: **0**.
- Severity-Verteilung (Issues):
  - BLOCKER: **0**
  - CRITICAL: **29**
  - MAJOR: **106**
  - MINOR: **77**
  - INFO: **1**
- Anzahl pro Regelcode (Issues):
  - cpp:S112: **31**
  - cpp:S6004: **31**
  - shelldre:S7688: **22**
  - cpp:S5812: **13**
  - cpp:S995: **13**
  - cpp:S134: **9**
  - cpp:S3776: **9**
  - cpp:S5415: **8**
  - cpp:S6009: **7**
  - cpp:S4962: **6**
  - cpp:S1121: **5**
  - cpp:S3628: **5**
  - cpp:S1181: **4**
  - cpp:S5945: **4**
  - cpp:S6022: **4**
  - cpp:S1172: **3**
  - cpp:S3230: **3**
  - cpp:S886: **3**
  - shelldre:S7682: **3**
  - cpp:S1117: **2**
  - cpp:S1155: **2**
  - cpp:S2807: **2**
  - cpp:S4144: **2**
  - cpp:S4998: **2**
  - cpp:S5274: **2**
  - cpp:S5421: **2**
  - cpp:S5817: **2**
  - cpp:S7121: **2**
  - cpp:S1135: **1**
  - cpp:S1188: **1**
  - cpp:S1481: **1**
  - cpp:S3562: **1**
  - cpp:S3624: **1**
  - cpp:S5025: **1**
  - cpp:S5827: **1**
  - cpp:S5952: **1**
  - cpp:S6018: **1**
  - cpp:S6024: **1**
  - cpp:S7127: **1**
  - cpp:S836: **1**

## 7) JSON-Backend Analyse (fokussiert auf offene Probleme)

### 7.1 Tabelle: Offene Befunde in JSON-Dateien

| Datei | Anzahl offener Befunde | Regelcodes | Problem | Minimaler Fix (konkret) |
|---|---:|---|---|---|
| src/request_body_processor/json.cc | 2 | cpp:S3230, cpp:S6009 | Do not use the constructor's initializer list for data member "m_data". Use the in-class initializer instead.; Replace this const reference to "std::string" by a "std::string_view". | Signatur/Überladung vereinheitlichen bzw. const-Korrektheit herstellen.; Grenzfallprüfung ergänzen (z. B. leere Eingaben, Null-/Range-Checks). |
| src/request_body_processor/json.h | 0 | – | Kein offener Sonar-Befund im aktuellen PR-Scan. | – |
| src/request_body_processor/json_adapter.cc | 2 | cpp:S6024, cpp:S995 | Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *".; Prefer free functions over member functions when handling objects of generic type "InputType". | Unnötige Kopie/Move vermeiden (Referenz/const& bevorzugen).; Ternären Ausdruck/Verzweigung vereinheitlichen und lesbarer machen. |
| src/request_body_processor/json_adapter.h | 0 | – | Kein offener Sonar-Befund im aktuellen PR-Scan. | – |
| src/request_body_processor/json_backend.h | 0 | – | Kein offener Sonar-Befund im aktuellen PR-Scan. | – |
| src/request_body_processor/json_backend_jsoncons.cc | 7 | cpp:S134, cpp:S3562, cpp:S3776, cpp:S6004 | 4 enumeration values not handled in switch: 'int64_value', 'uint64_value', 'half_value'...; Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements.; Refactor this function to reduce its Cognitive Complexity from 33 to the 25 allowed.; Refactor this function to reduce its Cognitive Complexity from 41 to the 25 allowed.; Refactor this function to reduce its Cognitive Complexity from 43 to the 25 allowed.; Use the init-statement to declare "sync_detail" inside the if statement. | Verschachtelung reduzieren (Guard Clauses / frühe continue-return).; String/Token-Synchronisation in dedizierte Utility-Funktion extrahieren.; Kognitive Komplexität reduzieren: frühe Returns und Teilfunktionen pro Event-Typ.; Funktion in kleinere, klar abgegrenzte Helfer zerlegen und Zweiglogik auslagern. |
| src/request_body_processor/json_backend_simdjson.cc | 1 | cpp:S5817 | This function should be declared "const". | Pointer/Array-Operation vor Nutzung explizit auf Grenzen prüfen. |
| src/request_body_processor/json_instrumentation.cc | 1 | cpp:S6018 | Use inline variables to define this global variable. | Statische Initialisierung vereinfachen oder klar kapseln, um Nebenwirkungen zu vermeiden. |
| src/request_body_processor/json_instrumentation.h | 0 | – | Kein offener Sonar-Befund im aktuellen PR-Scan. | – |

### 7.2 Architektur-Prüfung (Code-Fakten)
- Build-Time-Trennung: `configure.ac` erzwingt `--with-json-backend=simdjson|jsoncons` und setzt genau ein Compile-Define (`MSC_JSON_BACKEND_SIMDJSON` oder `MSC_JSON_BACKEND_JSONCONS`).
- Gleichzeitige Kompilierung beider Backends: `src/Makefile.am` nutzt exklusive Automake-Conditionals `JSON_BACKEND_SIMDJSON` bzw. `JSON_BACKEND_JSONCONS`; dadurch wird jeweils nur eine Backend-Implementierung in `BODY_PROCESSORS` aufgenommen.
- Gemeinsames Interface: `json_backend.h` definiert `JsonEventSink`, `JsonParseResult` und Parse-Entry-Points als gemeinsame Abstraktion.
- Adapter-Kopplung: `json_adapter.cc` dispatcht per Compile-Time-`#if defined(...)` auf genau ein Backend.
- Duplikation: Beide Backend-Dateien enthalten eigene Error-Mapping-/Traversal-Logik; gemeinsame Hilfsabstraktion ist in den geöffneten Ziel-Dateien begrenzt sichtbar.
- Gemeinsame Helper-Funktionen: In den Ziel-Dateien hauptsächlich via gemeinsames API (`JsonEventSink`, `JsonBackendParseOptions`), kein runtime-polymorpher Backend-Switch.
- **Bewertung: teilweise getrennt** (saubere Build-Selektion + gemeinsames Interface, aber weiterhin signifikante backend-spezifische Parallel-Logik).

## 8) Vergleich zu früheren Ständen
- Kein belastbarer Vergleich möglich.

## 9) Konkrete Fixes (nur offene Probleme)

| Regelcode | Betroffene Dateien (offen) | Problemtyp | Minimaler Fix | Erwarteter Sonar-Effekt | Risiko |
|---|---|---|---|---|---|
| cpp:S995 | src/request_body_processor/json_adapter.cc; test/common/json.h | Wiederkehrender Sonar-Verstoß in offenem Status. | Ternären Ausdruck/Verzweigung vereinheitlichen und lesbarer machen. | Reduktion offener Issues für cpp:S995 nach Re-Scan. | Niedrig |
| cpp:S1181 | test/common/json.h | Wiederkehrender Sonar-Verstoß in offenem Status. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Reduktion offener Issues für cpp:S1181 nach Re-Scan. | Niedrig |
| cpp:S6009 | src/request_body_processor/json.cc; test/benchmark/json_benchmark.cc | Wiederkehrender Sonar-Verstoß in offenem Status. | Grenzfallprüfung ergänzen (z. B. leere Eingaben, Null-/Range-Checks). | Reduktion offener Issues für cpp:S6009 nach Re-Scan. | Mittel |
| cpp:S6022 | src/operators/validate_byte_range.cc; src/utils/json_writer.cc | Wiederkehrender Sonar-Verstoß in offenem Status. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Reduktion offener Issues für cpp:S6022 nach Re-Scan. | Niedrig |
| cpp:S134 | src/request_body_processor/json_backend_jsoncons.cc; test/common/modsecurity_test.cc | Wiederkehrender Sonar-Verstoß in offenem Status. | Verschachtelung reduzieren (Guard Clauses / frühe continue-return). | Reduktion offener Issues für cpp:S134 nach Re-Scan. | Mittel |
| cpp:S3776 | src/request_body_processor/json_backend_jsoncons.cc | Wiederkehrender Sonar-Verstoß in offenem Status. | Kognitive Komplexität reduzieren: frühe Returns und Teilfunktionen pro Event-Typ. | Reduktion offener Issues für cpp:S3776 nach Re-Scan. | Mittel |
| cpp:S2807 | test/common/json.h | Wiederkehrender Sonar-Verstoß in offenem Status. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Reduktion offener Issues für cpp:S2807 nach Re-Scan. | Niedrig |
| cpp:S5817 | src/request_body_processor/json_backend_simdjson.cc; test/regression/regression_test.cc | Wiederkehrender Sonar-Verstoß in offenem Status. | Pointer/Array-Operation vor Nutzung explizit auf Grenzen prüfen. | Reduktion offener Issues für cpp:S5817 nach Re-Scan. | Mittel |
| cpp:S6004 | src/request_body_processor/json_backend_jsoncons.cc; test/benchmark/json_benchmark.cc | Wiederkehrender Sonar-Verstoß in offenem Status. | Funktion in kleinere, klar abgegrenzte Helfer zerlegen und Zweiglogik auslagern. | Reduktion offener Issues für cpp:S6004 nach Re-Scan. | Mittel |
| cpp:S886 | src/modsecurity.cc | Wiederkehrender Sonar-Verstoß in offenem Status. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Reduktion offener Issues für cpp:S886 nach Re-Scan. | Niedrig |
| cpp:S1188 | test/regression/regression_test.cc | Wiederkehrender Sonar-Verstoß in offenem Status. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Reduktion offener Issues für cpp:S1188 nach Re-Scan. | Niedrig |
| cpp:S3230 | src/request_body_processor/json.cc | Wiederkehrender Sonar-Verstoß in offenem Status. | Signatur/Überladung vereinheitlichen bzw. const-Korrektheit herstellen. | Reduktion offener Issues für cpp:S3230 nach Re-Scan. | Niedrig |
| cpp:S3562 | src/request_body_processor/json_backend_jsoncons.cc | Wiederkehrender Sonar-Verstoß in offenem Status. | String/Token-Synchronisation in dedizierte Utility-Funktion extrahieren. | Reduktion offener Issues für cpp:S3562 nach Re-Scan. | Mittel |
| cpp:S4144 | test/unit/json_backend_depth_tests.cc | Wiederkehrender Sonar-Verstoß in offenem Status. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Reduktion offener Issues für cpp:S4144 nach Re-Scan. | Niedrig |
| cpp:S4998 | test/benchmark/json_benchmark.cc | Wiederkehrender Sonar-Verstoß in offenem Status. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Reduktion offener Issues für cpp:S4998 nach Re-Scan. | Niedrig |
| cpp:S5945 | src/utils/json_writer.cc | Wiederkehrender Sonar-Verstoß in offenem Status. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Reduktion offener Issues für cpp:S5945 nach Re-Scan. | Niedrig |
| cpp:S5952 | test/benchmark/json_benchmark.cc | Wiederkehrender Sonar-Verstoß in offenem Status. | Regel-spezifischen Refactor gemäß Sonar-Regeltext umsetzen. | Reduktion offener Issues für cpp:S5952 nach Re-Scan. | Niedrig |
| cpp:S6018 | src/request_body_processor/json_instrumentation.cc | Wiederkehrender Sonar-Verstoß in offenem Status. | Statische Initialisierung vereinfachen oder klar kapseln, um Nebenwirkungen zu vermeiden. | Reduktion offener Issues für cpp:S6018 nach Re-Scan. | Niedrig |
| cpp:S6024 | src/request_body_processor/json_adapter.cc | Wiederkehrender Sonar-Verstoß in offenem Status. | Unnötige Kopie/Move vermeiden (Referenz/const& bevorzugen). | Reduktion offener Issues für cpp:S6024 nach Re-Scan. | Niedrig |

## 10) Grenzen der Analyse
- Issues konnten vollständig über alle Seiten (`total` und Paginierung) abgerufen werden.
- Ergebnis: **vollständige Sonar-Liste** (für den öffentlich abrufbaren PR-Scan).
- Security Hotspots API lieferte 0 Einträge; daher keine Hotspot-Detailbewertung möglich.
- Historischer Vorher/Nachher-Vergleich wurde nicht belastbar aus aktuellen Quellen ableitbar.

## Quellen (direkt abrufbar)
- GitHub PR API: https://api.github.com/repos/owasp-modsecurity/ModSecurity/pulls/3540
- GitHub PR Commits API: https://api.github.com/repos/owasp-modsecurity/ModSecurity/pulls/3540/commits?per_page=100
- Sonar PR Liste: https://sonarcloud.io/api/project_pull_requests/list?project=owasp-modsecurity_ModSecurity
- Sonar Quality Gate: https://sonarcloud.io/api/qualitygates/project_status?projectKey=owasp-modsecurity_ModSecurity&pullRequest=3540
- Sonar Measures: https://sonarcloud.io/api/measures/component?component=owasp-modsecurity_ModSecurity&pullRequest=3540&metricKeys=alert_status,bugs,vulnerabilities,code_smells,security_hotspots,new_bugs,new_vulnerabilities,new_code_smells,new_security_hotspots,coverage,new_coverage,duplicated_lines_density,new_duplicated_lines_density,ncloc,new_lines,sqale_index,new_maintainability_rating,new_reliability_rating,new_security_rating,new_security_hotspots_reviewed
- Sonar Issues: https://sonarcloud.io/api/issues/search?componentKeys=owasp-modsecurity_ModSecurity&pullRequest=3540&ps=100&p=1&additionalFields=_all (paginiert)
- Sonar Hotspots: https://sonarcloud.io/api/hotspots/search?projectKey=owasp-modsecurity_ModSecurity&pullRequest=3540&ps=100&p=1
