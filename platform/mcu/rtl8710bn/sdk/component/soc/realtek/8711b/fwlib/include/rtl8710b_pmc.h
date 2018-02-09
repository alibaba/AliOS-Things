/**
  ******************************************************************************
  * @file    rtl8710b_pmc.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the soc power management circut:
  *           - wakeup timer
  *           - wakeup pin
  *           - sleep option
  *           - sleep mode
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _RTL8710B_PMC_H_
#define _RTL8710B_PMC_H_

/** @addtogroup AmebaZ_Platform
  * @{
  */

/** @defgroup PMC
  * @brief PMC driver modules
  * @{
  */

/** @addtogroup PMC
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * we support  following soc power save functions:
  *		- sleep clock gating
  *		- sleep power gating
  *		- deep standby
  *		- deep sleep
  *
  *****************************************************************************************
  * sleep power gating
  *****************************************************************************************
  * following functions can be used when power gating:
  *		-UART0/UART1
  *		-TIM4/TIM5
  *		-RTC
  *		-WIFI
  *		-SDIO
  *		-USB   
  *		-I2C0/I2C1    
  *		-ADC  
  *		-GPIO
  *		-REGU timer
  *		-normal wakepin
  *		-ANA timer
  * following functions will be closed when power gating:
  *		-UART2 LOGUART
  *		-TIM0-TIM3
  *		-SPIC flash
  *
  *****************************************************************************************
  * deep standby
  *****************************************************************************************
  * following functions can be used when deep standby:
  *		-RTC
  *		-REGU timer
  *		-normal wakepin 
  *		-ANA timer
  *
  *****************************************************************************************
  * deep sleep
  *****************************************************************************************
  * following functions can be used when deep standby:
  *		-REGU timer
  *		-REGU wakepin 
  *
  *****************************************************************************************
  * wakepin (A18/A5/A22/A23: mux normal wakepin and REGU wakepin)
  *****************************************************************************************
  * normal wakepin:
  *		-SLP_CG
  *		-SLP_PG
  *		-STDBY
  * REGU wakepin:
  *		-just used in DSLP (1.2V closed)
  *		-just support high acive, so this pin should pull low on your board 
  *
  *****************************************************************************************
  *****************************************************************************************
  * SLP & SNZ power option
  *****************************************************************************************
  * BIT_SYSON_PMOPT_SLP_EN_SWR & BIT_SYSON_PMOPT_SNZ_EN_SWR
  *		-we have two 1.2V LDO
  *			-BIG LDO: SWR mode or LDO mode (can config )
  *			-LITTLE LDO: a little 1.2v LDO
  *		-BIT_SYSON_PMOPT_SLP_EN_SWR
  *			-ENABLE/DISABLE BIG LDO when SLP
  *		BIT_SYSON_PMOPT_SNZ_EN_SWR
  *			-ENABLE/DISABLE BIG LDO when SNZ, WIFI & ADC need open BIG LDO when SNZ
  *
  * BIT_SYSON_PMOPT_SLP_EN_PWM & BIT_SYSON_PMOPT_SNZ_EN_PWM
  *		-BIT_SYSON_PMOPT_SLP_EN_PWM
  *			-ENABLE/DISABLE LDO heavy loading current mode when SLP
  *		-BIT_SYSON_PMOPT_SNZ_EN_PWM
  *			-ENABLE/DISABLE heavy loading current mode when SNZ, WIFI & ADC need heavy loading when SNZ
  *
  * BIT_SYSON_PMOPT_SLP_XTAL_EN & BIT_SYSON_PMOPT_SNZ_XTAL_EN
  *		-WIFI and SOC both need XTAL when work, 
  *		-but WIFI have individual option to control XTAL, so BIT_SYSON_PMOPT_SNZ_XTAL_EN not needed
  *
  * BIT_SYSON_PMOPT_SLP_SYSPLL_EN & BIT_SYSON_PMOPT_SNZ_SYSPLL_EN
  *		-WIFI and SOC both have individual PLL, here is SOC 500M PLL
  *		-So BIT_SYSON_PMOPT_SNZ_SYSPLL_EN not needed
  *
  * BIT_SYSON_SNFEVT_WIFI_MSK = 1 & BIT_SYSON_BYPASS_SNZ_SLP = 1
  *		- after OS suspend, platform will enter SNZ and close CPU, then platform enter sleep mode when WIFI 32K
  *		- BIT_SYSON_PMOPT_SNZ_EN_SOC should never open, or CPU will not close when platform will enter SNZ
  *
  * BIT_SYSON_SNFEVT_WIFI_MSK = 1 & BIT_SYSON_BYPASS_SNZ_SLP = 0 (not use this config)
  *		- after OS suspend, platform will enter sleep mode & close CPU after WIFI 32K
  *****************************************************************************************
  * @endverbatim
  */


