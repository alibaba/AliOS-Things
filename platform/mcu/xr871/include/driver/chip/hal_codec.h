/**
  * @file  hal_codec.h
  * @author  XRADIO IOT WLAN Team
  */

/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DRIVER_CHIP_HAL_CODEC_H_
#define _DRIVER_CHIP_HAL_CODEC_H_

#include <stdbool.h>
#include "driver/chip/hal_gpio.h"
#include "driver/chip/hal_i2c.h"
#include "driver/chip/hal_audio.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
  * @brief The param for PA control.
  */
typedef struct {
	GPIO_Port     ctrl_port;
	GPIO_Pin      ctrl_pin;
	GPIO_PinState ctrl_on_state;
	GPIO_PinState ctrl_off_state;
} SPK_Param;

/**
  * @brief Audio device structures definition.
  */
typedef enum {
	AUDIO_DEVICE_HEADPHONE       = 1,  /*!< Headphone    */
	AUDIO_DEVICE_SPEAKER,              /*!< Speaker    */
	AUDIO_DEVICE_HEADPHONEMIC,         /*!< Headphone Mic    */
	AUDIO_DEVICE_MAINMIC,              /*!< Main Mic    */
	AUDIO_DEVICE_NONE                  /*!< none    */
} AUDIO_Device;

/**
  * @brief Audio volume gain structures definition.
  */
typedef enum {
	VOLUME_LEVEL0,
	VOLUME_LEVEL1,
	VOLUME_LEVEL2,
	VOLUME_LEVEL3,
	VOLUME_LEVEL4,
	VOLUME_LEVEL5,
	VOLUME_LEVEL6,
	VOLUME_LEVEL7,
	VOLUME_LEVEL8,
	VOLUME_LEVEL9,
	VOLUME_LEVEL10,
	VOLUME_LEVEL11,
	VOLUME_LEVEL12,
	VOLUME_LEVEL13,
	VOLUME_LEVEL14,
	VOLUME_LEVEL15,
	VOLUME_LEVEL16,
	VOLUME_LEVEL17,
	VOLUME_LEVEL18,
	VOLUME_LEVEL19,
	VOLUME_LEVEL20,
	VOLUME_LEVEL21,
	VOLUME_LEVEL22,
	VOLUME_LEVEL23,
	VOLUME_LEVEL24,
	VOLUME_LEVEL25,
	VOLUME_LEVEL26,
	VOLUME_LEVEL27,
	VOLUME_LEVEL28,
	VOLUME_LEVEL29,
	VOLUME_LEVEL30,
	VOLUME_LEVEL31,
	VOLUME_MAX_LEVEL = VOLUME_LEVEL31,
} Vollevel;

/**
  * @brief  Pcm communication parameters
  */
typedef struct {
	PCM_ClkMode             clkMode;         /*!< Specifies the pcm clk mode    */
	PCM_TranFmt             transferFormat;  /*!< Specifies the pcm operating mode    */
	PCM_SignalInv           signalInterval;  /*!< Specifies the pcm Clock Polarity    */
	uint32_t                slotWidth;       /*!< Specifies the pcm slot width    */
	uint32_t                wordWidth;       /*!< Specifies the data format for the communication    */
	uint32_t                freqIn;          /*!< Specifies the external input clock frequency    */
	uint32_t                freqOut;         /*!< Specifies the clock the module work properly    */
	uint32_t                pllId;
} DAI_FmtParam;

/**
  * @brief  Codec Gain parameters
  */
typedef struct {
	uint8_t        speaker_double_used;    /*!< Flag of speaker case (double or single)    */
	uint8_t        double_speaker_val;     /*!< Volume gain of double speaker    */
	uint8_t        single_speaker_val;     /*!< Volume gain of single speaker    */
	uint8_t        headset_val;            /*!< Volume gain of headset    */
	uint8_t        mainmic_val;            /*!< Volume gain of main mic    */
	uint8_t        headsetmic_val;         /*!< Volume gain of headset mic    */
} CODEC_InitParam;

/**
  * @brief  Data format Init parameters
  */
typedef struct {
	uint32_t         sampleRate;    /*!< Sample rate for the stream data    */
	AUDIO_Device     audioDev;      /*!< Audio device to play or capture    */
	DAI_FmtParam     *fmtParam;     /*!< Parameters for Pcm transfer initialization    */
} DATA_Param;

typedef int32_t (*hw_write)(I2C_ID i2cId, uint16_t devAddr, uint8_t memAddr, uint8_t *buf, int32_t size);
typedef int32_t (*hw_read)(I2C_ID i2cId, uint16_t devAddr, uint8_t memAddr, uint8_t *buf, int32_t size);

/**
  * @brief  CODEC Param Init structure definition
  */
typedef struct {
	uint8_t          *name;       /*!< Name of specific codec    */
	hw_write         write;       /*!< I2C write function    */
	hw_read          read;        /*!< I2C read function    */
	CODEC_InitParam  *param;      /*!< Parameters for codec gain initialization     */
	uint8_t          i2cId;       /*!< Index of I2C for control codec   */
} CODEC_Param;

HAL_Status HAL_CODEC_DeInit();
HAL_Status HAL_CODEC_Init(const CODEC_Param *param);
HAL_Status HAL_CODEC_Close(uint32_t dir);
HAL_Status HAL_CODEC_Open(DATA_Param *param);
HAL_Status HAL_CODEC_VOLUME_LEVEL_Set(AUDIO_Device dev,int volume);
HAL_Status HAL_CODEC_ROUTE_Set(AUDIO_Device dev);
HAL_Status HAL_CODEC_Mute(AUDIO_Device dev, uint32_t mute);
HAL_Status HAL_CODEC_Trigger(AUDIO_Device dev, uint32_t on);
uint32_t HAL_CODEC_MUTE_STATUS_Get();
HAL_Status HAL_CODEC_MUTE_STATUS_Init(int status);
HAL_Status HAL_CODEC_INIT_VOLUME_Set(AUDIO_Device dev,int volume);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_CODEC_H_ */
