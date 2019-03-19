OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(app_entry)
INPUT (crtbegin.o crti.o crtend.o crtn.o)
MEMORY
{
    OCRAM (rwx) : ORIGIN = 0x90000000, LENGTH = 2000K
}
SECTIONS
{
    . = ORIGIN(OCRAM);
    _text_start = ALIGN(32);
    .preamble :
    {
        KEEP (*(*.preamble))
    } > OCRAM
    .text : ALIGN(32)
    {
        *(.text .text.*)
    }> OCRAM
    .init :
    {
        KEEP (*(.init))
    } > OCRAM
    .fini :
    {
        KEEP (*(.fini))
    } > OCRAM
    .ctors :
    {
        KEEP (*crtbegin.o(.ctors))
            KEEP (*crtbegin?.o(.ctors))
            KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))
            KEEP (*(SORT(.ctors.*)))
            KEEP (*(.ctors))
            PROVIDE(__ctors_start__ = .);
        KEEP (*(SORT(.init_array.*)))
            KEEP (*(.init_array))
            PROVIDE(__ctors_end__ = .);
    } > OCRAM
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
    } > OCRAM
    .rodata :
    {
        *(.rodata .rodata.*)
    } > OCRAM
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > OCRAM
    _text_end = .;
    _data_start = ADDR(.data);
    .data :
    {
        *(.data .data.*)
        SORT(CONSTRUCTORS)
    } > OCRAM
    _data_end = .;
    _zero_start = .;
    .bss :
    {
        __bss_start = .;
        *(.shbss)
        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)
        *(.bss)
        __bss_end = .;
    } > OCRAM
    _zero_end = .;
    PROVIDE(_app_text_start = _text_start);
    PROVIDE(_app_text_end = _text_end);
    PROVIDE(_app_data_start = _data_start);
    PROVIDE(_app_data_end = _data_end);
    PROVIDE(_app_zero_start = _zero_start);
    PROVIDE(_app_zero_end = _zero_end);
}
