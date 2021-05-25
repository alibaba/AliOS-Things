/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef _HAL_8721D_H_
#define _HAL_8721D_H_

#include "platform_autoconf.h"
#include "basic_types.h"
#include "section_config.h"
#include "memproc.h"
#include "rtl8721d_sys_on.h"
#include "rtl8721d_peri_on.h"
#include "hal_platform.h"
#include "rtl8721d_vector.h"
#include "rtl8721d_loguart.h"
#include "rtl8721d_soc_ps.h"
#include "diag.h"
#include "shell.h"
#include "rand.h"
#include "monitor_rom.h"
#include "rtl8721d_syscfg.h"
#include "rtl8721d_pmc.h"
#include "rtl8721d_clk.h"
#include "rtl8721d_bor.h"
#include "rtl8721d_crypto.h"
#include "rtl8721d_crypto_api.h"
#include "rtl8721d_boot.h"
#include "rtl8721d_wl_on.h"
#include "rtl8721d_otf.h"
#include "rtl8721d_flash.h"
#include "rtl8721d_backup_reg.h"
#include "rtl8721d_pinmap.h"
#include "rtl8721d_ipc.h"
#include "rtl8721dhp_sysreg.h"
#include "rtl8721dlp_sysreg.h"
#include "rtl8721d_pinmux.h"
#ifndef CONFIG_BUILD_ROM
#include "rtl8721d_ipc_api.h"
#endif
#include "xmodem_update_rom.h"


#if defined (ARM_CORE_CM4)
#include "rtl8721dhp_rcc.h"
#else
#include "rtl8721dlp_rcc.h"
#endif

/* ===========================  Configuration of the ARM ARMV8MBL Processor and Core Peripherals  ============================ */

#if defined (ARM_CORE_CM4)
#ifdef AMEBAD_TODO
#define __ARMV8MML_REV                 0x0000U  /*!< ARMV8MML Core Revision                                                    */
#define __Vendor_SysTickConfig         0        /*!< Set to 1 if different SysTick Config is used                              */
#define __VTOR_PRESENT                 1        /*!< Set to 1 if CPU supports Vector Table Offset Register                     */
#define __FPU_DP                       0        /*!< Double Precision FPU                                                      */
#endif
#define __CM3_REV                      0x0200    /**< Core revision r0p0 */
#define __MPU_PRESENT                  1         /**< Defines if an MPU is present or not */
#define __NVIC_PRIO_BITS               3         /**< Number of priority bits implemented in the NVIC */
#define __Vendor_SysTickConfig         0         /**< Vendor specific implementation of SysTickConfig is defined *///see vPortSetupTimerInterrupt
#define __SAUREGION_PRESENT            1        /*!< SAU present or not                                                        */

#define __FPU_PRESENT             1       /*!< FPU present                                   */
#define __VFP_FP__	1
#ifndef __ARM_FEATURE_CMSE
#define __ARM_FEATURE_CMSE	3
#endif
#include <arm_cmse.h>   /* Use CMSE intrinsics */
#include "core_armv8mml.h"
#include "core_cache.h"
#elif defined (ARM_CORE_CM0)
#define __ARMV8MBL_REV                 0x0000U  /*!< ARMV8MBL Core Revision                                                    */
#define __NVIC_PRIO_BITS               2        /*!< Number of Bits used for Priority Levels                                   */
#define __Vendor_SysTickConfig         0        /*!< Set to 1 if different SysTick Config is used                              */
#define __VTOR_PRESENT                 1        /*!< Set to 1 if CPU supports Vector Table Offset Register                     */
#define __SAU_REGION_PRESENT           0        /*!< SAU present or not                                                        */

#define __MPU_PRESENT                  1         /**< Defines if an MPU is present or not */
#include "core_armv8mbl.h"
#include "core_cache.h"
#endif
#include "rtl8721d_trustzone.h"
#include "mpu_config.h"

