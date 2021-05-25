/**
  ******************************************************************************
  * @file    rtl8721d_rtc.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the RTC firmware
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

#ifndef _RTL8721D_RTC_H_
#define _RTL8721D_RTC_H_

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @defgroup RTC 
  * @brief RTC driver modules
  * @{
  */

/** @addtogroup RTC
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * The real-time clock (RTC) is an independent BCD timer/counter. 
  * One 32-bit register contains the seconds, minutes, hours (12 or 24-hour format) expressed in binary coded decimal format (BCD).
  * and days expressed in binary format.
  * Daylight saving time compensation can also be performed. 
  * One programmable alarm contains seconds, minutes, hours and days with interrupt function,
  * and can be triggered by any combination of the time fields.
  * A digital calibration feature is available to compensate for some deviation.
  * After backup domain reset, all RTC registers are protected against possible parasitic 
  * write accesses.
  * Support a programmable register for RTC trigger SDM32K periodical calibration with active and power save mode.
  * As long as the supply voltage remains in the operating range, the RTC never stops, 
  * regardless of the device status (Run mode, low power mode or under reset).
  *
  *****************************************************************************************
  * How to use RTC
  *****************************************************************************************
  *      To use the RTC, the following steps are mandatory:
  *       
  *      1. Initialize the RTC Clock Source.
  *			RCC_PeriphClockSource_RTC()
  *
  *      2. Fill the variable RTC_InitStruct with default parameters, or set the desired parameters manually:
  *			RTC_StructInit(&RTC_InitStruct)
  *        
  *      3. configure the RTC peripheral with the corresponding configurations contained in RTC_InitStruct:
  *			Call RTC_Init(&RTC_InitStruct)
  * 
  *      4. Fill the variable RTC_TimeStruct with default parameters() (Time = 00d:00h:00min:00sec)
  *			RTC_TimeStructInit(&RTC_TimeStruct)
  *			setting the desired calendar time parameters manually.
  *   
  *      5. Set calendar time:
  *			RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct).
  * 
  *      6. If RTC_OUT output is needed, the pinmux configuration is as follows:
  *			Pinmux_Config(PinName, PINMUX_FUNCTION_RTCOUT)
  *			
  *
  *****************************************************************************************     
  * How to use Alarm
  ***************************************************************************************** 
  *      To use the Alarm, the following steps are mandatory:
  *       
  *      1. Configure the RTC as described in the first part of this driver. 
  *
  *      2. Fill the variable RTC_AlarmStruct of type RTC_AlarmTypeDef with default parameters 
  *			RTC_AlarmStructInit(&RTC_AlarmStruct)
  *			or setting the desired Alarm time parameters manually.
  *       
  *      3. configure Alarm:
  *			RTC_SetAlarm(RTC_Format_BIN, &RTC_AlarmStruct)
  *          
  *      4. Enable alarm and alarm interrupt.
  *			RTC_AlarmCmd(ENABLE)
  *
  *      5. Enable IRQ as follows:
  *			InterruptRegister(RTC_IntHandler, RTC_IRQ, NULL, 4);
  *			InterruptEn(RTC_IRQ, 4);
  *
  * @endverbatim
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup RTC_Exported_Types RTC Exported Types
  * @{
  */

/** 
  * @brief  RTC Init structure definition  
  */
typedef struct {
	u32 RTC_HourFormat;   /*!< Specifies the RTC Hour Format.
					This parameter can be a value of @ref RTC_Hour_Formats */

	u32 RTC_AsynchPrediv; /*!< Specifies the RTC Asynchronous Predivider value.
					This parameter must be a value of @ref RTC_Asynchronous_Predivider */

	u32 RTC_SynchPrediv;  /*!< Specifies the RTC Synchronous Predivider value.
					This parameter must be a value of @ref RTC_Synchronous_Predivider */

	u32 RTC_DayThreshold;  /*!< Specifies the RTC Day Threshold value.
					This parameter must be a value of @ref RTC_Day_Threshold */
} RTC_InitTypeDef;

/** 
  * @brief  RTC Time structure definition  
  */
