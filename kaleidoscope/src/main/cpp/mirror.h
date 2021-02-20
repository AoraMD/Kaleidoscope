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

#ifndef KALEIDOSCOPE_MIRROR_H
#define KALEIDOSCOPE_MIRROR_H

#include <jni.h>
#include <string>

#include "declare.h"

namespace moe::aoramd::kaleidoscope::mirror {

    /**
     * The mirror class of art::runtime::Thread in Android Runtime.
     */
    class Thread final {
    };

    /**
     * The mirror class of art::mirror::Object in Android Runtime.
     */
    class Object final {
    };

    /**
     * The mirror class of art::ArtMethod in Android Runtime.
     *
     * In Kaleidoscope, art::ArtMethod is also called "Runtime Method".
     */
    class Method final {
        friend class runtime::Runtime;

    public:
        /**
         * Initialize runtime method related functions.
         *
         * @param env JNI environment.
         * @param current_thread current thread native peer for compile standard method.
         * @param standard_method standard method. It is used for calculate runtime method size.
         * @param relative_method relative method. It is used for calculate runtime method size.
         * @return
         */
        static bool Initialize(JNIEnv *env,
                               Thread *current_thread,
                               Method *standard_method,
                               Method *relative_method);

        /**
         * Set access flag private.
         */
        void SetPrivate();

        /**
         * Get size of runtime method object.
         *
         * Because this class is a mirror class, we cannot use sizeof() to get size of the actual
         * class: art::ArtMethod. This size must be calculated.
         *
         * @return size of runtime method object.
         */
        static int GetSize() {
            return runtime_method_size_;
        }

        /**
         * Returns data of the object saved as a string. It is used for debugging.
         *
         * @return string used to print the log.
         */
        std::string GetDataHexString();

    private:

        void *GetEntryPointFromQuickCompiledCode();

        /**
         * Force runtime method to be compiled in JIT mode.
         *
         * @param current_thread current thread native peer.
         * @return true if the runtime method is compiled.
         */
        bool Compile(Thread *current_thread);

        static Compiler *compiler_;
        static int runtime_method_size_;
        static int entry_point_for_quick_compiled_code_offset_;
        static int access_flag_offset_;
        static void *entry_point_for_jit_compile_;

        /**
         * Type of access flags in art::ArtMethod is std::atomic<std::uint32_t>.
         */
        static const std::uint32_t ACCESS_FLAG_PUBLIC_MASK = 0b01;
        static const std::uint32_t ACCESS_FLAG_PRIVATE_MASK = 0b10;
    };

    /**
     * Method JIT compiler. Different Android versions have different implementations.
     */
    class Compiler {
        friend class Method;

    protected:

        Compiler();

        static void *function_compile_method_;
        static void *function_compile_method_handler_;

        static constexpr const char *FUNCTION_LOAD_SYMBOL = "jit_load";

    private:
        virtual bool Compile(Method *method, Thread *current_thread) = 0;
    };

    /**
     * Method JIT compiler for Android 7 ~ Android 9.
     */
    class CompilerBase : public Compiler {
        friend class Method;

    private:
        CompilerBase();

        bool Compile(Method *method, Thread *current_thread) override;

        static constexpr const char *FUNCTION_COMPILE_METHOD_SYMBOL = "jit_compile_method";
    };

    /**
     * Method JIT compiler for Android 10.
     */
    class CompilerOnQ : public Compiler {
        friend class Method;

    private:
        CompilerOnQ();

        bool Compile(Method *method, Thread *current_thread) override;

        static constexpr const char *FUNCTION_COMPILE_METHOD_SYMBOL = "jit_compile_method";
    };

    /**
     * Method JIT compiler for Android 11.
     */
    class CompilerOnR : public Compiler {
        friend class Method;

    private:
        CompilerOnR(void *jit_code_cache);

        bool Compile(Method *method, Thread *current_thread) override;

        void *jit_code_cache_;
        void *function_get_current_region_;

        static constexpr const char *FUNCTION_COMPILE_METHOD_SYMBOL =
                "_ZN3art3jit11JitCompiler13CompileMethodEPNS_6ThreadEPNS0_15JitMemoryRegionEPNS_9ArtMethodEbb";
        static constexpr const char *FUNCTION_GET_CURRENT_REGION =
                "_ZN3art3jit12JitCodeCache16GetCurrentRegionEv";
    };
}

#endif
