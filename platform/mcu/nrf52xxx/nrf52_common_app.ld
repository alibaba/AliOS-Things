/* Linker script for Nordic Semiconductor nRF52 devices
 *
 * Version: Sourcery G++ 4.5-1
 * Support: https://support.codesourcery.com/GNUToolchain/
 *
 * Copyright (c) 2007, 2008, 2009, 2010 CodeSourcery, Inc.
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions.  No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */

OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapBase
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 */

  MEMORY
 {
   FLASH (rx) : ORIGIN = 0x00060000, LENGTH = 0x20000
   RAM (rwx) :  ORIGIN = 0x20020000, LENGTH = 0x20000
 }

ENTRY(application_start);


SECTIONS
{
    . = 0x00060000;

    .appinfo :
    {
        KEEP(*(*.app_info))
    } > FLASH

    . = ALIGN(8);
    .text :
    {
        *(.text*)

        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        *(.rodata*)

        KEEP(*(.eh_frame*))
    } > FLASH

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    __exidx_end = .;

    . = ALIGN(0x8);
    __etext = .;
    .data : AT (__etext)
    {
        __data_start__ = .;
        *(vtable)
        *(.data*)
		
	    . = ALIGN(8);	
		/*DEVICE_INIT_SECTIONS    start*/
		__device_init_start = .;
		
		__device_PRE_KERNEL_1_start = .;			
		KEEP(*(SORT(.init_PRE_KERNEL_1[0-9])));		
		KEEP(*(SORT(.init_PRE_KERNEL_1[1-9][0-9])));	
		
		__device_PRE_KERNEL_2_start = .;			
		KEEP(*(SORT(.init_PRE_KERNEL_2[0-9])));		
		KEEP(*(SORT(.init_PRE_KERNEL_2[1-9][0-9])));
		
		__device_POST_KERNEL_start = .;			
		KEEP(*(SORT(.init_POST_KERNEL[0-9])));		
		KEEP(*(SORT(.init_POST_KERNEL[1-9][0-9])));
		
		__device_APPLICATION_start = .;			
		KEEP(*(SORT(.init_APPLICATION[0-9])));		
		KEEP(*(SORT(.init_APPLICATION[1-9][0-9])));
		
		__device_init_end = .;
		/*DEVICE_INIT_SECTIONS    start*/
		
		
        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);


        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))
        . = ALIGN(4);
        /* All data end */
        __data_end__ = .;

    } > RAM

    PROVIDE(_app_data_flash_begin = LOADADDR(.data));
    PROVIDE(_app_data_ram_begin = ADDR(.data));
    PROVIDE(_app_data_ram_end = .);

    .bss ALIGN(8):
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > RAM

    PROVIDE(_app_bss_start = __bss_start__);
    PROVIDE(_app_bss_end = __bss_end__);

    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */
    .stack_dummy :
    {
		. = ALIGN(8);
		PROVIDE(__StackStart = .);
		. = . + 0x200;
		PROVIDE(__StackEnd = .);
    } > RAM
	
	__StackLen = __StackEnd - __StackStart;
    PROVIDE(__stack = __StackEnd);
	PROVIDE(__StackTop = __stack);

	PROVIDE (__HeapBase = __stack);
	PROVIDE (heap_start = __HeapBase );
	
    __HeapLimit = ORIGIN(RAM) + LENGTH(RAM);
	PROVIDE (heap_end = __HeapLimit);
	PROVIDE (heap_len = heap_end - heap_start);
	
    /* Check if data + heap + stack exceeds RAM limit */
    /*ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")*/
    
    /* Check if text sections + data exceeds FLASH limit */
    DataInitFlashUsed = __bss_start__ - __data_start__;
    CodeFlashUsed = __etext - ORIGIN(FLASH);
    TotalFlashUsed = CodeFlashUsed + DataInitFlashUsed;
    ASSERT(TotalFlashUsed <= LENGTH(FLASH), "region FLASH overflowed with .data and user data")

    PROVIDE(_app_heap_start = heap_start);
    PROVIDE(_app_heap_end = heap_end);

    PROVIDE(app_info_addr = 0x00060000);
    PROVIDE(kernel_download_addr = 0x10000);
    PROVIDE(app_download_addr = 0x00060000);
    PROVIDE(ota_download_addr = 0x0);

    PROVIDE(app_type = 0xABABABAB);
    
}

GROUP(
  libgcc.a
  libg.a
  libc.a
  libm.a
  libnosys.a
)

