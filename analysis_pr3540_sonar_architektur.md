# PR 3540 – Sonar + Architektur Neu-Analyse (aktueller Stand)

- Analysezeitpunkt (UTC): 2026-04-14
- PR: https://github.com/owasp-modsecurity/ModSecurity/pull/3540
- Head-Commit laut GitHub API: `7c4f24ace3a9ed6e60e1211da78f4894931e9ab0`
- PR zuletzt aktualisiert: 2026-04-14T18:41:07Z
- Sonar-PR-Key: 3540

## 1) Commit-Stand
**Analyse basiert auf Commit: `7c4f24ace3a9ed6e60e1211da78f4894931e9ab0`**

Zusatzprüfung: Die Commit-Liste des PR endet ebenfalls bei diesem SHA; damit wurde kein neuerer PR-Commit gegenüber dem ermittelten Head gefunden.

## 2) Sonar-Datenabruf (neu geladen)
- Quality Gate API abrufbar: Ja
- Measures API abrufbar: Ja
- Security Hotspots API abrufbar: Ja (Anzahl: 0)
- Issues API abrufbar: Ja (Anzahl laut `total`: 214, geladen: 214)

## 3) Quality Gate Analyse
- Gate-Status: **OK**

| Metric | Comparator | Threshold | Actual | Condition Status |
|---|---:|---:|---:|---|
| new_reliability_rating | GT | 1 | 1 | OK |
| new_security_rating | GT | 1 | 1 | OK |
| new_maintainability_rating | GT | 1 | 1 | OK |
| new_duplicated_lines_density | GT | 3 | 0.0 | OK |
| new_security_hotspots_reviewed | LT | 100 | 100.0 | OK |

Keine fehlschlagende Gate-Bedingung im aktuellen Stand.

## 4) Tabelle: Alle Sonar-Befunde (vollständig enumeriert)