/* Exported constants --------------------------------------------------------*/

/** @defgroup PMC_Exported_Constants PMC Exported Constants
  * @{
  */ 

/** @defgroup PMC_WakePin_definitions 
  * @{
  */ 
#define	WAKEUP_BY_GPIO_NONE				((u32)(0x00000000))
#define	WAKEUP_BY_GPIO_WAKEUP0_LOW	((u32)(0x00000001))		//PA18
#define	WAKEUP_BY_GPIO_WAKEUP0_HIG		((u32)(0x00000101))
#define	WAKEUP_BY_GPIO_WAKEUP1_LOW	((u32)(0x00000001 << 1))	//PA5
#define	WAKEUP_BY_GPIO_WAKEUP1_HIG		((u32)(0x00000101 << 1))
#define	WAKEUP_BY_GPIO_WAKEUP2_LOW	((u32)(0x00000001 << 2))	//PA22
#define	WAKEUP_BY_GPIO_WAKEUP2_HIG		((u32)(0x00000101 << 2))
#define	WAKEUP_BY_GPIO_WAKEUP3_LOW	((u32)(0x00000001 << 3))	//PA23
#define	WAKEUP_BY_GPIO_WAKEUP3_HIG		((u32)(0x00000101 << 3))
/**
  * @}
  */


/** @defgroup SOCPS_PS_Mode_definitions 
  * @{
  */ 
#define	SOCPS_MODE_DSLP					((u32)(0x00000001 << 0))
#define	SOCPS_MODE_DSTBY					((u32)(0x00000001 << 1))
#define	SOCPS_MODE_SLP					((u32)(0x00000001 << 2))
/**
  * @}
  */

/** @defgroup SOCPS_PS_ANA_Clk_definitions 
  * @{
  */
#define	ANACK_250K							((u32)0x00000000)
#define	ANACK_4M							((u32)0x00000001)
/**
  * @}
  */

/** @defgroup SOCPS_PS_Calibration_Clk_definitions 
  * @{
  */
#define	ANACK								((u32)0x00000000)
#define	A33CK								((u32)0x00000001)
/**
  * @}
  */

/** @defgroup SOCPS_PS_Wakeup_Pin_definitions 
  * @{
  */
#define	WAKUP_0							((u32)0x00000000)/*!< _PA_18 */
#define	WAKUP_1							((u32)0x00000001)/*!< _PA_5 */
#define	WAKUP_2							((u32)0x00000002)/*!< _PA_22 */
#define	WAKUP_3							((u32)0x00000003)/*!< _PA_23 */
/**
  * @}
  */

/** @defgroup SOCPS_PS_Wakeup_Pin_Mask_definitions 
  * @{
  */
