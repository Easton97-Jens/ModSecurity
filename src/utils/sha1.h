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

#include <array>
#include <cstddef>
#include <cstring>
#include <mutex>
#include <string>
#include <string_view>

#include "src/utils/string.h"
#include <psa/crypto.h>

namespace modsecurity::Utils {

namespace detail {

// Thread-safe PSA initialization shared by all digests
inline bool ensure_psa_init() {
    static std::once_flag once;
    static psa_status_t init_status = PSA_ERROR_GENERIC_ERROR;

    std::call_once(once, []() { init_status = psa_crypto_init(); });

    return init_status == PSA_SUCCESS;
}

}  // namespace detail

// C-friendly digest function signature (matches legacy wrappers like modsec_psa_md5)
template <std::size_t DigestSize>
using DigestOp = int (*)(const unsigned char* input,
                         std::size_t input_len,
                         unsigned char* output);

// Generic digest implementation
template <auto DigestFn, std::size_t DigestSize>
class DigestImpl {
 public:
    static std::string digest(const std::string& input) {
        return digestHelper(input, [](std::string_view d) {
            return std::string{d};
        });
    }

    static void digestReplace(std::string& value) {
        value = digest(value);
    }

    static std::string hexdigest(const std::string& input) {
        return digestHelper(input, [](std::string_view d) {
            return utils::string::string_to_hex(d);
        });
    }

 private:
    template <typename ConvertOp>
    static auto digestHelper(const std::string& input, ConvertOp convertOp)
        -> decltype(convertOp(std::string_view{})) {

        std::array<unsigned char, DigestSize> out{};
        const std::string_view sv{input};

        const auto* in_ptr =
            reinterpret_cast<const unsigned char*>(sv.data());
        const std::size_t in_len = sv.size();

        if (DigestFn(in_ptr, in_len, out.data()) != 0) {
            return convertOp(std::string_view{});
        }

        std::string raw(DigestSize, '\0');
        std::memcpy(raw.data(), out.data(), DigestSize);
        return convertOp(std::string_view{raw});
    }
};

// PSA wrapper for SHA-1 (0 = success, non-zero = error)
inline int modsec_psa_sha1(const unsigned char* input,
                           std::size_t ilen,
                           unsigned char* output) {
    if (!detail::ensure_psa_init()) {
        return -1;
    }

    size_t out_len = 0;
    psa_status_t status = psa_hash_compute(
        PSA_ALG_SHA_1,
        input,
        ilen,
        output,
        20,
        &out_len
    );

    return (status == PSA_SUCCESS && out_len == 20) ? 0 : -1;
}

class Sha1 : public DigestImpl<&modsec_psa_sha1, 20> {};

}  // namespace modsecurity::Utils

#endif  // SRC_UTILS_SHA1_H_