| ID | Kategorie | Regelcode | Severity | Status | Datei | Zeile | Nachricht | Codebereich | Direkt behebbar |
|---|---|---|---|---|---|---:|---|---|---|
| AZ2NTAUK-fxIQtZsdoYq | Issue | cpp:S134 | CRITICAL | OPEN | src/request_body_processor/xml.cc | 314 | Refactor this code to not nest more than 3 if|for|do|while|switch statements. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2H7l4Bym_e-6l8FQml | Issue | cpp:S6009 | MINOR | OPEN | src/request_body_processor/json.cc | 42 | Replace this const reference to "std::string" by a "std::string_view". | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2H7l3vym_e-6l8FQmj | Issue | cpp:S6024 | MINOR | OPEN | src/request_body_processor/json_adapter.cc | 56 | Prefer free functions over member functions when handling objects of generic type "InputType". | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2H7l3vym_e-6l8FQmk | Issue | cpp:S995 | MINOR | OPEN | src/request_body_processor/json_adapter.cc | 57 | Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *". | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2H7l3jym_e-6l8FQmi | Issue | cpp:S6004 | MINOR | OPEN | src/request_body_processor/json_backend_jsoncons.cc | 568 | Use the init-statement to declare "sync_detail" inside the if statement. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2H7l3jym_e-6l8FQmh | Issue | cpp:S1172 | MAJOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Remove the unused parameter "sink", make it unnamed, or declare it "[[maybe_unused]]". | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2H7l0Nym_e-6l8FQmg | Issue | cpp:S6018 | MAJOR | CLOSED | src/request_body_processor/json_instrumentation.cc |  | Use inline variables to define this global variable. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2HqeBsWym3B0O6okyS | Issue | cpp:S5952 | MINOR | OPEN | test/benchmark/json_benchmark.cc | 71 | Add a using-declaration to this derived class to inherit the constructors of "runtime_error", and remove the ones you manually duplicated. Note that this may add other constructors to your derived class. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2HqeBsWym3B0O6okyT | Issue | cpp:S6004 | MINOR | OPEN | test/benchmark/json_benchmark.cc | 144 | Use the init-statement to declare "current" inside the if statement. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2DWE24t-zbsGOGdN-_ | Issue | cpp:S3776 | CRITICAL | CLOSED | test/benchmark/json_benchmark.cc |  | Refactor this function to reduce its Cognitive Complexity from 33 to the 25 allowed. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2DWE24t-zbsGOGdN_A | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2DWE24t-zbsGOGdN_B | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2DWE24t-zbsGOGdN_C | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2DWE24t-zbsGOGdN_D | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2DWE24t-zbsGOGdN_E | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2DWE24t-zbsGOGdN_F | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2DWE24t-zbsGOGdN_G | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2DWE24t-zbsGOGdN_H | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2DWE24t-zbsGOGdN_I | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2DWE24t-zbsGOGdN_J | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2DWE24t-zbsGOGdN_K | Issue | cpp:S4998 | MAJOR | OPEN | test/benchmark/json_benchmark.cc | 316 | Replace this use of "unique_ptr" by a raw pointer or a reference (possibly const). | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2DWE24t-zbsGOGdN_M | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2DWE24t-zbsGOGdN_N | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2DWE24t-zbsGOGdN_O | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2DWE24t-zbsGOGdN_P | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2DVDgODPiZK5yPV1-J | Issue | cpp:S1188 | MAJOR | OPEN | test/regression/regression_test.cc | 235 | This lambda has 23 lines, which is greater than the 20 lines authorized. Split it into several lambdas or functions, or make it a named function. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2DR4Fykud7vHWq_QVC | Issue | cpp:S6009 | MINOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Replace this const reference to "std::string" by a "std::string_view". | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2DR4Fykud7vHWq_QVD | Issue | cpp:S995 | MINOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *". | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2DR4Fykud7vHWq_QVE | Issue | cpp:S995 | MINOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Make the type of this parameter a reference-to-const. The current type of "input" is "std::string &". | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2DK9KwXISY38E6wMPS | Issue | cpp:S995 | MINOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *". | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2C_aavSTzC4JOHsQM1 | Issue | cpp:S1121 | MAJOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Extract the assignment from this expression. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2CwcldK0fgB4uOpVKy | Issue | cpp:S995 | MINOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Make the type of this parameter a reference-to-const. The current type of "input" is "std::string &". | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2CwcldK0fgB4uOpVK0 | Issue | cpp:S995 | MINOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *". | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2CwcldK0fgB4uOpVKz | Issue | cpp:S1172 | MAJOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Remove the unused parameter "options", make it unnamed, or declare it "[[maybe_unused]]". | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2CwcnoK0fgB4uOpVK1 | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "tail" inside the if statement. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2CwcnoK0fgB4uOpVK2 | Issue | cpp:S1117 | MAJOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Declaration shadows a local variable "result" in the outer scope. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2CwcnoK0fgB4uOpVK3 | Issue | cpp:S1117 | MAJOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Declaration shadows a local variable "result" in the outer scope. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2CwcnoK0fgB4uOpVK4 | Issue | cpp:S5817 | MAJOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | This function should be declared "const". | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2CwcpOK0fgB4uOpVK5 | Issue | cpp:S6004 | MINOR | CLOSED | test/unit/json_backend_depth_tests.cc |  | Use the init-statement to declare "result" inside the if statement. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2CwcpOK0fgB4uOpVK6 | Issue | cpp:S6004 | MINOR | CLOSED | test/unit/json_backend_depth_tests.cc |  | Use the init-statement to declare "result" inside the if statement. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2CdKxRGCkM6OziHCww | Issue | cpp:S6004 | MINOR | CLOSED | test/unit/json_backend_depth_tests.cc |  | Use the init-statement to declare "result" inside the if statement. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2CdKxRGCkM6OziHCwx | Issue | cpp:S5945 | MAJOR | CLOSED | test/unit/json_backend_depth_tests.cc |  | Use "std::array" or "std::vector" instead of a C-style array. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2CdKxRGCkM6OziHCwy | Issue | cpp:S3628 | MINOR | CLOSED | test/unit/json_backend_depth_tests.cc |  | Convert this string literal to a raw string literal. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2CBI6Kkud7vHWq0tqj | Issue | cpp:S6022 | MAJOR | OPEN | src/operators/validate_byte_range.cc | 157 | Use "std::byte" for byte-oriented data manipulation. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2CA_0CGCkM6OziEPex | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2CA_xuGCkM6OziEPet | Issue | cpp:S5812 | MINOR | CLOSED | test/unit/json_backend_depth_tests.cc |  | Concatenate this namespace with the nested one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2CA_xuGCkM6OziEPeu | Issue | cpp:S4144 | MAJOR | OPEN | test/unit/json_backend_depth_tests.cc | 50 | Update this method so that its implementation is not identical to on_key. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2CA_xuGCkM6OziEPev | Issue | cpp:S6004 | MINOR | CLOSED | test/unit/json_backend_depth_tests.cc |  | Use the init-statement to declare "result" inside the if statement. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2CA_xuGCkM6OziEPew | Issue | cpp:S6004 | MINOR | CLOSED | test/unit/json_backend_depth_tests.cc |  | Use the init-statement to declare "result" inside the if statement. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2BthMEO-njQfcv_7WG | Issue | cpp:S3776 | CRITICAL | OPEN | src/request_body_processor/json_backend_jsoncons.cc | 348 | Refactor this function to reduce its Cognitive Complexity from 41 to the 25 allowed. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2BthMEO-njQfcv_7WH | Issue | cpp:S1121 | MAJOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Extract the assignment from this expression. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2BthMEO-njQfcv_7WI | Issue | cpp:S1121 | MAJOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Extract the assignment from this expression. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2BthMEO-njQfcv_7WJ | Issue | cpp:S3776 | CRITICAL | OPEN | src/request_body_processor/json_backend_jsoncons.cc | 412 | Refactor this function to reduce its Cognitive Complexity from 43 to the 25 allowed. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2BthMEO-njQfcv_7WK | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Use the init-statement to declare "decoded_number" inside the if statement. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2BthMEO-njQfcv_7WL | Issue | cpp:S134 | CRITICAL | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Refactor this code to not nest more than 3 if|for|do|while|switch statements. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ2BkKY5XISY38E6k-Ld | Issue | cpp:S1135 | INFO | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Complete the task associated to this "TODO" comment. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-dn-nXISY38E6Txop | Issue | cpp:S5025 | CRITICAL | CLOSED | src/request_body_processor/json.cc |  | Rewrite the code so that you no longer need this "delete". | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-dn-nXISY38E6Txoq | Issue | cpp:S5827 | MAJOR | CLOSED | src/request_body_processor/json.cc |  | Replace the redundant type with "auto". | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-dn9hXISY38E6Txon | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json_instrumentation.cc |  | Concatenate this namespace with the nested one. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-dn9hXISY38E6Txoo | Issue | cpp:S5421 | CRITICAL | CLOSED | src/request_body_processor/json_instrumentation.cc |  | Global variables should be const. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-dn9QXISY38E6Txom | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json_instrumentation.h |  | Concatenate this namespace with the nested one. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-dn4cXISY38E6Txoh | Issue | cpp:S4962 | CRITICAL | CLOSED | src/utils/msc_tree.cc |  | Use the "nullptr" literal. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6Txoz | Issue | cpp:S5421 | CRITICAL | CLOSED | test/benchmark/json_benchmark.cc |  | Global pointers should be const at every level. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6Txo0 | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6Txo1 | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6Txo6 | Issue | cpp:S886 | MINOR | CLOSED | test/benchmark/json_benchmark.cc |  | Refactor this loop so that it is less error-prone. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6Txo3 | Issue | cpp:S6004 | MINOR | CLOSED | test/benchmark/json_benchmark.cc |  | Use the init-statement to declare "output_format" inside the if statement. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6Txo4 | Issue | cpp:S6004 | MINOR | CLOSED | test/benchmark/json_benchmark.cc |  | Use the init-statement to declare "is_invalid_scenario" inside the if statement. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6TxpE | Issue | cpp:S5945 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Use "std::array" or "std::vector" instead of a C-style array. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6TxpF | Issue | cpp:S5945 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Use "std::array" or "std::vector" instead of a C-style array. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6TxpG | Issue | cpp:S7127 | CRITICAL | CLOSED | test/benchmark/json_benchmark.cc |  | Use "std::size" to get the size of this array. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6TxpK | Issue | cpp:S6009 | MINOR | OPEN | test/benchmark/json_benchmark.cc | 321 | Replace this const reference to "std::string" by a "std::string_view". | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6TxpM | Issue | cpp:S7121 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Remove this redundant call to "c_str" when initializing a const "std::string" reference parameter. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6TxpJ | Issue | cpp:S6004 | MINOR | CLOSED | test/benchmark/json_benchmark.cc |  | Use the init-statement to declare "parse_error" inside the if statement. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6TxpS | Issue | cpp:S3628 | MINOR | CLOSED | test/benchmark/json_benchmark.cc |  | Convert this string literal to a raw string literal. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6TxpT | Issue | cpp:S3628 | MINOR | CLOSED | test/benchmark/json_benchmark.cc |  | Convert this string literal to a raw string literal. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6TxpR | Issue | cpp:S6009 | MINOR | OPEN | test/benchmark/json_benchmark.cc | 419 | Replace this const reference to "std::string" by a "std::string_view". | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6TxpV | Issue | cpp:S6004 | MINOR | CLOSED | test/benchmark/json_benchmark.cc |  | Use the init-statement to declare "rules_path" inside the if statement. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6TxpU | Issue | cpp:S6009 | MINOR | OPEN | test/benchmark/json_benchmark.cc | 486 | Replace this const reference to "std::string" by a "std::string_view". | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6Txo2 | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6Txo5 | Issue | cpp:S3776 | CRITICAL | CLOSED | test/benchmark/json_benchmark.cc |  | Refactor this function to reduce its Cognitive Complexity from 33 to the 25 allowed. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6Txo7 | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6Txo8 | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6Txo9 | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6Txo- | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6Txo_ | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6TxpA | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6TxpB | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6TxpC | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6TxpD | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6TxpH | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6TxpI | Issue | cpp:S4998 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Replace this use of "unique_ptr" by a raw pointer or a reference (possibly const). | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6TxpN | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6TxpO | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6TxpP | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCWXISY38E6TxpQ | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCHXISY38E6Txor | Issue | shelldre:S7682 | MAJOR | CLOSED | test/benchmark/run-json-benchmarks.sh |  | Add an explicit return statement at the end of the function. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCHXISY38E6Txos | Issue | shelldre:S7688 | MAJOR | CLOSED | test/benchmark/run-json-benchmarks.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCHXISY38E6Txot | Issue | shelldre:S7688 | MAJOR | CLOSED | test/benchmark/run-json-benchmarks.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCHXISY38E6Txou | Issue | shelldre:S7688 | MAJOR | CLOSED | test/benchmark/run-json-benchmarks.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCHXISY38E6Txov | Issue | shelldre:S7688 | MAJOR | CLOSED | test/benchmark/run-json-benchmarks.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCHXISY38E6Txow | Issue | shelldre:S7688 | MAJOR | CLOSED | test/benchmark/run-json-benchmarks.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCHXISY38E6Txox | Issue | shelldre:S7688 | MAJOR | CLOSED | test/benchmark/run-json-benchmarks.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doCHXISY38E6Txoy | Issue | shelldre:S7688 | MAJOR | CLOSED | test/benchmark/run-json-benchmarks.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doDkXISY38E6TxpW | Issue | shelldre:S7682 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Add an explicit return statement at the end of the function. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doDkXISY38E6TxpX | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doDkXISY38E6TxpY | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doDkXISY38E6TxpZ | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doDkXISY38E6Txpa | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doDkXISY38E6Txpb | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doDkXISY38E6Txpc | Issue | shelldre:S7682 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Add an explicit return statement at the end of the function. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doDkXISY38E6Txpd | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doDkXISY38E6Txpe | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doDkXISY38E6Txpf | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doDkXISY38E6Txpg | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doDkXISY38E6Txph | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doDkXISY38E6Txpi | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doDkXISY38E6Txpj | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doDkXISY38E6Txpk | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-doDkXISY38E6Txpl | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QEMSTzC4JOHOn9q | Issue | cpp:S6022 | MAJOR | OPEN | src/operators/validate_byte_range.cc | 73 | Use "std::byte" for byte-oriented data manipulation. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QEMSTzC4JOHOn9r | Issue | cpp:S6004 | MINOR | CLOSED | src/operators/validate_byte_range.cc |  | Use the init-statement to declare "token" inside the if statement. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGTSTzC4JOHOn-H | Issue | cpp:S3776 | CRITICAL | CLOSED | src/request_body_processor/json.cc |  | Refactor this function to reduce its Cognitive Complexity from 37 to the 25 allowed. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGTSTzC4JOHOn-D | Issue | cpp:S3230 | MAJOR | CLOSED | src/request_body_processor/json.cc |  | Do not use the constructor's initializer list for data member "m_data". Use the in-class initializer instead. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGTSTzC4JOHOn-G | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json.cc |  | Use the init-statement to declare "result" inside the if statement. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGTSTzC4JOHOn-I | Issue | cpp:S134 | CRITICAL | CLOSED | src/request_body_processor/json.cc |  | Refactor this code to not nest more than 3 if|for|do|while|switch statements. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGTSTzC4JOHOn-J | Issue | cpp:S134 | CRITICAL | CLOSED | src/request_body_processor/json.cc |  | Refactor this code to not nest more than 3 if|for|do|while|switch statements. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGTSTzC4JOHOn-K | Issue | cpp:S134 | CRITICAL | CLOSED | src/request_body_processor/json.cc |  | Refactor this code to not nest more than 3 if|for|do|while|switch statements. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGTSTzC4JOHOn-L | Issue | cpp:S134 | CRITICAL | CLOSED | src/request_body_processor/json.cc |  | Refactor this code to not nest more than 3 if|for|do|while|switch statements. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGTSTzC4JOHOn-M | Issue | cpp:S134 | CRITICAL | CLOSED | src/request_body_processor/json.cc |  | Refactor this code to not nest more than 3 if|for|do|while|switch statements. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGTSTzC4JOHOn-E | Issue | cpp:S4144 | MAJOR | CLOSED | src/request_body_processor/json.cc |  | Update this method so that its implementation is not identical to on_end_object. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGTSTzC4JOHOn-N | Issue | cpp:S1155 | MINOR | CLOSED | src/request_body_processor/json.cc |  | Use "empty()" to check whether the container is empty or not. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGTSTzC4JOHOn-O | Issue | cpp:S1155 | MINOR | CLOSED | src/request_body_processor/json.cc |  | Use "empty()" to check whether the container is empty or not. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QEtSTzC4JOHOn9t | Issue | cpp:S3624 | CRITICAL | CLOSED | src/request_body_processor/json.h |  | Customize this class' copy constructor to participate in resource management. Customize or delete its copy assignment operator. Also consider whether move operations should be customized. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QF1STzC4JOHOn9- | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Concatenate this namespace with the nested one. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QF1STzC4JOHOn-A | Issue | cpp:S1172 | MAJOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Remove the unused parameter "options", make it unnamed, or declare it "[[maybe_unused]]". | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QF1STzC4JOHOn-B | Issue | cpp:S995 | MINOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *". | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QF1STzC4JOHOn9_ | Issue | cpp:S6009 | MINOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Replace this const reference to "std::string" by a "std::string_view". | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QF8STzC4JOHOn-C | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json_adapter.h |  | Concatenate this namespace with the nested one. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGbSTzC4JOHOn-P | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json_backend.h |  | Concatenate this namespace with the nested one. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QFsSTzC4JOHOn9v | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Concatenate this namespace with the nested one. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QFsSTzC4JOHOn9w | Issue | cpp:S3776 | CRITICAL | OPEN | src/request_body_processor/json_backend_jsoncons.cc | 92 | Refactor this function to reduce its Cognitive Complexity from 33 to the 25 allowed. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QFsSTzC4JOHOn9u | Issue | cpp:S3562 | MAJOR | OPEN | src/request_body_processor/json_backend_jsoncons.cc | 188 | 4 enumeration values not handled in switch: 'int64_value', 'uint64_value', 'half_value'... | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QFsSTzC4JOHOn9z | Issue | cpp:S3776 | CRITICAL | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Refactor this function to reduce its Cognitive Complexity from 41 to the 25 allowed. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QFsSTzC4JOHOn92 | Issue | cpp:S1121 | MAJOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Extract the assignment from this expression. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QFsSTzC4JOHOn9x | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Use the init-statement to declare "current" inside the if statement. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QFsSTzC4JOHOn93 | Issue | cpp:S1121 | MAJOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Extract the assignment from this expression. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QFsSTzC4JOHOn94 | Issue | cpp:S3776 | CRITICAL | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Refactor this function to reduce its Cognitive Complexity from 43 to the 25 allowed. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QFsSTzC4JOHOn90 | Issue | cpp:S134 | CRITICAL | OPEN | src/request_body_processor/json_backend_jsoncons.cc | 365 | Refactor this code to not nest more than 3 if|for|do|while|switch statements. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QFsSTzC4JOHOn91 | Issue | cpp:S134 | CRITICAL | OPEN | src/request_body_processor/json_backend_jsoncons.cc | 374 | Refactor this code to not nest more than 3 if|for|do|while|switch statements. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QFsSTzC4JOHOn9y | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Use the init-statement to declare "escaped" inside the if statement. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QFsSTzC4JOHOn97 | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Use the init-statement to declare "sync_detail" inside the if statement. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QFsSTzC4JOHOn96 | Issue | cpp:S6009 | MINOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Replace this const reference to "std::string" by a "std::string_view". | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QFsSTzC4JOHOn95 | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Use the init-statement to declare "end" inside the if statement. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QFsSTzC4JOHOn98 | Issue | cpp:S3776 | CRITICAL | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Refactor this function to reduce its Cognitive Complexity from 48 to the 25 allowed. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QFsSTzC4JOHOn99 | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Use the init-statement to declare "result" inside the if statement. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGkSTzC4JOHOn-Q | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Concatenate this namespace with the nested one. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGkSTzC4JOHOn-R | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "sink_status" inside the if statement. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGkSTzC4JOHOn-S | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "sink_status" inside the if statement. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGkSTzC4JOHOn-T | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "sink_status" inside the if statement. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGkSTzC4JOHOn-U | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "sink_status" inside the if statement. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGkSTzC4JOHOn-V | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "result" inside the if statement. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGkSTzC4JOHOn-W | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "sink_status" inside the if statement. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGkSTzC4JOHOn-X | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "result" inside the if statement. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGkSTzC4JOHOn-Y | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "sink_status" inside the if statement. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGkSTzC4JOHOn-Z | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "sink_status" inside the if statement. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGkSTzC4JOHOn-a | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "result" inside the if statement. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGkSTzC4JOHOn-b | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "sink_status" inside the if statement. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QHkSTzC4JOHOn-c | Issue | cpp:S7121 | MAJOR | CLOSED | src/transaction.cc |  | Remove this redundant call to "c_str" when initializing a const "std::string" reference parameter. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QA8STzC4JOHOn9j | Issue | cpp:S5812 | MINOR | CLOSED | src/utils/json_writer.cc |  | Concatenate this namespace with the nested one. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QA8STzC4JOHOn9h | Issue | cpp:S3230 | MAJOR | CLOSED | src/utils/json_writer.cc |  | Remove this use of the constructor's initializer list for data member "m_output". It is redundant with default initialization behavior. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QA8STzC4JOHOn9i | Issue | cpp:S3230 | MAJOR | CLOSED | src/utils/json_writer.cc |  | Remove this use of the constructor's initializer list for data member "m_stack". It is redundant with default initialization behavior. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QA8STzC4JOHOn9k | Issue | cpp:S5945 | MAJOR | OPEN | src/utils/json_writer.cc | 156 | Use "std::string" instead of a C-style char array. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QA8STzC4JOHOn9l | Issue | cpp:S3628 | MINOR | CLOSED | src/utils/json_writer.cc |  | Convert this string literal to a raw string literal. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QA8STzC4JOHOn9m | Issue | cpp:S3628 | MINOR | CLOSED | src/utils/json_writer.cc |  | Convert this string literal to a raw string literal. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QA8STzC4JOHOn9n | Issue | cpp:S6022 | MAJOR | OPEN | src/utils/json_writer.cc | 184 | Use "std::byte" for byte-oriented data manipulation. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QA8STzC4JOHOn9o | Issue | cpp:S6022 | MAJOR | OPEN | src/utils/json_writer.cc | 185 | Use "std::byte" for byte-oriented data manipulation. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QD4STzC4JOHOn9p | Issue | cpp:S5812 | MINOR | CLOSED | src/utils/json_writer.h |  | Concatenate this namespace with the nested one. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QIVSTzC4JOHOn-f | Issue | cpp:S5812 | MINOR | CLOSED | test/common/json.h |  | Concatenate this namespace with the nested one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QIVSTzC4JOHOn-g | Issue | cpp:S2807 | MAJOR | OPEN | test/common/json.h | 78 | Make this member overloaded operator a hidden friend. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QIVSTzC4JOHOn-h | Issue | cpp:S2807 | MAJOR | OPEN | test/common/json.h | 123 | Make this member overloaded operator a hidden friend. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QIVSTzC4JOHOn-i | Issue | cpp:S1181 | MAJOR | OPEN | test/common/json.h | 219 | Catch a more specific exception instead of a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QIVSTzC4JOHOn-j | Issue | cpp:S995 | MINOR | OPEN | test/common/json.h | 232 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QIVSTzC4JOHOn-k | Issue | cpp:S995 | MINOR | OPEN | test/common/json.h | 242 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QIVSTzC4JOHOn-l | Issue | cpp:S995 | MINOR | OPEN | test/common/json.h | 252 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QIVSTzC4JOHOn-m | Issue | cpp:S995 | MINOR | OPEN | test/common/json.h | 262 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QIVSTzC4JOHOn-n | Issue | cpp:S995 | MINOR | OPEN | test/common/json.h | 269 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QIVSTzC4JOHOn-o | Issue | cpp:S995 | MINOR | OPEN | test/common/json.h | 279 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QIVSTzC4JOHOn-p | Issue | cpp:S1181 | MAJOR | OPEN | test/common/json.h | 309 | Catch a more specific exception instead of a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QIVSTzC4JOHOn-q | Issue | cpp:S1181 | MAJOR | OPEN | test/common/json.h | 321 | Catch a more specific exception instead of a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QIVSTzC4JOHOn-r | Issue | cpp:S1181 | MAJOR | OPEN | test/common/json.h | 333 | Catch a more specific exception instead of a generic one. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QI2STzC4JOHOn-s | Issue | cpp:S134 | CRITICAL | OPEN | test/common/modsecurity_test.cc | 89 | Refactor this code to not nest more than 3 if|for|do|while|switch statements. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QKvSTzC4JOHOn-x | Issue | cpp:S5415 | MAJOR | CLOSED | test/regression/regression_test.cc |  | The result of "std::move" should not be passed as a const reference. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QKvSTzC4JOHOn-y | Issue | cpp:S5415 | MAJOR | CLOSED | test/regression/regression_test.cc |  | The result of "std::move" should not be passed as a const reference. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QKvSTzC4JOHOn-z | Issue | cpp:S5415 | MAJOR | CLOSED | test/regression/regression_test.cc |  | The result of "std::move" should not be passed as a const reference. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QKvSTzC4JOHOn-0 | Issue | cpp:S5415 | MAJOR | CLOSED | test/regression/regression_test.cc |  | The result of "std::move" should not be passed as a const reference. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QKvSTzC4JOHOn-1 | Issue | cpp:S5415 | MAJOR | CLOSED | test/regression/regression_test.cc |  | The result of "std::move" should not be passed as a const reference. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QKvSTzC4JOHOn-2 | Issue | cpp:S5415 | MAJOR | CLOSED | test/regression/regression_test.cc |  | The result of "std::move" should not be passed as a const reference. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QKvSTzC4JOHOn-3 | Issue | cpp:S5415 | MAJOR | CLOSED | test/regression/regression_test.cc |  | The result of "std::move" should not be passed as a const reference. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QKvSTzC4JOHOn-4 | Issue | cpp:S5817 | MAJOR | OPEN | test/regression/regression_test.cc | 431 | This function should be declared "const". | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QKvSTzC4JOHOn-v | Issue | cpp:S5274 | MAJOR | CLOSED | test/regression/regression_test.cc |  | moving a temporary object prevents copy elision | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QKvSTzC4JOHOn-w | Issue | cpp:S5274 | MAJOR | CLOSED | test/regression/regression_test.cc |  | moving a temporary object prevents copy elision | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QKvSTzC4JOHOn-5 | Issue | cpp:S1481 | MINOR | CLOSED | test/regression/regression_test.cc |  | Remove the unused lambda capture "writer". | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QJKSTzC4JOHOn-t | Issue | cpp:S5415 | MAJOR | CLOSED | test/unit/unit_test.cc |  | The result of "std::move" should not be passed as a const reference. | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QJYSTzC4JOHOn-u | Issue | cpp:S836 | MAJOR | CLOSED | test/unit/unit_test.h |  | Value assigned to field 'ret' in implicit constructor is garbage or undefined | Test | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QIESTzC4JOHOn-d | Issue | cpp:S886 | MINOR | OPEN | src/modsecurity.cc | 232 | Refactor this loop so that it is less error-prone. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QIESTzC4JOHOn-e | Issue | cpp:S886 | MINOR | OPEN | src/modsecurity.cc | 288 | Refactor this loop so that it is less error-prone. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QGTSTzC4JOHOn-F | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json.cc |  | Concatenate this namespace with the nested one. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ190QEtSTzC4JOHOn9s | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json.h |  | Concatenate this namespace with the nested one. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-dn4cXISY38E6Txog | Issue | cpp:S4962 | CRITICAL | CLOSED | src/utils/msc_tree.cc |  | Use the "nullptr" literal. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-dn4cXISY38E6Txoi | Issue | cpp:S4962 | CRITICAL | CLOSED | src/utils/msc_tree.cc |  | Use the "nullptr" literal. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-dn4cXISY38E6Txoj | Issue | cpp:S4962 | CRITICAL | CLOSED | src/utils/msc_tree.cc |  | Use the "nullptr" literal. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-dn4cXISY38E6Txok | Issue | cpp:S4962 | CRITICAL | CLOSED | src/utils/msc_tree.cc |  | Use the "nullptr" literal. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| AZ1-dn4cXISY38E6Txol | Issue | cpp:S4962 | CRITICAL | CLOSED | src/utils/msc_tree.cc |  | Use the "nullptr" literal. | Production | Nicht belegbar auf Basis der aktuellen Quellen |
| QG-1 | Gate Condition | new_reliability_rating | n/a | OK | n/a |  | comparator GT threshold 1 actual 1 | n/a | Nicht belegbar auf Basis der aktuellen Quellen |
| QG-2 | Gate Condition | new_security_rating | n/a | OK | n/a |  | comparator GT threshold 1 actual 1 | n/a | Nicht belegbar auf Basis der aktuellen Quellen |
| QG-3 | Gate Condition | new_maintainability_rating | n/a | OK | n/a |  | comparator GT threshold 1 actual 1 | n/a | Nicht belegbar auf Basis der aktuellen Quellen |
| QG-4 | Gate Condition | new_duplicated_lines_density | n/a | OK | n/a |  | comparator GT threshold 3 actual 0.0 | n/a | Nicht belegbar auf Basis der aktuellen Quellen |
| QG-5 | Gate Condition | new_security_hotspots_reviewed | n/a | OK | n/a |  | comparator LT threshold 100 actual 100.0 | n/a | Nicht belegbar auf Basis der aktuellen Quellen |

