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

#ifndef _RTL8710B_CLK_H_
#define _RTL8710B_CLK_H_
  
/** @addtogroup AmebaZ_Platform
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
  * CPU clock
  *****************************************************************************************
  *	-CLK_125M: 125000000
  *	-CLK_62_5M: 62500000
  *	-CLK_31_25M: 31250000
  *	-CLK_16_625M: 15625000
  *	-CLK_XTAL: XTAL
  *	-CLK_ANA_4M: 4000000
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

/** @defgroup CPU_CLK_definitions 
  * @{
  */ 
#define CLK_125M			0
#define CLK_62_5M			1
#define CLK_31_25M			2
#define CLK_16_625M			3
#define CLK_XTAL			4
#define CLK_ANA_4M			5
/**
  * @}
  */

/** @defgroup OSC32K_CLK_definitions
  * @note just used by function OSC32K_Calibration or OSC8M_Calibration
  * @{
  */ 
#define OSC32K_CALI_32KCYC_004			0
#define OSC32K_CALI_32KCYC_008			1
#define OSC32K_CALI_32KCYC_016			2
#define OSC32K_CALI_32KCYC_032			3
#define OSC32K_CALI_32KCYC_064			4
#define OSC32K_CALI_32KCYC_128			5
#define OSC32K_CALI_32KCYC_256			6
#define OSC32K_CALI_32KCYC_512			7
/**
  * @}
  */

/** @defgroup OSC8M_CLK_definitions
  * @note just used by function OSC8M_Calibration
  * @{
  */ 
