/**
  ******************************************************************************
  * @file    rtl8721dlp_km4.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for km0 control km4.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _RTL8721D_LP_KM4_H_
#define _RTL8721D_LP_KM4_H_

/** @addtogroup AmebaD_Platform
  * @{
  */

/** @defgroup KM0_CTRL_KM4 
  * @brief KM0_CTRL_KM4 driver modules
  * @{
  */


/** @addtogroup KM0_CTRL_KM4
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  *		- functions prototypes for for km0 control km4.
  *
  *****************************************************************************************
  *
  *****************************************************************************************
  * how to use
  *****************************************************************************************
  *
  *****************************************************************************************
  * @endverbatim
  */

/* Exported Types --------------------------------------------------------*/

/** @defgroup km4_sleep_parameter km4 sleep parameter
  * @{
  */
#ifndef CONFIG_BUILD_ROM

typedef struct
{
	u8	dlps_enable; 
	u8	sleep_type;
	u32	sleep_time;
	
} KM4SLEEP_ParamDef;

#endif
/**
  * @}
  */



/* Exported constants --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @defgroup KM4_CTRL_Exported_Functions KM4_CTRL Exported Functions
  * @{
  */
void km4_pm_init(void);
void km4_boot_on(void);
u32 km4_suspend(u32 type);
void km4_resume(void);
u32 km4_status_on(void);
void km4_set_wake_event(u32 wevt);
u32 km4_get_wake_event(void);
void km4_wake_event_update(void);
void km4_tickless_ipc_int(VOID *Data, u32 IrqStatus, u32 ChanNum);
void km4_flash_highspeed_resume(u32 Protection);
void km4_flash_highspeed_suspend(u32 Protection);
void km4_flash_highspeed_init(void);
/**
  * @}
  */


/**
  * @}
  */

/**
  * @}
  */

/* Registers Definitions --------------------------------------------------------*/
/* Other definations --------------------------------------------------------*/
extern u8 km4_sleep_type;
extern u32 km4_sleep_timeout;
extern void flash_operation_config(void);
#endif /* _RTL8721D_LP_KM4_H_ */
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
