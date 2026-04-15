/*
 * ModSecurity, http://www.modsecurity.org/
 * Copyright (c) 2015 - 2023 Trustwave Holdings, Inc. (http://www.trustwave.com/)
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

#ifndef SRC_OPERATORS_VALIDATE_SCHEMA_H_
#define SRC_OPERATORS_VALIDATE_SCHEMA_H_

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <string>
#include <memory>
#include <utility>

#include "src/operators/operator.h"


namespace modsecurity {
namespace operators {

class ValidateSchema : public Operator {
 public:
    /** @ingroup ModSecurity_Operator */
    explicit ValidateSchema(std::unique_ptr<RunTimeString> param)
        : Operator("ValidateSchema", std::move(param)) { }
#ifdef WITH_LIBXML2

    bool evaluate(Transaction *transaction, const std::string  &str) override;
    bool init(const std::string &file, std::string *error) override;

 private:
    std::string m_resource;
    std::string m_err;
#endif
};

}  // namespace operators
}  // namespace modsecurity


#endif  // SRC_OPERATORS_VALIDATE_SCHEMA_H_
