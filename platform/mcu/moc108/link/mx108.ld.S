/*
 * Script for GNU linker.
 * Describes layout of sections, location of stack.
 *
 * In this case vectors are at location 0 (reset @ 0x08)
 *
 */

/* Include memory map */
INCLUDE memory.ld

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_vector_table);
_vector_table       = 0x196a0;
_begin_of_ram       = ORIGIN(ram);
_end_of_ram         = ORIGIN(ram) + LENGTH(ram);
_FIQ_STACK_SIZE_    = 0x7F0;
_IRQ_STACK_SIZE_    = 0xFF0;
_SVC_STACK_SIZE_    = 0x3F0;
_SYS_STACK_SIZE_    = 0x3F0;
_UNUSED_STACK_SIZE_ = 0x010;

SECTIONS
{
    /* vectors go to vectors region */
    .vectors :
    {
        KEEP(*(*.vectors))
    } > flash

    /* instructions go to the text region*/

    /* code, instructions.for example: i=i+1; */
    .text : ALIGN(0x8)
    {
        *(.text)
        *(.text.*)
        *(.stub)
        /* .gnu.warning sections are handled specially by elf32.em.  */
        *(.gnu.warning)
        *(.gnu.linkonce.t*)
        *(.glue_7t) *(.glue_7)
    } > flash

    /* read only data.for example: const int rom_data[3]={1,2,3}; */
    .rodata : ALIGN(8)
    {
        *(.rodata)
        *(.rodata.*)
        *(.gnu.linkonce.r*)
    } > flash

    .ARM.exidx :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        *(.gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > flash

    /* OVERLAY ram: normal ram begin */
    .data _begin_of_ram :
    {
        *(.data .data.*)
        *(.sdata)
        *(.gnu.linkonce.d*)
        SORT(CONSTRUCTORS)
        . = ALIGN(8);
    } > ram AT > flash

    /* Loader will copy data from _data_flash_begin to _ram_begin..ram_end */
    PROVIDE (_data_flash_begin = LOADADDR (.data));
    PROVIDE (_data_flash_end   = LOADADDR (.data) + SIZEOF (.data));
    PROVIDE (_data_ram_begin   = ADDR (.data));
    PROVIDE (_data_ram_end     = ADDR (.data) + SIZEOF (.data));

    /* uninitialized data section - global   int i; */
    .bss _data_ram_end : ALIGN(8)
    {
        _bss_start = .;
        *(.bss .bss.*)
        *(.scommon)
        *(.sbss)
       *(.dynbss)
       *(COMMON)
       /* Align here to ensure that the .bss section occupies space up to
          _end.  Align after .bss to ensure correct alignment even if the
          .bss section disappears because there are no input sections.  */
        . = ALIGN(8);
        _bss_end = .;
    } > ram
    /* This symbol defines end of code/data sections. Heap starts here. */
    PROVIDE(_empty_ram = ALIGN (8));
    PROVIDE(end = _empty_ram);

    PROVIDE(kernel_download_addr = 0x0013200);
    PROVIDE(framework_download_addr = 0xffffffff);
    PROVIDE(app_download_addr = 0xffffffff);

    /* _stack symbol defines initial stack bottom addres. Stack grows to lower addresses.
       Typically you set this to be top of your RAM. Note: code never checks, if stack
       grows into heap area!
    */
    PROVIDE(_stack_unused = _end_of_ram - _SYS_STACK_SIZE_ - _FIQ_STACK_SIZE_ - _IRQ_STACK_SIZE_ - _SVC_STACK_SIZE_ - _UNUSED_STACK_SIZE_);
    PROVIDE(_stack_svc = _end_of_ram - _SYS_STACK_SIZE_ - _FIQ_STACK_SIZE_ - _IRQ_STACK_SIZE_ - _SVC_STACK_SIZE_);
    PROVIDE(_stack_irq = _end_of_ram - _SYS_STACK_SIZE_ - _FIQ_STACK_SIZE_ - _IRQ_STACK_SIZE_);
    PROVIDE(_stack_fiq = _end_of_ram - _SYS_STACK_SIZE_ - _FIQ_STACK_SIZE_);
    PROVIDE(_stack_sys = _end_of_ram - _SYS_STACK_SIZE_);

    ASSERT ((_stack_unused > end), "Error: No room left for the stack")
    PROVIDE(heap_start = _empty_ram);
    PROVIDE(heap_end      = _stack_unused);
    PROVIDE(heap_len      = heap_end -  heap_start);

    ASSERT ((heap_len > 0x1E000 - 1 ), "Error: No room left for the heap") /*heap must bigger than 120k*/
    /* OVERLAY ram: normal ram end */
}

GROUP(
    libgcc.a
    libg.a
    libc.a
    libm.a
    libnosys.a
)
