/*
 * Copyright (C) 2021-2022 Alibaba Group Holding Limited
 */

#include <k_api.h>

#define ST0_IE  0x00000001
#define ST0_EXL 0x00000002

cpu_cpsr_t cpu_intrpt_save(void)
{
    uint32_t sr_val;
    cpu_cpsr_t cpsr;

    asm volatile("mfc0 %0, $12" : "=r"(sr_val));
    cpsr = sr_val & ST0_IE;
    sr_val &= ~ST0_IE;
    asm volatile("mtc0 %0, $12\n\tnop\n\tnop\n\tnop" :: "r"(sr_val));

    return cpsr;
}

void cpu_intrpt_restore(cpu_cpsr_t cpsr)
{
    uint32_t sr_val;

    asm volatile("mfc0 %0, $12" : "=r"(sr_val));
    sr_val &= ~ST0_IE;
    sr_val |= cpsr & ST0_IE;
    asm volatile("mtc0 %0, $12\n\tnop\n\tnop\n\tnop" :: "r"(sr_val));
}

void *cpu_task_stack_init(cpu_stack_t *stack_base, size_t stack_size, void *arg, task_entry_t entry)
{
    cpu_stack_t *stk;
    uint32_t sr_val;
    uint32_t gp_val;

    stk = (cpu_stack_t *)((uint32_t)(stack_base + stack_size) & 0xFFFFFFF8);

    asm volatile("mfc0 %0, $12" : "=r"(sr_val));
    sr_val |= ST0_IE | ST0_EXL;
    asm volatile("addi %0, $28, 0" : "=r"(gp_val));

    *(--stk) = 0;                       /* reserved for argument 3 */
    *(--stk) = 0;                       /* reserved for argument 2 */
    *(--stk) = 0;                       /* reserved for argument 1 */
    *(--stk) = 0;                       /* reserved for argument 0 */

    *(--stk) = (uint32_t)entry;         /* GPR[31] */
    *(--stk) = (uint32_t)0x30303030;    /* GPR[30] */
    *(--stk) = gp_val;                  /* GPR[28] */
    *(--stk) = (uint32_t)0x27272727;    /* GPR[27] */
    *(--stk) = (uint32_t)0x26262626;    /* GPR[26] */
    *(--stk) = (uint32_t)0x25252525;    /* GPR[25] */
    *(--stk) = (uint32_t)0x24242424;    /* GPR[24] */
    *(--stk) = (uint32_t)0x23232323;    /* GPR[23] */
    *(--stk) = (uint32_t)0x22222222;    /* GPR[22] */
    *(--stk) = (uint32_t)0x21212121;    /* GPR[21] */
    *(--stk) = (uint32_t)0x20202020;    /* GPR[20] */
    *(--stk) = (uint32_t)0x19191919;    /* GPR[19] */
    *(--stk) = (uint32_t)0x18181818;    /* GPR[18] */
    *(--stk) = (uint32_t)0x17171717;    /* GPR[17] */
    *(--stk) = (uint32_t)0x16161616;    /* GPR[16] */
    *(--stk) = (uint32_t)0x15151515;    /* GPR[15] */
    *(--stk) = (uint32_t)0x14141414;    /* GPR[14] */
    *(--stk) = (uint32_t)0x13131313;    /* GPR[13] */
    *(--stk) = (uint32_t)0x12121212;    /* GPR[12] */
    *(--stk) = (uint32_t)0x11111111;    /* GPR[11] */
    *(--stk) = (uint32_t)0x10101010;    /* GPR[10] */
    *(--stk) = (uint32_t)0x09090909;    /* GPR[9] */
    *(--stk) = (uint32_t)0x08080808;    /* GPR[8] */
    *(--stk) = (uint32_t)0x07070707;    /* GPR[7] */
    *(--stk) = (uint32_t)0x06060606;    /* GPR[6] */
    *(--stk) = (uint32_t)0x05050505;    /* GPR[5] */
    *(--stk) = (uint32_t)arg;           /* GPR[4] */
    *(--stk) = (uint32_t)0x03030303;    /* GPR[3] */
    *(--stk) = (uint32_t)0x02020202;    /* GPR[2] */
    *(--stk) = (uint32_t)0x01010101;    /* GPR[1] */
    *(--stk) = (uint32_t)0x00000000;    /* Special Purpose HI Register */
    *(--stk) = (uint32_t)0x00000000;    /* Special Purpose LO Register */
    *(--stk) = (uint32_t)entry;         /* EPC */
    *(--stk) = sr_val;                  /* SR */
    *(--stk) = 0;                       /* reserved for argument 3 */
    *(--stk) = 0;                       /* reserved for argument 2 */
    *(--stk) = 0;                       /* reserved for argument 1 */
    *(--stk) = 0;                       /* reserved for argument 0 */

    return stk;
}
