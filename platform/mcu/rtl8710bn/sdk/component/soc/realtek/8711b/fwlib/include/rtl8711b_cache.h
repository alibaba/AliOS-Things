/**
  ******************************************************************************
  * @file    rtl8711b_cache.h
  * @author
  * @version V1.0.0
  * @date    2016-05-17
  * @brief   This file contains all the functions prototypes for the flash cache firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#ifndef _RTL8710B_CACHE_H_
#define _RTL8710B_CACHE_H_

/** @addtogroup AmebaZ_Platform
  * @{
  */

/** @defgroup CACHE 
  * @brief CACHE modules
  * @{
  */

/** @addtogroup CACHE
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  *		-just support read cache.
  *		-32K bytes.
  *		-used for flash read and XIP.
  *
  *****************************************************************************************
  * how to use
  *****************************************************************************************
  *		Cache_Enable: enable/disable cache
  *		Cache_Flush: flush cache, you should Cache_Flush after flash write or flash erase
  *****************************************************************************************
  * @endverbatim
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup CACHE_Exported_Functions FLash Cache Exported Functions
  * @{
  */
_LONG_CALL_ void Cache_Enable(u32 Enable);
_LONG_CALL_ void Cache_Flush(void);
_LONG_CALL_ void Cache_Debug(u32 PrintLog);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
#endif //_RTL8710B_CACHE_H_
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