typedef struct
{
	u16 RTC_Days;	/*!< Day in binary format 9bits 0~0x1FF */

	u8 RTC_Hours;    /*!< Specifies the RTC Time Hour.
				This parameter must be set to a value in the 1-12 range
				if the RTC_HourFormat_12 is selected or 0-23 range if
				the RTC_HourFormat_24 is selected. */

	u8 RTC_Minutes;  /*!< Specifies the RTC Time Minutes.
				This parameter must be set to a value in the 0-59 range. */

	u8 RTC_Seconds;  /*!< Specifies the RTC Time Seconds.
				This parameter must be set to a value in the 0-59 range. */

	u8 RTC_H12_PMAM;      /*!< Specifies the RTC AM/PM Time.
				This parameter can be a value of @ref RTC_AM_PM_Definitions */
}RTC_TimeTypeDef; 

/** 
  * @brief  RTC Alarm structure definition  
  */
typedef struct
{
	RTC_TimeTypeDef RTC_AlarmTime;     /*!< Specifies the RTC Alarm Time members. */

	u32 RTC_AlarmMask;            /*!< Specifies the RTC Alarm1 Masks(H:M:S).
					This parameter can be a value of @ref RTC_AlarmMask1_Definitions */
	u32 RTC_Alarm2Mask;            /*!< Specifies the RTC Alarm2 Masks Day).
					This parameter can be a value of @ref RTC_AlarmMask2_Definitions */
}RTC_AlarmTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup RTC_Exported_Constants RTC Exported Constants
  * @{
  */ 

/** @defgroup RTC_Input_parameter_format_definitions 
  * @{
  */ 
#define RTC_Format_BIN			((u32)0x000000000)
#define RTC_Format_BCD			((u32)0x000000001)
#define IS_RTC_FORMAT(FORMAT)	(((FORMAT) == RTC_Format_BIN) || ((FORMAT) == RTC_Format_BCD))
/**
  * @}
  */

/** @defgroup RTC_Hour_Formats 
  * @{
  */ 
#define RTC_HourFormat_24			((u32)0x00000000)
#define RTC_HourFormat_12			((u32)0x00000080)
#define IS_RTC_HOUR_FORMAT(FORMAT)     (((FORMAT) == RTC_HourFormat_12) || \
                                        ((FORMAT) == RTC_HourFormat_24))
/**
  * @}
  */

/** @defgroup RTC_Asynchronous_Predivider 
  * @{
  */ 
#define IS_RTC_ASYNCH_PREDIV(PREDIV)   ((PREDIV) <= 0x1FF)
/**
  * @}
  */

/** @defgroup RTC_Synchronous_Predivider 
  * @{
  */ 
#define IS_RTC_SYNCH_PREDIV(PREDIV)    ((PREDIV) <= 0x1FF)
/**
  * @}
  */

/** @defgroup RTC_Day_Threshold 
  * @{
  */ 
#define RTC_DAYTHRES_MSK			((u32)0xFF800000)
#define IS_RTC_DAY_THRES(DAYS)		((DAYS) <= 0x1FF)
/**
  * @}
  */

/** @defgroup RTC_Time_Definitions 
  * @{
  */ 
#define IS_RTC_HOUR12(HOUR)		(((HOUR) > 0) && ((HOUR) <= 12))
#define IS_RTC_HOUR24(HOUR)		((HOUR) <= 23)
#define IS_RTC_MINUTES(MINUTES)	((MINUTES) <= 59)
#define IS_RTC_SECONDS(SECONDS)	((SECONDS) <= 59)
/**
  * @}
  */

/** @defgroup RTC_AM_PM_Definitions 
  * @{
  */ 
#define RTC_H12_AM				((u8)0x00) //AM or 24-hour format
#define RTC_H12_PM				((u8)0x01) //PM
#define IS_RTC_H12_AMPM(PM)	(((PM) == RTC_H12_AM) || ((PM) == RTC_H12_PM))
/**
  * @}
  */

/** @defgroup RTC_AlarmMask1_Definitions 
  * @{
  */ 
#define RTC_AlarmMask_None		((u32)0x00000000)
#define RTC_AlarmMask_Hours	((u32)0x00800000)
#define RTC_AlarmMask_Minutes	((u32)0x00008000)
#define RTC_AlarmMask_Seconds	((u32)0x00000080)
#define RTC_AlarmMask_All		((u32)0x00808080)
#define IS_ALARM_MASK(MASK)	(((MASK) & 0x7F7F7F) == 0)
/**
  * @}
  */

