/*
 * MIT License
 *
 * Copyright (C) 2020 M.D.
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

#include <jni.h>

#include "log.h"
#include "internal.h"

#include "mirror.h"
#include "runtime.h"

using namespace moe::aoramd::kaleidoscope;

extern "C"
JNIEXPORT jboolean JNICALL
Java_moe_aoramd_kaleidoscope_internal_RuntimeKt_initializeNativeInternal(JNIEnv *env, jclass,
                                                                         jint log_level,
                                                                         jlong current_thread,
                                                                         jobject standard_method,
                                                                         jobject relative_method) {
    // Initialize log.
    Log::Initialize(log_level);

    // Initialize kaleidoscope native configuration.
    if (!runtime::Runtime::InitializeConfiguration()) {
        errorLog("Initialize kaleidoscope native configuration failed.")
        return false;
    }

    // Initialize dynamic library related.
    if (!internal::Library::Initialize()) {
        errorLog("Initialize dynamic library tool failed.")
        return false;
    }

    // Initialize JVM related.
    if (!internal::Jni::Initialize()) {
        errorLog("Initialize JVM tool failed.")
        return false;
    }

    // Initialize kaleidoscope runtime.
    if (!runtime::Runtime::Initialize()) {
        errorLog("Initialize kaleidoscope native runtime failed.")
        return false;
    }

    // Initialize runtime method related.
    if (!mirror::Method::Initialize(env, reinterpret_cast<mirror::Thread *>(current_thread),
                                    standard_method, relative_method)) {
        errorLog("Initialize runtime method failed.")
        return false;
    }

    return true;
}

extern "C"
JNIEXPORT void JNICALL
Java_moe_aoramd_kaleidoscope_internal_RuntimeKt_registerBridgeMethod(JNIEnv *env, jclass,
                                                                     jint key,
                                                                     jlong current_thread,
                                                                     jobject bridge_method) {
    runtime::Runtime::RegisterBridgeMethod(
            key, reinterpret_cast<mirror::Thread *>(current_thread),
            mirror::Method::GetFromReflectMethod(env, bridge_method));
}

extern "C"
JNIEXPORT jlong JNICALL
Java_moe_aoramd_kaleidoscope_internal_RuntimeKt_listenBridgeNative(JNIEnv *env, jclass,
                                                                   jobject method,
                                                                   jlong current_thread,
                                                                   jint bridge_type_key) {
    mirror::Method *runtime_method = mirror::Method::GetFromReflectMethod(env, method);
    runtime::ListenResult *result =
            runtime::Runtime::ListenBridge(runtime_method,
                                           reinterpret_cast<mirror::Thread *>(current_thread),
                                           bridge_type_key);
    if (result == nullptr) return 0;
    return reinterpret_cast<jlong>(result);
}

extern "C"
JNIEXPORT jlong JNICALL
Java_moe_aoramd_kaleidoscope_internal_RuntimeKt_replaceBridgeNative(JNIEnv *env, jclass,
                                                                    jobject method,
                                                                    jlong current_thread,
                                                                    jint bridge_type_key) {
    mirror::Method *runtime_method = mirror::Method::GetFromReflectMethod(env, method);
    runtime::ReplaceResult *result =
            runtime::Runtime::ReplaceBridge(runtime_method,
                                            reinterpret_cast<mirror::Thread *>(current_thread),
                                            bridge_type_key);
    if (result == nullptr) return 0;
    return reinterpret_cast<jlong>(result);
}

extern "C"
JNIEXPORT void JNICALL
Java_moe_aoramd_kaleidoscope_internal_RuntimeKt_restoreBridgeNativeInternal(JNIEnv *, jclass,
                                                                            jlong result_pointer) {
    runtime::Runtime::RestoreBridge(
            reinterpret_cast<runtime::InsertBridgeResult *>(result_pointer));
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_moe_aoramd_kaleidoscope_internal_RuntimeKt_convertBoolean(JNIEnv *, jclass, jlong data) {
    return internal::free_reinterpret_cast<jboolean>(data);
}

extern "C"
JNIEXPORT jbyte JNICALL
Java_moe_aoramd_kaleidoscope_internal_RuntimeKt_convertByte(JNIEnv *, jclass, jlong data) {
    return internal::free_reinterpret_cast<jbyte>(data);
}

extern "C"
JNIEXPORT jchar JNICALL
Java_moe_aoramd_kaleidoscope_internal_RuntimeKt_convertChar(JNIEnv *, jclass, jlong data) {
    return internal::free_reinterpret_cast<jchar>(data);
}

extern "C"
JNIEXPORT jshort JNICALL
Java_moe_aoramd_kaleidoscope_internal_RuntimeKt_convertShort(JNIEnv *, jclass, jlong data) {
    return internal::free_reinterpret_cast<jshort>(data);
}

extern "C"
JNIEXPORT jint JNICALL
Java_moe_aoramd_kaleidoscope_internal_RuntimeKt_convertInt(JNIEnv *, jclass, jlong data) {
    return internal::free_reinterpret_cast<jint>(data);
}

extern "C"
JNIEXPORT jlong JNICALL
Java_moe_aoramd_kaleidoscope_internal_RuntimeKt_convertLong(JNIEnv *, jclass, jlong data) {
    return internal::free_reinterpret_cast<jlong>(data);
}

extern "C"
JNIEXPORT jfloat JNICALL
Java_moe_aoramd_kaleidoscope_internal_RuntimeKt_convertFloat(JNIEnv *, jclass, jlong data) {
    return internal::free_reinterpret_cast<jfloat>(data);
}

extern "C"
JNIEXPORT jdouble JNICALL
Java_moe_aoramd_kaleidoscope_internal_RuntimeKt_convertDouble(JNIEnv *, jclass, jlong data) {
    return internal::free_reinterpret_cast<jdouble>(data);
}

extern "C"
JNIEXPORT jobject JNICALL
Java_moe_aoramd_kaleidoscope_internal_RuntimeKt_convertAny(JNIEnv *env, jclass, jlong data,
                                                           jlong current_thread) {
    return internal::Jni::GetObject(env,
                                    reinterpret_cast<mirror::Thread *>(current_thread),
                                    reinterpret_cast<mirror::Object *>(data));
}

extern "C"
JNIEXPORT jlong JNICALL
Java_moe_aoramd_kaleidoscope_internal_Box_00024Companion_unlockAndCopyInternal(JNIEnv *,
                                                                               jobject,
                                                                               jlong main) {
    return reinterpret_cast<jlong>(runtime::Runtime::UnlockAndCopyBox(
            reinterpret_cast<runtime::Box *>(main)));
}

extern "C"
JNIEXPORT void JNICALL
Java_moe_aoramd_kaleidoscope_internal_Box_00024Companion_releaseInternal(JNIEnv *,
                                                                         jobject,
                                                                         jlong native_peer) {
    free(reinterpret_cast<runtime::Box *>(native_peer));
}

extern "C"
JNIEXPORT jlong JNICALL
Java_moe_aoramd_kaleidoscope_internal_Box_00024Companion_calleeRuntimeMethod(JNIEnv *,
                                                                             jobject,
                                                                             jlong native_peer) {
    return reinterpret_cast<jlong>(
            reinterpret_cast<runtime::Box *>(native_peer)->callee_runtime_method_pointer_);
}

extern "C"
JNIEXPORT jlong JNICALL
Java_moe_aoramd_kaleidoscope_internal_Box_00024Companion_register2(JNIEnv *, jobject,
                                                                   jlong native_peer) {
    return reinterpret_cast<jlong>(
            reinterpret_cast<runtime::Box *>(native_peer)->register_2_);
}

extern "C"
JNIEXPORT jlong JNICALL
Java_moe_aoramd_kaleidoscope_internal_Box_00024Companion_register3(JNIEnv *, jobject,
                                                                   jlong native_peer) {
    return reinterpret_cast<jlong>(
            reinterpret_cast<runtime::Box *>(native_peer)->register_3_);
}

extern "C"
JNIEXPORT jfloat JNICALL
Java_moe_aoramd_kaleidoscope_internal_Box_00024Companion_parameterFromFloatRegister(JNIEnv *,
                                                                                    jobject,
                                                                                    jlong native_peer,
                                                                                    jint index) {
    return reinterpret_cast<runtime::Box *>(native_peer)->float_register_[index];
}

extern "C"
JNIEXPORT jdouble JNICALL
Java_moe_aoramd_kaleidoscope_internal_Box_00024Companion_parameterFromDoubleRegister(JNIEnv *,
                                                                                     jobject,
                                                                                     jlong native_peer,
                                                                                     jint index) {
    return reinterpret_cast<runtime::Box *>(native_peer)->double_register_[index];
}

extern "C"
#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"
JNIEXPORT jlong JNICALL
Java_moe_aoramd_kaleidoscope_internal_Box_00024Companion_parameterFromStack(JNIEnv *,
                                                                            jobject,
                                                                            jlong native_peer,
                                                                            jint byte_offset,
                                                                            jint byte_size) {
    std::uint64_t base =
            reinterpret_cast<runtime::Box *>(native_peer)->sp_pointer_ + sizeof(std::uint64_t);
    auto *pointer = reinterpret_cast<std::uint64_t *>(base + byte_offset);
    std::uint64_t value = *pointer;
    std::uint64_t mask = 0;

    // Delete the extra bits obtained.
    for (int i = 0; i < byte_size; i++) mask = (mask << 8) + 0xff;
    value = value & mask;

    return reinterpret_cast<jlong>(reinterpret_cast<void *>(value));
}
#pragma clang diagnostic pop

extern "C"
JNIEXPORT jlong JNICALL
Java_moe_aoramd_kaleidoscope_internal_InsertBridgeResult_00024Companion_originPointerNative(
        JNIEnv *, jobject,
        jlong native_peer) {
    return reinterpret_cast<jlong>(reinterpret_cast<runtime::InsertBridgeResult *>(native_peer)->origin_);
}

extern "C"
JNIEXPORT jlong JNICALL
Java_moe_aoramd_kaleidoscope_internal_ListenResult_00024Companion_clonePointerNative(JNIEnv *,
                                                                                     jobject,
                                                                                     jlong native_peer) {
    return reinterpret_cast<jlong>(reinterpret_cast<runtime::ListenResult *>(native_peer)->clone_);
}