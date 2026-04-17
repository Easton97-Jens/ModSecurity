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

#include "src/json_schema/blaze_bridge.h"

#include <filesystem>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <sourcemeta/blaze/compiler.h>
#include <sourcemeta/blaze/evaluator.h>
#include <sourcemeta/core/json.h>
#include <sourcemeta/core/jsonschema.h>

namespace {

thread_local std::string g_last_error;

enum class ContainerType {
    Object,
    Array
};

struct ContainerFrame {
    ContainerType type;
    sourcemeta::core::JSON value;
    std::string pending_key;
    bool has_pending_key{false};
};

auto setLastError(std::string message) -> void {
    g_last_error = std::move(message);
}

auto clearLastError() -> void {
    g_last_error.clear();
}

auto requireText(const msc_blaze_event &event,
    const char *label) -> std::string_view {
    if (event.text_length == 0U) {
        return std::string_view();
    }

    if (event.text == nullptr) {
        throw std::runtime_error(std::string(label)
            + " event is missing text storage");
    }

    return std::string_view(event.text, event.text_length);
}

auto appendValue(std::optional<sourcemeta::core::JSON> *root,
    std::vector<ContainerFrame> *stack, sourcemeta::core::JSON value) -> void {
    if (!stack->empty()) {
        ContainerFrame &parent = stack->back();
        if (parent.type == ContainerType::Array) {
            parent.value.push_back(std::move(value));
            return;
        }

        if (!parent.has_pending_key) {
            throw std::runtime_error("Object value is missing a preceding key");
        }

        // Core object assignment overwrites existing keys, which gives us the
        // documented "last duplicate wins" semantics for duplicate keys.
        parent.value.assign(parent.pending_key, std::move(value));
        parent.pending_key.clear();
        parent.has_pending_key = false;
        return;
    }

    if (root->has_value()) {
        throw std::runtime_error(
            "Validation input contains multiple root JSON values");
    }

    *root = std::move(value);
}

auto materializeJson(const msc_blaze_event *events, size_t event_count)
    -> sourcemeta::core::JSON {
    if (event_count > 0U && events == nullptr) {
        throw std::runtime_error("Validation input events pointer is null");
    }

    std::optional<sourcemeta::core::JSON> root;
    std::vector<ContainerFrame> stack;
    stack.reserve(event_count);

    for (size_t index = 0; index < event_count; index++) {
        const msc_blaze_event &event = events[index];
        switch (event.type) {
            case MSC_BLAZE_EVENT_START_OBJECT:
                stack.push_back(ContainerFrame{ContainerType::Object,
                    sourcemeta::core::JSON::make_object(), "", false});
                break;
            case MSC_BLAZE_EVENT_END_OBJECT: {
                if (stack.empty() || stack.back().type != ContainerType::Object) {
                    throw std::runtime_error("Unexpected end-object event");
                }
                if (stack.back().has_pending_key) {
                    throw std::runtime_error(
                        "Object ended while still waiting for a value");
                }

                sourcemeta::core::JSON value = std::move(stack.back().value);
                stack.pop_back();
                appendValue(&root, &stack, std::move(value));
                break;
            }
            case MSC_BLAZE_EVENT_START_ARRAY:
                stack.push_back(ContainerFrame{ContainerType::Array,
                    sourcemeta::core::JSON::make_array(), "", false});
                break;
            case MSC_BLAZE_EVENT_END_ARRAY: {
                if (stack.empty() || stack.back().type != ContainerType::Array) {
                    throw std::runtime_error("Unexpected end-array event");
                }

                sourcemeta::core::JSON value = std::move(stack.back().value);
                stack.pop_back();
                appendValue(&root, &stack, std::move(value));
                break;
            }
            case MSC_BLAZE_EVENT_KEY: {
                if (stack.empty() || stack.back().type != ContainerType::Object) {
                    throw std::runtime_error("Key event outside of an object");
                }

                ContainerFrame &frame = stack.back();
                frame.pending_key.assign(requireText(event, "key"));
                frame.has_pending_key = true;
                break;
            }
            case MSC_BLAZE_EVENT_STRING:
                appendValue(&root, &stack, sourcemeta::core::JSON{
                    requireText(event, "string")});
                break;
            case MSC_BLAZE_EVENT_NUMBER: {
                const std::string_view raw_number = requireText(event, "number");
                sourcemeta::core::JSON value =
                    sourcemeta::core::parse_json(std::string(raw_number));
                if (!value.is_number()) {
                    throw std::runtime_error(
                        "Number event did not materialize into a JSON number");
                }
                appendValue(&root, &stack, std::move(value));
                break;
            }
            case MSC_BLAZE_EVENT_BOOLEAN:
                appendValue(&root, &stack, sourcemeta::core::JSON{
                    event.boolean_value != 0});
                break;
            case MSC_BLAZE_EVENT_NULL:
                appendValue(&root, &stack, sourcemeta::core::JSON{nullptr});
                break;
            default:
                throw std::runtime_error("Unknown validation input event type");
        }
    }

    if (!stack.empty()) {
        throw std::runtime_error("Validation input ended with open containers");
    }

    if (!root.has_value()) {
        throw std::runtime_error(
            "Validation input did not contain a root JSON value");
    }

    return std::move(root.value());
}

}  // namespace

