/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <csi_core.h>

#ifdef __CSKY_DSPV2__
#define SAVE_HIGH_REGISTERS
#endif

#ifdef __CSKY_HARD_FLOAT__
#define SAVE_VR_REGISTERS
#endif

static uint32_t is_tee_mode(void)
{
    return (__get_PSR() & PSR_T_Msk) ? 1: 0;
}

void *cpu_task_stack_init(cpu_stack_t *stack_base, size_t stack_size,
                          void *arg, task_entry_t entry)
{
    cpu_stack_t *stk;
    uint32_t temp = (uint32_t)(stack_base + stack_size);
    uint32_t psr;

    temp &= 0xFFFFFFFCUL;

    stk = (cpu_stack_t *)temp;

    if (is_tee_mode()) {
        psr = 0xE0000340L;
    } else {
        psr = 0x80000340L;
    }

    *(--stk) = (uint32_t)entry;                 /* Entry Point   */
    *(--stk) = (uint32_t)psr;                   /* PSR           */
    *(--stk) = (uint32_t)krhino_task_deathbed;  /* R15 (LR)      */
    *(--stk) = (uint32_t)0x13131313L;           /* R13           */
    *(--stk) = (uint32_t)0x12121212L;           /* R12           */
    *(--stk) = (uint32_t)0x11111111L;           /* R11           */
    *(--stk) = (uint32_t)0x10101010L;           /* R10           */
    *(--stk) = (uint32_t)0x09090909L;           /* R9            */
    *(--stk) = (uint32_t)0x00000000L;           /* R8            */
    *(--stk) = (uint32_t)0x07070707L;           /* R7            */
    *(--stk) = (uint32_t)0x06060606L;           /* R6            */
    *(--stk) = (uint32_t)0x05050505L;           /* R5            */
    *(--stk) = (uint32_t)0x04040404L;           /* R4            */
    *(--stk) = (uint32_t)0x03030303L;           /* R3            */
    *(--stk) = (uint32_t)0x02020202L;           /* R2            */
    *(--stk) = (uint32_t)0x01010101L;           /* R1            */
    *(--stk) = (uint32_t)arg;                   /* R0 : Argument */

#ifdef SAVE_HIGH_REGISTERS
    *(--stk) = (uint32_t)0x31313131L;           /* R31           */
    *(--stk) = (uint32_t)0x30303030L;           /* R30           */
    *(--stk) = (uint32_t)0x29292929L;           /* R29           */
    *(--stk) = (uint32_t)0x28282828L;           /* R28           */
    *(--stk) = (uint32_t)0x27272727L;           /* R27           */
    *(--stk) = (uint32_t)0x26262626L;           /* R26           */
    *(--stk) = (uint32_t)0x25252525L;           /* R25           */
    *(--stk) = (uint32_t)0x24242424L;           /* R24           */
    *(--stk) = (uint32_t)0x23232323L;           /* R23           */
    *(--stk) = (uint32_t)0x22222222L;           /* R22           */
    *(--stk) = (uint32_t)0x21212121L;           /* R21           */
    *(--stk) = (uint32_t)0x20202020L;           /* R20           */
    *(--stk) = (uint32_t)0x19191919L;           /* R19           */
    *(--stk) = (uint32_t)0x18181818L;           /* R18           */
    *(--stk) = (uint32_t)0x17171717L;           /* R17           */
    *(--stk) = (uint32_t)0x16161616L;           /* R16           */
#endif

#ifdef SAVE_VR_REGISTERS
    *(--stk) = (uint32_t)0x12345678L;           /* VR15          */
    *(--stk) = (uint32_t)0x12345678L;           /* VR14          */
    *(--stk) = (uint32_t)0x12345678L;           /* VR13          */
    *(--stk) = (uint32_t)0x12345678L;           /* VR12          */
    *(--stk) = (uint32_t)0x12345678L;           /* VR11          */
    *(--stk) = (uint32_t)0x12345678L;           /* VR10          */
    *(--stk) = (uint32_t)0x12345678L;           /* VR9           */
    *(--stk) = (uint32_t)0x12345678L;           /* VR8           */
    *(--stk) = (uint32_t)0x12345678L;           /* VR7           */
    *(--stk) = (uint32_t)0x12345678L;           /* VR6           */
    *(--stk) = (uint32_t)0x12345678L;           /* VR5           */
    *(--stk) = (uint32_t)0x12345678L;           /* VR4           */
    *(--stk) = (uint32_t)0x12345678L;           /* VR3           */
    *(--stk) = (uint32_t)0x12345678L;           /* VR2           */
    *(--stk) = (uint32_t)0x12345678L;           /* VR1           */
    *(--stk) = (uint32_t)0x12345678L;           /* VR0           */
#endif

    return stk;
}
