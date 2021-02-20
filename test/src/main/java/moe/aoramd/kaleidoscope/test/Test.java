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

package moe.aoramd.kaleidoscope.test;

import android.util.Log;

@SuppressWarnings({"unused", "SameParameterValue"})
public class Test {

    public Object callArgumentCheck(Object thiz, long mask) {
        return argumentCheck((byte) 1,
                (short) 2,
                (int) 3,
                (long) 4,
                5.0f,
                6.0,
                true,
                (byte) 7,
                '8',
                (short) 9,
                10,
                11,
                12.0f,
                13.0,
                null,
                mask,
                thiz,
                mask);
    }

    private Object argumentCheck(byte register1, short register2, int register3, long register4, float register5, double register6,
                               boolean stack1, byte stack2, char stack3, short stack4, int stack5, long stack6, float stack7, double stack8,
                               Object objNull, long maskA, Object obj, long maskB) {
        Log.w("Kaleidoscope Sample", "argumentCheck() : Registers - " + register1 + " " + register2 + " " + register3 + " " + register4 + " " + register5 + " " + register6 +
                " - Stack - " + stack1 + " " + stack2 + " " + stack3 + " " + stack4 + " " + stack5 + " " + stack6 + " " + stack7 + " " + stack8 + " " + objNull + " " + obj);
        return this;
    }

    public static void callArgumentCheckStatic(Object thiz, long mask) {
        argumentCheckStatic((byte) 1,
                (short) 2,
                3,
                4,
                5.0f,
                6.0,
                true,
                (byte) 7,
                '8',
                (short) 9,
                10,
                11,
                12.0f,
                13.0,
                null,
                mask,
                thiz,
                mask);
    }

    public static void argumentCheckStatic(byte register1, short register2, int register3, long register4, float register5, double register6,
                                            boolean stack1, byte stack2, char stack3, short stack4, int stack5, long stack6, float stack7, double stack8,
                                            Object objNull, long maskA, Object obj, long maskB) {
        Log.w("Kaleidoscope Sample", "argumentCheckStatic() : Registers - " + register1 + " " + register2 + " " + register3 + " " + register4 + " " + register5 + " " + register6 +
                " - Stack - " + stack1 + " " + stack2 + " " + stack3 + " " + stack4 + " " + stack5 + " " + stack6 + " " + stack7 + " " + stack8 + " " + objNull + " " + obj);
    }
}
