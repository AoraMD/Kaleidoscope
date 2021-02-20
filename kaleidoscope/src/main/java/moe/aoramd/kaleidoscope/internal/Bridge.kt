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

import android.os.Build
import moe.aoramd.kaleidoscope.UnsupportedArchitectureException
import java.lang.reflect.Method

/**
 * A collection of bridge methods. The bridge method is invoked instead of the origin method
 * set to Kaleidoscope when the origin is called.
 */
internal sealed class Bridge {

    protected abstract fun bridgeMethod(type: Type): Method

    companion object {

        fun loadAll() {
            val arch = Build.SUPPORTED_ABIS.first()
            val bridge =
                when {
                    arch.startsWith("arm64") || arch.startsWith("x86_64") -> Bridge64
                    arch.startsWith("armeabi") || arch.startsWith("x86") -> Bridge32
                    else -> throw UnsupportedArchitectureException(arch)
                }
            Type.values().forEach {
                bridge.bridgeMethod(it).registerAsBridge(it.key)
            }
        }
    }

    enum class Type(val key: Int, val token: String, val clazz: Class<*>) {

        VOID(0, "void", Void::class.java),
        BOOLEAN(10, "boolean", Boolean::class.java),
        BYTE(20, "byte", Byte::class.java),
        CHAR(30, "char", Char::class.java),
        SHORT(40, "short", Short::class.java),
        INT(50, "int", Int::class.java),
        LONG(60, "long", Long::class.java),
        FLOAT(70, "float", Float::class.java),
        DOUBLE(80, "double", Double::class.java),
        ANY(90, "any", Any::class.java);

        companion object {
            val Class<*>.toBridgeType: Type
                get() = values().filter { it.clazz == this }.run {
                    if (isNotEmpty()) first()
                    else ANY
                }
        }
    }
}

@Suppress("unused")
private object Bridge64 : Bridge() {

    @JvmStatic
    private fun voidBridge(
        currentThread: Long, box: Long, x3: Long,
        x4: Long, x5: Long, x6: Long, x7: Long
    ) {
        invokeBridge64(currentThread, box, x3, x4, x5, x6, x7)
    }

    @JvmStatic
    private fun booleanBridge(
        currentThread: Long, box: Long, x3: Long,
        x4: Long, x5: Long, x6: Long, x7: Long
    ): Boolean = invokeBridge64(currentThread, box, x3, x4, x5, x6, x7) as Boolean

    @JvmStatic
    private fun byteBridge(
        currentThread: Long, box: Long, x3: Long,
        x4: Long, x5: Long, x6: Long, x7: Long
    ): Byte = invokeBridge64(currentThread, box, x3, x4, x5, x6, x7) as Byte

    @JvmStatic
    private fun charBridge(
        currentThread: Long, box: Long, x3: Long,
        x4: Long, x5: Long, x6: Long, x7: Long
    ): Char = invokeBridge64(currentThread, box, x3, x4, x5, x6, x7) as Char

    @JvmStatic
    private fun shortBridge(
        currentThread: Long, box: Long, x3: Long,
        x4: Long, x5: Long, x6: Long, x7: Long
    ): Short = invokeBridge64(currentThread, box, x3, x4, x5, x6, x7) as Short

    @JvmStatic
    private fun intBridge(
        currentThread: Long, box: Long, x3: Long,
        x4: Long, x5: Long, x6: Long, x7: Long
    ): Int = invokeBridge64(currentThread, box, x3, x4, x5, x6, x7) as Int

    @JvmStatic
    private fun longBridge(
        currentThread: Long, box: Long, x3: Long,
        x4: Long, x5: Long, x6: Long, x7: Long
    ): Long = invokeBridge64(currentThread, box, x3, x4, x5, x6, x7) as Long

    @JvmStatic
    private fun floatBridge(
        currentThread: Long, box: Long, x3: Long,
        x4: Long, x5: Long, x6: Long, x7: Long
    ): Float = invokeBridge64(currentThread, box, x3, x4, x5, x6, x7) as Float

    @JvmStatic
    private fun doubleBridge(
        currentThread: Long, box: Long, x3: Long,
        x4: Long, x5: Long, x6: Long, x7: Long
    ): Double = invokeBridge64(currentThread, box, x3, x4, x5, x6, x7) as Double

    @JvmStatic
    private fun anyBridge(
        currentThread: Long, box: Long, x3: Long,
        x4: Long, x5: Long, x6: Long, x7: Long
    ): Any? = invokeBridge64(currentThread, box, x3, x4, x5, x6, x7)

    override fun bridgeMethod(type: Type): Method =
        Bridge64::class.java.getDeclaredMethod(
            "${type.token}Bridge",
            Long::class.java,
            Long::class.java,
            Long::class.java,
            Long::class.java,
            Long::class.java,
            Long::class.java,
            Long::class.java
        )
}

@Suppress("unused")
private object Bridge32 : Bridge() {

    @JvmStatic
    private fun voidBridge(
        currentThread: Long, box: Long, x3: Long,
    ) {
        invokeBridge32(currentThread, box, x3)
    }

    @JvmStatic
    private fun booleanBridge(
        currentThread: Long, box: Long, x3: Long,
    ): Boolean = invokeBridge32(currentThread, box, x3) as Boolean

    @JvmStatic
    private fun byteBridge(
        currentThread: Long, box: Long, x3: Long,
    ): Byte = invokeBridge32(currentThread, box, x3) as Byte

    @JvmStatic
    private fun charBridge(
        currentThread: Long, box: Long, x3: Long,
    ): Char = invokeBridge32(currentThread, box, x3) as Char

    @JvmStatic
    private fun shortBridge(
        currentThread: Long, box: Long, x3: Long,
    ): Short = invokeBridge32(currentThread, box, x3) as Short

    @JvmStatic
    private fun intBridge(
        currentThread: Long, box: Long, x3: Long,
    ): Int = invokeBridge32(currentThread, box, x3) as Int

    @JvmStatic
    private fun longBridge(
        currentThread: Long, box: Long, x3: Long,
    ): Long = invokeBridge32(currentThread, box, x3) as Long

    @JvmStatic
    private fun floatBridge(
        currentThread: Long, box: Long, x3: Long,
    ): Float = invokeBridge32(currentThread, box, x3) as Float

    @JvmStatic
    private fun doubleBridge(
        currentThread: Long, box: Long, x3: Long,
    ): Double = invokeBridge32(currentThread, box, x3) as Double

    @JvmStatic
    private fun anyBridge(
        currentThread: Long, box: Long, x3: Long,
    ): Any? = invokeBridge32(currentThread, box, x3)

    override fun bridgeMethod(type: Type): Method =
        Bridge32::class.java.getDeclaredMethod(
            "${type.token}Bridge",
            Long::class.java,
            Long::class.java,
            Long::class.java
        )
}