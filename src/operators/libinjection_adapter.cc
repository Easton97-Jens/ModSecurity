/*
 * ModSecurity, http://www.modsecurity.org/
 */

#include "src/operators/libinjection_adapter.h"

#include "libinjection/src/libinjection.h"

namespace modsecurity::operators {
namespace {

// Per-thread overrides avoid cross-thread interference during mtstress tests.
// Intentional design:
// - thread_local to isolate tests across threads
// - function pointers to keep zero-overhead call path
// - mutable for test injection hooks
// NOSONAR: required for testing override mechanism (see set*OverrideForTesting)
thread_local DetectSQLiFn g_sqli_override = nullptr;  // NOSONAR
thread_local DetectXSSFn g_xss_override = nullptr;    // NOSONAR

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

// Test-only hook: allows injecting alternative detection functions
// NOSONAR: function pointer is intentional (no std::function overhead)
void setLibinjectionSQLiOverrideForTesting(DetectSQLiFn fn) {  // NOSONAR
    g_sqli_override = fn;
}

// Test-only hook: allows injecting alternative detection functions
// NOSONAR: function pointer is intentional (no std::function overhead)
void setLibinjectionXSSOverrideForTesting(DetectXSSFn fn) {  // NOSONAR
    g_xss_override = fn;
}

void clearLibinjectionOverridesForTesting() {
    g_sqli_override = nullptr;
    g_xss_override = nullptr;
}

}  // namespace modsecurity::operators
