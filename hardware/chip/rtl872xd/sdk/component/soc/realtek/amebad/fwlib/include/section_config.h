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

#define HAL_ROM_ENTRY_NS_SECTION                         \
        SECTION(".hal.rom.entryns.rodata")

#define SIM_ROM_DATA_SECTION                         \
        SECTION(".sim.rom.rodata")

#define HAL_ROM_DATA_SECTION                         \
        SECTION(".hal.rom.rodata")

#define HAL_ROM_BSS_SECTION                         \
        SECTION(".hal.rom.bss")

#define HAL_ROM_BSS_SECTION_BANK		\
        SECTION(".hal.rom.bank.bss")
        
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
        

//3 Shell
#if defined (ARM_CORE_CM0)
#define SHELL_ROM_TEXT_SECTION	HAL_ROM_TEXT_SECTION
#define SHELL_ROM_BSS_SECTION		HAL_ROM_BSS_SECTION
#define SHELL_ROM_DATA_SECTION	HAL_ROM_DATA_SECTION
#else
#define SHELL_ROM_TEXT_SECTION
#define SHELL_ROM_BSS_SECTION
#define SHELL_ROM_DATA_SECTION
#endif
#define CMD_TABLE_DATA_SECTION				\
	SECTION(".cmd.table.data")

//3 Image 1 data
#define IMAGE1_ENTRY_SECTION                         \
        SECTION(".image1.entry.data")

#define IMAGE1_VALID_PATTEN_SECTION                   \
        SECTION(".image1.validate.rodata")

#define IMAGE1_DATA_SECTION                   \
        SECTION(".image1.rodata")

#define IMAGE2_VALID_PATTEN_SECTION                   \
        SECTION(".image2.validate.rodata")

//3 SRAM Config Section
#define SRAM_BD_DATA_SECTION                        \
        SECTION(".bdsram.data")
#define SRAM_NOCACHE_DATA_SECTION		\
	SRAM_BD_DATA_SECTION

#define SRAM_BF_DATA_SECTION                        \
        SECTION(".bfsram.data")

 #define IMAGE2_ENTRY_SECTION                     \
            SECTION(".image2.entry.data")
#define IMAGE2_RAM_TEXT_SECTION                     \
            SECTION(".image2.ram.text")
            
#define SDRAM_DATA_SECTION				

#if defined (ARM_CORE_CM4)
#define PSRAM_TEXT_SECTION			SECTION(".psram.text")
#define PSRAM_DATA_SECTION			SECTION(".psram.data")
#define PSRAM_RODATA_SECTION		        SECTION(".psram.rodata")
#define PSRAM_BSS_SECTION			SECTION(".psram.bss")
#define PSRAM_HEAP_SECTION			SECTION(".psram.heap")
#endif

//3 Wlan  Section
#define WLAN_ROM_TEXT_SECTION 

#define WLAN_ROM_DATA_SECTION

#define WLAN_RAM_MAP_SECTION

#undef CONFIG_WIFI_CRITICAL_CODE_SECTION
#define CONFIG_WIFI_CRITICAL_CODE_SECTION	IMAGE2_RAM_TEXT_SECTION
#define CONFIG_FW_CRITICAL_CODE_SECTION		//IMAGE2_RAM_TEXT_SECTION
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


#define LIBC_HEAP_SECTION                         	\
        SECTION(".heap.stdlib")
        
//3 SSL Section
#define SSL_ROM_TEXT_SECTION				\
        SECTION(".ssl.rom.text")
#define SSL_ROM_DATA_SECTION				\
        SECTION(".ssl.rom.rodata")
#define SSL_RAM_MAP_SECTION				\
        SECTION(".ssl_ram_map")

//OS Section
#define OS_ROM_TEXT_SECTION                      \
        SECTION(".os.rom.text")

#define OS_ROM_DATA_SECTION                      \
        SECTION(".os.rom.rodata")
        
//FLASH RUN CODE
#define FLASH_BOOT_TEXT_SECTION		SECTION(".flashboot.text")
#define IMAGE2_CUSTOM_SIGNATURE		SECTION(".img2_custom_signature") /* 32B: for OTA update */

//RDP (read protect area just text)
#define RDP_TEXT_SECTION		SECTION(".rdp.ram.text")
#define RDP_DATA_SECTION		SECTION(".rdp.ram.data")

#define RETENTION_TEXT_SECTION		SECTION(".retention.ram.text")
#define RETENTION_DATA_SECTION		SECTION(".retention.ram.data")
#define RETENTION_ENTRY_SECTION		SECTION(".retention.entry.data")

/* rom map */
#define ROM_FUNCTION_MAP		SECTION(".rommap.data")

//3 FW Section
#define FW_ROM_TEXT_SECTION                         \
        SECTION(".FW.rom.text")

#define FW_ROM_DATA_SECTION                         \
        SECTION(".FW.rom.rodata")

#define FW_ROM_BSS_SECTION                         \
        SECTION(".FW.rom.bss") 

#define FW_RAM_TEXT_SECTION                         \
        SECTION(".FW.ram.text")

#define FW_RAM_DATA_SECTION                         \
        SECTION(".FW.ram.rodata")

#define FW_RAM_BSS_SECTION                         \
        SECTION(".FW.ram.bss")           

/* image3 secure image */
#define IMAGE3_ENTRY_SECTION                     \
	SECTION(".image3.nsc_entry.text")

/*USB_OTG define*/
#define OTG_ROM_TEXT_SECTION
#define START_OTG_RAM_FUN_SECTION
#define START_OTG_RAM_DATA_SECTION
#define OTG_ROM_DATA_SECTION

#endif //_SECTION_CONFIG_H_
