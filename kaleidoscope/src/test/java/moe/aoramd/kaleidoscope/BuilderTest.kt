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
import org.junit.Assert.assertTrue
import org.junit.Test
import java.lang.reflect.Method

class ListenBuilderTest {

    @Test
    fun testBuild() {
        val target = mockk<Method>()

        val result = mockk<InsertBridgeResult>().apply {
            mockkStatic(RuntimeMethod::registerRecord)
            justRun { originPointer.registerRecord(any()) }
        }

        val source = mockk<Method>().apply {
            justRun { isAccessible = any() }

            mockkStatic(Method::forceLoad)
            justRun { forceLoad() }

            mockkStatic(Method::listenBridge)
            every { listenBridge() } returns Pair(result, target)
        }

        val beforeListener = mockk<(Any?, Array<Any?>) -> Any?>()
        val afterListener = mockk<(Any?, Array<Any?>, Any?) -> Unit>()

        val scope = ListenBuilder(source)
            .before(beforeListener)
            .after(afterListener)
            .commit()

        verify { source.mark() }
        verify { source.isAccessible = true }
        verify { source.forceLoad() }
        assertEquals(
            ListenScope(beforeListener, afterListener, target, source, result),
            scope
        )
        assertTrue(scope is ListenScope)
        verify { result.originPointer.registerRecord(scope as ListenScope) }
    }

    @Test(expected = DuplicateMarkException::class)
    fun testBuildWithMarkedMethod() {
        val source = mockk<Method>().apply {
            mark()
            mockkStatic(Method::forceLoad)
            justRun { forceLoad() }
        }

        ListenBuilder(source)
            .before(mockk())
            .after(mockk())
            .commit()
    }

    @Test
    fun testBuildWithErrorBridge() {
        val source = mockk<Method>().apply {
            justRun { isAccessible = any() }

            mockkStatic(Method::forceLoad)
            justRun { forceLoad() }

            mockkStatic(Method::listenBridge)
            every { listenBridge() } returns null
        }

        val scope = ListenBuilder(source)
            .before(mockk())
            .after(mockk())
            .commit()

        verify { source.mark() }
        verify { source.unmark() }
        assertEquals(ErrorScope, scope)
    }
}

class ReplaceBuilderTest {

    @Test
    fun testBuild() {
        val returnType = Any::class.java
        val parameterTypes = Array(10) { Any::class.java }

        val source = mockk<Method>().apply {
            justRun { isAccessible = any() }

            every { this@apply.returnType } returns returnType
            every { this@apply.parameterTypes } returns parameterTypes

            mockkStatic(Method::forceLoad)
            justRun { forceLoad() }
        }

        val result = mockk<InsertBridgeResult>().apply {
            mockkStatic(Method::replaceBridge)
            every { source.replaceBridge() } returns this

            mockkStatic(RuntimeMethod::registerRecord)
            justRun { originPointer.registerRecord(any()) }
        }

        val target = mockk<Method>().apply {
            justRun { isAccessible = any() }
            every { this@apply.returnType } returns returnType
            every { this@apply.parameterTypes } returns parameterTypes
        }

        val scope = ReplaceBuilder(source)
            .target(target)
            .commit()

        verify { source.mark() }
        verify { source.isAccessible = true }
        verify { source.forceLoad() }
        assertEquals(
            ReplaceScope(target, source, result),
            scope
        )
        assertTrue(scope is ReplaceScope)
        verify { result.originPointer.registerRecord(scope as ReplaceScope) }
    }

    @Test(expected = DuplicateMarkException::class)
    fun testBuildWithMarkedMethod() {
        val returnType = Any::class.java
        val parameterTypes = Array(10) { Any::class.java }

        val source = mockk<Method>().apply {
            mark()

            every { this@apply.returnType } returns returnType
            every { this@apply.parameterTypes } returns parameterTypes

            mockkStatic(Method::forceLoad)
            justRun { this@apply.forceLoad() }
        }

        val target = mockk<Method>().apply {
            justRun { isAccessible = any() }

            every { this@apply.returnType } returns returnType
            every { this@apply.parameterTypes } returns parameterTypes
        }

        ReplaceBuilder(source)
            .target(target)
            .commit()
    }

    @Test
    fun testBuildWithErrorBridge() {
        val returnType = Any::class.java
        val parameterTypes = Array(10) { Any::class.java }

        val source = mockk<Method>().apply {
            every { this@apply.returnType } returns returnType
            every { this@apply.parameterTypes } returns parameterTypes
            justRun { isAccessible = any() }

            mockkStatic(Method::forceLoad)
            justRun { forceLoad() }

            mockkStatic(Method::replaceBridge)
            every { replaceBridge() } returns null
        }

        val target = mockk<Method>().apply {
            justRun { isAccessible = any() }

            every { this@apply.returnType } returns returnType
            every { this@apply.parameterTypes } returns parameterTypes
        }

        val scope = ReplaceBuilder(source)
            .target(target)
            .commit()

        verify { source.mark() }
        verify { source.unmark() }
        assertEquals(ErrorScope, scope)
    }
}