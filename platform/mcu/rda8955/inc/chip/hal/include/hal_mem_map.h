/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/


#ifndef _HAL_MEM_MAP_H_
#define _HAL_MEM_MAP_H_

#include "cs_types.h"
#include "hal_sys.h"

///
/// @defgroup mem_map HAL Memory Map
/// This document describes the memory map
///
/// This document describes the different memory location available.
/// The addresses are represented by the symbols given by the linker.
/// THIS FILE MUST THUS BE SYNCHED WITH THE chip_master.ld FILE.
///
/// For more information about the memory usage, please refer to the <B>
/// Application Note 0012 "Memory Usage" </B>.
///
///  @{
///

//  Symbols from linker

// =============================================================================
// _factory_start
// -----------------------------------------------------------------------------
/// Start address of the factory settings sector, in the flash
// =============================================================================
extern UINT32 _factory_start;

// =============================================================================
// _factory_size
// -----------------------------------------------------------------------------
/// Size of the the factory settings
// =============================================================================
extern UINT32 _factory_size;

// =============================================================================
// _board_config_start
// -----------------------------------------------------------------------------
/// Start address of tgt_BoardConfig
// =============================================================================
extern UINT32 _board_config_start;

// =============================================================================
// _board_config_end
// -----------------------------------------------------------------------------
/// End address of tgt_BoardConfig
// =============================================================================
extern UINT32 _board_config_end;

// =============================================================================
// _board_config_size
// -----------------------------------------------------------------------------
/// Size of tgt_BoardConfig
// =============================================================================
extern UINT32 _board_config_size;

// =============================================================================
// _boot_sys_sram
// -----------------------------------------------------------------------------
/// Addresses of the sections in system internal sram for booting process
// =============================================================================
extern UINT32 _boot_sys_sram_flash_start_location;
extern UINT32 _boot_sys_sram_flash_end_location;
extern UINT32 _boot_sys_sram_start;
extern UINT32 _boot_sys_sram_txt_start;
extern UINT32 _boot_sys_sram_txt_end;
extern UINT32 _boot_sys_sram_data_load_start;
extern UINT32 _boot_sys_sram_data_load_end;
extern UINT32 _boot_sys_sram_data_start;
extern UINT32 _boot_sys_sram_bss_start;
extern UINT32 _boot_sys_sram_bss_end;

// =============================================================================
// _boot_ram
// -----------------------------------------------------------------------------
/// Addresses of the sections in external ram for booting process
// =============================================================================
extern UINT32 _boot_ram_flash_start_location;
extern UINT32 _boot_ram_flash_end_location;
extern UINT32 _boot_ram_start;
extern UINT32 _boot_ram_txt_start;
extern UINT32 _boot_ram_txt_end;
extern UINT32 _boot_ram_bss_start;
extern UINT32 _boot_ram_bss_end;

// =============================================================================
// _bb_sram_flash_start_location
// -----------------------------------------------------------------------------
/// Start address of the flash image of the bb internal sram
// =============================================================================
extern UINT32 _bb_sram_flash_start_location;

#ifdef FPGA_NBCPU
// =============================================================================
// _nb_sram_patch_flash_start
// -----------------------------------------------------------------------------
/// Start address of the flash image of the bb internal sram
// =============================================================================
extern UINT32 _nb_sram_flash_start;
extern UINT32 _nb_sram_start;
extern UINT32 _nb_sram_flash_end_location;
#endif

// =============================================================================
// _bb_sram_flash_end_location
// -----------------------------------------------------------------------------
/// End address of the flash image of the bb internal sram
// =============================================================================
extern UINT32 _bb_sram_flash_end_location;

// =============================================================================
// _bb_sram_romed_start
// -----------------------------------------------------------------------------
/// Start address of the bb internal sram
// =============================================================================
extern UINT32 _bb_sram_romed_start;

// =============================================================================
// _bb_sram_romed_itlv_buf_end
// -----------------------------------------------------------------------------
/// End address of the reserved space for the interleaver buffer
// =============================================================================
extern UINT32 _bb_sram_romed_itlv_buf_end;

// =============================================================================
// _bb_sram_romed_cached_end
// -----------------------------------------------------------------------------
/// End address of the reserved space for the cached data section
// =============================================================================
extern UINT32 _bb_sram_romed_cached_end;

