/*
 * ModSecurity, http://www.modsecurity.org/
 * Copyright (c) 2015 - 2024 Trustwave Holdings, Inc. (http://www.trustwave.com/)
 *
 * You may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * If any of the files related to licensing are missing or if you have any
 * other questions related to licensing please contact Trustwave Holdings, Inc.
 * directly using the email address security@modsecurity.org.
 *
 */

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "src/request_body_processor/json_adapter.h"

namespace modsecurity {
namespace RequestBodyProcessor {
namespace {

class AcceptAllSink : public JsonEventSink {
 public:
    JsonSinkStatus on_start_object() override {
        return JsonSinkStatus::Continue;
    }

    JsonSinkStatus on_end_object() override {
        return JsonSinkStatus::Continue;
    }

    JsonSinkStatus on_start_array() override {
        return JsonSinkStatus::Continue;
    }

    JsonSinkStatus on_end_array() override {
        return JsonSinkStatus::Continue;
    }

    JsonSinkStatus on_key(std::string_view value) override {
        (void) value;
        return JsonSinkStatus::Continue;
    }

    JsonSinkStatus on_string(std::string_view value) override {
        (void) value;
        return JsonSinkStatus::Continue;
    }

    JsonSinkStatus on_number(std::string_view raw_number) override {
        (void) raw_number;
        return JsonSinkStatus::Continue;
    }

    JsonSinkStatus on_boolean(bool value) override {
        (void) value;
        return JsonSinkStatus::Continue;
    }

    JsonSinkStatus on_null() override {
        return JsonSinkStatus::Continue;
    }
};

class NumberCollectingSink : public AcceptAllSink {
 public:
    JsonSinkStatus on_number(std::string_view raw_number) override {
        numbers.emplace_back(raw_number.data(), raw_number.size());
        return JsonSinkStatus::Continue;
    }

