OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
/* Internal Memory Map*/
MEMORY
{
	rom (rx)  : ORIGIN = 0x00000000, LENGTH = 0x01000000
	ram (rwx) : ORIGIN = 0x20000400, LENGTH = 0x0000FC00
}

_eram = 0x20000400 + 0x0000FC00;
SECTIONS
{
	.text :
	{
		KEEP(*(.isr_vector))
		KEEP(*(.stub_section))
		KEEP(*(.shell0.cmd))
		KEEP(*(.shell1.cmd))
		KEEP(*(.shell2.cmd))
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
		
		KEEP(*(.eh_fram e*))
	} > rom 
	
	.ARM.extab : 
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > rom 
	
	__exidx_start = .;
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > rom 
	__exidx_end = .;
	__etext = .;
	
	/* _sidata is used in coide startup code */
	_sidata = __etext;
	
	.data : AT (__etext)
	{
		__data_start__ = .;
		
		/* _sdata is used in coide startup code */
		_sdata = __data_start__;
		
		*(vtable)
		*(.data*)
		
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
		
		/* _edata is used in coide startup code */
		_edata = __data_end__;
	} > ram 
	
	.bss :
	{
		. = ALIGN(4);
		__bss_start__ = .;
		_sbss = __bss_start__;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end__ = .;
		_ebss = __bss_end__;		
				
	} > ram 
	
}