/** @defgroup RTC_AlarmMask2_Definitions 
  * @{
  */ 
#define RTC_Alarm2Mask_None	((u32)0x00000000)
#define RTC_Alarm2Mask_Days	((u32)0x00000200)
#define IS_ALARM2_MASK(MASK)	(((MASK) & ~RTC_Alarm2Mask_Days) == 0)
/**
  * @}
  */

/** @defgroup RTC_Alarms_Definitions 
  * @{
  */ 
#define RTC_Alarm				((u32)0x00000100)
#define IS_RTC_ALARM(ALARM)	((ALARM) == RTC_Alarm)
#define IS_RTC_CMD_ALARM(ALARM) (((ALARM) & RTC_Alarm) != (u32)0)
/**
  * @}
  */

/** @defgroup RTC_Alarms_Interrupt_Definitions 
  * @{
  */ 
#define RTC_Alarm_IntEn				((u32)0x00001000)
/**
  * @}
  */

/** @defgroup RTC_DayLightSaving_Definitions 
  * @{
  */ 
#define RTC_DayLightSaving_SUB1H	((u32)0x00000002)
#define RTC_DayLightSaving_ADD1H	((u32)0x00000001)
#define IS_RTC_DAYLIGHT_SAVING(SAVE) (((SAVE) == RTC_DayLightSaving_SUB1H) || \
                                      ((SAVE) == RTC_DayLightSaving_ADD1H))

#define RTC_StoreOperation_Reset	((u32)0x00000000)
#define RTC_StoreOperation_Set	((u32)0x00000004)
#define IS_RTC_STORE_OPERATION(OPERATION) (((OPERATION) == RTC_StoreOperation_Reset) || \
                                           ((OPERATION) == RTC_StoreOperation_Set))
/**
  * @}
  */

/** @defgroup RTC_Output_selection_Definitions 
  * @{
  */ 
#define RTC_Output_Disable		((u32)0x00000000)
#define RTC_Output_Alarm		((u32)0x00000020) ////wakeup
#define RTC_Output_clkspre		((u32)0x00000040) ////1Hz
#define RTC_Output_clkapre		((u32)0x00000060) ////256Hz
 
#define IS_RTC_OUTPUT(OUTPUT) (((OUTPUT) == RTC_Output_Disable) || \
                               ((OUTPUT) == RTC_Output_Alarm) || \
                               ((OUTPUT) == RTC_Output_clkspre) || \
                               ((OUTPUT) == RTC_Output_clkapre))
/**
  * @}
  */

/** @defgroup RTC_Smooth_Calibration_Definitions
  * @{
  */ 
#define RTC_CalibPeriod_1MIN		((u32)0x00000000)
#define RTC_CalibPeriod_2MIN		((u32)0x00010000)
#define RTC_CalibPeriod_3MIN		((u32)0x00020000)
#define RTC_CalibPeriod_4MIN		((u32)0x00030000)
#define RTC_CalibPeriod_5MIN		((u32)0x00040000)
#define RTC_CalibPeriod_6MIN		((u32)0x00050000)
#define RTC_CalibPeriod_7MIN		((u32)0x00060000)
#define RTC_CalibPeriod_8MIN		((u32)0x00070000)

#define IS_RTC_CALIB_PERIOD(DCP) (((DCP) == RTC_CalibPeriod_1MIN) || \
                                 ((DCP) == RTC_CalibPeriod_2MIN) || \
                                 ((DCP) == RTC_CalibPeriod_3MIN) || \
                                 ((DCP) == RTC_CalibPeriod_4MIN) || \
                                 ((DCP) == RTC_CalibPeriod_5MIN) || \
                                 ((DCP) == RTC_CalibPeriod_6MIN) || \
                                 ((DCP) == RTC_CalibPeriod_7MIN) || \
                                 ((DCP) == RTC_CalibPeriod_8MIN))

#define RTC_Calib_Disable		((u32)0x00000000) 
#define RTC_Calib_Enable			((u32)0x00008000)

#define IS_RTC_CALIB_ENABLE(DCE) (((DCE) == RTC_Calib_Disable) || \
                                 ((DCE) == RTC_Calib_Enable))

