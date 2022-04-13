OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(app_entry)

INPUT (crtend.o crtn.o)

MEMORY
{
    RAM (rwx) : ORIGIN = 0x3C700000, LENGTH = 1024K
}

SECTIONS
{
    . = ORIGIN(RAM);

    _text_start = ALIGN(32);

    .preamble :
    {
        KEEP (*(*.preamble))
    } > RAM

    .text : ALIGN(32)
    {
        *(.text .text.*)
    }> RAM

    .init :
    {
        KEEP (*(.init))
    } > RAM

    .fini :
    {
        KEEP (*(.fini))
    } > RAM

    .ARM.exidx : {
       __exidx_start = .;
       *(.ARM.exidx*)
       *(.gnu.linkonce.armexidix.*.*)
       __exidx_end = .;
    } > RAM

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

            PROVIDE(__ctors_start__ = .);
        KEEP (*(SORT(.init_array.*)))
            KEEP (*(.init_array))
            PROVIDE(__ctors_end__ = .);
    } > RAM

    .dtors :
    {
        KEEP (*crtbegin.o(.dtors))
            KEEP (*crtbegin?.o(.dtors))
            KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))
            KEEP (*(SORT(.dtors.*)))
            KEEP (*(.dtors))

            PROVIDE(__dtors_start__ = .);
        KEEP(*(SORT(.dtors.*)))
            KEEP(*(.dtors))
            PROVIDE(__dtors_end__ = .);
    } > RAM

    .rodata :
    {
        *(.rodata .rodata.*)
    } > RAM

    . = ALIGN(0x1000);
    _text_end = .;

    _data_start = ADDR(.data);
    .data : ALIGN(0x1000)
    {
        *(.data .data.*)
        SORT(CONSTRUCTORS)
    } > RAM

    .got :
    {
        *(.igot .igot.* .got .got.*)
    } > RAM

    .CliRegion :
    {
        _cli_region_begin = .;
        KEEP(*(CliRegion))
        _cli_region_end = .;
    } > RAM

    _data_end = .;

    _zero_start = .;
    .bss : ALIGN(32)
    {
        __bss_start = .;
        *(.shbss)
        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)
        *(.bss)
        __bss_end = .;
    } > RAM

    . = ALIGN(32);

    _heap_start = .;
    _heap_end = ORIGIN(RAM) + LENGTH(RAM);

    _zero_end = _heap_end;

    PROVIDE(_app_text_start = _text_start);
    PROVIDE(_app_text_end = _text_end);

    PROVIDE(_app_data_start = _data_start);
    PROVIDE(_app_data_end = _data_end);

    PROVIDE(_app_zero_start = _zero_start);
    PROVIDE(_app_zero_end = _zero_end);


    PROVIDE(_app_heap_start = _heap_start);
    PROVIDE(_app_heap_end = _heap_end);

    PROVIDE(_app_flash_start = _text_start);
    PROVIDE(_app_flash_end = _text_end);

    PROVIDE(_app_ram_start = _data_start);
    PROVIDE(_app_ram_end = ORIGIN(RAM) + LENGTH(RAM));

    PROVIDE(_app_data_ram_start = _data_start);
    PROVIDE(_app_data_ram_end = _data_end);

    PROVIDE(_app_bss_start = _zero_start);
    PROVIDE(_app_bss_end = _zero_end);

    PROVIDE(_app_data_flash_start = 0);
}
