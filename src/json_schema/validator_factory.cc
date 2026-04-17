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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "src/json_schema/validator_factory.h"

#if defined(WITH_BLAZE)
#include "src/json_schema/validator_blaze.h"
#else
#include "src/json_schema/validator_unavailable.h"
#endif

namespace modsecurity::JsonSchema {

std::unique_ptr<JsonSchemaValidator> createDefaultJsonSchemaValidator() {
#if defined(WITH_BLAZE)
    return createBlazeJsonSchemaValidator();
#else
    return createUnavailableJsonSchemaValidator();
#endif
}

}  // namespace modsecurity::JsonSchema