## 5) Tabelle: Nur offene Befunde

| ID | Kategorie | Regelcode | Severity | Datei | Zeile | Kurzbeschreibung | Konkreter technischer Fix | Risiko der Änderung |
|---|---|---|---|---|---:|---|---|---|
| AZ2NTAUK-fxIQtZsdoYq | Issue | cpp:S134 | CRITICAL | src/request_body_processor/xml.cc | 314 | Refactor this code to not nest more than 3 if|for|do|while|switch statements. | Sonar-Meldung direkt umsetzen: Refactor this code to not nest more than 3 if|for|do|while|switch statements. | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ2H7l4Bym_e-6l8FQml | Issue | cpp:S6009 | MINOR | src/request_body_processor/json.cc | 42 | Replace this const reference to "std::string" by a "std::string_view". | Sonar-Meldung direkt umsetzen: Replace this const reference to "std::string" by a "std::string_view". | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ2H7l3vym_e-6l8FQmj | Issue | cpp:S6024 | MINOR | src/request_body_processor/json_adapter.cc | 56 | Prefer free functions over member functions when handling objects of generic type "InputType". | Sonar-Meldung direkt umsetzen: Prefer free functions over member functions when handling objects of generic type "InputType". | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ2H7l3vym_e-6l8FQmk | Issue | cpp:S995 | MINOR | src/request_body_processor/json_adapter.cc | 57 | Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *". | Sonar-Meldung direkt umsetzen: Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *". | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ2H7l3jym_e-6l8FQmi | Issue | cpp:S6004 | MINOR | src/request_body_processor/json_backend_jsoncons.cc | 568 | Use the init-statement to declare "sync_detail" inside the if statement. | Sonar-Meldung direkt umsetzen: Use the init-statement to declare "sync_detail" inside the if statement. | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ2HqeBsWym3B0O6okyS | Issue | cpp:S5952 | MINOR | test/benchmark/json_benchmark.cc | 71 | Add a using-declaration to this derived class to inherit the constructors of "runtime_error", and remove the ones you manually duplicated. Note that this may add other constructors to your derived class. | Sonar-Meldung direkt umsetzen: Add a using-declaration to this derived class to inherit the constructors of "runtime_error", and remove the ones you manually duplicated. Note that this may add other constructors to your derived class. | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ2HqeBsWym3B0O6okyT | Issue | cpp:S6004 | MINOR | test/benchmark/json_benchmark.cc | 144 | Use the init-statement to declare "current" inside the if statement. | Sonar-Meldung direkt umsetzen: Use the init-statement to declare "current" inside the if statement. | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ2DWE24t-zbsGOGdN_K | Issue | cpp:S4998 | MAJOR | test/benchmark/json_benchmark.cc | 316 | Replace this use of "unique_ptr" by a raw pointer or a reference (possibly const). | Sonar-Meldung direkt umsetzen: Replace this use of "unique_ptr" by a raw pointer or a reference (possibly const). | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ2DVDgODPiZK5yPV1-J | Issue | cpp:S1188 | MAJOR | test/regression/regression_test.cc | 235 | This lambda has 23 lines, which is greater than the 20 lines authorized. Split it into several lambdas or functions, or make it a named function. | Sonar-Meldung direkt umsetzen: This lambda has 23 lines, which is greater than the 20 lines authorized. Split it into several lambdas or functions, or make it a named function. | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ2CBI6Kkud7vHWq0tqj | Issue | cpp:S6022 | MAJOR | src/operators/validate_byte_range.cc | 157 | Use "std::byte" for byte-oriented data manipulation. | Sonar-Meldung direkt umsetzen: Use "std::byte" for byte-oriented data manipulation. | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ2CA_xuGCkM6OziEPeu | Issue | cpp:S4144 | MAJOR | test/unit/json_backend_depth_tests.cc | 50 | Update this method so that its implementation is not identical to on_key. | Sonar-Meldung direkt umsetzen: Update this method so that its implementation is not identical to on_key. | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ2BthMEO-njQfcv_7WG | Issue | cpp:S3776 | CRITICAL | src/request_body_processor/json_backend_jsoncons.cc | 348 | Refactor this function to reduce its Cognitive Complexity from 41 to the 25 allowed. | Sonar-Meldung direkt umsetzen: Refactor this function to reduce its Cognitive Complexity from 41 to the 25 allowed. | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ2BthMEO-njQfcv_7WJ | Issue | cpp:S3776 | CRITICAL | src/request_body_processor/json_backend_jsoncons.cc | 412 | Refactor this function to reduce its Cognitive Complexity from 43 to the 25 allowed. | Sonar-Meldung direkt umsetzen: Refactor this function to reduce its Cognitive Complexity from 43 to the 25 allowed. | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ1-doCWXISY38E6TxpK | Issue | cpp:S6009 | MINOR | test/benchmark/json_benchmark.cc | 321 | Replace this const reference to "std::string" by a "std::string_view". | Sonar-Meldung direkt umsetzen: Replace this const reference to "std::string" by a "std::string_view". | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ1-doCWXISY38E6TxpR | Issue | cpp:S6009 | MINOR | test/benchmark/json_benchmark.cc | 419 | Replace this const reference to "std::string" by a "std::string_view". | Sonar-Meldung direkt umsetzen: Replace this const reference to "std::string" by a "std::string_view". | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ1-doCWXISY38E6TxpU | Issue | cpp:S6009 | MINOR | test/benchmark/json_benchmark.cc | 486 | Replace this const reference to "std::string" by a "std::string_view". | Sonar-Meldung direkt umsetzen: Replace this const reference to "std::string" by a "std::string_view". | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ190QEMSTzC4JOHOn9q | Issue | cpp:S6022 | MAJOR | src/operators/validate_byte_range.cc | 73 | Use "std::byte" for byte-oriented data manipulation. | Sonar-Meldung direkt umsetzen: Use "std::byte" for byte-oriented data manipulation. | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ190QFsSTzC4JOHOn9w | Issue | cpp:S3776 | CRITICAL | src/request_body_processor/json_backend_jsoncons.cc | 92 | Refactor this function to reduce its Cognitive Complexity from 33 to the 25 allowed. | Sonar-Meldung direkt umsetzen: Refactor this function to reduce its Cognitive Complexity from 33 to the 25 allowed. | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ190QFsSTzC4JOHOn9u | Issue | cpp:S3562 | MAJOR | src/request_body_processor/json_backend_jsoncons.cc | 188 | 4 enumeration values not handled in switch: 'int64_value', 'uint64_value', 'half_value'... | Sonar-Meldung direkt umsetzen: 4 enumeration values not handled in switch: 'int64_value', 'uint64_value', 'half_value'... | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ190QFsSTzC4JOHOn90 | Issue | cpp:S134 | CRITICAL | src/request_body_processor/json_backend_jsoncons.cc | 365 | Refactor this code to not nest more than 3 if|for|do|while|switch statements. | Sonar-Meldung direkt umsetzen: Refactor this code to not nest more than 3 if|for|do|while|switch statements. | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ190QFsSTzC4JOHOn91 | Issue | cpp:S134 | CRITICAL | src/request_body_processor/json_backend_jsoncons.cc | 374 | Refactor this code to not nest more than 3 if|for|do|while|switch statements. | Sonar-Meldung direkt umsetzen: Refactor this code to not nest more than 3 if|for|do|while|switch statements. | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ190QA8STzC4JOHOn9k | Issue | cpp:S5945 | MAJOR | src/utils/json_writer.cc | 156 | Use "std::string" instead of a C-style char array. | Sonar-Meldung direkt umsetzen: Use "std::string" instead of a C-style char array. | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ190QA8STzC4JOHOn9n | Issue | cpp:S6022 | MAJOR | src/utils/json_writer.cc | 184 | Use "std::byte" for byte-oriented data manipulation. | Sonar-Meldung direkt umsetzen: Use "std::byte" for byte-oriented data manipulation. | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ190QA8STzC4JOHOn9o | Issue | cpp:S6022 | MAJOR | src/utils/json_writer.cc | 185 | Use "std::byte" for byte-oriented data manipulation. | Sonar-Meldung direkt umsetzen: Use "std::byte" for byte-oriented data manipulation. | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ190QIVSTzC4JOHOn-g | Issue | cpp:S2807 | MAJOR | test/common/json.h | 78 | Make this member overloaded operator a hidden friend. | Sonar-Meldung direkt umsetzen: Make this member overloaded operator a hidden friend. | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ190QIVSTzC4JOHOn-h | Issue | cpp:S2807 | MAJOR | test/common/json.h | 123 | Make this member overloaded operator a hidden friend. | Sonar-Meldung direkt umsetzen: Make this member overloaded operator a hidden friend. | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ190QIVSTzC4JOHOn-i | Issue | cpp:S1181 | MAJOR | test/common/json.h | 219 | Catch a more specific exception instead of a generic one. | Sonar-Meldung direkt umsetzen: Catch a more specific exception instead of a generic one. | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ190QIVSTzC4JOHOn-j | Issue | cpp:S995 | MINOR | test/common/json.h | 232 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Sonar-Meldung direkt umsetzen: Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ190QIVSTzC4JOHOn-k | Issue | cpp:S995 | MINOR | test/common/json.h | 242 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Sonar-Meldung direkt umsetzen: Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ190QIVSTzC4JOHOn-l | Issue | cpp:S995 | MINOR | test/common/json.h | 252 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Sonar-Meldung direkt umsetzen: Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ190QIVSTzC4JOHOn-m | Issue | cpp:S995 | MINOR | test/common/json.h | 262 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Sonar-Meldung direkt umsetzen: Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ190QIVSTzC4JOHOn-n | Issue | cpp:S995 | MINOR | test/common/json.h | 269 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Sonar-Meldung direkt umsetzen: Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ190QIVSTzC4JOHOn-o | Issue | cpp:S995 | MINOR | test/common/json.h | 279 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Sonar-Meldung direkt umsetzen: Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ190QIVSTzC4JOHOn-p | Issue | cpp:S1181 | MAJOR | test/common/json.h | 309 | Catch a more specific exception instead of a generic one. | Sonar-Meldung direkt umsetzen: Catch a more specific exception instead of a generic one. | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ190QIVSTzC4JOHOn-q | Issue | cpp:S1181 | MAJOR | test/common/json.h | 321 | Catch a more specific exception instead of a generic one. | Sonar-Meldung direkt umsetzen: Catch a more specific exception instead of a generic one. | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ190QIVSTzC4JOHOn-r | Issue | cpp:S1181 | MAJOR | test/common/json.h | 333 | Catch a more specific exception instead of a generic one. | Sonar-Meldung direkt umsetzen: Catch a more specific exception instead of a generic one. | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ190QI2STzC4JOHOn-s | Issue | cpp:S134 | CRITICAL | test/common/modsecurity_test.cc | 89 | Refactor this code to not nest more than 3 if|for|do|while|switch statements. | Sonar-Meldung direkt umsetzen: Refactor this code to not nest more than 3 if|for|do|while|switch statements. | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ190QKvSTzC4JOHOn-4 | Issue | cpp:S5817 | MAJOR | test/regression/regression_test.cc | 431 | This function should be declared "const". | Sonar-Meldung direkt umsetzen: This function should be declared "const". | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ190QIESTzC4JOHOn-d | Issue | cpp:S886 | MINOR | src/modsecurity.cc | 232 | Refactor this loop so that it is less error-prone. | Sonar-Meldung direkt umsetzen: Refactor this loop so that it is less error-prone. | Mittel (Codeänderung abhängig vom markierten Befund) |
| AZ190QIESTzC4JOHOn-e | Issue | cpp:S886 | MINOR | src/modsecurity.cc | 288 | Refactor this loop so that it is less error-prone. | Sonar-Meldung direkt umsetzen: Refactor this loop so that it is less error-prone. | Mittel (Codeänderung abhängig vom markierten Befund) |