#define	WAKUP_0_MASK						((u32)(0x00000001 << 0))/*!< _PA_18 */
#define	WAKUP_1_MASK						((u32)(0x00000001 << 1))/*!< _PA_5 */
#define	WAKUP_2_MASK						((u32)(0x00000001 << 2))/*!< _PA_22 */
#define	WAKUP_3_MASK						((u32)(0x00000001 << 3))/*!< _PA_23 */
#define	IS_WAKEPIN_MASK(MASK)			(((MASK) & 0x0000000F) != 0)
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup PMC_Exported_Functions PMC Exported Functions
  * @{
  */
_LONG_CALL_ void SOCPS_BackupCPUClk(void);
_LONG_CALL_ void SOCPS_RestoreCPUClk(void);

_LONG_CALL_ void SOCPS_BootFromPS(u32 NewStatus);
_LONG_CALL_ void SOCPS_TrapPin(u32 NewStatus);
_LONG_CALL_ void SOCPS_ANACKSel(u32 ANACLK);
_LONG_CALL_ void SOCPS_SetWakeEvent(u32 Option, u32 NewStatus);
_LONG_CALL_ void SOCPS_ClearWakeEvent(void);
_LONG_CALL_ void SOCPS_WakePinsCtrl(u32 GpioOption);
_LONG_CALL_ void SOCPS_WakePinCtrl(u32 Index, u32 Polarity);
_LONG_CALL_ void SOCPS_SetANATimer(u32 SDuration, u32 NewStatus);
_LONG_CALL_ void SOCPS_SetReguWakepin(void);
_LONG_CALL_ void SOCPS_SetReguTimer(u32 SDuration, u32 CalibData);
_LONG_CALL_ void SOCPS_PWRMode(u32 pwrmode, u32 snz_bypss);
_LONG_CALL_ void SOCPS_PWROption(u32 pwrmgt_option);
_LONG_CALL_ void SOCPS_PWROptionExt(u32 pwrmgt_option);
_LONG_CALL_ void SOCPS_SNZMode(u32 SnzMask, u32 NewStatus);

_LONG_CALL_ u32 SOCPS_CLKCal(u32 ClkSel);

_LONG_CALL_ void SOCPS_DeepStandby(void);
_LONG_CALL_ void SOCPS_DeepSleep(void);
_LONG_CALL_ void SOCPS_SleepPG(void);
_LONG_CALL_ void SOCPS_SetReguWakepins(u32 PinMask);
_LONG_CALL_ void SOCPS_ReguTimerCmd(u32 NewStatus);
_LONG_CALL_ void SOCPS_WakePinDebounce(u32 Index, u32 Status);

_LONG_CALL_ void SOCPS_DeepStandby_RAM(void);
_LONG_CALL_ void SOCPS_SleepPG_RAM(void);
_LONG_CALL_ void SOCPS_DeepSleep_RAM(void);
_LONG_CALL_ void SOCPS_FlashPin_PullUp_RAM(u32 NewStatus, u32 ChipIndex);
void SOCPS_SetReguWakepins_RAM(u32 PinMask);
void SOCPS_ReguTimerCmd_RAM(u32 NewStatus);
void SOCPS_WakePinDebounce_RAM(u32 Index, u32 Status);
void SOCPS_SetReguTimer_RAM(u32 SDuration, u32 CalibData);
void SOCPS_SleepCG(void);
int SOCPS_DsleepWakeReason(void);
/**
  * @}
  */


/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup PMC_Register_Definitions PMC Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup REG_SYS_ANA_TIM_CTRL
 * @{
 *****************************************************************************/
#define BIT_SYS_ANACK_TU_TIME(x)				(((x) & 0x0000003f) << 16)
#define BIT_SYS_DSBYCNT_EN						(0x00000001 << 15)
#define BIT_SYS_DSTDY_TIM_SCAL(x)				(((x) & 0x0000000f) << 8)
#define BIT_SYS_DSTBY_TIM_PERIOD(x)			(((x) & 0x000000ff) << 0)
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_DSLP_TIM_CTRL
 * @{
 *****************************************************************************/
#define BIT_SYS_REGU_ASIF_EN(x)				(((x) & 0x000000ff) << 24)
#define BIT_SYS_REGU_ASIF_THP_DA(x)			(((x) & 0x00000003) << 20)
#define BIT_SYS_REGU_ASIF_TPD_CK(x)			(((x) & 0x00000003) << 18)
#define BIT_SYS_REGU_ASIF_TSP_DA(x)			(((x) & 0x3) << 16)
#define BIT_SYS_REGU_ASIF_POLL					(0x00000001 << 15)
#define BIT_SYS_REGU_ASIF_MODE				(0x00000001 << 14)
#define BIT_SYS_REGU_ASIF_WE					(0x00000001 << 12)
#define BIT_SYS_REGU_ASIF_AD(x)				(((x) & 0x0000000f) << 8)
#define BIT_SYS_REGU_ASIF_WD(x)				(((x) & 0x000000ff) << 0)
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_DSLP_TIM_CAL_CTRL
 * @{
 *****************************************************************************/
#define BIT_SYS_DSLP_TIM_EN					(0x00000001 << 24)
#define BIT_SYS_DSLP_TIM_PERIOD(x)				(((x) & 0x7fffff) << 0)
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_GPIO_DSTBY_WAKE_CTRL0
 * @{
 *****************************************************************************/
#define BIT_SYS_WAKEPIN3_WEVENT_STS			(0x00000001 << 27)
#define BIT_SYS_WAKEPIN2_WEVENT_STS			(0x00000001 << 26)
#define BIT_SYS_WAKEPIN1_WEVENT_STS			(0x00000001 << 25)
#define BIT_SYS_WAKEPIN0_WEVENT_STS			(0x00000001 << 24)
#define BIT_SYS_WAKEPIN3_PULL_CTRL_EN		(0x00000001 << 19)
#define BIT_SYS_WAKEPIN2_PULL_CTRL_EN		(0x00000001 << 18)
#define BIT_SYS_WAKEPIN1_PULL_CTRL_EN		(0x00000001 << 17)
#define BIT_SYS_WAKEPIN0_PULL_CTRL_EN		(0x00000001 << 16)
#define BIT_SYS_WAKEPIN3_WINT_MODE			(0x00000001 << 11)
#define BIT_SYS_WAKEPIN2_WINT_MODE			(0x00000001 << 10)
#define BIT_SYS_WAKEPIN1_WINT_MODE			(0x00000001 << 9)
#define BIT_SYS_WAKEPIN0_WINT_MODE			(0x00000001 << 8)
#define BIT_SYS_WAKEPIN3_PIN_EN				(0x00000001 << 3)
#define BIT_SYS_WAKEPIN2_PIN_EN				(0x00000001 << 2)
#define BIT_SYS_WAKEPIN1_PIN_EN				(0x00000001 << 1)
#define BIT_SYS_WAKEPIN0_PIN_EN				(0x00000001 << 0)

#define MARSK_WAKEPIN0_HIGH_WAKE	(BIT_SYS_WAKEPIN0_PIN_EN | \
										BIT_SYS_WAKEPIN0_WINT_MODE | \
										BIT_SYS_WAKEPIN0_PULL_CTRL_EN | \
										BIT_SYS_WAKEPIN0_WEVENT_STS)

#define MARSK_WAKEPIN0_LOW_WAKE		(BIT_SYS_WAKEPIN0_PIN_EN | \
										BIT_SYS_WAKEPIN0_PULL_CTRL_EN | \
										BIT_SYS_WAKEPIN0_WEVENT_STS)
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_GPIO_DSTBY_WAKE_CTRL1
 * @{
 *****************************************************************************/										
#define BIT_SYS_BTCLKDET_MODE					(0x00000001 << 27)
#define BIT_SYS_BTCLKDET_DEBOUNCE_EN			(0x00000001 << 26)
#define BIT_SYS_BTCLKDET_TIM_SCAL				(0x00000003 << 24)

#define BIT_SYS_WAKEPIN3_SHTDN_N				(0x00000001 << 19)
#define BIT_SYS_WAKEPIN2_SHTDN_N				(0x00000001 << 18)
#define BIT_SYS_WAKEPIN1_SHTDN_N				(0x00000001 << 17)
#define BIT_SYS_WAKEPIN0_SHTDN_N				(0x00000001 << 16)
#define BIT_SYS_WINT_DEBOUNCE_TIM_SCAL(x)	(((x) & 0x3) << 8)

#define BIT_SYS_WAKEPIN3_WINT_DEBOUNCE_EN	(0x00000001 << 3)
#define BIT_SYS_WAKEPIN2_WINT_DEBOUNCE_EN	(0x00000001 << 2)
#define BIT_SYS_WAKEPIN1_WINT_DEBOUNCE_EN	(0x00000001 << 1)
#define BIT_SYS_WAKEPIN0_WINT_DEBOUNCE_EN	(0x00000001 << 0)
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_SLP_WAKE_EVENT_MSK0
 * @{
 *****************************************************************************/	
#define BIT_SYSON_WEVT_BOR2_MSK				(0x00000001 << 30)	/*!< BOR2 wakeup */

#define BIT_SYSON_WEVT_GPIO_DSTBY_MSK		(0x00000001 << 29)	/*!< 1: enable Wakepin Wakeup DSTBY event */
#define BIT_SYSON_WEVT_A33_AND_A33GPIO_MSK	(0x00000001 << 28)	/*!< 1: enable REGU A33(deepsleep mode, A33 Timer & A33 wakepin) Wakeup event */
#define BIT_SYSON_WEVT_ADC_MSK				(0x00000001 << 26)	/*!< 1: enable  ADC Wakeup  event */

#define BIT_SYSON_WEVT_SDIO_MSK				(0x00000001 << 14)	/*!< 1: enable  SDIO Wakeup  event */
#define BIT_SYSON_WEVT_RTC_MSK				(0x00000001 << 13)	/*!< 1: enable RTC Wakeup  event */
#define BIT_SYSON_WEVT_UART1_MSK				(0x00000001 << 12)	/*!< 1: enable UART1  Wakeup  event */
#define BIT_SYSON_WEVT_UART0_MSK				(0x00000001 << 11)	/*!< 1: enable UART0  Wakeup  event */
#define BIT_SYSON_WEVT_I2C1_MSK				(0x00000001 << 10)	/*!< 1: enable I2C1  Wakeup  event */
#define BIT_SYSON_WEVT_I2C0_MSK				(0x00000001 << 9)	/*!< 1: enable I2C0  Wakeup  event */

#define BIT_SYSON_WEVT_WLAN_MSK				(0x00000001 << 8)	/*!< 1: enable  WLAN Wakeup  event */

#define BIT_SYSON_WEVT_I2C1_ADDRMATCH_MSK	(0x00000001 << 7)	/*!< 1:enable wakeup event of I2C1 RX address match */
#define BIT_SYSON_WEVT_I2C0_ADDRMATCH_MSK	(0x00000001 << 6)	/*!< 1:enable wakeup event of I2C0 RX address match */

#define BIT_SYSON_WEVT_USB_MSK				(0x00000001 << 5)	/*!< 1: enable wakeup event of usb resume from suspend mode  */
#define BIT_SYSON_WEVT_GPIO_MSK				(0x00000001 << 4)	/*!< 1: enable GPIO  Wakeup  event */
#define BIT_SYSON_WEVT_CHIP_EN_MSK			(0x00000001 << 3)	/*!< 1: enable chip en wakeup  event */
#define BIT_SYSON_WEVT_OVER_CURRENT_MSK		(0x00000001 << 2)	/*!< 1: enable OCP wakeup  event, REGU OVER_CURRENT */
#define BIT_SYSON_WEVT_GTIM_MSK				(0x00000001 << 1)	/*!< 1: enable Gtimer 4/5 Wakeup SYSON event */
#define BIT_SYSON_WEVT_SYSTIM_MSK			(0x00000001 << 0)	/*!< 1: enable SYS Timer(ANA Timer, deepstandby mode) Wakeup SYSON event */
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_SLP_WAKE_EVENT_STATUS0
 * @{
 *****************************************************************************/
#define BIT_SYSON_WEVT_BOR2_STS				(0x00000001 << 30)

#define BIT_SYSON_WEVT_GPIO_DSTBY_STS		(0x00000001 << 29)
#define BIT_SYSON_WEVT_A33_STS				(0x00000001 << 28)
#define BIT_SYSON_WEVT_ADC_STS				(0x00000001 << 26)

#define BIT_SYSON_WEVT_SDIO_STS				(0x00000001 << 14)
#define BIT_SYSON_WEVT_RTC_STS				(0x00000001 << 13)
#define BIT_SYSON_WEVT_UART1_STS				(0x00000001 << 12)
#define BIT_SYSON_WEVT_UART0_STS				(0x00000001 << 11)
#define BIT_SYSON_WEVT_I2C1_STS				(0x00000001 << 10)
#define BIT_SYSON_WEVT_I2C0_STS				(0x00000001 << 9)

#define BIT_SYSON_WEVT_WLAN_STS				(0x00000001 << 8)

#define BIT_SYSON_WEVT_I2C1_ADDRMATCH_STS	(0x00000001 << 7)
#define BIT_SYSON_WEVT_I2C0_ADDRMATCH_STS	(0x00000001 << 6)

#define BIT_SYSON_WEVT_USB_STS				(0x00000001 << 5)
#define BIT_SYSON_WEVT_GPIO_STS				(0x00000001 << 4)
#define BIT_SYSON_WEVT_CHIP_EN_STS			(0x00000001 << 3)
#define BIT_SYSON_WEVT_OVER_CURRENT_STS		(0x00000001 << 2)
#define BIT_SYSON_WEVT_GTIM_STS				(0x00000001 << 1)
#define BIT_SYSON_WEVT_SYSTIM_STS			(0x00000001 << 0)
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_SNF_WAKE_EVENT_MSK0
 * @{
 *****************************************************************************/
#define BIT_SYSON_SNFEVT_WIFI_MSK				(0x00000001 << 1) /*!< 1: enable wlan power on wakeup event interrupt; 0: Disable */
#define BIT_SYSON_SNFEVT_ADC_MSK				(0x00000001 << 0) /*!< 1: enable ADC  Wakeup SYSON event from GTIM3 interrupt under oneshot mode ; 0: disable the event to wakeup system */
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_SNF_WAKE_EVENT_STATUS
 * @{
 *****************************************************************************/
#define BIT_SYSON_SNFEVT_WIFI_STS				(0x00000001 << 1) /*!< 1: indicate wlan power on wakeup status */
#define BIT_SYSON_SNFEVT_ADC_STS				(0x00000001 << 0) /*!< 1: indicate ADC wakeup  event  status */
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_PWRMGT_CTRL
 * @{
 *****************************************************************************/
#define BIT_SYSON_PMCOPT_BKPOFIMK2			(0x00000001 << 18) /*!< Enable wakeup interrupt to break power off flow during power off stage2 */
#define BIT_SYSON_PMCOPT_BKPOFIMK1			(0x00000001 << 17) /*!< Enable wakeup interrupt to break power off flow during power off stage1 */
#define BIT_SYSON_PMCOPT_BKPOFIMK0			(0x00000001 << 16) /*!< Enable wakeup interrupt to break power off flow during power off stage0 */


#define BIT_SYSON_BYPASS_SNZ_SLP				(0x00000001 << 8) /*!< If WIFI WOWLAN open we should open it, or SOC will sleep when WIFI active */
#define BIT_SYSON_PM_CMD_SLP					(0x00000001 << 2) /*!< 1: command SYSON SM to enter sleep state, when PMC finishes the process, this bit will be auto clear to 0 */
#define BIT_SYSON_PM_CMD_DSTBY				(0x00000001 << 1) /*!< 1: command SYSON SM to enter deep standby state,when PMC finishes the process, this bit will be auto clear to 0 */
#define BIT_SYSON_PM_CMD_DSLP					(0x00000001 << 0) /*!< 1: command SYSON SM to enter deep sleep state,when PMC finishes the process, this bit will be auto clear to 0 */
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_PWRMGT_OPTION
 * @{
 *****************************************************************************/
#define BIT_SYSON_PMOPT_NORM_SYSCLK_SEL		(0x00000001 << 30) /*!< 1: select system PLL clock as CPU clock when enter into active mode; 0: select ANA clock */
#define BIT_SYSON_PMOPT_NORM_SYSPLL_EN		(0x00000001 << 29) /*!< 1: enable syspem PLL when enter into active mode; 0: disable SYSPLL */
#define BIT_SYSON_PMOPT_NORM_XTAL_EN		(0x00000001 << 28) /*!< 1: enable XTAL when enter into active mode; 0: disable XTAL */
#define BIT_SYSON_PMOPT_NORM_EN_PWM			(0x00000001 << 26) /*!< 1: power enable SWR/LDO output heavy loading current mode when enter into power mode */

#define BIT_SYSON_PMOPT_SNZ_MEM2_EN			(0x00000001 << 24) /*!< 1: enable MEM2 when enter into snooze mode */
#define BIT_SYSON_PMOPT_SNZ_MEM1_EN			(0x00000001 << 23) /*!< 1: enable MEM1 when enter into snooze mode */
#define BIT_SYSON_PMOPT_SNZ_MEM0_EN			(0x00000001 << 22) /*!< 1: enable MEM0 when enter into snooze mode */
#define BIT_SYSON_PMOPT_SNZ_SYSPLL_EN		(0x00000001 << 21) /*!< 1: enable syspem PLL when enter into snooze mode, wifi not use, adc use */
#define BIT_SYSON_PMOPT_SNZ_XTAL_EN			(0x00000001 << 20) /*!< 1: enable XTAL when enter into snooze mode, wifi not use, adc use */
#define BIT_SYSON_PMOPT_SNZ_EN_SOC			(0x00000001 << 19) /*!< 1: power enable SOC platform when enter into snooze mode */
#define BIT_SYSON_PMOPT_SNZ_EN_PWM			(0x00000001 << 18) /*!< 1: power enable SWR/LDO output heavy loading current mode when enter into snooze mode, wifi use it when close BIT_SYSON_PMOPT_SLP_EN_PWM*/
#define BIT_SYSON_PMOPT_SNZ_EN_SWR			(0x00000001 << 17) /*!< 1: power enable SWR/LDO 1.2V when enter into snooze mode, wifi use it when close BIT_SYSON_PMOPT_SLP_EN_SWR*/

#define BIT_SYSON_PMOPT_SLP_MEM2_EN			(0x00000001 << 16) /*!< 1: enable MEM2 when enter into sleep mode, 64k */
#define BIT_SYSON_PMOPT_SLP_MEM1_EN			(0x00000001 << 15) /*!< 1: enable MEM1 when enter into sleep mode, 64k */
#define BIT_SYSON_PMOPT_SLP_MEM0_EN			(0x00000001 << 14) /*!< 1: enable MEM0 when enter into sleep mode, 128k */
#define BIT_SYSON_PMOPT_SLP_SYSPLL_EN		(0x00000001 << 13) /*!< 1: enable syspem PLL when enter into sleep mode; 0: disable SYSPLL */
#define BIT_SYSON_PMOPT_SLP_XTAL_EN			(0x00000001 << 12) /*!< 1: enable XTAL when enter into sleep mode; 0: disable XTAL */
#define BIT_SYSON_PMOPT_SLP_EN_SOC			(0x00000001 << 11) /*!< 1: power enable SOC platform when enter into sleep mode; 0: power off SoC domain */
#define BIT_SYSON_PMOPT_SLP_EN_PWM			(0x00000001 << 10) /*!< 1: power enable SWR/LDO output heavy loading current mode when enter into sleep mode, wifi on can not */
#define BIT_SYSON_PMOPT_SLP_EN_SWR			(0x00000001 << 9) /*!< 1: power enable BIG 1.2V LDO (SWR/LDO can cfg)  when enter into sleep mode, just keep little LDO; wifi on can not */
#define BIT_SYSON_PMOPT_SLP_LPLDO_SEL		(0x00000001 << 8) /*!< V12H LDO operation mode selection for sleep, 1: normal, 0: standby(low current mode), always-on power is here */

#define BIT_SYSON_PMOPT_DSTBY_LPLDO_SEL		(0x00000001 << 0) /*!< V12H LDO operation mode selection for deep standby, 1: normal, 0: standby */

/* 0x74000100 */
#define BIT_SYSON_PMOPT_MASK		(BIT_SYSON_PMOPT_NORM_EN_PWM | \
									BIT_SYSON_PMOPT_NORM_XTAL_EN | \
									BIT_SYSON_PMOPT_NORM_SYSPLL_EN | \
									BIT_SYSON_PMOPT_NORM_SYSCLK_SEL | \
									BIT_SYSON_PMOPT_SNZ_EN_SWR | \
									BIT_SYSON_PMOPT_SNZ_EN_PWM | \
									BIT_SYSON_PMOPT_SNZ_MEM0_EN | \
									BIT_SYSON_PMOPT_SNZ_MEM1_EN | \
									BIT_SYSON_PMOPT_SNZ_MEM2_EN)
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_PWRMGT_OPTION_EXT
 * @{
 *****************************************************************************/									
#define BIT_SYSON_PMOPT_SLP_BYPS_WL			(0x00000001 << 4) /*!< 1: CM4 can enter sleep mode with no  regard of wlan's power state */
#define BIT_SYSON_PMOPT_SLP_REDUCE_VOL		(0x00000001 << 3) /*!< 1: decrease SWR/LDO 1.2V voltage level when enter into sleep mode(decrease digital power); */
#define BIT_SYSON_PMOPT_SLP_ANACK_SEL		(0x00000001 << 2) /*!< 1: set ANA clock to 4MHz when enter into power mode; */
#define BIT_SYSON_PMOPT_SLP_ANACK_EN			(0x00000001 << 1) /*!< 1: enable syson register clock when enter into sleep mode; */
#define BIT_SYSON_PMOPT_SLP_SWR_ADJ			(0x00000001 << 0) /*!< 1: decrease VD12H 1.2V voltage level when enter into sleep mode(decrease always on power, should better not use); */
/** @} */

/**************************************************************************//**
 * @defgroup REG_SYS_DSLP_WEVENT
 * @{
 *****************************************************************************/	
#define BIT_SYSON_DSLP_GPIO					(0x00000001 << 2) /*!< REGU GPIO wake33 event */
#define BIT_SYSON_DSLP_WTIMER33				(0x00000001 << 0) /*!< REGU Timer33 wake event */
/** @} */
/** @} */

/**
  * @}
  */

/**
  * @}
  */

/* Other Definitions --------------------------------------------------------*/
#define REG_VDR_ANACK_CAL_CTRL 0xA0
#define NVIC_ICPR0_PS_MASK 0xFFFFFFFF

#ifdef CONFIG_CHIP_A_CUT
#define SOCPS_SET_REGUTIMER		SOCPS_SetReguTimer_RAM
#else
#define SOCPS_SET_REGUTIMER		SOCPS_SetReguTimer
#endif


#endif //_RTL8710B_PMC_H_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
