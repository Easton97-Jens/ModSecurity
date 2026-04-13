# Aktueller Commit
- Analyse basiert auf Commit: `bbde416a92212f1fc4a975361e73a3b07356a526`
- PR updated_at: `2026-04-13T16:33:35Z`
- Commits im PR laut API: 10
- Zuletzt bekannter Commit (letzter lokaler PR-Stand vor dieser Neuanalyse): `5dd7b3539977de85f0a2aebd2cb28b9ea1640211`
- Neue Commits seit zuletzt bekanntem Commit:
  - `c0f166800cb72a5289015dfc8859c9e00eb06ba3` | 2026-04-10T18:58:34Z | Pin simdjson to v4.6.1
  - `98fd8f715a4bfdb56729f12da111a05770e9436b` | 2026-04-10T20:03:44Z | Pin jsoncons to v1.6.0
  - `a30e0dacc720df5b28595194d2d0747200165b0b` | 2026-04-11T04:16:01Z | remove yajl
  - `7681a8e66b2aa38f2aa557bd38abc74262ab3e9e` | 2026-04-11T16:01:01Z | test error corrected
  - `82ca699a9aa220b0f2e5c957b01560d7f408e965` | 2026-04-11T16:37:54Z | add analysis
  - `c8506d3e5fc4a75500aa4086c2c67fe5a9f3f721` | 2026-04-11T17:37:34Z | test error corrected
  - `d0589143e206bd0c830e492cf7812ae1acc334a6` | 2026-04-11T21:28:19Z | Remove duplicate code, performance optimization
  - `ec9dce9afcf45440731e9496cce69d6875abb22d` | 2026-04-12T10:29:07Z | add build ordner
  - `c13f7d10f3db1d1bb80380d2882e0afbdbe2d5b8` | 2026-04-12T11:15:59Z | performance optimization
  - `bbde416a92212f1fc4a975361e73a3b07356a526` | 2026-04-12T19:05:19Z | fix windows

# Quality Gate (neu)
- Status: `OK`
- Bedingungen:
  - `new_reliability_rating`: status=OK, actual=1, threshold=1, comparator=GT
  - `new_security_rating`: status=OK, actual=1, threshold=1, comparator=GT
  - `new_maintainability_rating`: status=OK, actual=1, threshold=1, comparator=GT
  - `new_duplicated_lines_density`: status=OK, actual=1.3, threshold=3, comparator=GT
  - `new_security_hotspots_reviewed`: status=OK, actual=100.0, threshold=100, comparator=LT

# Measures (neu)
- Aktuelle Measure-Werte (PR-spezifisch):
  - `alert_status` = OK
  - `new_security_hotspots` = 0
  - `new_critical_violations` = 17
  - `new_violations` = 52
  - `new_blocker_violations` = 0
  - `new_security_hotspots_reviewed` = 100.0
  - `security_hotspots` = 0
  - `new_duplicated_lines_density` = 1.2939325056828117

# Issues (neu)
- Vollständigkeit:
  - API total: 207
  - Geladene Issues (p1..p3): 207
  - Eindeutige Issue-Keys: 207
- Wenn diese Zahlen abweichen würden, wäre die Liste unvollständig.

