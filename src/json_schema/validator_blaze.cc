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

#include "src/json_schema/validator_blaze.h"

#include <memory>
#include <string>
#include <vector>

#include "src/json_schema/blaze_bridge.h"
#include "src/json_schema/validation_input.h"

namespace modsecurity::JsonSchema {
namespace {

enum msc_blaze_event_type toBridgeEventType(ValidationEventType type) {
    switch (type) {
        case ValidationEventType::StartObject:
            return MSC_BLAZE_EVENT_START_OBJECT;
        case ValidationEventType::EndObject:
            return MSC_BLAZE_EVENT_END_OBJECT;
        case ValidationEventType::StartArray:
            return MSC_BLAZE_EVENT_START_ARRAY;
        case ValidationEventType::EndArray:
            return MSC_BLAZE_EVENT_END_ARRAY;
        case ValidationEventType::Key:
            return MSC_BLAZE_EVENT_KEY;
        case ValidationEventType::String:
            return MSC_BLAZE_EVENT_STRING;
        case ValidationEventType::Number:
            return MSC_BLAZE_EVENT_NUMBER;
        case ValidationEventType::Boolean:
            return MSC_BLAZE_EVENT_BOOLEAN;
        case ValidationEventType::Null:
            return MSC_BLAZE_EVENT_NULL;
    }

    return MSC_BLAZE_EVENT_NULL;
}

std::string lastBridgeErrorOrDefault(const char *fallback) {
    const char *message = msc_blaze_last_error();
    if (message != nullptr && message[0] != '\0') {
        return std::string(message);
    }

    return std::string(fallback);
}

class BlazeJsonSchemaValidator : public JsonSchemaValidator {
 public:
    BlazeJsonSchemaValidator() = default;

    ~BlazeJsonSchemaValidator() override {
        if (m_schema != nullptr) {
            msc_blaze_schema_destroy(m_schema);
        }
    }

    bool loadSchemaFromFile(const std::string &path, std::string *error)
        override {
        if (m_schema != nullptr) {
            msc_blaze_schema_destroy(m_schema);
            m_schema = nullptr;
        }

        if (msc_blaze_schema_create_from_file(path.c_str(), path.size(),
                &m_schema) != MSC_BLAZE_BRIDGE_STATUS_OK) {
            if (error != nullptr) {
                error->assign(lastBridgeErrorOrDefault(
                    "Failed to load Blaze schema."));
            }
            return false;
        }

        if (error != nullptr) {
            error->clear();
        }
        return true;
    }

    ValidationResult validate(const ValidationInput &input) override {
        if (m_schema == nullptr) {
            return ValidationResult{ValidationStatus::SchemaError,
                "No Blaze schema has been loaded."};
        }

        const auto &events = input.events();
        std::vector<msc_blaze_event> bridge_events;
        bridge_events.reserve(events.size());

        for (const auto &event : events) {
            bridge_events.push_back(msc_blaze_event{
                toBridgeEventType(event.type),
                event.text.empty() ? nullptr : event.text.c_str(),
                event.text.size(),
                event.boolean_value ? 1 : 0});
        }

        switch (msc_blaze_schema_validate(m_schema, bridge_events.data(),
                bridge_events.size())) {
            case MSC_BLAZE_BRIDGE_STATUS_VALID:
                return ValidationResult{ValidationStatus::Valid, ""};
            case MSC_BLAZE_BRIDGE_STATUS_INVALID:
                return ValidationResult{ValidationStatus::Invalid, ""};
            case MSC_BLAZE_BRIDGE_STATUS_INPUT_ERROR:
                return ValidationResult{ValidationStatus::InputError,
                    lastBridgeErrorOrDefault("Blaze input validation failed.")};
            case MSC_BLAZE_BRIDGE_STATUS_SCHEMA_ERROR:
                return ValidationResult{ValidationStatus::SchemaError,
                    lastBridgeErrorOrDefault(
                        "Blaze schema validation failed.")};
            case MSC_BLAZE_BRIDGE_STATUS_OK:
                break;
        }

        return ValidationResult{ValidationStatus::SchemaError,
            "Unexpected Blaze bridge status."};
    }

 private:
    msc_blaze_schema_handle *m_schema{nullptr};
};

}  // namespace

std::unique_ptr<JsonSchemaValidator> createBlazeJsonSchemaValidator() {
    return std::make_unique<BlazeJsonSchemaValidator>();
}

}  // namespace modsecurity::JsonSchema
