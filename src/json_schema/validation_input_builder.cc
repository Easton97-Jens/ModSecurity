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

#include "src/json_schema/validation_input_builder.h"

#include <utility>

namespace modsecurity::JsonSchema {

ValidationInputBuilder::ValidationInputBuilder(double max_depth)
    : m_input(std::make_unique<ValidationInput>()),
      m_max_depth(max_depth),
      m_current_depth(0),
      m_depth_limit_exceeded(false) {
}

std::unique_ptr<ValidationInput> ValidationInputBuilder::release() {
    return std::move(m_input);
}

RequestBodyProcessor::JsonSinkStatus ValidationInputBuilder::startContainer(
    ValidationEventType type) {
    m_input->add(type);
    m_current_depth++;
    if (m_current_depth > m_max_depth) {
        m_depth_limit_exceeded = true;
        return RequestBodyProcessor::JsonSinkStatus::DepthLimitExceeded;
    }

    return RequestBodyProcessor::JsonSinkStatus::Continue;
}

RequestBodyProcessor::JsonSinkStatus ValidationInputBuilder::endContainer(
    ValidationEventType type) {
    m_input->add(type);
    m_current_depth--;
    if (m_current_depth < 0) {
        m_current_depth = 0;
        return RequestBodyProcessor::JsonSinkStatus::InternalError;
    }

    return RequestBodyProcessor::JsonSinkStatus::Continue;
}

RequestBodyProcessor::JsonSinkStatus ValidationInputBuilder::on_start_object() {
    return startContainer(ValidationEventType::StartObject);
}

RequestBodyProcessor::JsonSinkStatus ValidationInputBuilder::on_end_object() {
    return endContainer(ValidationEventType::EndObject);
}

RequestBodyProcessor::JsonSinkStatus ValidationInputBuilder::on_start_array() {
    return startContainer(ValidationEventType::StartArray);
}

RequestBodyProcessor::JsonSinkStatus ValidationInputBuilder::on_end_array() {
    return endContainer(ValidationEventType::EndArray);
}

RequestBodyProcessor::JsonSinkStatus ValidationInputBuilder::on_key(
    std::string_view value) {
    m_input->add(ValidationEventType::Key, value);
    return RequestBodyProcessor::JsonSinkStatus::Continue;
}

RequestBodyProcessor::JsonSinkStatus ValidationInputBuilder::on_string(
    std::string_view value) {
    m_input->add(ValidationEventType::String, value);
    return RequestBodyProcessor::JsonSinkStatus::Continue;
}

RequestBodyProcessor::JsonSinkStatus ValidationInputBuilder::on_number(
    std::string_view raw_number) {
    m_input->add(ValidationEventType::Number, raw_number);
    return RequestBodyProcessor::JsonSinkStatus::Continue;
}

RequestBodyProcessor::JsonSinkStatus ValidationInputBuilder::on_boolean(
    bool value) {
    m_input->add(ValidationEventType::Boolean, std::string_view(),
        value);
    return RequestBodyProcessor::JsonSinkStatus::Continue;
}

RequestBodyProcessor::JsonSinkStatus ValidationInputBuilder::on_null() {
    m_input->add(ValidationEventType::Null);
    return RequestBodyProcessor::JsonSinkStatus::Continue;
}

}  // namespace modsecurity::JsonSchema
