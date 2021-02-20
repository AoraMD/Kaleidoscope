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

import moe.aoramd.kaleidoscope.internal.RuntimeMethod
import java.lang.reflect.Method

// Framework

internal class InitializeErrorException :
    RuntimeException("Initialize Kaleidoscope framework error.")

internal class NotInitializeException :
    RuntimeException("Kaleidoscope is not initialized.")

internal class NullTargetMethodException :
    RuntimeException("The target method must be set by invoking function target().")

internal class ReturnTypeNotMatchException(source: Method, target: Method) :
    RuntimeException("Return type of source method [$source] and target method [$target]'s are not the same.")

internal class ParameterTypeNotMatchException(
    source: Method,
    target: Method,
    parameterIndex: Int
) : RuntimeException("Parameter type of source method [$source] index of $parameterIndex and target method [$target]'s are not the same.")

// Runtime

internal class MethodCloneException(method: Method) :
    RuntimeException("Clone method [$method] failed.")

internal class ReplaceRuntimeMethodAddressErrorException(method: Method) :
    RuntimeException("Cannot replace runtime method address while cloning method [$method].")

// Record

internal class DuplicateRegisterException(runtimeMethod: RuntimeMethod) :
    RuntimeException("Runtime method ${runtimeMethod.nativePeer} was registered repeatedly.")

internal class UnexpectedTokenException(runtimeMethod: RuntimeMethod) :
    RuntimeException("None of registered method matching runtime method ${runtimeMethod.nativePeer} was found.")

internal class DuplicateMarkException(method: Method) :
    RuntimeException("Method [$method] was set to Kaleidoscope repeatedly.")

internal class RepeatInvokeRestoreException(scope: Scope) :
    RuntimeException("Function restore() of scope $scope can only be invoked once.")

// Bridge

internal class UnsupportedArchitectureException(architecture: String) :
    RuntimeException("Current architecture $architecture is not yet support.")