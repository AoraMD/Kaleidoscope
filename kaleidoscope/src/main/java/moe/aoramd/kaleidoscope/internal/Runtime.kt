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

package moe.aoramd.kaleidoscope.internal

import moe.aoramd.kaleidoscope.internal.Bridge.Type.Companion.toBridgeType
import java.lang.reflect.Method

/**
 * Initialize Kaleidoscope native runtime.
 */
internal fun initializeNative(
    logLevel: Int,
    currentThread: Long,
    standardMethod: Method,
    relativeMethod: Method
): Boolean = initializeNativeInternal(
    logLevel,
    currentThread,
    standardMethod,
    relativeMethod
)

private external fun initializeNativeInternal(
    logLevel: Int,
    currentThread: Long,
    standardMethod: Method,
    relativeMethod: Method
): Boolean

private val threadNativePeerField by lazy {
    Thread::class.java.getDeclaredField("nativePeer").apply {
        isAccessible = true
    }
}

/**
 * Get current thread native peer.
 */
internal val currentThreadNativePeer: Long
    get() = threadNativePeerField.getLong(Thread.currentThread())

/**
 * Register method as a bridge method in runtime.
 */
internal fun Method.registerAsBridge(key: Int) {
    forceLoad()
    registerBridgeMethod(key, currentThreadNativePeer, this)
}

private external fun registerBridgeMethod(key: Int, currentThread: Long, bridgeMethod: Method)

/**
 * Insert bridge code into entrance of runtime method of method for listening method invocation.
 */
internal fun Method.listenBridge(): Pair<InsertBridgeResult, Method>? {
    val nativePeer =
        listenBridgeNative(
            this, currentThreadNativePeer, returnType.toBridgeType.key
        )
    if (nativePeer == 0L) return null
    val result = ListenResult(nativePeer)
    val clone = runtimeClone(result.clonePointer)
    return Pair(result, clone)
}

private external fun listenBridgeNative(
    method: Method,
    currentThread: Long,
    bridgeTypeKey: Int
): Long

/**
 * Insert bridge code into entrance of runtime method of method for replacing method invocation.
 */
internal fun Method.replaceBridge(): InsertBridgeResult? {
    val nativePeer =
        replaceBridgeNative(
            this, currentThreadNativePeer, returnType.toBridgeType.key
        )
    return if (nativePeer == 0L) null else ReplaceResult(nativePeer)
}

private external fun replaceBridgeNative(
    method: Method,
    currentThread: Long,
    bridgeTypeKey: Int
): Long

internal fun restoreBridgeNative(resultPointer: Long) = restoreBridgeNativeInternal(resultPointer)

private external fun restoreBridgeNativeInternal(resultPointer: Long)

internal fun invokeBridge32(
    currentThread: Long, mainBox: Long, x3: Long
): Any? = TODO("Not yet implement.")

internal fun invokeBridge64(
    currentThread: Long, mainBox: Long, x3: Long,
    x4: Long, x5: Long, x6: Long, x7: Long
): Any? {
    val box = Box.unlockAndCopy(mainBox)
    val scope = box.calleeRuntimeMethod.searchRecord()
    val types = mutableListOf<Class<*>>().apply {
        if (!scope.source.isStatic) add(Any::class.java)
        addAll(scope.source.parameterTypes)
    }
    val data = mutableListOf<Any?>()

    val size = types.size
    var offset = 0

    val generalPurposeRegisters = longArrayOf(box.x1, box.x2, x3, x4, x5, x6, x7)

    /*
        Index used to get the general purpose register data.

        Index for getting integer data is not equal to the index of parameters, but the
        index of integer parameters, so the variable is calculated separately from the
        parameter index.
     */
    var integerIndex = 0

    /*
        Index used to get the floating point register data.

        In ARM 64, because of the particularity of floating point numbers, they are stored in
        floating point number registers (float, or named single, is stored in registers s0 ~ s7,
        and double is stored in registers d0 ~ d7. sX is dX low 32-bit.). In order to ensure the
        precision of floating point, we need to get data from floating point registers instead of
        general purpose registers.

        Index for getting floating point data is not equal to the index of parameters, but the
        index of floating point parameters, so the variable is calculated separately from the
        parameter index.
     */
    var floatingIndex = 0

    for (i in 0 until size) {
        types[i].apply {
            data.add(
                when (this) {
                    Float::class.java ->
                        if (floatingIndex < 8)
                            box.parameterFromFloatRegister(floatingIndex++)
                        else
                            convert(box.parameterFromStack(offset, stackSize), currentThread)
                    Double::class.java ->
                        if (floatingIndex < 8)
                            box.parameterFromDoubleRegister(floatingIndex++)
                        else
                            convert(box.parameterFromStack(offset, stackSize), currentThread)
                    else ->
                        if (integerIndex < 7)
                            convert(generalPurposeRegisters[integerIndex++], currentThread)
                        else
                            convert(box.parameterFromStack(offset, stackSize), currentThread)
                }
            )
            offset += stackSize
        }
    }

    // data.size == types.size == size.

    // Release box object to prevent memory leaks after obtaining all data.
    box.release()

    val thiz: Any?
    val parameters: Array<Any?>
    if (scope.source.isStatic) {
        thiz = null
        parameters = data.toTypedArray()
    } else {
        thiz = data[0]
        parameters = data.subList(1, size).toTypedArray()
    }
    return scope.invoke(thiz, parameters)
}

private fun Class<*>.convert(data: Long, currentThread: Long): Any? =
    when (this) {
        Boolean::class.java -> convertBoolean(data)
        Byte::class.java -> convertByte(data)
        Char::class.java -> convertChar(data)
        Short::class.java -> convertShort(data)
        Int::class.java -> convertInt(data)
        Long::class.java -> convertLong(data)
        Float::class.java -> convertFloat(data)
        Double::class.java -> convertDouble(data)
        else -> convertAny(data, currentThread)
    }

/**
 * Get the size in bytes of data type on stack.
 */
private val Class<*>.stackSize: Int
    get() = when (this) {
        Boolean::class.javaPrimitiveType, Byte::class.javaPrimitiveType,
        Char::class.javaPrimitiveType, Short::class.javaPrimitiveType,
        Int::class.javaPrimitiveType, Float::class.javaPrimitiveType -> 4
        Long::class.javaPrimitiveType, Double::class.javaPrimitiveType -> 8
        // Object pointer is 32-bit in Android Runtime.
        else -> 4
    }

private external fun convertBoolean(data: Long): Boolean

private external fun convertByte(data: Long): Byte

private external fun convertChar(data: Long): Char

private external fun convertShort(data: Long): Short

private external fun convertInt(data: Long): Int

private external fun convertLong(data: Long): Long

private external fun convertFloat(data: Long): Float

private external fun convertDouble(data: Long): Double

private external fun convertAny(data: Long, currentThread: Long): Any?

/**
 * An unused class with two adjacent methods for calculating runtime method object size.
 */
@Suppress("unused")
internal class Holder {
    private fun functionStandard() {}
    private fun functionRelative() {}
}