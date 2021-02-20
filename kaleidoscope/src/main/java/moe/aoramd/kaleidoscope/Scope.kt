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

package moe.aoramd.kaleidoscope

import moe.aoramd.kaleidoscope.internal.InsertBridgeResult
import moe.aoramd.kaleidoscope.internal.releaseRecord
import moe.aoramd.kaleidoscope.internal.unmark
import java.lang.reflect.Method

interface Scope {
    fun restore()
}

object ErrorScope : Scope {
    override fun restore() {}
}

sealed class ValidScope(
    internal val source: Method,
    private val result: InsertBridgeResult
) : Scope {
    internal abstract fun invoke(thiz: Any?, parameters: Array<Any?>): Any?
    override fun restore() {
        result.restoreBridge()
        if (!source.unmark()) throw RepeatInvokeRestoreException(this)
        result.originPointer.releaseRecord()
    }
}

class ListenScope internal constructor(
    private val before: (Any?) -> Any?,
    private val after: (Any?, Any?) -> Unit,
    private val target: Method,
    source: Method,
    result: InsertBridgeResult
) : ValidScope(source, result) {
    override fun invoke(thiz: Any?, parameters: Array<Any?>): Any? {
        val store = before.invoke(thiz)
        val result = target.invoke(thiz, *parameters)
        after.invoke(thiz, store)
        return result
    }
}

class ReplaceScope internal constructor(
    private val target: Method,
    source: Method,
    result: InsertBridgeResult
) : ValidScope(source, result) {
    override fun invoke(thiz: Any?, parameters: Array<Any?>): Any? {
        return target.invoke(thiz, *parameters)
    }
}