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

#include "mirror.h"

#include "log.h"
#include "internal.h"
#include "macro.h"

#include "runtime.h"

namespace moe::aoramd::kaleidoscope::mirror {

    jclass Method::jvm_executable_class_ = nullptr;
    Compiler *Method::compiler_ = nullptr;
    int Method::runtime_method_size_ = -1;
    int Method::entry_point_for_quick_compiled_code_offset_ = -1;
    int Method::access_flag_offset_ = -1;
    void *Method::entry_point_for_jit_compile_ = nullptr;

    template<typename T>
    ALWAYS_INLINE int find_offset(void *start, T target, int range) {
        for (int i = 0; i < range; i++) {
            auto current = reinterpret_cast<T *>(reinterpret_cast<std::size_t>(start) + i);
            if (*current == target) return i;
        }
        return -1;
    }

    /**
     * The mirror class of art::Runtime in Android 11 Runtime.
     *
     * This class is only used to initialize runtime method related functions,
     * so it is not put in the header file.
     */
    class AndroidRuntimeOnR final {
    private:
        [[maybe_unused]] void *java_vm_;
        [[maybe_unused]] void *unused_13_;

        void *jit_code_cache_;

        /**
         * The mirror struct of JavaVMExt.
         */
        struct MirrorJavaVM {
            [[maybe_unused]] void *unused;
            void *runtime;
        };

        static AndroidRuntimeOnR *GetInstanceFromJVM(JNIEnv *env) {
            JavaVM *java_vm;
            env->GetJavaVM(&java_vm);
            auto *mirror_java_vm = reinterpret_cast<MirrorJavaVM *>(java_vm);
            int runtime_offset = find_offset(mirror_java_vm->runtime, java_vm, 2000);
            return reinterpret_cast<AndroidRuntimeOnR *>(
                    reinterpret_cast<char *>(mirror_java_vm->runtime) +
                    runtime_offset - offsetof(AndroidRuntimeOnR, java_vm_));
        }

        friend bool Method::Initialize(JNIEnv *env, Thread *current_thread,
                                       jobject standard_method,
                                       jobject relative_method);
    };

