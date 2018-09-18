/**
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "hal_memmap.h"
#include "tee_addr_map.h"

/*
 * import/export variable or functions.
 */
.import __data_copy_start
.import __data_start
.import __data_end
.import __bss_start
.import __bss_end
.import get_mft_idx_addr
.import tee_main
.import hal_platform_init
.import hal_get_ree_entry
.export cpu_vsr_table       /* Vector table base address. */
.export _start              /* The system entry. */
.export g_mft_idx_base

.section .exp_table
/* Vector table space. */
.align 10
cpu_vsr_table:

/* 0: Rest */
.long wsc_intr_handler   /* Use cpu_vsr_table[0] as ebr entry */
/* 1 ~~ ARCH_WSC_INTR_ID-1: default exception */
.rept ARCH_WSC_INTR_ID - 1
.long sys_exception_handler
.endr

/* ARCH_WSC_INTR_ID: */
.long wsc_intr_handler

/* ARCH_WSC_INTR_ID+1 ~~ ARCH_SYS_EXCEPTION_NUM: default exception */
.rept ARCH_SYS_EXCEPTION_NUM - ARCH_WSC_INTR_ID
.long sys_exception_handler
.endr

/* ARCH_SYS_EXCEPTION_NUM ~~ ARCH_SYS_EXCEPTION_NUM: default exception */
.rept ARCH_SYS_EXCEPTION_NUM - ARCH_INTERRUPT_NUM
.long default_interrupt_handler
.endr

/* The cpu startup codes. */
.text
    /*
     *
     * This is the codes first entry point. This is where it all
     * begins...
     */

_start:

    /* Enable instruction cache */
    lrw r0, 0xe000f000   /* Cache register base address */
    lrw r1, 0x10000063   /* CRCR0 value means: Cached data from 0x100000000, Size=256KB, Enable*/
    stw r1, (r0, 0x8)    /* store 0x10000063 to CRCR0(0xe000f008) */
    lrw r1, 0x1          /* store 0x1 to r1 */
    stw r1, (r0, 0x4)    /* set CIR(0xe000f004) to 0x1, means invalid all caches */
    lrw r1, 0x3          /* store 0x3 to r1 */
    stw r1, (r0, 0x0)    /* set CER(0xe000f000) to 0x3, means only enable instructions cache*/

    /* FXIME, remove this hard-code */
    lrw     r0, PLATFORM_OTP_SECURE_SECTOR_START

    /* save r0 to r9,
     * make sure r9 not be used in boot stage
     */
    mov      r9, r0
    /*
     * psr = 0xc0000000
     */
    movi     a3, 0
    bseti    a3, 31 /* S */
    bseti    a3, 30 /* SE */
    mtcr     a3, psr

    /*
     * Setup secure initial vector base table for interrupts and exceptions
     */
    lrw      a2, cpu_vsr_table
    mtcr     a2, vbr
    mtcr     a2, cr<1,1>          /* Use cpu_vsr_table[0] as ebr entry. cr<1,1> is T_EBR */

    /* Re-Init psr */
    mfcr     a3, psr
    bseti    a3, 8  /* enable EE */
    mtcr     a3, psr

    /* Initialize the normal stack pointer from the end of TW volatile mem. */
    lrw      a1, TW_RW_ADDR + TW_RW_SIZE
    bclri    a1, 0  /* Make sure 4Bytes align */
    bclri    a1, 1
    mov      sp, a1

    /*
     * Copy the data section to ISRAM.
     * data in flash start: __data_copy_start
     * data in isram start: __data_start
     * data size:           __data_start - __data_end
     */
    lrw      a3, __data_copy_start
    lrw      a2, __data_start    /* Get start of bss from linking script file */
    lrw      a1, __data_end    /* Get end of bss from linking script file */
    subu     a1, a2      /* Calculate size of bss */
    lsri     a1, 2        /* Size of words */

    /* a1: size in worlds. a2: start addr in isram. a3: start addr in flash */
    cmpnei   a1, 0
    bf       __skip_data_copy

7:
    ld.w     a0, (a3)
    st.w     a0, (a2)
    addi     a3, 4
    addi     a2, 4
    decne    a1            /* Decrease counter */
    bt       7b            /* Repeat for all bss */
__skip_data_copy:

    /*
     * Zero out the bss region.
     * NOTE: __bss_start must align 4
     */
    lrw      a3, __bss_start    /* Get start of bss from linking script file */
    lrw      a2, __bss_end    /* Get end of bss from linking script file */
    subu     a2, a3        /* Calculate size of bss */
    lsri     a2, 2        /* Size of whole words */
    cmpnei   a2, 0
    bf       __goto_c
    movi     a1, 0        /* Set zero value to write */
2:
    stw      a1, (a3)    /* Zero next word */
    addi     a3, 4        /* Increase bss pointer */
    decne    a2            /* Decrease counter */
    bt       2b            /* Repeat for all bss */

    /*
     * Assember start up done, C codes start here.
     */
__goto_c:
    bsr      hal_platform_init

    /* Store OTP base address to g_mtf_idx_base */
    lrw      r1, g_mft_idx_base
    stw      r9, (r1, 0)

    bsr      hal_get_ree_entry

    bsr      tee_main


    /* Should never get here. */
__exit:
    br       __exit

.section .data
.align  2
g_mft_idx_base:
.rept   1
.long   0x00000000
.endr