## 6) Summenübersicht
- Gesamtanzahl aller Befunde (Issues + Gate Conditions + Hotspots): **219**
- Gesamtanzahl Issues: **214**
- Anzahl offene Befunde (Issues-Status OPEN/TO_REVIEW/CONFIRMED/REOPENED): **40**
- Anzahl geschlossene Befunde (restliche Issue-Status): **174**
- Security Hotspots offen: **0**
- Security Hotspots reviewed: **0**

### Severity-Verteilung (Issues)
| Severity | Anzahl |
|---|---:|
| BLOCKER | 0 |
| CRITICAL | 30 |
| MAJOR | 106 |
| MINOR | 77 |
| INFO | 1 |

### Anzahl pro Regelcode (Issues)
| Regelcode | Anzahl |
|---|---:|
| cpp:S112 | 31 |
| cpp:S6004 | 31 |
| shelldre:S7688 | 22 |
| cpp:S5812 | 13 |
| cpp:S995 | 13 |
| cpp:S134 | 10 |
| cpp:S3776 | 9 |
| cpp:S5415 | 8 |
| cpp:S6009 | 7 |
| cpp:S4962 | 6 |
| cpp:S1121 | 5 |
| cpp:S3628 | 5 |
| cpp:S1181 | 4 |
| cpp:S5945 | 4 |
| cpp:S6022 | 4 |
| cpp:S1172 | 3 |
| cpp:S3230 | 3 |
| cpp:S886 | 3 |
| shelldre:S7682 | 3 |
| cpp:S1117 | 2 |
| cpp:S1155 | 2 |
| cpp:S2807 | 2 |
| cpp:S4144 | 2 |
| cpp:S4998 | 2 |
| cpp:S5274 | 2 |
| cpp:S5421 | 2 |
| cpp:S5817 | 2 |
| cpp:S7121 | 2 |
| cpp:S1135 | 1 |
| cpp:S1188 | 1 |
| cpp:S1481 | 1 |
| cpp:S3562 | 1 |
| cpp:S3624 | 1 |
| cpp:S5025 | 1 |
| cpp:S5827 | 1 |
| cpp:S5952 | 1 |
| cpp:S6018 | 1 |
| cpp:S6024 | 1 |
| cpp:S7127 | 1 |
| cpp:S836 | 1 |