    std::vector<std::string> numbers;
};

const char *parseStatusName(JsonParseStatus status) {
    switch (status) {
        case JsonParseStatus::Ok:
            return "Ok";
        case JsonParseStatus::ParseError:
            return "ParseError";
        case JsonParseStatus::TruncatedInput:
            return "TruncatedInput";
        case JsonParseStatus::Utf8Error:
            return "Utf8Error";
        case JsonParseStatus::EngineAbort:
            return "EngineAbort";
        case JsonParseStatus::InternalError:
            return "InternalError";
    }

    return "UnknownParseStatus";
}

const char *sinkStatusName(JsonSinkStatus status) {
    switch (status) {
        case JsonSinkStatus::Continue:
            return "Continue";
        case JsonSinkStatus::EngineAbort:
            return "EngineAbort";
        case JsonSinkStatus::DepthLimitExceeded:
            return "DepthLimitExceeded";
        case JsonSinkStatus::InternalError:
            return "InternalError";
    }

    return "UnknownSinkStatus";
}

std::string makeNestedArrayJson(std::size_t depth) {
    std::string input(depth, '[');
    input.push_back('0');
    input.append(depth, ']');
    return input;
}

std::string describeUnexpectedResult(const JsonParseResult &result,
    const char *expectation) {
    std::string detail = std::string("Expected ") + expectation + ", got "
        + parseStatusName(result.parse_status) + "/"
        + sinkStatusName(result.sink_status) + ".";
    if (!result.detail.empty()) {
        detail.append(" ");
        detail.append(result.detail);
    }
    return detail;
}

std::string describeStringList(const std::vector<std::string> &values) {
    std::string description = "[";

    for (std::size_t i = 0; i < values.size(); i++) {
        if (i != 0) {
            description.append(", ");
        }
        description.push_back('"');
        description.append(values[i]);
        description.push_back('"');
    }

    description.push_back(']');
    return description;
}

bool collectNumberLexemes(const std::string &input,
    std::vector<std::string> *numbers, std::string *failure_detail) {
    NumberCollectingSink sink;
    JSONAdapter adapter;
    JsonParseResult result = adapter.parse(input, &sink, JsonBackendParseOptions());

    if (!result.ok()) {
        if (failure_detail != nullptr) {
            *failure_detail = describeUnexpectedResult(result, "Ok/Continue");
            failure_detail->append(" Input: ");
            failure_detail->append(input);
        }
        return false;
    }

    if (numbers != nullptr) {
        *numbers = sink.numbers;
    }
    return true;
}

bool expectNumberLexemes(const char *case_name, const std::string &input,
    const std::vector<std::string> &expected, std::string *failure_detail) {
    std::vector<std::string> actual;
    if (!collectNumberLexemes(input, &actual, failure_detail)) {
        return false;
    }

    if (actual != expected) {
        if (failure_detail != nullptr) {
            *failure_detail = std::string("Case '") + case_name
                + "' expected " + describeStringList(expected)
                + ", got " + describeStringList(actual) + ".";
        }
        return false;
    }

    return true;
}

bool expectBackendDepthLimitParseError(std::string *failure_detail) {
    AcceptAllSink sink;
    JSONAdapter adapter;
    JsonBackendParseOptions options;

    options.technical_max_depth = 2;
    JsonParseResult result = adapter.parse(makeNestedArrayJson(8), &sink,
        options);

    if (result.parse_status != JsonParseStatus::ParseError
        || result.sink_status != JsonSinkStatus::Continue) {
        if (failure_detail != nullptr) {
            *failure_detail = describeUnexpectedResult(result,
                "ParseError/Continue");
        }
        return false;
    }

    return true;
}

bool expectBackendDepthHeadroomSuccess(std::string *failure_detail) {
    AcceptAllSink sink;
    JSONAdapter adapter;
    JsonBackendParseOptions options;

    options.technical_max_depth = 32;
    JsonParseResult result = adapter.parse(makeNestedArrayJson(8), &sink,
        options);

    if (!result.ok()) {
        if (failure_detail != nullptr) {
            *failure_detail = describeUnexpectedResult(result, "Ok/Continue");
        }
        return false;
    }

    return true;
}

bool expectExactRootScalarNumberLexemes(std::string *failure_detail) {
    struct NumberLexemeCase {
        const char *name;
        const char *input;
    };

    const NumberLexemeCase cases[] = {
        {"zero", "0"},
        {"negative_zero", "-0"},
        {"decimal", "1.0"},
        {"scientific", "1e3"},
        {"negative_fraction_with_exponent", "-1.25e-4"},
        {"uint64_max", "18446744073709551615"},
        {"uint64_overflow", "18446744073709551616"},
        {"large_integer", "123456789012345678901234567890"}
    };

    for (const auto &test_case : cases) {
        if (!expectNumberLexemes(test_case.name, test_case.input,
                std::vector<std::string>{test_case.input}, failure_detail)) {
            return false;
        }
    }

    return true;
}

bool expectExactContainerNumberLexemes(std::string *failure_detail) {
    const std::string input =
        "{ \"arr\" : [ 0 , -0 , 1.0 , 1e3 ], "
        "\"obj\" : { \"frac\" : -1.25e-4 , "
        "\"max\" : 18446744073709551615 , "
        "\"over\" : 18446744073709551616 , "
        "\"big\" : 123456789012345678901234567890 } }";

    return expectNumberLexemes("container_numbers_with_whitespace_and_boundaries",
        input, std::vector<std::string>{
            "0",
            "-0",
            "1.0",
            "1e3",
            "-1.25e-4",
            "18446744073709551615",
            "18446744073709551616",
            "123456789012345678901234567890"
        }, failure_detail);
}

bool reportTestResult(const char *name, bool passed,
    const std::string &detail) {
    std::cout << ":test-result: " << (passed ? "PASS " : "FAIL ")
        << "json_backend_depth_tests:" << name << std::endl;
    if (!passed && !detail.empty()) {
        std::cerr << name << ": " << detail << std::endl;
    }
    return passed;
}

}  // namespace

int runJsonBackendDepthTests() {
    int failures = 0;
    std::string detail;

    if (!reportTestResult("technical_depth_limit_returns_parse_error",
            expectBackendDepthLimitParseError(&detail), detail)) {
        failures++;
    }

    detail.clear();
    if (!reportTestResult("technical_depth_with_headroom_succeeds",
            expectBackendDepthHeadroomSuccess(&detail), detail)) {
        failures++;
    }

    detail.clear();
    if (!reportTestResult("number_lexemes_for_root_scalars_remain_exact",
            expectExactRootScalarNumberLexemes(&detail), detail)) {
        failures++;
    }

    detail.clear();
    if (!reportTestResult("number_lexemes_in_containers_remain_exact",
            expectExactContainerNumberLexemes(&detail), detail)) {
        failures++;
    }

    return failures == 0 ? 0 : 1;
}

}  // namespace RequestBodyProcessor
}  // namespace modsecurity

int main() {
    return modsecurity::RequestBodyProcessor::runJsonBackendDepthTests();
}