    bool Method::Initialize(JNIEnv *env,
                            Thread *current_thread,
                            jobject standard_method,
                            jobject relative_method) {

        // Initialize Java class references for Android 11.
        if (runtime::Runtime::AndroidVersionAtLeast(runtime::AndroidVersion::kR, true)) {
            jvm_executable_class_ =
                    internal::Jni::GetClassGlobalReference(env, JVM_EXECUTABLE_CLASS_NAME);
            if (env->ExceptionCheck()) env->ExceptionClear();
            if (jvm_executable_class_ == nullptr) {
                errorLog("Cannot find class java.lang.reflect.Executable in runtime.")
                return false;
            }
        }

        // Initialize method compile entrances.
        if (runtime::Runtime::AndroidVersionAtLeast(runtime::AndroidVersion::kR, true)) {
            AndroidRuntimeOnR *runtime = AndroidRuntimeOnR::GetInstanceFromJVM(env);
            if (runtime == nullptr) {
                errorLog("Cannot get runtime reference.")
                return false;
            }
            compiler_ = new CompilerOnR(runtime->jit_code_cache_);
        } else if (runtime::Runtime::AndroidVersionAtLeast(runtime::AndroidVersion::kQ)) {
            compiler_ = new CompilerOnQ();
        } else if (runtime::Runtime::AndroidVersionAtLeast(runtime::AndroidVersion::kNougat)) {
            compiler_ = new CompilerBase();
        }

        // Calculate runtime method size.

        Method *standard_runtime_method = GetFromReflectMethod(env, standard_method);
        auto standard_address =
                reinterpret_cast<std::size_t>(standard_runtime_method);
        auto relative_address =
                reinterpret_cast<std::size_t>(GetFromReflectMethod(env, relative_method));
        // In current test environments (include x86_64 and arm64), relative_method is less than standard_method.
        // However, according to debugging result from LLDB, two standard_method objects are indeed adjacent.
        // TODO: Find out why relative_method is less than standard_method.
        std::size_t size =
                relative_address > standard_address ?
                relative_address - standard_address : standard_address - relative_address;
        runtime_method_size_ = static_cast<int>(size);

        // Calculate runtime method field offsets.

        if (runtime::Runtime::AndroidVersionAtLeast(runtime::AndroidVersion::kPie, true)) {
            entry_point_for_quick_compiled_code_offset_ = 32;
            access_flag_offset_ = 4;
        } else if (runtime::Runtime::AndroidVersionAtLeast(
                runtime::AndroidVersion::kOreo)) { // NOLINT(bugprone-branch-clone)
            entry_point_for_quick_compiled_code_offset_ = 40;
            access_flag_offset_ = 4;
        } else if (runtime::Runtime::AndroidVersionAtLeast(runtime::AndroidVersion::kMarshmallow)) {
            entry_point_for_quick_compiled_code_offset_ = 48;
            access_flag_offset_ = 4;
        } else if (runtime::Runtime::AndroidVersionAtLeast(
                runtime::AndroidVersion::kLollipopPlus)) {
            entry_point_for_quick_compiled_code_offset_ = 52;
            access_flag_offset_ = 20;
        } else {
            // runtime::Runtime::AndroidVersionAtLeast(runtime::AndroidVersion::kLollipop)
            entry_point_for_quick_compiled_code_offset_ = 40;
            access_flag_offset_ = 56;
        }

        // Get Android Runtime JIT entry point.

        if (!runtime::Runtime::AndroidVersionAtLeast(runtime::AndroidVersion::kNougat)) {
            // Below Android 7, Android Runtime only uses AOT-compile mode.
            entry_point_for_jit_compile_ = nullptr;
        } else {
            void *entry_point_before_compile =
                    standard_runtime_method->GetEntryPointFromQuickCompiledCode();
            // Use CompileInternal() instead of Compile() to skip the compilation check,
            // because the data related to the compilation check has not been initialized.
            standard_runtime_method->Compile(current_thread);
            void *entry_point_after_compile =
                    standard_runtime_method->GetEntryPointFromQuickCompiledCode();
            if (entry_point_before_compile != entry_point_after_compile) {
                entry_point_for_jit_compile_ = entry_point_before_compile;
                debugLog("Entry point for JNI compile is set to 0x%016lx",
                         reinterpret_cast<std::size_t>(entry_point_for_jit_compile_))
            } else {
                errorLog("The standard method is already compiled before used.")
                entry_point_for_jit_compile_ = nullptr;
                return false;
            }
        }

        return true;
    }

    void Method::SetPrivate() {
        auto *pointer = reinterpret_cast<std::uint32_t *>(this + access_flag_offset_);
        *pointer = *pointer & ~ACCESS_FLAG_PUBLIC_MASK | ACCESS_FLAG_PRIVATE_MASK;
    }

    Method *
    Method::GetFromReflectMethod(JNIEnv *env, jobject reflect_method) {
        if (runtime::Runtime::AndroidVersionAtLeast(runtime::AndroidVersion::kR, true)) {
            return GetFromReflectMethodOnR(env, reflect_method);
        }
        jmethodID reflect_method_id = env->FromReflectedMethod(reflect_method);
        return reinterpret_cast<Method *>(reflect_method_id);
    }

    std::string Method::GetDataHexString() {
        auto base = reinterpret_cast<std::size_t>(this);
        std::string data = "[";
        char buffer[11];
        for (std::size_t i = 0; i < runtime_method_size_; i += sizeof(std::uint32_t)) {
            sprintf(buffer, "0x%08x", *reinterpret_cast<std::uint32_t *>(base + i));
            if (i != 0) data += ", ";
            data += buffer;
        }
        data += "]";
        return data;
    }

