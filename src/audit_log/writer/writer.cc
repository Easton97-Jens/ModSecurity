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

#include "src/audit_log/writer/writer.h"

#include <random>
#include <string>

#include "modsecurity/audit_log.h"

namespace modsecurity {
namespace audit_log {
namespace writer {

namespace {
char randomBoundaryChar() {
    static constexpr char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    static thread_local std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<size_t> dist(0, sizeof(alphanum) - 2);

    return alphanum[dist(gen)];
}
}  // namespace

void Writer::generateBoundary(std::string *boundary) {
    for (int i = 0; i < SERIAL_AUDIT_LOG_BOUNDARY_LENGTH; ++i) {
        boundary->append(1, randomBoundaryChar());
    }
}

}  // namespace writer
}  // namespace audit_log
}  // namespace modsecurity
