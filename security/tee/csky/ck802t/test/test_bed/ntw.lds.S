/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 */

#include "tee_addr_map.h"

ENTRY(__start)
STARTUP(startup.o)

MEMORY
{
    FLASH : ORIGIN = NTW_RO_ADDR, LENGTH = NTW_RO_SIZE
    ISRAM : ORIGIN = NTW_RW_ADDR, LENGTH = NTW_RW_SIZE
}

SECTIONS
{
    .text : {
        . = ALIGN(0x4);
        KEEP(*(.exp_table))
		// vincent add
		. = ALIGN(0x400);
		*(.wsc.data)
		*(.wsc.data*)
		// end
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
        . = ALIGN(0x4);
        __bss_end = .;
    } > ISRAM
}