// =============================================================================
// _bb_sram_romed_end
// -----------------------------------------------------------------------------
/// End address of the allocated bb internal internal sram
// =============================================================================
extern UINT32 _bb_sram_romed_end;

// =============================================================================
// _bb_sram_romed_globals_start
// -----------------------------------------------------------------------------
/// Start address of the section used to store global variables used by romed
/// BCPU code
// =============================================================================
extern UINT32 _bb_sram_romed_globals_start;

// =============================================================================
// _bb_sram_romed_globals_end
// -----------------------------------------------------------------------------
/// End address of the section used to store global variables used by romed
/// BCPU code
// =============================================================================
extern UINT32 _bb_sram_romed_globals_end;

// =============================================================================
// _bb_sram_patch
// -----------------------------------------------------------------------------
/// Addresses of the sections in bb internal sram used to patch BCPU code
// =============================================================================
extern UINT32 _bb_sram_patch_flash_start;
extern UINT32 _bb_sram_patch_flash_end;
extern UINT32 _bb_sram_patch_start;
extern UINT32 _bb_sram_patch_bss_start;
extern UINT32 _bb_sram_patch_bss_end;


#ifdef FPGA_NBCPU
// =============================================================================
// _nb_sram_romed_end
// -----------------------------------------------------------------------------
/// End address of the allocated nb internal internal sram
// =============================================================================
extern UINT32 _nb_sram_romed_end;

// =============================================================================
// _nb_sram_romed_globals_start
// -----------------------------------------------------------------------------
/// Start address of the section used to store global variables used by romed
/// BCPU code
// =============================================================================
extern UINT32 _nb_sram_romed_globals_start;

// _nb_sram_patch
// -----------------------------------------------------------------------------
/// Addresses of the sections in nb internal sram used to patch BCPU code
// =============================================================================
extern UINT32 _nb_sram_patch_flash_start;
extern UINT32 _nb_sram_patch_flash_end;
extern UINT32 _nb_sram_patch_start;
extern UINT32 _nb_sram_patch_bss_start;
extern UINT32 _nb_sram_patch_bss_end;
#endif

// =============================================================================
// _dualport_sram
// -----------------------------------------------------------------------------
/// Addresses of the sections in dualport sram
// =============================================================================
extern UINT32 _dualport_sram_flash_start_location;
extern UINT32 _dualport_sram_flash_end_location;
extern UINT32 _dualport_sram_start;
extern UINT32 _dualport_bss_start;
extern UINT32 _dualport_bss_end;
extern UINT32 _bcpu_dualport_sram_flash_start_location;
extern UINT32 _bcpu_dualport_sram_flash_end_location;
extern UINT32 _bcpu_dualport_sram_start;
extern UINT32 _bcpu_dualport_bss_start;
extern UINT32 _bcpu_dualport_bss_end;

// =============================================================================
// _bcpu_sys_sram
// -----------------------------------------------------------------------------
/// Addresses of the sections in system internal sram for BCPU
// =============================================================================
extern UINT32 _bcpu_sys_sram_flash_start_location;
extern UINT32 _bcpu_sys_sram_flash_end_location;
extern UINT32 _bcpu_sys_sram_start;
extern UINT32 _bcpu_sys_sram_bss_start;
extern UINT32 _bcpu_sys_sram_bss_end;

// =============================================================================
// _bcpu_ram
// -----------------------------------------------------------------------------
/// Addresses of the sections in external ram for BCPU
// =============================================================================
extern UINT32 _bcpu_ram_flash_start_location;
extern UINT32 _bcpu_ram_flash_end_location;
extern UINT32 _bcpu_ram_start;
extern UINT32 _bcpu_ram_bss_start;
extern UINT32 _bcpu_ram_bss_end;

extern UINT32 _bcpu_ram_data_load_start;
extern UINT32 _bcpu_ram_data_load_end;
extern UINT32 _bcpu_ram_data_start;

extern UINT32 _bcpu_flash_end;

// =============================================================================
// _irq_sys_sram_flash_start_location
// -----------------------------------------------------------------------------
/// Start address of the flash image of the irq handler in the system internal sram
// =============================================================================
extern UINT32 _irq_sys_sram_flash_start_location;

// =============================================================================
// _irq_sys_sram_flash_end_location
// -----------------------------------------------------------------------------
/// End address of the flash image of the irq handler in the system internal sram
// =============================================================================
extern UINT32 _irq_sys_sram_flash_end_location;

