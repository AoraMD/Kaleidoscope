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

#include <string>

#include "runtime.h"

#include "log.h"
#include "internal.h"
#include "macro.h"

#include "bridge.h"
#include "mirror.h"

namespace moe::aoramd::kaleidoscope::runtime {

    int Runtime::android_version_ = AndroidVersion::kInDevelopment;
    int Runtime::preview_android_version_ = AndroidVersion::kInDevelopment;

    std::map<int, mirror::Method *> Runtime::bridge_runtime_method_;

    InsertBridgeResult::InsertBridgeResult(mirror::Method *origin) :
            origin_(origin) {
        bridge_box_ = new Box;
    }

    InsertBridgeResult::~InsertBridgeResult() {
        delete bridge_box_;
    }

    ListenResult::ListenResult(mirror::Method *origin) : InsertBridgeResult(origin) {
        clone_ = reinterpret_cast<mirror::Method *>(malloc(mirror::Method::GetSize()));
        debugLog("Original runtime method " __log_memory_specifier__ " data : %s.",
                 reinterpret_cast<std::size_t>(origin), origin->GetDataHexString().c_str())
        internal::Memory::Copy(clone_, origin, mirror::Method::GetSize());
        debugLog("Clone runtime method " __log_memory_specifier__ " data : %s.",
                 reinterpret_cast<std::size_t>(clone_), clone_->GetDataHexString().c_str())
        clone_->SetPrivate();
    }

    ListenResult::~ListenResult() {
        free(clone_);
        clone_ = nullptr;
    }

    bool Runtime::InitializeConfiguration() {
        char api_level[5];
        if (__system_property_get("ro.build.version.sdk", api_level) < 1) {
            errorLog("Unable to get system property ro.build.version.sdk.")
            return false;
        }
        android_version_ = static_cast<int>(strtol(api_level, nullptr, 10));
        char preview_api_level[5];
        if (__system_property_get("ro.build.version.preview_sdk", preview_api_level) < 1) {
            errorLog("Unable to get system property ro.build.version.preview.sdk.")
            return false;
        }
        preview_android_version_ = static_cast<int>(strtol(preview_api_level, nullptr, 10));
        return true;
    }

    bool Runtime::Initialize() {

        // Initialize scoped suspend threads tool class.
        if (!ScopedSuspendAll::Initialize()) {
            errorLog("Initialize scoped suspend threads tool class failed.")
            return false;
        }

        return true;
    }

    ListenResult *
    Runtime::ListenBridge(mirror::Method *method, mirror::Thread *current_thread,
                          int bridge_type_key) {
        method->Compile(current_thread);
        auto *result = new ListenResult(method);
        if (Bridge(method, result, bridge_type_key)) return result;
        delete result;
        return nullptr;
    }

    ReplaceResult *
    Runtime::ReplaceBridge(mirror::Method *method, mirror::Thread *current_thread,
                           int bridge_type_key) {
        method->Compile(current_thread);
        auto *result = new ReplaceResult(method);
        if (Bridge(method, result, bridge_type_key)) return result;
        delete result;
        return nullptr;
    }

    void Runtime::RestoreBridge(InsertBridgeResult *result) {
        if (result->secondary_bridge_ != nullptr) {
            free(result->secondary_bridge_);
        }
        if (result->origin_bridge_ != nullptr) {
            bridge::Bridge::RecoverMain(
                    result->origin_->GetEntryPointFromQuickCompiledCode(),
                    result->origin_bridge_);
            free(result->origin_bridge_);
        }
        delete result;
    }

    void Runtime::RegisterBridgeMethod(int key, mirror::Thread *current_thread,
                                       mirror::Method *bridge_method) {
        bridge_method->Compile(current_thread);
        bridge_runtime_method_[key] = bridge_method;
    }

    Box *Runtime::UnlockAndCopyBox(Box *origin) {
        auto *clone = reinterpret_cast<Box *>(malloc(sizeof(Box)));
        internal::Memory::Copy(clone, origin, sizeof(Box));
        // Unlock.
        origin->lock_ = 0;
        return clone;
    }

    bool Runtime::AndroidVersionAtLeast(AndroidVersion version, bool warnDevelopment) {
        if (warnDevelopment) {
            if (android_version_ == AndroidVersion::kInDevelopment ||
                (android_version_ == AndroidVersion::kInDevelopment - 1 &&
                 preview_android_version_ > 0)) {
                warnLog("Current Android version is higher than highest supported version, unexpected error may occur.")
            }
        }
        return android_version_ >= version;
    }

    bool
    Runtime::Bridge(mirror::Method *method, InsertBridgeResult *result, int bridge_type_key) {

        void *entrance = method->GetEntryPointFromQuickCompiledCode();

        // Create origin bridge.
        result->origin_bridge_ = bridge::Bridge::CreateOrigin(entrance);
        if (result->origin_bridge_ == nullptr) {
            errorLog("Unable to create origin bridge for runtime method " __log_memory_specifier__ ".",
                     reinterpret_cast<std::size_t>(method))
            return false;
        }

        mirror::Method *bridge_runtime_method = bridge_runtime_method_[bridge_type_key];
        if (bridge_runtime_method == nullptr) {
            errorLog("Unable to find bridge method for runtime method " __log_memory_specifier__ ".",
                     reinterpret_cast<std::size_t>(method))
            return false;
        }

        // Create secondary bridge.
        result->secondary_bridge_ = bridge::Bridge::CreateSecondary(
                method,
                bridge_runtime_method,
                bridge_runtime_method->GetEntryPointFromQuickCompiledCode(),
                result->bridge_box_,
                result->origin_bridge_
        );
        if (result->secondary_bridge_ == nullptr) {
            errorLog("Unable to create secondary bridge for runtime method "  __log_memory_specifier__ ".",
                     reinterpret_cast<std::size_t>(method))
            return false;
        }

        {
            ScopedSuspendAll suspendAll;

            // Insert main bridge.
            if (!bridge::Bridge::SetMain(entrance, result->secondary_bridge_)) {
                errorLog("Unable to set main bridge for runtime method " __log_memory_specifier__ ".",
                         reinterpret_cast<std::size_t>(method))
                return false;
            }
        }
        return true;
    }

    void
    (*Runtime::ScopedSuspendAll::suspend_function_)(ScopedSuspendAll *, const char *) = nullptr;

    void (*Runtime::ScopedSuspendAll::resume_function_)(ScopedSuspendAll *) = nullptr;

    bool Runtime::ScopedSuspendAll::Initialize() {
        suspend_function_ = reinterpret_cast<void (*)(ScopedSuspendAll *, const char *)>(
                internal::Library::SymbolInArtLibrary(kFunctionSuspendAllSymbol));
        resume_function_ = reinterpret_cast<void (*)(ScopedSuspendAll *)>(
                internal::Library::SymbolInArtLibrary(kFunctionResumeAllSymbol));
        return true;
    }

    Runtime::ScopedSuspendAll::ScopedSuspendAll(const char *cause) {
        if (suspend_function_) {
            suspend_function_(this, cause);
            debugLog("Scoped suspend all thread.")
        }
    }

    Runtime::ScopedSuspendAll::~ScopedSuspendAll() {
        if (resume_function_) {
            resume_function_(this);
            debugLog("Scoped resume all thread.")
        }
    }
}