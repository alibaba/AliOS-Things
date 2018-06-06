/* ======================================================================= */
/*                  TARGET CONFIGURATION SECTION                           */
/* ======================================================================= */
/* modem2g_master.ld */
/* The following values are defined in ff_target.def and exported by the Makefile */
/*  FLASH_SIZE              */
/*  RAM_SIZE                */
/*  CALIB_BASE              */
/*  FACT_SETTINGS_BASE      */
/*  CODE_BASE               */
/*  USER_DATA_BASE          */
/*  USER_DATA_SIZE          */
/* ======================================================================= */
/*                  END CONFIGURATION SECTION                              */
/* ======================================================================= */
/* The linker script depends on the following defines */
/*  - CONST_IN_RAM : all constants are placed in RAM, this is useful for ram_run */
/*  - CODE_IN_RAM : All code is executed from RAM, this is useful for ram_run or code running from RAM on romulator */
/*  - PRELOADED_INT_SRAM : The internal SRAM are already preloaded, useful for ram_run */
/*  - PRELOADED_RAM : The external RAM is already preloaded, useful for ram_run or simu */
/* Those are constants of the system */


/* ROM content selection: things to include
    WARNING: the DISCARD section uses the opposite -->
    do all inclusion before discard !*/


/* Take into account assert bug in binutils-2.15*/
ENTRY (boot_Sector)
/*Mandatory to have the BB side irq handler linked */
EXTERN (__bb_int_handler)
EXTERN (_sxr_Irq_handler)
EXTERN (boot_LoaderEnter)
EXTERN (boot_Sector)
EXTERN (boot_Sector_Nand)
SECTIONS
{
    .check_boot_banned_sections :
    {
        *:boot_*.o(.sramtext .sramrodata .sramucdata .sramdata .sramucbss .srambss .data .data.* .gnu.linkonce.d.* .bss .bss.* COMMON .scommon .sbss .sbss.* .sbss2 .sbss2.* .rambss .gnu.linkonce.b.*) */boot_*.o(.sramtext .sramrodata .sramucdata .sramdata .sramucbss .srambss .data .data.* .gnu.linkonce.d.* .bss .bss.* COMMON .scommon .sbss .sbss.* .sbss2 .sbss2.* .rambss .gnu.linkonce.b.*)
        *:target_boot_config.o(.sramtext .sramrodata .sramucdata .sramdata .sramucbss .srambss .data .data.* .gnu.linkonce.d.* .bss .bss.* COMMON .scommon .sbss .sbss.* .sbss2 .sbss2.* .rambss .gnu.linkonce.b.*) */target_boot_config.o(.sramtext .sramrodata .sramucdata .sramdata .sramucbss .srambss .data .data.* .gnu.linkonce.d.* .bss .bss.* COMMON .scommon .sbss .sbss.* .sbss2 .sbss2.* .rambss .gnu.linkonce.b.*)
        *:drv_flsh_map.o(.sramtext .sramrodata .sramucdata .sramdata .sramucbss .srambss .data .data.* .gnu.linkonce.d.* .bss .bss.* COMMON .scommon .sbss .sbss.* .sbss2 .sbss2.* .rambss .gnu.linkonce.b.*) */drv_flsh_map.o(.sramtext .sramrodata .sramucdata .sramdata .sramucbss .srambss .data .data.* .gnu.linkonce.d.* .bss .bss.* COMMON .scommon .sbss .sbss.* .sbss2 .sbss2.* .rambss .gnu.linkonce.b.*)
        /* For backward compatibility, some object files are allowed to
         * have normal data or bss sections, e.g., drv_flsh.o
         */
    }
    ASSERT(SIZEOF(.check_boot_banned_sections) == 0, "Boot files cannot contain normal data or bss sections")
    /* ========================================== */
    /* in External RAM                            */
    /* ========================================== */
    . = 0xFFFFFFFF82000000;
    /* ------------------------------------------------ */
    /* SPI REG DEBUG - Ext RAM UNCACHED */
    /* ------------------------------------------------ */







    /* Go Uncached */
    . += 0x20000000;

    _ram_spi_reg_debug_uncached_start = .;

    .spi_reg_debug (_ram_spi_reg_debug_uncached_start) (NOLOAD) :
        AT (ADDR(.spi_reg_debug))
    {
        _spi_reg_debug_start = .;
        *(.xcv_reg_value)
        . = _spi_reg_debug_start + 0x500;
        *(.abb_reg_value)
        . = _spi_reg_debug_start + 0x500 + 0x140;

        *(.fmd_reg_value)
        . = _spi_reg_debug_start + 0x500 + 0x140 + 0x200;

        *(.pmu_reg_value)
        . = _spi_reg_debug_start + 0x500 + 0x140 + 0x200 + 0x140;
        *(.ap_reg_value)
        . = _spi_reg_debug_start + 0x500 + 0x140 + 0x200 + 0x140 + 0x0;
    }

    _ram_spi_reg_debug_uncached_end = .;
    _ram_spi_reg_debug_uncached_size = _ram_spi_reg_debug_uncached_end - _ram_spi_reg_debug_uncached_start;

    /* Go Cached */
    . -= 0x20000000;

    . = ALIGN(0x100);

    /* In RAMRUN, all the RAM used by BCPU will be indicated by _bcpu_flash_end
     * instead of _bcpu_ram_bss_end */


    . = _bcpu_ram_bss_end;



    . = ALIGN(0x100);

    _ram_available_base = .;


    /* ========================================== */
    /* set current location counter to flash base */
    /* ========================================== */
    . = 0xFFFFFFFF88000000 + 0x10000;
    _flash_start = .;
    /* ============================= */
    /* boot sector in flash          */
    /* ============================= */
    .boot_code _flash_start : AT (ADDR(.boot_code))
    {
/* keep 4 word boot sector offset */
        LONG(0xFFFFFFFF)
        . = ALIGN(0x10);
        _boot_sector = .;
        *(.boot_sector_start)
        *(.boottext)
        *(.bootrodata)
        *boot_sector.o(.text .text.* .gnu.linkonce.t.* .rdata .rodata .rodata.* .gnu.linkonce.r.*)
        *boot_sector_chip.o(.text .text.* .gnu.linkonce.t.* .rdata .rodata .rodata.* .gnu.linkonce.r.*)
        *boot_sys.o(.text .text.* .gnu.linkonce.t.* .rdata .rodata .rodata.* .gnu.linkonce.r.*)
        *boot_mem_init.o(.text .text.* .gnu.linkonce.t.* .rdata .rodata .rodata.* .gnu.linkonce.r.*)
        *boot_key.o(.text .text.* .gnu.linkonce.t.* .rdata .rodata .rodata.* .gnu.linkonce.r.*)
        *boot_sector_driver.o(.text .text.* .gnu.linkonce.t.* .rdata .rodata .rodata.* .gnu.linkonce.r.*)
        *target_boot_config.o(.text .text.* .gnu.linkonce.t.* .rdata .rodata .rodata.* .gnu.linkonce.r.*)
        . = ALIGN(0x10);
    }
    /* ======================================== */
    /* BCPU elf in XCPU elf */
    /* ======================================== */
.bcpu_rotext (0xffffffff88011100) : AT (0xffffffff88011100) { KEEP(*stripped_bcpu_elf_file.elf(.bcpu_rotext)) }
.bcpu_rodata (0xffffffff88011610) : AT (0xffffffff88011610) { KEEP(*stripped_bcpu_elf_file.elf(.bcpu_rodata)) }
.bcpu_rom_text (0xffffffffa1e80000) : AT (0xffffffff81e80000) { KEEP(*stripped_bcpu_elf_file.elf(.bcpu_rom_text)) }
.bcpu_rom_rodata (0xffffffff81e937b0) : AT (0xffffffff81e937b0) { KEEP(*stripped_bcpu_elf_file.elf(.bcpu_rom_rodata)) }
.bbsram (0xffffffff81980000) (NOLOAD) : AT (0xffffffff81980000) { KEEP(*stripped_bcpu_elf_file.elf(.bbsram)) }
.bbsramu (0xffffffffa1981e00) (NOLOAD) : AT (0xffffffffa1981e00) { KEEP(*stripped_bcpu_elf_file.elf(.bbsramu)) }
.bbsram_globals (0xffffffff81986640) (NOLOAD) : AT (0xffffffff81986640) { KEEP(*stripped_bcpu_elf_file.elf(.bbsram_globals)) }
.mailbox (0xffffffffa1b00580) (NOLOAD) : AT (0xffffffffa1b00580) { KEEP(*stripped_bcpu_elf_file.elf(.mailbox)) }
.bcpu_rom_version_number (0xffffffff81e97ff8) : AT (0xffffffff81e97ff8) { KEEP(*stripped_bcpu_elf_file.elf(.bcpu_rom_version_number)) }
.bcpu_rom.crc (0xffffffff81e97ffc) : AT (0xffffffff81e97ffc) { KEEP(*stripped_bcpu_elf_file.elf(.bcpu_rom.crc)) }
.bbsram_patch_text (0xffffffff81986800) : AT (0xffffffff88011620) { KEEP(*stripped_bcpu_elf_file.elf(.bbsram_patch_text)) }
.bbsram_patch_data (0xffffffff81987870) : AT (0xffffffff88012690) { KEEP(*stripped_bcpu_elf_file.elf(.bbsram_patch_data)) }
.bbsram_patch_ucdata (0xffffffffa19878a0) : AT (0xffffffff880126c0) { KEEP(*stripped_bcpu_elf_file.elf(.bbsram_patch_ucdata)) }
.bbsram_patch_ucbss (0xffffffffa1987bc0) (NOLOAD) : AT (0xffffffffa1987bc0) { KEEP(*stripped_bcpu_elf_file.elf(.bbsram_patch_ucbss)) }
.bcpu_sramtext (0xffffffff81c00680) : AT (0xffffffff880129e0) { KEEP(*stripped_bcpu_elf_file.elf(.bcpu_sramtext)) }
.bcpu_sramucdata (0xffffffffa1c01ad0) : AT (0xffffffff88013e30) { KEEP(*stripped_bcpu_elf_file.elf(.bcpu_sramucdata)) }
.bcpu_bss (0xffffffff82000a00) (NOLOAD) : AT (0xffffffff82000a00) { KEEP(*stripped_bcpu_elf_file.elf(.bcpu_bss)) }
.bcpu_flash_end (0xffffffff88013e70) : AT (0xffffffff88013e70) { KEEP(*stripped_bcpu_elf_file.elf(.bcpu_flash_end)) }
    . = _bcpu_flash_end;
    /* ============================== */
    /* Main entry point after monitor */
    /* ============================== */
    .main_entry_section . : AT (ADDR(.main_entry_section))
    {
        _code_base = .;
        *boot_loader.o(.text.boot_loader_start)
        *boot_loader.o(.text .text.* .gnu.linkonce.t.* .rdata .rodata .rodata.* .gnu.linkonce.r.*)
        . = ALIGN(0x10);
        _board_config_start = .;
        *(.board_config)
        . = ALIGN(0x10);
        _board_config_end = .;
    }
    _board_config_size = _board_config_end - _board_config_start;
    _int_rom_flash_start_location = .;
    /* ======================================== */
    /* internal rom in flash elf */
    /* ======================================== */
.internal_rom (0xffffffff81e00000) : AT (0xffffffff81e00000) { KEEP(*mem_bridge_rom_CHIP.elf(.internal_rom)) }
.rom_entries_uncached (0xffffffffa1c000a0) (NOLOAD) : AT (0xffffffffa1c000a0) { KEEP(*mem_bridge_rom_CHIP.elf(.rom_entries_uncached)) }
.rom_entries_cached (0xffffffff81c00194) (NOLOAD) : AT (0xffffffff81c00194) { KEEP(*mem_bridge_rom_CHIP.elf(.rom_entries_cached)) }
.boot_rom_version_number (0xffffffff81e05ff8) : AT (0xffffffff81e05ff8) { KEEP(*mem_bridge_rom_CHIP.elf(.boot_rom_version_number)) }
.internal_rom.crc (0xffffffff81e05ffc) : AT (0xffffffff81e05ffc) { KEEP(*mem_bridge_rom_CHIP.elf(.internal_rom.crc)) }
    _int_rom_flash_end_location = _int_rom_flash_start_location;
    /* ===================================================== */
    /* in BB DUALPORT SRAM                                      */
    /* ===================================================== */
    _dualport_sram_flash_start_location = _int_rom_flash_end_location;
    . = 0xFFFFFFFF81B00000;
    . = _bcpu_dualport_sram_end;
    _dualport_sram_start = .;
    .dualport_sram_text . :
        AT (_dualport_sram_flash_start_location)
    {
        _dualport_sram_text_start = .;
        *(.dualport_sram_text)
        *(.dualport_sram_rodata)
        . = ALIGN(0x10);
        _dualport_sram_text_end = .;
    }
    _dualport_sram_text_size = _dualport_sram_text_end - _dualport_sram_text_start;
    .dualport_sram_data . :
        AT (_dualport_sram_flash_start_location + _dualport_sram_text_size)
    {
        _dualport_sram_cached_data_start = .;
        *(.dualport_sram_data)
        . = ALIGN(0x10);
        _dualport_sram_cached_data_end = .;
    }
    _dualport_sram_cached_data_size = _dualport_sram_cached_data_end - _dualport_sram_cached_data_start;
    /* Go Uncached */
    . += 0x20000000;
    .dualport_sram_ucdata . :
        AT (_dualport_sram_flash_start_location + _dualport_sram_text_size + _dualport_sram_cached_data_size)
    {
        _dualport_sram_uncached_data_start = .;
        *(.dualport_sram_ucdata)
        . = ALIGN(0x10);
        _dualport_sram_uncached_data_end = .;
    }
    _dualport_sram_uncached_data_size = _dualport_sram_uncached_data_end - _dualport_sram_uncached_data_start;
    /* Go Cached */
    . -= 0x20000000;
    _dualport_sram_end = .;
    _dualport_sram_size = _dualport_sram_end - _dualport_sram_start;
    /*----------------------- */
    /* BB DUALPORT SRAM BSS      */
    /*----------------------- */
    _dualport_bss_start = .;
    /* Go Uncached */
    . += 0x20000000;
    .dualport_sram_ucbss . (NOLOAD) : AT (ADDR(.dualport_sram_ucbss))
    {
        *(.dualport_sram_ucbss)
        . = ALIGN(0x10);
    }
    /* Go Cached */
    . -= 0x20000000;
    .dualport_sram_bss . (NOLOAD) : AT (ADDR(.dualport_sram_bss))
    {
        *(.dualport_sram_bss)
        . = ALIGN(0x10);
    }
    _dualport_bss_end = .;
    _dualport_bss_size = _dualport_bss_end - _dualport_bss_start;
    ASSERT(ABSOLUTE(0xFFFFFFFF81B00000 + 0x800 - 0x280) >= ABSOLUTE(_dualport_bss_end), "BB DUALPORT SRAM overflow.")
    _dualport_sram_flash_end_location = _dualport_sram_flash_start_location + _dualport_sram_size;
    /* ===================================================== */
    /* in SYS SRAM                                           */
    /* ===================================================== */
    /*----------------- */
    /* SYS SRAM UNCACHED */
    /*----------------- */
    /*----------------- */
    /* SYS SRAM CACHED  */
    /*----------------- */
    ASSERT(ABSOLUTE(__int_rom_reserved_end) <= 0xFFFFFFFF81C000A0 + 0x180, "Reserved Ram Reserved for Rom Overflowed")
    . = 0xFFFFFFFF81C000A0 + 0x180;
    /* HAL Boot Sector reload structure, contains information written by
     * HAL during the normal execution of the code and used by the
     * Boot Sector to reload the ram image when the phone boots.
     * It is mapped in the Internal SRAM because we don't want the Boot
     * Sector code to access the External RAM. And it is mapped at a fixed
     * location (after the last patch for the chip) because we
     * don't want the RAMRUN to overwrite it.
     * This structure is "no load" as we don't want it to be loaded
     * by the Ramrun process. */
    .hal_boot_sector_reload_struct . (NOLOAD) :
        AT (ADDR(.hal_boot_sector_reload_struct))
    {
        KEEP(*(.hal_boot_sector_reload_struct))
    }
    /* HAL Boot Sector structure, contains information written by
     * HAL during the normal execution of the code and used by the
     * Boot Sector when the phone boots.
     * It is mapped in the Internal SRAM because we don't want the Boot
     * Sector code to access the External RAM. And it is mapped at a fixed
     * location (after the last patch for the chip) because we
     * don't want the RAMRUN to overwrite it.
     * This structure is "no load" as we don't want it to be loaded
     * by the Ramrun process. */
    .boot_sector_struct . (NOLOAD) :
        AT (ADDR(.boot_sector_struct))
    {
        KEEP(*(.boot_sector_struct))
    }
    /* Boot Sector reload structure pointer, used to retrieve the
     * structure filled by HAL and used by the Boot Sector code.
     * This structure is "no load" as we don't want it to be loaded
     * by the Ramrun process. */
    .boot_sector_reload_struct_ptr (0xFFFFFFFF81C00280 -4-4-4) (NOLOAD) :
        AT (ADDR(.boot_sector_reload_struct_ptr))
    {
        KEEP(*(.boot_sector_reload_struct_ptr))
    }
    /* Boot Sector structure pointer, used to retrieve the command
     * structure filled by HAL and used by the Boot Sector code.
     * This structure is "no load" as we don't want it to be loaded
     * by the Ramrun process. */
    .boot_sector_struct_ptr (0xFFFFFFFF81C00280 -4-4) (NOLOAD) :
        AT (ADDR(.boot_sector_struct_ptr))
    {
        KEEP(*(.boot_sector_struct_ptr))
    }
    /* Fixed pointer, used to retrieve the map structure. */
    .fixptr (0xFFFFFFFF81C00280 -4) (NOLOAD) : AT (ADDR(.fixptr))
    {
        KEEP(*(.fixptr))
    }
    . = 0xFFFFFFFF81C00280;
    /* ------------------------------------------------ */
    /* SYS SRAM IRQ - IRQ HANDLER IN SYS SRAM */
    /* ------------------------------------------------ */
    _irq_sys_sram_flash_start_location = _dualport_sram_flash_end_location;
    _irq_sys_sram_start = .;
    .irqsram . : AT (_irq_sys_sram_flash_start_location)
    {
        *hal_irq_handler.o(.irqtext)
        *hal_irq_handler.o(.text .text.* .sramtext .rdata .rodata .rodata.*)
        . = ALIGN(0x10);
    }=0
    _irq_sys_sram_end = .;
    _irq_sys_sram_size = _irq_sys_sram_end - _irq_sys_sram_start;
    _irq_sys_sram_flash_end_location = _irq_sys_sram_flash_start_location + _irq_sys_sram_size;
    . = _bcpu_sys_sram_end;
    /* ------------------------------------------------ */
    /* BOOTING TEXT AND DATA SECTIONS IN SYS SRAM */
    /* ------------------------------------------------ */
    _boot_sys_sram_flash_start_location = _irq_sys_sram_flash_end_location;
    _boot_sys_sram_start = .;
    _boot_sys_sram_txt_start = .;
    .bootsramtext . : AT (_boot_sys_sram_flash_start_location)
    {
        *(.bootsramtext)
        *drv_flsh.o(.sramtext)
        *drv_ddr.o(.sramtext)
        *hal_ebc.o(.sramtext)
        *hal_flash.o(.sramtext)
        . = ALIGN(0x10);
    }
    _boot_sys_sram_txt_end = .;
    _boot_sys_sram_txt_size = _boot_sys_sram_txt_end - _boot_sys_sram_txt_start;
    _boot_sys_sram_data_start = .;
    .bootsramdata . : AT (_boot_sys_sram_flash_start_location + _boot_sys_sram_txt_size)
    {
        *(.bootsramdata)
        *drv_flsh.o(.sramdata)
        *drv_ddr.o(.sramdata)
        *hal_ebc.o(.sramdata)
        *hal_flash.o(.sramdata)
        . = ALIGN(0x10);
    }
    _boot_sys_sram_data_end = .;
    _boot_sys_sram_data_size = _boot_sys_sram_data_end - _boot_sys_sram_data_start;
    _boot_sys_sram_end = .;
    _boot_sys_sram_size = _boot_sys_sram_end - _boot_sys_sram_start;
    _boot_sys_sram_flash_end_location = _boot_sys_sram_flash_start_location + _boot_sys_sram_size;
    /* ------------------------------------------------ */
    /* NON-BOOTING TEXT AND DATA SECTIONS IN SYS SRAM */
    /* ------------------------------------------------ */
    _sys_sram_flash_start_location = _boot_sys_sram_flash_end_location;
    _sys_sram_start = .;
    _sys_sram_cached_start = .;
    .sramtext . : AT (_sys_sram_flash_start_location)
    {
        _sys_sram_txt_start = .;
        *(.sramtext .sramrodata)
        *pal_gsmr.o(.text .text.* .gnu.linkonce.t.*)
        *sxr_mem.o(.text .text.* .gnu.linkonce.t.*)
        *sxr_jbsd.o(.text .text.* .gnu.linkonce.t.*)
        *(.syssram_L1_text)
        . = ALIGN(0x10);
    }
    _sys_sram_txt_end = .;
    _sys_sram_txt_size = _sys_sram_txt_end - _sys_sram_start;
    .sramdata _sys_sram_txt_end : AT (_sys_sram_flash_start_location + _sys_sram_txt_size )
    {
        _sys_sram_data_start = .;
        *(.sramdata)
        *pal_gsmr.o(.data .data.* .gnu.linkonce.d.* .data1)
        *pal_gsmr.o(.sdata .sdata.* .gnu.linkonce.s.*)
        *pal_gsmr.o(.sdata2 .sdata2.* .gnu.linkonce.s2.*)
        *(.srrodata)
        . = ALIGN(0x10);
    }
    _sys_sram_data_end = .;
    _sys_sram_cached_end = .;
    _sys_sram_data_size = _sys_sram_data_end - _sys_sram_data_start;
    _sys_sram_cached_size = _sys_sram_cached_end - _sys_sram_cached_start;
    /*-------------------- */
    /* SYS SRAM UNCACHED   */
    /*-------------------- */
    /* Go Uncached */
    . += 0x20000000;
    .sramucdata . : AT (_sys_sram_flash_start_location + _sys_sram_cached_size)
    {
        _sys_sram_uncached_start = .;
        *(.sramucdata)
        *(.srroucdata)
        . = ALIGN(0x10);
    }
    _sys_sram_uncached_end = .;
    _sys_sram_uncached_size = _sys_sram_uncached_end - _sys_sram_uncached_start;
    /* Go Cached */
    . -= 0x20000000;
    _sys_sram_end = .;
    _sys_sram_size = _sys_sram_end - _sys_sram_start;
    _sys_sram_flash_end_location = _sys_sram_flash_start_location + _sys_sram_size;
    /* ------------------------------------------------ */
    /* BOOTING BSS IN SYS SRAM (content reset to 0 ) */
    /* ------------------------------------------------ */
    _boot_sys_sram_bss_start = .;
    .bootsrambss . (NOLOAD) : AT (ADDR(.bootsrambss))
    {
        *(.bootsrambss)
        . = ALIGN(0x10);
    }
    _boot_sys_sram_bss_end = .;
    _boot_sys_sram_bss_size = _boot_sys_sram_bss_end - _boot_sys_sram_bss_start;
    /*----------------------- */
    /* SYS SRAM BSS           */
    /*----------------------- */
    _sys_sram_bss_start = .;
    /* Go Uncached */
    . += 0x20000000;
    .sramucbss . (NOLOAD) : AT (ADDR(.sramucbss))
    {
        *(.sramucbss)
        . = ALIGN(0x10);
    }
    /* Go Cached */
    . -= 0x20000000;
    .srambss . (NOLOAD) : AT (ADDR(.srambss))
    {
        *(.srambss)
        . = ALIGN(0x10);
    }
    _sys_sram_bss_end = .;
    _sys_sram_bss_size = _sys_sram_bss_end - _sys_sram_bss_start;
    /* Overlay in SYS SRAM*/
    _sys_sram_overlay_start = .;
    OVERLAY . : NOCROSSREFS AT (_sys_sram_flash_end_location)
    {
        .sys_sram_overlay_0
         {
             /* FIXME: LD 2.19 doesn't like empty overlays... */
             LONG(0)
             . = ALIGN(0x10);
         }
         .sys_sram_overlay_1
         {
             /* FIXME: LD 2.19 doesn't like empty overlays... */
             LONG(0)
             . = ALIGN(0x10);
         }
         .sys_sram_overlay_2
         {
             *mmc_audio_overlay.o(.text .text.* .sramtext)
             *mmc_audio_overlay.o(.rdata .rodata .rodata.* .gnu.linkonce.t.* .stub .data .data.* .gnu.linkonce.d.* .data1 .sdata .sdata.* .gnu.linkonce.s.* .sdata2 .sdata2.* .gnu.linkonce.s2.*)
             *mmc_audio_overlay.o(.bss .bss.* .srambss COMMON .scommon .sbss .sbss.* .sbss2 .sbss2.*)
             /* FIXME: LD 2.19 doesn't like empty overlays... */
             LONG(0)
             . = ALIGN(0x10);
         }
         .sys_sram_overlay_3
         {
             *zyvc_idct.o(.text .text.* .sramtext)
             *yuv420_to_rgb16.o(.text .text.* .sramtext)
             *zyvc_idct.o(.rdata .rodata .rodata.* .gnu.linkonce.t.* .stub .data .data.* .gnu.linkonce.d.* .data1 .sdata .sdata.* .gnu.linkonce.s.* .sdata2 .sdata2.* .gnu.linkonce.s2.*)
             *yuv420_to_rgb16.o(.rdata .rodata .rodata.* .gnu.linkonce.t.* .stub .data .data.* .gnu.linkonce.d.* .data1 .sdata .sdata.* .gnu.linkonce.s.* .sdata2 .sdata2.* .gnu.linkonce.s2.*)
             *zyvc_idct.o(.bss .bss.* .srambss COMMON .scommon .sbss .sbss.* .sbss2 .sbss2.*)
             *yuv420_to_rgb16.o(.bss .bss.* .srambss COMMON .scommon .sbss .sbss.* .sbss2 .sbss2.*)
             /* FIXME: LD 2.19 doesn't like empty overlays... */
             LONG(0)
             . = ALIGN(0x10);
         }
         .sys_sram_overlay_4
         {
             *rv_overlay_set.o(.text .text.* .sramtext)
             *deblock.o(.text .text.* .sramtext)
             *rv_bs.o(.text .text.* .sramtext)
             *rv_overlay_set.o(.rdata .rodata .rodata.* .gnu.linkonce.t.* .stub .data .data.* .gnu.linkonce.d.* .data1 .sdata .sdata.* .gnu.linkonce.s.* .sdata2 .sdata2.* .gnu.linkonce.s2.*)
             *deblock.o(.rdata .rodata .rodata.* .gnu.linkonce.t.* .stub .data .data.* .gnu.linkonce.d.* .data1 .sdata .sdata.* .gnu.linkonce.s.* .sdata2 .sdata2.* .gnu.linkonce.s2.*)
             *rv_bs.o(.rdata .rodata .rodata.* .gnu.linkonce.t.* .stub .data .data.* .gnu.linkonce.d.* .data1 .sdata .sdata.* .gnu.linkonce.s.* .sdata2 .sdata2.* .gnu.linkonce.s2.*)
             *rv_overlay_set.o(.bss .bss.* .srambss COMMON .scommon .sbss .sbss.* .sbss2 .sbss2.*)
             *deblock.o(.bss .bss.* .srambss COMMON .scommon .sbss .sbss.* .sbss2 .sbss2.*)
             *rv_bs.o(.bss .bss.* .srambss COMMON .scommon .sbss .sbss.* .sbss2 .sbss2.*)
             /* FIXME: LD 2.19 doesn't like empty overlays... */
             LONG(0)
             . = ALIGN(0x10);
         }
         .sys_sram_overlay_5
         {
             *mmc_amr_overlay.o(.sram_overlay)
             *vpp_speech_asm.o(.sram_overlay)
             /* FIXME: LD 2.19 doesn't like empty overlays... */
             LONG(0)
             . = ALIGN(0x10);
         }
    }
    /* ld-2.15 End of overlay bug workaround (Manual calculation)*/
    _sys_sram_overlay_end = _sys_sram_overlay_start + MAX(SIZEOF(.sys_sram_overlay_5), MAX(SIZEOF(.sys_sram_overlay_4), MAX(SIZEOF(.sys_sram_overlay_3), MAX(SIZEOF(.sys_sram_overlay_2), MAX(SIZEOF(.sys_sram_overlay_1), MAX(SIZEOF(.sys_sram_overlay_0), 18432))))));
    . = _sys_sram_overlay_end;
    _sys_sram_overlay_flash_size = 0 + SIZEOF(.sys_sram_overlay_0) + SIZEOF(.sys_sram_overlay_1) + SIZEOF(.sys_sram_overlay_2) + SIZEOF(.sys_sram_overlay_3) + SIZEOF(.sys_sram_overlay_4) + SIZEOF(.sys_sram_overlay_5);
    _sys_sram_overlay_flash_end_location = _sys_sram_flash_end_location + _sys_sram_overlay_flash_size;
    /*----------------------- */
    /* SYS SRAM HEAP 1        */
    /*----------------------- */
    _sys_sram_heap_start = _sys_sram_overlay_end;
    _sys_sram_top = 0xFFFFFFFF81C00000 +0x10000 -4; /* End of internal sram */
    _sys_sram_heap_end = _sys_sram_top - (0x80) - (0xE00);
    _sys_sram_heap_size = _sys_sram_heap_end - _sys_sram_heap_start;
    /* Check that the INT SRAM is not over stuffed */
    ASSERT(ABSOLUTE(_sys_sram_heap_end) > ABSOLUTE(_sys_sram_heap_start), "Internal SRAM overflow !")
    /*----------------------- */
    /* SYS SRAM IRQ STACK WATCH */
    /*----------------------- */
    _sys_sram_irq_stackwatch_size = (0x80) ;
    _sys_sram_irq_stackwatch_start = _sys_sram_heap_end;
    _sys_sram_irq_stackwatch_end = _sys_sram_irq_stackwatch_start + _sys_sram_irq_stackwatch_size;
    /* Caution, _sys_sram_non_romed_top is used by the boot ROM code. */
    .sram_romed_top (0xFFFFFFFF81C00000 +0x10000 -4) (NOLOAD) : AT (ADDR(.sram_romed_top))
    {
        _sys_sram_non_romed_top = .;
    }
    /*---------------- */
    /* End of SYS SRAM */
    /*---------------- */
    _fast_cpu_memory_overlay_flash_end_location = _sys_sram_overlay_flash_end_location;
    /* ========================================== */
    /* continued in External RAM                  */
    /* ========================================== */
    _boot_ram_flash_start_location = _fast_cpu_memory_overlay_flash_end_location;
    . = _ram_available_base;
    .dsp_iq_data . : AT (ADDR(.dsp_iq_data))
    {
        *(.dsp_iq_data)
        . = ALIGN(0x10);
    }
    /* ------------------------------------------------ */
    /* BOOTING TEXT AND DATA SECTIONS IN RAM */
    /* ------------------------------------------------ */
    _boot_ram_start = .;
    _boot_ram_txt_start = .;
    .bootramtext . : AT (_boot_ram_flash_start_location )
    {
        /* The flash driver must always be in RAM
         * because we need to be able to execute its code
         * when writing any sectors (For examplen, the calibration
         * sector which is on the first bank.
         * And so must be the functions called by the
         * flash drivers. */
        *(.bootramtext)
        *hal_spi_flash.o(.text .text.* .gnu.linkonce.t.* .rdata .rodata .rodata.* .gnu.linkonce.r.*)
        *drv_flsh.o(.text .text.* .gnu.linkonce.t.* .rdata .rodata .rodata.* .gnu.linkonce.r.*)
        *drv_ddr.o(.text .text.* .gnu.linkonce.t.* .rdata .rodata .rodata.* .gnu.linkonce.r.*)
        *drv_flsh_map.o(.text .text.* .gnu.linkonce.t.* .rdata .rodata .rodata.* .gnu.linkonce.r.*)
        *hal_ebc.o(.text .text.* .gnu.linkonce.t.* .rdata .rodata .rodata.* .gnu.linkonce.r.*)
        *hal_flash.o(.text .text.* .gnu.linkonce.t.* .rdata .rodata .rodata.* .gnu.linkonce.r.*)
        *InfoNES.o(.text .text.* .gnu.linkonce.t.* .stub .rodata .rodata.*)
        *K6502.o(.text .text.* .gnu.linkonce.t.* .stub .rodata .rodata.*)
        *InfoNES_pAPU.o(.text .text.* .gnu.linkonce.t.* .stub .rodata .rodata.*)
        . = ALIGN(0x10);
    }
    _boot_ram_txt_end = .;
    _boot_ram_txt_size = _boot_ram_txt_end - _boot_ram_txt_start;
    _boot_ram_cached_data_start = .;
    .bootdata . : AT (_boot_ram_flash_start_location + _boot_ram_txt_size)
    {
        /* The flash driver must always be in RAM
         * because we need to be able to execute its code
         * when writing any sectors (For examplen, the calibration
         * sector which is on the first bank.
         * And so must be the functions called by the
         * flash drivers. */
        *(.bootdata)
        *hal_spi_flash.o(.data .data.* .gnu.linkonce.d.*)
        *drv_flsh.o(.data .data.* .gnu.linkonce.d.*)
        *hal_ebc.o(.data .data.* .gnu.linkonce.d.*)
        *hal_flash.o(.data .data.* .gnu.linkonce.d.*)
        . = ALIGN(0x10);
    }
    _boot_ram_cached_data_end = .;
    _boot_ram_cached_data_size = _boot_ram_cached_data_end - _boot_ram_cached_data_start;
    _boot_ram_end = .;
    _boot_ram_size = _boot_ram_end - _boot_ram_start;
    _boot_ram_flash_end_location = _boot_ram_flash_start_location + _boot_ram_size;
    /* ------------------------------------------------ */
    /* NON-BOOTING TEXT AND DATA SECTIONS IN RAM */
    /* ------------------------------------------------ */
    _ram_flash_start_location = _boot_ram_flash_end_location;
    _ram_start = .;
    /* ------------------------------------------------ */
    /* Ext RAM CACHED  - SECTIONS ALWAYS IN RAM */
    /* ------------------------------------------------ */
    .ram . : AT (_ram_flash_start_location)
    {
        _ram_special_cached_start = .;
        _ram_txt_start = .;
        *(.ram)
        . = ALIGN(0x10);
    }
    _ram_special_cached_end = .;
    _ram_special_cached_size = _ram_special_cached_end - _ram_special_cached_start;
    /*---------------------------- */
    /* Ext RAM CACHED TXT          */
    /*---------------------------- */
    .ramtext _ram_special_cached_end : AT (_ram_flash_start_location + _ram_special_cached_size)
    {
        _ram_cached_txt_start = .;
/*****************************stack/l1****************************************/
   *stack_vers.o(.text .text.* .rodata .rodata.*)
   *stk_profile.o(.text .text.* .rodata .rodata.*)
/**********************************hal*******************************************/
   *hal_aif.o(.text .text.* .rodata .rodata.*)
   *hal_ana_gpadc.o(.text .text.* .rodata .rodata.*)
   *hal_comregs.o(.text .text.* .rodata .rodata.*)
   *hal_config.o(.text .text.* .rodata .rodata.*)
   *hal_debug.o(.text .text.* .rodata .rodata.*)
   *hal_debugpaltrc.o(.text .text.* .rodata .rodata.*)
   *hal_dma.o(.text .text.* .rodata .rodata.*)
   *hal_ebc.o(.text .text.* .rodata .rodata.*)
   *hal_gdb.o(.text .text.* .rodata .rodata.*)
   *hal_getk1.o(.text .text.* .rodata .rodata.*)
   *hal_host.o(.text .text.* .rodata .rodata.*)
   *hal_host_ram.o(.text .text.* .rodata .rodata.*)
   *hal_pwm.o(.text .text.* .rodata .rodata.*)
   *hal_rda_abb.o(.text .text.* .rodata .rodata.*)
   *hal_sim.o(.text .text.* .rodata .rodata.*)
   *hal_trace.o(.text .text.* .rodata .rodata.*)
   *hal_sys.o(.text .text.* .rodata .rodata.*)
   *hal_cipher.o(.text .text.* .rodata .rodata.*)
   *hal_profile.o(.text .text.* .rodata .rodata.*)
   *hal_assert.o(.text .text.* .rodata .rodata.*)
   *hal_fint.o(.text .text.* .rodata .rodata.*)
   *hal_gpio.o(.text .text.* .rodata .rodata.*)
   *hal_irq.o(.text .text.* .rodata .rodata.*)
   *hal_irq_handler.o(.text .text.* .rodata .rodata.*)
   *hal_ispi.o(.text .text.* .rodata .rodata.*)
   *hal_lps.o(.text .text.* .rodata .rodata.*)
   *hal_lps_util.o(.text .text.* .rodata .rodata.*)
   *hal_open.o(.text .text.* .rodata .rodata.*)
   *hal_pagy_spy.o(.text .text.* .rodata .rodata.*)
   *hal_pwm.o(.text .text.* .rodata .rodata.*)
   *hal_rfspi.o(.text .text.* .rodata .rodata.*)
   *hal_spi.o(.text .text.* .rodata .rodata.*)
   *hal_sxr.o(.text .text.* .rodata .rodata.*)
   *hal_sxr_mips.o(.text .text.* .rodata .rodata.*)
   *hal_sys_ifc.o(.text .text.* .rodata .rodata.*)
   *hal_tcu.o(.text .text.* .rodata .rodata.*)
   *hal_timer.o(.text .text.* .rodata .rodata.*)
   *hal_utils_mips.o(.text .text.* .rodata .rodata.*)
   *setjmp.o(.text .text.* .rodata .rodata.*)
/**********************************pal*******************************************/
  *pal_gsmr.o(.text .text.* .rodata .rodata.*)
  *pal_gsms.o(.text .text.* .rodata .rodata.*)
/**********************************base/sx*******************************************/
  *sxr_jbsd.o(.text .text.* .rodata .rodata.*)
  *sxr_mem.o(.text .text.* .rodata .rodata.*)
  *sxr_mutx.o(.text .text.* .rodata .rodata.*)
  *sxr_queu.o(.text .text.* .rodata .rodata.*)
  *sxr_sbx.o(.text .text.* .rodata .rodata.*)
  *sxr_tim.o(.text .text.* .rodata .rodata.*)
  *sxr_tksd.o(.text .text.* .rodata .rodata.*)
  *sxr_tls.o(.text .text.* .rodata .rodata.*)
  *sxr_usrmem.o(.text .text.* .rodata .rodata.*)
  *sxs_flsh.o(.text .text.* .rodata .rodata.*)
  *sxs_io.o(.text .text.* .rodata .rodata.*)
  *sxs_jbsd.o(.text .text.* .rodata .rodata.*)
  *sxs_mem.o(.text .text.* .rodata .rodata.*)
  *sxs_mutx.o(.text .text.* .rodata .rodata.*)
  *sxs_rmc.o(.text .text.* .rodata .rodata.*)
  *sxs_sbx.o(.text .text.* .rodata .rodata.*)
  *sxs_spy.o(.text .text.* .rodata .rodata.*)
  *sxs_srl.o(.text .text.* .rodata .rodata.*)
  *sxs_tim.o(.text .text.* .rodata .rodata.*)
  *sxs_tksd.o(.text .text.* .rodata .rodata.*)
  *sxs_usrmem.o(.text .text.* .rodata .rodata.*)
  *sxs_cksum.o(.text .text.* .rodata .rodata.*)
/**********************************rfd*******************************************/
  *drv_xcv.o(.text .text.* .rodata .rodata.*)
  *drv_pasw.o(.text .text.* .rodata .rodata.*)
/**********************************pmd*******************************************/
  *pmd_charger.o(.text .text.* .rodata .rodata.*)
  *pmd.o(.text .text.* .rodata .rodata.*)
 *vpp_audio_dec_tab_asm.o(.text .text.* .rodata .rodata.*)
 *resource_audio.o(.text .text.* .rodata .rodata.*)
 *resource_audio_app.o(.text .text.* .rodata .rodata.*)
        /* libgcc's softfloat library */
        _pack*f.o(.text .text.* .rodata .rodata.*)
        _unpack*f.o(.text .text.* .rodata .rodata.*)
        _*f.o(.text .text.* .rodata .rodata.*)
        *_lshrdi3.o(.text .text.* .rodata .rodata.*)
        *_ashldi3.o(.text .text.* .rodata .rodata.*)
        *_fixunssfsi.o(.text .text.* .rodata .rodata.*)
        *_fixunsdfsi.o(.text .text.* .rodata .rodata.*)
        *_fixdfdi.o(.text .text.* .rodata .rodata.*)
        *_fixunsdfdi.o(.text .text.* .rodata .rodata.*)
        *_divdi3.o(.text .text.* .rodata .rodata.*)
        *_moddi3.o(.text .text.* .rodata .rodata.*)
        *_udivdi3.o(.text .text.* .rodata .rodata.*)
        *_umoddi3.o(.text .text.* .rodata .rodata.*)
        *_sf_to_si.o(.text .text.* .rodata .rodata.*)
        *_df_to_si.o(.text .text.* .rodata .rodata.*)
        *_divdi3.o(.text .text.* .rodata .rodata.*)
        *_ashrdi3.o(.text .text.* .rodata .rodata.*)
        *_clzsi2.o(.text .text.* .rodata .rodata.*)
        . = ALIGN(0x10);
    } = 0
    _ram_cached_txt_end = .;
    ram_cached_txt_size = _ram_cached_txt_end - _ram_cached_txt_start;
    _ram_txt_end = .;
    /*---------------------- */
    /* Ext RAM UNCACHED Data */
    /*---------------------- */
    /* Go Uncached */
    . += 0x20000000;
    .ucdata . : AT (_ram_flash_start_location + _ram_special_cached_size + ram_cached_txt_size)
    {
        _ram_uncached_data_start = .;
        *(.ucdata)
        *(*.globalvars.uc)
        . = ALIGN(0x20);
    }
    _ram_uncached_data_end = .;
    _ram_uncached_data_size = _ram_uncached_data_end - _ram_uncached_data_start;
    /*---------------------- */
    /* Ext RAM CACHED Data */
    /*---------------------- */
    /* Go Cached */
    . -= 0x20000000;
    .data . : AT (_ram_flash_start_location + _ram_special_cached_size + ram_cached_txt_size + _ram_uncached_data_size)
    {
        _ram_cached_data_start = . ;
        /* NOTE: globalvars.c sections are to be removed.
         * They are kept here for backward compatibility of SVC/VPP codes.
         * These codes are linked from Total_BJ branch and the special global
         * variable sections will be removed in the future. */
        *(*.globalvars.c *.globalvars.c.*)
        *( .data)
        *( .data.*)
        *( .gnu.linkonce.d.*)
        *(.data1)
        _gp = ALIGN(16) + 0x7ff0;
        *(.sdata .sdata.* .gnu.linkonce.s.*)
        *(.sdata2 .sdata2.* .gnu.linkonce.s2.*)
        *(.lit8)
        *(.lit4)
        . = ALIGN(0x4);
        /***barrier : do not remove or edit */
        . = ALIGN(0x20);
    }
    _ram_cached_data_end = . ;
    _ram_cached_data_size = _ram_cached_data_end - _ram_cached_data_start;
    _ram_end = .;
    _ram_size = _ram_end - _ram_start;
    _ram_flash_end_location = _ram_flash_start_location + _ram_size;
    /*=================================================== */
    /* Code sections to be compressed (link stage 1)      */
    /*=================================================== */
    /*-------------------------------------------------------  */
    /* Uninitialized section in RAM (content kept after reboot)*/
    /*-------------------------------------------------------  */
    .backup . (NOLOAD) : AT (ADDR(.backup))
    {
        *(.backup)
        . = ALIGN(0x10);
    }
    /* Go Uncached */
    . += 0x20000000;
    .ucbackup . (NOLOAD) : AT (ADDR(.ucbackup))
    {
        *(.ucbackup)
        . = ALIGN(0x10);
    }
    /* Go Cached */
    . -= 0x20000000;
    /* ------------------------------------------------ */
    /* BOOTING BSS (content reset to 0 ) */
    /* ------------------------------------------------ */
    _boot_ram_bss_start = .;
    .bootbss . (NOLOAD) : AT (ADDR(.bootbss))
    {
        /* The flash driver must always be in RAM
         * because we need to be able to execute its code
         * when writing any sectors (For examplen, the calibration
         * sector which is on the first bank.
         * And so must be the functions called by the
         * flash drivers.*/
        *(.bootbss)
        *hal_spi_flash.o(.bss .bss.* COMMON .scommon .sbss .sbss.* .sbss2 .sbss2.* .rambss .gnu.linkonce.b.*)
        *drv_flsh.o(.bss .bss.* COMMON .scommon .sbss .sbss.* .sbss2 .sbss2.* .rambss .gnu.linkonce.b.*)
        *hal_ebc.o(.bss .bss.* COMMON .scommon .sbss .sbss.* .sbss2 .sbss2.* .rambss .gnu.linkonce.b.*)
        *hal_flash.o(.bss .bss.* COMMON .scommon .sbss .sbss.* .sbss2 .sbss2.* .rambss .gnu.linkonce.b.*)
        . = ALIGN(0x10);
    }
    _boot_ram_bss_end = .;
    _boot_ram_bss_size = _boot_ram_bss_end - _boot_ram_bss_start;
    /*-------------------------------------------------------  */
    /* RAM BSS (content reset to 0 )                           */
    /*-------------------------------------------------------  */
    _ram_bss_start = .;
    /* Go Uncached */
    . += 0x20000000;
    .ucbss . (NOLOAD) : AT (ADDR(.ucbss))
    {
        *(.ucbss)
        *(.ramucbss)
        . = ALIGN(0x10);
    }
    /* Go Cached */
    . -= 0x20000000;
    .bss . (NOLOAD) : AT (ADDR(.bss))
    {
        *(.sbss .sbss.* .gnu.linkonce.sb.*)
        *(.sbss2 .sbss2.* .gnu.linkonce.sb2.*)
        *(.scommon)
        *(.dynbss)
        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)
        *(.rambss)
        . = ALIGN(0x10);
    }
    _ram_bss_end = .;
    _ram_bss_size = _ram_bss_end - _ram_bss_start;
    /*----------- */
    /* End of RAM */
    /*----------- */
    _ram_top = 0xFFFFFFFF82000000 + 0x00400000;
    _heap_size = _ram_top -_ram_bss_end;
    _heap_start = _ram_bss_end;
    _heap_end = _ram_bss_end + _heap_size;
    /* Check that the external RAM is not over stuffed */
    ASSERT(ABSOLUTE(_ram_top) > ABSOLUTE(_ram_bss_end), "External RAM overflow !")
    /*========================================================= */
    /* In FLASH                                                 */
    /*========================================================= */
    _rom_flash_start_location = _ram_flash_end_location;
    . = _rom_flash_start_location;
    /* .crossrefs section can be used to list functions which is share by other application */
    .crossrefs . : AT (ADDR(.crossrefs))
    {
        *(.crossref)
        . = ALIGN(0x10);
    }
    .init . : AT(ADDR(.init))
    {
        *(.init)
        . = ALIGN(0x10);
    }
    .rotext . : AT (ADDR(.rotext))
    {
        *(.romtext)
        *(.text)
        *(.text.*)
        *(.stub .gnu.linkonce.t.*)
        . = ALIGN(0x20);
    }
    .rodata . : AT (ADDR(.rodata))
    {
        *(.rdata)
        *(.rodata)
        *(.rodata.*)
        *(.rodata.*_png)
        *(.gnu.linkonce.r.*)
        *(.rodata1)
        *(.roresdata)
        *(.robsdata)
        *(.extra)
        . = ALIGN(0x20);
    }
    /*=================================================== */
    /* Placeholder for misc hacks, not used for real apps */
    /*=================================================== */
    .misc . : AT (ADDR(.misc))
    {
        *(.misc)
        KEEP(*(.dbgfunc))
         . = ALIGN(0x10);
    }
    _flash_end = .;
    /* kept for compatibility */
    _etext = _ram_txt_end;
    _esramtext = _sys_sram_txt_end;
    _end = _ram_bss_end;
    /*=================================================== */
    /* Stub section to get compressed code & data         */
    /*=================================================== */
    /*=================================================== */
    /* Codes to fill the hole between user sector and     */
    /* calib sector                                       */
    /*=================================================== */
    /* Check that the data and code in flash do not overwrite */
    ASSERT(ABSOLUTE(_flash_end) <= 0xFFFFFFFF88000000+0x380000, "Flash overflow: there is no more space available in flash memory.")
    /*================================================= */
    /* Define user data boundaries based on the defines */
    /*================================================= */
    .user_data 0xFFFFFFFF88000000+0x3F6000 : AT (ADDR(.user_data))
    {
        _user_data_start = .;
        *user.bin.o(.user_data);
        KEEP(*(.user_data))
    }
    _user_data_end = .;
    _user_data_size = _user_data_end - _user_data_start;
    /* ============================== */
    /* calib sector in flash          */
    /* ============================== */
    .calib 0xFFFFFFFF88000000+0x3FA000 : AT (ADDR(.calib))
    {
        _calib_start = .;
        *(.calib);
        . = ALIGN(0x10);
    }
    _calib_end = .;
    _calib_size = _calib_end - _calib_start;
    /* ============================== */
    /* factory settings in flash      */
    /* ============================== */
    .factory_settings 0xFFFFFFFF88000000+0x3FE000 : AT (ADDR(.factory_settings))
    {
        _factory_start = .;
        *(.factory_settings)
        . = ALIGN(0x10);
    }
    _factory_end = .;
    _factory_size = _factory_end - _factory_start;
    /* Legal layout patterns:
     * 1) Code (including boot code) -> User Data -> Calib Data -> Factory Data
     * 2) Boot Sector -> Calib Data -> Factory Data -> Code (excluding boot code) -> User Data
     * /

    /*Check that the user data in flash do not overwrite
     * the calib data or factory setting space */
    ASSERT(ABSOLUTE(_calib_start) >= ABSOLUTE(_user_data_end) || ABSOLUTE(_factory_start) > ABSOLUTE(_code_base), "Calib data is overlapped with user data or factory data is overlapped with code.")
    /*Check that the calib data in flash do not overwrite
     * the factory setting data space */
    ASSERT(ABSOLUTE(_factory_start) > ABSOLUTE(_calib_start), "Factory setting data is overlapped with calib data.")