## 7.1) Offene Befunde in JSON-Dateien (vorgegebene Dateiliste)
| Datei | Anzahl offener Befunde | Regelcodes | Problem | Minimaler Fix |
|---|---:|---|---|---|
| src/request_body_processor/json.cc | 1 | cpp:S6009 | Replace this const reference to "std::string" by a "std::string_view". | Sonar-Meldung direkt umsetzen: Replace this const reference to "std::string" by a "std::string_view". |
| src/request_body_processor/json.h | 0 | — | — | — |
| src/request_body_processor/json_adapter.cc | 2 | cpp:S6024, cpp:S995 | Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *".; Prefer free functions over member functions when handling objects of generic type "InputType". | Sonar-Meldung direkt umsetzen: Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *".; Sonar-Meldung direkt umsetzen: Prefer free functions over member functions when handling objects of generic type "InputType". |
| src/request_body_processor/json_adapter.h | 0 | — | — | — |
| src/request_body_processor/json_backend.h | 0 | — | — | — |
| src/request_body_processor/json_backend_jsoncons.cc | 7 | cpp:S134, cpp:S3562, cpp:S3776, cpp:S6004 | 4 enumeration values not handled in switch: 'int64_value', 'uint64_value', 'half_value'...; Refactor this code to not nest more than 3 if|for|do|while|switch statements.; Refactor this function to reduce its Cognitive Complexity from 33 to the 25 allowed.; Refactor this function to reduce its Cognitive Complexity from 41 to the 25 allowed.; Refactor this function to reduce its Cognitive Complexity from 43 to the 25 allowed.; Use the init-statement to declare "sync_detail" inside the if statement. | Sonar-Meldung direkt umsetzen: 4 enumeration values not handled in switch: 'int64_value', 'uint64_value', 'half_value'...; Sonar-Meldung direkt umsetzen: Refactor this code to not nest more than 3 if|for|do|while|switch statements.; Sonar-Meldung direkt umsetzen: Refactor this function to reduce its Cognitive Complexity from 33 to the 25 allowed.; Sonar-Meldung direkt umsetzen: Refactor this function to reduce its Cognitive Complexity from 41 to the 25 allowed.; Sonar-Meldung direkt umsetzen: Refactor this function to reduce its Cognitive Complexity from 43 to the 25 allowed.; Sonar-Meldung direkt umsetzen: Use the init-statement to declare "sync_detail" inside the if statement. |
| src/request_body_processor/json_backend_simdjson.cc | 0 | — | — | — |
| src/request_body_processor/json_instrumentation.cc | 0 | — | — | — |
| src/request_body_processor/json_instrumentation.h | 0 | — | — | — |

