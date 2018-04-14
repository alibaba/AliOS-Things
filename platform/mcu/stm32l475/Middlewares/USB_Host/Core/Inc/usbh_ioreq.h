/**
  ******************************************************************************
  * @file    usbh_ioreq.h
  * @author  MCD Application Team
  * @version V3.2.2
  * @date    07-July-2015
  * @brief   Header file for usbh_ioreq.c
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

/* Define to prevent recursive  ----------------------------------------------*/
#ifndef __USBH_IOREQ_H
#define __USBH_IOREQ_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbh_conf.h"
#include "usbh_core.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_LIB_CORE
* @{
*/
  
/** @defgroup USBH_IOREQ
  * @brief This file is the header file for usbh_ioreq.c
  * @{
  */ 


/** @defgroup USBH_IOREQ_Exported_Defines
  * @{
  */
    
#define USBH_PID_SETUP                            0
#define USBH_PID_DATA                             1

#define USBH_EP_CONTROL                           0
#define USBH_EP_ISO                               1
#define USBH_EP_BULK                              2
#define USBH_EP_INTERRUPT                         3

#define USBH_SETUP_PKT_SIZE                       8
/**
  * @}
  */ 


/** @defgroup USBH_IOREQ_Exported_Types
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_IOREQ_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_IOREQ_Exported_Variables
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_IOREQ_Exported_FunctionsPrototype
  * @{
  */
USBH_StatusTypeDef USBH_CtlSendSetup (USBH_HandleTypeDef *phost, 
                                uint8_t *buff, 
                                uint8_t hc_num);

USBH_StatusTypeDef USBH_CtlSendData (USBH_HandleTypeDef *phost, 
                                uint8_t *buff, 
                                uint16_t length,
                                uint8_t hc_num,
                                uint8_t do_ping );

USBH_StatusTypeDef USBH_CtlReceiveData(USBH_HandleTypeDef *phost, 
                                uint8_t *buff, 
                                uint16_t length,
                                uint8_t hc_num);

USBH_StatusTypeDef USBH_BulkReceiveData(USBH_HandleTypeDef *phost, 
                                uint8_t *buff, 
                                uint16_t length,
                                uint8_t hc_num);

USBH_StatusTypeDef USBH_BulkSendData (USBH_HandleTypeDef *phost, 
                                uint8_t *buff, 
                                uint16_t length,
                                uint8_t hc_num,
                                uint8_t do_ping );

USBH_StatusTypeDef USBH_InterruptReceiveData(USBH_HandleTypeDef *phost, 
                                       uint8_t             *buff, 
                                       uint8_t             length,
                                       uint8_t             hc_num);

USBH_StatusTypeDef USBH_InterruptSendData(USBH_HandleTypeDef *phost, 
                                    uint8_t *buff, 
                                    uint8_t length,
                                    uint8_t hc_num);


USBH_StatusTypeDef USBH_IsocReceiveData(USBH_HandleTypeDef *phost, 
                                uint8_t *buff, 
                                uint32_t length,
                                uint8_t hc_num);


USBH_StatusTypeDef USBH_IsocSendData(USBH_HandleTypeDef *phost, 
                                uint8_t *buff, 
                                uint32_t length,
                                uint8_t hc_num);
/**
  * @}
  */ 

#ifdef __cplusplus
}
#endif

#endif /* __USBH_IOREQ_H */

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


