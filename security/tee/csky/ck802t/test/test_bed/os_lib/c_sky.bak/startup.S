/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "cksmart.h"
#include "tee_addr_map.h"

.file "startup.S"


// vincent add
.import ebr_except_table
//


/*
 * For importing variable or functions from script file "ckcpu.ld".
 */
.import    __sbss        /* The end of bss from linking script file */
.import __ebss        /* The end of bss from linking script file */

/*
 * For importing variable or functions from other c or assemble files.
 */
.import main                /* The c codes entry */

/*
 * For importing variable or functions from entry.S.
 */
.import default_exception_handler /* default exception service routine */

/* For exporting varialbe or functions */
.export    ckcpu_vsr_table    /* Vector table base address. */
.export __start                /* The system entry. */

.section .exp_table
/* Vector table space. */

.align 10
ckcpu_vsr_table:

.long __start
.rept 127
.long default_exception_handler
.endr

/* The ckcpu startup codes. */
.text
    /*
     *
     * This is the codes first entry point. This is where it all
     * begins...
     */

__start:
    /*
     * Init Epsr value, enable exception, disable interrupt and fast interrupt.
     * psr = 0x80000100
     */
    movi    a3, 0
    bseti    a3, 31
    bseti    a3, 8
    mtcr    a3, psr

    /*
     * Setup initial vector base table for interrupts and exceptions
     */
    lrw        a2, ckcpu_vsr_table
    mtcr    a2, vbr

    /* Initialize the normal stack pointer from the end of NTW rw. */
    lrw        a1, NTW_RW_ADDR + NTW_RW_SIZE
    mov        sp, a1

    /*
     * Copy the data section to ISRAM.
     * data in flash start: __data_copy_start
     * data in isram start: __data_start
     * data size:           __data_start - __data_end
     */
    lrw        a3, __data_copy_start
    lrw        a2, __data_start    /* Get start of bss from linking script file */
    lrw        a1, __data_end    /* Get end of bss from linking script file */
    subu    a1, a2      /* Calculate size of bss */
    lsri    a1, 2        /* Size of words */

    /* a1: size in worlds. a2: start addr in isram. a3: start addr in flash */
    cmpnei    a1, 0
    bf      __skip_data_copy

7:
    ld.w    a0, (a3)
    st.w    a0, (a2)
    addi    a3, 4
    addi    a2, 4
    decne    a1            /* Decrease counter */
    bt        7b            /* Repeat for all bss */
__skip_data_copy:

    /*
     * Zero out the bss region.
     * NOTE: __bss_start must align 4
     */
    lrw        a3, __bss_start    /* Get start of bss from linking script file */
    lrw        a2, __bss_end    /* Get end of bss from linking script file */
    subu    a2, a3        /* Calculate size of bss */
    lsri    a2, 2        /* Size of whole words */
    cmpnei    a2, 0
    bf        __goto_c
    movi    a1, 0        /* Set zero value to write */
2:
    stw        a1, (a3)    /* Zero next word */
    addi    a3, 4        /* Increase bss pointer */
    decne    a2            /* Decrease counter */
    bt        2b            /* Repeat for all bss */

    /*
     * Assember start up done, C codes start here.
     */

    // vincent add
    lrw   r0, ebr_except_table
    mtcr  r0, cr<1,1>
    //
__goto_c:
    bsr        main

    /* Should never get here. */
__exit:
    br    __exit
