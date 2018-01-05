/**
  ******************************************************************************
  * @file    usbd_ioreq.c
  * @author  MCD Application Team
  * @version V2.4.2
  * @date    11-December-2015
  * @brief   This file provides the IO requests APIs for control endpoints.
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
#include "usbd_ioreq.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_IOREQ 
  * @brief control I/O requests module
  * @{
  */ 

/** @defgroup USBD_IOREQ_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_IOREQ_Private_Defines
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup USBD_IOREQ_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_IOREQ_Private_Variables
  * @{
  */ 

/**
  * @}
  */ 


/** @defgroup USBD_IOREQ_Private_FunctionPrototypes
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_IOREQ_Private_Functions
  * @{
  */ 

/**
* @brief  USBD_CtlSendData
*         send data on the ctl pipe
* @param  pdev: device instance
* @param  buff: pointer to data buffer
* @param  len: length of data to be sent
* @retval status
*/
USBD_StatusTypeDef  USBD_CtlSendData (USBD_HandleTypeDef  *pdev, 
                               uint8_t *pbuf,
                               uint16_t len)
{
  /* Set EP0 State */
  pdev->ep0_state          = USBD_EP0_DATA_IN;                                      
  pdev->ep_in[0].total_length = len;
  pdev->ep_in[0].rem_length   = len;
 /* Start the transfer */
  USBD_LL_Transmit (pdev, 0x00, pbuf, len);  
  
  return USBD_OK;
}

/**
* @brief  USBD_CtlContinueSendData
*         continue sending data on the ctl pipe
* @param  pdev: device instance
* @param  buff: pointer to data buffer
* @param  len: length of data to be sent
* @retval status
*/
USBD_StatusTypeDef  USBD_CtlContinueSendData (USBD_HandleTypeDef  *pdev, 
                                       uint8_t *pbuf,
                                       uint16_t len)
{
 /* Start the next transfer */
  USBD_LL_Transmit (pdev, 0x00, pbuf, len);   
  
  return USBD_OK;
}

/**
* @brief  USBD_CtlPrepareRx
*         receive data on the ctl pipe
* @param  pdev: device instance
* @param  buff: pointer to data buffer
* @param  len: length of data to be received
* @retval status
*/
USBD_StatusTypeDef  USBD_CtlPrepareRx (USBD_HandleTypeDef  *pdev,
                                  uint8_t *pbuf,                                  
                                  uint16_t len)
{
  /* Set EP0 State */
  pdev->ep0_state = USBD_EP0_DATA_OUT; 
  pdev->ep_out[0].total_length = len;
  pdev->ep_out[0].rem_length   = len;
  /* Start the transfer */
  USBD_LL_PrepareReceive (pdev,
                          0,
                          pbuf,
                         len);
  
  return USBD_OK;
}

/**
* @brief  USBD_CtlContinueRx
*         continue receive data on the ctl pipe
* @param  pdev: device instance
* @param  buff: pointer to data buffer
* @param  len: length of data to be received
* @retval status
*/
USBD_StatusTypeDef  USBD_CtlContinueRx (USBD_HandleTypeDef  *pdev, 
                                          uint8_t *pbuf,                                          
                                          uint16_t len)
{

  USBD_LL_PrepareReceive (pdev,
                          0,                     
                          pbuf,                         
                          len);
  return USBD_OK;
}
/**
* @brief  USBD_CtlSendStatus
*         send zero lzngth packet on the ctl pipe
* @param  pdev: device instance
* @retval status
*/
USBD_StatusTypeDef  USBD_CtlSendStatus (USBD_HandleTypeDef  *pdev)
{

  /* Set EP0 State */
  pdev->ep0_state = USBD_EP0_STATUS_IN;
  
 /* Start the transfer */
  USBD_LL_Transmit (pdev, 0x00, NULL, 0);   
  
  return USBD_OK;
}

/**
* @brief  USBD_CtlReceiveStatus
*         receive zero lzngth packet on the ctl pipe
* @param  pdev: device instance
* @retval status
*/
USBD_StatusTypeDef  USBD_CtlReceiveStatus (USBD_HandleTypeDef  *pdev)
{
  /* Set EP0 State */
  pdev->ep0_state = USBD_EP0_STATUS_OUT; 
  
 /* Start the transfer */  
  USBD_LL_PrepareReceive ( pdev,
                    0,
                    NULL,
                    0);  

  return USBD_OK;
}


/**
* @brief  USBD_GetRxCount
*         returns the received data length
* @param  pdev: device instance
* @param  ep_addr: endpoint address
* @retval Rx Data blength
*/
uint16_t  USBD_GetRxCount (USBD_HandleTypeDef  *pdev , uint8_t ep_addr)
{
  return USBD_LL_GetRxDataSize(pdev, ep_addr);
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