// =============================================================================
// _irq_sys_sram_start
// -----------------------------------------------------------------------------
/// Start address of the irq handler in the system internal sram
// =============================================================================
extern UINT32 _irq_sys_sram_start;

// =============================================================================
// _irq_sys_sram_end
// -----------------------------------------------------------------------------
/// End address of the irq handler in the system internal sram
// =============================================================================
extern UINT32 _irq_sys_sram_end;

// =============================================================================
// _sys_sram_flash_start_location
// -----------------------------------------------------------------------------
/// Start address of the flash image of the system internal sram
// =============================================================================
extern UINT32 _sys_sram_flash_start_location;

// =============================================================================
// _sys_sram_flash_end_location
// -----------------------------------------------------------------------------
/// End address of the flash image of the system internal sram
// =============================================================================
extern UINT32 _sys_sram_flash_end_location;

// =============================================================================
// _sys_sram_start
// -----------------------------------------------------------------------------
/// Start address of the system internal sram
// =============================================================================
extern UINT32 _sys_sram_start;

// =============================================================================
// _sys_sram_end
// -----------------------------------------------------------------------------
/// End address of the allocated system internal sram
// =============================================================================
extern UINT32 _sys_sram_end;

// =============================================================================
// _sys_sram_txt_start
// -----------------------------------------------------------------------------
/// Start address of the Text (where the code is) section in the system internal sram
// =============================================================================
extern UINT32 _sys_sram_txt_start;

// =============================================================================
// _sys_sram_txt_end
// -----------------------------------------------------------------------------
/// End address of the Text (where the code is) section in the system internal sram
// =============================================================================
extern UINT32 _sys_sram_txt_end;

// =============================================================================
// _sys_sram_bss_start
// -----------------------------------------------------------------------------
/// Start address of the cached BSS section in the system internal sram
// =============================================================================
extern UINT32 _sys_sram_bss_start;

// =============================================================================
// _sys_sram_bss_end
// -----------------------------------------------------------------------------
/// End address of the cached BSS section in the system internal sram
// =============================================================================
extern UINT32 _sys_sram_bss_end;

// =============================================================================
// _sys_sram_heap_start
// -----------------------------------------------------------------------------
/// Start address of the heap area in the system internal sram
// =============================================================================
extern UINT32 _sys_sram_heap_start;

// =============================================================================
// _sys_sram_heap_end
// -----------------------------------------------------------------------------
/// End address of the heap area in the system internal sram
// =============================================================================
extern UINT32 _sys_sram_heap_end;

// =============================================================================
// _sys_sram_heap_size
// -----------------------------------------------------------------------------
/// Size of the heap area in the system internal sram
// =============================================================================
extern UINT32 _sys_sram_heap_size;

// =============================================================================
// _sys_sram_irq_stackwatch_start
// -----------------------------------------------------------------------------
/// Start address of the protection zone section in the system internal sram
// =============================================================================
extern UINT32 _sys_sram_irq_stackwatch_start;

// =============================================================================
// _sys_sram_irq_stackwatch_end
// -----------------------------------------------------------------------------
/// Start address of the protection zone section in the system internal sram
// =============================================================================
extern UINT32 _sys_sram_irq_stackwatch_end;

// =============================================================================
// _sys_sram_top
// -----------------------------------------------------------------------------
/// End of system internal sram : x*32k - mailbox size
// =============================================================================
extern UINT32 _sys_sram_top;

// =============================================================================
// _ram_flash_start_location
// -----------------------------------------------------------------------------
/// Start address of the flash image of the external ram
// =============================================================================
extern UINT32 _ram_flash_start_location;

// =============================================================================
// _ram_flash_end_location
// -----------------------------------------------------------------------------
/// End address of the flash image of the external ram
// =============================================================================
extern UINT32 _ram_flash_end_location;

// =============================================================================
// _ram_start
// -----------------------------------------------------------------------------
/// Start address of the external ram
// =============================================================================
extern UINT32 _ram_start;

// =============================================================================
// _ram_end
// -----------------------------------------------------------------------------
/// End address of initialized section in external ram
// =============================================================================
extern UINT32 _ram_end;

// =============================================================================
// _ram_txt_start
// -----------------------------------------------------------------------------
/// Start address of the Text area in external ram
// =============================================================================
extern UINT32 _ram_txt_start;

