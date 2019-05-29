/* Linker script for ARM M-profile Simulator
 *
 * Version: Sourcery G++ Lite 2010q1-188
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
ENTRY(Reset)
SEARCH_DIR(.)
GROUP(-lgcc -lc -lnosys)




MEMORY
{
    rom (rx) : ORIGIN = 0x0, LENGTH = 131072
    ram (rwx) : ORIGIN = 0x20000000, LENGTH = 16384
}


CY_APPL_ORIGIN                  = 0;
CY_FLASH_ROW_SIZE               = 256;
CY_APPL_NUM                     = 1;
CY_APPL_MAX                     = 1;
CY_METADATA_SIZE                = 64;
CY_APPL_LOADABLE                = 0;
CY_CHECKSUM_EXCLUDE_SIZE        = ALIGN(0, CY_FLASH_ROW_SIZE);
CY_APP_FOR_STACK_AND_COPIER     = 0;


/* These force the linker to search for particular symbols from
 * the start of the link process and thus ensure the user's
 * overrides are picked up
 */
EXTERN(Reset)

/* Bring in the interrupt routines & vector */
EXTERN(main)

/* Bring in the romvector */
EXTERN(RomVectors)

/* Bring in the ramvector */
EXTERN(CyRamVectors)

/* Bring in the meta data */
EXTERN(cy_meta_loader cy_bootloader cy_meta_loadable cy_meta_bootloader)
EXTERN(cy_meta_flashprotect cy_metadata cy_meta_chipprotect)
EXTERN(cy_heap)

/* Provide fall-back values */
PROVIDE(__cy_heap_start = _end);
PROVIDE(__cy_region_num = (__cy_regions_end - __cy_regions) / 16);

/* Set stack top to end of RAM, and stack limit move down by
 * size of .stack section.
 */
PROVIDE(__cy_stack = ORIGIN(ram) + LENGTH(ram));

PROVIDE(__cy_heap_end = __cy_stack - 0x0080);


