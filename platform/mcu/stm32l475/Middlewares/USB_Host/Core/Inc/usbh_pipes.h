/**
  ******************************************************************************
  * @file    usbh_pipes.h
  * @author  MCD Application Team
  * @version V3.2.2
  * @date    07-July-2015
  * @brief   Header file for usbh_pipes.c
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
#ifndef __USBH_PIPES_H
#define __USBH_PIPES_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbh_core.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_LIB_CORE
* @{
*/
  
/** @defgroup USBH_PIPES
  * @brief This file is the header file for usbh_pipes.c
  * @{
  */ 

/** @defgroup USBH_PIPES_Exported_Defines
  * @{
  */
/**
  * @}
  */ 

/** @defgroup USBH_PIPES_Exported_Types
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_PIPES_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_PIPES_Exported_Variables
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_PIPES_Exported_FunctionsPrototype
  * @{
  */

USBH_StatusTypeDef USBH_OpenPipe  (USBH_HandleTypeDef *phost,
                            uint8_t ch_num,
                            uint8_t epnum,                            
                            uint8_t dev_address,
                            uint8_t speed,
                            uint8_t ep_type,
                            uint16_t mps);

USBH_StatusTypeDef USBH_ClosePipe  (USBH_HandleTypeDef *phost,
                            uint8_t pipe_num);

uint8_t USBH_AllocPipe  (USBH_HandleTypeDef *phost, 
                         uint8_t ep_addr);

USBH_StatusTypeDef USBH_FreePipe  (USBH_HandleTypeDef *phost, 
                                   uint8_t idx);




/**
  * @}
  */ 


#ifdef __cplusplus
}
#endif

#endif /* __USBH_PIPES_H */


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


