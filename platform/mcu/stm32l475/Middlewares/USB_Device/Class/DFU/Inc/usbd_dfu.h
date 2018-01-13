/**
  ******************************************************************************
  * @file    usbd_dfu.h
  * @author  MCD Application Team
  * @version V2.4.2
  * @date    11-December-2015
  * @brief   Header file for the usbd_dfu.c file.
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

/* Define to prevent recursive inclusion -------------------------------------*/ 
#ifndef __USB_DFU_H
#define __USB_DFU_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */
  
/** @defgroup USBD_DFU
  * @brief This file is the Header file for usbd_dfu.c
  * @{
  */ 


/** @defgroup USBD_DFU_Exported_Defines
  * @{
  */ 
    
#define USB_DFU_CONFIG_DESC_SIZ        (18 + (9 * USBD_DFU_MAX_ITF_NUM))
#define USB_DFU_DESC_SIZ               9
    
#define DFU_DESCRIPTOR_TYPE            0x21
    

/**************************************************/
/* DFU Requests  DFU states                       */
/**************************************************/
#define APP_STATE_IDLE                 0
#define APP_STATE_DETACH               1
#define DFU_STATE_IDLE                 2
#define DFU_STATE_DNLOAD_SYNC          3
#define DFU_STATE_DNLOAD_BUSY          4
#define DFU_STATE_DNLOAD_IDLE          5
#define DFU_STATE_MANIFEST_SYNC        6
#define DFU_STATE_MANIFEST             7
#define DFU_STATE_MANIFEST_WAIT_RESET  8
#define DFU_STATE_UPLOAD_IDLE          9
#define DFU_STATE_ERROR                10

/**************************************************/
/* DFU errors                                     */
/**************************************************/
#define DFU_ERROR_NONE                 0x00
#define DFU_ERROR_TARGET               0x01
#define DFU_ERROR_FILE                 0x02
#define DFU_ERROR_WRITE                0x03
#define DFU_ERROR_ERASE                0x04
#define DFU_ERROR_CHECK_ERASED         0x05
#define DFU_ERROR_PROG                 0x06
#define DFU_ERROR_VERIFY               0x07
#define DFU_ERROR_ADDRESS              0x08
#define DFU_ERROR_NOTDONE              0x09
#define DFU_ERROR_FIRMWARE             0x0A
#define DFU_ERROR_VENDOR               0x0B
#define DFU_ERROR_USB                  0x0C
#define DFU_ERROR_POR                  0x0D
#define DFU_ERROR_UNKNOWN              0x0E
#define DFU_ERROR_STALLEDPKT           0x0F

/**************************************************/
/* DFU Manifestation State                        */
/**************************************************/
#define DFU_MANIFEST_COMPLETE          0x00
#define DFU_MANIFEST_IN_PROGRESS       0x01


/**************************************************/
/* Special Commands  with Download Request        */
/**************************************************/
#define DFU_CMD_GETCOMMANDS            0x00
#define DFU_CMD_SETADDRESSPOINTER      0x21
#define DFU_CMD_ERASE                  0x41
    
#define DFU_MEDIA_ERASE                0x00
#define DFU_MEDIA_PROGRAM              0x01

/**************************************************/
/* Other defines                                  */
/**************************************************/
/* Bit Detach capable = bit 3 in bmAttributes field */
#define DFU_DETACH_MASK                (uint8_t)(1 << 4) 
#define DFU_STATUS_DEPTH               (6) 
    
typedef enum 
{
  DFU_DETACH = 0,
  DFU_DNLOAD ,
  DFU_UPLOAD,
  DFU_GETSTATUS,
  DFU_CLRSTATUS,
  DFU_GETSTATE,
  DFU_ABORT
} DFU_RequestTypeDef;

typedef  void (*pFunction)(void);


/**********  Descriptor of DFU interface 0 Alternate setting n ****************/  
#define USBD_DFU_IF_DESC(n)            0x09,   /* bLength: Interface Descriptor size */ \
                                      USB_DESC_TYPE_INTERFACE,   /* bDescriptorType */ \
                                      0x00,   /* bInterfaceNumber: Number of Interface */ \
                                      (n),      /* bAlternateSetting: Alternate setting */ \
                                      0x00,   /* bNumEndpoints*/ \
                                      0xFE,   /* bInterfaceClass: Application Specific Class Code */ \
                                      0x01,   /* bInterfaceSubClass : Device Firmware Upgrade Code */ \
                                      0x02,   /* nInterfaceProtocol: DFU mode protocol */ \
                                      USBD_IDX_INTERFACE_STR + (n) + 1 /* iInterface: Index of string descriptor */ \
                                
#define TRANSFER_SIZE_BYTES(size)      ((uint8_t)(size)), /* XFERSIZEB0 */\
                                       ((uint8_t)(size >> 8)) /* XFERSIZEB1 */
                                             
#define IS_PROTECTED_AREA(add)         (uint8_t)(((add >= 0x08000000) && (add < (APP_DEFAULT_ADD)))? 1:0)                                             
                                            
/**
  * @}
  */ 


/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */

typedef struct
{
  union
  {
    uint32_t d32[USBD_DFU_XFER_SIZE/4];
    uint8_t  d8[USBD_DFU_XFER_SIZE];
  }buffer;
  
  uint8_t              dev_state; 
  uint8_t              dev_status[DFU_STATUS_DEPTH];
  uint8_t              manif_state;    
  
  uint32_t             wblock_num;
  uint32_t             wlength;
  uint32_t             data_ptr; 
  __IO uint32_t        alt_setting;
  
}
USBD_DFU_HandleTypeDef; 


typedef struct
{
  const uint8_t* pStrDesc;
  uint16_t (* Init)     (void);   
  uint16_t (* DeInit)   (void);   
  uint16_t (* Erase)    (uint32_t Add);
  uint16_t (* Write)    (uint8_t *src, uint8_t *dest, uint32_t Len);
  uint8_t* (* Read)     (uint8_t *src, uint8_t *dest, uint32_t Len);
  uint16_t (* GetStatus)(uint32_t Add, uint8_t cmd, uint8_t *buff);  
}
USBD_DFU_MediaTypeDef;
/**
  * @}
  */ 



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */ 

extern USBD_ClassTypeDef  USBD_DFU;
#define USBD_DFU_CLASS    &USBD_DFU
/**
  * @}
  */ 

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */ 
uint8_t  USBD_DFU_RegisterMedia    (USBD_HandleTypeDef   *pdev, 
                                    USBD_DFU_MediaTypeDef *fops);
/**
  * @}
  */ 

#ifdef __cplusplus
}
#endif

#endif  /* __USB_DFU_H */
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
