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

@file:JvmName("Kaleidoscope")

package moe.aoramd.kaleidoscope

import android.content.Context
import android.os.Build
import me.weishu.reflection.Reflection
import moe.aoramd.kaleidoscope.internal.*
import java.lang.reflect.Method

private const val LOG_TAG = "Kaleidoscope"

private const val NATIVE_LIBRARY_NAME = "kaleidoscope"

private enum class State {
    NOT_INITIALIZED,
    NATIVE_ERROR,
    SUCCESS,
}

@Volatile
private var currentState = State.NOT_INITIALIZED

@Synchronized
@JvmName("initialize")
fun Context.initializeKaleidoscope(logLevel: LogLevel = LogLevel.ERROR): Boolean {
    if (currentState == State.SUCCESS) return true
    if (currentState == State.NATIVE_ERROR) {
        errorLog(
            LOG_TAG,
            "Failed to initialize the framework last time, please check the log for error information."
        )
        return false
    }

    if (Build.VERSION.SDK_INT > Build.VERSION_CODES.R) {
        warnLog(
            LOG_TAG,
            "Kaleidoscope is running on an Android version higher than highest supported version (currently Android 11). Unexpected errors may occur."
        )
    }

    // Enable free reflection.
    Reflection.unseal(this)

    // Initialize configuration.
    initializeLog(logLevel)

    // Initialize kaleidoscope native.
    System.loadLibrary(NATIVE_LIBRARY_NAME)
    val initialized = initializeNative(
        logLevel = logLevel.value,
        currentThread = currentThreadNativePeer,
        standardMethod = Holder::class.java.getDeclaredMethod("functionStandard"),
        relativeMethod = Holder::class.java.getDeclaredMethod("functionRelative"),
    )
    if (!initialized) {
        currentState = State.NATIVE_ERROR
        throw InitializeErrorException()
    }

    // Register bridge methods.
    Bridge.loadAll()

    currentState = State.SUCCESS
    return true
}

fun Method.listen(): ListenBuilder {
    if (currentState != State.SUCCESS) throw NotInitializeException()
    return ListenBuilder(this)
}

fun Method.replace(): ReplaceBuilder {
    if (currentState != State.SUCCESS) throw NotInitializeException()
    return ReplaceBuilder(this)
}