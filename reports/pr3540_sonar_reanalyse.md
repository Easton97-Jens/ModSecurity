# Aktueller Commit
Analyse basiert auf Commit: `bbde416a92212f1fc4a975361e73a3b07356a526`.

# Quality Gate
Status: **OK**

## Measures (Sonar API)
- minor_violations: 17
- new_minor_violations: 17
- code_smells: 52
- new_vulnerabilities: 0
- violations: 52
- new_info_violations: 0
- security_hotspots: 0
- new_bugs: 0
- new_code_smells: 52
- critical_violations: 17
- new_major_violations: 18
- blocker_violations: 0
- bugs: 0
- info_violations: 0
- alert_status: OK
- new_security_hotspots: 0
- new_critical_violations: 17
- major_violations: 18
- new_violations: 52
- new_blocker_violations: 0
- vulnerabilities: 0

# Tabelle: Alle Befunde
| ID | Kategorie | Regelcode | Severity | Status | Datei | Zeile | Nachricht | Codebereich | Behebbar |
|---|---|---|---|---|---|---:|---|---|---|
| GATE-3540 | Gate | quality_gate | OK | OK | - | - | Quality Gate für PR 3540 | Production | Nein |
| AZ2HqeBsWym3B0O6okyS | Issue | cpp:S5952 | MINOR | OPEN | test/benchmark/json_benchmark.cc | 71 | Add a using-declaration to this derived class to inherit the constructors of "runtime_error", and remove the ones you manually duplicated. Note that this may add other constructors to your derived class. | Test | Ja |
| AZ2HqeBsWym3B0O6okyT | Issue | cpp:S6004 | MINOR | OPEN | test/benchmark/json_benchmark.cc | 144 | Use the init-statement to declare "current" inside the if statement. | Test | Ja |
| AZ2DWE24t-zbsGOGdN-_ | Issue | cpp:S3776 | CRITICAL | CLOSED | test/benchmark/json_benchmark.cc |  | Refactor this function to reduce its Cognitive Complexity from 33 to the 25 allowed. | Test | Nein |
| AZ2DWE24t-zbsGOGdN_A | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ2DWE24t-zbsGOGdN_B | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ2DWE24t-zbsGOGdN_C | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ2DWE24t-zbsGOGdN_D | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ2DWE24t-zbsGOGdN_E | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ2DWE24t-zbsGOGdN_F | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ2DWE24t-zbsGOGdN_G | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ2DWE24t-zbsGOGdN_H | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ2DWE24t-zbsGOGdN_I | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ2DWE24t-zbsGOGdN_J | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ2DWE24t-zbsGOGdN_K | Issue | cpp:S4998 | MAJOR | OPEN | test/benchmark/json_benchmark.cc | 316 | Replace this use of "unique_ptr" by a raw pointer or a reference (possibly const). | Test | Ja |
| AZ2DWE24t-zbsGOGdN_M | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ2DWE24t-zbsGOGdN_N | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ2DWE24t-zbsGOGdN_O | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ2DWE24t-zbsGOGdN_P | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ2DVDgODPiZK5yPV1-J | Issue | cpp:S1188 | MAJOR | OPEN | test/regression/regression_test.cc | 235 | This lambda has 23 lines, which is greater than the 20 lines authorized. Split it into several lambdas or functions, or make it a named function. | Test | Ja |
| AZ2DR4Fykud7vHWq_QVC | Issue | cpp:S6009 | MINOR | OPEN | src/request_body_processor/json_adapter.cc | 58 | Replace this const reference to "std::string" by a "std::string_view". | Production | Ja |
| AZ2DR4Fykud7vHWq_QVD | Issue | cpp:S995 | MINOR | OPEN | src/request_body_processor/json_adapter.cc | 59 | Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *". | Production | Ja |
| AZ2DR4Fykud7vHWq_QVE | Issue | cpp:S995 | MINOR | OPEN | src/request_body_processor/json_adapter.cc | 81 | Make the type of this parameter a reference-to-const. The current type of "input" is "std::string &". | Production | Ja |
| AZ2DK9KwXISY38E6wMPS | Issue | cpp:S995 | MINOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *". | Production | Nein |
| AZ2C_aavSTzC4JOHsQM1 | Issue | cpp:S1121 | MAJOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Extract the assignment from this expression. | Production | Nein |
| AZ2CwcldK0fgB4uOpVKy | Issue | cpp:S995 | MINOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Make the type of this parameter a reference-to-const. The current type of "input" is "std::string &". | Production | Nein |
| AZ2CwcldK0fgB4uOpVK0 | Issue | cpp:S995 | MINOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *". | Production | Nein |
| AZ2CwcldK0fgB4uOpVKz | Issue | cpp:S1172 | MAJOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Remove the unused parameter "options", make it unnamed, or declare it "[[maybe_unused]]". | Production | Nein |
| AZ2CwcnoK0fgB4uOpVK1 | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Use the init-statement to declare "tail" inside the if statement. | Production | Nein |
| AZ2CwcnoK0fgB4uOpVK2 | Issue | cpp:S1117 | MAJOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Declaration shadows a local variable "result" in the outer scope. | Production | Nein |
| AZ2CwcnoK0fgB4uOpVK3 | Issue | cpp:S1117 | MAJOR | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Declaration shadows a local variable "result" in the outer scope. | Production | Nein |
| AZ2CwcnoK0fgB4uOpVK4 | Issue | cpp:S5817 | MAJOR | OPEN | src/request_body_processor/json_backend_simdjson.cc | 450 | This function should be declared "const". | Production | Ja |
| AZ2CwcpOK0fgB4uOpVK5 | Issue | cpp:S6004 | MINOR | CLOSED | test/unit/json_backend_depth_tests.cc |  | Use the init-statement to declare "result" inside the if statement. | Test | Nein |
| AZ2CwcpOK0fgB4uOpVK6 | Issue | cpp:S6004 | MINOR | CLOSED | test/unit/json_backend_depth_tests.cc |  | Use the init-statement to declare "result" inside the if statement. | Test | Nein |
| AZ2CdKxRGCkM6OziHCww | Issue | cpp:S6004 | MINOR | CLOSED | test/unit/json_backend_depth_tests.cc |  | Use the init-statement to declare "result" inside the if statement. | Test | Nein |
| AZ2CdKxRGCkM6OziHCwx | Issue | cpp:S5945 | MAJOR | CLOSED | test/unit/json_backend_depth_tests.cc |  | Use "std::array" or "std::vector" instead of a C-style array. | Test | Nein |
| AZ2CdKxRGCkM6OziHCwy | Issue | cpp:S3628 | MINOR | CLOSED | test/unit/json_backend_depth_tests.cc |  | Convert this string literal to a raw string literal. | Test | Nein |
| AZ2CBI6Kkud7vHWq0tqj | Issue | cpp:S6022 | MAJOR | OPEN | src/operators/validate_byte_range.cc | 156 | Use "std::byte" for byte-oriented data manipulation. | Production | Ja |
| AZ2CA_0CGCkM6OziEPex | Issue | shelldre:S7688 | MAJOR | CLOSED | test/run-json-backend-matrix.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nein |
| AZ2CA_xuGCkM6OziEPet | Issue | cpp:S5812 | MINOR | CLOSED | test/unit/json_backend_depth_tests.cc |  | Concatenate this namespace with the nested one. | Test | Nein |
| AZ2CA_xuGCkM6OziEPeu | Issue | cpp:S4144 | MAJOR | OPEN | test/unit/json_backend_depth_tests.cc | 50 | Update this method so that its implementation is not identical to on_key. | Test | Ja |
| AZ2CA_xuGCkM6OziEPev | Issue | cpp:S6004 | MINOR | CLOSED | test/unit/json_backend_depth_tests.cc |  | Use the init-statement to declare "result" inside the if statement. | Test | Nein |
| AZ2CA_xuGCkM6OziEPew | Issue | cpp:S6004 | MINOR | CLOSED | test/unit/json_backend_depth_tests.cc |  | Use the init-statement to declare "result" inside the if statement. | Test | Nein |
| AZ2BthMEO-njQfcv_7WG | Issue | cpp:S3776 | CRITICAL | OPEN | src/request_body_processor/json_backend_jsoncons.cc | 362 | Refactor this function to reduce its Cognitive Complexity from 41 to the 25 allowed. | Production | Ja |
| AZ2BthMEO-njQfcv_7WH | Issue | cpp:S1121 | MAJOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Extract the assignment from this expression. | Production | Nein |
| AZ2BthMEO-njQfcv_7WI | Issue | cpp:S1121 | MAJOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Extract the assignment from this expression. | Production | Nein |
| AZ2BthMEO-njQfcv_7WJ | Issue | cpp:S3776 | CRITICAL | OPEN | src/request_body_processor/json_backend_jsoncons.cc | 426 | Refactor this function to reduce its Cognitive Complexity from 43 to the 25 allowed. | Production | Ja |
| AZ2BthMEO-njQfcv_7WK | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Use the init-statement to declare "decoded_number" inside the if statement. | Production | Nein |
| AZ2BthMEO-njQfcv_7WL | Issue | cpp:S134 | CRITICAL | OPEN | src/request_body_processor/json_backend_jsoncons.cc | 634 | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements. | Production | Ja |
| AZ2BkKY5XISY38E6k-Ld | Issue | cpp:S1135 | INFO | CLOSED | src/request_body_processor/json_backend_simdjson.cc |  | Complete the task associated to this "TODO" comment. | Production | Nein |
| AZ1-dn-nXISY38E6Txop | Issue | cpp:S5025 | CRITICAL | OPEN | src/request_body_processor/json.cc | 57 | Rewrite the code so that you no longer need this "delete". | Production | Ja |
| AZ1-dn-nXISY38E6Txoq | Issue | cpp:S5827 | MAJOR | CLOSED | src/request_body_processor/json.cc |  | Replace the redundant type with "auto". | Production | Nein |
| AZ1-dn9hXISY38E6Txon | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json_instrumentation.cc |  | Concatenate this namespace with the nested one. | Production | Nein |
| AZ1-dn9hXISY38E6Txoo | Issue | cpp:S5421 | CRITICAL | OPEN | src/request_body_processor/json_instrumentation.cc | 12 | Global variables should be const. | Production | Ja |
| AZ1-dn9QXISY38E6Txom | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json_instrumentation.h |  | Concatenate this namespace with the nested one. | Production | Nein |
| AZ1-dn4cXISY38E6Txoh | Issue | cpp:S4962 | CRITICAL | CLOSED | src/utils/msc_tree.cc |  | Use the "nullptr" literal. | Production | Nein |
| AZ1-doCWXISY38E6Txoz | Issue | cpp:S5421 | CRITICAL | CLOSED | test/benchmark/json_benchmark.cc |  | Global pointers should be const at every level. | Test | Nein |
| AZ1-doCWXISY38E6Txo0 | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ1-doCWXISY38E6Txo1 | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ1-doCWXISY38E6Txo6 | Issue | cpp:S886 | MINOR | CLOSED | test/benchmark/json_benchmark.cc |  | Refactor this loop so that it is less error-prone. | Test | Nein |
| AZ1-doCWXISY38E6Txo3 | Issue | cpp:S6004 | MINOR | CLOSED | test/benchmark/json_benchmark.cc |  | Use the init-statement to declare "output_format" inside the if statement. | Test | Nein |
| AZ1-doCWXISY38E6Txo4 | Issue | cpp:S6004 | MINOR | CLOSED | test/benchmark/json_benchmark.cc |  | Use the init-statement to declare "is_invalid_scenario" inside the if statement. | Test | Nein |
| AZ1-doCWXISY38E6TxpE | Issue | cpp:S5945 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Use "std::array" or "std::vector" instead of a C-style array. | Test | Nein |
| AZ1-doCWXISY38E6TxpF | Issue | cpp:S5945 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Use "std::array" or "std::vector" instead of a C-style array. | Test | Nein |
| AZ1-doCWXISY38E6TxpG | Issue | cpp:S7127 | CRITICAL | CLOSED | test/benchmark/json_benchmark.cc |  | Use "std::size" to get the size of this array. | Test | Nein |
| AZ1-doCWXISY38E6TxpK | Issue | cpp:S6009 | MINOR | OPEN | test/benchmark/json_benchmark.cc | 321 | Replace this const reference to "std::string" by a "std::string_view". | Test | Ja |
| AZ1-doCWXISY38E6TxpM | Issue | cpp:S7121 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Remove this redundant call to "c_str" when initializing a const "std::string" reference parameter. | Test | Nein |
| AZ1-doCWXISY38E6TxpJ | Issue | cpp:S6004 | MINOR | CLOSED | test/benchmark/json_benchmark.cc |  | Use the init-statement to declare "parse_error" inside the if statement. | Test | Nein |
| AZ1-doCWXISY38E6TxpS | Issue | cpp:S3628 | MINOR | CLOSED | test/benchmark/json_benchmark.cc |  | Convert this string literal to a raw string literal. | Test | Nein |
| AZ1-doCWXISY38E6TxpT | Issue | cpp:S3628 | MINOR | CLOSED | test/benchmark/json_benchmark.cc |  | Convert this string literal to a raw string literal. | Test | Nein |
| AZ1-doCWXISY38E6TxpR | Issue | cpp:S6009 | MINOR | OPEN | test/benchmark/json_benchmark.cc | 419 | Replace this const reference to "std::string" by a "std::string_view". | Test | Ja |
| AZ1-doCWXISY38E6TxpV | Issue | cpp:S6004 | MINOR | CLOSED | test/benchmark/json_benchmark.cc |  | Use the init-statement to declare "rules_path" inside the if statement. | Test | Nein |
| AZ1-doCWXISY38E6TxpU | Issue | cpp:S6009 | MINOR | OPEN | test/benchmark/json_benchmark.cc | 486 | Replace this const reference to "std::string" by a "std::string_view". | Test | Ja |
| AZ1-doCWXISY38E6Txo2 | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ1-doCWXISY38E6Txo5 | Issue | cpp:S3776 | CRITICAL | CLOSED | test/benchmark/json_benchmark.cc |  | Refactor this function to reduce its Cognitive Complexity from 33 to the 25 allowed. | Test | Nein |
| AZ1-doCWXISY38E6Txo7 | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ1-doCWXISY38E6Txo8 | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ1-doCWXISY38E6Txo9 | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ1-doCWXISY38E6Txo- | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ1-doCWXISY38E6Txo_ | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ1-doCWXISY38E6TxpA | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ1-doCWXISY38E6TxpB | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ1-doCWXISY38E6TxpC | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ1-doCWXISY38E6TxpD | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ1-doCWXISY38E6TxpH | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ1-doCWXISY38E6TxpI | Issue | cpp:S4998 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Replace this use of "unique_ptr" by a raw pointer or a reference (possibly const). | Test | Nein |
| AZ1-doCWXISY38E6TxpN | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ1-doCWXISY38E6TxpO | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ1-doCWXISY38E6TxpP | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ1-doCWXISY38E6TxpQ | Issue | cpp:S112 | MAJOR | CLOSED | test/benchmark/json_benchmark.cc |  | Define and throw a dedicated exception instead of using a generic one. | Test | Nein |
| AZ1-doCHXISY38E6Txor | Issue | shelldre:S7682 | MAJOR | CLOSED | test/benchmark/run-json-benchmarks.sh |  | Add an explicit return statement at the end of the function. | Test | Nein |
| AZ1-doCHXISY38E6Txos | Issue | shelldre:S7688 | MAJOR | CLOSED | test/benchmark/run-json-benchmarks.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nein |
| AZ1-doCHXISY38E6Txot | Issue | shelldre:S7688 | MAJOR | CLOSED | test/benchmark/run-json-benchmarks.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nein |
| AZ1-doCHXISY38E6Txou | Issue | shelldre:S7688 | MAJOR | CLOSED | test/benchmark/run-json-benchmarks.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nein |
| AZ1-doCHXISY38E6Txov | Issue | shelldre:S7688 | MAJOR | CLOSED | test/benchmark/run-json-benchmarks.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nein |
| AZ1-doCHXISY38E6Txow | Issue | shelldre:S7688 | MAJOR | CLOSED | test/benchmark/run-json-benchmarks.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nein |
| AZ1-doCHXISY38E6Txox | Issue | shelldre:S7688 | MAJOR | CLOSED | test/benchmark/run-json-benchmarks.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nein |
| AZ1-doCHXISY38E6Txoy | Issue | shelldre:S7688 | MAJOR | CLOSED | test/benchmark/run-json-benchmarks.sh |  | Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich. | Test | Nein |
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
| AZ190QEMSTzC4JOHOn9q | Issue | cpp:S6022 | MAJOR | OPEN | src/operators/validate_byte_range.cc | 72 | Use "std::byte" for byte-oriented data manipulation. | Production | Ja |
| AZ190QEMSTzC4JOHOn9r | Issue | cpp:S6004 | MINOR | CLOSED | src/operators/validate_byte_range.cc |  | Use the init-statement to declare "token" inside the if statement. | Production | Nein |
| AZ190QGTSTzC4JOHOn-D | Issue | cpp:S3230 | MAJOR | OPEN | src/request_body_processor/json.cc | 79 | Do not use the constructor's initializer list for data member "m_data". Use the in-class initializer instead. | Production | Ja |
| AZ190QGTSTzC4JOHOn-H | Issue | cpp:S3776 | CRITICAL | OPEN | src/request_body_processor/json.cc | 121 | Refactor this function to reduce its Cognitive Complexity from 37 to the 25 allowed. | Production | Ja |
| AZ190QGTSTzC4JOHOn-G | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json.cc |  | Use the init-statement to declare "result" inside the if statement. | Production | Nein |
| AZ190QGTSTzC4JOHOn-I | Issue | cpp:S134 | CRITICAL | OPEN | src/request_body_processor/json.cc | 135 | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements. | Production | Ja |
| AZ190QGTSTzC4JOHOn-J | Issue | cpp:S134 | CRITICAL | OPEN | src/request_body_processor/json.cc | 142 | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements. | Production | Ja |
| AZ190QGTSTzC4JOHOn-K | Issue | cpp:S134 | CRITICAL | OPEN | src/request_body_processor/json.cc | 149 | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements. | Production | Ja |
| AZ190QGTSTzC4JOHOn-L | Issue | cpp:S134 | CRITICAL | OPEN | src/request_body_processor/json.cc | 156 | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements. | Production | Ja |
| AZ190QGTSTzC4JOHOn-M | Issue | cpp:S134 | CRITICAL | OPEN | src/request_body_processor/json.cc | 163 | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements. | Production | Ja |
| AZ190QGTSTzC4JOHOn-E | Issue | cpp:S4144 | MAJOR | CLOSED | src/request_body_processor/json.cc |  | Update this method so that its implementation is not identical to on_end_object. | Production | Nein |
| AZ190QGTSTzC4JOHOn-N | Issue | cpp:S1155 | MINOR | CLOSED | src/request_body_processor/json.cc |  | Use "empty()" to check whether the container is empty or not. | Production | Nein |
| AZ190QGTSTzC4JOHOn-O | Issue | cpp:S1155 | MINOR | CLOSED | src/request_body_processor/json.cc |  | Use "empty()" to check whether the container is empty or not. | Production | Nein |
| AZ190QEtSTzC4JOHOn9t | Issue | cpp:S3624 | CRITICAL | OPEN | src/request_body_processor/json.h | 52 | Customize this class' copy constructor to participate in resource management. Customize or delete its copy assignment operator. Also consider whether move operations should be customized. | Production | Ja |
| AZ190QF1STzC4JOHOn9- | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Concatenate this namespace with the nested one. | Production | Nein |
| AZ190QF1STzC4JOHOn-A | Issue | cpp:S1172 | MAJOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Remove the unused parameter "options", make it unnamed, or declare it "[[maybe_unused]]". | Production | Nein |
| AZ190QF1STzC4JOHOn-B | Issue | cpp:S995 | MINOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *". | Production | Nein |
| AZ190QF1STzC4JOHOn9_ | Issue | cpp:S6009 | MINOR | CLOSED | src/request_body_processor/json_adapter.cc |  | Replace this const reference to "std::string" by a "std::string_view". | Production | Nein |
| AZ190QF8STzC4JOHOn-C | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json_adapter.h |  | Concatenate this namespace with the nested one. | Production | Nein |
| AZ190QGbSTzC4JOHOn-P | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json_backend.h |  | Concatenate this namespace with the nested one. | Production | Nein |
| AZ190QFsSTzC4JOHOn9v | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Concatenate this namespace with the nested one. | Production | Nein |
| AZ190QFsSTzC4JOHOn9w | Issue | cpp:S3776 | CRITICAL | OPEN | src/request_body_processor/json_backend_jsoncons.cc | 106 | Refactor this function to reduce its Cognitive Complexity from 33 to the 25 allowed. | Production | Ja |
| AZ190QFsSTzC4JOHOn9u | Issue | cpp:S3562 | MAJOR | OPEN | src/request_body_processor/json_backend_jsoncons.cc | 202 | 4 enumeration values not handled in switch: 'int64_value', 'uint64_value', 'half_value'... | Production | Ja |
| AZ190QFsSTzC4JOHOn9z | Issue | cpp:S3776 | CRITICAL | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Refactor this function to reduce its Cognitive Complexity from 41 to the 25 allowed. | Production | Nein |
| AZ190QFsSTzC4JOHOn92 | Issue | cpp:S1121 | MAJOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Extract the assignment from this expression. | Production | Nein |
| AZ190QFsSTzC4JOHOn9x | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Use the init-statement to declare "current" inside the if statement. | Production | Nein |
| AZ190QFsSTzC4JOHOn93 | Issue | cpp:S1121 | MAJOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Extract the assignment from this expression. | Production | Nein |
| AZ190QFsSTzC4JOHOn94 | Issue | cpp:S3776 | CRITICAL | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Refactor this function to reduce its Cognitive Complexity from 43 to the 25 allowed. | Production | Nein |
| AZ190QFsSTzC4JOHOn90 | Issue | cpp:S134 | CRITICAL | OPEN | src/request_body_processor/json_backend_jsoncons.cc | 379 | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements. | Production | Ja |
| AZ190QFsSTzC4JOHOn9y | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Use the init-statement to declare "escaped" inside the if statement. | Production | Nein |
| AZ190QFsSTzC4JOHOn91 | Issue | cpp:S134 | CRITICAL | OPEN | src/request_body_processor/json_backend_jsoncons.cc | 388 | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements. | Production | Ja |
| AZ190QFsSTzC4JOHOn97 | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Use the init-statement to declare "sync_detail" inside the if statement. | Production | Nein |
| AZ190QFsSTzC4JOHOn96 | Issue | cpp:S6009 | MINOR | OPEN | src/request_body_processor/json_backend_jsoncons.cc | 546 | Replace this const reference to "std::string" by a "std::string_view". | Production | Ja |
| AZ190QFsSTzC4JOHOn95 | Issue | cpp:S6004 | MINOR | CLOSED | src/request_body_processor/json_backend_jsoncons.cc |  | Use the init-statement to declare "end" inside the if statement. | Production | Nein |
| AZ190QFsSTzC4JOHOn98 | Issue | cpp:S3776 | CRITICAL | OPEN | src/request_body_processor/json_backend_jsoncons.cc | 577 | Refactor this function to reduce its Cognitive Complexity from 48 to the 25 allowed. | Production | Ja |
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
| AZ190QA8STzC4JOHOn9k | Issue | cpp:S5945 | MAJOR | OPEN | src/utils/json_writer.cc | 155 | Use "std::string" instead of a C-style char array. | Production | Ja |
| AZ190QA8STzC4JOHOn9l | Issue | cpp:S3628 | MINOR | CLOSED | src/utils/json_writer.cc |  | Convert this string literal to a raw string literal. | Production | Nein |
| AZ190QA8STzC4JOHOn9m | Issue | cpp:S3628 | MINOR | CLOSED | src/utils/json_writer.cc |  | Convert this string literal to a raw string literal. | Production | Nein |
| AZ190QA8STzC4JOHOn9n | Issue | cpp:S6022 | MAJOR | OPEN | src/utils/json_writer.cc | 183 | Use "std::byte" for byte-oriented data manipulation. | Production | Ja |
| AZ190QA8STzC4JOHOn9o | Issue | cpp:S6022 | MAJOR | OPEN | src/utils/json_writer.cc | 184 | Use "std::byte" for byte-oriented data manipulation. | Production | Ja |
| AZ190QD4STzC4JOHOn9p | Issue | cpp:S5812 | MINOR | CLOSED | src/utils/json_writer.h |  | Concatenate this namespace with the nested one. | Production | Nein |
| AZ190QIVSTzC4JOHOn-f | Issue | cpp:S5812 | MINOR | CLOSED | test/common/json.h |  | Concatenate this namespace with the nested one. | Test | Nein |
| AZ190QIVSTzC4JOHOn-g | Issue | cpp:S2807 | MAJOR | OPEN | test/common/json.h | 78 | Make this member overloaded operator a hidden friend. | Test | Ja |
| AZ190QIVSTzC4JOHOn-h | Issue | cpp:S2807 | MAJOR | OPEN | test/common/json.h | 123 | Make this member overloaded operator a hidden friend. | Test | Ja |
| AZ190QIVSTzC4JOHOn-i | Issue | cpp:S1181 | MAJOR | OPEN | test/common/json.h | 219 | Catch a more specific exception instead of a generic one. | Test | Ja |
| AZ190QIVSTzC4JOHOn-j | Issue | cpp:S995 | MINOR | OPEN | test/common/json.h | 232 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Test | Ja |
| AZ190QIVSTzC4JOHOn-k | Issue | cpp:S995 | MINOR | OPEN | test/common/json.h | 242 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Test | Ja |
| AZ190QIVSTzC4JOHOn-l | Issue | cpp:S995 | MINOR | OPEN | test/common/json.h | 252 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Test | Ja |
| AZ190QIVSTzC4JOHOn-m | Issue | cpp:S995 | MINOR | OPEN | test/common/json.h | 262 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Test | Ja |
| AZ190QIVSTzC4JOHOn-n | Issue | cpp:S995 | MINOR | OPEN | test/common/json.h | 269 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Test | Ja |
| AZ190QIVSTzC4JOHOn-o | Issue | cpp:S995 | MINOR | OPEN | test/common/json.h | 279 | Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *". | Test | Ja |
| AZ190QIVSTzC4JOHOn-p | Issue | cpp:S1181 | MAJOR | OPEN | test/common/json.h | 309 | Catch a more specific exception instead of a generic one. | Test | Ja |
| AZ190QIVSTzC4JOHOn-q | Issue | cpp:S1181 | MAJOR | OPEN | test/common/json.h | 321 | Catch a more specific exception instead of a generic one. | Test | Ja |
| AZ190QIVSTzC4JOHOn-r | Issue | cpp:S1181 | MAJOR | OPEN | test/common/json.h | 333 | Catch a more specific exception instead of a generic one. | Test | Ja |
| AZ190QI2STzC4JOHOn-s | Issue | cpp:S134 | CRITICAL | OPEN | test/common/modsecurity_test.cc | 89 | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements. | Test | Ja |
| AZ190QKvSTzC4JOHOn-x | Issue | cpp:S5415 | MAJOR | CLOSED | test/regression/regression_test.cc |  | The result of "std::move" should not be passed as a const reference. | Test | Nein |
| AZ190QKvSTzC4JOHOn-y | Issue | cpp:S5415 | MAJOR | CLOSED | test/regression/regression_test.cc |  | The result of "std::move" should not be passed as a const reference. | Test | Nein |
| AZ190QKvSTzC4JOHOn-z | Issue | cpp:S5415 | MAJOR | CLOSED | test/regression/regression_test.cc |  | The result of "std::move" should not be passed as a const reference. | Test | Nein |
| AZ190QKvSTzC4JOHOn-0 | Issue | cpp:S5415 | MAJOR | CLOSED | test/regression/regression_test.cc |  | The result of "std::move" should not be passed as a const reference. | Test | Nein |
| AZ190QKvSTzC4JOHOn-1 | Issue | cpp:S5415 | MAJOR | CLOSED | test/regression/regression_test.cc |  | The result of "std::move" should not be passed as a const reference. | Test | Nein |
| AZ190QKvSTzC4JOHOn-2 | Issue | cpp:S5415 | MAJOR | CLOSED | test/regression/regression_test.cc |  | The result of "std::move" should not be passed as a const reference. | Test | Nein |
| AZ190QKvSTzC4JOHOn-3 | Issue | cpp:S5415 | MAJOR | CLOSED | test/regression/regression_test.cc |  | The result of "std::move" should not be passed as a const reference. | Test | Nein |
| AZ190QKvSTzC4JOHOn-4 | Issue | cpp:S5817 | MAJOR | OPEN | test/regression/regression_test.cc | 431 | This function should be declared "const". | Test | Ja |
| AZ190QKvSTzC4JOHOn-v | Issue | cpp:S5274 | MAJOR | CLOSED | test/regression/regression_test.cc |  | moving a temporary object prevents copy elision | Test | Nein |
| AZ190QKvSTzC4JOHOn-w | Issue | cpp:S5274 | MAJOR | CLOSED | test/regression/regression_test.cc |  | moving a temporary object prevents copy elision | Test | Nein |
| AZ190QKvSTzC4JOHOn-5 | Issue | cpp:S1481 | MINOR | CLOSED | test/regression/regression_test.cc |  | Remove the unused lambda capture "writer". | Test | Nein |
| AZ190QJKSTzC4JOHOn-t | Issue | cpp:S5415 | MAJOR | CLOSED | test/unit/unit_test.cc |  | The result of "std::move" should not be passed as a const reference. | Test | Nein |
| AZ190QJYSTzC4JOHOn-u | Issue | cpp:S836 | MAJOR | CLOSED | test/unit/unit_test.h |  | Value assigned to field 'ret' in implicit constructor is garbage or undefined | Test | Nein |
| AZ190QIESTzC4JOHOn-d | Issue | cpp:S886 | MINOR | OPEN | src/modsecurity.cc | 232 | Refactor this loop so that it is less error-prone. | Production | Ja |
| AZ190QIESTzC4JOHOn-e | Issue | cpp:S886 | MINOR | OPEN | src/modsecurity.cc | 288 | Refactor this loop so that it is less error-prone. | Production | Ja |
| AZ190QGTSTzC4JOHOn-F | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json.cc |  | Concatenate this namespace with the nested one. | Production | Nein |
| AZ190QEtSTzC4JOHOn9s | Issue | cpp:S5812 | MINOR | CLOSED | src/request_body_processor/json.h |  | Concatenate this namespace with the nested one. | Production | Nein |
| AZ1-dn4cXISY38E6Txog | Issue | cpp:S4962 | CRITICAL | CLOSED | src/utils/msc_tree.cc |  | Use the "nullptr" literal. | Production | Nein |
| AZ1-dn4cXISY38E6Txoi | Issue | cpp:S4962 | CRITICAL | CLOSED | src/utils/msc_tree.cc |  | Use the "nullptr" literal. | Production | Nein |
| AZ1-dn4cXISY38E6Txoj | Issue | cpp:S4962 | CRITICAL | CLOSED | src/utils/msc_tree.cc |  | Use the "nullptr" literal. | Production | Nein |
| AZ1-dn4cXISY38E6Txok | Issue | cpp:S4962 | CRITICAL | CLOSED | src/utils/msc_tree.cc |  | Use the "nullptr" literal. | Production | Nein |
| AZ1-dn4cXISY38E6Txol | Issue | cpp:S4962 | CRITICAL | CLOSED | src/utils/msc_tree.cc |  | Use the "nullptr" literal. | Production | Nein |

