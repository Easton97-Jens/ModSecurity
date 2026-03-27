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

#include "src/operators/detect_xss.h"

#include <string>

#include "src/operators/libinjection_utils.h"
#include "src/operators/operator.h"
#include "libinjection/src/libinjection.h"
#include "libinjection/src/libinjection_error.h"


namespace modsecurity {
namespace operators {


bool DetectXSS::evaluate(Transaction *t, RuleWithActions *rule,
    const std::string& input, RuleMessage &ruleMessage) {
    const injection_result_t result =
        libinjection_xss(input.c_str(), input.length());

    if (t == nullptr) {
        return is_malicious(result);
    }

    if (result == LIBINJECTION_RESULT_ERROR) {
        ms_dbg_a(t, 3, "libinjection XSS parser error on input: '" + input
            + "'. Blocking request by fail-safe policy.");
        if (rule && rule->hasCaptureAction()) {
            /*
             * Parser errors are treated as malicious by policy; TX.0 must be
             * updated for this evaluation to avoid stale captures.
             */
            t->m_collections.m_tx_collection->storeOrUpdateFirst(
                "0", std::string(input));
            ms_dbg_a(t, 7, "Added DetectXSS parser-error match TX.0: " +
                std::string(input));
        }
    } else if (result == LIBINJECTION_RESULT_TRUE) {
        ms_dbg_a(t, 5, "detected XSS using libinjection.");
        if (rule && rule->hasCaptureAction()) {
            t->m_collections.m_tx_collection->storeOrUpdateFirst(
                "0", std::string(input));
            ms_dbg_a(t, 7, "Added DetectXSS match TX.0: " +
                std::string(input));
        }
    } else {
        if (input.empty()) {
            ms_dbg_a(t, 9, "detected XSS: empty input; no XSS detected.");
        } else {
            ms_dbg_a(t, 9, "libinjection was not able to "
                "find any XSS in: " + input);
        }
    }

    return is_malicious(result);
}


}  // namespace operators
}  // namespace modsecurity
