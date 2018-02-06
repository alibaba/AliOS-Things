/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "tee_addr_map.h"

ENTRY(_start)
STARTUP(startup.o)

MEMORY
{
    FLASH : ORIGIN = TW_RO_ADDR, LENGTH = TW_RO_SIZE
    ISRAM : ORIGIN = TW_RW_ADDR, LENGTH = TW_RW_SIZE
}

SECTIONS
{
    .text : {
        . = ALIGN(0x4);
        __text_start = .;
        KEEP(*(.text))
        KEEP(*(.text*))
        . = ALIGN(0x4);
        __text_end = .;
    } > FLASH

    .rodata : {
        . = ALIGN(0x4);
        __rodata_start = .;
        KEEP(*(.rodata))
        KEEP(*(.rodata*))
        . = ALIGN(0x4);
        KEEP(*(SORT(.table.*)))
        . = ALIGN(0x4);
        __rodata_end = .;
    } > FLASH

    __data_copy_start = .;

    .data : AT(__data_copy_start) {
        . = ALIGN(0x4);
        __data_start = .;
        KEEP(*(.exp_table))
        KEEP(*(.data))
        KEEP(*(.data*))
        . = ALIGN(0x4);
        __data_end = .;
    } > ISRAM

    .bss : AT(ADDR(.data) + SIZEOF(.data)) {
        . = ALIGN(0x4);
        __bss_start = .;
        KEEP(*(.bss))
        KEEP(*(.bss*))
        *(COMMON*)
        . = ALIGN(0x4);
        __bss_end = .;
    } > ISRAM
}
