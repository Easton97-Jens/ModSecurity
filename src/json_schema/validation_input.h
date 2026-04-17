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

#ifndef SRC_JSON_SCHEMA_VALIDATION_INPUT_H_
#define SRC_JSON_SCHEMA_VALIDATION_INPUT_H_

#include <string>
#include <string_view>
#include <vector>

namespace modsecurity::JsonSchema {

class ValidationInputBuilder;

enum class ValidationEventType {
    StartObject,
    EndObject,
    StartArray,
    EndArray,
    Key,
    String,
    Number,
    Boolean,
    Null
};

struct ValidationEvent {
    ValidationEventType type;
    std::string text;
    bool boolean_value{false};

    bool operator==(const ValidationEvent &other) const {
        return type == other.type && text == other.text
            && boolean_value == other.boolean_value;
    }
};

class ValidationInput {
 public:
    using EventTape = std::vector<ValidationEvent>;

    const EventTape &events() const noexcept {
        return m_events;
    }

    bool empty() const noexcept {
        return m_events.empty();
    }

 private:
    friend class ValidationInputBuilder;

    void add(ValidationEventType type, std::string_view text = std::string_view(),
        bool boolean_value = false) {
        m_events.push_back(ValidationEvent{
            type,
            text.empty() ? std::string()
                : std::string(text.data(), text.size()),
            boolean_value});
    }

    EventTape m_events;
};

}  // namespace modsecurity::JsonSchema

#endif  // SRC_JSON_SCHEMA_VALIDATION_INPUT_H_