    void *Method::GetEntryPointFromQuickCompiledCode() {
        if (UNLIKELY(entry_point_for_quick_compiled_code_offset_ < 0)) return nullptr;
        return *reinterpret_cast<void **>(
                reinterpret_cast<std::size_t>(this) +
                entry_point_for_quick_compiled_code_offset_);
    }

    bool Method::Compile(Thread *current_thread) {
        if (!runtime::Runtime::AndroidVersionAtLeast(runtime::AndroidVersion::kNougat)) return true;
        // TODO: Check whether is compiled.
        auto *state_and_flags = reinterpret_cast<int32_t *>(current_thread);
        std::int32_t state_and_flags_backup = *state_and_flags;
        bool result = compiler_->Compile(this, current_thread);
        *state_and_flags = state_and_flags_backup;
        debugLog("Entry point of compiled runtime method 0x%016lx is 0x%016lx",
                 reinterpret_cast<std::size_t>(this),
                 reinterpret_cast<std::size_t>(GetEntryPointFromQuickCompiledCode()))
        return result;
    }

    Method *Method::GetFromReflectMethodOnR(JNIEnv *env, jobject reflect_method) {
        if (jvm_executable_class_ == nullptr) {
            errorLog("Cannot find class java.lang.reflect.Executable.")
            return nullptr;
        }
        jfieldID art_method_field_id = env->GetFieldID(jvm_executable_class_, "artMethod", "J");
        if (UNLIKELY(!art_method_field_id)) {
            errorLog("Cannot find field artMethod in class java.lang.reflect.Executable.")
            return nullptr;
        }
        jlong art_method_pointer = env->GetLongField(reflect_method, art_method_field_id);
        return reinterpret_cast<Method *>(art_method_pointer);
    }

    void *Compiler::function_compile_method_ = nullptr;
    void *Compiler::function_compile_method_handler_ = nullptr;

    Compiler::Compiler() {
        auto compile_method_handler_loader =
                reinterpret_cast<void *(*)(bool *)>(
                        internal::Library::SymbolInJitLibrary(FUNCTION_LOAD_SYMBOL));

        bool generate_debug_info = false;
        function_compile_method_handler_ = compile_method_handler_loader(&generate_debug_info);
    }

    CompilerBase::CompilerBase() : Compiler() {
        function_compile_method_ =
                internal::Library::SymbolInJitLibrary(FUNCTION_COMPILE_METHOD_SYMBOL);
    }

    bool CompilerBase::Compile(Method *method, Thread *current_thread) {
        return reinterpret_cast<bool (*)(void *, void *, void *, bool)>(function_compile_method_)(
                function_compile_method_handler_, method, current_thread, false);
    }

    CompilerOnQ::CompilerOnQ() : Compiler() {
        function_compile_method_ =
                internal::Library::SymbolInJitLibrary(FUNCTION_COMPILE_METHOD_SYMBOL);
    }

    bool CompilerOnQ::Compile(Method *method, Thread *current_thread) {
        return reinterpret_cast<bool (*)(void *, void *, void *, bool,
                                         bool)>(function_compile_method_)(
                function_compile_method_handler_, method, current_thread, false, false);
    }

    CompilerOnR::CompilerOnR(void *jit_code_cache) : Compiler(),
                                                     jit_code_cache_(jit_code_cache) {
        function_compile_method_ =
                internal::Library::SymbolInJitLibrary(FUNCTION_COMPILE_METHOD_SYMBOL);

        function_get_current_region_ =
                internal::Library::SymbolInArtLibrary(FUNCTION_GET_CURRENT_REGION);
    }

    bool CompilerOnR::Compile(Method *method, Thread *current_thread) {
        void *current_region = reinterpret_cast<void *(*)(void *)>(function_get_current_region_)(
                jit_code_cache_);
        return reinterpret_cast<bool (*)(void *, void *, void *, void *, bool,
                                         bool)>(function_compile_method_)(
                function_compile_method_handler_, current_thread, current_region, method,
                false, false);
    }
}