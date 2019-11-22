/********************************************************  
memory map 
  Start Address	Size	CM4 view Devices	
	0x0400_0000	32KB	CM4 TCM / cache	
	0x0400_8000	64KB	CM4 TCM	
	0x0401_8000	32KB	Reserved	
	0x0402_0000	896KB	Reserved	
	0x0410_0000	1MB	Boot ROM CM4
	0x0420_0000	384KB	SYSRAM
	0x0430_0000	8KB	Retention SRAM
	0x0440_0000	1MB	WIFI ROM
	0x0800_0000	128MB	SFC0
 	0x1000_0000	128MB	SW virtual memory management
	0x1800_0000	128MB	SW virtual memory management
*********************************************************/
 
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
 
/*7682  Memory Spaces Definition: 1M FLASH, 384K SYSRAM */
MEMORY 
{
    ROM_HEAD_1(rx)        : ORIGIN = 0x08000000, LENGTH = 4K
    ROM_HEAD_2(rx)        : ORIGIN = 0x08001000, LENGTH = 4K
    ROM_BL(rx)            : ORIGIN = 0x08002000, LENGTH = 64K
    ROM_RTOS(rx)          : ORIGIN = 0x08012000, LENGTH = 884K
    ROM_NVDM_RESERVED(rx) : ORIGIN = 0x080EF000, LENGTH = 44K
    ROM_KV(rwx)           : ORIGIN = 0x080FA000, LENGTH = 20K 
    WIFI_TXPOWER_DATA(rx) : ORIGIN = 0x080FF000, LENGTH = 4K
    TCM (rwx)             : ORIGIN = 0x04008000, LENGTH = 64K
    SYSRAM (rwx)          : ORIGIN = 0x04200000, LENGTH = 359K
    WIFI_DATA(rwx)        : ORIGIN = 0x04259C00, LENGTH = 25K 
    VSYSRAM (rwx)         : ORIGIN = 0x14200000, LENGTH = 359K      
}
 
 /* Highest address of the stack */
_stack_end = ORIGIN(TCM) + LENGTH(TCM);    /* end of TCM */

/* Generate a link error if stack don't fit into TCM */
_stack_size = 0x1000; /* required amount of stack 4KB*/
 
/* stack start */
_stack_start = _stack_end - _stack_size;
 
_heap_size = 0x2F800;

/* Entry Point */
ENTRY(Reset_Handler)

SECTIONS
{
    . = ORIGIN(ROM_RTOS);
    .text :
    {
        _text_start = .;
        Image$$TEXT$$Base = .;
        
        KEEP(*(.reset_handler))
        KEEP(*(.init))
        KEEP(*(.fini))
        
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        
         . = ALIGN(4);
      __boson_data_lmastart = .;
        KEEP(*(.boson_data))
         . = ALIGN(4);
        Image$$TEXT$$Limit = .;
        _text_end = .;
        
    } > ROM_RTOS  AT> ROM_RTOS
   
   

     . = ALIGN(32);
      _sysram_code_load = LOADADDR(.sysram_text);
    .sysram_text :
    {
        Image$$CACHED_SYSRAM_TEXT$$Base = .;
        _sysram_code_start = .;
        

        *(.sysram_code)
        *(.sysram_rodata)
        
        . = ALIGN(4);
        Image$$CACHED_SYSRAM_TEXT$$Limit = .;
        _sysram_code_end = .;
    } > VSYSRAM   AT> ROM_RTOS
	
     . = ALIGN(4);
     _cached_sysram_data_load = LOADADDR(.data);
	 
	
     .data  :
    {       
        _cached_sysram_data_start = .;
        Image$$CACHED_SYSRAM_DATA$$RW$$Base = .;

        *(.data)
        *(.data*)
        
        . = ALIGN(4);
        /* preinit data */
        PROVIDE (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE (__init_array_end = .);
        
        . = ALIGN(4);
        /* finit data */
        PROVIDE (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE (__fini_array_end = .);

        _cached_sysram_data_end = .; 
       Image$$CACHED_SYSRAM_DATA$$RW$$Limit = .;
        
    } > VSYSRAM  AT> ROM_RTOS

    
    . = ALIGN(4);
    .bss (NOLOAD): 
    {
        _cached_sysram_bss_start = .;
        Image$$CACHED_SYSRAM_DATA$$ZI$$Base = .;
        
        *(.bss)
        *(.bss*)
        *(COMMON)
        
        . = ALIGN(4);
        _cached_sysram_bss_end = .;
        Image$$CACHED_SYSRAM_DATA$$ZI$$Limit = .;
        
    } > VSYSRAM  AT> ROM_RTOS  
	
	. = ALIGN(4);
    .heap :
	{
		heap_start = .;
		. += _heap_size;
	}> VSYSRAM  AT> ROM_RTOS
	
     . = ALIGN(32);
    _noncached_sysram_rw_load = LOADADDR(.noncached_sysram_data);
     . = . - ORIGIN(VSYSRAM) + ORIGIN(SYSRAM);
	
    .noncached_sysram_data . :
    {

        Image$$NONCACHED_SYSRAM_DATA$$Base = .;
        _noncached_sysram_rw_start = .;
        *(.noncached_sysram_rwdata)
        
        
        Image$$NONCACHED_SYSRAM_DATA$$Limit = .;
        _noncached_sysram_rw_end = .; 
    } > SYSRAM  AT> ROM_RTOS
    
     
    
    . = ALIGN(4);
    .noncached_sysram_bss . (NOLOAD) :
    {                
        Image$$NONCACHED_SYSRAM_ZI$$Base = .;
        _noncached_sysram_zi_start = .;
        *(.noncached_sysram_zidata)
        
        
        Image$$NONCACHED_SYSRAM_ZI$$Limit = .;
        _noncached_sysram_zi_end = .;
    } > SYSRAM  AT> ROM_RTOS
     
		. = ALIGN(8);

	
    ASSERT(_noncached_sysram_zi_end  <= (ORIGIN(SYSRAM) + LENGTH(SYSRAM)),"SYSRAM expired")
  
    . = ALIGN(4);
    _tcm_text_load = LOADADDR(.tcm);

    .tcm :
    {
        
        _tcm_text_start = .;
        Image$$TCM$$RO$$Base = .;
        Image$$VECTOR$$TABLE$$Base = .;
         KEEP(*(.isr_vector))
        *(.tcm_code)
        *(.tcm_rodata)
        Image$$TCM$$RO$$Limit = .;
        Image$$TCM$$RW$$Base = .;
        
        *(.tcm_rwdata)
        . = ALIGN(4);
        Image$$TCM$$RW$$Limit = .;
        _tcm_text_end = .;
    }> TCM  AT> ROM_RTOS

    . = ALIGN(4);
    .tcm_bss (NOLOAD) :
    {
        _tcm_zi_start = .;
        Image$$TCM$$ZI$$Base = .;
        
        *(.tcm_zidata)
        
        _tcm_zi_end = .;
        Image$$TCM$$ZI$$Limit = .;
        
        
    }> TCM  
    
    /* use to check if the stack exceeds the total TCM size*/
    .stack :
    {
        . = ALIGN(4);
        
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        . = . + _stack_size;
        . = ALIGN(4);
        
    } > TCM
  
    Image$$STACK$$ZI$$Base = _stack_end - _stack_size;
    Image$$STACK$$ZI$$Limit = _stack_end;
    

    /* provide nvdm start and length for NVDM management */
    Image$$NVDM$$ZI$$Base = ORIGIN(ROM_NVDM_RESERVED);
    Image$$NVDM$$ZI$$Length = LENGTH(ROM_NVDM_RESERVED);

}
