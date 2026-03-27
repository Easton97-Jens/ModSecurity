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

#include "src/operators/detect_sqli.h"

#include <string>
#include <list>

#include "src/operators/libinjection_utils.h"
#include "src/operators/operator.h"
#include "libinjection/src/libinjection.h"
#include "libinjection/src/libinjection_error.h"

namespace modsecurity {
namespace operators {


bool DetectSQLi::evaluate(Transaction *t, RuleWithActions *rule,
    const std::string& input, RuleMessage &ruleMessage) {
    char fingerprint[8] = { 0 };
    const injection_result_t result =
        libinjection_sqli(input.c_str(), input.length(), fingerprint);

    if (t == nullptr) {
        return is_malicious(result);
    }

    if (result == LIBINJECTION_RESULT_ERROR) {
        ms_dbg_a(t, 3, "libinjection SQLi parser error on input: '" + input
            + "'. Blocking request by fail-safe policy.");
        if (rule && rule->hasCaptureAction()) {
            /*
             * Parser errors are treated as malicious by policy; TX.0 must be
             * updated for this evaluation to avoid stale captures.
             */
            t->m_collections.m_tx_collection->storeOrUpdateFirst(
                "0", std::string(input));
            ms_dbg_a(t, 7, "Added DetectSQLi parser-error match TX.0: " +
                std::string(input));
        }
    } else if (result == LIBINJECTION_RESULT_TRUE) {
        t->m_matched.push_back(fingerprint);
        ms_dbg_a(t, 4, "detected SQLi using libinjection with "
            "fingerprint '" + std::string(fingerprint) + "' at: '"
            + input + "'");
        if (rule && rule->hasCaptureAction()) {
            t->m_collections.m_tx_collection->storeOrUpdateFirst(
                "0", std::string(fingerprint));
            ms_dbg_a(t, 7, "Added DetectSQLi match TX.0: " +
                std::string(fingerprint));
        }
    } else {
        if (input.empty()) {
            ms_dbg_a(t, 9, "detected SQLi: empty input; no SQLi detected.");
        } else {
            ms_dbg_a(t, 9, "detected SQLi: not able to find an "
                "inject on '" + input + "'");
        }
    }

    return is_malicious(result);
}


}  // namespace operators
}  // namespace modsecurity