// =============================================================================
// _ram_txt_end
// -----------------------------------------------------------------------------
/// End address of the Text area in external ram
// =============================================================================
extern UINT32 _ram_txt_end;

// =============================================================================
// _ram_data_load_start
// -----------------------------------------------------------------------------
/// Load start address of the Data area in external ram
// =============================================================================
extern UINT32 _ram_data_load_start;

// =============================================================================
// _ram_data_load_end
// -----------------------------------------------------------------------------
/// Load end address of the Data area in external ram
// =============================================================================
extern UINT32 _ram_data_load_end;

// =============================================================================
// _ram_data_start
// -----------------------------------------------------------------------------
/// Start address of the Data area in external ram
// =============================================================================
extern UINT32 _ram_data_start;

// =============================================================================
// _ram_data_end
// -----------------------------------------------------------------------------
/// End address of the Data area in external ram
// =============================================================================
extern UINT32 _ram_data_end;

// =============================================================================
// _ram_bss_start
// -----------------------------------------------------------------------------
/// Start address of the cached BSS area in external ram
// =============================================================================
extern UINT32 _ram_bss_start;

// =============================================================================
// _ram_bss_end
// -----------------------------------------------------------------------------
/// End address of the cached BSS area in external ram
// =============================================================================
extern UINT32 _ram_bss_end;

// =============================================================================
// _ram_top
// -----------------------------------------------------------------------------
/// (Physical) End of the ram
// =============================================================================
extern UINT32 _ram_top;

// =============================================================================
// _heap_start
// -----------------------------------------------------------------------------
/// Start of heap area in External RAM
// =============================================================================
extern UINT32 _heap_start;

// =============================================================================
// _heap_end
// -----------------------------------------------------------------------------
/// End of heap area in External RAM
// =============================================================================
extern UINT32 _heap_end;

// =============================================================================
// _heap_size
// -----------------------------------------------------------------------------
/// Available heap size for External ram
// =============================================================================
extern UINT32 _heap_size;

// =============================================================================
// _flash_start
// -----------------------------------------------------------------------------
/// Start addresss of the flash
// =============================================================================
extern UINT32 _flash_start;

// =============================================================================
// _flash_end
// -----------------------------------------------------------------------------
/// End address of the allocated flash
// =============================================================================
extern UINT32 _flash_end;

// =============================================================================
// _user_data_start
// -----------------------------------------------------------------------------
/// Start address of the reserved space for the user data in flash
// =============================================================================
extern UINT32 _user_data_start;

// =============================================================================
// _user_data_end
// -----------------------------------------------------------------------------
/// End address of the reserved space for the user data in flash
// =============================================================================
extern UINT32 _user_data_end;

// =============================================================================
// _user_data_size
// -----------------------------------------------------------------------------
/// Size of the reserved space for the user data in flash
// =============================================================================
extern UINT32 _user_data_size;

// =============================================================================
// _int_rom_start
// -----------------------------------------------------------------------------
/// Start address of the  internal rom
// =============================================================================
extern UINT32 _int_rom_start;

// =============================================================================
// _int_rom_end
// -----------------------------------------------------------------------------
/// End address of the  internal rom
// =============================================================================
extern UINT32 _int_rom_end;

// =============================================================================
// _int_rom_flash_start_location
// -----------------------------------------------------------------------------
/// Start address of the flash image of the rom
// =============================================================================
extern UINT32 _int_rom_flash_start_location;

// =============================================================================
// _int_rom_flash_end_location
// -----------------------------------------------------------------------------
/// End address of the flash image of the system internal sram
// =============================================================================
extern UINT32 _int_rom_flash_end_location;

// =============================================================================
// _bcpu_rom
// -----------------------------------------------------------------------------
/// Addresses of the sections related to the BCPU ROM
// =============================================================================
extern UINT32 _bcpu_rom_start;
extern UINT32 _bcpu_rom_flash_start_location;
extern UINT32 _bcpu_rom_flash_end_location;

// =============================================================================
// _proxy_memory_start
// -----------------------------------------------------------------------------
/// Start address of the system proxy memory.
// =============================================================================
extern UINT32 _proxy_memory_start;

// =============================================================================
// _proxy_memory_end
// -----------------------------------------------------------------------------
/// End address of the system proxy memory.
// =============================================================================
extern UINT32 _proxy_memory_end;

