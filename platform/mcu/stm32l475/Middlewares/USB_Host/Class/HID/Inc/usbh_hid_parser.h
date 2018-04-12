/**
  ******************************************************************************
  * @file    usbh_hid_parser.c 
  * @author  MCD Application Team
  * @version V3.2.2
  * @date    07-July-2015
  * @brief   This file is the header file of the usbh_hid_parser.c        
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive -----------------------------------------------*/ 
#ifndef __USBH_HID_PARSER_H
#define __USBH_HID_PARSER_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbh_hid.h"
#include "usbh_hid_usage.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_CLASS
  * @{
  */

/** @addtogroup USBH_HID_CLASS
  * @{
  */
  
/** @defgroup USBH_HID_PARSER
  * @brief This file is the Header file for usbh_hid_parser.c
  * @{
  */ 


/** @defgroup USBH_HID_PARSER_Exported_Types
  * @{
  */ 
typedef struct 
{
  uint8_t  *data;
  uint32_t size;
  uint8_t  shift;
  uint8_t  count;
  uint8_t  sign;
  uint32_t logical_min;  /*min value device can return*/
  uint32_t logical_max;  /*max value device can return*/
  uint32_t physical_min; /*min vale read can report*/
  uint32_t physical_max; /*max value read can report*/
  uint32_t resolution;
} 
HID_Report_ItemTypedef;


uint32_t HID_ReadItem (HID_Report_ItemTypedef *ri, uint8_t ndx);
uint32_t HID_WriteItem(HID_Report_ItemTypedef *ri, uint32_t value, uint8_t ndx);


/**
  * @}
  */ 

#ifdef __cplusplus
}
#endif

#endif /* __USBH_HID_PARSER_H */

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 

/**
  * @}
  */ 
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
