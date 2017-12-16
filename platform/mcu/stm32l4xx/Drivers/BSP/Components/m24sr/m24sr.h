/**
  ******************************************************************************
  * @file    m24sr.h
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    21-April-2017
  * @brief   This file provides a set of functions needed to manage M24SR
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
#ifndef __M24SR_H
#define __M24SR_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Component
  * @{
  */

/** @addtogroup M24SR 
  * @{
  */

/** @defgroup M24SR_Exported_Constants   M24SR Exported Constants
  * @{
  */
/* NFC IO specific config parameters */
#define NFC_IO_STATUS_SUCCESS      (uint16_t) 0x0000
#define NFC_IO_ERROR_TIMEOUT       (uint16_t) 0x0011
#define NFC_IO_TRIALS              (uint32_t) 1 /* In case M24SR will reply ACK failed allow to perform retry */
    
/* Status and error code -----------------------------------------------------*/   
#define M24SR_ACTION_COMPLETED     (uint16_t) 0x9000
#define M24SR_STATUS_SUCCESS       (uint16_t) 0x0000
#define M24SR_ERROR_DEFAULT        (uint16_t) 0x0010
#define M24SR_ERROR_TIMEOUT        (uint16_t) 0x0011
#define M24SR_ERROR_CRC            (uint16_t) 0x0012
#define M24SR_ERROR_NACK           (uint16_t) 0x0013
#define M24SR_ERROR_PARAMETER      (uint16_t) 0x0014 
#define M24SR_ERROR_NBATEMPT       (uint16_t) 0x0015 
#define M24SR_ERROR_NOACKNOWLEDGE  (uint16_t) 0x0016

#define M24SR_ANSWER_TIMEOUT   (uint32_t)  80 /* Timeout used by the component function NFC_IO_IsDeviceReady() */
#define M24SR_ANSWER_STABLE    (uint8_t)    5 /* Loop repetition used by the component function NFC_IO_IsDeviceReady() */


/*-------------------------- GPO_Mode ----------------------------*/
#define M24SR_GPO_POLLING      (uint8_t) 0x00 /* Normal  I²C polling */
#define M24SR_GPO_SYNCHRO      (uint8_t) 0x01 /* allow to use GPO polling as I2C synchronization */ 
#define M24SR_GPO_INTERRUPT    (uint8_t) 0x02 /* allow to use GPO interrupt as I2C synchronization */ 


/*-------------------------- Password_Management ----------------------------*/
#define M24SR_READ_PWD         (uint16_t) 0x0001
#define M24SR_WRITE_PWD        (uint16_t) 0x0002
#define M24SR_I2C_PWD          (uint16_t) 0x0003

/*-------------------------- Verify command answer ----------------------------*/
#define M24SR_PWD_NOT_NEEDED   (uint16_t) 0x9000
#define M24SR_PWD_NEEDED       (uint16_t) 0x6300
#define M24SR_PWD_CORRECT      (uint16_t) 0x9000

/**
  * @}
  */

/** @defgroup M24SR_Exported_FunctionsPrototypes   M24SR Exported FunctionsPrototypes
  * @{
  */  
/*  public function  --------------------------------------------------------------------------*/
void     M24SR_Init                           (uint16_t DeviceAddr, uint8_t GpoMode);
uint16_t M24SR_GetSession                     (uint16_t DeviceAddr);
uint16_t M24SR_KillSession                    (uint16_t DeviceAddr);
uint16_t M24SR_Deselect                       (uint16_t DeviceAddr);
uint16_t M24SR_SelectApplication              (uint16_t DeviceAddr);
uint16_t M24SR_SelectCCfile                   (uint16_t DeviceAddr);
uint16_t M24SR_SelectNDEFfile                 (uint16_t DeviceAddr, uint16_t NDEFfileId);
uint16_t M24SR_SelectSystemfile               (uint16_t DeviceAddr);
uint16_t M24SR_ReadBinary                     (uint16_t DeviceAddr, uint16_t Offset, uint8_t NbByteToRead, uint8_t *pBufferRead);
uint16_t M24SR_STReadBinary                   (uint16_t DeviceAddr, uint16_t Offset, uint8_t NbByteToRead, uint8_t *pBufferRead);
uint16_t M24SR_UpdateBinary                   (uint16_t DeviceAddr, uint16_t Offset, uint8_t NbByteToWrite, uint8_t *pDataToWrite);
uint16_t M24SR_Verify                         (uint16_t DeviceAddr, uint16_t uPwdId, uint8_t NbPwdByte, uint8_t *pPwd);
uint16_t M24SR_ChangeReferenceData            (uint16_t DeviceAddr, uint16_t uPwdId, uint8_t *pPwd);
uint16_t M24SR_EnableVerificationRequirement  (uint16_t DeviceAddr, uint16_t uReadOrWrite);
uint16_t M24SR_DisableVerificationRequirement (uint16_t DeviceAddr, uint16_t uReadOrWrite);
uint16_t M24SR_EnablePermanentState           (uint16_t DeviceAddr, uint16_t uReadOrWrite);
uint16_t M24SR_DisablePermanentState          (uint16_t DeviceAddr, uint16_t uReadOrWrite);
uint16_t M24SR_SendInterrupt                  (uint16_t DeviceAddr);
uint16_t M24SR_StateControl                   (uint16_t DeviceAddr, uint8_t uSetOrReset);
uint16_t M24SR_ManageI2CGPO                   (uint16_t DeviceAddr, uint8_t GPO_I2Cconfig);
uint16_t M24SR_ManageRFGPO                    (uint16_t DeviceAddr, uint8_t GPO_RFconfig);
void     M24SR_RFConfig                       (uint8_t OnOffChoice);
void     M24SR_GPO_Callback                   (void );

/**
  * @}
  */ 
  

/** @defgroup M24SR_Imported_Functions M24SR Imported Functions
 * @{
 */
/* IO functions */
extern void     NFC_IO_Init(uint8_t GpoIrqEnable);
extern void     NFC_IO_DeInit(void);
extern uint16_t NFC_IO_ReadMultiple (uint8_t Addr, uint8_t *pBuffer, uint16_t Length );
extern uint16_t NFC_IO_WriteMultiple (uint8_t Addr, uint8_t *pBuffer, uint16_t Length);
extern uint16_t NFC_IO_IsDeviceReady (uint8_t Addr, uint32_t Trials);
extern void     NFC_IO_ReadState(uint8_t * pPinState);
extern void     NFC_IO_RfDisable(uint8_t PinState);
extern void     NFC_IO_Delay(uint32_t Delay);

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

#ifdef __cplusplus
}
#endif

#endif /* __M24SR_H */




/******************* (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
