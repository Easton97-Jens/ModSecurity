/*
 * ModSecurity, http://www.modsecurity.org/
 * Copyright (c) 2015 - 2024 Trustwave Holdings, Inc. (http://www.trustwave.com/)
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

#ifndef SRC_JSON_SCHEMA_BLAZE_BRIDGE_H_
#define SRC_JSON_SCHEMA_BLAZE_BRIDGE_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct msc_blaze_schema_handle msc_blaze_schema_handle;

enum msc_blaze_event_type {
    MSC_BLAZE_EVENT_START_OBJECT = 0,
    MSC_BLAZE_EVENT_END_OBJECT = 1,
    MSC_BLAZE_EVENT_START_ARRAY = 2,
    MSC_BLAZE_EVENT_END_ARRAY = 3,
    MSC_BLAZE_EVENT_KEY = 4,
    MSC_BLAZE_EVENT_STRING = 5,
    MSC_BLAZE_EVENT_NUMBER = 6,
    MSC_BLAZE_EVENT_BOOLEAN = 7,
    MSC_BLAZE_EVENT_NULL = 8
};

enum msc_blaze_bridge_status {
    MSC_BLAZE_BRIDGE_STATUS_OK = 0,
    MSC_BLAZE_BRIDGE_STATUS_VALID = 1,
    MSC_BLAZE_BRIDGE_STATUS_INVALID = 2,
    MSC_BLAZE_BRIDGE_STATUS_INPUT_ERROR = 3,
    MSC_BLAZE_BRIDGE_STATUS_SCHEMA_ERROR = 4
};

struct msc_blaze_event {
    enum msc_blaze_event_type type;
    const char *text;
    size_t text_length;
    int boolean_value;
};

const char *msc_blaze_last_error(void);

int msc_blaze_schema_create_from_file(const char *path, size_t path_length,
    msc_blaze_schema_handle **out_handle);

void msc_blaze_schema_destroy(msc_blaze_schema_handle *handle);

int msc_blaze_schema_validate(msc_blaze_schema_handle *handle,
    const struct msc_blaze_event *events, size_t event_count);

#ifdef __cplusplus
}
#endif

#endif  // SRC_JSON_SCHEMA_BLAZE_BRIDGE_H_
