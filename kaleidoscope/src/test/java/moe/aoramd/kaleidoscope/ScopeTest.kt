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

import io.mockk.*
import moe.aoramd.kaleidoscope.internal.*
import org.junit.Assert.assertEquals
import org.junit.Test
import java.lang.reflect.Method

class ListenScopeTest {

    @Test
    fun testInvoke() {
        val listenerResult = Any()

        val beforeListener = mockk<(Any?, Array<Any?>) -> Any?>().apply {
            every { this@apply.invoke(any(), any()) } returns listenerResult
        }
        val afterListener = mockk<(Any?, Array<Any?>, Any?) -> Unit>().apply {
            justRun { this@apply.invoke(any(), any(), any()) }
        }

        val targetResult = Any()

        val target = mockk<Method>().apply {
            every { this@apply.invoke(any(), *anyVararg()) } returns targetResult
        }

        val thiz = Any()
        val parameters = Array<Any?>(10) { Any() }

        val scopeResult = ListenScope(
            beforeListener, afterListener, target,
            mockk(), mockk()
        ).invoke(thiz, parameters)

        verify { beforeListener.invoke(thiz, parameters) }
        verify { afterListener.invoke(thiz, parameters, listenerResult) }

        verify { target.invoke(thiz, *parameters) }
        assertEquals(targetResult, scopeResult)
    }

    @Test
    fun testRestore() {
        val source = mockk<Method>().apply {
            mark()
        }

        val result = mockk<InsertBridgeResult>().apply {
            justRun { restoreBridge() }

            mockkStatic(RuntimeMethod::releaseRecord)
            justRun { originPointer.releaseRecord() }
        }

        ListenScope(
            mockk(), mockk(), mockk(),
            source, result
        ).restore()

        verify { result.restoreBridge() }
        verify { result.originPointer.releaseRecord() }
    }

    @Test(expected = RepeatInvokeRestoreException::class)
    fun testRestoreWithUnmarkedMethod() {
        val source = mockk<Method>()

        val result = mockk<InsertBridgeResult>().apply {
            justRun { restoreBridge() }

            mockkStatic(RuntimeMethod::releaseRecord)
            justRun { originPointer.releaseRecord() }
        }

        ListenScope(
            mockk(), mockk(), mockk(),
            source, result
        ).restore()

        verify { result.restoreBridge() }
    }
}

class ReplaceScopeTest {

    @Test
    fun testInvoke() {
        val targetResult = Any()

        val target = mockk<Method>().apply {
            every { this@apply.invoke(any(), *anyVararg()) } returns targetResult
        }

        val thiz = Any()
        val parameters = Array<Any?>(10) { Any() }

        val scopeResult = ReplaceScope(target, mockk(), mockk()).invoke(thiz, parameters)

        verify { target.invoke(thiz, *parameters) }
        assertEquals(targetResult, scopeResult)
    }

    @Test
    fun testRestore() {
        val source = mockk<Method>().apply {
            mark()
        }

        val result = mockk<InsertBridgeResult>().apply {
            justRun { restoreBridge() }

            mockkStatic(RuntimeMethod::releaseRecord)
            justRun { originPointer.releaseRecord() }
        }

        ReplaceScope(mockk(), source, result).restore()

        verify { result.restoreBridge() }
        verify { result.originPointer.releaseRecord() }
    }

    @Test(expected = RepeatInvokeRestoreException::class)
    fun testRestoreWithUnmarkedMethod() {
        val source = mockk<Method>()

        val result = mockk<InsertBridgeResult>().apply {
            justRun { restoreBridge() }

            mockkStatic(RuntimeMethod::releaseRecord)
            justRun { originPointer.releaseRecord() }
        }

        ReplaceScope(mockk(), source, result).restore()

        verify { result.restoreBridge() }
    }
}