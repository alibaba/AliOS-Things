OUTPUT_FORMAT("elf32-tradlittlemips", "elf32-tradbigmips", "elf32-tradlittlemips")
OUTPUT_ARCH(mips)
GROUP(-lgcc -lc)
ENTRY(_start)

SECTIONS
{
  . = 0xffffffff80010000;
  .text :
  {
    _ftext = .;
    *(.text)
    *(.text.*)
    *(.rodata)
    *(.rodata.*)
    *(.rodata1)
    *(.reginfo)
    *(.init)
    *(.stub)
    *(.gnu.warning)
    _cli_region_begin = .;
    KEEP(*(CliRegion))
    _cli_region_end = .;
    __core_driver_start__ = .;
    KEEP(*(.core_driver_entry))
    __core_driver_end__ = .;
    __bus_driver_start__ = .;
    KEEP(*(*.bus_driver_entry))
    __bus_driver_end__ = .;
    __early_driver_start__ = .;
    KEEP(*(*.early_driver_entry))
    __early_driver_end__ = .;
    __vfs_driver_start__ = .;
    KEEP(*(*.vfs_driver_entry))
    __vfs_driver_end__ = .;
    __level0_driver_start__ = .;
    KEEP(*(*.level0_driver_entry))
    __level0_driver_end__ = .;
    __level1_driver_start__ = .;
    KEEP(*(*.level1_driver_entry))
    __level1_driver_end__ = .;
    __level2_driver_start__ = .;
    KEEP(*(*.level2_driver_entry))
    __level2_driver_end__ = .;
    __level3_driver_start__ = .;
    KEEP(*(*.level3_driver_entry))
    __level3_driver_end__ = .;
    __post_driver_start__ = .;
    KEEP(*(*.post_driver_entry))
    __post_driver_end__ = .;
  } = 0
  _etext = .;
  PROVIDE (etext = .);
  .fini : { *(.fini) } = 0
  .eh_frame_hdr :
  {
    *(.eh_frame_hdr)
    *(.eh_frame_entry .eh_frame_entry.*)
  }
  .eh_frame : ONLY_IF_RO
  {
    KEEP (*(.eh_frame))
  }
  .data :
  {
    _fdata = .;
    *(.data)
    *(.data.*)
    . = ALIGN(32);
    *(.data.align32)
    . = ALIGN(64);
    *(.data.align64)
    . = ALIGN(128);
    *(.data.align128)
    . = ALIGN(4096);
    *(.data.align4096)
    CONSTRUCTORS
  }
  .data1 : { *(.data1) }
  .ctors :
  {
    __CTOR_LIST__ = .;
    LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
    *(.ctors)
    LONG(0)
    __CTOR_END__ = .;
  }
  .dtors :
  {
    __DTOR_LIST__ = .;
    LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
    *(.dtors)
    LONG(0)
    __DTOR_END__ = .;
  }
  _gp = ALIGN(16) + 0x7ff0;
  .got :
  {
    *(.got.plt) *(.got)
  }
  .sdata : { *(.sdata) }
  .lit8 : { *(.lit8) }
  .lit4 : { *(.lit4) }
  _edata = .;
  PROVIDE (edata = .);
  __bss_start = .;
  _fbss = .;
  .sbss : { *(.sbss) *(.scommon) }
  .bss :
  {
    *(.dynbss)
    *(.bss)
    *(.bss.*)
    . = ALIGN(32);
    *(.bss.align32)
    . = ALIGN(64);
    *(.bss.align64)
    . = ALIGN(128);
    *(.bss.align128)
    . = ALIGN(4096);
    *(.bss.align4096)
    *(COMMON)
  }
  . = ALIGN(4);
  _end = .;
  PROVIDE (end = .);

  . = ALIGN(1024);
  __heap_start = .;

  .stab 0 : { *(.stab) }
  .stabstr 0 : { *(.stabstr) }

  .debug 0 : { *(.debug) }
  .debug_srcinfo 0 : { *(.debug_srcinfo) }
  .debug_aranges 0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  .debug_sfnames 0 : { *(.debug_sfnames) }
  .line 0 : { *(.line) }

  .gptab.sdata : { *(.gptab.data) *(.gptab.sdata) }
  .gptab.sbss : { *(.gptab.bss) *(.gptab.sbss) }
}
