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
.import csky_deal_tee_caller_task
.import csky_get_tee_caller_task

/******************************************************************************
 *                                 EXPORT FUNCTIONS
 ******************************************************************************/

.global cpu_intrpt_save
.global cpu_intrpt_restore
.global cpu_task_switch
.global cpu_intrpt_switch
.global cpu_first_task_start
.global tspend_handler

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
    mfcr    r0, psr
    psrclr  ie
    rts

.type cpu_intrpt_restore, %function
cpu_intrpt_restore:
    mtcr    r0, psr
    rts

/******************************************************************************
 * Functions:
 *     void cpu_intrpt_switch(void);
 *     void cpu_task_switch(void);
 ******************************************************************************/

.type cpu_task_switch, %function
cpu_task_switch:
    lrw     r0, g_intrpt_nested_level
    ldb     r0, (r0)
    cmpnei  r0, 0
    jbf     __task_switch

    lrw     r0, g_active_task
    lrw     r1, g_preferred_ready_task
    ldw     r2, (r1)
    stw     r2, (r0)

    rts


.type cpu_intrpt_switch, %function
cpu_intrpt_switch:
    lrw     r0, g_active_task
    lrw     r1, g_preferred_ready_task
    ldw     r2, (r1)
    stw     r2, (r0)

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
    subi    sp, 68
    stm     r0-r13, (sp)
    stw     r15, (sp, 56)
    mfcr    r0, psr
    stw     r0, (sp, 60)
    stw     r15, (sp, 64)

    lrw     r1, g_active_task
    ldw     r1, (r1)
    stw     sp, (r1)

#if (YUNOS_CONFIG_TASK_STACK_OVF_CHECK > 0)
    jbsr    krhino_stack_ovf_check
#endif

__task_switch_nosave:

#ifdef CONFIG_SUPPORT_REE_SCHEDULE_IN_TEE
    jbsr    csky_get_tee_caller_task
#endif

    lrw     r4, g_preferred_ready_task
    lrw     r5, g_active_task
    ldw     r6, (r4)
    stw     r6, (r5)

#ifdef CONFIG_SUPPORT_REE_SCHEDULE_IN_TEE
    jbsr    csky_deal_tee_caller_task
#endif

#ifdef CONFIG_STACK_GUARD
    jbsr    csky_set_stackbound
#endif
    ldw     sp, (r6)

#ifdef CONFIG_STACK_GUARD
    mfcr    r3, cr<0, 4>
    bseti   r3, 0
    bseti   r3, 1
    mtcr    r3, cr<0, 4>
#endif

    ldw     r0, (sp, 64)
    mtcr    r0, epc
    ldw     r0, (sp, 60)
    mtcr    r0, epsr
    ldw     r15, (sp, 56)
    ldm     r0-r13, (sp)
    addi    sp, 68

    rte

.global tspend_handler
.type   tspend_handler, %function
tspend_handler:
    bkpt
