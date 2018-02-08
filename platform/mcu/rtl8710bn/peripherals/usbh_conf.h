/**
  ******************************************************************************
  * @file    FatFs/FatFs_USBDisk_RTOS/Inc/usbh_conf.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-March-2014
  * @brief   General low level driver configuration
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBH_CONF_H
#define __USBH_CONF_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Exported types ------------------------------------------------------------*/
#define USBH_MAX_NUM_ENDPOINTS                2
#define USBH_MAX_NUM_INTERFACES               2
#define USBH_MAX_NUM_CONFIGURATION            1
#define USBH_MAX_NUM_SUPPORTED_CLASS          1
#define USBH_KEEP_CFG_DESCRIPTOR              0
#define USBH_MAX_SIZE_CONFIGURATION           0x200
#define USBH_MAX_DATA_BUFFER                  0x200
#define USBH_DEBUG_LEVEL                      0
#define USBH_USE_OS                           1
    
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* CMSIS OS macros */ 
#if (USBH_USE_OS == 1)
  #include "MICO.h"
#endif    

/* Memory management macros */    
#define USBH_malloc               malloc
#define USBH_free                 free
#define USBH_memset               memset
#define USBH_memcpy               memcpy

#ifdef USE_USB_FS
void OTG_FS_IRQHandler(void);
#endif
#ifdef __cplusplus
}
#endif
    
/* DEBUG macros */   
#if (USBH_DEBUG_LEVEL > 0)
#define USBH_UsrLog(...) custom_log("USB", M, ##__VA_ARGS__)
                           
#else
#define USBH_UsrLog(...)   
#endif 
                            
                            
#if (USBH_DEBUG_LEVEL > 1)

#define USBH_ErrLog(...) custom_log("ERROR USB", M, ##__VA_ARGS__)
                           
#else
#define USBH_ErrLog(...)   
#endif 
                                                      
#if (USBH_DEBUG_LEVEL > 2)                         
#define  USBH_DbgLog(...)   custom_log("DEBUG USB", M, ##__VA_ARGS__)
                                                  
#else
#define USBH_DbgLog(...)                         
#endif

/* Exported functions ------------------------------------------------------- */

#endif /* __USB_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

