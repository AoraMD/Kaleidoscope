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

#include <dlfcn.h>
#include <unistd.h>
#include <sys/mman.h>

#include "internal.h"

#include "log.h"
#include "macro.h"

#include "runtime.h"

#include "library/nougat_dlfunctions/fake_dlfcn.h"

namespace moe::aoramd::kaleidoscope::internal {

    void *Library::art_library_handle_ = nullptr;
    void *Library::jit_library_handle_ = nullptr;

    bool Library::Initialize() {
        art_library_handle_ = Open(LIBRARY_ART_NAME);
        if (art_library_handle_ == nullptr) {
            errorLog("Unable to load dynamic library %s.", LIBRARY_ART_NAME)
            return false;
        }
        jit_library_handle_ = Open(LIBRARY_JIT_NAME);
        if (jit_library_handle_ == nullptr) {
            errorLog("Unable to load dynamic library %s.", LIBRARY_JIT_NAME)
            return false;
        }
        return true;
    }

    void *Library::SymbolInArtLibrary(const char *symbol) {
        return Symbol(art_library_handle_, symbol);
    }

    void *Library::SymbolInJitLibrary(const char *symbol) {
        return Symbol(jit_library_handle_, symbol);
    }

    void *Library::Open(const char *filename) {
        void *library;
        if (LIKELY(runtime::Runtime::AndroidVersionAtLeast(runtime::AndroidVersion::kNougat))) {
            library = fake_dlopen(filename, RTLD_NOW);
        } else {
            library = dlopen(filename, RTLD_NOW);
        }
        return library;
    }

    void *Library::Symbol(void *handle, const char *symbol) {
        if (LIKELY(runtime::Runtime::AndroidVersionAtLeast(runtime::AndroidVersion::kNougat)))
            return fake_dlsym(handle, symbol);
        else return dlsym(handle, symbol);
    }

    bool Memory::Unprotect(void *start, std::size_t size) {
        std::size_t page_size = sysconf(_SC_PAGESIZE);
        std::size_t alignment = reinterpret_cast<std::size_t>(start) % page_size;
        return mprotect(
                reinterpret_cast<void *>(reinterpret_cast<std::size_t>(start) - alignment),
                size + alignment, PROT_READ | PROT_WRITE | PROT_EXEC) == 0;
    }

    void Memory::Copy(void *destination, void *source, std::size_t size) {
        memcpy(destination, source, size);
    }

    jclass Jni::jvm_executable_class_ = nullptr;

    void *Jni::function_add_weak_global_reference_ = nullptr;

    bool Jni::Initialize(JNIEnv *env) {
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

        if (runtime::Runtime::AndroidVersionAtLeast(runtime::AndroidVersion::kOreo, true)) {
            function_add_weak_global_reference_ =
                    Library::SymbolInArtLibrary(FUNCTION_ADD_WEAK_GLOBAL_REFERENCE_ON_AND_ABOVE_O);
        } else if (runtime::Runtime::AndroidVersionAtLeast(runtime::AndroidVersion::kMarshmallow)) {
            function_add_weak_global_reference_ =
                    Library::SymbolInArtLibrary(FUNCTION_ADD_WEAK_GLOBAL_REFERENCE_ON_M_AND_N);
        } else {
            // runtime::Runtime::AndroidVersionAtLeast(runtime::AndroidVersion::kLollipop)
            function_add_weak_global_reference_ =
                    Library::SymbolInArtLibrary(FUNCTION_ADD_WEAK_GLOBAL_REFERENCE_ON_L);
        }
        return true;
    }

    jclass Jni::GetClassGlobalReference(JNIEnv *env, const char *class_name) {
        jclass class_local = env->FindClass(class_name);
        if (env->ExceptionCheck()) env->ExceptionClear();
        if (class_local == nullptr) return nullptr;
        auto result = reinterpret_cast<jclass>(env->NewGlobalRef(class_local));
        env->DeleteLocalRef(class_local);
        return result;
    }

    mirror::Method *Jni::GetRuntimeMethodFromReflectMethod(JNIEnv *env, jobject reflect_method) {
        if (runtime::Runtime::AndroidVersionAtLeast(runtime::AndroidVersion::kR, true)) {
            return GetRuntimeMethodFromReflectMethodOnR(env, reflect_method);
        }
        jmethodID reflect_method_id = env->FromReflectedMethod(reflect_method);
        return reinterpret_cast<mirror::Method *>(reflect_method_id);
    }

    jobject Jni::GetObject(JNIEnv *env, mirror::Thread *thread, mirror::Object *object) {
        if (function_add_weak_global_reference_ == nullptr) {
            errorLog("Symbol of function JavaVMExt.AddWeakGlobalRef() cannot be found.")
            return nullptr;
        }
        JavaVM *java_vm;
        env->GetJavaVM(&java_vm);
        return reinterpret_cast<jobject (*)(JavaVM *, mirror::Thread *, mirror::Object *)>(
                function_add_weak_global_reference_)(java_vm, thread, object);
    }

    mirror::Method *Jni::GetRuntimeMethodFromReflectMethodOnR(JNIEnv *env, jobject reflect_method) {
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
        return reinterpret_cast<mirror::Method *>(art_method_pointer);
    }
}