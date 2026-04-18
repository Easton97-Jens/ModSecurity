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

#include "src/variables/xml.h"

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <utility>

#include "modsecurity/transaction.h"
#include "modsecurity/rules_set_properties.h"
#include "modsecurity/rules_set.h"

#include "src/request_body_processor/xml.h"
#include "modsecurity/actions/action.h"
#include "src/actions/xmlns.h"

namespace modsecurity {
namespace variables {

#ifndef WITH_LIBXML2
void XML::evaluate(Transaction *t,
    RuleWithActions *rule,
    std::vector<const VariableValue *> *l) { }
#else

void XML::evaluate(Transaction *t,
    RuleWithActions *rule,
    std::vector<const VariableValue *> *l) {
    std::string param;

    param = m_name;
    /*
    pos = m_name.find_first_of(":");
    if (pos == std::string::npos) {
        param = "";
    } else {
        param = std::string(m_name, pos+1, m_name.length() - (pos + 1));
    }
    */
    /* Is there an XML document tree at all? */
    if (!t->m_xml->hasDocument()) {
        /* Sorry, we've got nothing to give! */
        return;
    }

    /* Process the XPath expression. */
    std::vector<RequestBodyProcessor::XML::NamespaceDecl> namespaces;
    if (rule == NULL) {
        ms_dbg_a(t, 2, "XML: Can't look for xmlns, internal error.");
    } else {
        std::vector<actions::Action *> acts = rule->getActionsByName("xmlns", t);
        for (auto &x : acts) {
            actions::XmlNS *z = static_cast<actions::XmlNS *>(x);
            namespaces.push_back({z->m_scope, z->m_href});

            ms_dbg_a(t, 4, "Registered XML namespace href \"" + z->m_href + \
                "\" prefix \"" + z->m_scope + "\"");
        }
    }

    std::string error;
    std::vector<std::string> values;
    if (!t->m_xml->evaluateXPath(param, namespaces, &values, &error)) {
        if (!error.empty()) {
            ms_dbg_a(t, 1, error);
        }
        return;
    }

    for (const std::string &value : values) {
        std::unique_ptr<VariableValue> var(
            new VariableValue(m_fullName.get(), &value));
        if (!m_keyExclusion.toOmit(*m_fullName)) {
            l->push_back(var.release());
        }
    }
}

#endif

}  // namespace variables
}  // namespace modsecurity
