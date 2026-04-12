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

    return failures == 0 ? 0 : 1;
}

}  // namespace RequestBodyProcessor
}  // namespace modsecurity

int main() {
    return modsecurity::RequestBodyProcessor::runJsonBackendDepthTests();
}
