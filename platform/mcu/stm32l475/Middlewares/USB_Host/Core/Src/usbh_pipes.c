/**
  ******************************************************************************
  * @file    usbh_pipes.c
  * @author  MCD Application Team
  * @version V3.2.2
  * @date    07-July-2015
  * @brief   This file implements functions for opening and closing Pipes
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

/* Includes ------------------------------------------------------------------*/
#include "usbh_pipes.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_LIB_CORE
* @{
*/
  
/** @defgroup USBH_PIPES
  * @brief This file includes opening and closing Pipes
  * @{
  */ 

/** @defgroup USBH_PIPES_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_PIPES_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_PIPES_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBH_PIPES_Private_Variables
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup USBH_PIPES_Private_Functions
  * @{
  */ 
static uint16_t USBH_GetFreePipe (USBH_HandleTypeDef *phost);


/**
  * @brief  USBH_Open_Pipe
  *         Open a  pipe
  * @param  phost: Host Handle
  * @param  pipe_num: Pipe Number
  * @param  dev_address: USB Device address allocated to attached device
  * @param  speed : USB device speed (Full/Low)
  * @param  ep_type: end point type (Bulk/int/ctl)
  * @param  mps: max pkt size
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_OpenPipe  (USBH_HandleTypeDef *phost,
                            uint8_t pipe_num,
                            uint8_t epnum,
                            uint8_t dev_address,
                            uint8_t speed,
                            uint8_t ep_type,
                            uint16_t mps)
{

  USBH_LL_OpenPipe(phost,
                        pipe_num,
                        epnum,
                        dev_address,
                        speed,
                        ep_type,
                        mps);
  
  return USBH_OK; 

}

/**
  * @brief  USBH_ClosePipe
  *         Close a  pipe
  * @param  phost: Host Handle
  * @param  pipe_num: Pipe Number
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_ClosePipe  (USBH_HandleTypeDef *phost,
                            uint8_t pipe_num)
{

  USBH_LL_ClosePipe(phost, pipe_num);
  
  return USBH_OK; 

}

/**
  * @brief  USBH_Alloc_Pipe
  *         Allocate a new Pipe
  * @param  phost: Host Handle
  * @param  ep_addr: End point for which the Pipe to be allocated
  * @retval Pipe number
  */
uint8_t USBH_AllocPipe  (USBH_HandleTypeDef *phost, uint8_t ep_addr)
{
  uint16_t pipe;
  
  pipe =  USBH_GetFreePipe(phost);

  if (pipe != 0xFFFF)
  {
	phost->Pipes[pipe] = 0x8000 | ep_addr;
  }
  return pipe;
}

/**
  * @brief  USBH_Free_Pipe
  *         Free the USB Pipe
  * @param  phost: Host Handle
  * @param  idx: Pipe number to be freed 
  * @retval USBH Status
  */
USBH_StatusTypeDef USBH_FreePipe  (USBH_HandleTypeDef *phost, uint8_t idx)
{
   if(idx < 11)
   {
	 phost->Pipes[idx] &= 0x7FFF;
   }
   return USBH_OK;
}

/**
  * @brief  USBH_GetFreePipe
  * @param  phost: Host Handle
  *         Get a free Pipe number for allocation to a device endpoint
  * @retval idx: Free Pipe number
  */
static uint16_t USBH_GetFreePipe (USBH_HandleTypeDef *phost)
{
  uint8_t idx = 0;
  
  for (idx = 0 ; idx < 11 ; idx++)
  {
	if ((phost->Pipes[idx] & 0x8000) == 0)
	{
	   return idx;
	} 
  }
  return 0xFFFF;
}
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


