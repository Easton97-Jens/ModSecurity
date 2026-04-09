/*
 * ModSecurity targeted rule-parser/evaluation fuzz harness.
 */

#include <cstdint>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include "modsecurity/modsecurity.h"
#include "modsecurity/rules_set.h"
#include "modsecurity/transaction.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size == 0) {
        return 0;
    }

    std::string payload(reinterpret_cast<const char *>(data), size);
    for (char &c : payload) {
        if (c == '\0') {
            c = 'A';
        }
    }

    modsecurity::ModSecurity ms;
    modsecurity::RulesSet rules;

    std::string dynamicRule =
        "SecRuleEngine On\n"
        "SecRule ARGS \"@rx " + payload +
        "\" \"id:2001,phase:2,deny,status:403,t:none,t:urlDecodeUni\"\n";

    rules.load(dynamicRule.c_str());

    modsecurity::Transaction t(&ms, &rules, nullptr);
    t.processConnection("127.0.0.1", 49152, "127.0.0.1", 8080);
    t.processURI("/fuzz?a=b&payload=test", "POST", "1.1");
    t.addRequestHeader("Host", "fuzz.local");
    t.addRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    t.addRequestHeader("Transfer-Encoding", "chunked");
    t.processRequestHeaders();

    const std::string chunkedBody = "4\r\na=1&\r\n6\r\nb=%3C%3E\r\n0\r\n\r\n";
    t.appendRequestBody(reinterpret_cast<const unsigned char *>(chunkedBody.data()), chunkedBody.size());
    t.processRequestBody();

    return 0;
}

int main() {
    std::vector<uint8_t> data((std::istreambuf_iterator<char>(std::cin)), std::istreambuf_iterator<char>());
    LLVMFuzzerTestOneInput(data.data(), data.size());
    return 0;
}
