/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

/*
 * int32_t _call_ntw(
 *       uint32_t fid, void *args, uint32_t count);
 *
 * tee call ntw \n
 */

    .text
    .globl _call_ntw
    .type _call_ntw, function
_call_ntw:
    push r15
    /* Push all registers to stack */
    subi sp, sp, 40
    stm  r4-r13, (sp)

    /* clear all GP registers */
    movi r4, 0
    movi r5, 0
    movi r6, 0
    movi r7, 0
    movi r8, 0
    movi r9, 0
    movi r10, 0
    movi r11, 0
    movi r12, 0
    movi r13, 0

    /* world switch */
    wsc

    ldm  r4-r13, (sp)
    addi sp, sp, 40

    pop  r15
    .size _call_ntw, . - _call_ntw
