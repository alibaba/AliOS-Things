/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#ifndef _HAL_8710B_H_
#define _HAL_8710B_H_

#include "platform_autoconf.h"
#include "basic_types.h"
#include "section_config.h"
#include "memproc.h"
#include "rtl8710b_sys_on.h"
#include "rtl8711b_peri_on.h"
#include "hal_platform.h"
#include "rtl8710b_pinmux.h"
#include "rtl8711b_vector.h"
#include "rtl8711b_diag.h"
#include "rtl8711b_efuse.h"
#include "rtl8710b_soc_ps.h"
#include "diag.h"
#include "rtl_consol.h"
#include "rtl8711b_rcc.h"
#include "rtl8711b_cache.h"
#include "rtl8711b_syscfg.h"
#include "rtl8710b_pmc.h"
#include "rtl8710b_clk.h"
#include "rtl8710b_bor.h"
#include "rtl8711b_crypto.h"
#include "rtl8710b_crypto_api.h"
#include "rtl8710b_boot.h"
#include "rtl8710b_wl_on.h"
#include "rtl8711b_otf.h"
#include "rtl8711b_flash.h"
#include "rtl8710b_backup_reg.h"
#include "rtl8710b_pinmap.h"
#include "hal_api.h"

#define __CM3_REV                      0x0200    /**< Core revision r0p0 */
#define __MPU_PRESENT                  1         /**< Defines if an MPU is present or not */
#define __NVIC_PRIO_BITS               4         /**< Number of priority bits implemented in the NVIC */
#define __Vendor_SysTickConfig         0         /**< Vendor specific implementation of SysTickConfig is defined *///see vPortSetupTimerInterrupt

#if !defined  (__FPU_PRESENT) 
#define __FPU_PRESENT             1       /*!< FPU present                                   */
#define __VFP_FP__	1
#endif /* __FPU_PRESENT */   

#if defined (ARM_CORE_CM4)
#include "core_cm4.h"
#elif defined (ARM_CORE_CM3)
#include "core_cm3.h"
#endif

#include "rtl8711b_gdma.h"
#include "rtl8711b_tim.h"
#include "rtl8711b_gpio.h"
#include "rtl8711b_ssi.h"
#include "rtl8711b_uart.h"
#include "rtl8711b_i2c.h"
#include "rtl8711b_i2s.h"
#include "rtl8711b_adc.h"
#include "rtl8710b_sdio.h"
#include "rtl8711b_wdg.h"
#include "rtl8710b_usb.h"
#include "rtl8710b_dongle.h"
#include "rtl8711b_otf.h"
#include "rtl8711b_rtc.h"
#include "rtl8710b_delay.h"

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

#define FLASH_RESERVED_DATA_BASE		0x8000  // reserve 32K for Image1
#define FLASH_SYSTEM_DATA_ADDR		0x9000  // reserve 32K+4K for Image1 + Reserved data
#define FLASH_OTA1_CODE_ADDR			0xB020
// Flash Map for Calibration data
#define FLASH_CAL_DATA_BASE			0xA000
#define FLASH_CAL_DATA_ADDR(_offset)	(FLASH_CAL_DATA_BASE + _offset)
#define FLASH_CAL_DATA_SIZE			0x1000
#define FLASH_SECTOR_SIZE				0x1000
// SPIC Calibration Data
#define FLASH_USB_PARA_OFFSET			0x100
#define FLASH_USB_PARA_BASE			(FLASH_SYSTEM_DATA_ADDR+FLASH_SPIC_PARA_OFFSET)
// ADC Calibration Data
#define FLASH_ADC_PARA_OFFSET			0x200
#define FLASH_ADC_PARA_BASE			(FLASH_SYSTEM_DATA_ADDR+FLASH_ADC_PARA_OFFSET)

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
	u32 reserved[4];
} IMAGE_HEADER;

typedef struct {
	u32 OTA2_FlashAddr;
	u32 Valid_Image2;							
	u32 OTA1_ForceGpio;
	u32 Rsvd1;

	u32 RDP_FlashAddr;
	u32 RDP_Len; /* not include check sum */
	u32 Rsvd2;
	u32 Rsvd3;

	u16 Spic_Mode;
	u16 Spic_Speed;
	u16 Flash_ID;
	u16 Flash_Size;
	u16 Flash_Status;
	u16 Rsvd4;
	u32 Rsvd5;

	/* uart log */
	u32 UlogRate;
	u32 UlogDbgEn;
	u32 UlogRsvd2;
	u32 UlogRsvd3;
	
	/* Jtag Password */
	u32 JtagErrMap;
	u8 JtagKey[8];
	u32 JtagRsvd1;

	u32 RSIPMask1;
	u32 RSIPMask2;
	u32 RSIPRsvd1;
	u32 RSIPRsvd2;
	
} SYSTEM_DATA;

#define USE_FULL_ASSERT

#ifdef  USE_FULL_ASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function which reports 
  *         the name of the source file and the source line number of the call 
  *         that failed. If expr is true, it returns no value.
  * @retval None
  */
  #define assert_param(expr) ((expr) ? (void)0 : io_assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void io_assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#endif //_HAL_8710B_H_