# Tabelle: Nur offene Befunde
| ID | Kategorie | Regelcode | Severity | Datei | Zeile | Kurzbeschreibung | Konkreter Fix (technisch) | Risiko der Änderung |
|---|---|---|---|---|---:|---|---|---|
| AZ2HqeBsWym3B0O6okyS | Issue | cpp:S5952 | MINOR | test/benchmark/json_benchmark.cc | 71 | Add a using-declaration to this derived class to inherit the constructors of "runtime_error", and remove the ones you manually duplicated | Regelspezifische Refaktorierung gemäß Sonar-Regelbeschreibung umsetzen. | Niedrig |
| AZ2HqeBsWym3B0O6okyT | Issue | cpp:S6004 | MINOR | test/benchmark/json_benchmark.cc | 144 | Use the init-statement to declare "current" inside the if statement | Regelspezifische Refaktorierung gemäß Sonar-Regelbeschreibung umsetzen. | Niedrig |
| AZ2DWE24t-zbsGOGdN_K | Issue | cpp:S4998 | MAJOR | test/benchmark/json_benchmark.cc | 316 | Replace this use of "unique_ptr" by a raw pointer or a reference (possibly const) | Regelspezifische Refaktorierung gemäß Sonar-Regelbeschreibung umsetzen. | Mittel |
| AZ2DVDgODPiZK5yPV1-J | Issue | cpp:S1188 | MAJOR | test/regression/regression_test.cc | 235 | This lambda has 23 lines, which is greater than the 20 lines authorized | Regelspezifische Refaktorierung gemäß Sonar-Regelbeschreibung umsetzen. | Mittel |
| AZ2DR4Fykud7vHWq_QVC | Issue | cpp:S6009 | MINOR | src/request_body_processor/json_adapter.cc | 58 | Replace this const reference to "std::string" by a "std::string_view" | `const std::string&`-Parameter auf `std::string_view` umstellen. | Niedrig |
| AZ2DR4Fykud7vHWq_QVD | Issue | cpp:S995 | MINOR | src/request_body_processor/json_adapter.cc | 59 | Make the type of this parameter a pointer-to-const | Parameter als `const T*` bzw. `const T&` deklarieren. | Niedrig |
| AZ2DR4Fykud7vHWq_QVE | Issue | cpp:S995 | MINOR | src/request_body_processor/json_adapter.cc | 81 | Make the type of this parameter a reference-to-const | Parameter als `const T*` bzw. `const T&` deklarieren. | Niedrig |
| AZ2CwcnoK0fgB4uOpVK4 | Issue | cpp:S5817 | MAJOR | src/request_body_processor/json_backend_simdjson.cc | 450 | This function should be declared "const" | Methode als `const` markieren, wenn Objektzustand nicht geändert wird. | Mittel |
| AZ2CBI6Kkud7vHWq0tqj | Issue | cpp:S6022 | MAJOR | src/operators/validate_byte_range.cc | 156 | Use "std::byte" for byte-oriented data manipulation | Regelspezifische Refaktorierung gemäß Sonar-Regelbeschreibung umsetzen. | Mittel |
| AZ2CA_xuGCkM6OziEPeu | Issue | cpp:S4144 | MAJOR | test/unit/json_backend_depth_tests.cc | 50 | Update this method so that its implementation is not identical to on_key | Duplizierte Methode entfernen oder Implementierung eindeutig differenzieren. | Mittel |
| AZ2BthMEO-njQfcv_7WG | Issue | cpp:S3776 | CRITICAL | src/request_body_processor/json_backend_jsoncons.cc | 362 | Refactor this function to reduce its Cognitive Complexity from 41 to the 25 allowed | Funktion aufteilen, Guard-Clauses extrahieren, Hilfsfunktionen einführen bis Cognitive Complexity <= 25. | Mittel |
| AZ2BthMEO-njQfcv_7WJ | Issue | cpp:S3776 | CRITICAL | src/request_body_processor/json_backend_jsoncons.cc | 426 | Refactor this function to reduce its Cognitive Complexity from 43 to the 25 allowed | Funktion aufteilen, Guard-Clauses extrahieren, Hilfsfunktionen einführen bis Cognitive Complexity <= 25. | Mittel |
| AZ2BthMEO-njQfcv_7WL | Issue | cpp:S134 | CRITICAL | src/request_body_processor/json_backend_jsoncons.cc | 634 | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements | Verschachtelung reduzieren: frühzeitige Returns und Hilfsfunktionen. | Mittel |
| AZ1-dn-nXISY38E6Txop | Issue | cpp:S5025 | CRITICAL | src/request_body_processor/json.cc | 57 | Rewrite the code so that you no longer need this "delete" | Rohes `delete` durch `std::unique_ptr`/RAII ersetzen. | Mittel |
| AZ1-dn9hXISY38E6Txoo | Issue | cpp:S5421 | CRITICAL | src/request_body_processor/json_instrumentation.cc | 12 | Global variables should be const | Globale Variable als `const` oder kapseln (z.B. in Funktion/Singleton) und unveränderlich halten. | Mittel |
| AZ1-doCWXISY38E6TxpK | Issue | cpp:S6009 | MINOR | test/benchmark/json_benchmark.cc | 321 | Replace this const reference to "std::string" by a "std::string_view" | `const std::string&`-Parameter auf `std::string_view` umstellen. | Niedrig |
| AZ1-doCWXISY38E6TxpR | Issue | cpp:S6009 | MINOR | test/benchmark/json_benchmark.cc | 419 | Replace this const reference to "std::string" by a "std::string_view" | `const std::string&`-Parameter auf `std::string_view` umstellen. | Niedrig |
| AZ1-doCWXISY38E6TxpU | Issue | cpp:S6009 | MINOR | test/benchmark/json_benchmark.cc | 486 | Replace this const reference to "std::string" by a "std::string_view" | `const std::string&`-Parameter auf `std::string_view` umstellen. | Niedrig |
| AZ190QEMSTzC4JOHOn9q | Issue | cpp:S6022 | MAJOR | src/operators/validate_byte_range.cc | 72 | Use "std::byte" for byte-oriented data manipulation | Regelspezifische Refaktorierung gemäß Sonar-Regelbeschreibung umsetzen. | Mittel |
| AZ190QGTSTzC4JOHOn-D | Issue | cpp:S3230 | MAJOR | src/request_body_processor/json.cc | 79 | Do not use the constructor's initializer list for data member "m_data" | Member im Konstruktor-Body initialisieren statt Initializer-Liste für Zuweisung. | Mittel |
| AZ190QGTSTzC4JOHOn-H | Issue | cpp:S3776 | CRITICAL | src/request_body_processor/json.cc | 121 | Refactor this function to reduce its Cognitive Complexity from 37 to the 25 allowed | Funktion aufteilen, Guard-Clauses extrahieren, Hilfsfunktionen einführen bis Cognitive Complexity <= 25. | Mittel |
| AZ190QGTSTzC4JOHOn-I | Issue | cpp:S134 | CRITICAL | src/request_body_processor/json.cc | 135 | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements | Verschachtelung reduzieren: frühzeitige Returns und Hilfsfunktionen. | Mittel |
| AZ190QGTSTzC4JOHOn-J | Issue | cpp:S134 | CRITICAL | src/request_body_processor/json.cc | 142 | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements | Verschachtelung reduzieren: frühzeitige Returns und Hilfsfunktionen. | Mittel |
| AZ190QGTSTzC4JOHOn-K | Issue | cpp:S134 | CRITICAL | src/request_body_processor/json.cc | 149 | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements | Verschachtelung reduzieren: frühzeitige Returns und Hilfsfunktionen. | Mittel |
| AZ190QGTSTzC4JOHOn-L | Issue | cpp:S134 | CRITICAL | src/request_body_processor/json.cc | 156 | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements | Verschachtelung reduzieren: frühzeitige Returns und Hilfsfunktionen. | Mittel |
| AZ190QGTSTzC4JOHOn-M | Issue | cpp:S134 | CRITICAL | src/request_body_processor/json.cc | 163 | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements | Verschachtelung reduzieren: frühzeitige Returns und Hilfsfunktionen. | Mittel |
| AZ190QEtSTzC4JOHOn9t | Issue | cpp:S3624 | CRITICAL | src/request_body_processor/json.h | 52 | Customize this class' copy constructor to participate in resource management | Copy-/Move-Konstruktor und Assignment gemäß Rule of 5 korrekt definieren oder löschen. | Mittel |
| AZ190QFsSTzC4JOHOn9w | Issue | cpp:S3776 | CRITICAL | src/request_body_processor/json_backend_jsoncons.cc | 106 | Refactor this function to reduce its Cognitive Complexity from 33 to the 25 allowed | Funktion aufteilen, Guard-Clauses extrahieren, Hilfsfunktionen einführen bis Cognitive Complexity <= 25. | Mittel |
| AZ190QFsSTzC4JOHOn9u | Issue | cpp:S3562 | MAJOR | src/request_body_processor/json_backend_jsoncons.cc | 202 | 4 enumeration values not handled in switch: 'int64_value', 'uint64_value', 'half_value' | `switch` um fehlende Enum-Cases ergänzen (int64_value, uint64_value, half_value, byte_string_value). | Mittel |
| AZ190QFsSTzC4JOHOn90 | Issue | cpp:S134 | CRITICAL | src/request_body_processor/json_backend_jsoncons.cc | 379 | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements | Verschachtelung reduzieren: frühzeitige Returns und Hilfsfunktionen. | Mittel |
| AZ190QFsSTzC4JOHOn91 | Issue | cpp:S134 | CRITICAL | src/request_body_processor/json_backend_jsoncons.cc | 388 | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements | Verschachtelung reduzieren: frühzeitige Returns und Hilfsfunktionen. | Mittel |
| AZ190QFsSTzC4JOHOn96 | Issue | cpp:S6009 | MINOR | src/request_body_processor/json_backend_jsoncons.cc | 546 | Replace this const reference to "std::string" by a "std::string_view" | `const std::string&`-Parameter auf `std::string_view` umstellen. | Niedrig |
| AZ190QFsSTzC4JOHOn98 | Issue | cpp:S3776 | CRITICAL | src/request_body_processor/json_backend_jsoncons.cc | 577 | Refactor this function to reduce its Cognitive Complexity from 48 to the 25 allowed | Funktion aufteilen, Guard-Clauses extrahieren, Hilfsfunktionen einführen bis Cognitive Complexity <= 25. | Mittel |
| AZ190QA8STzC4JOHOn9k | Issue | cpp:S5945 | MAJOR | src/utils/json_writer.cc | 155 | Use "std::string" instead of a C-style char array | Regelspezifische Refaktorierung gemäß Sonar-Regelbeschreibung umsetzen. | Mittel |
| AZ190QA8STzC4JOHOn9n | Issue | cpp:S6022 | MAJOR | src/utils/json_writer.cc | 183 | Use "std::byte" for byte-oriented data manipulation | Regelspezifische Refaktorierung gemäß Sonar-Regelbeschreibung umsetzen. | Mittel |
| AZ190QA8STzC4JOHOn9o | Issue | cpp:S6022 | MAJOR | src/utils/json_writer.cc | 184 | Use "std::byte" for byte-oriented data manipulation | Regelspezifische Refaktorierung gemäß Sonar-Regelbeschreibung umsetzen. | Mittel |
| AZ190QIVSTzC4JOHOn-g | Issue | cpp:S2807 | MAJOR | test/common/json.h | 78 | Make this member overloaded operator a hidden friend | Operatorüberladung als hidden friend außerhalb der Klasse definieren. | Mittel |
| AZ190QIVSTzC4JOHOn-h | Issue | cpp:S2807 | MAJOR | test/common/json.h | 123 | Make this member overloaded operator a hidden friend | Operatorüberladung als hidden friend außerhalb der Klasse definieren. | Mittel |
| AZ190QIVSTzC4JOHOn-i | Issue | cpp:S1181 | MAJOR | test/common/json.h | 219 | Catch a more specific exception instead of a generic one | Spezifischere Exception-Typen catchen. | Mittel |
| AZ190QIVSTzC4JOHOn-j | Issue | cpp:S995 | MINOR | test/common/json.h | 232 | Make the type of this parameter a pointer-to-const | Parameter als `const T*` bzw. `const T&` deklarieren. | Niedrig |
| AZ190QIVSTzC4JOHOn-k | Issue | cpp:S995 | MINOR | test/common/json.h | 242 | Make the type of this parameter a pointer-to-const | Parameter als `const T*` bzw. `const T&` deklarieren. | Niedrig |
| AZ190QIVSTzC4JOHOn-l | Issue | cpp:S995 | MINOR | test/common/json.h | 252 | Make the type of this parameter a pointer-to-const | Parameter als `const T*` bzw. `const T&` deklarieren. | Niedrig |
| AZ190QIVSTzC4JOHOn-m | Issue | cpp:S995 | MINOR | test/common/json.h | 262 | Make the type of this parameter a pointer-to-const | Parameter als `const T*` bzw. `const T&` deklarieren. | Niedrig |
| AZ190QIVSTzC4JOHOn-n | Issue | cpp:S995 | MINOR | test/common/json.h | 269 | Make the type of this parameter a pointer-to-const | Parameter als `const T*` bzw. `const T&` deklarieren. | Niedrig |
| AZ190QIVSTzC4JOHOn-o | Issue | cpp:S995 | MINOR | test/common/json.h | 279 | Make the type of this parameter a pointer-to-const | Parameter als `const T*` bzw. `const T&` deklarieren. | Niedrig |
| AZ190QIVSTzC4JOHOn-p | Issue | cpp:S1181 | MAJOR | test/common/json.h | 309 | Catch a more specific exception instead of a generic one | Spezifischere Exception-Typen catchen. | Mittel |
| AZ190QIVSTzC4JOHOn-q | Issue | cpp:S1181 | MAJOR | test/common/json.h | 321 | Catch a more specific exception instead of a generic one | Spezifischere Exception-Typen catchen. | Mittel |
| AZ190QIVSTzC4JOHOn-r | Issue | cpp:S1181 | MAJOR | test/common/json.h | 333 | Catch a more specific exception instead of a generic one | Spezifischere Exception-Typen catchen. | Mittel |
| AZ190QI2STzC4JOHOn-s | Issue | cpp:S134 | CRITICAL | test/common/modsecurity_test.cc | 89 | Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements | Verschachtelung reduzieren: frühzeitige Returns und Hilfsfunktionen. | Mittel |
| AZ190QKvSTzC4JOHOn-4 | Issue | cpp:S5817 | MAJOR | test/regression/regression_test.cc | 431 | This function should be declared "const" | Methode als `const` markieren, wenn Objektzustand nicht geändert wird. | Mittel |
| AZ190QIESTzC4JOHOn-d | Issue | cpp:S886 | MINOR | src/modsecurity.cc | 232 | Refactor this loop so that it is less error-prone | Regelspezifische Refaktorierung gemäß Sonar-Regelbeschreibung umsetzen. | Niedrig |
| AZ190QIESTzC4JOHOn-e | Issue | cpp:S886 | MINOR | src/modsecurity.cc | 288 | Refactor this loop so that it is less error-prone | Regelspezifische Refaktorierung gemäß Sonar-Regelbeschreibung umsetzen. | Niedrig |

