/*
 * Script for GNU linker.
 * Describes layout of sections, location of stack.
 *
 * In this case vectors are at location 0 (reset @ 0x08)
 *
 */

/* Include memory map */
INCLUDE memory_framework.ld

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)

SECTIONS
{
#ifdef MBEDTLS_IN_KERNEL
	. = 0x007D400;
#else
    . = 0x0070400;
#endif

    .frameworkinfo :
    {
        KEEP(*(*.framework_info))
    } > flash

    . = ALIGN(0x8);
    .syscall_ftbl :
    {
        KEEP(*(.syscall_ftbl))
    } > flash

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
	    *(.ARM.exidx*)
	    *(.gnu.linkonce.armexidx.*)
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
	PROVIDE(_framework_data_flash_begin = LOADADDR(.data));
	PROVIDE(_framework_data_ram_begin = ADDR(.data));
	PROVIDE(_framework_data_ram_end = .);

/* uninitialized data section - global   int i; */
	.bss ALIGN(8):
	{
		PROVIDE(_framework_bss_start = .);
		*(.bss .bss.*)
		*(.scommon)
		*(.sbss)
	   *(.dynbss)
	   *(COMMON)
	   /* Align here to ensure that the .bss section occupies space up to
	      _end.  Align after .bss to ensure correct alignment even if the
	      .bss section disappears because there are no input sections.  */
	   . = ALIGN(32 / 8);
		PROVIDE(_framework_bss_end = .);
	} > ram						/* in RAM */

    . = ALIGN (8);

    /* This symbol defines end of code/data sections. Heap starts here. */
    PROVIDE(end       = .);
    PROVIDE(_framework_heap_start        = .);

    ASSERT ((0x00410020 > end), "Error: No room left for the framework stack")

    PROVIDE(_framework_heap_end      = 0x00410020);
    PROVIDE(_framework_heap_len      = _framework_heap_end -  _framework_heap_start);

#ifdef MBEDTLS_IN_KERNEL
    PROVIDE(app_info_addr = 0x00A3000);
    PROVIDE(kernel_download_addr = 0x0013200);
    PROVIDE(framework_download_addr = 0x0085140);
    PROVIDE(app_download_addr = 0x00AD300);
    PROVIDE(ota_download_addr = 0x00BA340);
#else
    PROVIDE(app_info_addr = 0x00AB800);
    PROVIDE(kernel_download_addr = 0x0013200);
    PROVIDE(framework_download_addr = 0x0077440);
    PROVIDE(app_download_addr = 0X00B6380);
    PROVIDE(ota_download_addr = 0x00c4080);
#endif

}
 GROUP(
   libgcc.a
   libg.a
   libc.a
   libm.a
   libnosys.a
 )
