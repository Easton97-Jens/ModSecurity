/*
 * ModSecurity, http://www.modsecurity.org/
 */

#include "src/operators/libinjection_adapter.h"

#include <atomic>

#include "libinjection/src/libinjection.h"

namespace modsecurity::operators {
namespace {
std::atomic<DetectSQLiFn> g_sqli_override{nullptr};
std::atomic<DetectXSSFn> g_xss_override{nullptr};
}

injection_result_t runLibinjectionSQLi(const char *input, size_t len,
    char *fingerprint) {
    if (DetectSQLiFn fn = g_sqli_override.load(std::memory_order_acquire)) {
        return fn(input, len, fingerprint);
    }

    return libinjection_sqli(input, len, fingerprint);
}

injection_result_t runLibinjectionXSS(const char *input, size_t len) {
    if (DetectXSSFn fn = g_xss_override.load(std::memory_order_acquire)) {
        return fn(input, len);
    }

    return libinjection_xss(input, len);
}

void setLibinjectionSQLiOverrideForTesting(DetectSQLiFn fn) {
    g_sqli_override.store(fn, std::memory_order_release);
}

void setLibinjectionXSSOverrideForTesting(DetectXSSFn fn) {
    g_xss_override.store(fn, std::memory_order_release);
}

void clearLibinjectionOverridesForTesting() {
    g_sqli_override.store(nullptr, std::memory_order_release);
    g_xss_override.store(nullptr, std::memory_order_release);
}

}  // namespace modsecurity::operators
