/**
  ******************************************************************************
  * @file    rtl8711b_syscfg.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for SYSCFG firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _RTL8710B_SYSCFG_H_
#define _RTL8710B_SYSCFG_H_

/** @addtogroup AmebaZ_Platform
  * @{
  */

/** @defgroup SYSCFG 
  * @brief SYSCFG driver modules
  * @{
  */

/** @addtogroup SYSCFG
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * Used for system, user can not used it if not needed.
  *
  *****************************************************************************************
  * REG_SYS_SYSTEM_CFG0 Introduction
  *****************************************************************************************
  *
  *    BIT[31]		Trapped PKG_ENG_SEL value 0: normal package; 1: engineering mode
  *    BIT[30]		CHIP_EN PIN input value
  *    BIT[27:24]	BD info
  *    BIT[16]		1: Test chip;  0:MP
  *    BIT[11:8]	Vendor ID
  *    BIT[7:4]		Chip version
  *    BIT[3:0]		Vendor ID defined in RF
  *
  *****************************************************************************************
  * REG_SYS_SYSTEM_CFG1 Introduction
  *****************************************************************************************
  *
  *    BIT[31:28]	is BIT_SYSCFG_TRP_ICFG, value is load from following trap pin:
  *    			ICFG[0]/ICFG[1]/ICFG[2]/ICFG[3] when trap pin TSET_MODE_SEL = 1
  *    BIT[27]		is BIT_SYSCFG_TRP_BOOT_SEL
  *    			0: boot normal, 1: uart flash download
  *    			value load from trap pin UART_DOWNLOAD_IMAGE
  *    BIT[25]		is BIT_SYSCFG_TRP_SPSLDO_SEL, Trapped Selection for SPS
  *    			0: SWR mode; 1: LDO mode
  *    			load from trap pin SPS_LDO_SEL
  *    BIT[16]		BIT_V15_VLD 1.5V Power Ready, 1: Power Ready
  *    BIT[9]		BIT_SYS_SYSPLL_CLK_RDY	SYS PLL  Clock Stable, 1: Clock Stable
  *    BIT[8]		BIT_SYS_XCLK_VLD	Xtal Clock Stable, 1: Clock Stable
  *    BIT[0]		BIT_SYSCFG_ALDN_STS 1: SYS CFG autoload done; 0; SYSCFG autoload not ready
  *
  *****************************************************************************************
  * REG_SYS_SYSTEM_CFG2 Introduction
  *****************************************************************************************
  *
  *    BIT[7:0]		ROM Information
  *
  *****************************************************************************************
  * REG_SYS_EFUSE_SYSCFGx Introduction
  *****************************************************************************************
  *
  *    REG_SYS_EFUSE_SYSCFG0: load from EFUSE 0x00~0x03
  *    REG_SYS_EFUSE_SYSCFG1: load from EFUSE 0x04~0x07
  *    REG_SYS_EFUSE_SYSCFG2: load from EFUSE 0x08~0x0B
  *    REG_SYS_EFUSE_SYSCFG3: load from EFUSE 0x0C~0x0F
  *    REG_SYS_EFUSE_SYSCFG4: load from EFUSE 0x10~0x13
  *    REG_SYS_EFUSE_SYSCFG5: load from EFUSE 0x14~0x17
  *    REG_SYS_EFUSE_SYSCFG6: load from EFUSE 0x18~0x1B
  *    REG_SYS_EFUSE_SYSCFG7: load from EFUSE 0x1C~0x1F
  *
  *****************************************************************************************
    * trap pins
  *****************************************************************************************
  *
  *    GPIOA_0:
  *			TEST_MODE_SEL
  *			default PD
  *
  *    GPIOA_3: 
  *			SPS_LDO_SEL
  *			default PU, internal PU
  *			0: SWR 1: LDO
  *			load to 0x1F4[25]
  *
  *    GPIOA_30:
  *			UART_DOWNLOAD_IMAGE
  *			default PU
  *			load to 0x1F4[27]
  *
  *****************************************************************************************
  * @endverbatim
  */
  
/* Exported constants --------------------------------------------------------*/

/** @defgroup SYSCFG_Exported_Constants SYSCFG Exported Constants
  * @{
  */ 

/** @defgroup SYSCFG_Bounding_Option_definitions 
  * @{
  */ 
