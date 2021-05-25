/**
  ******************************************************************************
  * @file    rtl8711b_clk.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of clock control:
  *           - NCO32K clock
  *           - NCO8M clock
  *           - CPU clock
  *           - XTAL clock get
  *           - OSC32K clock
  *           - EXT32K clock
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _RTL8721D_CLK_H_
#define _RTL8721D_CLK_H_
  
/** @addtogroup AmebaD_Platform
  * @{
  */

/** @defgroup CLOCK
  * @brief CLOCK driver modules
  * @{
  */

/** @addtogroup CLOCK
  * @verbatim
  *****************************************************************************************
  * NCO32K
  *****************************************************************************************
  *	-RTC clock in
  *	-TIM0-TIM3 clock in
  *	-WIFI 32K clock in
  *****************************************************************************************
  * OSC32K OSC8M
  *****************************************************************************************
  *	-OSC32K is used to calibration OSC8M
  *	-OSC8M is used for LP UART when SOC suspend and close XTAL
  *****************************************************************************************
  *****************************************************************************************      
  * NCO8M
  *****************************************************************************************   
  *	-used for LP UART when SOC active
  *	-Clock in is XTAL (40MHz)
  *	-Clock out is 8MHz
  *
  *****************************************************************************************
  * HS CPU clock
  *****************************************************************************************
  *	-CLK_KM4_200M: 200MHz
  *	-CLK_KM4_100M: 100MHz
  *	-CLK_KM4_50M: 50MHz
  *	-CLK_KM4_25M: 25MHz
  *	-CLK_KM4_XTAL: XTAL
  *
  *****************************************************************************************     
  * XTAL clock
  *****************************************************************************************
  *	-Get XTAL clock from EFUSE setting:
  *	-40000000
  *	-25000000
  *	-13000000
  *	-19200000
  *	-20000000
  *	-26000000
  *	-38400000
  *	-17664000
  *	-16000000
  *	-14318000
  *	-12000000
  *	-52000000
  *	-48000000
  *	-26000000
  *	-27000000
  *	-24000000

  *****************************************************************************************    
  * EXT32K
  *****************************************************************************************    
  *	-External 32K: 32K clock from external 32k source
  *	-Internal 32K: 32K clock from internal 32K source: NCO32K
  *
  *****************************************************************************************     
  * @endverbatim
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup CLK_Exported_Constants CLK Exported Constants
  * @{
  */ 

/** @defgroup SDM32K_Cal_Type_definitions 
  * @{
  */ 
#define SDM32K_ONE_CAL			0
#define SDM32K_AUTO_CAL		1
#define SDM32K_ALWAYS_CAL		2
/**
  * @}
  */

/** @defgroup LP_CPU_CLK_definitions 
  * @{
  */ 
#define CLK_KM0_XTAL		0 /* if XTAL is 26MHz, we can use it */
#define CLK_KM0_XTALDIV2	1 /* if XTAL is 40MHz, we should use it */
#define CLK_KM0_ANA_4M		2 /* if XTAL OFF, default config */
/**
  * @}
  */

/** @defgroup HS_CPU_CLK_definitions 
  * @{
  */ 
#define CLK_KM4_200M		0
#define CLK_KM4_100M		1
#define CLK_KM4_50M			2
#define CLK_KM4_25M			3
#define CLK_KM4_XTAL		4
/**
  * @}
  */

/** @defgroup PLL_SEL_definitions 
  * @{
  */ 
#define PLL_I2S		0
#define PLL_PCM		1

/** @defgroup PLL_ClkTune_definitions 
  * @{
  */ 
#define PLL_AUTO		0
#define PLL_FASTER		1
#define PLL_SLOWER		2

/**
  * @}
  */
  
/** @defgroup OSC2M_Cal_definitions 
  * @{
  */ 
#define OSC2M_CAL_CYC_16	0
#define OSC2M_CAL_CYC_32	1
#define OSC2M_CAL_CYC_64	2
#define OSC2M_CAL_CYC_128	3
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup SDM32K_CLK_Exported_Functions SDM32K_CLK Exported Functions
  * @{
  */
_LONG_CALL_ void SDM32K_Enable(u32 AutoCalibration);
_LONG_CALL_ void SDM32K_RTCCalEnable(u32 RTCCalibration);
/**
  * @}
  */

/** @defgroup EXT32K_CLK_Exported_Functions EXT32K_CLK Exported Functions
  * @{
  */
_LONG_CALL_ void EXT32K_Cmd(u32 NewStatus);
/**
  * @}
  */

/** @defgroup CPU_CLK_Exported_Functions CPU_CLK Exported Functions
  * @{
  */
_LONG_CALL_ u32 CPU_ClkGet(u8 Is_FPGA);
_LONG_CALL_ void CPU_ClkSet(u8  CpuType);
/**
  * @}
  */

/** @defgroup XTAL_CLK_Exported_Functions XTAL_CLK Exported Functions
  * @{
  */
_LONG_CALL_ u32 XTAL_ClkGet(void);
/**
  * @}
  */

/** @defgroup NCO2M_CLK_Exported_Functions NCO2M_CLK Exported Functions
  * @{
  */
_LONG_CALL_ void NCO2M_Init(u32 clk_out_Hz);
_LONG_CALL_ void NCO2M_Cmd(u32 NewState);
/**
  * @}
  */

/** @defgroup PLL_Exported_Functions PLL Exported Functions
  * @{
  */
_LONG_CALL_ void PLL_I2S_Set(u32 new_state);
_LONG_CALL_ void PLL_PCM_Set(u32 new_state);
_LONG_CALL_ void PLL2_Set(u32 BitMask, u32 NewState);
_LONG_CALL_ void PLL3_Set(u32 BitMask, u32 NewState);
/**
  * @}
  */

/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup CLK_Register_Definitions CLK Register Definitions
 * @{
 *****************************************************************************/

/**************************************************************************//**
 * @defgroup REG_SYS_SYSPLL_CTRL2
 * @{
 *****************************************************************************/
#define BIT_SYS_SYSPLL_CK_ADC_EN		(0x00000001 << 25) /*!< Set ADC PLL EN */
/** @} */

/** @} */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/* Other definations --------------------------------------------------------*/
u32 OSC2M_Calibration(u32 cal_osc_cycles, u32 ppm_limit);
u32 OSC131K_Calibration(u32 ppm_limit);
void OSC4M_Init(void);

#endif //_RTL8721D_CLK_H_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
