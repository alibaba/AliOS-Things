/**
 * Copyright (C) 2015 The YunOS Project. All rights reserved.
 * This file will be included by assembly!!
 */

#ifndef _HAL_MEMMAP_H_
#define _HAL_MEMMAP_H_
#include "tee_addr_map.h"

/**************************************
 * Memory Map
 *************************************/
/* VIC */
#define PLATFORM_INTC_BASEADDRESS         (0xE000E000)

#if HOBBIT_NBIOT_256
/* Memory map for HOBBIT_NBIOT_256 */
#define PLATFORM_UART_ADDRBASE0           (0xb0005000)
#define PLATFORM_UART_GPIO_REUSE_BASE     (0xb0006008)

#define PLATFORM_RSA_ADDRBASE             (0xa000A000)
#define PLATFORM_AES_ADDRBASE             (0xa0006000)
#define PLATFORM_SHA_BADDR                (0xa0008000)
#define PLATFORM_SRAM_SASC_BADDR          (0xa0007000)
#define PLATFORM_PMU_ADDRBASE             (0xa0002000)

#define PLATFORM_BUS_PRIO_BASE            (0xa0000000)
#define PLATFORM_OTP_SECURE_SECTOR_START  (0xa003f800)

#elif defined(PHOBOS_GENERAL)

/* Memory map for PHOBOS_GENERAL */
#define PLATFORM_UART_ADDRBASE0           (0x50010400)
#define PLATFORM_UART_GPIO_REUSE_BASE     (0x50018008)
#define PLATFORM_RSA_ADDRBASE             (0x4000f000)
#define PLATFORM_AES_ADDRBASE             (0x4000d000)
#define PLATFORM_SHA_BADDR                (0x4000e000)
#define PLATFORM_SRAM_SASC_BADDR          (0x40009000)
#define PLATFORM_PMU_ADDRBASE             (0x40000000)
#define PLATFORM_BUS_PRIO_BASE            (0x40000000)
#define PLATFORM_OTP_SECURE_SECTOR_START  (0x40006000)

#else
/* Memory map for others */
#define PLATFORM_UART_ADDRBASE0           (0x50005000)
#define PLATFORM_UART_GPIO_REUSE_BASE     (0x50006008)
#define PLATFORM_RSA_ADDRBASE             (0x4000A000)
#define PLATFORM_AES_ADDRBASE             (0x40006000)
#define PLATFORM_SHA_BADDR                (0x40008000)
#define PLATFORM_SRAM_SASC_BADDR          (0x40007000)
#define PLATFORM_PMU_ADDRBASE             (0x40002000)

#define PLATFORM_BUS_PRIO_BASE            (0x40000000)
#define PLATFORM_OTP_SECURE_SECTOR_START  (0x4003f800)

#endif /* HOBBIT_NBIOT_256 */

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
