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

#include <array>
#include <cstddef>
#include <iostream>
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

#include "src/config.h"
#include "src/json_schema/validation_input_builder.h"
#include "src/json_schema/validator_factory.h"
#include "src/request_body_processor/json.h"
#include "src/request_body_processor/json_adapter.h"
#include "test/common/modsecurity_test_context.h"

#if defined(WITH_BLAZE)
#include "src/json_schema/blaze_bridge.h"
#endif

namespace modsecurity::JsonSchema {
namespace {

using modsecurity::RequestBodyProcessor::JSONAdapter;
using modsecurity::RequestBodyProcessor::JSON;
using modsecurity::RequestBodyProcessor::JsonParseResult;
using modsecurity::RequestBodyProcessor::JsonParseStatus;
using modsecurity::RequestBodyProcessor::JsonSinkStatus;

constexpr double kValidationDepthLimit = 10000.0;

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

const char *eventTypeName(ValidationEventType type) {
    switch (type) {
        case ValidationEventType::StartObject:
            return "StartObject";
        case ValidationEventType::EndObject:
            return "EndObject";
        case ValidationEventType::StartArray:
            return "StartArray";
        case ValidationEventType::EndArray:
            return "EndArray";
        case ValidationEventType::Key:
            return "Key";
        case ValidationEventType::String:
            return "String";
        case ValidationEventType::Number:
            return "Number";
        case ValidationEventType::Boolean:
            return "Boolean";
        case ValidationEventType::Null:
            return "Null";
    }

    return "UnknownEvent";
}

const char *validationStatusName(ValidationStatus status) {
    switch (status) {
        case ValidationStatus::Valid:
            return "Valid";
        case ValidationStatus::Invalid:
            return "Invalid";
        case ValidationStatus::InputError:
            return "InputError";
        case ValidationStatus::SchemaError:
            return "SchemaError";
        case ValidationStatus::Unavailable:
            return "Unavailable";
    }

    return "UnknownValidationStatus";
}

std::string describeUnexpectedParseResult(const JsonParseResult &result,
    const char *expected) {
    std::string detail = std::string("Expected ") + expected + ", got "
        + parseStatusName(result.parse_status) + "/"
        + sinkStatusName(result.sink_status) + ".";
    if (!result.detail.empty()) {
        detail.append(" ");
        detail.append(result.detail);
    }
    return detail;
}

std::string describeEvents(const ValidationInput::EventTape &events) {
    std::string result = "[";

    for (std::size_t index = 0; index < events.size(); index++) {
        if (index != 0U) {
            result.append(", ");
        }

        result.append(eventTypeName(events[index].type));
        if (!events[index].text.empty()) {
            result.append("(\"");
            result.append(events[index].text);
            result.append("\")");
        } else if (events[index].type == ValidationEventType::Boolean) {
            result.append(events[index].boolean_value ? "(true)" : "(false)");
        }
    }

    result.push_back(']');
    return result;
}

ValidationInput::EventTape makeEventTape(
    std::initializer_list<ValidationEvent> events) {
    ValidationInput::EventTape tape;
    tape.reserve(events.size());
    for (const auto &event : events) {
        tape.push_back(event);
    }

    return tape;
}

bool buildValidationInput(const std::string &input,
    std::unique_ptr<ValidationInput> *output, std::string *failure_detail) {
    ValidationInputBuilder builder(kValidationDepthLimit);
    JSONAdapter adapter;
    if (JsonParseResult result = adapter.parse(input, &builder);
            !result.ok()) {
        if (failure_detail != nullptr) {
            *failure_detail = describeUnexpectedParseResult(result,
                "Ok/Continue");
            failure_detail->append(" Input: ");
            failure_detail->append(input);
        }
        return false;
    }

    if (output != nullptr) {
        *output = builder.release();
    }
    return true;
}

std::string schemaPath(const char *name) {
    return std::string(TEST_TOP_SRCDIR) + "/test/unit/data/" + name;
}

bool reportTestResult(const char *name, bool passed,
    const std::string &detail) {
    std::cout << ":test-result: " << (passed ? "PASS " : "FAIL ")
        << "json_schema_tests:" << name << std::endl;
    if (!passed && !detail.empty()) {
        std::cerr << name << ": " << detail << std::endl;
    }
    return passed;
}

bool expectDuplicateKeysArePreservedInValidationTape(
    std::string *failure_detail) {
    std::unique_ptr<ValidationInput> input;
    if (!buildValidationInput(R"({"dup":"first","dup":"second"})", &input,
            failure_detail)) {
        return false;
    }

    const ValidationInput::EventTape expected = makeEventTape({
        {ValidationEventType::StartObject, "", false},
        {ValidationEventType::Key, "dup", false},
        {ValidationEventType::String, "first", false},
        {ValidationEventType::Key, "dup", false},
        {ValidationEventType::String, "second", false},
        {ValidationEventType::EndObject, "", false}
    });

    if (input->events() != expected) {
        if (failure_detail != nullptr) {
            *failure_detail = std::string("Expected ")
                + describeEvents(expected) + ", got "
                + describeEvents(input->events()) + ".";
        }
        return false;
    }

    return true;
}

bool expectRootScalarNumbersRemainExact(std::string *failure_detail) {
    const std::array<const char *, 5> cases{{
        "0",
        "-0",
        "1.0",
        "1e3",
        "-1.25e-4"
    }};

    for (const char *raw_number : cases) {
        std::unique_ptr<ValidationInput> input;
        if (!buildValidationInput(raw_number, &input, failure_detail)) {
            return false;
        }

        const ValidationInput::EventTape expected = makeEventTape({
            {ValidationEventType::Number, raw_number, false}
        });

        if (input->events() != expected) {
            if (failure_detail != nullptr) {
                *failure_detail = std::string("Expected exact number lexeme ")
                    + raw_number + ", got "
                    + describeEvents(input->events()) + ".";
            }
            return false;
        }
    }

    return true;
}

bool expectUnicodeStringsRemainStrings(std::string *failure_detail) {
    std::unique_ptr<ValidationInput> input;
    if (!buildValidationInput(R"("\u00E9")", &input, failure_detail)) {
        return false;
    }

    const ValidationInput::EventTape expected = makeEventTape({
        {ValidationEventType::String, std::string("\xC3\xA9", 2), false}
    });

    if (input->events() != expected) {
        if (failure_detail != nullptr) {
            *failure_detail = std::string("Expected ")
                + describeEvents(expected) + ", got "
                + describeEvents(input->events()) + ".";
        }
        return false;
    }

    return true;
}

bool expectLazyValidationInputLifecycle(std::string *failure_detail) {
    modsecurity_test::ModSecurityTestContext context(
        "ModSecurity JSON schema test context");
    auto transaction = context.create_transaction();
    JSON processor(&transaction);
    const std::string input = R"({"dup":"first","dup":"second"})";
    std::string error;

    if (!processor.init()) {
        if (failure_detail != nullptr) {
            *failure_detail = "JSON processor init returned false.";
        }
        return false;
    }

    processor.processChunk(input.c_str(), input.size(), nullptr);
    if (processor.hasCachedValidationInput()) {
        if (failure_detail != nullptr) {
            *failure_detail = "Validation input should not be cached before "
                "the first internal access.";
        }
        return false;
    }

    if (processor.validationInputBuildCount() != 0U) {
        if (failure_detail != nullptr) {
            *failure_detail = "Validation input build count must start at 0.";
        }
        return false;
    }

    if (!processor.complete(&error) || !error.empty()) {
        if (failure_detail != nullptr) {
            *failure_detail = std::string("JSON complete failed: ") + error;
        }
        return false;
    }

    if (processor.hasCachedValidationInput()) {
        if (failure_detail != nullptr) {
            *failure_detail = "complete() must not eagerly build the cached "
                "validation input.";
        }
        return false;
    }

    const ValidationInput *first = processor.getValidationInput(&error);
    if (first == nullptr || !error.empty()) {
        if (failure_detail != nullptr) {
            *failure_detail = std::string("Failed to build validation input: ")
                + error;
        }
        return false;
    }

    if (!processor.hasCachedValidationInput()
        || processor.validationInputBuildCount() != 1U) {
        if (failure_detail != nullptr) {
            *failure_detail = "First validation-input access must populate the "
                "cache exactly once.";
        }
        return false;
    }

    const ValidationInput *second = processor.getValidationInput(&error);
    if (second == nullptr || !error.empty()) {
        if (failure_detail != nullptr) {
            *failure_detail = std::string("Cached validation input lookup "
                "failed: ") + error;
        }
        return false;
    }

    if (first != second || processor.validationInputBuildCount() != 1U) {
        if (failure_detail != nullptr) {
            *failure_detail = "Repeated validation-input access must reuse the "
                "same cached object without rebuilding.";
        }
        return false;
    }

    processor.processChunk(nullptr, 0, nullptr);
    if (processor.hasCachedValidationInput()) {
        if (failure_detail != nullptr) {
            *failure_detail = "processChunk() must invalidate a cached "
                "validation input.";
        }
        return false;
    }

    const ValidationInput *third = processor.getValidationInput(&error);
    if (third == nullptr || !error.empty()) {
        if (failure_detail != nullptr) {
            *failure_detail = std::string("Validation input rebuild failed: ")
                + error;
        }
        return false;
    }

    if (processor.validationInputBuildCount() != 2U) {
        if (failure_detail != nullptr) {
            *failure_detail = "Validation input must rebuild after "
                "processChunk()-driven invalidation.";
        }
        return false;
    }

    processor.init();
    if (processor.hasCachedValidationInput()
        || processor.validationInputBuildCount() != 0U) {
        if (failure_detail != nullptr) {
            *failure_detail = "init() must clear the cached validation input "
                "and reset build bookkeeping.";
        }
        return false;
    }

    return third != nullptr;
}

bool expectUnavailableValidatorWhenBlazeDisabled(std::string *failure_detail) {
    auto validator = createDefaultJsonSchemaValidator();
    std::string error;

    if (validator->loadSchemaFromFile(schemaPath("blaze_const_string_schema.json"),
            &error)) {
        if (failure_detail != nullptr) {
            *failure_detail = "Unavailable validator unexpectedly accepted a "
                "schema file.";
        }
        return false;
    }

    std::unique_ptr<ValidationInput> input;
    if (!buildValidationInput("\"1.0\"", &input, failure_detail)) {
        return false;
    }

    const ValidationResult result = validator->validate(*input);
    if (result.status != ValidationStatus::Unavailable) {
        if (failure_detail != nullptr) {
            *failure_detail = std::string("Expected Unavailable, got ")
                + validationStatusName(result.status) + ".";
        }
        return false;
    }

    if (error.empty() || result.message.empty()) {
        if (failure_detail != nullptr) {
            *failure_detail = "Unavailable validator must return explanatory "
                "messages for both load and validate.";
        }
        return false;
    }

    return true;
}

#if defined(WITH_BLAZE)
bool expectBlazeValidatorKeepsStringTokensAsStrings(
    std::string *failure_detail) {
    auto validator = createDefaultJsonSchemaValidator();
    std::string error;

    if (!validator->loadSchemaFromFile(schemaPath("blaze_const_string_schema.json"),
            &error)) {
        if (failure_detail != nullptr) {
            *failure_detail = std::string("Failed to load Blaze schema: ")
                + error;
        }
        return false;
    }

    std::unique_ptr<ValidationInput> valid_input;
    if (!buildValidationInput("\"1.0\"", &valid_input, failure_detail)) {
        return false;
    }

    ValidationResult result = validator->validate(*valid_input);
    if (result.status != ValidationStatus::Valid) {
        if (failure_detail != nullptr) {
            *failure_detail = std::string("Expected Valid for string token, got ")
                + validationStatusName(result.status) + ".";
        }
        return false;
    }

    std::unique_ptr<ValidationInput> invalid_input;
    if (!buildValidationInput("\"2.0\"", &invalid_input, failure_detail)) {
        return false;
    }

    result = validator->validate(*invalid_input);
    if (result.status != ValidationStatus::Invalid) {
        if (failure_detail != nullptr) {
            *failure_detail = std::string("Expected Invalid for mismatched "
                "string const, got ") + validationStatusName(result.status)
                + ".";
        }
        return false;
    }

    return true;
}

bool expectDuplicateKeysUseLastValueDuringBlazeValidation(
    std::string *failure_detail) {
    auto validator = createDefaultJsonSchemaValidator();
    std::string error;
    std::unique_ptr<ValidationInput> input;

    if (!buildValidationInput(R"({"dup":"first","dup":"second"})", &input,
            failure_detail)) {
        return false;
    }

    if (!validator->loadSchemaFromFile(
            schemaPath("blaze_duplicate_second_schema.json"), &error)) {
        if (failure_detail != nullptr) {
            *failure_detail = std::string("Failed to load second-value schema: ")
                + error;
        }
        return false;
    }

    ValidationResult result = validator->validate(*input);
    if (result.status != ValidationStatus::Valid) {
        if (failure_detail != nullptr) {
            *failure_detail = std::string("Expected duplicate-key instance to "
                "validate against last-value schema, got ")
                + validationStatusName(result.status) + ".";
        }
        return false;
    }

    if (!validator->loadSchemaFromFile(
            schemaPath("blaze_duplicate_first_schema.json"), &error)) {
        if (failure_detail != nullptr) {
            *failure_detail = std::string("Failed to load first-value schema: ")
                + error;
        }
        return false;
    }

    result = validator->validate(*input);
    if (result.status != ValidationStatus::Invalid) {
        if (failure_detail != nullptr) {
            *failure_detail = std::string("Expected duplicate-key instance to "
                "fail against first-value schema, got ")
                + validationStatusName(result.status) + ".";
        }
        return false;
    }

    return true;
}

bool expectBridgeInputErrorsDoNotEscapeAcrossAbi(std::string *failure_detail) {
    msc_blaze_schema_handle *handle = nullptr;
    const std::string path = schemaPath("blaze_const_string_schema.json");
    if (msc_blaze_schema_create_from_file(path.c_str(), path.size(), &handle)
            != MSC_BLAZE_BRIDGE_STATUS_OK || handle == nullptr) {
        if (failure_detail != nullptr) {
            *failure_detail = std::string("Failed to create bridge schema: ")
                + msc_blaze_last_error();
        }
        return false;
    }

    const msc_blaze_event malformed{
        MSC_BLAZE_EVENT_END_OBJECT,
        nullptr,
        0U,
        0
    };

    const int status = msc_blaze_schema_validate(handle, &malformed, 1U);
    msc_blaze_schema_destroy(handle);

    if (status != MSC_BLAZE_BRIDGE_STATUS_INPUT_ERROR) {
        if (failure_detail != nullptr) {
            *failure_detail = "Malformed event tape must return an input-error "
                "status code.";
        }
        return false;
    }

    if (std::string(msc_blaze_last_error()).empty()) {
        if (failure_detail != nullptr) {
            *failure_detail = "Bridge input errors must expose a stable C "
                "error string.";
        }
        return false;
    }

    return true;
}

bool expectBridgeSchemaErrorsDoNotEscapeAcrossAbi(std::string *failure_detail) {
    const std::string path = schemaPath("missing-schema.json");
    msc_blaze_schema_handle *handle = nullptr;
    const int status = msc_blaze_schema_create_from_file(path.c_str(),
        path.size(), &handle);

    if (status != MSC_BLAZE_BRIDGE_STATUS_SCHEMA_ERROR) {
        if (failure_detail != nullptr) {
            *failure_detail = "Missing schema file must return a schema-error "
                "status code.";
        }
        return false;
    }

    if (handle != nullptr) {
        if (failure_detail != nullptr) {
            *failure_detail = "Schema-error path must not return a handle.";
        }
        msc_blaze_schema_destroy(handle);
        return false;
    }

    if (std::string(msc_blaze_last_error()).empty()) {
        if (failure_detail != nullptr) {
            *failure_detail = "Bridge schema errors must expose a stable C "
                "error string.";
        }
        return false;
    }

    return true;
}
#endif

}  // namespace

int runJsonSchemaTests() {
    int failures = 0;
    std::string detail;

    if (!reportTestResult("duplicate_keys_preserved_in_validation_tape",
            expectDuplicateKeysArePreservedInValidationTape(&detail), detail)) {
        failures++;
    }

    detail.clear();
    if (!reportTestResult("root_scalar_numbers_remain_exact",
            expectRootScalarNumbersRemainExact(&detail), detail)) {
        failures++;
    }

    detail.clear();
    if (!reportTestResult("unicode_strings_remain_strings",
            expectUnicodeStringsRemainStrings(&detail), detail)) {
        failures++;
    }

    detail.clear();
    if (!reportTestResult("lazy_validation_input_cache_lifecycle",
            expectLazyValidationInputLifecycle(&detail), detail)) {
        failures++;
    }

#if defined(WITH_BLAZE)
    detail.clear();
    if (!reportTestResult("blaze_validator_keeps_string_tokens_as_strings",
            expectBlazeValidatorKeepsStringTokensAsStrings(&detail), detail)) {
        failures++;
    }

    detail.clear();
    if (!reportTestResult("duplicate_keys_use_last_value_during_validation",
            expectDuplicateKeysUseLastValueDuringBlazeValidation(&detail),
            detail)) {
        failures++;
    }

    detail.clear();
    if (!reportTestResult("bridge_input_errors_stay_in_c_abi",
            expectBridgeInputErrorsDoNotEscapeAcrossAbi(&detail), detail)) {
        failures++;
    }

    detail.clear();
    if (!reportTestResult("bridge_schema_errors_stay_in_c_abi",
            expectBridgeSchemaErrorsDoNotEscapeAcrossAbi(&detail), detail)) {
        failures++;
    }
#else
    detail.clear();
    if (!reportTestResult("validator_is_unavailable_when_blaze_disabled",
            expectUnavailableValidatorWhenBlazeDisabled(&detail), detail)) {
        failures++;
    }
#endif

    return failures == 0 ? 0 : 1;
}

}  // namespace modsecurity::JsonSchema

int main() {
    return modsecurity::JsonSchema::runJsonSchemaTests();
}
