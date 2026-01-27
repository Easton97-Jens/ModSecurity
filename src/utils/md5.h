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

#ifndef SRC_UTILS_MD5_H_
#define SRC_UTILS_MD5_H_

#include "src/utils/sha1.h"   // uses DigestImpl + detail::ensure_psa_init()
#include <string>

#include <psa/crypto.h>       // optional (since sha1.h already includes it), but ok

namespace modsecurity::Utils {

// PSA wrapper with legacy signature
inline int modsec_psa_md5(const unsigned char *input,
                          size_t ilen,
                          unsigned char output[16])
{
    if (!detail::ensure_psa_init()) {
        return -1;
    }

    size_t out_len = 0;
    psa_status_t status = psa_hash_compute(
        PSA_ALG_MD5,
        input,
        ilen,
        output,
        16,
        &out_len
    );

    return (status == PSA_SUCCESS && out_len == 16) ? 0 : -1;
}

class Md5 : public DigestImpl<&modsec_psa_md5, 16> {};

}  // namespace modsecurity::Utils

#endif  // SRC_UTILS_MD5_H_