#define RTC_CalibSign_Positive	((u32)0x00000000) 
#define RTC_CalibSign_Negative	((u32)0x00004000)

#define IS_RTC_CALIB_SIGN(SIGN) (((SIGN) == RTC_CalibSign_Positive) || \
                                 ((SIGN) == RTC_CalibSign_Negative))

#define IS_RTC_CALIB_VALUE(VALUE) ((VALUE) <= 0x7F)
/**
  * @}
  */

/** @defgroup RTC_32K_Auto_Calibration_Definitions 
  * @{
  */ 
#define RTC_32K_AUTOCAL_DISABLE		((u32)0x00000000)
#define RTC_32K_AUTOCAL_MINUTES		((u32)0x00000001)
#define RTC_32K_AUTOCAL_HOURS		((u32)0x00000002)
#define RTC_32K_AUTOCAL_DAYS			((u32)0x00000003)

#define IS_RTC_32K_AUTOCAL_SIGN(SIGN)		(((SIGN) == RTC_32K_AUTOCAL_DISABLE) || \
                                      ((SIGN) == RTC_32K_AUTOCAL_MINUTES) || \
                                      ((SIGN) == RTC_32K_AUTOCAL_HOURS) || \
                                      ((SIGN) == RTC_32K_AUTOCAL_DAYS))
/**
  * @}
  */

/**
  * @}
  */

/** @defgroup RTC_Exported_Functions RTC Exported Functions
  * @{
  */
_LONG_CALL_ u32 RTC_BypassShadowCmd(u32 NewState);
_LONG_CALL_ void RTC_StructInit(RTC_InitTypeDef* RTC_InitStruct);
_LONG_CALL_ u32 RTC_Init(RTC_InitTypeDef* RTC_InitStruct);
_LONG_CALL_ void RTC_TimeStructInit(RTC_TimeTypeDef* RTC_TimeStruct);
_LONG_CALL_ u32 RTC_SetTime(u32 RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct);
_LONG_CALL_ void RTC_GetTime(u32 RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct);
_LONG_CALL_ void RTC_DayIntClear(void);
_LONG_CALL_ u32 RTC_DayIntCmd(u32 NewState);
_LONG_CALL_ u32 RTC_DayThresSet(u32 DayThres);
_LONG_CALL_ u32 RTC_DayThresGet(void);
_LONG_CALL_ u32 RTC_SetAlarm(u32 RTC_Format, RTC_AlarmTypeDef* RTC_AlarmStruct);
_LONG_CALL_ void RTC_AlarmStructInit(RTC_AlarmTypeDef* RTC_AlarmStruct);
_LONG_CALL_ void RTC_GetAlarm(u32 RTC_Format, RTC_AlarmTypeDef* RTC_AlarmStruct);
_LONG_CALL_ void RTC_AlarmCmd(u32 NewState);
_LONG_CALL_ void RTC_AlarmClear(void);
_LONG_CALL_ u32 RTC_DayLightSavingConfig(u32 RTC_DayLightSaving, u32 RTC_StoreOperation);
_LONG_CALL_ u32 RTC_GetStoreOperation(void);
_LONG_CALL_ u32 RTC_OutputConfig(u32 RTC_Output);
_LONG_CALL_ u32 RTC_SmoothCalibConfig(u32 CalibSign, u32 Value, u32 CalibPeriod, u32 Calib_Enable);
_LONG_CALL_ u32 RTC_32KAutoCalibConfig(u32 Cal_Period, u32 Unit_Sel);
/**
  * @}
  */

