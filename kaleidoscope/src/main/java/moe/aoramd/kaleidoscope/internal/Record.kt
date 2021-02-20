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

import moe.aoramd.kaleidoscope.*
import java.lang.reflect.Method

private val records = mutableMapOf<Long, ValidScope>()

/**
 * Register [scope] into record when the scope is created.
 *
 * [this] is the source runtime method pointer which will be captured from bridge function.
 *
 * This function is paired with [releaseRecord].
 */
internal fun RuntimeMethod.registerRecord(scope: ValidScope) {
    if (records.containsKey(this.nativePeer)) throw DuplicateRegisterException(this)
    records[this.nativePeer] = scope
}

internal fun RuntimeMethod.searchRecord(): ValidScope {
    if (!records.containsKey(this.nativePeer)) throw UnexpectedTokenException(this)
    return records[this.nativePeer]!!
}

/**
 * Release scope using [this] when [Scope.restore] is invoked.
 *
 * This function is paired with [registerRecord].
 */
internal fun RuntimeMethod.releaseRecord() {
    if (!records.containsKey(this.nativePeer)) throw UnexpectedTokenException(this)
    records.remove(this.nativePeer)
}

private val marks = mutableSetOf<Method>()

/**
 * Mark a method set to Kaleidoscope to avoid repeated settings.
 *
 * This function is paired with [unmark].
 */
internal fun Method.mark() {
    if (marks.contains(this)) throw DuplicateMarkException(this)
    marks.add(this)
}

/**
 * Unmark a method set to Kaleidoscope.
 * The function return false if no method mark record was found.
 *
 * This function is paired with [mark].
 */
internal fun Method.unmark(): Boolean {
    if (!marks.contains(this)) return false
    marks.remove(this)
    return true
}