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

#ifndef KALEIDOSCOPE_INTERNAL_H
#define KALEIDOSCOPE_INTERNAL_H

#include <jni.h>
#include <map>

#include "declare.h"

namespace moe::aoramd::kaleidoscope::internal {

    template<typename T>
    T free_reinterpret_cast(jlong data) {
        void *pointer = &data;
        return *reinterpret_cast<T *>(pointer);
    }

    class Library final {
    public:
        /**
         * Initialize dynamic library related functions.
         *
         * @return true if initialize successfully.
         */
        static bool Initialize();

        /**
         * Find function symbol in dynamic library libart.so.
         *
         * @param symbol function symbol.
         * @return function pointer or nullptr on failure.
         */
        static void *SymbolInArtLibrary(const char *symbol);

        /**
         * Find function symbol in dynamic library libart-compiler.so.
         *
         * @param symbol function symbol.
         * @return function pointer or nullptr on failure.
         */
        static void *SymbolInJitLibrary(const char *symbol);

    private:
        static void *Open(const char *filename);

        static void *Symbol(void *handle, const char *symbol);

        static void *art_library_handle_;
        static void *jit_library_handle_;

        static constexpr const char *LIBRARY_ART_NAME = "libart.so";
        static constexpr const char *LIBRARY_JIT_NAME = "libart-compiler.so";
    };

    class Memory final {
    public:
        /**
         * Disable all access restrictions for the specified memory in units of memory pages.
         *
         * @param start memory start address. The starting address of affected space may be
         *        smaller than this due to memory page alignment.
         * @param size memory size. The size of affected space may be larger than this due to
         *        memory page alignment.
         * @return 0 on success or -1 on failure.
         */
        static bool Unprotect(void *start, std::size_t size);

        /**
         * Copies the values of num bytes from the location pointed to by source directly
         * to the memory block pointed to by destination.
         *
         * @param destination pointer to the destination array where the content is to be copied.
         * @param source pointer to the source of data to be copied.
         * @param size number of bytes to copy.
         */
        static void Copy(void *destination, void *source, std::size_t size);
    };

    class Jni final {
    public:
        /**
         * Initialize JNI related functions.
         *
         * @return true if initialize successfully.
         */
        static bool Initialize();

        /**
         * Get java class global reference by class name.
         *
         * @param env JNI environment.
         * @param class_name java class name.
         * @return class global reference.
         */
        static jclass GetClassGlobalReference(JNIEnv *env, const char *class_name);

        /**
         * Get jobject from mirror::Object.
         *
         * @param env JNI environment.
         * @param thread thread native peer.
         * @param object the object to be converted.
         * @return converted jobject.
         */
        static jobject GetObject(JNIEnv *env, mirror::Thread *thread, mirror::Object *object);

    private:
        static void *function_add_weak_global_reference_;

        static constexpr const char *FUNCTION_ADD_WEAK_GLOBAL_REFERENCE_ON_L =
                "_ZN3art9JavaVMExt22AddWeakGlobalReferenceEPNS_6ThreadEPNS_6mirror6ObjectE";
        static constexpr const char *FUNCTION_ADD_WEAK_GLOBAL_REFERENCE_ON_M_AND_N =
                "_ZN3art9JavaVMExt16AddWeakGlobalRefEPNS_6ThreadEPNS_6mirror6ObjectE";
        static constexpr const char *FUNCTION_ADD_WEAK_GLOBAL_REFERENCE_ON_AND_ABOVE_O =
                "_ZN3art9JavaVMExt16AddWeakGlobalRefEPNS_6ThreadENS_6ObjPtrINS_6mirror6ObjectEEE";
    };
}

#endif
