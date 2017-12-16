/* FILE GENERATED AUTOMATICALLY - DON'T CHANGE IT MANUALLY */

/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

MEMORY
{
    /* eXecuting-In-Place */
    XIP_MEM : ORIGIN = 0x1000a12c, LENGTH = 0x00005ed4
    /* RELocating */
    REL_MEM : ORIGIN = 0x20002954, LENGTH = 0x000016ac
}

SECTIONS
{
    /* srv_head ********************************************************/
    .srv.head : {
        LONG(0x76527354);           /* 0x00000000: magic */
        LONG(0x00000001);           /* 0x00000004: version */
        LONG(0x00000000);           /* 0x00000008: tee_fix */
        LONG(__srv_data_start);     /* 0x0000000C: data start */
        LONG(__srv_data_end);       /* 0x00000010: data end */
        LONG(SIZEOF(.srv.head) +
                SIZEOF(.srv.data) +
                SIZEOF(.text) +
                SIZEOF(.rodata));   /* 0x00000014: data offset */
        LONG(__srv_bss_start);      /* 0x00000018: bss start */
        LONG(__srv_bss_end);        /* 0x0000001C: bss end */
        LONG(SIZEOF(.srv.head) +
                SIZEOF(.srv.data) +
                SIZEOF(.text) +
                SIZEOF(.rodata) +
                SIZEOF(.data));     /* 0x00000020: bss offset */
    } > XIP_MEM
    .srv.data : AT(ADDR(.srv.head) + SIZEOF(.srv.head)) {
        KEEP(*(.srv.data))          /* 0x00000024: srv_data */
    } > XIP_MEM
    /* srv_head ********************************************************/

    .text : AT(ADDR(.srv.data) + SIZEOF(.srv.data)) {
        . = ALIGN(0x4);
        __srv_text_start = .;
        KEEP(*(.text))
        KEEP(*(.text*))
        . = ALIGN(0x4);
        __srv_text_end = .;
    } > XIP_MEM

    .rodata : AT(ADDR(.text) + SIZEOF(.text)) {
        . = ALIGN(0x4);
        __srv_rodata_start = .;
        KEEP(*(.rodata))
        KEEP(*(.rodata*))
        . = ALIGN(0x4);
        __srv_rodata_end = .;
    } > XIP_MEM

    .data : AT(ADDR(.rodata) + SIZEOF(.rodata)) {
        . = ALIGN(0x4);
        __srv_data_start = .;
        KEEP(*(.data))
        KEEP(*(.data*))
        . = ALIGN(0x4);
        __srv_data_end = .;
    } > REL_MEM

    .bss : AT(ADDR(.data) + SIZEOF(.data)) {
        . = ALIGN(0x4);
        __srv_bss_start = .;
        KEEP(*(.bss))
        KEEP(*(.bss*))
        . = ALIGN(0x4);
        __srv_bss_end = .;
    } > REL_MEM
}
