/* This linker script generated from xt-genldscripts.tpp for LSP . */
/* Linker Script for ld -N */

PHDRS
{
  dport0_0_phdr PT_LOAD;
  dram0_0_phdr PT_LOAD;
  dram0_0_bss_phdr PT_LOAD;
  iram1_0_phdr PT_LOAD;
  irom0_0_phdr PT_LOAD;
}

/*  Default entry point:  */
ENTRY(call_user_start)
EXTERN(_DebugExceptionVector)
EXTERN(_DoubleExceptionVector)
EXTERN(_KernelExceptionVector)
EXTERN(_NMIExceptionVector)
EXTERN(_UserExceptionVector)
PROVIDE(_memmap_vecbase_reset = 0x40000000);
/* Various memory-map dependent cache attribute settings: */
_memmap_cacheattr_wb_base = 0x00000110;
_memmap_cacheattr_wt_base = 0x00000110;
_memmap_cacheattr_bp_base = 0x00000220;
_memmap_cacheattr_unused_mask = 0xFFFFF00F;
_memmap_cacheattr_wb_trapnull = 0x2222211F;
_memmap_cacheattr_wba_trapnull = 0x2222211F;
_memmap_cacheattr_wbna_trapnull = 0x2222211F;
_memmap_cacheattr_wt_trapnull = 0x2222211F;
_memmap_cacheattr_bp_trapnull = 0x2222222F;
_memmap_cacheattr_wb_strict = 0xFFFFF11F;
_memmap_cacheattr_wt_strict = 0xFFFFF11F;
_memmap_cacheattr_bp_strict = 0xFFFFF22F;
_memmap_cacheattr_wb_allvalid = 0x22222112;
_memmap_cacheattr_wt_allvalid = 0x22222112;
_memmap_cacheattr_bp_allvalid = 0x22222222;
PROVIDE(_memmap_cacheattr_reset = _memmap_cacheattr_wb_trapnull);

_begin_of_ram   = ORIGIN(dram0_0_seg);
_end_of_ram     = ORIGIN(dram0_0_seg) + LENGTH(dram0_0_seg);
_SYS_STACK_SIZE_ = 0x800;

