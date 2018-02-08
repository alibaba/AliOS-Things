/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#ifndef _SECTION_CONFIG_H_
#define _SECTION_CONFIG_H_

#include "basic_types.h"

#define RAM_VECTOR_TABLE1_SECTION          \
        SECTION(".ram_vector_table1")
#define RAM_VECTOR_TABLE2_SECTION          \
        SECTION(".ram_vector_table2")
#define RAM_VECTOR_TABLE3_SECTION          \
        SECTION(".ram_vector_table3")

//3 //3 Hal Section
#define HAL_ROM_TEXT_SECTION                         \
        SECTION(".hal.rom.text")

#define HAL_ROM_DATA_SECTION                         \
        SECTION(".hal.rom.rodata")

#define BOOT_RAM_TEXT_SECTION                         \
        SECTION(".boot.ram.text")

#define BOOT_RAM_RODATA_SECTION                         \
        SECTION(".boot.rodata")

#define BOOT_RAM_DATA_SECTION                         \
        SECTION(".boot.ram.data")

#define BOOT_RAM_BSS_SECTION                         \
        SECTION(".boot.ram.bss")

#define BOOT_RAM_END_BSS_SECTION                         \
        SECTION(".boot.ram.end.bss")
        
#define HAL_ROM_BSS_SECTION                         \
        SECTION(".hal.rom.bss")

//3 Store the Image 1 validate code
#define IMAGE1_VALID_PATTEN_SECTION                   \
        SECTION(".image1.validate.rodata")

#define IMAGE2_VALID_PATTEN_SECTION                   \
        SECTION(".image2.validate.rodata")

//3 SRAM Config Section
#define SRAM_BD_DATA_SECTION                        \
        SECTION(".bdsram.data")

#define SRAM_BF_DATA_SECTION                        \
        SECTION(".bfsram.data")

    
#define IMAGE1_ENTRY_SECTION                         \
        SECTION(".image1.entry.data")

#define IMAGE1_EXPORT_SYMB_SECTION                         \
        SECTION(".image1.export.symb")
// USB OTG Section
#if 0
#define OTG_ROM_BSS_SECTION                             \
        SECTION(".otg.rom.bss")
        
#define OTG_ROM_TEXT_SECTION                            \
        SECTION(".otg.rom.text")

#define OTG_ROM_DATA_SECTION                      \
        SECTION(".otg.rom.rodata")
        
#define START_OTG_RAM_FUN_SECTION                       \
        SECTION(".ram.otg.data.a")

#define START_OTG_RAM_DATA_SECTION                      \
        SECTION(".ram.otg.data.b")
#endif

#define IMAGE2_ENTRY_SECTION                     \
            SECTION(".image2.entry.data")
#define IMAGE2_RAM_TEXT_SECTION                     \
            SECTION(".image2.ram.text")

//3 Wlan  Section
#define WLAN_ROM_TEXT_SECTION                      \
        SECTION(".wlan.rom.text")

#define WLAN_ROM_DATA_SECTION                      \
        SECTION(".wlan.rom.rodata")

#define WLAN_RAM_MAP_SECTION             \
        SECTION(".wlan_ram_map")

//3 Apple  Section
#define APPLE_ROM_TEXT_SECTION                      \
        SECTION(".apple.rom.text")

#define APPLE_ROM_DATA_SECTION                      \
        SECTION(".apple.rom.rodata")

//3 Libc Section
#define LIBC_ROM_TEXT_SECTION                      \
        SECTION(".libc.rom.text")

#define LIBC_ROM_DATA_SECTION                      \
        SECTION(".libc.rom.rodata")
        
#define LIBC_RAM_BSS_SECTION                         \
        SECTION(".libc.ram.bss")

#define LIBC_HEAP_SECTION                         	\
        SECTION(".heap.stdlib")
        
//3 SSL Section
#define SSL_ROM_TEXT_SECTION				\
        SECTION(".ssl.rom.text")
#define SSL_ROM_DATA_SECTION				\
        SECTION(".ssl.rom.rodata")
#define SSL_RAM_MAP_SECTION				\
        SECTION(".ssl_ram_map")

//FLASH RUN CODE
#define FLASH_BOOT_TEXT_SECTION		SECTION(".flashboot.text")
#define IMAGE2_CUSTOM_SIGNATURE		SECTION(".img2_custom_signature") /* 32B: for OTA update */

/** @brief Put .debug_trace symbol in .TRACE section to reduce code size. */
#define DEBUG_TRACE_DATA_SECTION		SECTION(".debug_trace") //__attribute__((aligned(4)))

//RDP (read protect area just text)
#define RDP_TEXT_SECTION		SECTION(".rdp.ram.text")
#define RDP_DATA_SECTION		SECTION(".rdp.ram.data")
#endif //_SECTION_CONFIG_H_