#include "rtl8721d_gdma.h"
#include "rtl8721d_tim.h"
#include "rtl8721d_gpio.h"
#include "rtl8721d_ssi.h"
#include "rtl8721d_uart.h"
#include "rtl8721d_i2c.h"
#include "rtl8721d_i2s.h"
#include "rtl8721d_adc.h"
#include "rtl8721d_comparator.h"
#include "rtl8721d_sdio.h"
#include "rtl8721d_wdg.h"
#include "rtl8721d_rtc.h"
#include "rtl8721d_delay.h"
#include "rtl8721d_ir.h"
#include "rtl8721d_keyscan.h"
#include "rtl8721d_sgpio.h"
#include "rtl8721d_qdec.h"
#include "rtl8721d_usi.h"
#include "rtl8721d_usi_uart.h"
#include "rtl8721d_usi_ssi.h"
#include "rtl8721d_usi_i2c.h"
#include "rtl8721d_crc.h"
#include "rtl8721d_lcdc.h"
#include "rtl8721d_audio.h"
#include "rtl8721d_efuse.h"
#include "rtl8721d_cache.h"
#include "rtl8721d_psram.h"
#include "rtl8721d_sdioh.h"
#include "rtl8721dhp_sd.h"

// firmware information, located at the header of Image2
#define FW_VERSION          (0x0100)
#define FW_SUBVERSION       (0x0001)
#define FW_CHIP_ID          (0x8195)
#define FW_CHIP_VER         (0x01)
#define FW_BUS_TYPE         (0x01)          // the iNIC firmware type: USB/SDIO
#define FW_INFO_RSV1        (0x00)          // the firmware information reserved
#define FW_INFO_RSV2        (0x00)          // the firmware information reserved
#define FW_INFO_RSV3        (0x00)          // the firmware information reserved
#define FW_INFO_RSV4        (0x00)          // the firmware information reserved

#define FLASH_HS_BOOT_ADDR				(SPI_FLASH_BASE + 0x4000)
#define FLASH_RESERVED_DATA_BASE		0x2000  // reserve 8K for Image1
#define FLASH_SYSTEM_DATA_ADDR		0x3000  // reserve 8K+4K for Image1 + Reserved data
#define FLASH_OTA1_CODE_ADDR			0x6020
#define FLASH_SECTOR_SIZE				0x1000
//BT calibration Data
#define FLASH_BT_PARA_ADDR			0x5FF0

/* Spic_Mode */
#define ReadQuadIOMode			0
#define ReadQuadOMode			1
#define ReadDualIOMode			2
#define ReadDualOMode 			3
#define ReadOneMode				4

#define IMAGE_HEADER_LEN		0x20
typedef struct {
	u32 signature[2];
	u32 image_size;							
	u32 image_addr;

	/* reserved for extention */
	u32 sb_header;
	u32 reserved[3];
} IMAGE_HEADER;

typedef struct {
	/* 0x00 */
	u32 Rsvd00;
	u32 Valid_Image2;
	u32 Rsvd01[2];

	/* 0x10 */
	u32 Rsvd10[4];

	/* 0x20 */
	u32 Rsvd20[2];
	u32 BT_FW_DBG;
	u32 FTL_GC_Status;

} SYSTEM_DATA;


typedef struct {
	u32 reserved[12];

	unsigned char sb_sig[64];
} SB_HEADER;

typedef enum  _HAL_Status
{
	HAL_OK            = 0x00,
	HAL_BUSY          = 0x01,
	HAL_TIMEOUT       = 0x02,
	HAL_ERR_PARA      = 0x03,     // error with invaild parameters 
	HAL_ERR_MEM       = 0x04,     // error with memory allocation failed
	HAL_ERR_HW        = 0x05,     // error with hardware error

	HAL_ERR_UNKNOWN   = 0xee      // unknown error
  
} HAL_Status;


#define USE_FULL_ASSERT

#ifdef  USE_FULL_ASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function which reports 
  *         the name of the source file and the source line number of the call 
  *         that failed. If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0 : io_assert_failed((uint8_t *)__FUNCTION__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void io_assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#endif //_HAL_8721D_H_