/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup RTC_Register_Definitions RTC Register Definitions
 * @{
 *****************************************************************************/
 
/**************************************************************************//**
 * @defgroup RTC_TR
 * @{
 *****************************************************************************/
#define RTC_TR_DAY			((u32)0xFF800000)
#define RTC_TR_PM			((u32)0x00400000)
#define RTC_TR_HT			((u32)0x00300000)
#define RTC_TR_HU			((u32)0x000F0000)
#define RTC_TR_MNT			((u32)0x00007000)
#define RTC_TR_MNU			((u32)0x00000F00)
#define RTC_TR_ST			((u32)0x00000070)
#define RTC_TR_SU			((u32)0x0000000F)
/** @} */

/**************************************************************************//**
 * @defgroup RTC_CR
 * @{
 *****************************************************************************/
#define RTC_CR_DOVTHIE		((u32)0x00010000)
#define RTC_CR_ALRAIE		((u32)0x00001000)
#define RTC_CR_ALRAE		((u32)0x00000100)
#define RTC_CR_FMT			((u32)0x00000080)
#define RTC_CR_OSEL			((u32)0x00000060)
#define RTC_CR_BYPSHAD		((u32)0x00000008)
#define RTC_CR_BCK			((u32)0x00000004)
#define RTC_CR_SUB1H		((u32)0x00000002)
#define RTC_CR_ADD1H		((u32)0x00000001)
/** @} */

/**************************************************************************//**
 * @defgroup RTC_ISR
 * @{
 *****************************************************************************/
#define RTC_ISR_RECALPF		((u32)0x00010000)
#define RTC_ISR_DOVTHF		((u32)0x00008000)
#define RTC_ISR_ALRAF		((u32)0x00000100)

#define RTC_ISR_INIT			((u32)0x00000080)
#define RTC_ISR_INITF		((u32)0x00000040)
#define RTC_ISR_RSF			((u32)0x00000020)
#define RTC_ISR_INITS		((u32)0x00000010)

#define RTC_ISR_ALMWF		((u32)0x00000001)
/** @} */

/**************************************************************************//**
 * @defgroup RTC_PRER
 * @{
 *****************************************************************************/
#define RTC_PRER_PREDIV_A	((u32)0x01FF0000)
#define RTC_PRER_PREDIV_S	((u32)0x000001FF)
/** @} */

/**************************************************************************//**
 * @defgroup RTC_CALIBR
 * @{
 *****************************************************************************/
#define RTC_CALIBR_CALP		((u32)0x00070000)
#define RTC_CALIBR_DCE		((u32)0x00008000)
#define RTC_CALIBR_DCS		((u32)0x00004000)
#define RTC_CALIBR_DC		((u32)0x0000007F)
/** @} */

/**************************************************************************//**
 * @defgroup RTC_ALRMAR
 * @{
 *****************************************************************************/
#define RTC_ALRMAR_MSK2	((u32)0x00800000)
#define RTC_ALRMAR_PM		((u32)0x00400000)
#define RTC_ALRMAR_HT		((u32)0x00300000)
#define RTC_ALRMAR_HU		((u32)0x000F0000)

#define RTC_ALRMAR_MSK1	((u32)0x00008000)
#define RTC_ALRMAR_MNT		((u32)0x00007000)
#define RTC_ALRMAR_MNU		((u32)0x00000F00)

#define RTC_ALRMAR_MSK0	((u32)0x00000080)
#define RTC_ALRMAR_ST		((u32)0x00000070)
#define RTC_ALRMAR_SU		((u32)0x0000000F)
/** @} */

/**************************************************************************//**
 * @defgroup RTC_ALRMBR
 * @{
 *****************************************************************************/
#define RTC_ALRMBR_MSK3	((u32)0x00000200)
#define RTC_ALRMBR_DT		((u32)0x000001FF)
/** @} */

/**************************************************************************//**
 * @defgroup RTC_WPR
 * @{
 *****************************************************************************/
#define RTC_WPR_KEY		((u32)0x000000FF)
/** @} */

/**************************************************************************//**
 * @defgroup RTC_CLKACALR
 * @{
 *****************************************************************************/
#define RTC_32K_AUTOCAL_SEL	((u32)0x00000003)
#define RTC_AUTOCAL_THRES_MSK		((u32)0x000000FC)

#define IS_RTC_32K_AUTOCALIB_THRES(THRES)	(THRES <= 0x3F)
/** @} */
/** @} */

/**
  * @}
  */ 

/**
  * @}
  */

  /* Masks Definition */
#define RTC_TR_RESERVED_MASK	((u32)0xFFFF7F7F)
#define INITMODE_TIMEOUT	((u32) 0x00010000)
#define SYNCHRO_TIMEOUT	((u32) 0x00020000)
#define RECALPF_TIMEOUT	((u32) 0x00020000)
#define ALARMDIS_TIMEOUT	((u32) 0x00020000)

#endif //_RTL8721D_RTC_H_

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
