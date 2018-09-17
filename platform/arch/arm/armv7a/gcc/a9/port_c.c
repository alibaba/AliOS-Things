/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>

extern int fpu_reg_count(void);


/* ARMCC || IAR || GNU */
#define FPU_AVL                                          \
    ((defined(__CC_ARM) && defined(__TARGET_FPU_VFP)) || \
     (defined(__ICCARM__) && defined(__ARMVFP__)) ||     \
     (defined(__GNUC__) && defined(__VFP_FP__) && !defined(__SOFTFP__)))

#define ARM_MODE_ARM 0x00000000u
#define ARM_MODE_THUMB 0x00000020u

#define ARM_SVC_MODE_THUMB (0x00000013u + ARM_MODE_THUMB)
#define ARM_SVC_MODE_ARM (0x00000013u + ARM_MODE_ARM)

void *cpu_task_stack_init(cpu_stack_t *stack_base, size_t stack_size, void *arg,
                          task_entry_t entry)
{
    cpu_stack_t *stk;
    cpu_stack_t  task_addr;
    uint32_t     temp = (uint32_t)(stack_base + stack_size);

    /* stack aligned by 8 byte */
    temp &= 0xfffffff8;
    stk = (cpu_stack_t *)temp;

    task_addr = (cpu_stack_t)entry &
                ~1u; /* Mask off lower bit in case task is thumb mode        */

    *(--stk) = (cpu_stack_t)task_addr;            /* Entry Point            */
    *(--stk) = (cpu_stack_t)krhino_task_deathbed; /* Reg R14 (LR) */
    *(--stk) = (cpu_stack_t)0x12121212u;          /* Reg R12          */
    *(--stk) = (cpu_stack_t)0x11111111u;          /* Reg R11          */
    *(--stk) = (cpu_stack_t)0x10101010u;          /* Reg R10          */
    *(--stk) = (cpu_stack_t)0x09090909u;          /* Reg R9          */
    *(--stk) = (cpu_stack_t)0x08080808u;          /* Reg R8          */
    *(--stk) = (cpu_stack_t)0x07070707u;          /* Reg R7          */
    *(--stk) = (cpu_stack_t)0x06060606u;          /* Reg R6          */
    *(--stk) = (cpu_stack_t)0x05050505u;          /* Reg R5          */
    *(--stk) = (cpu_stack_t)0x04040404u;          /* Reg R4          */
    *(--stk) = (cpu_stack_t)0x03030303u;          /* Reg R3          */
    *(--stk) = (cpu_stack_t)0x02020202u;          /* Reg R2          */
    *(--stk) = (cpu_stack_t)0x01010101u;          /* Reg R1          */
    *(--stk) = (cpu_stack_t)arg;                  /* Reg R0 : argument                  */

    if (((cpu_stack_t)entry & 0x01u) ==
        0x01u) { /* See if task runs in Thumb or ARM mode                */
        *(--stk) =
          (cpu_stack_t)ARM_SVC_MODE_THUMB; /* CPSR  (Enable IRQ and FIQ
                                              interrupts, THUMB-mode)    */
    } else {
        *(--stk) = (cpu_stack_t)ARM_SVC_MODE_ARM; /* CPSR  (Enable IRQ and FIQ
                                                     interrupts, ARM-mode) */
    }

#if (FPU_AVL > 0)
    uint32_t fpu_reg_cnt = fpu_reg_count();
    uint32_t i;


    if (fpu_reg_cnt != 0u) {
        *(--stk) = (cpu_stack_t)0; /* Initialize Floating point status & control
                                      register  */
        /* Initialize general-purpose Floating point registers  */
        for (i = 0u; i < fpu_reg_cnt * 2u; i++) {
            *(--stk) = (cpu_stack_t)0;
        }

        *(--stk) = (cpu_stack_t)(0x40000000); /* Initialize Floating-Point
                                                 Exception Register (Enable)*/
    }
#endif

    return stk;
}
