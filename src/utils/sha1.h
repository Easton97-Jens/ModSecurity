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

#ifndef SRC_UTILS_SHA1_H_
#define SRC_UTILS_SHA1_H_

#include <string>
#include <cassert>

#include "src/utils/string.h"
#include "mbedtls/md.h"

namespace modsecurity::Utils {


template<mbedtls_md_type_t DigestType, int DigestSize>
class DigestImpl {
 public:

    static std::string digest(const std::string& input) {
        return digestHelper(input, [](const auto digest) { 
            return std::string(digest);
        });
    }

    static void digestReplace(std::string& value) {
        digestHelper(value, [&value](const auto digest) mutable { 
            value = digest;
        });
    }

    static std::string hexdigest(const std::string &input) {
        return digestHelper(input, [](const auto digest) { 
            return utils::string::string_to_hex(digest);
        });
    }

private:

    template<typename ConvertOp>
    static auto digestHelper(const std::string &input,
        ConvertOp convertOp) -> auto {
        char digest[DigestSize];
        const auto *mdInfo = mbedtls_md_info_from_type(DigestType);
        assert(mdInfo != nullptr);

        const auto ret = mbedtls_md(mdInfo,
                 reinterpret_cast<const unsigned char *>(input.c_str()),
                 input.size(), reinterpret_cast<unsigned char *>(digest));
        assert(ret == 0);

        return convertOp(std::string_view(digest, DigestSize));
    }
};


class Sha1 : public DigestImpl<MBEDTLS_MD_SHA1, 20> {
};


}  // namespace modsecurity::Utils

#endif  // SRC_UTILS_SHA1_H_
