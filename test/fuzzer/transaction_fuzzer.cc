/*
 * ModSecurity targeted transaction/API fuzz harness.
 */

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include "modsecurity/modsecurity.h"
#include "modsecurity/rules_set.h"
#include "modsecurity/transaction.h"

namespace {

std::string readString(const uint8_t *data, size_t size, size_t *offset, size_t maxLen) {
    if (*offset >= size) {
        return {};
    }

    const size_t remaining = size - *offset;
    const size_t requested = data[*offset];
    *offset += 1;

    const size_t len = std::min({requested, maxLen, remaining});
    if (len == 0 || *offset + len > size) {
        return {};
    }

    std::string out(reinterpret_cast<const char *>(data + *offset), len);
    *offset += len;
    return out;
}

void addHeadersFromBlob(modsecurity::Transaction *t, const std::string &blob) {
    std::stringstream ss(blob);
    std::string line;
    int count = 0;

    while (std::getline(ss, line) && count++ < 24) {
        const auto pos = line.find(':');
        if (pos == std::string::npos) {
            continue;
        }

        const auto name = line.substr(0, pos);
        const auto value = line.substr(pos + 1);
        if (name.empty()) {
            continue;
        }
        t->addRequestHeader(name.c_str(), value.c_str());
    }
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < 8) {
        return 0;
    }

    const char *methods[] = {"GET", "POST", "PUT", "PATCH", "DELETE", "OPTIONS"};
    size_t offset = 0;

    const auto method = methods[data[offset++] % (sizeof(methods) / sizeof(methods[0]))];
    std::string uri = readString(data, size, &offset, 128);
    if (uri.empty()) {
        uri = "/";
    }

    std::string headerBlob = readString(data, size, &offset, 512);
    std::string body;
    if (offset < size) {
        body.assign(reinterpret_cast<const char *>(data + offset), size - offset);
    }

    modsecurity::ModSecurity ms;
    modsecurity::RulesSet rules;
    std::string rulesText =
        "SecRuleEngine On\n"
        "SecRequestBodyAccess On\n"
        "SecRule REQUEST_URI \"@rx [\\\\x00-\\\\x1f]\" \"id:1001,phase:1,deny,status:403\"\n"
        "SecRule REQUEST_HEADERS \"@contains ..\" \"id:1002,phase:1,log,pass,t:urlDecodeUni,t:lowercase\"\n"
        "SecRule REQUEST_BODY \"@rx (?i:select|union|<script)\" \"id:1003,phase:2,deny,status:403,t:none,t:compressWhitespace\"\n";
    rules.load(rulesText.c_str());

    modsecurity::Transaction t(&ms, &rules, nullptr);
    t.processConnection("127.0.0.1", 12345, "127.0.0.1", 80);
    t.processURI(uri.c_str(), method, "1.1");

    t.addRequestHeader("Host", "fuzz.local");
    t.addRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    addHeadersFromBlob(&t, headerBlob);
    t.processRequestHeaders();

    t.appendRequestBody(reinterpret_cast<const unsigned char *>(body.data()), body.size());
    t.processRequestBody();

    modsecurity::ModSecurityIntervention it;
    std::memset(&it, 0, sizeof(it));
    t.intervention(&it);
    if (it.url != nullptr) {
        free(it.url);
        it.url = nullptr;
    }
    if (it.log != nullptr) {
        free(it.log);
        it.log = nullptr;
    }

    return 0;
}

int main() {
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(std::cin)), std::istreambuf_iterator<char>());
    LLVMFuzzerTestOneInput(data.data(), data.size());
    return 0;
}
