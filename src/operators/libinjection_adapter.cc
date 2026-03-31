/*
 * ModSecurity, http://www.modsecurity.org/
 */

#include "src/operators/libinjection_adapter.h"

#include "libinjection/src/libinjection.h"

namespace modsecurity::operators {
namespace {
// Per-thread overrides avoid cross-thread interference during mtstress tests.
thread_local DetectSQLiFn g_sqli_override = nullptr;
thread_local DetectXSSFn g_xss_override = nullptr;
}

injection_result_t runLibinjectionSQLi(const char *input, size_t len,
    char *fingerprint) {
    if (DetectSQLiFn fn = g_sqli_override) {
        return fn(input, len, fingerprint);
    }

    return libinjection_sqli(input, len, fingerprint);
}

injection_result_t runLibinjectionXSS(const char *input, size_t len) {
    if (DetectXSSFn fn = g_xss_override) {
        return fn(input, len);
    }

    return libinjection_xss(input, len);
}

void setLibinjectionSQLiOverrideForTesting(DetectSQLiFn fn) {
    g_sqli_override = fn;
}

void setLibinjectionXSSOverrideForTesting(DetectXSSFn fn) {
    g_xss_override = fn;
}

void clearLibinjectionOverridesForTesting() {
    g_sqli_override = nullptr;
    g_xss_override = nullptr;
}

}  // namespace modsecurity::operators
