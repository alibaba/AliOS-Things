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

#include <k_config.h>
#include <csi_config.h>

.extern g_active_task
.extern g_preferred_ready_task

/******************************************************************************
 *                                 EXPORT FUNCTIONS
 ******************************************************************************/

.global cpu_intrpt_save
.global cpu_intrpt_restore
.global cpu_task_switch
.global cpu_intrpt_switch
.global cpu_first_task_start

/******************************************************************************
 *                                 EQUATES
 ******************************************************************************/

/******************************************************************************
 *                                 CODE GENERATION DIRECTIVES
 ******************************************************************************/

.text
.align 2

/******************************************************************************
 * Functions:
 *     size_t cpu_intrpt_save(void);
 *     void   cpu_intrpt_restore(size_t psr);
 ******************************************************************************/

.type cpu_intrpt_save, %function
cpu_intrpt_save:
    mfcr    r2, psr
    psrclr  ie
    rts

.type cpu_intrpt_restore, %function
cpu_intrpt_restore:
    mtcr    r2, psr
    rts

/******************************************************************************
 * Functions:
 *     void cpu_intrpt_switch(void);
 *     void cpu_task_switch(void);
 ******************************************************************************/

.type cpu_task_switch, %function
cpu_task_switch:
    lrw     r2, g_intrpt_nested_level
    ldb     r2, (r2)
    cmpnei  r2, 0
    jbf     __task_switch

    lrw     r2, g_active_task
    lrw     r3, g_preferred_ready_task
    ldw     r4, (r3)
    stw     r4, (r2)

    rts


.type cpu_intrpt_switch, %function
cpu_intrpt_switch:
    lrw     r2, g_active_task
    lrw     r3, g_preferred_ready_task
    ldw     r4, (r3)
    stw     r4, (r2)

    rts

/******************************************************************************
 * Functions:
 *     void cpu_first_task_start(void);
 ******************************************************************************/

.type cpu_first_task_start, %function
cpu_first_task_start:
    psrclr  ie
    jbr     __task_switch_nosave

/******************************************************************************
 * Functions:
 *     void __task_switch(void);
 ******************************************************************************/

.type __task_switch, %function
__task_switch:
    subi    sp, 32
    subi    sp, 28
    stm     r1-r15, (sp)
    subi    sp, 8
    mfcr    r1, epsr
    stw     r1, (sp, 4)
    stw     r15, (sp, 0)

    lrw     r2, g_active_task
    ldw     r3, (r2)
    stw     sp, (r3)

#if (YUNOS_CONFIG_TASK_STACK_OVF_CHECK > 0)
    jbsr    krhino_stack_ovf_check
#endif

__task_switch_nosave:

    lrw     r2, g_preferred_ready_task
    lrw     r3, g_active_task
    ldw     r4, (r2)
    stw     r4, (r3)
    ldw     sp, (r4)

    ldw     r1, (sp, 0)
    mtcr    r1, epc
    ldw     r1, (sp, 4)
    mtcr    r1, epsr
    addi    sp, 8
    ldm     r1-r15, (sp)
    addi    sp, 32
    addi    sp, 28
    rte

