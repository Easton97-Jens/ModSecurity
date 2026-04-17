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

#include "src/json_schema/validator_unavailable.h"

#include <memory>
#include <string>

#include "src/json_schema/validation_input.h"

namespace modsecurity::JsonSchema {
namespace {

constexpr const char *kUnavailableMessage =
    "Blaze support is disabled at build time.";

class UnavailableJsonSchemaValidator : public JsonSchemaValidator {
 public:
    bool loadSchemaFromFile(const std::string &path, std::string *error)
        override {
        (void) path;
        if (error != nullptr) {
            error->assign(kUnavailableMessage);
        }
        return false;
    }

    ValidationResult validate(const ValidationInput &input) override {
        (void) input;
        return ValidationResult{ValidationStatus::Unavailable,
            kUnavailableMessage};
    }
};

}  // namespace

std::unique_ptr<JsonSchemaValidator> createUnavailableJsonSchemaValidator() {
    return std::make_unique<UnavailableJsonSchemaValidator>();
}

}  // namespace modsecurity::JsonSchema
