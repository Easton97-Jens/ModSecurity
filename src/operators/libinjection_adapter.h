/*
 * ModSecurity, http://www.modsecurity.org/
 */

#ifndef SRC_OPERATORS_LIBINJECTION_ADAPTER_H_
#define SRC_OPERATORS_LIBINJECTION_ADAPTER_H_

#include <cstddef>

#include "libinjection/src/libinjection_error.h"

namespace modsecurity::operators {

using DetectSQLiFn = injection_result_t (*)(const char *, size_t, char *);
using DetectXSSFn = injection_result_t (*)(const char *, size_t);

injection_result_t runLibinjectionSQLi(const char *input, size_t len,
    char *fingerprint);
injection_result_t runLibinjectionXSS(const char *input, size_t len);

void setLibinjectionSQLiOverrideForTesting(DetectSQLiFn fn);
void setLibinjectionXSSOverrideForTesting(DetectXSSFn fn);
void clearLibinjectionOverridesForTesting();

}  // namespace modsecurity::operators

#endif  // SRC_OPERATORS_LIBINJECTION_ADAPTER_H_