// =============================================================================
// _proxy_memory_flash_start_location
// -----------------------------------------------------------------------------
/// Start address of the flash image of the system proxy memory.
// =============================================================================
extern UINT32 _proxy_memory_flash_start_location;

// =============================================================================
// _proxy_memory_flash_end_location
// -----------------------------------------------------------------------------
/// End address of the flash image of the system proxy memory.
// =============================================================================
extern UINT32 _proxy_memory_flash_end_location;

// =============================================================================
// _proxy_memory_top
// -----------------------------------------------------------------------------
/// End of system proxy memory : 1*32k
// =============================================================================
extern UINT32 _proxy_memory_top;


// =============================================================================
// HAL_FUNC_INTERNAL
// -----------------------------------------------------------------------------
/// Macro used to put a specific function in internal SRAM
// =============================================================================
#define HAL_FUNC_INTERNAL __attribute__((section(".sramtext")))


// =============================================================================
// HAL_RODATA_INTERNAL
// -----------------------------------------------------------------------------
/// Macro used to put some specific constant variables in internal SRAM
// =============================================================================
#define HAL_RODATA_INTERNAL __attribute__((section(".sramrodata")))


// =============================================================================
// HAL_DATA_INTERNAL
// -----------------------------------------------------------------------------
/// Macro used to put some specific variables in internal SRAM
// =============================================================================
#define HAL_DATA_INTERNAL __attribute__((section(".sramdata")))


// =============================================================================
// HAL_BSS_INTERNAL
// -----------------------------------------------------------------------------
/// Macro used to put some specific variables in internal SRAM
// =============================================================================
#define HAL_BSS_INTERNAL __attribute__((section(".srambss")))


// =============================================================================
// HAL_UNCACHED_DATA_INTERNAL
// -----------------------------------------------------------------------------
/// Macro used to put some specific variables in uncached internal SRAM
// =============================================================================
#define HAL_UNCACHED_DATA_INTERNAL __attribute__((section(".sramucdata")))


// =============================================================================
// HAL_UNCACHED_BSS_INTERNAL
// -----------------------------------------------------------------------------
/// Macro used to put some specific variables in uncached internal SRAM
// =============================================================================
#define HAL_UNCACHED_BSS_INTERNAL __attribute__((section(".sramucbss")))


// =============================================================================
// HAL_UNCACHED_DATA
// -----------------------------------------------------------------------------
/// Macro used to put some specific variables in uncached external RAM
// =============================================================================
#define HAL_UNCACHED_DATA __attribute__((section(".ucdata")))


// =============================================================================
// HAL_UNCACHED_BSS
// -----------------------------------------------------------------------------
/// Macro used to put some specific variables in uncached external RAM BSS
// =============================================================================
#define HAL_UNCACHED_BSS __attribute__((section(".ucbss")))


// =============================================================================
// HAL_BOOT_SECTOR_RELOAD_STRUCT_SECTION
// -----------------------------------------------------------------------------
/// Macro used to put the Boot Sector reload structure at a fixed location in internal
/// SRAM. This will avoid it to be overwritten by a RAMRUN, for instance.
// =============================================================================
#define HAL_BOOT_SECTOR_RELOAD_STRUCT_SECTION __attribute__((section (".hal_boot_sector_reload_struct")))


// =============================================================================
// HAL_BOOT_FUNC
// -----------------------------------------------------------------------------
/// Macro used to put a specific function in flash boot area
// =============================================================================
#define HAL_BOOT_FUNC __attribute__((section(".boottext")))


// =============================================================================
// HAL_BOOT_RODATA
// -----------------------------------------------------------------------------
/// Macro used to put some specific readonly variables in flash boot area
// =============================================================================
#define HAL_BOOT_RODATA __attribute__((section(".bootrodata")))


// =============================================================================
// HAL_BOOT_FUNC_INTERNAL
// -----------------------------------------------------------------------------
/// Macro used to put a specific function in internal RAM boot area
// =============================================================================
#define HAL_BOOT_FUNC_INTERNAL __attribute__((section(".bootsramtext")))


// =============================================================================
// HAL_BOOT_DATA_INTERNAL
// -----------------------------------------------------------------------------
/// Macro used to put some specific variables in internal RAM boot area
// =============================================================================
#define HAL_BOOT_DATA_INTERNAL __attribute__((section(".bootsramdata")))