## 7.2) Architektur-Prüfung (code-faktenbasiert)
- Build-Time Auswahl: In `configure.ac` ist `--with-json-backend={simdjson|jsoncons}` definiert, inklusive Fehler bei ungültigem Wert; dazu werden je nach Auswahl `MSC_JSON_BACKEND_SIMDJSON` oder `MSC_JSON_BACKEND_JSONCONS` gesetzt.
- Unix/Autotools-Kompilation: `src/Makefile.am` nimmt abhängig von `JSON_BACKEND_SIMDJSON` oder `JSON_BACKEND_JSONCONS` **genau eine** Backend-Datei in die Build-Liste auf.
- Windows/CMake-Kompilation: `build/win32/CMakeLists.txt` entfernt beide Backend-Dateien aus der globalen Liste und hängt dann nur `JSON_BACKEND_SOURCES` der gewählten Variante an.
- Gemeinsame Interfaces: `json_backend.h` definiert `JsonEventSink`, `JsonParseResult`, `JsonBackendParseOptions` und Funktionssignaturen für beide Backends.
- Adapter-Schicht: `json_adapter.cc` dispatcht per Präprozessor auf genau ein Backend (simdjson oder jsoncons).
- Tatsächliche Kopplung: beide Backend-Implementierungen inkludieren nur `json_backend.h` + `json_backend_common.h` (plus jeweilige Third-Party-Header), keine direkte gegenseitige Include-Kopplung.
- Duplikation: Beide Backends enthalten jeweils eigene Fehler-Mappings und Event-Walker; gemeinsame Hilfen sind in `json_backend_common.h`, aber Parsing-/Traversal-Logik ist backend-spezifisch.

