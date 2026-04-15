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

#include "src/request_body_processor/xml.h"

#include <cstddef>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "modsecurity/rules_set.h"
#include "modsecurity/rules_set_properties.h"
#include "modsecurity/transaction.h"


namespace modsecurity::RequestBodyProcessor {

#ifdef WITH_LIBXML2
namespace {
struct XmlSecurityPolicy {
    bool deny_all_external_resources{false};
};

constexpr int kXmlParserCommonOptions = XML_PARSE_NOWARNING | XML_PARSE_NOERROR;
constexpr int kXmlParserMandatorySecurityOptions = XML_PARSE_NONET;
#ifdef XML_PARSE_NO_XXE
constexpr bool kXmlParseNoXxeAvailable = true;
#else
constexpr bool kXmlParseNoXxeAvailable = false;
#endif

bool isLikelyNetworkResource(const char *url) {
    if (url == nullptr) {
        return false;
    }
    return std::strncmp(url, "http://", 7) == 0
        || std::strncmp(url, "https://", 8) == 0
        || std::strncmp(url, "ftp://", 6) == 0;
}

xmlParserErrors xmlBackendResourceLoader(void *ctxt, const char *url,
    const char *, xmlResourceType, xmlParserInputFlags flags,
    xmlParserInput **out) {
    if (out == nullptr) {
        return XML_ERR_ARGUMENT;
    }
    *out = nullptr;

    const auto *policy = reinterpret_cast<const XmlSecurityPolicy *>(ctxt);
    if (policy != nullptr && policy->deny_all_external_resources) {
        return XML_IO_LOAD_ERROR;
    }

    if (isLikelyNetworkResource(url)) {
        return XML_IO_NETWORK_ATTEMPT;
    }

    xmlParserErrors parser_error = xmlNewInputFromUrl(url, flags, out);
    if (parser_error != XML_ERR_OK || *out == nullptr) {
        return XML_IO_LOAD_ERROR;
    }

    return XML_ERR_OK;
}

bool finalizeArgsParsingContext(xml_data *data, std::string *error) {
    if (xmlParseChunk(data->parsing_ctx_arg, nullptr, 0, 1) == 0) {
        xmlFreeParserCtxt(data->parsing_ctx_arg);
        data->parsing_ctx_arg = nullptr;
        return true;
    }

    if (!data->xml_error.empty()) {
        error->assign(data->xml_error);
    } else {
        error->assign("XML: Failed to parse document for ARGS.");
    }
    xmlFreeParserCtxt(data->parsing_ctx_arg);
    data->parsing_ctx_arg = nullptr;
    return false;
}

void configureParserSecurityPolicy(xmlParserCtxtPtr ctx,
    XmlSecurityPolicy *policy) {
    if (ctx == nullptr) {
        return;
    }

    int options = kXmlParserCommonOptions | kXmlParserMandatorySecurityOptions;
#ifdef XML_PARSE_NO_XXE
    options |= XML_PARSE_NO_XXE;
#endif
    xmlCtxtSetOptions(ctx, ctx->options | options);

    if (policy != nullptr) {
#ifndef XML_PARSE_NO_XXE
        policy->deny_all_external_resources = true;
#endif
        xmlCtxtSetResourceLoader(ctx, xmlBackendResourceLoader, policy);
    }
}

void appendToString(void *ctx, const std::string &message) {
    auto *value = reinterpret_cast<std::string *>(ctx);
    if (value != nullptr) {
        value->append(message);
    }
}

void debugTransactionMessage(const void *ctx, const std::string &message) {
    auto *tx = reinterpret_cast<const Transaction *>(ctx);
    if (tx != nullptr) {
        ms_dbg_a(tx, 4, message);
    }
}

template <typename Sink>
void xmlErrorCallback(void *ctx, Sink sink, const char *prefix,
    const char *msg, va_list args) {
    if (ctx == nullptr || msg == nullptr) {
        return;
    }

    char buf[1024];
    const auto len = vsnprintf(buf, sizeof(buf), msg, args);
    if (len > 0) {
        sink(ctx, std::string(prefix) + std::string(buf));
    }
}

void schemaParserError(void *ctx, const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    xmlErrorCallback(ctx, appendToString, "XML Error: ", msg, args);
    va_end(args);
}

void schemaParserWarning(void *ctx, const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    xmlErrorCallback(ctx, appendToString, "XML Warning: ", msg, args);
    va_end(args);
}

void schemaRuntimeError(void *ctx, const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    xmlErrorCallback(ctx, debugTransactionMessage, "XML Error: ", msg, args);
    va_end(args);
}

void schemaRuntimeWarning(void *ctx, const char *msg, ...) {
    va_list args;
    va_start(args, msg);
    xmlErrorCallback(ctx, debugTransactionMessage, "XML Warning: ", msg, args);
    va_end(args);
}
}  // namespace

/*
* NodeData for parsing XML into args
*/
NodeData::NodeData() = default;

NodeData::~NodeData() = default;

/*
* XMLNodes for parsing XML into args
*/
XMLNodes::XMLNodes(Transaction *transaction) 
    : m_transaction(transaction)
    {}

XMLNodes::~XMLNodes() = default;

/*
* SAX handler for parsing XML into args
*/
class MSCSAXHandler {
    public:
        void onStartElement(void * ctx, const xmlChar *localname) {

            std::string name = reinterpret_cast<const char*>(localname);

            auto *xml_data = static_cast<XMLNodes*>(ctx);
            xml_data->nodes.push_back(std::make_shared<NodeData>());
            xml_data->node_depth++;
            // if it's not the first (root) item, then append a '.'
            // note, the condition should always be true because there is always a pseudo root element: 'xml'
            if (xml_data->nodes.size() > 1) {
                xml_data->currpath.append(".");
                const std::size_t parent_index = xml_data->nodes.size() - 2;
                xml_data->nodes[parent_index]->has_child = true;
            }
            xml_data->currpath.append(name);
            // set the current value empty
            // this is necessary because if there is any text between the tags (new line, etc)
            // it will be added to the current value
            xml_data->currval.clear();
            xml_data->currval_is_set = false;
        }

