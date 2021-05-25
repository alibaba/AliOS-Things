/*
 * Copyright (C) 2017-2019 Alibaba Group Holding Limited
 */

 /******************************************************************************
 * @file     startup.S
 * @brief    startup file. Should use with
 *           GCC for CSKY Embedded Processors
 * @version  V1.0
 * @date     29. July 2019
 ******************************************************************************/

#include <csi_config.h>

.section .vectors
    .align  6
    .globl  __Vectors
    .type   __Vectors, @object
__Vectors:
    j   Default_Handler				/* 0 */
    j   Stspend_Handler			/* 1 */
    j   Default_Handler				/* 2 */
    j   Mtspend_Handler			/* 3 */
    j   Default_Handler				/* 4 */
    j   Scoret_Handler			/* 5 */
    j   Default_Handler				/* 6 */
    j   Mcoret_Handler			/* 7 */
    j   Default_Handler				/* 8 */
    j   Sirq_Handler			/* 9 */
    j   Default_Handler				/* 10 */
    j   Default_IRQHandler			/* 11 */
    j   Default_Handler
    j   Default_Handler
    j   Default_Handler
    j   Default_Handler


    .text
    .align  2
_start:
    .text
    .align  2
    .globl  Reset_Handler
    .type   Reset_Handler, %function
Reset_Handler:
.option push
.option norelax
    la      gp, __global_pointer$
.option pop
    /* disable ie and clear all interrupts */
    csrw    mie, zero
    csrw    mip, zero

    /* C910 will invalid all i-cache and d-cache automatically when reset */
    /* invalid all MMU TLB Entry */
    sfence.vma x0,x0
	/* setup mmu VA(0~1G) <==>  PA(0~1G) */
    /* set SMEH, page size 1G */
    /* set SMEH, 0x0 maps 0x0, ASID = 0 */
    li		a0, (0x40 << 16)
    csrw	smeh, a0
    /* set SMEL, 0x0 maps 0x0, So = 0, C = 0, B = 0, SH = 0, D = 1, A = 1, G = 1, U = 1, XWR = 7, V = 1 */
    li		a0, 0xFF
    csrw	smel, a0
    /* set SMCIR, ramdom write */
    li		a0, 0x10000000
    csrw	smcir, a0
    /* set SATP mode sv39 , enable mmu */
    li		a0, 8 << 60
    csrw	satp, a0

    /* setup mmu VA(256G~256G+1G) <==>  PA(256G~256G+1G) */
    /* set SMEH, page size 1G */
    /* set SMEH, 256G maps 256G */
    li		a0, (0x40 << 16)
    li		a1, 0x1 << (26 + 18)
    or		a0, a0,a1
    csrw	smeh, a0
    /* set SMEL, 256G maps 256G */
    li		a0, 0xFF
    li		a1, 0x1 << (26 + 10)
    or		a0, a0,a1
    csrw	smel, a0
    /* set SMCIR, ramdom write */
    li		a0, 0x10000000
    csrw	smcir, a0
    /* set SATP mode sv39 , enable mmu */
    li		a0, 8 << 60
    csrw	satp, a0
    sfence.vma



    la      a0, __Vectors
    li	    a1, 0x1
    or	    a0, a0,a1
    csrw    mtvec, a0

#if ((CONFIG_CPU_E906F==1) || (CONFIG_CPU_E906D==1))|| (CONFIG_CPU_C910D==1)
    li      a0, 0x2000
    csrs    mstatus, a0
#endif

#if ((CONFIG_CPU_E906F==1) || (CONFIG_CPU_E906D==1))
    la      a0, __Vectors
    csrw    mtvt, a0
#endif

    la      sp, g_top_irqstack

    /* Load data section */
    la      a0, __erodata
    la      a1, __data_start__
    la      a2, __data_end__
    bgeu    a1, a2, 2f
1:
    ld      t0, (a0)
    sd      t0, (a1)
    addi    a0, a0, 4
    addi    a1, a1, 4
    bltu    a1, a2, 1b
2:

    /* Clear bss section */
    la      a0, __bss_start__
    la      a1, __bss_end__
    bgeu    a0, a1, 2f
1:
    sd      zero, (a0)
    addi    a0, a0, 4
    bltu    a0, a1, 1b
2:

#ifndef __NO_SYSTEM_INIT
    jal     SystemInit
#endif

#ifndef __NO_BOARD_INIT
    jal     board_pre_init
#endif

    jal     main

    .size   Reset_Handler, . - Reset_Handler

__exit:
    j      __exit

.section .bss
    .align  3
    .global g_base_irqstack
    .global g_top_irqstack
g_base_irqstack:
    .space CONFIG_ARCH_INTERRUPTSTACK
g_top_irqstack:
