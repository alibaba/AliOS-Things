/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef CKCPU_H
#define CKCPU_H

.macro  csky_cpu_initpsr
    mfcr    a3, psr
    bseti   a3, 8
    bseti   a3, 5
    bseti   a3, 31
    bseti   a3, 30
    mtcr    a3, psr
.endm

.macro  csky_cpu_initnspsr
    mfcr    a3, cr<0,3>
    bseti   a3, 5
    bseti   a3, 6
    bseti   a3, 8
    bseti   a3, 31
    mtcr    a3, cr<0,3>
.endm

.macro  csky_cpu_initvec vsrtable
    lrw     a2, \vsrtable
    mtcr    a2, vbr
.endm

.macro  csky_cpu_initnsvec nsvsrtable
    /* setup initial security vector base table for interrupts and exceptions */
    lrw     a2, \nsvsrtable
    mtcr    a2, cr<1,0>
.endm

.macro  csky_cpu_initstack stack
    /* initialize the normal stack pointer from the linker definition */
    lrw     a3, \stack
    mov     sp, a3
.endm

.macro  csky_cpu_init_nor_stack _nor_stack
    /* initialize the normal stack pointer from the linker definition */
    lrw     a3, \_nor_stack
    mtcr    a3,  cr<14,3>
.endm

.macro  csky_cpu_init_ns_stack ns_stack
    /* initialize the normal stack pointer from the linker definition */
    lrw     a3, \ns_stack
    mov     sp, a3
.endm

.macro  csky_cpu_init_nor_ns_stack _nor_ns_stack
    /* initialize the normal stack pointer from the linker definition */
    lrw     a3, \_nor_ns_stack
    mtcr    a3, cr<14,1>
.endm

.macro  csky_cpu_initfstack fstack
    /* initialize the fast interrupt stack pointer */
    psrset  af
    lrw     a3, \fstack
    mov     sp, a3
    psrclr  af
.endm

.macro  csky_bss_init sadr eadr
    lrw     r7, \sadr        /* get start of bss from linking script file */
    lrw     r6, \eadr        /* get end of bss from linking script file */
    subu    r6, r7           /* calculate size of bss */
    lsri    r6, r6, 2        /* size of whole words */
    cmpnei  r6, 0
    bf      .L_bss_init_end
    movi    r5, 0            /* set zero value to write */

.L_bss_init_loop:
    stw     r5, (r7)         /* zero next word */
    addi    r7, 4            /* increase bss pointer */
    subi    r6, 1            /* decrease counter */
    cmpnei  r6, 0
    bt      .L_bss_init_loop /* repeat for all bss */

.L_bss_init_end:
.endm

.macro csky_load_data saddr eaddr eeaddr
    lrw     a3, \saddr   /* get start of data from linking script file */
    lrw     a1, \eaddr   /* get end of data from linking script file */
    cmphs   a3, a1       /* galculate size of data */
    lrw     a2, \eeaddr  /* get end of rodata from linking script file */
.L_load_data:
    ld.w    a0, (a2, 0)  /* load data from flash */
    st.w    a0, (a3, 0)  /* store data to SSRAM */
    addi    a3, 4        /* increase data pointer of flash */
    addi    a2, 4        /* increase data pointer of SSRAM */
    cmphs   a3, a1
    bf      .L_load_data /* repeat for all data */
.endm

#endif

