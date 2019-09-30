/*
*****************************************************************************
**

**  File        : LinkerScript.ld
**
**  Abstract    : Linker script for STM32L496VGTx Device with
**                1024KByte FLASH, 320KByte RAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : STMicroelectronics STM32
**
**
**  Distribution: The file is distributed as is, without any warranty
**                of any kind.
**
**  (c)Copyright Ac6.
**  You may use this file as-is or modify it according to the needs of your
**  project. Distribution of this file (unmodified or modified) is not
**  permitted. Ac6 permit registered System Workbench for MCU users the
**  rights to distribute the assembled, compiled & linked contents of this
**  file as part of an application binary file, provided that it is built
**  using the System Workbench for MCU toolchain.
**
*****************************************************************************
*/

ENTRY(app_entry)

/* Specify the memory areas */
MEMORY
{
    RAM (xrw)   : ORIGIN = 0x20020000, LENGTH = 64K
    FLASH (rx)  : ORIGIN = 0x8080000,  LENGTH = 128K
}

/* Define output sections */
SECTIONS
{
    . = ORIGIN(FLASH);

    .appinfo : {
        . = ALIGN(8);
        KEEP(*(.preamble))
    } > FLASH

    /* The program code and other data goes into FLASH */
    .text : {
        . = ALIGN(8);
        *(.text)           /* .text sections (code) */
        *(.text*)          /* .text* sections (code) */
        *(.glue_7)         /* glue arm to thumb code */
        *(.glue_7t)        /* glue thumb to arm code */
        *(.eh_frame)

        KEEP (*(.init))
        KEEP (*(.fini))

        . = ALIGN(8);
        _etext = .;        /* define a global symbols at end of code */
    } >FLASH

    .ctors : {
        . = ALIGN(8);
        PROVIDE(__ctors_start__ = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        PROVIDE(__ctors_end__ = .);
    } >FLASH

    .dtors : {
        . = ALIGN(8);
        PROVIDE(__dtors_start__ = .);
        KEEP(*(SORT(.dtors.*)))
        KEEP(*(.dtors))
        PROVIDE(__dtors_end__ = .);
    } >FLASH

    /* Constant data goes into FLASH */
    .rodata : {
        . = ALIGN(8);
        *(.rodata)         /* .rodata sections (constants, strings, etc.) */
        *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
        . = ALIGN(8);
    } >FLASH

    .ARM.extab : {
        . = ALIGN(8);
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        . = ALIGN(8);
    } >FLASH

    .ARM : {
        . = ALIGN(8);
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
        . = ALIGN(8);
    } >FLASH

    .preinit_array : {
        . = ALIGN(8);
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP (*(.preinit_array*))
        PROVIDE_HIDDEN (__preinit_array_end = .);
        . = ALIGN(8);
    } >FLASH

    .init_array : {
        . = ALIGN(8);
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array*))
        PROVIDE_HIDDEN (__init_array_end = .);
        . = ALIGN(8);
    } >FLASH

    .fini_array : {
        . = ALIGN(8);
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP (*(SORT(.fini_array.*)))
        KEEP (*(.fini_array*))
        PROVIDE_HIDDEN (__fini_array_end = .);
        . = ALIGN(8);
    } >FLASH

    /* used by the startup to initialize data */
    _sidata = LOADADDR(.data);

    /* Initialized data sections goes into RAM, load LMA copy after code */
    .data : {
        . = ALIGN(8);
        _sdata = .;        /* create a global symbol at data start */
        *(.data)           /* .data sections */
        *(.data*)          /* .data* sections */
        . = ALIGN(8);
        _edata = .;        /* define a global symbol at data end */
    } >RAM AT> FLASH

    /* Uninitialized data section */
    .bss : {
        /* This is used by the startup in order to initialize the .bss secion */
        . = ALIGN(4);
        _sbss = .;         /* define a global symbol at bss start */
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;         /* define a global symbol at bss end */
    } >RAM

    PROVIDE(_app_flash_start = ORIGIN(FLASH));
    PROVIDE(_app_flash_end = ORIGIN(FLASH)+LENGTH(FLASH));

    PROVIDE(_app_ram_start = ORIGIN(RAM));
    PROVIDE(_app_ram_end = ORIGIN(RAM)+LENGTH(RAM));

    PROVIDE(_app_data_flash_start = _sidata);
    PROVIDE(_app_data_ram_start = _sdata);
    PROVIDE(_app_data_ram_end = _edata);

    PROVIDE(_app_bss_start = _sbss);
    PROVIDE(_app_bss_end = _ebss);

    PROVIDE(_app_heap_start = _ebss);
    PROVIDE(_app_heap_end = ORIGIN(RAM) + LENGTH(RAM));

    /* Remove information from the standard libraries */
    /DISCARD/ : {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }

    .ARM.attributes 0 : { *(.ARM.attributes) }
}

