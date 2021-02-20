/*
 * MIT License
 *
 * Copyright (C) 2020 M.D.
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

object Deps {

    object Project {
        const val androidGradlePlugin = "com.android.tools.build:gradle:4.1.1"
    }

    object Kotlin {

        private const val VERSION = "1.4.30"

        const val gradlePlugin = "org.jetbrains.kotlin:kotlin-gradle-plugin:$VERSION"
        const val standardLibrary = "org.jetbrains.kotlin:kotlin-stdlib:$VERSION"
    }

    const val freeReflection = "me.weishu:free_reflection:3.0.1"

    object Test {
        const val junit = "junit:junit:4.13.1"
    }
}