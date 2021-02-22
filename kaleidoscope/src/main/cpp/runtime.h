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

#ifndef KALEIDOSCOPE_RUNTIME_H
#define KALEIDOSCOPE_RUNTIME_H

#include <map>

#include "declare.h"

namespace moe::aoramd::kaleidoscope::runtime {

    /**
     * A class whose object is bound to bridge is used to capture registers data.
     */
    class Box final {
        friend class Runtime;

        friend class InsertBridgeResult;

    private:
        /**
         * A spin lock used to ensure concurrency safety.
         */
        [[maybe_unused]] std::size_t lock_ = 0;

    public:
        /**
         * sp register data.
         */
        std::size_t sp_pointer_;

        /**
         * The pointer of callee runtime method.
         * It is used to obtain the target scope at the Java level.
         */
        mirror::Method *callee_runtime_method_pointer_;

        /**
         * x2 register data.
         */
        std::size_t register_1_;

        /**
         * x3 register data.
         */
        std::size_t register_2_;

#if defined(__aarch64__) || defined(__x86_64__)

        /**
         * floating registers data.
         *
         * In arm64, floating registers is d0 ~ d7.
         */
        std::size_t floating_registers_[8];

#elif defined(__arm__) || defined(__i386__)

        /**
         * floating registers data.
         *
         * In x86, floating registers is xmm0 ~ xmm3.
         */
        std::size_t floating_registers_[4];

#endif
    };

    /**
     * Class for saving insert bridge code results.
     */
    class InsertBridgeResult {
        friend class Runtime;

    public:

        /**
         * Runtime method of method inserted into bridge code.
         */
        mirror::Method *origin_;

    protected:
        InsertBridgeResult(mirror::Method *origin);

        ~InsertBridgeResult();

    private:

        /**
         * Secondary bridge code entrance.
         */
        void *secondary_bridge_ = nullptr;

        /**
         * Origin bridge code entrance.
         */
        void *origin_bridge_ = nullptr;

        /**
         * Box for capture data.
         */
        Box *bridge_box_;
    };

    /**
     * Class for saving listen insert bridge code results.
     */
    class ListenResult : public InsertBridgeResult {
        friend class Runtime;

    public:

        /**
         * Copy of runtime method of method inserted into bridge code.
         */
        mirror::Method *clone_;

    private:
        ListenResult(mirror::Method *origin);

        ~ListenResult();
    };

    /**
     * Class for saving replace insert bridge code results.
     */
    class ReplaceResult : public InsertBridgeResult {
        friend class Runtime;

    private:
        ReplaceResult(mirror::Method *origin) :
                InsertBridgeResult(origin) {}
    };

    /**
     * Kaleidoscope native runtime.
     */
    class Runtime final {
    public:
        /**
         * Initialize runtime configurations, such as Android version.
         * 
         * @return true if initialize successfully.
         */
        static bool InitializeConfiguration();

        /**
         * Initialize runtime.
         * 
         * @return true if initialize successfully.
         */
        static bool Initialize();

        /**
         * Insert bridge code into entrance of runtime method for listening method invocation.
         * 
         * @param method runtime method inserted into bridge code.
         * @param current_thread current thread native peer.
         * @param bridge_type_key bridge method key.
         * @return result of insert or null on failure.
         */
        static ListenResult *
        ListenBridge(mirror::Method *method, mirror::Thread *current_thread, int bridge_type_key);

        /**
         * Insert bridge code into entrance of runtime method for replacing method invocation.
         * 
         * @param method runtime method inserted into bridge code.
         * @param current_thread current thread native peer.
         * @param bridge_type_key bridge method key.
         * @return result of insert or null on failure.
         */
        static ReplaceResult *
        ReplaceBridge(mirror::Method *method, mirror::Thread *current_thread, int bridge_type_key);

        /**
         * Recover runtime method entrance and free related resources.
         *
         * @param result result will be free.
         */
        static void RestoreBridge(InsertBridgeResult *result);

        /**
         * Register bridge method into runtime.
         *
         * @param key bridge method key.
         * @param current_thread current thread native peer.
         * @param bridge_runtime_method runtime method of bridge method.
         */
        static void RegisterBridgeMethod(int key, mirror::Thread *current_thread,
                                         mirror::Method *bridge_runtime_method);

        /**
         * Unlock spin lock of box and return a copy of box. In order to ensure that spin lock is
         * not occupied while reading data to improve performance, the copy should be used for
         * reading data instead of the origin one. The spin lock will be released immediately after
         * completing copy.
         *
         * @param origin the origin box.
         * @return copy of origin box.
         */
        static Box *UnlockAndCopyBox(Box *origin);

        /**
         * Check whether current Android version is equal to or higher than the specified.
         *
         * @param version specified Android version.
         * @return true if equal or higher.
         */
        static bool AndroidVersionAtLeast(AndroidVersion version, bool warnDevelopment = false);

    private:
        static bool
        Bridge(mirror::Method *method, InsertBridgeResult *result, int bridge_type_key);

        static int android_version_;
        static int preview_android_version_;

        static std::map<int, mirror::Method *> bridge_runtime_method_;

        /**
         * A tool class for scoped suspending all thread.
         */
        class ScopedSuspendAll final {
            friend class Runtime;

        private:
            static bool Initialize();

            ScopedSuspendAll(const char *cause = "Kaleidoscope Internal Scope");

            ~ScopedSuspendAll();

            static void (*suspend_function_)(ScopedSuspendAll *, const char *);

            static void (*resume_function_)(ScopedSuspendAll *);

            static constexpr const char *FUNCTION_SUSPEND_ALL_SYMBOL = "_ZN3art16ScopedSuspendAllC1EPKcb";
            static constexpr const char *FUNCTION_RESUME_ALL_SYMBOL = "_ZN3art16ScopedSuspendAllD1Ev";
        };
    };
}

#endif