SECTIONS
{
  .dport0.rodata : ALIGN(4)
  {
    _dport0_rodata_start = ABSOLUTE(.);
    *(.dport0.rodata)
    *(.dport.rodata)
    _dport0_rodata_end = ABSOLUTE(.);
  } >dport0_0_seg :dport0_0_phdr

  .dport0.literal : ALIGN(4)
  {
    _dport0_literal_start = ABSOLUTE(.);
    *(.dport0.literal)
    *(.dport.literal)
    _dport0_literal_end = ABSOLUTE(.);
  } >dport0_0_seg :dport0_0_phdr

  .dport0.data : ALIGN(4)
  {
    _dport0_data_start = ABSOLUTE(.);
    *(.dport0.data)
    *(.dport.data)
    _dport0_data_end = ABSOLUTE(.);
  } >dport0_0_seg :dport0_0_phdr

  .text : ALIGN(4)
  {
    _stext = .;
    _text_start = ABSOLUTE(.);
    *(.UserEnter.text)
    . = ALIGN(16);
    *(.DebugExceptionVector.text)
    . = ALIGN(16);
    *(.NMIExceptionVector.text)
    . = ALIGN(16);
    *(.KernelExceptionVector.text)
    LONG(0)
    LONG(0)
    LONG(0)
    LONG(0)
    . = ALIGN(16);
    *(.UserExceptionVector.text)
    LONG(0)
    LONG(0)
    LONG(0)
    LONG(0)
    . = ALIGN(16);
    *(.DoubleExceptionVector.text)
    LONG(0)
    LONG(0)
    LONG(0)
    LONG(0)
    . = ALIGN (16);
    *(.entry.text)
    *(.init.literal)
    *(.init)
    *mcu_esp8266.a:entry.o(.literal .text .literal.* .text.*)
	*board_esp8266.a:startup.o(.literal .text .literal.* .text.*)	
    *libmain.a:spi_flash.o(.literal .text .literal.* .text.*)
    *arch_xtensa_lx106.a:xtensa_vectors.o(.literal .text .literal.* .text.*)
    *arch_xtensa_lx106.a:port.o(.literal .text .literal.* .text.*)

    *rhino.a:(.literal.ready_list_add_head .text.ready_list_add_head)
    *rhino.a:(.literal.ready_list_add_tail .text.ready_list_add_tail)
    *rhino.a:(.literal.ready_list_add .text.ready_list_add)
    *rhino.a:(.literal.ringbuf_push .text.ringbuf_push)
    *rhino.a:(.literal.pend_task_wakeup .text.pend_task_wakeup)
    *rhino.a:(.literal.buf_queue_send .text.buf_queue_send)
    *rhino.a:(.literal.krhino_buf_queue_send .text.krhino_buf_queue_send)
    *rhino.a:(.literal.k_err_proc .text.k_err_proc)
    *main.o:(.literal.user_init .text.user_init)
	*main.o:(.literal.maintask .text.maintask)

    *(.literal .text)
    _text_end = ABSOLUTE(.);
    _etext = .;
    . = ALIGN (1024);
    _iram_heap_start = ABSOLUTE(.);
  } >iram1_0_seg :iram1_0_phdr

  .irom0.text : ALIGN(4)
  {
    _irom0_text_start = ABSOLUTE(.);
	*(.rodata)
    *(.rodata.*)
    *(.irom0.literal .irom.literal .irom.text.literal .irom0.text .irom.text)
    *(.literal.* .text.*)
    _irom0_text_end = ABSOLUTE(.);
  } >irom0_0_seg :irom0_0_phdr

  .data : ALIGN(4)
  {
    _data_start = ABSOLUTE(.);
    *(.data)
    *(.data.*)
    _data_end = ABSOLUTE(.);
  } >dram0_0_seg :dram0_0_phdr

  .rodata : ALIGN(4)
  {
    _rodata_start = ABSOLUTE(.);
    __init_array_start = ABSOLUTE(.);
    __init_array_end = ABSOLUTE(.);
    _bss_table_start = ABSOLUTE(.);
    LONG(_bss_start)
    LONG(_bss_end)
    _bss_table_end = ABSOLUTE(.);
    _rodata_end = ABSOLUTE(.);
  } >dram0_0_seg :dram0_0_phdr

  .UserExceptionVector.literal : AT(LOADADDR(.rodata) + (ADDR(.UserExceptionVector.literal) - ADDR(.rodata))) ALIGN(4)
  {
    _UserExceptionVector_literal_start = ABSOLUTE(.);
    *(.UserExceptionVector.literal)
    _UserExceptionVector_literal_end = ABSOLUTE(.);
  } >dram0_0_seg :dram0_0_phdr

  .bss ALIGN(8) (NOLOAD) : ALIGN(4)
  {
    . = ALIGN (8);
    _bss_start = ABSOLUTE(.);
    *mcu_esp8266.a:flash.o(.bss .bss.* COMMON)
    *mcu_esp8266.a:heap_iram.o(.bss .bss.* COMMON)
    *mcu_esp8266.a:entry.o(.bss .bss.* COMMON)
	*board_esp8266.a:startup.o(.bss .bss.* COMMON)
    *libmain.a:spi_flash.o(.bss .bss.* COMMON)
    *libgcc.a:(.bss .bss.* COMMON)
    *libmain.a:(.bss .bss.* COMMON)
    *arch_xtensa_lx106.a:(.bss .bss.* COMMON)
    *libhal.a:(.bss .bss.* COMMON)
    *libnet80211.a:(.bss .bss.* COMMON)
    *mcu_esp8266.a:(.bss .bss.* COMMON)
    *libcirom.a:(.bss .bss.* COMMON)
    *libcrypto.a:(.bss .bss.* COMMON)
    *libwpa.a:(.bss .bss.* COMMON)
    *liblwip.a:(.bss .bss.* COMMON)
    *libphy.a:(.bss .bss.* COMMON)
    *libpp.a:(.bss .bss.* COMMON)
    *rhino.a:(.bss .bss.* COMMON)
    *(.bss)
    *(.bss.*)
    *(COMMON)
    . = ALIGN (8);
    _bss_end = ABSOLUTE(.);
    _heap_start = ABSOLUTE(.);
  } >dram0_0_seg :dram0_0_bss_phdr

  .lit4 : ALIGN(4)
  {
    _lit4_start = ABSOLUTE(.);
    *(*.lit4)
    *(.lit4.*)
    *(.gnu.linkonce.lit4.*)
    _lit4_end = ABSOLUTE(.);
  } >iram1_0_seg :iram1_0_phdr
}

/* get ROM code address */
INCLUDE "../ld/eagle.rom.addr.v6.ld"
