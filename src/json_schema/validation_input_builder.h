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

#ifndef SRC_JSON_SCHEMA_VALIDATION_INPUT_BUILDER_H_
#define SRC_JSON_SCHEMA_VALIDATION_INPUT_BUILDER_H_

#include <cstdint>
#include <memory>
#include <string_view>

#include "src/json_schema/validation_input.h"
#include "src/request_body_processor/json_backend.h"

namespace modsecurity::JsonSchema {

class ValidationInputBuilder : public RequestBodyProcessor::JsonEventSink {
 public:
    explicit ValidationInputBuilder(double max_depth);

    std::unique_ptr<ValidationInput> release();

    bool depthLimitExceeded() const noexcept {
        return m_depth_limit_exceeded;
    }

    RequestBodyProcessor::JsonSinkStatus on_start_object() override;
    RequestBodyProcessor::JsonSinkStatus on_end_object() override;
    RequestBodyProcessor::JsonSinkStatus on_start_array() override;
    RequestBodyProcessor::JsonSinkStatus on_end_array() override;
    RequestBodyProcessor::JsonSinkStatus on_key(std::string_view value) override;
    RequestBodyProcessor::JsonSinkStatus on_string(std::string_view value)
        override;
    RequestBodyProcessor::JsonSinkStatus on_number(std::string_view raw_number)
        override;
    RequestBodyProcessor::JsonSinkStatus on_boolean(bool value) override;
    RequestBodyProcessor::JsonSinkStatus on_null() override;

 private:
    RequestBodyProcessor::JsonSinkStatus startContainer(
        ValidationEventType type);
    RequestBodyProcessor::JsonSinkStatus endContainer(
        ValidationEventType type);

    std::unique_ptr<ValidationInput> m_input;
    double m_max_depth;
    int64_t m_current_depth;
    bool m_depth_limit_exceeded;
};

}  // namespace modsecurity::JsonSchema

#endif  // SRC_JSON_SCHEMA_VALIDATION_INPUT_BUILDER_H_
