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

#ifdef WITH_LIBXML2
#include <libxml/xmlschemas.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/SAX2.h>
#endif

#include <memory>
#include <utility>
#include <string>
#include <vector>

#ifndef SRC_REQUEST_BODY_PROCESSOR_XML_H_
#define SRC_REQUEST_BODY_PROCESSOR_XML_H_

namespace modsecurity {
class Transaction;
}


namespace modsecurity::RequestBodyProcessor {

#ifdef WITH_LIBXML2

/*
* NodeData for parsing XML into args
*/
class NodeData {
    public:
        explicit NodeData();
        ~NodeData();

        bool has_child = false;
};

/*
* XMLNodes for parsing XML into args
*/
class XMLNodes {
    public:
        std::vector<std::shared_ptr<NodeData>> nodes;
        unsigned long int node_depth = 0;
        std::string       currpath;
        std::string       currval;
        bool              currval_is_set = false;
        Transaction      *m_transaction = nullptr;
        // need to store context - this is the same as in xml_data
        // need to stop parsing if the number of arguments reached the limit
        xmlParserCtxtPtr  parsing_ctx_arg = nullptr;

        explicit XMLNodes (Transaction *);
        ~XMLNodes();
};

struct xml_data {
    std::unique_ptr<xmlSAXHandler> sax_handler;
    xmlParserCtxtPtr parsing_ctx = nullptr;
    xmlDocPtr doc = nullptr;

    unsigned int well_formed = 0;

    /* error reporting and XML array flag */
    std::string               xml_error;

    /* additional parser context for arguments */
    xmlParserCtxtPtr          parsing_ctx_arg = nullptr;

    /* parser state for SAX parser */
    std::unique_ptr<XMLNodes> xml_parser_state;
};

class XML {
 public:
    enum class XmlErrorCode {
        None,
        ParseError,
        InvalidInput,
        LimitExceeded,
        ValidationError,
        SecurityError,
        InternalError
    };

    struct XmlError {
        XmlErrorCode code{XmlErrorCode::None};
        std::string detail;
    };

    struct NamespaceDecl {
        std::string prefix;
        std::string href;
    };

    explicit XML(Transaction *transaction);
    ~XML();
    bool init();
    bool processChunk(const char *buf, unsigned int size, std::string *err);
    bool complete(std::string *err);
    bool hasDocument() const;
    bool isWellFormed() const;
    bool validateDocumentAgainstDtd(const std::string &resource) const;
    bool validateDocumentAgainstSchema(const std::string &resource,
        std::string *load_error) const;
    bool evaluateXPath(const std::string &expression,
        const std::vector<NamespaceDecl> &namespaces,
        std::vector<std::string> *values, std::string *error) const;

    xml_data m_data;

 private:
    Transaction *m_transaction = nullptr;
    std::string m_header;
};

#endif

}  // namespace modsecurity::RequestBodyProcessor

#endif  // SRC_REQUEST_BODY_PROCESSOR_XML_H_
