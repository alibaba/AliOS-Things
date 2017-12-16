/**
  ******************************************************************************
  * @file    cs42l51.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    17-February-2017
  * @brief   This file contains all the functions prototypes for the cs42l51.c driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CS42L51_H
#define __CS42L51_H

/* Includes ------------------------------------------------------------------*/
#include "../Common/audio.h"

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup Component
  * @{
  */ 
  
/** @addtogroup CS42L51
  * @{
  */

/** @defgroup CS42L51_Exported_Types
  * @{
  */

/**
  * @}
  */

/** @defgroup CS42L51_Exported_Constants
  * @{
  */ 

/******************************************************************************/
/***************************  Codec User defines ******************************/
/******************************************************************************/
/* Codec output devices */
#define OUTPUT_DEVICE_HEADPHONE       0x01

/* Codec input devices */
#define INPUT_DEVICE_MIC1             0x10

/* Volume Levels values */
#define DEFAULT_VOLMIN                0x00
#define DEFAULT_VOLMAX                0xFF
#define DEFAULT_VOLSTEP               0x04

#define AUDIO_PAUSE                   0
#define AUDIO_RESUME                  1

/* Codec POWER DOWN modes */
#define CODEC_PDWN_HW                 1
#define CODEC_PDWN_SW                 2

/* MUTE commands */
#define AUDIO_MUTE_ON                 1
#define AUDIO_MUTE_OFF                0

/* AUDIO FREQUENCY */
#define AUDIO_FREQUENCY_192K          ((uint32_t)192000)
#define AUDIO_FREQUENCY_96K           ((uint32_t)96000)
#define AUDIO_FREQUENCY_48K           ((uint32_t)48000)
#define AUDIO_FREQUENCY_44K           ((uint32_t)44100)
#define AUDIO_FREQUENCY_32K           ((uint32_t)32000)
#define AUDIO_FREQUENCY_22K           ((uint32_t)22050)
#define AUDIO_FREQUENCY_16K           ((uint32_t)16000)
#define AUDIO_FREQUENCY_11K           ((uint32_t)11025)
#define AUDIO_FREQUENCY_8K            ((uint32_t)8000)  

/******************************************************************************/
/****************************** REGISTER MAPPING ******************************/
/******************************************************************************/
/** 
  * @brief  CS42L51 ID  
  */  
#define  CS42L51_ID            0xD8
#define  CS42L51_ID_MASK       0xF8
/**
  * @brief Chip ID Register: Chip I.D. and Revision Register
  *  Read only register
  *  Default value: 0x01
  *  [7:3] CHIPID[4:0]: I.D. code for the CS42L51.
  *        Default value: 11100b
  *  [2:0] REVID[2:0]: CS42L51 revision level.
  *        Default value: 
  *        000 - Rev A0
  *        001 - Rev A1
  *        010 - Rev B0
  *        011 - Rev B1
  */
#define CS42L51_CHIPID_ADDR    0x01

/**
  * @}
  */ 

/** @defgroup CS42L51_Exported_Macros
  * @{
  */
#define VOLUME_CONVERT(Volume)    ((Volume >= 100) ? 0 : ((uint8_t)(((Volume * 2) + 56))))
/**
  * @}
  */ 

/** @defgroup CS42L51_Exported_Functions
  * @{
  */
    
/*------------------------------------------------------------------------------
                           Audio Codec functions 
------------------------------------------------------------------------------*/
/* High Layer codec functions */
uint32_t cs42l51_Init(uint16_t DeviceAddr, uint16_t Device, uint8_t Volume, uint32_t AudioFreq);
void     cs42l51_DeInit(void);
uint32_t cs42l51_ReadID(uint16_t DeviceAddr);
uint32_t cs42l51_Play(uint16_t DeviceAddr, uint16_t* pBuffer, uint16_t Size);
uint32_t cs42l51_Pause(uint16_t DeviceAddr);
uint32_t cs42l51_Resume(uint16_t DeviceAddr);
uint32_t cs42l51_Stop(uint16_t DeviceAddr, uint32_t Cmd);
uint32_t cs42l51_SetVolume(uint16_t DeviceAddr, uint8_t Volume);
uint32_t cs42l51_SetFrequency(uint16_t DeviceAddr, uint32_t AudioFreq);
uint32_t cs42l51_SetMute(uint16_t DeviceAddr, uint32_t Cmd);
uint32_t cs42l51_SetOutputMode(uint16_t DeviceAddr, uint8_t Output);
uint32_t cs42l51_Reset(uint16_t DeviceAddr);

/* AUDIO IO functions */
void      AUDIO_IO_Init(void);
void      AUDIO_IO_DeInit(void);
void      AUDIO_IO_Write(uint8_t Addr, uint8_t Reg, uint8_t Value);
uint8_t   AUDIO_IO_Read(uint8_t Addr, uint8_t Reg);
void      AUDIO_IO_Delay(uint32_t Delay);

/* Audio driver structure */
extern AUDIO_DrvTypeDef   cs42l51_drv;

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

#endif /* __CS42L51_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