|#|Regelcode|Datei|Zeile|Severity|Status|Beschreibung|
|---:|---|---|---:|---|---|---|
|1|cpp:S5952|test/benchmark/json_benchmark.cc|71|MINOR|OPEN|Add a using-declaration to this derived class to inherit the constructors of "runtime_error", and remove the ones you manually duplicated. Note that this may add other constructors to your derived class.|
|2|cpp:S6004|test/benchmark/json_benchmark.cc|144|MINOR|OPEN|Use the init-statement to declare "current" inside the if statement.|
|3|cpp:S3776|test/benchmark/json_benchmark.cc||CRITICAL|CLOSED|Refactor this function to reduce its Cognitive Complexity from 33 to the 25 allowed.|
|4|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|5|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|6|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|7|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|8|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|9|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|10|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|11|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|12|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|13|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|14|cpp:S4998|test/benchmark/json_benchmark.cc|316|MAJOR|OPEN|Replace this use of "unique_ptr" by a raw pointer or a reference (possibly const).|
|15|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|16|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|17|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|18|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|19|cpp:S1188|test/regression/regression_test.cc|235|MAJOR|OPEN|This lambda has 23 lines, which is greater than the 20 lines authorized. Split it into several lambdas or functions, or make it a named function.|
|20|cpp:S6009|src/request_body_processor/json_adapter.cc|58|MINOR|OPEN|Replace this const reference to "std::string" by a "std::string_view".|
|21|cpp:S995|src/request_body_processor/json_adapter.cc|59|MINOR|OPEN|Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *".|
|22|cpp:S995|src/request_body_processor/json_adapter.cc|81|MINOR|OPEN|Make the type of this parameter a reference-to-const. The current type of "input" is "std::string &".|
|23|cpp:S995|src/request_body_processor/json_adapter.cc||MINOR|CLOSED|Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *".|
|24|cpp:S1121|src/request_body_processor/json_backend_jsoncons.cc||MAJOR|CLOSED|Extract the assignment from this expression.|
|25|cpp:S995|src/request_body_processor/json_adapter.cc||MINOR|CLOSED|Make the type of this parameter a reference-to-const. The current type of "input" is "std::string &".|
|26|cpp:S995|src/request_body_processor/json_adapter.cc||MINOR|CLOSED|Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *".|
|27|cpp:S1172|src/request_body_processor/json_adapter.cc||MAJOR|CLOSED|Remove the unused parameter "options", make it unnamed, or declare it "[[maybe_unused]]".|
|28|cpp:S6004|src/request_body_processor/json_backend_simdjson.cc||MINOR|CLOSED|Use the init-statement to declare "tail" inside the if statement.|
|29|cpp:S1117|src/request_body_processor/json_backend_simdjson.cc||MAJOR|CLOSED|Declaration shadows a local variable "result" in the outer scope.|
|30|cpp:S1117|src/request_body_processor/json_backend_simdjson.cc||MAJOR|CLOSED|Declaration shadows a local variable "result" in the outer scope.|
|31|cpp:S5817|src/request_body_processor/json_backend_simdjson.cc|450|MAJOR|OPEN|This function should be declared "const".|
|32|cpp:S6004|test/unit/json_backend_depth_tests.cc||MINOR|CLOSED|Use the init-statement to declare "result" inside the if statement.|
|33|cpp:S6004|test/unit/json_backend_depth_tests.cc||MINOR|CLOSED|Use the init-statement to declare "result" inside the if statement.|
|34|cpp:S6004|test/unit/json_backend_depth_tests.cc||MINOR|CLOSED|Use the init-statement to declare "result" inside the if statement.|
|35|cpp:S5945|test/unit/json_backend_depth_tests.cc||MAJOR|CLOSED|Use "std::array" or "std::vector" instead of a C-style array.|
|36|cpp:S3628|test/unit/json_backend_depth_tests.cc||MINOR|CLOSED|Convert this string literal to a raw string literal.|
|37|cpp:S6022|src/operators/validate_byte_range.cc|156|MAJOR|OPEN|Use "std::byte" for byte-oriented data manipulation.|
|38|shelldre:S7688|test/run-json-backend-matrix.sh||MAJOR|CLOSED|Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich.|
|39|cpp:S5812|test/unit/json_backend_depth_tests.cc||MINOR|CLOSED|Concatenate this namespace with the nested one.|
|40|cpp:S4144|test/unit/json_backend_depth_tests.cc|50|MAJOR|OPEN|Update this method so that its implementation is not identical to on_key.|
|41|cpp:S6004|test/unit/json_backend_depth_tests.cc||MINOR|CLOSED|Use the init-statement to declare "result" inside the if statement.|
|42|cpp:S6004|test/unit/json_backend_depth_tests.cc||MINOR|CLOSED|Use the init-statement to declare "result" inside the if statement.|
|43|cpp:S3776|src/request_body_processor/json_backend_jsoncons.cc|362|CRITICAL|OPEN|Refactor this function to reduce its Cognitive Complexity from 41 to the 25 allowed.|
|44|cpp:S1121|src/request_body_processor/json_backend_jsoncons.cc||MAJOR|CLOSED|Extract the assignment from this expression.|
|45|cpp:S1121|src/request_body_processor/json_backend_jsoncons.cc||MAJOR|CLOSED|Extract the assignment from this expression.|
|46|cpp:S3776|src/request_body_processor/json_backend_jsoncons.cc|426|CRITICAL|OPEN|Refactor this function to reduce its Cognitive Complexity from 43 to the 25 allowed.|
|47|cpp:S6004|src/request_body_processor/json_backend_jsoncons.cc||MINOR|CLOSED|Use the init-statement to declare "decoded_number" inside the if statement.|
|48|cpp:S134|src/request_body_processor/json_backend_jsoncons.cc|634|CRITICAL|OPEN|Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements.|
|49|cpp:S1135|src/request_body_processor/json_backend_simdjson.cc||INFO|CLOSED|Complete the task associated to this "TODO" comment.|
|50|cpp:S5025|src/request_body_processor/json.cc|57|CRITICAL|OPEN|Rewrite the code so that you no longer need this "delete".|
|51|cpp:S5827|src/request_body_processor/json.cc||MAJOR|CLOSED|Replace the redundant type with "auto".|
|52|cpp:S5812|src/request_body_processor/json_instrumentation.cc||MINOR|CLOSED|Concatenate this namespace with the nested one.|
|53|cpp:S5421|src/request_body_processor/json_instrumentation.cc|12|CRITICAL|OPEN|Global variables should be const.|
|54|cpp:S5812|src/request_body_processor/json_instrumentation.h||MINOR|CLOSED|Concatenate this namespace with the nested one.|
|55|cpp:S4962|src/utils/msc_tree.cc||CRITICAL|CLOSED|Use the "nullptr" literal.|
|56|cpp:S5421|test/benchmark/json_benchmark.cc||CRITICAL|CLOSED|Global pointers should be const at every level.|
|57|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|58|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|59|cpp:S886|test/benchmark/json_benchmark.cc||MINOR|CLOSED|Refactor this loop so that it is less error-prone.|
|60|cpp:S6004|test/benchmark/json_benchmark.cc||MINOR|CLOSED|Use the init-statement to declare "output_format" inside the if statement.|
|61|cpp:S6004|test/benchmark/json_benchmark.cc||MINOR|CLOSED|Use the init-statement to declare "is_invalid_scenario" inside the if statement.|
|62|cpp:S5945|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Use "std::array" or "std::vector" instead of a C-style array.|
|63|cpp:S5945|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Use "std::array" or "std::vector" instead of a C-style array.|
|64|cpp:S7127|test/benchmark/json_benchmark.cc||CRITICAL|CLOSED|Use "std::size" to get the size of this array.|
|65|cpp:S6009|test/benchmark/json_benchmark.cc|321|MINOR|OPEN|Replace this const reference to "std::string" by a "std::string_view".|
|66|cpp:S7121|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Remove this redundant call to "c_str" when initializing a const "std::string" reference parameter.|
|67|cpp:S6004|test/benchmark/json_benchmark.cc||MINOR|CLOSED|Use the init-statement to declare "parse_error" inside the if statement.|
|68|cpp:S3628|test/benchmark/json_benchmark.cc||MINOR|CLOSED|Convert this string literal to a raw string literal.|
|69|cpp:S3628|test/benchmark/json_benchmark.cc||MINOR|CLOSED|Convert this string literal to a raw string literal.|
|70|cpp:S6009|test/benchmark/json_benchmark.cc|419|MINOR|OPEN|Replace this const reference to "std::string" by a "std::string_view".|
|71|cpp:S6004|test/benchmark/json_benchmark.cc||MINOR|CLOSED|Use the init-statement to declare "rules_path" inside the if statement.|
|72|cpp:S6009|test/benchmark/json_benchmark.cc|486|MINOR|OPEN|Replace this const reference to "std::string" by a "std::string_view".|
|73|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|74|cpp:S3776|test/benchmark/json_benchmark.cc||CRITICAL|CLOSED|Refactor this function to reduce its Cognitive Complexity from 33 to the 25 allowed.|
|75|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|76|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|77|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|78|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|79|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|80|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|81|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|82|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|83|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|84|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|85|cpp:S4998|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Replace this use of "unique_ptr" by a raw pointer or a reference (possibly const).|
|86|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|87|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|88|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|89|cpp:S112|test/benchmark/json_benchmark.cc||MAJOR|CLOSED|Define and throw a dedicated exception instead of using a generic one.|
|90|shelldre:S7682|test/benchmark/run-json-benchmarks.sh||MAJOR|CLOSED|Add an explicit return statement at the end of the function.|
|91|shelldre:S7688|test/benchmark/run-json-benchmarks.sh||MAJOR|CLOSED|Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich.|
|92|shelldre:S7688|test/benchmark/run-json-benchmarks.sh||MAJOR|CLOSED|Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich.|
|93|shelldre:S7688|test/benchmark/run-json-benchmarks.sh||MAJOR|CLOSED|Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich.|
|94|shelldre:S7688|test/benchmark/run-json-benchmarks.sh||MAJOR|CLOSED|Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich.|
|95|shelldre:S7688|test/benchmark/run-json-benchmarks.sh||MAJOR|CLOSED|Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich.|
|96|shelldre:S7688|test/benchmark/run-json-benchmarks.sh||MAJOR|CLOSED|Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich.|
|97|shelldre:S7688|test/benchmark/run-json-benchmarks.sh||MAJOR|CLOSED|Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich.|
|98|shelldre:S7682|test/run-json-backend-matrix.sh||MAJOR|CLOSED|Add an explicit return statement at the end of the function.|
|99|shelldre:S7688|test/run-json-backend-matrix.sh||MAJOR|CLOSED|Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich.|
|100|shelldre:S7688|test/run-json-backend-matrix.sh||MAJOR|CLOSED|Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich.|
|101|shelldre:S7688|test/run-json-backend-matrix.sh||MAJOR|CLOSED|Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich.|
|102|shelldre:S7688|test/run-json-backend-matrix.sh||MAJOR|CLOSED|Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich.|
|103|shelldre:S7688|test/run-json-backend-matrix.sh||MAJOR|CLOSED|Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich.|
|104|shelldre:S7682|test/run-json-backend-matrix.sh||MAJOR|CLOSED|Add an explicit return statement at the end of the function.|
|105|shelldre:S7688|test/run-json-backend-matrix.sh||MAJOR|CLOSED|Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich.|
|106|shelldre:S7688|test/run-json-backend-matrix.sh||MAJOR|CLOSED|Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich.|
|107|shelldre:S7688|test/run-json-backend-matrix.sh||MAJOR|CLOSED|Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich.|
|108|shelldre:S7688|test/run-json-backend-matrix.sh||MAJOR|CLOSED|Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich.|
|109|shelldre:S7688|test/run-json-backend-matrix.sh||MAJOR|CLOSED|Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich.|
|110|shelldre:S7688|test/run-json-backend-matrix.sh||MAJOR|CLOSED|Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich.|
|111|shelldre:S7688|test/run-json-backend-matrix.sh||MAJOR|CLOSED|Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich.|
|112|shelldre:S7688|test/run-json-backend-matrix.sh||MAJOR|CLOSED|Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich.|
|113|shelldre:S7688|test/run-json-backend-matrix.sh||MAJOR|CLOSED|Use '[[' instead of '[' for conditional tests. The '[[' construct is safer and more feature-rich.|
|114|cpp:S6022|src/operators/validate_byte_range.cc|72|MAJOR|OPEN|Use "std::byte" for byte-oriented data manipulation.|
|115|cpp:S6004|src/operators/validate_byte_range.cc||MINOR|CLOSED|Use the init-statement to declare "token" inside the if statement.|
|116|cpp:S3230|src/request_body_processor/json.cc|79|MAJOR|OPEN|Do not use the constructor's initializer list for data member "m_data". Use the in-class initializer instead.|
|117|cpp:S3776|src/request_body_processor/json.cc|121|CRITICAL|OPEN|Refactor this function to reduce its Cognitive Complexity from 37 to the 25 allowed.|
|118|cpp:S6004|src/request_body_processor/json.cc||MINOR|CLOSED|Use the init-statement to declare "result" inside the if statement.|
|119|cpp:S134|src/request_body_processor/json.cc|135|CRITICAL|OPEN|Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements.|
|120|cpp:S134|src/request_body_processor/json.cc|142|CRITICAL|OPEN|Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements.|
|121|cpp:S134|src/request_body_processor/json.cc|149|CRITICAL|OPEN|Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements.|
|122|cpp:S134|src/request_body_processor/json.cc|156|CRITICAL|OPEN|Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements.|
|123|cpp:S134|src/request_body_processor/json.cc|163|CRITICAL|OPEN|Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements.|
|124|cpp:S4144|src/request_body_processor/json.cc||MAJOR|CLOSED|Update this method so that its implementation is not identical to on_end_object.|
|125|cpp:S1155|src/request_body_processor/json.cc||MINOR|CLOSED|Use "empty()" to check whether the container is empty or not.|
|126|cpp:S1155|src/request_body_processor/json.cc||MINOR|CLOSED|Use "empty()" to check whether the container is empty or not.|
|127|cpp:S3624|src/request_body_processor/json.h|52|CRITICAL|OPEN|Customize this class' copy constructor to participate in resource management. Customize or delete its copy assignment operator. Also consider whether move operations should be customized.|
|128|cpp:S5812|src/request_body_processor/json_adapter.cc||MINOR|CLOSED|Concatenate this namespace with the nested one.|
|129|cpp:S1172|src/request_body_processor/json_adapter.cc||MAJOR|CLOSED|Remove the unused parameter "options", make it unnamed, or declare it "[[maybe_unused]]".|
|130|cpp:S995|src/request_body_processor/json_adapter.cc||MINOR|CLOSED|Make the type of this parameter a pointer-to-const. The current type of "sink" is "class modsecurity::RequestBodyProcessor::JsonEventSink *".|
|131|cpp:S6009|src/request_body_processor/json_adapter.cc||MINOR|CLOSED|Replace this const reference to "std::string" by a "std::string_view".|
|132|cpp:S5812|src/request_body_processor/json_adapter.h||MINOR|CLOSED|Concatenate this namespace with the nested one.|
|133|cpp:S5812|src/request_body_processor/json_backend.h||MINOR|CLOSED|Concatenate this namespace with the nested one.|
|134|cpp:S5812|src/request_body_processor/json_backend_jsoncons.cc||MINOR|CLOSED|Concatenate this namespace with the nested one.|
|135|cpp:S3776|src/request_body_processor/json_backend_jsoncons.cc|106|CRITICAL|OPEN|Refactor this function to reduce its Cognitive Complexity from 33 to the 25 allowed.|
|136|cpp:S3562|src/request_body_processor/json_backend_jsoncons.cc|202|MAJOR|OPEN|4 enumeration values not handled in switch: 'int64_value', 'uint64_value', 'half_value'...|
|137|cpp:S3776|src/request_body_processor/json_backend_jsoncons.cc||CRITICAL|CLOSED|Refactor this function to reduce its Cognitive Complexity from 41 to the 25 allowed.|
|138|cpp:S1121|src/request_body_processor/json_backend_jsoncons.cc||MAJOR|CLOSED|Extract the assignment from this expression.|
|139|cpp:S6004|src/request_body_processor/json_backend_jsoncons.cc||MINOR|CLOSED|Use the init-statement to declare "current" inside the if statement.|
|140|cpp:S1121|src/request_body_processor/json_backend_jsoncons.cc||MAJOR|CLOSED|Extract the assignment from this expression.|
|141|cpp:S3776|src/request_body_processor/json_backend_jsoncons.cc||CRITICAL|CLOSED|Refactor this function to reduce its Cognitive Complexity from 43 to the 25 allowed.|
|142|cpp:S134|src/request_body_processor/json_backend_jsoncons.cc|379|CRITICAL|OPEN|Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements.|
|143|cpp:S6004|src/request_body_processor/json_backend_jsoncons.cc||MINOR|CLOSED|Use the init-statement to declare "escaped" inside the if statement.|
|144|cpp:S134|src/request_body_processor/json_backend_jsoncons.cc|388|CRITICAL|OPEN|Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements.|
|145|cpp:S6004|src/request_body_processor/json_backend_jsoncons.cc||MINOR|CLOSED|Use the init-statement to declare "sync_detail" inside the if statement.|
|146|cpp:S6009|src/request_body_processor/json_backend_jsoncons.cc|546|MINOR|OPEN|Replace this const reference to "std::string" by a "std::string_view".|
|147|cpp:S6004|src/request_body_processor/json_backend_jsoncons.cc||MINOR|CLOSED|Use the init-statement to declare "end" inside the if statement.|
|148|cpp:S3776|src/request_body_processor/json_backend_jsoncons.cc|577|CRITICAL|OPEN|Refactor this function to reduce its Cognitive Complexity from 48 to the 25 allowed.|
|149|cpp:S6004|src/request_body_processor/json_backend_jsoncons.cc||MINOR|CLOSED|Use the init-statement to declare "result" inside the if statement.|
|150|cpp:S5812|src/request_body_processor/json_backend_simdjson.cc||MINOR|CLOSED|Concatenate this namespace with the nested one.|
|151|cpp:S6004|src/request_body_processor/json_backend_simdjson.cc||MINOR|CLOSED|Use the init-statement to declare "sink_status" inside the if statement.|
|152|cpp:S6004|src/request_body_processor/json_backend_simdjson.cc||MINOR|CLOSED|Use the init-statement to declare "sink_status" inside the if statement.|
|153|cpp:S6004|src/request_body_processor/json_backend_simdjson.cc||MINOR|CLOSED|Use the init-statement to declare "sink_status" inside the if statement.|
|154|cpp:S6004|src/request_body_processor/json_backend_simdjson.cc||MINOR|CLOSED|Use the init-statement to declare "sink_status" inside the if statement.|
|155|cpp:S6004|src/request_body_processor/json_backend_simdjson.cc||MINOR|CLOSED|Use the init-statement to declare "result" inside the if statement.|
|156|cpp:S6004|src/request_body_processor/json_backend_simdjson.cc||MINOR|CLOSED|Use the init-statement to declare "sink_status" inside the if statement.|
|157|cpp:S6004|src/request_body_processor/json_backend_simdjson.cc||MINOR|CLOSED|Use the init-statement to declare "result" inside the if statement.|
|158|cpp:S6004|src/request_body_processor/json_backend_simdjson.cc||MINOR|CLOSED|Use the init-statement to declare "sink_status" inside the if statement.|
|159|cpp:S6004|src/request_body_processor/json_backend_simdjson.cc||MINOR|CLOSED|Use the init-statement to declare "sink_status" inside the if statement.|
|160|cpp:S6004|src/request_body_processor/json_backend_simdjson.cc||MINOR|CLOSED|Use the init-statement to declare "result" inside the if statement.|
|161|cpp:S6004|src/request_body_processor/json_backend_simdjson.cc||MINOR|CLOSED|Use the init-statement to declare "sink_status" inside the if statement.|
|162|cpp:S7121|src/transaction.cc||MAJOR|CLOSED|Remove this redundant call to "c_str" when initializing a const "std::string" reference parameter.|
|163|cpp:S5812|src/utils/json_writer.cc||MINOR|CLOSED|Concatenate this namespace with the nested one.|
|164|cpp:S3230|src/utils/json_writer.cc||MAJOR|CLOSED|Remove this use of the constructor's initializer list for data member "m_output". It is redundant with default initialization behavior.|
|165|cpp:S3230|src/utils/json_writer.cc||MAJOR|CLOSED|Remove this use of the constructor's initializer list for data member "m_stack". It is redundant with default initialization behavior.|
|166|cpp:S5945|src/utils/json_writer.cc|155|MAJOR|OPEN|Use "std::string" instead of a C-style char array.|
|167|cpp:S3628|src/utils/json_writer.cc||MINOR|CLOSED|Convert this string literal to a raw string literal.|
|168|cpp:S3628|src/utils/json_writer.cc||MINOR|CLOSED|Convert this string literal to a raw string literal.|
|169|cpp:S6022|src/utils/json_writer.cc|183|MAJOR|OPEN|Use "std::byte" for byte-oriented data manipulation.|
|170|cpp:S6022|src/utils/json_writer.cc|184|MAJOR|OPEN|Use "std::byte" for byte-oriented data manipulation.|
|171|cpp:S5812|src/utils/json_writer.h||MINOR|CLOSED|Concatenate this namespace with the nested one.|
|172|cpp:S5812|test/common/json.h||MINOR|CLOSED|Concatenate this namespace with the nested one.|
|173|cpp:S2807|test/common/json.h|78|MAJOR|OPEN|Make this member overloaded operator a hidden friend.|
|174|cpp:S2807|test/common/json.h|123|MAJOR|OPEN|Make this member overloaded operator a hidden friend.|
|175|cpp:S1181|test/common/json.h|219|MAJOR|OPEN|Catch a more specific exception instead of a generic one.|
|176|cpp:S995|test/common/json.h|232|MINOR|OPEN|Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *".|
|177|cpp:S995|test/common/json.h|242|MINOR|OPEN|Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *".|
|178|cpp:S995|test/common/json.h|252|MINOR|OPEN|Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *".|
|179|cpp:S995|test/common/json.h|262|MINOR|OPEN|Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *".|
|180|cpp:S995|test/common/json.h|269|MINOR|OPEN|Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *".|
|181|cpp:S995|test/common/json.h|279|MINOR|OPEN|Make the type of this parameter a pointer-to-const. The current type of "error" is "std::string *".|
|182|cpp:S1181|test/common/json.h|309|MAJOR|OPEN|Catch a more specific exception instead of a generic one.|
|183|cpp:S1181|test/common/json.h|321|MAJOR|OPEN|Catch a more specific exception instead of a generic one.|
|184|cpp:S1181|test/common/json.h|333|MAJOR|OPEN|Catch a more specific exception instead of a generic one.|
|185|cpp:S134|test/common/modsecurity_test.cc|89|CRITICAL|OPEN|Refactor this code to not nest more than 3 if\|for\|do\|while\|switch statements.|
|186|cpp:S5415|test/regression/regression_test.cc||MAJOR|CLOSED|The result of "std::move" should not be passed as a const reference.|
|187|cpp:S5415|test/regression/regression_test.cc||MAJOR|CLOSED|The result of "std::move" should not be passed as a const reference.|
|188|cpp:S5415|test/regression/regression_test.cc||MAJOR|CLOSED|The result of "std::move" should not be passed as a const reference.|
|189|cpp:S5415|test/regression/regression_test.cc||MAJOR|CLOSED|The result of "std::move" should not be passed as a const reference.|
|190|cpp:S5415|test/regression/regression_test.cc||MAJOR|CLOSED|The result of "std::move" should not be passed as a const reference.|
|191|cpp:S5415|test/regression/regression_test.cc||MAJOR|CLOSED|The result of "std::move" should not be passed as a const reference.|
|192|cpp:S5415|test/regression/regression_test.cc||MAJOR|CLOSED|The result of "std::move" should not be passed as a const reference.|
|193|cpp:S5817|test/regression/regression_test.cc|431|MAJOR|OPEN|This function should be declared "const".|
|194|cpp:S5274|test/regression/regression_test.cc||MAJOR|CLOSED|moving a temporary object prevents copy elision|
|195|cpp:S5274|test/regression/regression_test.cc||MAJOR|CLOSED|moving a temporary object prevents copy elision|
|196|cpp:S1481|test/regression/regression_test.cc||MINOR|CLOSED|Remove the unused lambda capture "writer".|
|197|cpp:S5415|test/unit/unit_test.cc||MAJOR|CLOSED|The result of "std::move" should not be passed as a const reference.|
|198|cpp:S836|test/unit/unit_test.h||MAJOR|CLOSED|Value assigned to field 'ret' in implicit constructor is garbage or undefined|
|199|cpp:S886|src/modsecurity.cc|232|MINOR|OPEN|Refactor this loop so that it is less error-prone.|
|200|cpp:S886|src/modsecurity.cc|288|MINOR|OPEN|Refactor this loop so that it is less error-prone.|
|201|cpp:S5812|src/request_body_processor/json.cc||MINOR|CLOSED|Concatenate this namespace with the nested one.|
|202|cpp:S5812|src/request_body_processor/json.h||MINOR|CLOSED|Concatenate this namespace with the nested one.|
|203|cpp:S4962|src/utils/msc_tree.cc||CRITICAL|CLOSED|Use the "nullptr" literal.|
|204|cpp:S4962|src/utils/msc_tree.cc||CRITICAL|CLOSED|Use the "nullptr" literal.|
|205|cpp:S4962|src/utils/msc_tree.cc||CRITICAL|CLOSED|Use the "nullptr" literal.|
|206|cpp:S4962|src/utils/msc_tree.cc||CRITICAL|CLOSED|Use the "nullptr" literal.|
|207|cpp:S4962|src/utils/msc_tree.cc||CRITICAL|CLOSED|Use the "nullptr" literal.|

