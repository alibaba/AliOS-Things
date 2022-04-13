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
    addi    sp, sp, -60

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
    csrr    t0, mepc
    sw      t0, 56(sp)

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

    lw      t0, 56(sp)
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

    addi    sp, sp, 60
    mret