struct msc_blaze_schema_handle {
    explicit msc_blaze_schema_handle(sourcemeta::blaze::Template compiled)
        : compiled_schema(std::move(compiled)) {
    }

    sourcemeta::blaze::Template compiled_schema;
    sourcemeta::blaze::Evaluator evaluator;
};

extern "C" const char *msc_blaze_last_error(void) {
    return g_last_error.c_str();
}

extern "C" int msc_blaze_schema_create_from_file(const char *path,
    size_t path_length, msc_blaze_schema_handle **out_handle) {
    clearLastError();

    if (out_handle == nullptr) {
        setLastError("Blaze schema output handle is null");
        return MSC_BLAZE_BRIDGE_STATUS_SCHEMA_ERROR;
    }

    *out_handle = nullptr;
    if (path == nullptr || path_length == 0U) {
        setLastError("Blaze schema path is empty");
        return MSC_BLAZE_BRIDGE_STATUS_SCHEMA_ERROR;
    }

    try {
        const std::string path_string(path, path_length);
        const sourcemeta::core::JSON schema =
            sourcemeta::core::read_json(std::filesystem::path(path_string));
        sourcemeta::blaze::Template compiled = sourcemeta::blaze::compile(
            schema, sourcemeta::core::schema_official_walker,
            sourcemeta::core::schema_official_resolver,
            sourcemeta::blaze::default_schema_compiler,
            sourcemeta::blaze::Mode::FastValidation);
        *out_handle = new msc_blaze_schema_handle(std::move(compiled));
        return MSC_BLAZE_BRIDGE_STATUS_OK;
    } catch (const std::exception &exception) {
        setLastError(exception.what());
        return MSC_BLAZE_BRIDGE_STATUS_SCHEMA_ERROR;
    } catch (...) {
        setLastError("Unknown Blaze schema load failure");
        return MSC_BLAZE_BRIDGE_STATUS_SCHEMA_ERROR;
    }
}

extern "C" void msc_blaze_schema_destroy(msc_blaze_schema_handle *handle) {
    delete handle;
}

extern "C" int msc_blaze_schema_validate(msc_blaze_schema_handle *handle,
    const struct msc_blaze_event *events, size_t event_count) {
    clearLastError();

    if (handle == nullptr) {
        setLastError("Blaze schema handle is null");
        return MSC_BLAZE_BRIDGE_STATUS_SCHEMA_ERROR;
    }

    try {
        const sourcemeta::core::JSON instance = materializeJson(events,
            event_count);
        try {
            const bool valid =
                handle->evaluator.validate(handle->compiled_schema, instance);
            return valid ? MSC_BLAZE_BRIDGE_STATUS_VALID
                : MSC_BLAZE_BRIDGE_STATUS_INVALID;
        } catch (const std::exception &exception) {
            setLastError(exception.what());
            return MSC_BLAZE_BRIDGE_STATUS_SCHEMA_ERROR;
        } catch (...) {
            setLastError("Unknown Blaze validation failure");
            return MSC_BLAZE_BRIDGE_STATUS_SCHEMA_ERROR;
        }
    } catch (const std::exception &exception) {
        setLastError(exception.what());
        return MSC_BLAZE_BRIDGE_STATUS_INPUT_ERROR;
    } catch (...) {
        setLastError("Unknown Blaze input materialization failure");
        return MSC_BLAZE_BRIDGE_STATUS_INPUT_ERROR;
    }
}
