/**
  ******************************************************************************
  * @file    usbh_msc_bot.h
  * @author  MCD Application Team
  * @version V3.2.2
  * @date    07-July-2015
  * @brief   Header file for usbh_msc_bot.c
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
#ifndef __USBH_MSC_BOT_H
#define __USBH_MSC_BOT_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbh_core.h"
#include "usbh_msc_bot.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_CLASS
  * @{
  */

/** @addtogroup USBH_MSC_CLASS
  * @{
  */
  
/** @defgroup USBH_MSC_BOT
  * @brief This file is the Header file for usbh_msc_bot.c
  * @{
  */ 


/** @defgroup USBH_MSC_BOT_Exported_Types
  * @{
  */ 
    
typedef enum {
  BOT_OK          = 0,
  BOT_FAIL        = 1,
  BOT_PHASE_ERROR = 2,
  BOT_BUSY        = 3
}
BOT_StatusTypeDef;

typedef enum {
  BOT_CMD_IDLE  = 0,
  BOT_CMD_SEND,
  BOT_CMD_WAIT,
} 
BOT_CMDStateTypeDef;  

/* CSW Status Definitions */
typedef enum 
{

   BOT_CSW_CMD_PASSED   =        0x00,
   BOT_CSW_CMD_FAILED   =        0x01,
   BOT_CSW_PHASE_ERROR  =        0x02,
} 
BOT_CSWStatusTypeDef;  

typedef enum {
  BOT_SEND_CBW  = 1,
  BOT_SEND_CBW_WAIT,         
  BOT_DATA_IN,    
  BOT_DATA_IN_WAIT,    
  BOT_DATA_OUT, 
  BOT_DATA_OUT_WAIT,     
  BOT_RECEIVE_CSW,
  BOT_RECEIVE_CSW_WAIT,       
  BOT_ERROR_IN,         
  BOT_ERROR_OUT, 
  BOT_UNRECOVERED_ERROR
} 
BOT_StateTypeDef;  
  
typedef union 
{
  struct __CBW
  {
    uint32_t Signature;
    uint32_t Tag;
    uint32_t DataTransferLength;
    uint8_t  Flags;
    uint8_t  LUN; 
    uint8_t  CBLength;
    uint8_t  CB[16];
  }field;
  uint8_t data[31];
}
BOT_CBWTypeDef;

typedef union 
{
  struct __CSW
  {
    uint32_t Signature;
    uint32_t Tag;
    uint32_t DataResidue;
    uint8_t  Status;
  }field;
  uint8_t data[13];
}
BOT_CSWTypeDef;

typedef struct
{
  uint32_t                   data[16];    
  BOT_StateTypeDef           state;
  BOT_StateTypeDef           prev_state;  
  BOT_CMDStateTypeDef        cmd_state;
  BOT_CBWTypeDef             cbw;
  uint8_t                    Reserved1;
  BOT_CSWTypeDef             csw; 
  uint8_t                    Reserved2[3];  
  uint8_t                    *pbuf;
} 
BOT_HandleTypeDef;

/**
  * @}
  */ 



/** @defgroup USBH_MSC_BOT_Exported_Defines
  * @{
  */ 
#define BOT_CBW_SIGNATURE            0x43425355
#define BOT_CBW_TAG                  0x20304050             
#define BOT_CSW_SIGNATURE            0x53425355           
#define BOT_CBW_LENGTH               31
#define BOT_CSW_LENGTH               13     



#define BOT_SEND_CSW_DISABLE         0
#define BOT_SEND_CSW_ENABLE          1

#define BOT_DIR_IN                   0
#define BOT_DIR_OUT                  1
#define BOT_DIR_BOTH                 2

#define BOT_PAGE_LENGTH              512


#define BOT_CBW_CB_LENGTH            16


#define USB_REQ_BOT_RESET                0xFF
#define USB_REQ_GET_MAX_LUN              0xFE

#define MAX_BULK_STALL_COUNT_LIMIT       0x04   /* If STALL is seen on Bulk 
                                         Endpoint continuously, this means 
                                         that device and Host has phase error
                                         Hence a Reset is needed */

/**
  * @}
  */ 

/** @defgroup USBH_MSC_BOT_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_MSC_BOT_Exported_Variables
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBH_MSC_BOT_Exported_FunctionsPrototype
  * @{
  */ 
USBH_StatusTypeDef USBH_MSC_BOT_REQ_Reset(USBH_HandleTypeDef *phost);
USBH_StatusTypeDef USBH_MSC_BOT_REQ_GetMaxLUN(USBH_HandleTypeDef *phost, uint8_t *Maxlun);

USBH_StatusTypeDef USBH_MSC_BOT_Init(USBH_HandleTypeDef *phost);
USBH_StatusTypeDef USBH_MSC_BOT_Process (USBH_HandleTypeDef *phost, uint8_t lun);
USBH_StatusTypeDef USBH_MSC_BOT_Error(USBH_HandleTypeDef *phost, uint8_t lun);



/**
  * @}
  */ 

#ifdef __cplusplus
}
#endif

#endif  /* __USBH_MSC_BOT_H__ */


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

