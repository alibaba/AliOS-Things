/**
  ******************************************************************************
  * @file    usbh_msc.h
  * @author  MCD Application Team
  * @version V3.2.2
  * @date    07-July-2015
  * @brief   This file contains all the prototypes for the usbh_msc.c
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
#ifndef __USBH_MSC_H
#define __USBH_MSC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbh_core.h"
#include "usbh_msc_bot.h"
#include "usbh_msc_scsi.h"

/** @addtogroup USBH_LIB
  * @{
  */

/** @addtogroup USBH_CLASS
  * @{
  */

/** @addtogroup USBH_MSC_CLASS
  * @{
  */
  
/** @defgroup USBH_MSC_CORE
  * @brief This file is the Header file for usbh_msc.c
  * @{
  */ 


/** @defgroup USBH_MSC_CORE_Exported_Types
  * @{
  */ 

typedef enum
{
  MSC_INIT = 0,     
  MSC_IDLE,    
  MSC_TEST_UNIT_READY,          
  MSC_READ_CAPACITY10,
  MSC_READ_INQUIRY,
  MSC_REQUEST_SENSE,
  MSC_READ,
  MSC_WRITE,
  MSC_UNRECOVERED_ERROR,  
  MSC_PERIODIC_CHECK,    
}
MSC_StateTypeDef;

typedef enum
{
  MSC_OK,
  MSC_NOT_READY,
  MSC_ERROR,  

}
MSC_ErrorTypeDef;

typedef enum
{
  MSC_REQ_IDLE = 0,
  MSC_REQ_RESET,                
  MSC_REQ_GET_MAX_LUN,  
  MSC_REQ_ERROR,  
}
MSC_ReqStateTypeDef;

#ifndef MAX_SUPPORTED_LUN       
    #define MAX_SUPPORTED_LUN       2
#endif


/* Structure for LUN */
typedef struct
{
  MSC_StateTypeDef            state; 
  MSC_ErrorTypeDef            error;   
  USBH_StatusTypeDef          prev_ready_state;
  SCSI_CapacityTypeDef        capacity;
  SCSI_SenseTypeDef           sense;  
  SCSI_StdInquiryDataTypeDef  inquiry;
  uint8_t                     state_changed; 
  
}
MSC_LUNTypeDef; 

/* Structure for MSC process */
typedef struct _MSC_Process
{
  uint32_t             max_lun;   
  uint8_t              InPipe; 
  uint8_t              OutPipe; 
  uint8_t              OutEp;
  uint8_t              InEp;
  uint16_t             OutEpSize;
  uint16_t             InEpSize;
  MSC_StateTypeDef     state;
  MSC_ErrorTypeDef     error;
  MSC_ReqStateTypeDef  req_state;
  MSC_ReqStateTypeDef  prev_req_state;  
  BOT_HandleTypeDef    hbot;
  MSC_LUNTypeDef       unit[MAX_SUPPORTED_LUN];
  uint16_t             current_lun; 
  uint16_t             rw_lun;   
  uint32_t             timer;
}
MSC_HandleTypeDef; 


/**
  * @}
  */ 



/** @defgroup USBH_MSC_CORE_Exported_Defines
  * @{
  */

#define USB_REQ_BOT_RESET                0xFF
#define USB_REQ_GET_MAX_LUN              0xFE
   

/* MSC Class Codes */
#define USB_MSC_CLASS                                   0x08

/* Interface Descriptor field values for HID Boot Protocol */
#define MSC_BOT                                        0x50 
#define MSC_TRANSPARENT                                0x06     
/**
  * @}
  */ 

/** @defgroup USBH_MSC_CORE_Exported_Macros
  * @{
  */ 
/**
  * @}
  */ 

/** @defgroup USBH_MSC_CORE_Exported_Variables
  * @{
  */ 
extern USBH_ClassTypeDef  USBH_msc;
#define USBH_MSC_CLASS    &USBH_msc

/**
  * @}
  */ 

/** @defgroup USBH_MSC_CORE_Exported_FunctionsPrototype
  * @{
  */ 

/* Common APIs */     
uint8_t            USBH_MSC_IsReady (USBH_HandleTypeDef *phost);

/* APIs for LUN */
int8_t             USBH_MSC_GetMaxLUN (USBH_HandleTypeDef *phost);

uint8_t            USBH_MSC_UnitIsReady (USBH_HandleTypeDef *phost, uint8_t lun);

USBH_StatusTypeDef USBH_MSC_GetLUNInfo(USBH_HandleTypeDef *phost, uint8_t lun, MSC_LUNTypeDef *info);
                                 
USBH_StatusTypeDef USBH_MSC_Read(USBH_HandleTypeDef *phost,
                                     uint8_t lun,
                                     uint32_t address,
                                     uint8_t *pbuf,
                                     uint32_t length);

USBH_StatusTypeDef USBH_MSC_Write(USBH_HandleTypeDef *phost,
                                     uint8_t lun,
                                     uint32_t address,
                                     uint8_t *pbuf,
                                     uint32_t length);
/**
  * @}
  */ 

#ifdef __cplusplus
}
#endif

#endif  /* __USBH_MSC_H */


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



