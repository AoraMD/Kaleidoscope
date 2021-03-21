/*
 * MIT License
 *
 * Copyright (C) 2021 M.D.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "bridge.h"

#include "log.h"
#include "internal.h"

#include "mirror.h"

namespace moe::aoramd::kaleidoscope::bridge {

    bool Bridge::SetMain(void *origin_entrance, void *secondary_bridge) {
        // Check whether compiled code size is less than main bridge.
        auto *size_pointer = reinterpret_cast<std::int32_t *>(
                reinterpret_cast<std::size_t>(origin_entrance) - sizeof(std::int32_t));
        if (*size_pointer < kMainBridgeSize) {
            errorLog(
                    "Method code length is less than main bridge code length, so it cannot be inserted.")
            return false;
        }

        if (!internal::Memory::Unprotect(origin_entrance, kMainBridgeSize)) {
            errorLog(
                    "Unable to disable memory protection on runtime method entry point " __log_memory_specifier__ ".",
                    reinterpret_cast<std::size_t>(origin_entrance))
            return false;
        }
        internal::Memory::Copy(origin_entrance, reinterpret_cast<void *>(MainBridge),
                               kMainBridgeSize);

        // Set parameter - target.
        *reinterpret_cast<void **>(
                reinterpret_cast<std::size_t>(origin_entrance) + kMainBridgeTargetOffset
        ) = secondary_bridge;

        return true;
    }

    void Bridge::RecoverMain(void *entrance, void *origin_bridge) {
        internal::Memory::Copy(entrance, origin_bridge, kMainBridgeSize);
    }

    void *Bridge::CreateSecondary(mirror::Method *source_method, mirror::Method *bridge_method,
                                  void *bridge_entrance, runtime::Box *box,
                                  void *origin_bridge) {
        void *result = malloc(kSecondaryBridgeSize);
        debugLog("Create secondary bridge pointer : " __log_memory_specifier__ ".",
                 reinterpret_cast<std::size_t>(result))
        internal::Memory::Copy(result, reinterpret_cast<void *>(SecondaryBridge),
                               kSecondaryBridgeSize);

        // Set parameter - source method.
        *reinterpret_cast<mirror::Method **>(
                reinterpret_cast<std::size_t>(result) + kSecondaryBridgeSourceMethodOffset
        ) = source_method;
        // Set parameter - bridge method.
        *reinterpret_cast<mirror::Method **>(
                reinterpret_cast<std::size_t>(result) + kSecondaryBridgeBridgeMethodOffset
        ) = bridge_method;
        // Set parameter - bridge entrance.
        *reinterpret_cast<void **>(
                reinterpret_cast<std::size_t>(result) + kSecondaryBridgeBridgeEntranceOffset
        ) = bridge_entrance;
        // Set parameter - bridge box.
        *reinterpret_cast<runtime::Box **>(
                reinterpret_cast<std::size_t>(result) + kSecondaryBridgeBoxPointerOffset
        ) = box;
        // Set parameter - origin bridge.
        *reinterpret_cast<void **>(
                reinterpret_cast<std::size_t>(result) + kSecondaryBridgeOriginBridgeOffset
        ) = origin_bridge;

        // Unprotect memory.
        if (!internal::Memory::Unprotect(result, kSecondaryBridgeSize)) {
            errorLog(
                    "Unable to disable memory protection on secondary bridge " __log_memory_specifier__ ".",
                    reinterpret_cast<std::size_t>(result))
            free(result);
            return nullptr;
        }
        return result;
    }

    void *Bridge::CreateOrigin(void *origin_entrance) {

        std::int32_t code_size = *reinterpret_cast<std::int32_t *>(
                reinterpret_cast<std::size_t>(origin_entrance) - sizeof(std::int32_t));

        debugLog("Get origin code size %d", code_size)

        void *result = malloc(code_size);
        debugLog("Create origin bridge pointer : " __log_memory_specifier__ ".",
                 reinterpret_cast<std::size_t>(result))

        // Unprotect memory.
        if (!internal::Memory::Unprotect(result, code_size)) {
            errorLog(
                    "Unable to disable memory protection on origin bridge " __log_memory_specifier__ ".",
                    reinterpret_cast<std::size_t>(result))
            free(result);
            return nullptr;
        }

        internal::Memory::Copy(result, origin_entrance, code_size);

        return result;
    }
}