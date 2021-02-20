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

import android.annotation.SuppressLint
import android.os.Build
import moe.aoramd.kaleidoscope.MethodCloneException
import moe.aoramd.kaleidoscope.ReplaceRuntimeMethodAddressErrorException
import moe.aoramd.kaleidoscope.debugTrace
import java.lang.reflect.Method
import java.lang.reflect.Modifier

internal val Method.isStatic: Boolean
    get() = Modifier.isStatic(modifiers)

/**
 * Methods are lazy loaded, so we have to call it manually before hooking.
 *
 * However, its internal code cannot be executed, so we have to pass wrong type of arguments and
 * force to ignore all exceptions thrown by the execution.
 */
internal fun Method.forceLoad() {
    try {
        invoke(
            if (isStatic) null else Any(),
            if (parameterTypes.isEmpty()) arrayOf(Any()) else null
        )
    } catch (exception: Exception) {
        // Force to ignore all exceptions.
    }
}

@delegate:SuppressLint("SoonBlockedPrivateApi")
private val internalCloneMethod by lazy {
    Object::class.java.getDeclaredMethod("internalClone").apply {
        isAccessible = true
    }
}

private val artMethodField by lazy {
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
        Class.forName("java.lang.reflect.Executable")
            .getDeclaredField("artMethod").apply {
                isAccessible = true
            }
    } else {
        Class.forName("java.lang.reflect.AbstractMethod")
            .getDeclaredField("artMethod").apply {
                isAccessible = true
            }
    }
}

/**
 * Clone the java.lang.reflect.Method object and replace its runtime method pointer with [runtimeMethod].
 */
internal fun Method.runtimeClone(runtimeMethod: RuntimeMethod): Method {
    val clone: Method
    try {
        clone = internalCloneMethod.invoke(this) as Method
    } catch (exception: RuntimeException) {
        exception.debugTrace()
        throw MethodCloneException(this)
    }
    try {
        artMethodField.set(clone, runtimeMethod.nativePeer)
    } catch (exception: Exception) {
        exception.debugTrace()
        throw ReplaceRuntimeMethodAddressErrorException(this)
    }
    return clone
}