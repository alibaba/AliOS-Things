/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Enable interrupts when returning from the handler */
#define MSTATUS_PRV1 0x1880

/******************************************************************************
 * Functions:
 *     size_t cpu_intrpt_save(void);
 *     void   cpu_intrpt_restore(size_t psr);
 ******************************************************************************/

.global cpu_intrpt_save
.type cpu_intrpt_save, %function
cpu_intrpt_save:
    csrr    a0, mstatus
    csrc    mstatus, 8
    ret

.global cpu_intrpt_restore
.type cpu_intrpt_restore, %function
cpu_intrpt_restore:
    csrw    mstatus, a0
    ret

/******************************************************************************
 * Functions:
 *     void cpu_intrpt_switch(void);
 *     void cpu_task_switch(void);
 ******************************************************************************/

.global cpu_task_switch
.type cpu_task_switch, %function
cpu_task_switch:
    li      t0, 0xE080100C
    lb      t1, (t0)
    li      t2, 0x01
    or      t1, t1, t2
    sb      t1, (t0)

    ret

.global cpu_intrpt_switch
.type cpu_intrpt_switch, %function
cpu_intrpt_switch:
    li      t0, 0xE080100C
    lb      t1, (t0)
    li      t2, 0x01
    or      t1, t1, t2
    sb      t1, (t0)

    ret

/******************************************************************************
 * Functions:
 *     void cpu_first_task_start(void);
 ******************************************************************************/
.global cpu_first_task_start
.type cpu_first_task_start, %function
cpu_first_task_start:
    j       __task_switch_nosave

/******************************************************************************
 * Functions:
 *     void tspend_handler(void);
 ******************************************************************************/

.global tspend_handler
.type tspend_handler, %function
tspend_handler:
    addi    sp, sp, -128
    fsw      f31, 0(sp)
    fsw      f30, 4(sp)
    fsw      f29, 8(sp)
    fsw      f28, 12(sp)
    fsw      f27, 16(sp)
    fsw      f26, 20(sp)
    fsw      f25, 24(sp)
    fsw      f24, 28(sp)
    fsw      f23, 32(sp)
    fsw      f22, 36(sp)
    fsw      f21, 40(sp)
    fsw      f20, 44(sp)
    fsw      f19, 48(sp)
    fsw      f18, 52(sp)
    fsw      f17, 56(sp)
    fsw      f16, 60(sp)
    fsw      f15, 64(sp)
    fsw      f14, 68(sp)
    fsw      f13, 72(sp)
    fsw      f12, 76(sp)
    fsw      f11, 80(sp)
    fsw      f10, 84(sp)
    fsw      f9, 88(sp)
    fsw      f8, 92(sp)
    fsw      f7, 96(sp)
    fsw      f6, 100(sp)
    fsw      f5, 104(sp)
    fsw      f4, 108(sp)
    fsw      f3, 112(sp)
    fsw      f2, 116(sp)
    fsw      f1, 120(sp)
    fsw      f0, 124(sp)


    addi    sp, sp, -124

    sw      x1, 0(sp)
    sw      x3, 4(sp)
    sw      x4, 8(sp)
    sw      x5, 12(sp)
    sw      x6, 16(sp)
    sw      x7, 20(sp)
    sw      x8, 24(sp)
    sw      x9, 28(sp)
    sw      x10, 32(sp)
    sw      x11, 36(sp)
    sw      x12, 40(sp)
    sw      x13, 44(sp)
    sw      x14, 48(sp)
    sw      x15, 52(sp)
    sw      x16, 56(sp)
    sw      x17, 60(sp)
    sw      x18, 64(sp)
    sw      x19, 68(sp)
    sw      x20, 72(sp)
    sw      x21, 76(sp)
    sw      x22, 80(sp)
    sw      x23, 84(sp)
    sw      x24, 88(sp)
    sw      x25, 92(sp)
    sw      x26, 96(sp)
    sw      x27, 100(sp)
    sw      x28, 104(sp)
    sw      x29, 108(sp)
    sw      x30, 112(sp)
    sw      x31, 116(sp)
    csrr    t0, mepc
    sw      t0, 120(sp)

    la      a1, g_active_task
    lw      a1, (a1)
    sw      sp, (a1)

    li      t0, 0xE000E100
    lw      t1, (t0)
    li      t2, 0xFEFFFFFF
    and     t1, t1, t2
    sw      t1, (t0)

__task_switch_nosave:
    la      a0, g_preferred_ready_task
    la      a1, g_active_task
    lw      a2, (a0)
    sw      a2, (a1)

    lw      sp, (a2)

    /* Run in machine mode */
    li      t0, MSTATUS_PRV1
    csrs    mstatus, t0

    lw      t0, 120(sp)
    csrw    mepc, t0

    lw      x1, 0(sp)
    lw      x3, 4(sp)
    lw      x4, 8(sp)
    lw      x5, 12(sp)
    lw      x6, 16(sp)
    lw      x7, 20(sp)
    lw      x8, 24(sp)
    lw      x9, 28(sp)
    lw      x10, 32(sp)
    lw      x11, 36(sp)
    lw      x12, 40(sp)
    lw      x13, 44(sp)
    lw      x14, 48(sp)
    lw      x15, 52(sp)
    lw      x16, 56(sp)
    lw      x17, 60(sp)
    lw      x18, 64(sp)
    lw      x19, 68(sp)
    lw      x20, 72(sp)
    lw      x21, 76(sp)
    lw      x22, 80(sp)
    lw      x23, 84(sp)
    lw      x24, 88(sp)
    lw      x25, 92(sp)
    lw      x26, 96(sp)
    lw      x27, 100(sp)
    lw      x28, 104(sp)
    lw      x29, 108(sp)
    lw      x30, 112(sp)
    lw      x31, 116(sp)

    addi    sp, sp, 124

    flw      f31, 0(sp)
    flw      f30, 4(sp)
    flw      f29, 8(sp)
    flw      f28, 12(sp)
    flw      f27, 16(sp)
    flw      f26, 20(sp)
    flw      f25, 24(sp)
    flw      f24, 28(sp)
    flw      f23, 32(sp)
    flw      f22, 36(sp)
    flw      f21, 40(sp)
    flw      f20, 44(sp)
    flw      f19, 48(sp)
    flw      f18, 52(sp)
    flw      f17, 56(sp)
    flw      f16, 60(sp)
    flw      f15, 64(sp)
    flw      f14, 68(sp)
    flw      f13, 72(sp)
    flw      f12, 76(sp)
    flw      f11, 80(sp)
    flw      f10, 84(sp)
    flw      f9, 88(sp)
    flw      f8, 92(sp)
    flw      f7, 96(sp)
    flw      f6, 100(sp)
    flw      f5, 104(sp)
    flw      f4, 108(sp)
    flw      f3, 112(sp)
    flw      f2, 116(sp)
    flw      f1, 120(sp)
    flw      f0, 124(sp)

    addi    sp, sp, 128


    mret
