/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

void *cpu_task_stack_init(cpu_stack_t *stack_base, size_t stack_size, void *arg,
                          task_entry_t entry)
{
    cpu_stack_t *stk;
    uint32_t     temp = (uint32_t)(stack_base + stack_size);

    /* stack aligned by 8 byte */
    temp &= 0xfffffff8;
    stk = (cpu_stack_t *)temp;

    /* Exception stack frame with non-floating-point state  */
    *(--stk) = (cpu_stack_t)0x01000000L; /* xPSR: EPSR.T = 1, thumb mode   */
    *(--stk) = (cpu_stack_t)entry;       /* Entry Point                    */
    *(--stk) = (cpu_stack_t)krhino_task_deathbed; /* R14 (LR)              */
    *(--stk) = (cpu_stack_t)0x12121212L; /* R12                            */
    *(--stk) = (cpu_stack_t)0x03030303L; /* R3                             */
    *(--stk) = (cpu_stack_t)0x02020202L; /* R2                             */
    *(--stk) = (cpu_stack_t)0x01010101L; /* R1                             */
    *(--stk) = (cpu_stack_t)arg;         /* R0 : argument                  */

    /* EXC_RETURN = 0xFFFFFFFDL
       Task begin state: Thread mode +  non-floating-point state + PSP */
    *(--stk) = (cpu_stack_t)0xFFFFFFFDL;

    *(--stk) = (cpu_stack_t)0x11111111L; /* R11 */
    *(--stk) = (cpu_stack_t)0x10101010L; /* R10 */
    *(--stk) = (cpu_stack_t)0x09090909L; /* R9  */
    *(--stk) = (cpu_stack_t)0x08080808L; /* R8  */
    *(--stk) = (cpu_stack_t)0x07070707L; /* R7  */
    *(--stk) = (cpu_stack_t)0x06060606L; /* R6  */
    *(--stk) = (cpu_stack_t)0x05050505L; /* R5  */
    *(--stk) = (cpu_stack_t)0x04040404L; /* R4  */

    return stk;
}
