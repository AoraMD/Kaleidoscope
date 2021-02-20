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

// void MainBridge();
    .text
    .align 4
	.global	MainBridge
	.type	MainBridge, %function
MainBridge:
    ldr x16, target
    br x16
target:
    .quad 0
    .size MainBridge, .-MainBridge

// void SecondaryBridge();
    .text
    .align 4
	.global	SecondaryBridge
	.type	SecondaryBridge, %function
SecondaryBridge:
    nop
    ldr x16, source_method
    cmp x0, x16
    beq bridge_match
    ldr x16, origin_bridge
    br x16
bridge_match:
    ldr x16, bridge_box_pointer
check_lock:
    ldr x17, [x16]
    cmp x17, #0
    bne check_lock
    str x19, [x16]          // lock_
    mov x17, sp
    str x17, [x16, #8*1]    // sp_pointer_
    str x0, [x16, #8*2]     // callee_runtime_method_pointer_
    str x1, [x16, #8*3]     // register_1_
    str x2, [x16, #8*4]     // register_2_
    str d0, [x16, #40+8*0]  // floating_registers_, 40 = 8 * 5
    str d1, [x16, #40+8*1]
    str d2, [x16, #40+8*2]
    str d3, [x16, #40+8*3]
    str d4, [x16, #40+8*4]
    str d5, [x16, #40+8*5]
    str d6, [x16, #40+8*6]
    str d7, [x16, #40+8*7]
    ldr x0, bridge_method
    mov x1, x19
    mov x2, x16
    ldr x16, bridge_entrance
    br x16
source_method:
    .quad 0
bridge_method:
    .quad 0
bridge_entrance:
    .quad 0
bridge_box_pointer:
    .quad 0
origin_bridge:
    .quad 0
    .size SecondaryBridge, .-SecondaryBridge

// void OriginBridge();
    .text
    .align 4
	.global	OriginBridge
	.type	OriginBridge, %function
OriginBridge:
    .quad 0
    .quad 0
    ldr x16, left
    br x16
left:
    .quad 0
    .size OriginBridge, .-OriginBridge