**Bewertung:** teilweise getrennt (saubere Build-Time-Selektion und gemeinsames Interface; gleichzeitig bewusst doppelte backend-spezifische Traversal-/Fehlerlogik).

## 8) Vergleich
Kein belastbarer Vergleich möglich (es wurden in dieser Analyse keine historischen Sonar-Snapshots als Referenz verwendet).

## 9) Konkrete Fixes für offene Probleme (regelbasiert)
| Regelcode | Datei(en) (offen) | Problem (aus Sonar-Meldungen) | Minimaler Fix | Erwarteter Sonar-Effekt | Risiko |
|---|---|---|---|---|---|
| cpp:S1181 | test/common/json.h | Catch a more specific exception instead of a generic one. | Sonar-Meldung direkt umsetzen (siehe Problemspalte). | Reduktion offener cpp:S1181-Issues nach Reanalyse | Mittel |
| cpp:S1188 | test/regression/regression_test.cc | This lambda has 23 lines, which is greater than the 20 lines authorized. Split it into several lambdas or functions, or make it a named function. | Sonar-Meldung direkt umsetzen (siehe Problemspalte). | Reduktion offener cpp:S1188-Issues nach Reanalyse | Mittel |
| cpp:S134 | src/request_body_processor/json_backend_jsoncons.cc, src/request_body_processor/xml.cc, test/common/modsecurity_test.cc | Refactor this code to not nest more than 3 if|for|do|while|switch statements. | Sonar-Meldung direkt umsetzen (siehe Problemspalte). | Reduktion offener cpp:S134-Issues nach Reanalyse | Mittel |
| cpp:S2807 | test/common/json.h | Make this member overloaded operator a hidden friend. | Sonar-Meldung direkt umsetzen (siehe Problemspalte). | Reduktion offener cpp:S2807-Issues nach Reanalyse | Mittel |
| cpp:S3562 | src/request_body_processor/json_backend_jsoncons.cc | 4 enumeration values not handled in switch: 'int64_value', 'uint64_value', 'half_value'... | Sonar-Meldung direkt umsetzen (siehe Problemspalte). | Reduktion offener cpp:S3562-Issues nach Reanalyse | Mittel |
| cpp:S3776 | src/request_body_processor/json_backend_jsoncons.cc | Refactor this function to reduce its Cognitive Complexity from 33 to the 25 allowed.; Refactor this function to reduce its Cognitive Complexity from 41 to the 25 allowed.; Refactor this function to reduce its Cognitive Complexity from 43 to the 25 allowed. | Sonar-Meldung direkt umsetzen (siehe Problemspalte). | Reduktion offener cpp:S3776-Issues nach Reanalyse | Mittel |
| cpp:S4144 | test/unit/json_backend_depth_tests.cc | Update this method so that its implementation is not identical to on_key. | Sonar-Meldung direkt umsetzen (siehe Problemspalte). | Reduktion offener cpp:S4144-Issues nach Reanalyse | Mittel |
| cpp:S4998 | test/benchmark/json_benchmark.cc | Replace this use of "unique_ptr" by a raw pointer or a reference (possibly const). | Sonar-Meldung direkt umsetzen (siehe Problemspalte). | Reduktion offener cpp:S4998-Issues nach Reanalyse | Mittel |
| cpp:S5817 | test/regression/regression_test.cc | This function should be declared "const". | Sonar-Meldung direkt umsetzen (siehe Problemspalte). | Reduktion offener cpp:S5817-Issues nach Reanalyse | Mittel |
| cpp:S5945 | src/utils/json_writer.cc | Use "std::string" instead of a C-style char array. | Sonar-Meldung direkt umsetzen (siehe Problemspalte). | Reduktion offener cpp:S5945-Issues nach Reanalyse | Mittel |
| cpp:S5952 | test/benchmark/json_benchmark.cc | Add a using-declaration to this derived class to inherit the constructors of "runtime_error", and remove the ones you manually duplicated. Note that this may add other constructors to your derived class. | Sonar-Meldung direkt umsetzen (siehe Problemspalte). | Reduktion offener cpp:S5952-Issues nach Reanalyse | Mittel |
| cpp:S6004 | src/request_body_processor/json_backend_jsoncons.cc, test/benchmark/json_benchmark.cc | Use the init-statement to declare "current" inside the if statement.; Use the init-statement to declare "sync_detail" inside the if statement. | Sonar-Meldung direkt umsetzen (siehe Problemspalte). | Reduktion offener cpp:S6004-Issues nach Reanalyse | Mittel |
| cpp:S6009 | src/request_body_processor/json.cc, test/benchmark/json_benchmark.cc | Replace this const reference to "std::string" by a "std::string_view". | Sonar-Meldung direkt umsetzen (siehe Problemspalte). | Reduktion offener cpp:S6009-Issues nach Reanalyse | Mittel |
| cpp:S6022 | src/operators/validate_byte_range.cc, src/utils/json_writer.cc | Use "std::byte" for byte-oriented data manipulation. | Sonar-Meldung direkt umsetzen (siehe Problemspalte). | Reduktion offener cpp:S6022-Issues nach Reanalyse | Mittel |
| cpp:S6024 | src/request_body_processor/json_adapter.cc | Prefer free functions over member functions when handling objects of generic type "InputType". | Sonar-Meldung direkt umsetzen (siehe Problemspalte). | Reduktion offener cpp:S6024-Issues nach Reanalyse | Mittel |
| cpp:S886 | src/modsecurity.cc | Refactor this loop so that it is less error-prone. | Sonar-Meldung direkt umsetzen (siehe Problemspalte). | Reduktion offener cpp:S886-Issues nach Reanalyse | Mittel |
| cpp:S995 | src/request_body_processor/json_adapter.cc, test/common/json.h | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *".; Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *". | Sonar-Meldung direkt umsetzen (siehe Problemspalte). | Reduktion offener cpp:S995-Issues nach Reanalyse | Mittel |

## 10) Grenzen der Analyse
- Abrufbar waren: PR-Metadaten, Quality Gate, Measures, vollständige Issues-Seite (total=214, geladen=214), Hotspots (total=0).
- Nicht abrufbar auf Basis der aktuellen Quellen: objektiver Wert für „Direkt behebbar (Ja/Teilweise/Nein)“ je Befund; daher als **Nicht belegbar auf Basis der aktuellen Quellen** markiert.
- Historischer Vorher/Nachher-Vergleich ohne zusätzliche Baseline nicht verifizierbar.

**Vollständigkeitsstatus Sonar-Liste:** vollständige Sonar-Liste der aktuell abrufbaren Issues + Gate Conditions + Hotspots für PR 3540.