# Tabelle: JSON-Dateien (nur offene)
| Datei | Anzahl offener Befunde | Regelcodes | Problem | Minimaler Fix |
|---|---:|---|---|---|
| src/request_body_processor/json.cc | 8 | cpp:S134, cpp:S3230, cpp:S3776, cpp:S5025 | Do not use the constructor's initializer list for data member "m_data"; Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements | Große Parser-Funktionen in kleinere Handler aufteilen. |
| src/request_body_processor/json.h | 1 | cpp:S3624 | Customize this class' copy constructor to participate in resource management | Regelspezifische Minimalanpassung durchführen. |
| src/request_body_processor/json_adapter.cc | 3 | cpp:S6009, cpp:S995 | Make the type of this parameter a pointer-to-const; Make the type of this parameter a reference-to-const | String-Referenzen in `std::string_view` ändern. |
| src/request_body_processor/json_backend_jsoncons.cc | 9 | cpp:S134, cpp:S3562, cpp:S3776, cpp:S6009 | 4 enumeration values not handled in switch: 'int64_value', 'uint64_value', 'half_value'; Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements | Große Parser-Funktionen in kleinere Handler aufteilen. |
| src/request_body_processor/json_backend_simdjson.cc | 1 | cpp:S5817 | This function should be declared "const" | Regelspezifische Minimalanpassung durchführen. |
| src/request_body_processor/json_instrumentation.cc | 1 | cpp:S5421 | Global variables should be const | Regelspezifische Minimalanpassung durchführen. |
| test/common/json.h | 12 | cpp:S1181, cpp:S2807, cpp:S995 | Catch a more specific exception instead of a generic one; Make the type of this parameter a pointer-to-const | Pointer/Referenzen const-korrekt machen. |
| test/unit/json_backend_depth_tests.cc | 1 | cpp:S4144 | Update this method so that its implementation is not identical to on_key | Regelspezifische Minimalanpassung durchführen. |

# Summenübersicht
- Gesamtanzahl aller Befunde (Issue + Hotspot + Gate): 208
- Gesamtanzahl Sonar-Issues: 207
- Anzahl offene Befunde (Issues+Hotspots): 52
- Anzahl geschlossene Befunde (Issues+Hotspots): 155
- Security Hotspots: offen 0, reviewed 0
- BLOCKER: 0
- CRITICAL: 29
- MAJOR: 104
- MINOR: 73
- INFO: 1

# Grenzen der Analyse
- Sonar-Issues, Quality Gate, Measures und Hotspots wurden live über SonarCloud API mit `pullRequest=3540` abgerufen.
- Eine vollständige Issue-Liste wurde technisch abgerufen (`total=207`, `ps=500`), daher **keine** Teilmengenkennzeichnung nötig.