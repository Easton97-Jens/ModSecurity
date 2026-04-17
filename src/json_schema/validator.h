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

#ifndef SRC_JSON_SCHEMA_VALIDATOR_H_
#define SRC_JSON_SCHEMA_VALIDATOR_H_

#include <string>

namespace modsecurity::JsonSchema {

class ValidationInput;

enum class ValidationStatus {
    Valid,
    Invalid,
    InputError,
    SchemaError,
    Unavailable
};

struct ValidationResult {
    ValidationStatus status{ValidationStatus::Unavailable};
    std::string message;

    bool ok() const noexcept {
        return status == ValidationStatus::Valid;
    }
};

class JsonSchemaValidator {
 public:
    virtual ~JsonSchemaValidator() = default;

    virtual bool loadSchemaFromFile(const std::string &path,
        std::string *error) = 0;

    virtual ValidationResult validate(const ValidationInput &input) = 0;
};

}  // namespace modsecurity::JsonSchema

#endif  // SRC_JSON_SCHEMA_VALIDATOR_H_