// =============================================================================
// HAL_BOOT_BSS_INTERNAL
// -----------------------------------------------------------------------------
/// Macro used to put some specific variables in internal SRAM BSS boot area
// =============================================================================
#define HAL_BOOT_BSS_INTERNAL __attribute__((section(".bootsrambss")))


// =============================================================================
// HAL_BOOT_FUNC_RAM
// -----------------------------------------------------------------------------
/// Macro used to put a specific function in external RAM boot area
// =============================================================================
#define HAL_BOOT_FUNC_RAM __attribute__((section(".bootramtext")))


// =============================================================================
// HAL_BOOT_DATA
// -----------------------------------------------------------------------------
/// Macro used to put some specific variables in external RAM boot area
// =============================================================================
#define HAL_BOOT_DATA __attribute__((section(".bootdata")))


// =============================================================================
// HAL_BOOT_BSS
// -----------------------------------------------------------------------------
/// Macro used to put some specific variables in external RAM BSS boot area
// =============================================================================
#define HAL_BOOT_BSS __attribute__((section(".bootbss")))


// =============================================================================
// HAL_FUNC_RAM
// -----------------------------------------------------------------------------
/// Macro used to put a specific function in external RAM
// =============================================================================
#define HAL_FUNC_RAM __attribute__((section (".ram")))


// =============================================================================
// HAL_FUNC_ROM
// -----------------------------------------------------------------------------
/// Macro used to put a specific function in ROM
// =============================================================================
#define HAL_FUNC_ROM __attribute__((section (".romtext")))


// =============================================================================
// HAL_DATA_BACKUP
// -----------------------------------------------------------------------------
/// Macro used to put some specific variables in an area in external RAM
/// which will NOT be reinitialized during boot process.
// =============================================================================
#define HAL_DATA_BACKUP __attribute__((section (".backup")))


// =============================================================================
// HAL_UNCACHED_DATA_BACKUP
// -----------------------------------------------------------------------------
/// Macro used to put some specific variables in an area in uncached external RAM
/// which will NOT be reinitialized during boot process.
// =============================================================================
#define HAL_UNCACHED_DATA_BACKUP __attribute__((section (".ucbackup")))

#define HAL_DBG_FUNC __attribute__((section (".dbgfunc")))

// =============================================================================
// HAL_NB_RAM_AREA
// -----------------------------------------------------------------------------
//  External RAM is area 0, internal sram is area 1
/// This macro defines the number of ram spaces accessible by SX
// =============================================================================
#define HAL_NB_RAM_AREA 2

// =============================================================================
// HAL_HEAP_START
// -----------------------------------------------------------------------------
/// This macro defines the start of the heap for each accessible ram spaces
// =============================================================================
#define HAL_HEAP_START(area) ((area) ? (VOID*) &_sys_sram_heap_start : (VOID*) &_heap_start)

// =============================================================================
// HAL_HEAP_SIZE
// -----------------------------------------------------------------------------
/// This macro defines the available size of the heap for each accessible ram spaces
// =============================================================================
#define HAL_HEAP_SIZE(area) ((area) ? (UINT32) &_sys_sram_heap_size : (UINT32) &_heap_size)

// =============================================================================
// HAL_HEAP_END
// -----------------------------------------------------------------------------
/// This macro defines the end of the heap for each accessible ram spaces
// =============================================================================
#define HAL_HEAP_END(area)  ((area) ? (UINT32) &_sys_sram_heap_end : (UINT32) &_heap_end)

// =============================================================================
// HAL_IS_CODE
// -----------------------------------------------------------------------------
//  Is addr a valid address for code ?
/// This macro returns TRUE if the address can correspond to a code location
// =============================================================================
#define HAL_IS_CODE(addr)   (   ((HAL_SYS_GET_CACHED_ADDR(addr) > (UINT32) &_ram_txt_start) && (HAL_SYS_GET_CACHED_ADDR(addr) <  (UINT32)&_ram_txt_end)) \
                    || ((HAL_SYS_GET_CACHED_ADDR(addr) > (UINT32)&_sys_sram_txt_start) && (HAL_SYS_GET_CACHED_ADDR(addr) < (UINT32)&_sys_sram_txt_end)) \
                    || ((HAL_SYS_GET_CACHED_ADDR(addr) > (UINT32)&_flash_start) && (HAL_SYS_GET_CACHED_ADDR(addr) < (UINT32)&_flash_end))  )



///  @} <- End of the mem_map group


#endif