/* All sections we want to discard from the final linked elf.*/
    /*
     * DWARF debug sections.
     * Symbols in the DWARF debugging sections are relative to
     * the beginning of the section so we begin them at 0.
     */
    /* DWARF 1 */
    .debug (0) : { *(EXCLUDE_FILE(*.elf) .debug ) }
    .line (0) : { *(EXCLUDE_FILE(*.elf) .line ) }
    /* GNU DWARF 1 extensions */
    .debug_srcinfo (0) : { *(EXCLUDE_FILE(*.elf) .debug_srcinfo ) }
    .debug_sfnames (0) : { *(EXCLUDE_FILE(*.elf) .debug_sfnames ) }
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges (0) : { *(EXCLUDE_FILE(*.elf) .debug_aranges ) }
    .debug_pubnames (0) : { *(EXCLUDE_FILE(*.elf) .debug_pubnames ) }
    /* DWARF 2 */
    .debug_info (0) : { *(EXCLUDE_FILE(*.elf) .debug_info .gnu.linkonce.wi.* ) }
    .debug_abbrev (0) : { *(EXCLUDE_FILE(*.elf) .debug_abbrev ) }
    .debug_line (0) : { *(EXCLUDE_FILE(*.elf) .debug_line ) }
    .debug_frame (0) : { *(EXCLUDE_FILE(*.elf) .debug_frame ) }
    .debug_str (0) : { *(EXCLUDE_FILE(*.elf) .debug_str ) }
    .debug_loc (0) : { *(EXCLUDE_FILE(*.elf) .debug_loc ) }
    .debug_macinfo (0) : { *(EXCLUDE_FILE(*.elf) .debug_macinfo ) }
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames (0) : { *(EXCLUDE_FILE(*.elf) .debug_weaknames ) }
    .debug_funcnames (0) : { *(EXCLUDE_FILE(*.elf) .debug_funcnames ) }
    .debug_typenames (0) : { *(EXCLUDE_FILE(*.elf) .debug_typenames ) }
    .debug_varnames (0) : { *(EXCLUDE_FILE(*.elf) .debug_varnames ) }
    /* Stabs debugging sections.  */
    .stab (0) : { *(EXCLUDE_FILE(*.elf) .stab ) }
    .stabstr (0) : { *(EXCLUDE_FILE(*.elf) .stabstr ) }
    .stab.excl (0) : { *(EXCLUDE_FILE(*.elf) .stab.excl ) }
    .stab.exclstr (0) : { *(EXCLUDE_FILE(*.elf) .stab.exclstr ) }
    .stab.index (0) : { *(EXCLUDE_FILE(*.elf) .stab.index ) }
    .stab.indexstr (0) : { *(EXCLUDE_FILE(*.elf) .stab.indexstr ) }
    .comment (0) : { *(EXCLUDE_FILE(*.elf) .comment ) }
    /* These must appear regardless of  .  */
    .gptab.sdata :
    {
        *(.gptab.data)
        *(.gptab.sdata)
    }
    .gptab.sbss :
    {
        *(.gptab.bss)
        *(.gptab.sbss)
    }
    /DISCARD/ :
    {
        /* Error if a required input section is not listed in any output section */
        /* Useful to check the missing section descriptions */
        *(*)
    }
}
