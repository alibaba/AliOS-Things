OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_vector_table);

SECTIONS
{
    . = 0x80000000;

    __text_start = .;
    .text :
    {
        KEEP(*(*.vectors))
        /* make the ISRs close to vectors may be more cache-friendly */
        *(.text.isr)
        *(.text)
        *(.text.*)
    } = 0

    . = ALIGN(4);
    .ctors :
    {
        PROVIDE(__ctors_start__ = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        PROVIDE(__ctors_end__ = .);
    }

    .dtors :
    {
        PROVIDE(__dtors_start__ = .);
        KEEP(*(SORT(.dtors.*)))
        KEEP(*(.dtors))
        PROVIDE(__dtors_end__ = .);
    }

    . = ALIGN(4);
    
    // as a standalone section if you use ARM target.
    
    /* The .ARM.exidx section is used for C++ exception handling. */
    /* .ARM.exidx is sorted, so has to go in its own output section.  */
    __exidx_start = .;
    ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)

        /* This is used by the startup in order to initialize the .data secion */
        _sidata = .;
    }
    __exidx_end = .;

    __text_end = .;

    __rodata_start = .;
    .rodata   : { *(.rodata) *(.rodata.*) }
    __rodata_end = .;

    __data_start = .;
    . = ALIGN(4);
    .data :
    {
        KEEP(*(.resource_table))
        *(.data)
        *(.data.*)
    }
    __data_end = .;

    . = ALIGN(4);
    __bss_start = __data_end;
    .bss       :
    {
    *(.bss)
    *(.bss.*)
    *(COMMON)
    . = ALIGN(4);
    }
    . = ALIGN(8);
    __bss_end = .;
    PROVIDE(heap_start      = .);
	PROVIDE(heap_len      = 0x82000000 - heap_start);

    /* Stabs debugging sections.  */
    .stab 0 : { *(.stab) }
    .stabstr 0 : { *(.stabstr) }
    .stab.excl 0 : { *(.stab.excl) }
    .stab.exclstr 0 : { *(.stab.exclstr) }
    .stab.index 0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment 0 : { *(.comment) }

    _end = .;
}
