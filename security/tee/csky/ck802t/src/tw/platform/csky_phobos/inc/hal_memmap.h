/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 * This file will be included by assembly!!
 */

#ifndef _HAL_MEMMAP_H_
#define _HAL_MEMMAP_H_

#define _SIZE_256K          (0x00040000)
#define _SIZE_224K          (0x00038000)
#define _SIZE_192K          (0x00030000)
#define _SIZE_184K          (0x0002E000)
#define _SIZE_96K           (0x00018000)
#define _SIZE_64K           (0x00010000)
#define _SIZE_48K           (0x0000c000)
#define _SIZE_36K           (0x00009000)
#define _SIZE_34K           (0x00008800)
#define _SIZE_32K           (0x00008000)
#define _SIZE_16K           (0x00004000)
#define _SIZE_10K           (0x00002800)
#define _SIZE_8K            (0x00002000)
#define _SIZE_6K            (0x00001800)
#define _SIZE_5K            (0x00001400)
#define _SIZE_4K            (0x00001000)
#define _SIZE_2K            (0x00000800)
#define _SIZE_1K            (0x00000400)

/**************************************
 * Memory Map
 *************************************/
#define FLASH_START           (0x10000000)
#define FLASH_SIZE            (_SIZE_256K)
#define FLASH_END             (FLASH_START + FLASH_SIZE)
#define SRAM_START            (0x20000000)
#define SRAM_SIZE             (_SIZE_64K)
#define SRAM_END              (SRAM_START + SRAM_SIZE)

#if CONFIG_TEST
#define HAL_TW_RO_SIZE         (_SIZE_64K)  /* 64K Flash */
#define HAL_TW_RW_SIZE         (_SIZE_16K)  /* 16K ISRAM */
#else
#define HAL_TW_RO_SIZE         (_SIZE_36K)  /* 36K Flash */
#define HAL_TW_RW_SIZE         (_SIZE_8K)   /* 8K ISRAM */
#endif

#if CONFIG_TEST
#define HAL_NTW_RO_SIZE        (_SIZE_64K)  /* 64K Flash */
#define HAL_NTW_RW_SIZE        (_SIZE_16K)  /* 16K ISRAM */
#else
#define HAL_NTW_RO_SIZE         (_SIZE_64K)  /* 64K Flash */
#define HAL_NTW_RW_SIZE         (_SIZE_16K)  /* 16K ISRAM */
#endif

/* VIC */
#define PLATFORM_INTC_BASEADDRESS         (0xE000E000)

/* Memory map  */
#define PLATFORM_UART_ADDRBASE0           (0x50010400)
#define PLATFORM_UART_GPIO_REUSE_BASE     (0x50018008)
#define PLATFORM_RSA_ADDRBASE             (0x4000f000)
#define PLATFORM_AES_ADDRBASE             (0x4000d000)
#define PLATFORM_SHA_BADDR                (0x4000e000)
#define PLATFORM_SRAM_SASC_BADDR          (0x40009000)
#define PLATFORM_PMU_ADDRBASE             (0x40000000)
#define PLATFORM_BUS_PRIO_BASE            (0x40000000)
#define PLATFORM_OTP_SECURE_SECTOR_START  (0x40006000)

#define PLATFORM_FLASH_SASC_BADDR           (0x40005038)
#define PLATFORM_OTP_SECURE_SECTOR_SIZE     (0x800)            /* 2K OTP */
#define PLATFORM_OTP_BANK_SIZE              (PLATFORM_OTP_SECURE_SECTOR_SIZE)

/**************************************
 * LPM config address
 *************************************/
/* in LPM save to disk mode, save all the RW data from LPM_RW_BASE to LPM_RW_SAVED_FLASH_ADDR */
#define LPM_RW_BASE                     (SRAM_START)
#define LPM_RW_SIZE                     (SRAM_SIZE)
#define LPM_RW_SAVED_SIZE               (LPM_RW_SIZE) /* Save all sram data to flash */
#define LPM_RW_SAVED_ADDR               (FLASH_START + FLASH_SIZE - (SRAM_SIZE + 4))
#define LPM_RESUEM_POINT_SAVED_ADDR     (FLASH_START + FLASH_SIZE - 4)

/**************************************
 * Frequency
 *************************************/
/* CPU frequence definition */
#define CPU_DEFAULT_FREQ        20000000  /* Hz */
/* AHB frequence definition */
#define AHB_DEFAULT_FREQ        20000000   /* Hz */
/* APB frequence definition */
#define APB_DEFAULT_FREQ        20000000   /* Hz */

/**************************************
 * Misc
 *************************************/
#define ARCH_WSC_INTR_ID          (12)
#define ARCH_SYS_EXCEPTION_NUM    (32)
#define ARCH_INTERRUPT_NUM        (128)

#define GP_CONTEXT_SIZE         0x30

#endif /*  _HAL_MEMMAP_H_ */
