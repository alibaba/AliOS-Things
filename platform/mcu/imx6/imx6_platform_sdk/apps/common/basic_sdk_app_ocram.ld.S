/*
 * Copyright (c) 2010-2012, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
#define OCRAM_ONLY_APP 1
/*
 * OCRAM
 */
#if defined(CHIP_MX6SDL) || defined(CHIP_MX6SL)
    // mx6sl and mx6sdl both have 128kB of OCRAM.
    #define OCRAM_LEN 300K
#elif defined(CHIP_MX6DQ)
    // mx6dq has 256kB of OCRAM
    #define OCRAM_LEN 256K
#else
    #error Unknown chip!
#endif

// Maximum size of the signed image
#if !defined(MAX_IMAGE_SIZE)
#define MAX_IMAGE_SIZE  300K
//400K
#endif

// Maximum size of HAB CSF data
#if !defined(MAX_HAB_CSF_DATA_SIZE)
#define MAX_HAB_CSF_DATA_SIZE 0K
#endif

// Size of stacks section
#if !defined(STACK_SIZE)
#define STACK_SIZE 20K
#endif

// Size of the malloc heap, defaults to 128MB
#if !defined(HEAP_SIZE)
#define HEAP_SIZE 1K
#endif

// Size of the RAM vectors table at the top of OCRAM.
//
// The vectors in ROM at address 0 point to these RAM vectors.
#define RAM_VECTORS_SIZE 72

// Size of the L1 page table.
#define L1_PAGE_TABLE_SIZE 0x1000

INPUT (crtbegin.o crti.o crtend.o crtn.o)

MEMORY
{
    OCRAM (rwx) : ORIGIN = 0x40000000, LENGTH = MAX_IMAGE_SIZE
}

SECTIONS
{
/*
 * -- OCRAM --
 *
 * Nothing in OCRAM can be loaded at boot, because the boot image must be a contiguous
 * region of memory.
 */

    /* -- read-only sections -- */
    
    _start_image_add = ORIGIN(OCRAM);
    
    .ivt (ORIGIN(OCRAM)) :
    {
        . = . + 0x400;
        *(.ivt)
    } > OCRAM
    
    .boot_data :
    {
        __start_boot_data = .;
        *(.boot_data)
    } > OCRAM
    
    .init :
    {
        KEEP (*(.init))
    } > OCRAM =0
    
    .text : ALIGN(16)
    {
		KEEP(*(*.vectors))
        //*(.startup)
        *(.text .text.*)
    } > OCRAM

    .fini :
    {
        KEEP (*(.fini))
    } > OCRAM =0

    .ctors :
    {
        /* gcc uses crtbegin.o to find the start of
           the constructors, so we make sure it is
           first.  Because this is a wildcard, it
           doesn't matter if the user does not
           actually link against crtbegin.o; the
           linker won't look for a file to match a
           wildcard.  The wildcard also means that it
           doesn't matter which directory crtbegin.o
           is in.  */
        KEEP (*crtbegin.o(.ctors))
        KEEP (*crtbegin?.o(.ctors))
        /* We don't want to include the .ctor section from
           the crtend.o file until after the sorted ctors.
           The .ctor section from the crtend file contains the
           end of ctors marker and it must be last */
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
    } > OCRAM
    .dtors :
    {
        KEEP (*crtbegin.o(.dtors))
        KEEP (*crtbegin?.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
    } > OCRAM

    .rodata :
    {
        *(.rodata .rodata.*)
    } > OCRAM

    /* Unwind index. This section is related to C++ exceptions, and is required even
       though exceptions are disabled with -fno-exceptions. */
    PROVIDE_HIDDEN(__exidx_start = .);
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > OCRAM
    PROVIDE_HIDDEN(__exidx_end = .);

    /* -- read-write sections -- */
    
    .data :
    {
        *(.data .data.*)
        SORT(CONSTRUCTORS)
    } > OCRAM

    _image_size = . - _start_image_add;

    /* The .bss section comes after the hab data because it is not signed */
    .bss :
    {
        __bss_start__ = .;
        *(.shbss)
        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)
        *(.bss)
        __bss_end__ = .;
    } > OCRAM

    /* Region to contain exception handler stacks */
    .stacks (NOLOAD) :
    {
	    . = ALIGN(16);
        __stacks_start = .;
        . += STACK_SIZE;
        __stacks_end = .;
        top_of_stacks = .;
    } > OCRAM
    
    /* define range of the malloc heap */
    free_memory_start = ALIGN(32); /* malloc starts allocating from here */
    . += HEAP_SIZE;
    free_memory_end = .;

    /* MMU L1 page table */
    .l1_page_table (NOLOAD) :
    {
		. = ALIGN(0x4000);
        __l1_page_table_start = .;
        . += L1_PAGE_TABLE_SIZE;
    } > OCRAM

   
    /* RAM vector table comes at the end of OCRAM */
    .ram_vectors (NOLOAD) :
    {
		. = ALIGN(16);
        __ram_vectors_start = .;
        . += RAM_VECTORS_SIZE;
        __ram_vectors_end = .;
    } > OCRAM

    _end = .;
}