        void onEndElement(void * ctx, const xmlChar *localname) {
            std::string name = reinterpret_cast<const char*>(localname);
            auto *xml_data = static_cast<XMLNodes*>(ctx);
            if (const auto &nd =
                    xml_data->nodes.back();
                    !nd->has_child && !xml_data->m_transaction->addArgument(
                        "XML", xml_data->currpath, xml_data->currval, 0)) {
                // check the return value
                // if false, then stop parsing
                // this means the number of arguments reached the limit
                xmlStopParser(xml_data->parsing_ctx_arg);
            }
            if (!xml_data->currpath.empty()) {
                // set an offset to store whether this is the first item, in order to know whether to remove the '.'
                const std::size_t offset = (xml_data->nodes.size() > 1) ? 1 : 0;
                xml_data->currpath.erase(
                    xml_data->currpath.size() - (name.size() + offset));
            }
            xml_data->nodes.pop_back();
            xml_data->node_depth--;
            xml_data->currval.clear();
            xml_data->currval_is_set = false;
        }

        void onCharacters(void *ctx, const xmlChar *ch, int len) {
            auto *xml_data = static_cast<XMLNodes*>(ctx);
            std::string content(reinterpret_cast<const char *>(ch), len);

            // libxml2 SAX parser will call this function multiple times
            // during the parsing of a single node, if the value has multibyte
            // characters, so we need to concatenate the values
            if (!xml_data->currval_is_set) {
                xml_data->currval = content;
                xml_data->currval_is_set = true;
            } else {
                xml_data->currval += content;
            }
        }
};

extern "C" {
    void MSC_startElement(void *userData,
        const xmlChar *name,
        const xmlChar *,
        const xmlChar *,
        int,
        const xmlChar **,
        int,
        int,
        const xmlChar **) {

            auto *handler = static_cast<MSCSAXHandler*>(userData);
            handler->onStartElement(userData, name);
    }

    void MSC_endElement(
        void *userData,
        const xmlChar *name,
        const xmlChar*,
        const xmlChar*) {

            auto *handler = static_cast<MSCSAXHandler*>(userData);
            handler->onEndElement(userData, name);
    }

    void MSC_xmlcharacters(void *userData, const xmlChar *ch, int len) {
        auto *handler = static_cast<MSCSAXHandler*>(userData);
        handler->onCharacters(userData, ch, len);
    }
}

XML::XML(Transaction *transaction)
    : m_transaction(transaction) { }


XML::~XML() {
    if (m_data.parsing_ctx != nullptr) {
        xmlFreeParserCtxt(m_data.parsing_ctx);
        m_data.parsing_ctx = nullptr;
    }
    if (m_data.doc != nullptr) {
        xmlFreeDoc(m_data.doc);
        m_data.doc = nullptr;
    }
}

bool XML::init() {
#ifndef XML_PARSE_NO_XXE
    ms_dbg_a(m_transaction, 3, "XML: XML_PARSE_NO_XXE is not available in this "
        "libxml2 build; external resources are denied via the XML backend "
        "resource loader fallback.");
#endif
    if (m_transaction->m_secXMLParseXmlIntoArgs
        == RulesSetProperties::TrueConfigXMLParseXmlIntoArgs ||
        m_transaction->m_secXMLParseXmlIntoArgs
        == RulesSetProperties::OnlyArgsConfigXMLParseXmlIntoArgs) {
        ms_dbg_a(m_transaction, 9,
                "XML: SecParseXmlIntoArgs is set to " \
                + RulesSetProperties::configXMLParseXmlIntoArgsString(static_cast<RulesSetProperties::ConfigXMLParseXmlIntoArgs>(m_transaction->m_secXMLParseXmlIntoArgs)));
        m_data.sax_handler = std::make_unique<xmlSAXHandler>();
        memset(m_data.sax_handler.get(), 0, sizeof(xmlSAXHandler));

        m_data.sax_handler->initialized = XML_SAX2_MAGIC;
        m_data.sax_handler->startElementNs = &MSC_startElement;
        m_data.sax_handler->endElementNs = &MSC_endElement;
        m_data.sax_handler->characters = &MSC_xmlcharacters;

        // set the parser state struct
        m_data.xml_parser_state                  = std::make_unique<XMLNodes>(m_transaction);
        // the XML will contain at least one node, which is the pseudo root node 'xml'
        m_data.xml_parser_state->currpath        = "xml.";
    }

    return true;
}


bool XML::processChunk(const char *buf, unsigned int size,
    std::string *error) {
    static XmlSecurityPolicy main_policy;
    static XmlSecurityPolicy args_policy;

    /* We want to initialise our parsing context here, to
     * enable us to pass it the first chunk of data so that
     * it can attempt to auto-detect the encoding.
     */
    if (m_data.parsing_ctx == nullptr && m_data.parsing_ctx_arg == nullptr) {
        /* First invocation. */

        ms_dbg_a(m_transaction, 4, "XML: Initialising parser.");

        if (m_transaction->m_secXMLParseXmlIntoArgs
            != RulesSetProperties::OnlyArgsConfigXMLParseXmlIntoArgs) {
            m_data.parsing_ctx = xmlCreatePushParserCtxt(nullptr, nullptr,
                buf, size, "body.xml");

            if (m_data.parsing_ctx == nullptr) {
                ms_dbg_a(m_transaction, 4,
                    "XML: Failed to create parsing context.");
                error->assign("XML: Failed to create parsing context.");
                return false;
            }
            configureParserSecurityPolicy(m_data.parsing_ctx, &main_policy);
        }

        if (m_transaction->m_secXMLParseXmlIntoArgs
            == RulesSetProperties::OnlyArgsConfigXMLParseXmlIntoArgs ||
            m_transaction->m_secXMLParseXmlIntoArgs
            == RulesSetProperties::TrueConfigXMLParseXmlIntoArgs) {
            m_data.parsing_ctx_arg = xmlCreatePushParserCtxt(
                m_data.sax_handler.get(),
                m_data.xml_parser_state.get(),
                buf,
                size,
                nullptr);
            if (m_data.parsing_ctx_arg == nullptr) {
                error->assign("XML: Failed to create parsing context for ARGS.");
                return false;
            }
            configureParserSecurityPolicy(m_data.parsing_ctx_arg, &args_policy);
        }

        return true;
    }

    /* Not a first invocation. */
    if (m_data.parsing_ctx != nullptr &&
        m_transaction->m_secXMLParseXmlIntoArgs
        != RulesSetProperties::OnlyArgsConfigXMLParseXmlIntoArgs) {
        xmlParseChunk(m_data.parsing_ctx, buf, size, 0);
        m_data.xml_parser_state->parsing_ctx_arg = m_data.parsing_ctx_arg;
        if (m_data.parsing_ctx->wellFormed != 1) {
            error->assign("XML: Failed to parse document.");
            ms_dbg_a(m_transaction, 4, "XML: Failed to parse document.");
            return false;
        }
    }

    if (m_data.parsing_ctx_arg != nullptr &&
        (
            m_transaction->m_secXMLParseXmlIntoArgs
              == RulesSetProperties::OnlyArgsConfigXMLParseXmlIntoArgs
            ||
            m_transaction->m_secXMLParseXmlIntoArgs
              == RulesSetProperties::TrueConfigXMLParseXmlIntoArgs)
        ) {
        xmlParseChunk(m_data.parsing_ctx_arg, buf, size, 0);
        if (m_data.parsing_ctx_arg->wellFormed != 1) {
            error->assign("XML: Failed to parse document for ARGS.");
            ms_dbg_a(m_transaction, 4, "XML: Failed to parse document for ARGS.");
            return false;
        }
    }

    return true;
}


bool XML::complete(std::string *error) {
    /* Only if we have a context, meaning we've done some work. */
    if (m_data.parsing_ctx != nullptr || m_data.parsing_ctx_arg != nullptr) {
        if (m_data.parsing_ctx != nullptr &&
            m_transaction->m_secXMLParseXmlIntoArgs
            != RulesSetProperties::OnlyArgsConfigXMLParseXmlIntoArgs) {
            /* This is how we signal the end of parsing to libxml. */
            xmlParseChunk(m_data.parsing_ctx, nullptr, 0, 1);

            /* Preserve the results for our reference. */
            m_data.well_formed = m_data.parsing_ctx->wellFormed;
            m_data.doc = m_data.parsing_ctx->myDoc;

            /* Clean up everything else. */
            xmlFreeParserCtxt(m_data.parsing_ctx);
            m_data.parsing_ctx = nullptr;
            ms_dbg_a(m_transaction, 4, "XML: Parsing complete (well_formed " \
                + std::to_string(m_data.well_formed) + ").");

            if (m_data.well_formed != 1) {
                error->assign("XML: Failed to parse document.");
                ms_dbg_a(m_transaction, 4, "XML: Failed to parse document.");
                return false;
            }
        }
        if (m_data.parsing_ctx_arg != nullptr &&
            (
                m_transaction->m_secXMLParseXmlIntoArgs
                  == RulesSetProperties::OnlyArgsConfigXMLParseXmlIntoArgs
                  ||
                m_transaction->m_secXMLParseXmlIntoArgs
                  == RulesSetProperties::TrueConfigXMLParseXmlIntoArgs)
            ) {
            /* This is how we signale the end of parsing to libxml. */
            if (!finalizeArgsParsingContext(&m_data, error)) {
                return false;
            }
        }
    }

    return true;
}

bool XML::hasDocument() const {
    return m_data.doc != nullptr;
}

bool XML::isWellFormed() const {
    return m_data.well_formed == 1;
}

bool XML::validateDocumentAgainstDtd(const std::string &resource) const {
    xmlDtdPtr dtd = xmlParseDTD(nullptr,
        reinterpret_cast<const xmlChar *>(resource.c_str()));
    if (dtd == nullptr) {
        ms_dbg_a(m_transaction, 4, std::string("XML: Failed to load DTD: ")
            + resource);
        return false;
    }

    xmlValidCtxtPtr validation_context = xmlNewValidCtxt();
    if (validation_context == nullptr) {
        ms_dbg_a(m_transaction, 4, "XML: Failed to create a validation context.");
        xmlFreeDtd(dtd);
        return false;
    }

    validation_context->error = reinterpret_cast<xmlValidityErrorFunc>(
        schemaRuntimeError);
    validation_context->warning = reinterpret_cast<xmlValidityWarningFunc>(
        schemaRuntimeWarning);
    validation_context->userData = m_transaction;

    const bool valid = xmlValidateDtd(validation_context, m_data.doc, dtd) != 0;

    xmlFreeValidCtxt(validation_context);
    xmlFreeDtd(dtd);

    return valid;
}

bool XML::validateDocumentAgainstSchema(const std::string &resource,
    std::string *load_error) const {
    static XmlSecurityPolicy schema_policy;
    xmlSchemaParserCtxtPtr parser_context = xmlSchemaNewParserCtxt(
        resource.c_str());
    if (parser_context == nullptr) {
        if (load_error != nullptr) {
            load_error->assign("XML: Failed to load Schema from file: "
                + resource);
        }
        return false;
    }

    std::string parser_messages;
    xmlSchemaSetParserErrors(parser_context,
        reinterpret_cast<xmlSchemaValidityErrorFunc>(schemaParserError),
        reinterpret_cast<xmlSchemaValidityWarningFunc>(schemaParserWarning),
        &parser_messages);
    xmlSchemaSetResourceLoader(parser_context, xmlBackendResourceLoader,
        &schema_policy);

    xmlSchemaPtr schema = xmlSchemaParse(parser_context);
    if (schema == nullptr) {
        if (load_error != nullptr) {
            load_error->assign("XML: Failed to load Schema: " + resource + ". "
                + parser_messages);
        }
        xmlSchemaFreeParserCtxt(parser_context);
        return false;
    }

    xmlSchemaValidCtxtPtr validation_context = xmlSchemaNewValidCtxt(schema);
    if (validation_context == nullptr) {
        if (load_error != nullptr) {
            load_error->assign("XML: Failed to create validation context. "
                + parser_messages);
        }
        xmlSchemaFree(schema);
        xmlSchemaFreeParserCtxt(parser_context);
        return false;
    }

    xmlSchemaSetValidErrors(validation_context,
        reinterpret_cast<xmlSchemaValidityErrorFunc>(schemaRuntimeError),
        reinterpret_cast<xmlSchemaValidityWarningFunc>(schemaRuntimeWarning),
        m_transaction);
    const bool valid = xmlSchemaValidateDoc(validation_context, m_data.doc) == 0;

    xmlSchemaFreeValidCtxt(validation_context);
    xmlSchemaFree(schema);
    xmlSchemaFreeParserCtxt(parser_context);
    return valid;
}

bool XML::evaluateXPath(const std::string &expression,
    const std::vector<NamespaceDecl> &namespaces, std::vector<std::string> *values,
    std::string *error) const {
    if (values == nullptr) {
        if (error != nullptr) {
            error->assign("XML: Internal error: output collection is null.");
        }
        return false;
    }

    xmlXPathContextPtr xpath_context = xmlXPathNewContext(m_data.doc);
    if (xpath_context == nullptr) {
        if (error != nullptr) {
            error->assign("XML: Unable to create new XPath context.");
        }
        return false;
    }

    for (const NamespaceDecl &namespace_decl : namespaces) {
        if (xmlXPathRegisterNs(xpath_context,
                reinterpret_cast<const xmlChar *>(namespace_decl.prefix.c_str()),
                reinterpret_cast<const xmlChar *>(namespace_decl.href.c_str()))
            != 0) {
            if (error != nullptr) {
                error->assign("Failed to register XML namespace href \""
                    + namespace_decl.href + "\" prefix \""
                    + namespace_decl.prefix + "\".");
            }
            xmlXPathFreeContext(xpath_context);
            return false;
        }
    }

    xmlXPathObjectPtr xpath_object = xmlXPathEvalExpression(
        reinterpret_cast<const xmlChar *>(expression.c_str()), xpath_context);
    if (xpath_object == nullptr) {
        if (error != nullptr) {
            error->assign("XML: Unable to evaluate xpath expression.");
        }
        xmlXPathFreeContext(xpath_context);
        return false;
    }

    xmlNodeSetPtr nodes = xpath_object->nodesetval;
    if (nodes != nullptr) {
        for (int index = 0; index < nodes->nodeNr; ++index) {
            xmlChar *content = xmlNodeGetContent(nodes->nodeTab[index]);
            if (content != nullptr) {
                values->emplace_back(reinterpret_cast<const char *>(content));
                xmlFree(content);
            }
        }
    }

    xmlXPathFreeObject(xpath_object);
    xmlXPathFreeContext(xpath_context);
    return true;
}

#endif

}  // namespace modsecurity::RequestBodyProcessor
