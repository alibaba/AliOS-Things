/*
 * Script for GNU linker.
 * Describes layout of sections, location of stack.
 *
 * In this case vectors are at location 0 (reset @ 0x08)
 *
 */

/* Include memory map */
INCLUDE board/bk7231sdevkitc/bk7231sdevkitc.ld

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_vector_start);
_vector_start = 0x00010000;

SECTIONS
{
/* vectors go to vectors region */
	.vectors 0x00010000 :
	{
      KEEP(*(*.vectors))
	} > flash

/* instructions go to the text region*/

	. = ALIGN(0x8);
/* code, instructions.for example: i=i+1; */
	.text :
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
	.rodata ALIGN(8) :
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

/* globals.for example: int ram_data[3]={4,5,6}; */		/* VMA in RAM, but keep LMA in flash */
	. = ALIGN(0x8);
	_begin_data = .;
	.data : AT ( _begin_data )
	{
		*(.data .data.*)
		*(.sdata)
		*(.gnu.linkonce.d*)
        SORT(CONSTRUCTORS)
	} >ram

	/* Loader will copy data from _flash_begin to _ram_begin..ram_end */
	_data_flash_begin = LOADADDR(.data);
	_data_ram_begin = ADDR(.data);
	_data_ram_end = .;

/* uninitialized data section - global   int i; */
	.bss ALIGN(8):
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
	   . = ALIGN(32 / 8);
		_bss_end = .;
	} > ram						/* in RAM */

	. = ALIGN (8);
	_empty_ram = .;

/* This symbol defines end of code/data sections. Heap starts here. */
	PROVIDE(end    	  = .);
	PROVIDE(heap_start    	  = .);

    PROVIDE(kernel_download_addr = 0x0008800);
    PROVIDE(framework_download_addr = 0xffffffff);
    PROVIDE(app_download_addr = 0xffffffff);

/* _stack symbol defines initial stack bottom addres. Stack grows to lower addresses.
   Typically you set this to be top of your RAM. Note: code never checks, if stack
   grows into heap area!
*/
	PROVIDE(_stack_unused      = 0x440000 - 0x3F0 - 0x7F0 - 0xFF0 - 0x3F0 - 0x10); /* 0x10*/
	PROVIDE(_stack_svc         = 0x440000 - 0x3F0 - 0x7F0 - 0xFF0 - 0x3F0); /* 0x3F0*/
	PROVIDE(_stack_irq         = 0x440000 - 0x3F0 - 0x7F0 - 0xFF0); /* 0xFF0*/
	PROVIDE(_stack_fiq         = 0x440000 - 0x3F0 - 0x7F0); /* 0x7F0*/
	PROVIDE(_stack_sys         = 0x440000 - 0x3F0); /* 0x3F0*/

	ASSERT ((_stack_unused > end), "Error: No room left for the stack")

	PROVIDE(heap_end      = _stack_unused);
	PROVIDE(heap_len      = heap_end -  heap_start);

	ASSERT ((heap_len > 0x10000 - 1 ), "Error: No room left for the heap") /*heap must bigger than 120k*/
}
 GROUP(
   libgcc.a
   libg.a
   libc.a
   libm.a
   libnosys.a
 )