SECTIONS
{
    /* The bootloader location */
    .cybootloader 0x0 : { KEEP(*(.cybootloader)) } >rom

    /* Calculate where the loadables should start */
    appl1_start = CY_APPL_ORIGIN ? CY_APPL_ORIGIN : ALIGN(CY_FLASH_ROW_SIZE);
    appl2_start = appl1_start + ALIGN((LENGTH(rom) - appl1_start - 2 * CY_FLASH_ROW_SIZE) / 2, CY_FLASH_ROW_SIZE);
    appl_start = (CY_APPL_NUM == 1) ? appl1_start : appl2_start;


    cy_project_type_bootloader = (appl_start == 0) ? 1 : 0;
    cy_project_type_app_for_stack_and_copier = (CY_APP_FOR_STACK_AND_COPIER == 1) ? 1 : 0;


    .text appl_start :
    {
      CREATE_OBJECT_SYMBOLS
      PROVIDE(__cy_interrupt_vector = RomVectors);

      KEEP(*(.romvectors))

      /* Make sure we pulled in an interrupt vector.  */
      ASSERT (. != __cy_interrupt_vector, "No interrupt vector");

      ASSERT (CY_APPL_ORIGIN ? (SIZEOF(.cybootloader) <= CY_APPL_ORIGIN) : 1, "Wrong image location");

      PROVIDE(__cy_reset = Reset);

      *(.text.Reset)

      /* Make sure we pulled in some reset code.  */
      ASSERT (. != __cy_reset, "No reset code");

      *(.psocinit)

      /* The first 0x100 Flash bytes become unavailable right after remapping of the vector table to RAM. */
      . = MAX(., 0x100);

      *(.text .text.* .gnu.linkonce.t.*)
      *(.plt)
      *(.gnu.warning)
      *(.glue_7t) *(.glue_7) *(.vfp11_veneer)

      KEEP(*(.bootloader)) /* necessary for bootloader's, but doesn't impact non-bootloaders */

      *(.ARM.extab* .gnu.linkonce.armextab.*)
      *(.gcc_except_table)
    } >rom

    .eh_frame_hdr : ALIGN (4)
    {
      KEEP (*(.eh_frame_hdr))
    } >rom

    .eh_frame : ALIGN (4)
    {
      KEEP (*(.eh_frame))
    } >rom

    /* .ARM.exidx is sorted, so has to go in its own output section.  */
    PROVIDE_HIDDEN (__exidx_start = .);

    .ARM.exidx :
    {
      *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } >rom

    __exidx_end = .;


    .rodata : ALIGN (4)
    {
      *(.rodata .rodata.* .gnu.linkonce.r.*)

      . = ALIGN(4);
      KEEP(*(.init))

      . = ALIGN(4);
      __preinit_array_start = .;
      KEEP (*(.preinit_array))
      __preinit_array_end = .;

      . = ALIGN(4);
      __init_array_start = .;
      KEEP (*(SORT(.init_array.*)))
      KEEP (*(.init_array))
      __init_array_end = .;

      . = ALIGN(4);
      KEEP(*(.fini))

      . = ALIGN(4);
      __fini_array_start = .;
      KEEP (*(.fini_array))
      KEEP (*(SORT(.fini_array.*)))
      __fini_array_end = .;

      . = ALIGN(0x4);
      KEEP (*crtbegin.o(.ctors))
      KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
      KEEP (*(SORT(.ctors.*)))
      KEEP (*crtend.o(.ctors))

      . = ALIGN(0x4);
      KEEP (*crtbegin.o(.dtors))
      KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
      KEEP (*(SORT(.dtors.*)))
      KEEP (*crtend.o(.dtors))

      . = ALIGN(4);
      __cy_regions = .;
      LONG (__cy_region_init_ram)
      LONG (__cy_region_start_data)
      LONG (__cy_region_init_size_ram)
      LONG (__cy_region_zero_size_ram)
      __cy_regions_end = .;

      . = ALIGN (8);
      _etext = .;
    } >rom


    /***************************************************************************
    * Checksum Exclude Section for non-bootloadable projects. See below.
    ***************************************************************************/
    .cy_checksum_exclude : { KEEP(*(.cy_checksum_exclude)) } >rom


    .ramvectors (NOLOAD) : ALIGN(8)
    {
      __cy_region_start_ram = .;
      KEEP(*(.ramvectors))
    }

    

    .noinit (NOLOAD) : ALIGN(8)
    {
      KEEP(*(.noinit))
    }

    .data : ALIGN(8)
    {
      __cy_region_start_data = .;

      KEEP(*(.jcr))
      *(.got.plt) *(.got)
      *(.shdata)
      *(.data .data.* .gnu.linkonce.d.*)
      . = ALIGN (8);
      *(.ram)
      _edata = .;
    } >ram AT>rom

    .bss : ALIGN(8)
    {
      PROVIDE(__bss_start__ = .);
      *(.shbss)
      *(.bss .bss.* .gnu.linkonce.b.*)
      *(COMMON)
      . = ALIGN (8);
      *(.ram.b)
      _end = .;
      __end = .;
    } >ram AT>rom



    PROVIDE(end = .);
    PROVIDE(__bss_end__ = .);

    __cy_region_init_ram = LOADADDR (.data);
    __cy_region_init_size_ram = _edata - ADDR (.data);
    __cy_region_zero_size_ram = _end - _edata;

    /* The .stack and .heap sections don't contain any symbols.
     * They are only used for linker to calculate RAM utilization.
     */
    .heap (NOLOAD) :
    {
      . = _end;
      . += 0x1100;
      __cy_heap_limit = .;
    } >ram

    .stack (__cy_stack - 0x0080) (NOLOAD) :
    {
      __cy_stack_limit = .;
      . += 0x0080;
    } >ram

    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__cy_stack_limit >= __cy_heap_limit, "region RAM overflowed with stack")


    /***************************************************************************
     * Checksum Exclude Section
     ***************************************************************************
     *
     * For the normal and bootloader projects this section is placed at any
     * place. For the Bootloadable applications, it is placed at the specific
     * address.
     *
     * Case # 1. Bootloadable application
     *
     *  _______________________________
     * | Metadata (BTLDBL)             |
     * |-------------------------------|
     * | Checksum Exclude (BTLDBL)     |
     * |-------------------------------|
     * |                               |
     * |                               |
     * |                               |
     * |-------------------------------|
     * |                               |
     * |                               |
     * |                               |
     * | BTLDBL                        |
     * |                               |
     * |                               |
     * |                               |
     * |-------------------------------|
     * |                               |
     * | BTLDR                         |
     * |_______________________________|
     *
     *
     *  Case # 2. Bootloadable application for Dual-Application Bootloader
     *
     *  _______________________________
     * | Metadata (BTLDBL # 1)         |
     * |-------------------------------|
     * | Metadata (BTLDBL # 2)         |
     * |-------------------------------|
     * | Checksum Exclude (BTLDBL # 2) |
     * |-------------------------------|
     * |                               |
     * |                               |
     * |                               |
     * |-------------------------------|
     * |                               |
     * | BTLDBL # 2                    |
     * |_______________________________|____BTLDBL # 2 Start address___
     * | Checksum Exclude (BTLDBL # 1) |
     * |-------------------------------|
     * |                               |
     * |                               |
     * |                               |
     * |-------------------------------|
     * |                               |
     * | BTLDBL # 1                    |
     * |                               |
     * |-------------------------------|
     * | BTLDR                         |
     * |_______________________________|
     *
     *
     *  Case # 3. OTA updatable stack
     *
     *  _______________________________
     * | Metadata (BTLDBL # 1)         |
     * |-------------------------------|
     * | Metadata (BTLDBL # 2)         |
     * |-------------------------------|
     * | Checksum Exclude (BTLDBL # 2) |
     * |-------------------------------|
     * |                               |
     * |                               |
     * |                               |
     * |                               |
     * |-------------------------------|
     * |_______________________________|____Temporary location for BTLDBL # 1 update(Former BTLDBL # 2 start)___
     * |                               |
     * | BTLDBL # 2                    |
     * |                               |
     * |-------------------------------|
     * | Checksum Exclude (BTLDBL # 1) |
     * |-------------------------------|
     * |                               |
     * | BTLDBL # 1                    |
     * |                               |
     * |-------------------------------|
     * | BTLDR                         |
     * |_______________________________|
     */
    


    /* Bootloadable applications only: verify that size of the data in the section is within the specified limit. */
    cy_checksum_exclude_size = (CY_APPL_LOADABLE == 1) ? SIZEOF(.cy_checksum_exclude) : 0;
    ASSERT(cy_checksum_exclude_size <= CY_CHECKSUM_EXCLUDE_SIZE, "CY_BOOT: Section .cy_checksum_exclude size exceedes specified limit.")


    /***************************************************************************
     * Bootloader Metadata Section
     ***************************************************************************
     *
     * Case # 1. Bootloader project
     *
     *  _______________________________
     * |    BTLDR Metadata             |
     * |-------------------------------|
     * |                               |
     * |                               |
     * |                               |
     * |                               |
     * |-------------------------------|
     * |                               |
     * | Bootloader  (BTLDR)           |
     * |_______________________________|
     *
     *
     * Case # 2. Code sharing
     *
     *  _______________________________
     * |    SP/L Metadata              |  CY_APPL_METADATA_SLOT_NUM == 0
     * |-------------------------------|
     * |    App for SP+L Metadata      |  CY_APPL_METADATA_SLOT_NUM == 1
     * |-------------------------------|
     * |                               |
     * |                               |
     * |                               |
     * |-------------------------------|
     * |                               |
     * |   App for SP+L                |  ((CYDEV_IS_IMPORTING_CODE == 1) && (CY_FIRST_AVAILABLE_META_ROW == 2))
     * |                               |
     * |-------------------------------|
     * |                               |
     * |   Stack Project (SP)          |  (CYDEV_IS_EXPORTING_CODE == 1)
     * |                               |
     * |-------------------------------|
     * |                               |
     * |   Launcher  (L)               |
     * |_______________________________|
     *
     * Notes:
     * - App for SP+L start just after the SP
     * - SP treated as a single bootloadable application
     * - App for SP+L treats SP+L as a bootloader
     */

    /* For the bootloader project, place bootloader metadata at the last flash row, otherwise place beyond map */
    cyloadermeta_start = (cy_project_type_bootloader || cy_project_type_app_for_stack_and_copier) ?
        (LENGTH(rom) - CY_METADATA_SIZE) : 0xF0000000;
    .cyloadermeta (cyloadermeta_start) :
    {
      KEEP(*(.cyloadermeta))
    } : NONE


    cyloadablemeta_start = (cy_project_type_app_for_stack_and_copier) ?
        (LENGTH(rom) - CY_FLASH_ROW_SIZE - CY_METADATA_SIZE) : (LENGTH(rom) - CY_FLASH_ROW_SIZE * (CY_APPL_NUM - 1) - CY_METADATA_SIZE);
    .cyloadablemeta (cyloadablemeta_start) :
    {
      KEEP(*(.cyloadablemeta))
    } >rom

    .cyflashprotect 0x90400000 : { KEEP(*(.cyflashprotect)) } :NONE
    .cymeta         0x90500000 : { KEEP(*(.cymeta)) } :NONE
    .cychipprotect  0x90600000 : { KEEP(*(.cychipprotect)) } :NONE

    .stab 0 (NOLOAD) : { *(.stab) }
    .stabstr 0 (NOLOAD) : { *(.stabstr) }
    /* DWARF debug sections.
     * Symbols in the DWARF debugging sections are relative to the beginning
     * of the section so we begin them at 0.
     */
    /* DWARF 1 */
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    /* GNU DWARF 1 extensions */
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2 */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* DWARF 2.1 */
    .debug_ranges   0 : { *(.debug_ranges) }
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }

    .note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }
    .ARM.attributes 0 : { KEEP (*(.ARM.attributes)) }
    /DISCARD/ : { *(.note.GNU-stack) }
}