# Hotspots (neu)
- Anzahl: 0
- Keine Hotspots im aktuellen Abruf.

# Änderungen seit letzter Analyse
- Head-Commit geändert: `5dd7b3539977de85f0a2aebd2cb28b9ea1640211` -> `bbde416a92212f1fc4a975361e73a3b07356a526`
- Vergleich mit vorherigem Report im Repository (dok. Werte):
  - Issues: 205 -> 207
  - Hotspots: 1 -> 0
  - Quality Gate Fehlerbedingungen: 1 -> 0
- Neue Issues / weniger Issues (nur Mengenvergleich): +2 Issues gegenüber dem vorherigen Report.

# JSON-Backend Bewertung (neu)
- Build-Time-Trennung:
  - `configure.ac` erlaubt `--with-json-backend=simdjson|jsoncons` und definiert backend-spezifische Conditions/Defines.
  - `src/Makefile.am` nimmt abhängig von der Condition nur `json_backend_simdjson.cc` oder `json_backend_jsoncons.cc` in `BODY_PROCESSORS` auf.
- Gemeinsame Interfaces/Abstraktion:
  - `json_backend.h` enthält `JsonEventSink` und gemeinsame Status-/Ergebnis-Typen.
  - `json_adapter.cc` ruft per Präprozessor genau ein Backend auf.
- Compiler beide Backends gleichzeitig:
  - Für den `src`-Build in den gezeigten Conditions nicht vorgesehen.
- Überschneidungen/Kopplung:
  - Gemeinsame Adapter-/Sink-Schicht ist vorhanden (gewollte Kopplung).
  - `test/Makefile.am` enthält jsoncons Include-Pfad global in Test-CPPFLAGS.
- Neue Duplikation seit letztem Stand: Nicht belegbar auf Basis der aktuellen Quellen.

# Grenzen der Analyse
- Sonar-Issue-Liste wurde vollständig aus `api/issues/search` für PR 3540 geladen.
- Duplikationsblöcke auf Code-Block-Ebene wurden nicht über einen dedizierten Sonar-Duplikations-Endpunkt verifiziert.
  - Nicht belegbar auf Basis der aktuellen Quellen.
- Ein Teil der GitHub-Checks war beim Abruf noch `in_progress`; diese Aussage betrifft CI-Status, nicht Sonar-Issue-Vollständigkeit.