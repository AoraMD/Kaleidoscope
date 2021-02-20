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

import moe.aoramd.kaleidoscope.internal.*
import java.lang.reflect.Method

sealed class Builder(protected val source: Method) {
    /**
     * Commit and enable settings.
     * An exception will be thrown if method was set repeatedly.
     */
    abstract fun commit(): Scope
}

class ListenBuilder internal constructor(method: Method) : Builder(method) {

    private var beforeListener: (Any?) -> Any? = { null }
    private var afterListener: (Any?, Any?) -> Unit = { _, _ -> }

    /**
     * Add a listener which is called before the method is invoked.
     *
     * The parameter of listener is the callee object of the method, which is "this".
     * if method is static, This parameter always be null.
     *
     * The return value of listener is the data that needs to be obtained in listener
     * after the method is invoked. See [after].
     */
    fun before(listener: (thiz: Any?) -> Any?): ListenBuilder = apply {
        beforeListener = listener
    }

    /**
     * Add a listener which is called after the method is invoked.
     *
     * The first parameter of listener is the callee object of the method, which is "this".
     * if method is static, this parameter always be null.
     *
     * The second parameter of listener is the data stored by listener called
     * before the method is invoked. See [before].
     */
    fun after(listener: (thiz: Any?, store: Any?) -> Unit): ListenBuilder = apply {
        afterListener = listener
    }

    override fun commit(): Scope {
        source.mark()
        source.isAccessible = true
        source.forceLoad()
        val (result, target) = source.listenBridge() ?: run {
            source.unmark()
            return ErrorScope
        }
        return ListenScope(beforeListener, afterListener, target, source, result).also {
            result.originPointer.registerRecord(it)
        }
    }
}

class ReplaceBuilder internal constructor(method: Method) : Builder(method) {

    private var target: Method? = null

    /**
     * Add a target method, when the original method is invoked,
     * this method is invoked instead of the original method.
     *
     * The return type and parameter types of the target method must be exactly the same as the original one.
     */
    fun target(method: Method): ReplaceBuilder = apply {
        target = method
    }

    override fun commit(): Scope {
        if (target == null) throw NullTargetMethodException()
        val actualTarget = target!!

        // Match return type and parameter type.
        if (source.returnType != actualTarget.returnType)
            throw ReturnTypeNotMatchException(source, actualTarget)
        for (i in source.parameterTypes.indices) {
            if (source.parameterTypes[i] != actualTarget.parameterTypes[i])
                throw ParameterTypeNotMatchException(source, actualTarget, i)
        }

        source.mark()
        source.isAccessible = true
        source.forceLoad()
        actualTarget.isAccessible = true
        val result = source.replaceBridge() ?: run {
            source.unmark()
            return ErrorScope
        }
        return ReplaceScope(actualTarget, source, result).also {
            result.originPointer.registerRecord(it)
        }
    }
}