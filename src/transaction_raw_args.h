/*
 * ModSecurity, http://www.modsecurity.org/
 */

#ifndef SRC_TRANSACTION_RAW_ARGS_H_
#define SRC_TRANSACTION_RAW_ARGS_H_

#include <string>

#include "modsecurity/transaction.h"

namespace modsecurity {

inline void addRawArgument(Transaction *t, const std::string &orig,
    const std::string &key, const std::string &value, size_t offset) {
    offset = offset + key.size() + 1;
    t->m_variableArgsRaw.set(key, value, offset);

    if (orig == "GET") {
        t->m_variableArgsGetRaw.set(key, value, offset);
    } else if (orig == "POST") {
        t->m_variableArgsPostRaw.set(key, value, offset);
    }
}

}  // namespace modsecurity

#endif  // SRC_TRANSACTION_RAW_ARGS_H_
