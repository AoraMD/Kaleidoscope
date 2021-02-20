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

@file:Suppress("unused")

package moe.aoramd.kaleidoscope

import android.util.Log

enum class LogLevel(val value: Int) {
    DEBUG(0),
    WARN(1),
    ERROR(2),
    SILENCE(3),
    UNKNOWN(100)
}

private var logLevel = LogLevel.ERROR

private const val DEFAULT_TAG = "Kaleidoscope"

internal fun initializeLog(level: LogLevel) {
    logLevel = level
}

internal fun Exception.debugTrace() {
    if (LogLevel.DEBUG >= logLevel) printStackTrace()
}

internal fun debugLog(tag: String, message: String) {
    if (LogLevel.DEBUG >= logLevel) Log.d(DEFAULT_TAG, "Kotlin $tag - $message")
}

internal fun warnLog(tag: String, message: String) {
    if (LogLevel.WARN >= logLevel) Log.w(DEFAULT_TAG, "Kotlin $tag - $message")
}

internal fun errorLog(tag: String, message: String) {
    if (LogLevel.ERROR >= logLevel) Log.e(DEFAULT_TAG, "Kotlin $tag - $message")
}