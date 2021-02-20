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
                reinterpret_cast<std::uint64_t>(origin_entrance) - 4);
        if (*size_pointer < MAIN_BRIDGE_SIZE) {
            errorLog(
                    "Method code length is less than main bridge code length, so it cannot be inserted.")
            return false;
        }

        if (!internal::Memory::Unprotect(origin_entrance, MAIN_BRIDGE_SIZE)) {
            errorLog(
                    "Unable to disable memory protection on runtime method entry point 0x%016lx",
                    reinterpret_cast<std::uint64_t>(origin_entrance))
            return false;
        }
        internal::Memory::Copy(origin_entrance, reinterpret_cast<void *>(MainBridge),
                               MAIN_BRIDGE_SIZE);

        // Set parameter - target.
        *reinterpret_cast<std::uint64_t *>(
                reinterpret_cast<std::uint64_t>(origin_entrance) +
                MAIN_BRIDGE_TARGET_OFFSET
        ) = reinterpret_cast<std::uint64_t>(secondary_bridge);

        return true;
    }

    void Bridge::RecoverMain(void *entrance, void *origin_bridge) {
        internal::Memory::Copy(entrance, origin_bridge, MAIN_BRIDGE_SIZE);
    }

    void *Bridge::CreateSecondary(mirror::Method *source_method, mirror::Method *bridge_method,
                                  void *bridge_entrance, runtime::Box *box,
                                  void *origin_bridge) {
        void *result = malloc(SECONDARY_BRIDGE_SIZE);
        debugLog("Create secondary bridge pointer : 0x%016lx",
                 reinterpret_cast<std::uint64_t>(result))
        internal::Memory::Copy(result, reinterpret_cast<void *>(SecondaryBridge),
                               SECONDARY_BRIDGE_SIZE);

        // Set parameter - source method.
        *reinterpret_cast<std::uint64_t *>(
                reinterpret_cast<std::uint64_t>(result) +
                SECONDARY_BRIDGE_SOURCE_METHOD_OFFSET
        ) = reinterpret_cast<std::uint64_t>(source_method);
        // Set parameter - bridge method.
        *reinterpret_cast<std::uint64_t *>(
                reinterpret_cast<std::uint64_t>(result) +
                SECONDARY_BRIDGE_BRIDGE_METHOD_OFFSET
        ) = reinterpret_cast<std::uint64_t>(bridge_method);
        // Set parameter - bridge entrance.
        *reinterpret_cast<std::uint64_t *>(
                reinterpret_cast<std::uint64_t>(result) +
                SECONDARY_BRIDGE_BRIDGE_ENTRANCE_OFFSET
        ) = reinterpret_cast<std::uint64_t>(bridge_entrance);
        // Set parameter - bridge box.
        *reinterpret_cast<std::uint64_t *>(
                reinterpret_cast<std::uint64_t>(result) +
                SECONDARY_BRIDGE_BRIDGE_BOX_POINTER_OFFSET
        ) = reinterpret_cast<std::uint64_t>(box);
        // Set parameter - origin bridge.
        *reinterpret_cast<std::uint64_t *>(
                reinterpret_cast<std::uint64_t>(result) +
                SECONDARY_BRIDGE_ORIGIN_BRIDGE_OFFSET
        ) = reinterpret_cast<std::uint64_t>(origin_bridge);

        // Unprotect memory.
        if (!internal::Memory::Unprotect(result, SECONDARY_BRIDGE_SIZE)) {
            errorLog(
                    "Unable to disable memory protection on secondary bridge 0x%016lx",
                    reinterpret_cast<std::uint64_t>(result))
            free(result);
            return nullptr;
        }
        return result;
    }

    void *Bridge::CreateOrigin(void *origin_entrance) {
        void *result = malloc(ORIGIN_BRIDGE_SIZE);
        debugLog("Create origin bridge pointer : 0x%016lx", reinterpret_cast<std::uint64_t>(result))
        internal::Memory::Copy(result, reinterpret_cast<void *>(OriginBridge), ORIGIN_BRIDGE_SIZE);
        internal::Memory::Copy(result, origin_entrance, MAIN_BRIDGE_SIZE);

        // Set parameter - left.
        *reinterpret_cast<std::uint64_t *>(
                reinterpret_cast<std::uint64_t>(result) +
                ORIGIN_BRIDGE_LEFT_OFFSET
        ) = reinterpret_cast<std::uint64_t>(origin_entrance) + MAIN_BRIDGE_SIZE;

        // Unprotect memory.
        if (!internal::Memory::Unprotect(result, ORIGIN_BRIDGE_SIZE)) {
            errorLog(
                    "Unable to disable memory protection on origin bridge 0x%016lx",
                    reinterpret_cast<std::uint64_t>(result))
            free(result);
            return nullptr;
        }
        return result;
    }
}