/*
 * ModSecurity, http://www.modsecurity.org/
 * Copyright (c) 2015 - 2021 Trustwave Holdings, Inc. (http://www.trustwave.com/)
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

#ifndef SRC_OPERATORS_LIBINJECTION_UTILS_H_
#define SRC_OPERATORS_LIBINJECTION_UTILS_H_

#include "libinjection/src/libinjection_error.h"

namespace modsecurity {
namespace operators {

/*
 * libinjection v4.0 reports parser failures via LIBINJECTION_RESULT_ERROR.
 * We intentionally treat parser failures as malicious (fail-safe) so that
 * unexpected parser states cannot silently bypass protection.
 */
static inline bool is_malicious(const injection_result_t result) {
    return result == LIBINJECTION_RESULT_TRUE
        || result == LIBINJECTION_RESULT_ERROR;
}

}  // namespace operators
}  // namespace modsecurity

#endif  // SRC_OPERATORS_LIBINJECTION_UTILS_H_
