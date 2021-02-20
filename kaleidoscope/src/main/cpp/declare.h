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

#ifndef KALEIDOSCOPE_DECLARE_H
#define KALEIDOSCOPE_DECLARE_H

namespace moe::aoramd::kaleidoscope {

    namespace internal {
        class Library;

        class Memory;

        class Jni;
    }

    namespace mirror {
        class Thread;

        class Object;

        class Method;

        class Compiler;
    }

    namespace runtime {
        class Runtime;

        class Box;

        enum AndroidVersion {
            kLollipop = 21,
            kLollipopPlus = 22,
            kMarshmallow = 23,
            kNougat = 24,
            kNougatPlus = 25,
            kOreo = 26,
            kOreoPlus = 27,
            kPie = 28,
            kQ = 29,
            kR = 30,
            kInDevelopment = 31
        };
    }
}

#endif
