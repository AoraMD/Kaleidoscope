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

import java.lang.reflect.Method
import java.lang.reflect.Type

/**
 * A collection of bridge methods. The bridge method is invoked instead of the origin method
 * set to Kaleidoscope when the origin is called.
 */
@Suppress("unused")
internal object Bridge {

    @JvmStatic
    private fun voidBridge(
        thisOrX1: Long, currentThread: Long, box: Long,
        x4: Long, x5: Long, x6: Long, x7: Long
    ) {
        invokeBridge(thisOrX1, currentThread, box, x4, x5, x6, x7)
    }

    @JvmStatic
    private fun booleanBridge(
        thisOrX1: Long, currentThread: Long, box: Long,
        x4: Long, x5: Long, x6: Long, x7: Long
    ): Boolean = invokeBridge(thisOrX1, currentThread, box, x4, x5, x6, x7) as Boolean

    @JvmStatic
    private fun byteBridge(
        thisOrX1: Long, currentThread: Long, box: Long,
        x4: Long, x5: Long, x6: Long, x7: Long
    ): Byte = invokeBridge(thisOrX1, currentThread, box, x4, x5, x6, x7) as Byte

    @JvmStatic
    private fun charBridge(
        thisOrX1: Long, currentThread: Long, box: Long,
        x4: Long, x5: Long, x6: Long, x7: Long
    ): Char = invokeBridge(thisOrX1, currentThread, box, x4, x5, x6, x7) as Char

    @JvmStatic
    private fun shortBridge(
        thisOrX1: Long, currentThread: Long, box: Long,
        x4: Long, x5: Long, x6: Long, x7: Long
    ): Short = invokeBridge(thisOrX1, currentThread, box, x4, x5, x6, x7) as Short

    @JvmStatic
    private fun intBridge(
        thisOrX1: Long, currentThread: Long, box: Long,
        x4: Long, x5: Long, x6: Long, x7: Long
    ): Int = invokeBridge(thisOrX1, currentThread, box, x4, x5, x6, x7) as Int

    @JvmStatic
    private fun longBridge(
        thisOrX1: Long, currentThread: Long, box: Long,
        x4: Long, x5: Long, x6: Long, x7: Long
    ): Long = invokeBridge(thisOrX1, currentThread, box, x4, x5, x6, x7) as Long

    @JvmStatic
    private fun floatBridge(
        thisOrX1: Long, currentThread: Long, box: Long,
        x4: Long, x5: Long, x6: Long, x7: Long
    ): Float = invokeBridge(thisOrX1, currentThread, box, x4, x5, x6, x7) as Float

    @JvmStatic
    private fun doubleBridge(
        thisOrX1: Long, currentThread: Long, box: Long,
        x4: Long, x5: Long, x6: Long, x7: Long
    ): Double = invokeBridge(thisOrX1, currentThread, box, x4, x5, x6, x7) as Double

    @JvmStatic
    private fun anyBridge(
        thisOrX1: Long, currentThread: Long, box: Long,
        x4: Long, x5: Long, x6: Long, x7: Long
    ): Any? = invokeBridge(thisOrX1, currentThread, box, x4, x5, x6, x7)
}

internal enum class BridgeType(
    val key: Int,
    val token: String
) {
    VOID(0, "void"),
    BOOLEAN(10, "boolean"),
    BYTE(20, "byte"),
    CHAR(30, "char"),
    SHORT(40, "short"),
    INT(50, "int"),
    LONG(60, "long"),
    FLOAT(70, "float"),
    DOUBLE(80, "double"),
    ANY(90, "any");

    val bridgeMethod: Method
        get() = Bridge::class.java.getDeclaredMethod(
            "${token}Bridge",
            Long::class.java,
            Long::class.java,
            Long::class.java,
            Long::class.java,
            Long::class.java,
            Long::class.java,
            Long::class.java
        )

    companion object {
        fun Type.toBridgeType(): BridgeType =
            values().filter { it.token == typeName }.run {
                if (isNotEmpty()) first()
                else ANY
            }
    }
}