#define OSC8M_8388608HZ			0
#define OSC8M_8192000HZ			1
#define OSC8M_8000000HZ			2
#define OSC8M_16777216HZ			3
/**
  * @}
  */
  
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup NCO32K_CLK_Exported_Functions NCO32K_CLK Exported Functions
  * @{
  */
_LONG_CALL_ void NCO32K_Init(u32 clk_out, u32 clk_ref, u8 calibration_cycles, u8 calibration_thrs);
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


/** @defgroup NCO8M_CLK_Exported_Functions NCO8M_CLK Exported Functions
  * @{
  */
_LONG_CALL_ void NCO8M_Init(u32 clk_ref_M, u32 clk_out_M);
_LONG_CALL_ void NCO8M_Cmd(u32 NewState);
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

/** @defgroup OSC_CLK_Exported_Functions OSC_CLK Exported Functions
  * @{
  */
_LONG_CALL_ u32 OSC8M_Calibration(u32 LOG_EN, u32 CaliCycles, u32 TargetClock);
_LONG_CALL_ void OSC32K_Calibration(u32 LOG_EN, u32 CaliCycles);
_LONG_CALL_ void OSC32K_Cmd(u32 NewStatus);
_LONG_CALL_ u32 OSC8M_Get(void);
/**
  * @}
  */

/** @defgroup ISO_CTL_Exported_Functions ISO_CTL Exported Functions
  * @{
  */
_LONG_CALL_ void ISO_Set(u32 BitMask, u32 NewState);
/**
  * @}
  */

/** @defgroup PLL_Exported_Functions PLL Exported Functions
  * @{
  */
_LONG_CALL_ void PLL0_Set(u32 BitMask, u32 NewState);
_LONG_CALL_ void PLL1_Set(u32 BitMask, u32 NewState);
_LONG_CALL_ void PLL2_Set(u32 BitMask, u32 NewState);
_LONG_CALL_ void PLL3_Set(u32 BitMask, u32 NewState);
/**
  * @}
  */

/** @defgroup XTAL_CTL_Exported_Functions XTAL_CTL Exported Functions
  * @{
  */
_LONG_CALL_ void XTAL0_Set(u32 BitMask, u32 NewState);
_LONG_CALL_ void XTAL1_Set(u32 BitMask, u32 NewState);
_LONG_CALL_ void XTAL2_Set(u32 BitMask, u32 NewState);
/**
  * @}
  */



/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup CLK_Register_Definitions CLK Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup NCO1_CLK_INFO
 * @{
 *****************************************************************************/
#define NCO1_BIT_32K_CLK_OUT_RDY			(0x00000001 << 24)
/** @} */

/**************************************************************************//**
 * @defgroup NCO1_CTRL
 * @{
 *****************************************************************************/
#define NCO1_BIT_32K_CLK_EN				(0x00000001 << 16)
#define NCO1_BIT_REF_CLK_EN				(0x00000001 << 17)
#define NCO1_BIT_CALI_CYCLES				(0x00F00000)
#define NCO1_BIT_CALI_THRS					(0x7F000000)
/** @} */

/**************************************************************************//**
 * @defgroup NCO8M
 * @{
 *****************************************************************************/
#define NCO2_BIT_OUT_CLK_EN				(0x00000001)
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_CLK_CTRL0
 * @{
 *****************************************************************************/
#define BIT_SOC_OCP_IOBUS_CK_EN		(0x00000001 <<  2)
#define BIT_SYSON_CK_EELDR_EN			(0x00000001 <<  1)
#define BIT_SYSON_CK_SYSREG_EN		(0x00000001 <<  0)
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_CLK_CTRL1
 * @{
 *****************************************************************************/
#define BIT_PESOC_EXT32K_CK_SEL		(0x00000001 << 8)
#define BIT_PESOC_OCP_CPU_CK_SEL		(0x00000007 << 4)
#define BIT_PESOC_EELDR_CK_SEL			(0x00000001 <<  0)
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_XTAL_CTRL0
 * @{
 *****************************************************************************/
#define BIT_SYS_XTAL_XQSEL_RF			(0x00000001 <<  31)
#define BIT_SYS_XTAL_GATED_OK0		(0x00000001 <<  30)
#define BIT_SYS_XTAL_SC_XO(x)			(((x) & 0x0000003f) << 24)
#define BIT_SYS_XTAL_SC_XI(x)			(((x) & 0x0000003f) << 18)
#define BIT_SYS_XTAL_GMN(x)			(((x) & 0x0000001f) << 13)
#define BIT_SYS_XTAL_GMP(x)				(((x) & 0x0000001f) << 8)
#define BIT_SYS_XTAL_GSPL_EN			(0x00000001 <<  4) /*!< 1: Gate PLL ref clock from XTAL ;   0: not gated */
#define BIT_SYS_XTAL_EN					(0x00000001 <<  1)
#define BIT_SYS_XTAL_BGMB_EN			(0x00000001 <<  0)
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_XTAL_CTRL1
 * @{
 *****************************************************************************/
#define BIT_SYS_XTAL_DELAY_SYSPLL		(0x00000001 <<  25)
#define BIT_SYS_XTAL_DELAY_USB		(0x00000001 <<  24)
#define BIT_SYS_XTAL_DELAY_WLAFE		(0x00000001 <<  23)
#define BIT_SYS_XTAL_EN_AAC_GM		(0x00000001 <<  21)
#define BIT_SYS_XTAL_EN_AAC_PEAKDET	(0x00000001 <<  20)
#define BIT_SYS_XTAL_AGPIO(x)			(((x) & 0x00000001) << 17)
#define BIT_SYS_XTAL_DRV_SYSPLL(x)		(((x) & 0x00000003) << 15)
#define BIT_SYS_XTAL_GATE_SYSPLL		(0x00000001 <<  14)
#define BIT_SYS_XTAL_DRV_USB(x)		(((x) & 0x00000003) << 12)
#define BIT_SYS_XTAL_GATE_USB			(0x00000001 <<  11)
#define BIT_SYS_XTAL_DRV_WLAFE(x)		(((x) & 0x00000003) << 9)
#define BIT_SYS_XTAL_GATE_WLAFE		(0x00000001 <<  8)
#define BIT_SYS_XTAL_DRV_RF2(x)		(((x) & 0x00000003) << 6)
#define BIT_SYS_XTAL_GATE_RF2			(0x00000001 <<  5)
#define BIT_SYS_XTAL_DRV_RF1(x)		(((x) & 0x00000003) << 3)
#define BIT_SYS_XTAL_GATE_RF1			(0x00000001 <<  2)
#define BIT_SYS_XTAL_LDO(x)				(((x) & 0x00000003) << 0)
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_SYSPLL_CTRL0
 * @{
 *****************************************************************************/
#define BIT_SYS_SYSPLL_CKTST_EN		(0x00000001 <<  22)
#define BIT_SYS_SYSPLL_MONCK_SEL(x)	(((x) & 0x00000007) << 19)
#define BIT_SYS_SYSPLL_CP_IOFFSET(x)	(((x) & 0x0000001f) << 14)
#define BIT_SYS_SYSPLL_FREF_EDGE		(0x00000001 <<  9)
#define BIT_SYS_SYSPLL_EN				(0x00000001 <<  1)
#define BIT_SYS_SYSPLL_LVPC_EN			(0x00000001 <<  0)
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_SYSPLL_CTRL1
 * @{
 *****************************************************************************/
#define BIT_SYS_SYSPLL_CL200M_SEL		(0x00000001 << 17) /*!< 1:200MHz, 0:166.666MHz */
#define BIT_SYS_SYSPLL_CK500K_SEL		(0x00000001 << 15) /*!< 1:external source 0:PLL */
#define BIT_SYS_SYSPLL_CK200M_EN		(0x00000001 << 14) /*!< 1:  enable CK200M */
#define BIT_SYS_SYSPLL_CKSDR_EN		(0x00000001 << 13) /*!< 1: enable CK_SDRAM */
#define BIT_SYS_SYSPLL_CKSDR_DIV		(0x00000003 << 11) /*!< SDR PLL select: 00/01/10/11  no clock/25M/50M/100M */
#define BIT_SYS_SYSPLL_CK24P576_EN	(0x00000001 << 10) /*!< 1:enable CK24.576M PLL */
#define BIT_SYS_SYSPLL_CK22P5792_EN	(0x00000001 << 9) /*!< 1: enable CK22.5792M PLL */
#define BIT_SYS_SYSPLL_CK83P33M_EN	(0x00000001 << 8) /*!< 1: enable CK83.33M PLL */
#define BIT_SYS_SYSPLL_CK_PS_EN		(0x00000001 << 7) /*!< reg_ps_en Enable phase shift */
#define BIT_SYS_SYSPLL_CK_PS_SEL		(0x00000007 << 4) /*!< "decide clock phase when reg_ps_enb = 1000/001бн/111: phase 0, 45бн315 */
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_SYSPLL_CTRL2
 * @{
 *****************************************************************************/
#define BIT_SYS_SYSPLL_CK_ADC_EN		(0x00000001 << 25) /*!< Set ADC PLL EN */
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_SYSPLL_CTRL3
 * @{
 *****************************************************************************/
#define BIT_DIV_SEL						(0x000001FF << 6) /*!< The period of signal CK500M_SYNC/CK500M_PS_SYNC is: (r_DIV_SEL[8:0] + 2) cycles of 500MHz clock */
#define BIT_PHASE_SEL					(0x00000007 << 3) /*!< Phase selection for CK_500M_PS 0: 0,  1: 45, 2: 90, 3: 135, 4: 180,  5: 225, 6: 270, 7: 315 */
#define BIT_EN_CK_500M_PS				(0x00000001 <<  2) /*!< 1: enable PLL 500M phase shift clock; 0: disable, Enable this bit when use FLASH_CalibrationPhase */
#define BIT_EN_CK_500M					(0x00000001 <<  1) /*!< 1: enable PLL 500M clock; 0: disable, HW auto enable this bit */
/** @} */

/**************************************************************************//**
 * @defgroup REG_OSC32K_CTRL
 * @{
 *****************************************************************************/
#define BIT_SYS_OSC32K_POW_CKGEN		(0x00000001 << 0) /*!< 1: enable OSC 32K clock generator; 0: turn off OSC 32K clock gen */	
#define BIT_SYS_OSC32K_OUT_SEL		(0x00000001 << 1) /*!< 1: New Calibration Mode(REGUOUT=OSCIN=8M) 0: Original Calibration Mode (REGUOUT=OSCIN=32k) */
#define BIT_SYS_OSC32K_RESISTOR_COM	(0x00000003 << 2) /*!< Compensate resistor control */
#define BIT_SYS_OSC32K_CKE_8M			(0x00000001 << 4) /*!< 1: osc 8m clk is enabled */
#define BIT_SYS_OSC32K_CKE_FREF		(0x00000001 << 5) /*!< 1: referece 25M clk is enabled */
#define BIT_32K_BIAS_CURRENT			(0x0000FFFF << 16) /*! Bias current control */
/** @} */

/**************************************************************************//**
 * @defgroup REG_OSC32K_REG_CTRL0
 * @{
 *****************************************************************************/
#define BIT_32K_REG_INDIRT_CMD		(0x00000001 << 23) /*!< 1: write command, 0: read command */
#define BIT_32K_REG_INDIRT_ADDR		(0x0000003F << 16)
#define BIT_32K_REG_INDIRT_WDATA		(0x0000FFFF << 0)
/** @} */

/**************************************************************************//**
 * @defgroup REG_OSC32K_REG_CTRL0
 * @{
 *****************************************************************************/
#define BIT_32K_REG_INDIRT_RDATA		(0x0000FFFF << 0) /*!< 32K OSC register indirect read data */
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
extern u32 NCO32K_Enable;
extern u32 OSC8M_CLOCK_GLB;

/**
  * @brief  Configures NCO 32K monitor function
  * @param  NewStatus: Disable/Enable
  * @note you should disable monitor when XTAL 40M close
  */
static inline void NCO32K_Monitor(u32 NewStatus)
{
	if (NewStatus == ENABLE) {
		NCO32k->CTRL |= NCO1_BIT_REF_CLK_EN;
	} else {
		NCO32k->CTRL &= ~NCO1_BIT_REF_CLK_EN;
	}
}
#endif //_RTL8710B_CLK_H_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
