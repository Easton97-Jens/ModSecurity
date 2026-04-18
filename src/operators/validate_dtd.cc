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

#include "src/operators/validate_dtd.h"

#include <string>

#include "src/request_body_processor/xml.h"
#include "src/utils/system.h"
#include "src/operators/operator.h"

namespace modsecurity {
namespace operators {

#ifdef WITH_LIBXML2
bool ValidateDTD::init(const std::string &file, std::string *error) {
    std::string err;
    m_resource = utils::find_resource(m_param, file, &err);
    if (m_resource == "") {
        error->assign("XML: File not found: " + m_param + ". " + err);
        return false;
    }

    return true;
}


bool ValidateDTD::evaluate(Transaction *transaction, const std::string &str) {
    if (!transaction->m_xml->hasDocument()) {
        ms_dbg_a(transaction, 4, "XML document tree could not "\
            "be found for DTD validation.");
        return true;
    }

    if (!transaction->m_xml->isWellFormed()) {
        ms_dbg_a(transaction, 4, "XML: DTD validation failed because " \
            "content is not well formed.");
        return true;
    }

#if 0
    /* Make sure there were no other generic processing errors */
    if (msr->msc_reqbody_error) {
        *error_msg = apr_psprintf(msr->mp,
                "XML: DTD validation could not proceed due to previous"
                " processing errors.");
        return 1;
    }
#endif

    if (!transaction->m_xml->validateDocumentAgainstDtd(m_resource)) {
        ms_dbg_a(transaction, 4, "XML: DTD validation failed.");
        return true;
    }

    ms_dbg_a(transaction, 4, std::string("XML: Successfully validated " \
        "payload against DTD: ") + m_resource);

    return false;
}
#endif

}  // namespace operators
}  // namespace modsecurity
