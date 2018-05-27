/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <xtensa/config/core.h>
#include "frxt/xtensa_rtos.h"
#include "frxt/xtensa_api.h"

#include <k_api.h>
#include "esp_log.h"

/* ARMCC || IAR || GNU */
#define FPU_AVL ((defined(__CC_ARM) && defined(__TARGET_FPU_VFP)) \
                || (defined(__ICCARM__) && defined(__ARMVFP__))   \
                || (defined(__GNUC__) && defined(__VFP_FP__) && !defined(__SOFTFP__)))
extern void _xt_coproc_init(void);
extern void _frxt_tick_timer_init(void);
extern void _frxt_setup_switch();
extern void task_switch();
extern void _xt_user_exit(void);
unsigned krhino_sys_run[portNUM_PROCESSORS] = {0};
unsigned krhino_sys_nest[portNUM_PROCESSORS] = {0};



void *cpu_task_stack_init(cpu_stack_t *stack_base, size_t stack_size,
                          void *arg, task_entry_t entry)
{
    cpu_stack_t *sp;
    uint32_t pxTopOfStack = (uint32_t)(stack_base + stack_size);

    XtExcFrame  *frame;
#if XCHAL_CP_NUM > 0
    uint32_t *p;
#endif

    /* Create interrupt stack frame aligned to 16 byte boundary */
    sp = (cpu_stack_t *) (((uint32_t)(pxTopOfStack + 1) - XT_CP_SIZE - XT_STK_FRMSZ) & ~0xf);

#if 0
    /* Clear the entire frame (do not use memset() because we don't depend on C library) */
    for (tp = sp; tp <= pxTopOfStack; ++tp) {
        *tp = 0;
    }
#endif

    frame = (XtExcFrame *) sp;

    /* Explicitly initialize certain saved registers */
    frame->pc   = (uint32_t) entry;                        /* task entrypoint                */

#ifdef __XTENSA_CALL0_ABI__
    frame->a0   = (uint32_t)krhino_task_deathbed;          /* to terminate GDB backtrace     */
#else
    frame->a4   = (uint32_t)krhino_task_deathbed;          /* to terminate GDB backtrace     */
#endif
    frame->a1   = (uint32_t) sp + XT_STK_FRMSZ;  /* physical top of stack frame    */
    frame->exit = (uint32_t) _xt_user_exit;      /* user exception exit dispatcher */

    /* Set initial PS to int level 0, EXCM disabled ('rfe' will enable), user mode. */
    /* Also set entry point argument parameter. */
#ifdef __XTENSA_CALL0_ABI__
    frame->a2 = (uint32_t) arg;
    frame->ps = PS_UM | PS_EXCM;
#else
    /* + for windowed ABI also set WOE and CALLINC (pretend task was 'call4'd). */
    frame->a6 = (uint32_t) arg;
    frame->ps = PS_UM | PS_EXCM | PS_WOE | PS_CALLINC(1);
#endif

#ifdef XT_USE_SWPRI
    /* Set the initial virtual priority mask value to all 1's. */
    frame->vpri = 0xFFFFFFFF;
#endif

#if XCHAL_CP_NUM > 0
    /* Init the coprocessor save area (see xtensa_context.h) */
    /* No access to TCB here, so derive indirectly. Stack growth is top to bottom.
         * //p = (uint32_t *) xMPUSettings->coproc_area;
     */
    p = (uint32_t *)(((uint32_t)(pxTopOfStack + 1) - XT_CP_SIZE) & ~0xf);
    p[0] = 0;
    p[1] = 0;
    p[2] = (((uint32_t) p) + 12 + XCHAL_TOTAL_SA_ALIGN - 1) & -XCHAL_TOTAL_SA_ALIGN;
#endif

    return sp;
}

void cpu_first_task_start()
{

    // Interrupts are disabled at this point and stack contains PS with enabled interrupts when task context is restored
#if XCHAL_CP_NUM > 0
    /* Initialize co-processor management for tasks. Leave CPENABLE alone. */
    _xt_coproc_init();
#endif
    /* Init the tick divisor value */
    _xt_tick_divisor_init();
    /* Setup the hardware to generate the tick. */
    _frxt_tick_timer_init();
    krhino_sys_run[xPortGetCoreID()] = 1;
    __asm__ volatile ("call0    _frxt_dispatch\n");
}

void krhino_switch_context()
{
    g_active_task[cpu_cur_get()] = g_preferred_ready_task[cpu_cur_get()];
}

void cpu_intrpt_switch()
{
    g_active_task[cpu_cur_get()] = g_preferred_ready_task[cpu_cur_get()];
    _frxt_setup_switch();
}

void _krhino_tick_proc()
{
    krhino_intrpt_enter();
    krhino_tick_proc();
    krhino_intrpt_exit();
}

void cpu_task_switch()
{
    task_switch();
}
