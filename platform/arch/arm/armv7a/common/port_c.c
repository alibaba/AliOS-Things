/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <k_arch.h>

void *cpu_task_stack_init(cpu_stack_t *stack_base, size_t stack_size,
                          void *arg, task_entry_t entry)
{
    context_t *ctx;
#if FPU_AVL
    uint32_t i;
#endif

    /* stack aligned by 8 byte */
    ctx = (context_t *)((long)(stack_base + stack_size)&0xfffffff8);
    ctx--;

    if (((long)entry & 1u) == 1u) {     /* If task runs in Thumb or ARM mode                */
       ctx->CPSR = CPSR_SVC_THUMB;      /* CPSR  (Enable IRQ and FIQ interrupts, THUMB-mode)    */
    } else {
       ctx->CPSR = CPSR_SVC_ARM;        /* CPSR  (Enable IRQ and FIQ interrupts, ARM-mode)      */
    }
    ctx->R0     = (long)arg;
    ctx->R1     = 0x01010101u;
    ctx->R2     = 0x02020202u;
    ctx->R3     = 0x03030303u;
    ctx->R4     = 0x04040404u;
    ctx->R5     = 0x05050505u;
    ctx->R6     = 0x06060606u;
    ctx->R7     = 0x07070707u;
    ctx->R8     = 0x08080808u;
    ctx->R9     = 0x09090909u;
    ctx->R10    = 0x10101010u;
    ctx->R11    = 0x11111111u;
    ctx->R12    = 0x12121212u;
    ctx->LR     = (long)krhino_task_deathbed;
    ctx->PC     = (long)entry & ~1u;

#if FPU_AVL
    ctx->FPSCR = 0;                     /* Initialize Floating point status & control register  */
    ctx->FPEXC = 0x40000000;            /* Initialize Floating-Point Exception Register (Enable)*/
#if NEON_AVL
    for (i = 0u; i < 64; i++) {         /* Initialize general-purpose Floating point registers  */
        ctx->FPU[i] = 0;
    }
#else
    for (i = 0u; i < 32; i++) {         /* Initialize general-purpose Floating point registers  */
        ctx->FPU[i] = 0;
    }
#endif
#endif

    return (void *)ctx;
}

