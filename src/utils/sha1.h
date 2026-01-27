/*
 * ModSecurity, http://www.modsecurity.org/
 * Copyright (c) 2015 - 2021 Trustwave Holdings, Inc.
 *
 * Licensed under the Apache License, Version 2.0
 */

#ifndef SRC_UTILS_SHA1_H_
#define SRC_UTILS_SHA1_H_

#include <array>
#include <cstddef>    // std::byte
#include <cstring>    // std::memcpy
#include <mutex>      // std::once_flag, std::call_once
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "src/utils/string.h"

// PSA instead of mbedtls/sha1.h
#include <psa/crypto.h>

namespace modsecurity::Utils {

// Digest operation: takes input bytes and writes DigestSize bytes to output.
template <std::size_t DigestSize>
using DigestOp = int (*)(std::span<const std::byte> input,
                         std::span<std::byte, DigestSize> output);

// Shared, thread-safe PSA initialization for all digests.
namespace detail {

inline bool ensure_psa_init() {
    static std::once_flag once;
    static psa_status_t init_status = PSA_ERROR_GENERIC_ERROR;

    std::call_once(once, []() { init_status = psa_crypto_init(); });

    return init_status == PSA_SUCCESS;
}

inline std::span<const std::byte> to_bytes(std::string_view s) noexcept {
    const std::span<const char> chars{s.data(), s.size()};
    return std::as_bytes(chars);
}

}  // namespace detail

template <auto DigestFn, std::size_t DigestSize>
class DigestImpl {
 public:
    static std::string digest(const std::string& input) {
        return digestHelper(input, [](std::string_view d) { return std::string{d}; });
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

        std::array<std::byte, DigestSize> digest_bytes{};

        if (DigestFn(detail::to_bytes(input),
                     std::span<std::byte, DigestSize>{digest_bytes}) != 0) {
            // Empty digest signals an error.
            return convertOp(std::string_view{});
        }

        // Convert byte array to a binary std::string without pointer punning.
        std::string raw(DigestSize, '\0');
        std::memcpy(raw.data(), digest_bytes.data(), DigestSize);

        return convertOp(std::string_view{raw});
    }
};

// PSA wrapper for SHA-1 (legacy-friendly error convention: 0 = success, non-zero = error).
inline int modsec_psa_sha1(std::span<const std::byte> input,
                           std::span<std::byte, 20> output) {
    if (!detail::ensure_psa_init()) {
        return -1;
    }

    // psa_hash_compute uses uint8_t; copy to avoid unsafe casts.
    std::vector<uint8_t> input_u8(input.size());
    std::memcpy(input_u8.data(), input.data(), input.size());

    std::array<uint8_t, 20> output_u8{};
    size_t out_len = 0;

    const auto status = psa_hash_compute(
        PSA_ALG_SHA_1,
        input_u8.data(),
        input_u8.size(),
        output_u8.data(),
        output_u8.size(),
        &out_len
    );

    if (status != PSA_SUCCESS || out_len != output_u8.size()) {
        return -1;
    }

    std::memcpy(output.data(), output_u8.data(), output_u8.size());
    return 0;
}

class Sha1 : public DigestImpl<&modsec_psa_sha1, 20> {};

}  // namespace modsecurity::Utils

#endif  // SRC_UTILS_SHA1_H_
