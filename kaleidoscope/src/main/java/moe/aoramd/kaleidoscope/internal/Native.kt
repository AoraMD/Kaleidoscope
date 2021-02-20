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

/*
 * TODO: Disable warning suppression after inline class feature becomes stable.
 */

/**
 * The mirror class used to obtain pointer of [mirror::Method]
 * for restricting extension functions type.
 */
internal inline class RuntimeMethod(val nativePeer: Long)

/**
 * The mirror class used to obtain data of native class [runtime::Box] objects.
 */
internal inline class Box(private val nativePeer: Long) {

    fun release() = releaseInternal(nativePeer)

    val calleeRuntimeMethod: RuntimeMethod
        get() = RuntimeMethod(calleeRuntimeMethod(nativePeer))

    val x1: Long
        get() = register1(nativePeer)

    val x2: Long
        get() = register2(nativePeer)

    fun parameterFromFloatRegister(index: Int): Float =
        Companion.parameterFromFloatRegister(nativePeer, index)

    fun parameterFromDoubleRegister(index: Int): Double =
        Companion.parameterFromDoubleRegister(nativePeer, index)

    fun parameterFromStack(byteOffset: Int, byteSize: Int): Long =
        parameterFromStack(nativePeer, byteOffset, byteSize)

    companion object {
        fun unlockAndCopy(main: Long): Box = Box(unlockAndCopyInternal(main))
        private external fun unlockAndCopyInternal(main: Long): Long
        private external fun releaseInternal(nativePeer: Long)
        private external fun calleeRuntimeMethod(nativePeer: Long): Long
        private external fun register1(nativePeer: Long): Long
        private external fun register2(nativePeer: Long): Long
        private external fun parameterFromFloatRegister(
            nativePeer: Long, index: Int
        ): Float
        private external fun parameterFromDoubleRegister(
            nativePeer: Long, index: Int
        ): Double
        private external fun parameterFromStack(
            nativePeer: Long, byteOffset: Int, byteSize: Int
        ): Long
    }
}

/**
 * The mirror class used to obtain data of native class [runtime::InsertBridgeResult] objects.
 */
internal interface InsertBridgeResult {
    val nativePeer: Long

    val originPointer: RuntimeMethod
        get() = RuntimeMethod(originPointerNative(nativePeer))

    fun restoreBridge() = restoreBridgeNative(nativePeer)

    companion object {
        external fun originPointerNative(nativePeer: Long): Long
    }
}

/**
 * The mirror class used to obtain data of native class [runtime::ListenResult] objects.
 */
internal inline class ListenResult(override val nativePeer: Long) : InsertBridgeResult {

    val clonePointer: RuntimeMethod
        get() = RuntimeMethod(clonePointerNative(nativePeer))

    companion object {
        private external fun clonePointerNative(nativePeer: Long): Long
    }
}


/**
 * The mirror class used to obtain data of native class [runtime::ReplaceResult] objects.
 */
internal inline class ReplaceResult(override val nativePeer: Long) : InsertBridgeResult