#define SYSCFG_BD_QFN32						((u32)0x000000000)
#define SYSCFG_BD_QFN48_MCM_8MBFlash			((u32)0x000000001)
#define SYSCFG_BD_QFN48						((u32)0x000000002)
#define SYSCFG_BD_QFN48_NEW					((u32)0x000000000)
#define SYSCFG_BD_QFN68						((u32)0x000000007)
#define SYSCFG_BD_QFN68_NEW					((u32)0x000000005)
#define SYSCFG_BD_TFBGA_MCM_8761A			((u32)0x000000004)
/**
  * @}
  */


/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup SYSCFG_Exported_Functions SYSCFG Exported Functions
  * @{
  */
_LONG_CALL_ u32 SYSCFG0_Get(void);
_LONG_CALL_ u32 SYSCFG0_CUTVersion(void);
_LONG_CALL_ u32 SYSCFG0_BDOption(void);
_LONG_CALL_ u32 SYSCFG1_Get(void);
_LONG_CALL_ u32 SYSCFG1_AutoLoadDone(void);
_LONG_CALL_ u32 SYSCFG1_TRP_LDOMode(void);
_LONG_CALL_ u32 SYSCFG1_TRP_UARTImage(void);
_LONG_CALL_ u32 SYSCFG1_TRP_ICFG(void);
_LONG_CALL_ u32 SYSCFG2_Get(void);
_LONG_CALL_ u32 SYSCFG2_ROMINFO_Get(void);
_LONG_CALL_ void SYSCFG2_ROMINFO_Set(void);
/**
  * @}
  */

/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup SYSCFG_Register_Definitions SYSCFG Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup REG_SYS_EFUSE_SYSCFG0
 * @{
 *****************************************************************************/
#define BIT_MASK_SYS_EEROM_SWR_PAR_05_00	(0x0000003F << 24)
#define BIT_MASK_SYS_EEROM_LDO_PAR_07_04	(0x0000000F << 20)
#define BIT_SYS_CHIPPDN_EN						(0x00000001 << 17) /* 0:reset pin just give interrupt; 1:reset pin will powerdown chip directy */
#define BIT_SYS_EEROM_B12V_EN					(0x00000001 << 16)
#define BIT_MASK_SYS_EEROM_VID1				(0x000000FF << 8)
#define BIT_MASK_SYS_EEROM_VID0				(0x000000FF)
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_EFUSE_SYSCFG1
 * @{
 *****************************************************************************/
#define BIT_MASK_SYS_PDSPL_STL				(0x00000003 << 24)
#define BIT_MASK_SYS_PDSOC_STL				(0x00000003 << 22)
#define BIT_MASK_SYS_PDPON_STL				(0x00000003 << 20)
#define BIT_MASK_SYS_SWREG_XRT				(0x00000003 << 18)
#define BIT_MASK_SYS_SWSLC_STL				(0x00000003 << 16)
#define BIT_MASK_SYS_EEROM_SWR_PAR_46_45	(0x00000003 << 14)
#define BIT_MASK_SYS_EEROM_SWR_PAR_40_39	(0x00000003 << 12)
#define BIT_MASK_SYS_EEROM_SWR_PAR_33_26	(0x000000ff << 4)
#define BIT_MASK_SYS_EEROM_SWSLD_VOL		(0x00000007)
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_EFUSE_SYSCFG2
 * @{
 *****************************************************************************/
#define BIT_MASK_SYS_EERROM_ANAPAR_SPLL_24_15	(0x000003ff << 21)
#define BIT_MASK_SYS_EEROM_ANAPAR_SPLL_05_02	(0x0000000f << 16)
#define BIT_MASK_SYS_EEROM_XTAL_STEL_SEL			(0x00000003 << 12)
#define BIT_MASK_SYS_EEROM_XTAL_FREQ_SEL		(0x0000000f << 8)
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_EFUSE_SYSCFG3
 * @{
 *****************************************************************************/
#define BIT_MASK_SYS_DBG_PINGP_EN				(0x0000000f << 28)
#define BIT_MASK_SYS_DBG_SEL						(0x00000fff << 16)
#define BIT_MASK_SYS_DBGBY3_LOC_SEL				(0x00000003 << 14)
#define BIT_MASK_SYS_DBGBY2_LOC_SEL				(0x00000003 << 12)
#define BIT_MASK_SYS_DBGBY1_LOC_SEL				(0x00000003 << 10)
#define BIT_MASK_SYS_DBGBY0_LOC_SEL				(0x00000003 << 8)
#define BIT_SYS_EEROM_ANAPAR_SPLL_49				(0x00000001 << 3)	
#define BIT_MASK_SYS_EEROM_ANAPAR_SPLL_27_25	(0x00000007)
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_EFUSE_SYSCFG4
 * @{
 *****************************************************************************/
#define BIT_MASK_SYS_GPIOA_E2			(0x00000007 << 1)
#define BIT_SYS_GPIOA_H3L1				(0x00000001)
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_EFUSE_SYSCFG6
 * @{
 *****************************************************************************/
#define BIT_SYS_ICFG2_TEST				(0x00000001 << 31)
#define BIT_MASK_SYS_BOOT_TYPE_SEL	(0x00000003 << 28)
#define BIT_MASK_SYS_CPU_CLK_SEL		(0x00000003 << 26)
#define BIT_SYS_FLASH_DEEP_SLEEP_EN	(0x00000001 << 25)
#define BIT_SYS_SPIC_PARA_EN			(0x00000001 << 24)
#define BIT_USB_AUTOINSTALL_EN		(0x00000001 << 6)
#define BIT_USB_AUTOLOAD_UPHY_EN		(0x00000001 << 7)

/* 0x19 */
#define BIT_SYS_DIS_BOOT_LOG_EN		(0x00000001 << 15)
#define BIT_SYS_UART2_DEFAULT_GPIO	(0x00000001 << 14) /*!< 0: PA29 & PA30, 1: PA16 & PA17 */
#define BIT_SYS_FLASH_ENCRYPT_EN		(0x00000001 << 13)

#define BIT_SYS_SDIO_FEN				(0x00000001 << 1) /*!< SDIO Function enable */
#define BIT_SYS_SDIO_GP_SEL			(0x00000001 << 0) /*!< SDIO Pinmux selection 0: GPIOA_18 ~ GPIOA_23 1: GPIOA_6 ~ GPIOA_11 */
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_EFUSE_SYSCFG7
 * @{
 *****************************************************************************/
#define BIT_SYS_MEM_RMV_SIGN			(0x00000001 << 31)
#define BIT_SYS_MEM_RMV_1PRF1			(0x00000001 << 29)
#define BIT_SYS_MEM_RMV_1PRF0			(0x00000001 << 28)
#define BIT_SYS_MEM_RMV_1PSR			(0x00000001 << 27)
#define BIT_SYS_MEM_RMV_1PHSR			(0x00000001 << 26)
#define BIT_SYS_MEM_RMV_ROM			(0x00000001 << 25)
#define BIT_MASK_SYS_MEM_RME_CPU		(0x00000007 << 22)
#define BIT_MASK_SYS_MEM_RME_WLAN	(0x00000007 << 19)
#define BIT_SYS_MEM_RME_USB			(0x00000001 << 18)
#define BIT_SYS_MEM_RME_SDIO			(0x00000001 << 17)
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_SYSTEM_CFG0
 * @{
 *****************************************************************************/
#define BIT_SYSCFG_RF_RL_ID			(0x0000000F) /*!< Vendor ID defined in RF */
#define BIT_MASK_SYSCFG_CUT_VER		(0x0000000F << 4)
#define BIT_MASK_SYSCFG_VENDOR_ID	(0x0000000F << 8)
#define BIT_MASK_SYSCFG_CHIP_TYPE		(0x00000001 << 16)
#define BIT_MASK_SYSCFG_BD_OPT		(0x0000000F << 24)
#define BIT_SYSCFG_BD_PKG_SEL			(0x00000001 << 31)
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_SYSTEM_CFG1
 * @{
 *****************************************************************************/
#define BIT_MASK_SYSCFG_TRP_ICFG		(0x0000000f << 28)

#define BIT_SysCFG_TRP_SPSLDO_SEL		(0x00000001 << 25)
#define BIT_SysCFG_TRP_UART_IMAGE		(0x00000001 << 27)
#define BIT_V15_VLD						(0x00000001 << 16)
#define BIT_SYS_SYSPLL_CLK_RDY			(0x00000001 << 9)
#define BIT_SYS_XCLK_VLD				(0x00000001 << 8)
#define BIT_SYSCFG_ALDN_STS			(0x00000001 << 0)
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_SYSTEM_CFG2
 * @{
 *****************************************************************************/
#define BIT_MASK_SYSCFG_ROM_INFO		(0x000000FF)
/** @} */
/** @} */

/**
  * @}
  */

/**
  * @}
  */

/* Other definations --------------------------------------------------------*/
/*******************  Macro definition for TRP_ICFG ********************/
#define SYSCFG_TRP_ICFG_STOP_IN_ROMBOOT		2
#define SYSCFG_TRP_ICFG_FLASH_LOCATION		3 /* Ameba1 used, AmebaZ not use */
#endif //_RTL8710B_SYSCFG_H_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
