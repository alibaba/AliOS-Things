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

#define AC101_I2C_ADDR          	0x1a
#define AC102_I2C_ADDR1          	0x33 /* when PIN DEVID is low */
#define AC102_I2C_ADDR2          	0x30 /* when PIN DEVID is high */

#define CODEC_DIR_OUT				(0)
#define CODEC_DIR_IN				(1)

/**
  * @brief The param for PA control.
  */
typedef struct {
	GPIO_Port     ctrl_port;
	GPIO_Pin      ctrl_pin;
	GPIO_PinState ctrl_on_state;
	GPIO_PinState ctrl_off_state;
	uint16_t	  ctrl_on_delay;
	uint16_t	  ctrl_off_delay;
} SPK_Param;

/**
  * @brief The param for LINEIN control.
  */
typedef struct {
	GPIO_Port     detect_port;
	GPIO_Pin      detect_pin;
	GPIO_PinState insert_state;
} LINEIN_Param;

/**
  * @brief Codec detect callback.
  */
typedef void (*codec_detect_cb)(uint32_t present);

#define AUDIO_IN_DEV_SHIFT		(0)
#define AUDIO_OUT_DEV_SHIFT		(8)

/**
  * @brief Audio device structures definition.
  */
typedef enum {
	AUDIO_IN_DEV_MAINMIC		= HAL_BIT(AUDIO_IN_DEV_SHIFT),		/*!< Main Mic */
	AUDIO_IN_DEV_HEADPHONEMIC	= HAL_BIT(AUDIO_IN_DEV_SHIFT + 1),	/*!< Headphone Mic */
	AUDIO_IN_DEV_LINEIN			= HAL_BIT(AUDIO_IN_DEV_SHIFT + 2),	/*!< Line In */
	AUDIO_IN_DEV_ALL	   		= (0x07 << AUDIO_IN_DEV_SHIFT),

	AUDIO_OUT_DEV_HEADPHONE		= HAL_BIT(AUDIO_OUT_DEV_SHIFT),		/*!< Headphone */
	AUDIO_OUT_DEV_SPEAKER		= HAL_BIT(AUDIO_OUT_DEV_SHIFT + 1),	/*!< Speaker */
	AUDIO_OUT_DEV_ALL			= (0x03 << AUDIO_OUT_DEV_SHIFT),

	AUDIO_DEVICE_NUM			= 5
} AUDIO_Device;

#define AUDIO_DEV_ALL			(AUDIO_IN_DEV_ALL | AUDIO_OUT_DEV_ALL)

/**
  * @brief For compatibility.
  */
#define AUDIO_DEVICE_MAINMIC		AUDIO_IN_DEV_MAINMIC
#define AUDIO_DEVICE_HEADPHONEMIC	AUDIO_IN_DEV_HEADPHONEMIC
#define AUDIO_DEVICE_LINEIN			AUDIO_IN_DEV_LINEIN
#define AUDIO_DEVICE_HEADPHONE		AUDIO_OUT_DEV_HEADPHONE
#define AUDIO_DEVICE_SPEAKER		AUDIO_OUT_DEV_SPEAKER

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

typedef enum {
	CODEC_LIFT,
	CODEC_RIGHT,
} CODEC_Ch;

typedef enum {
	AUDIO_CODEC_AC101,
	AUDIO_CODEC_AC102,
	AUDIO_CODEC_NONE,
} AUDIO_CODEC_Type;

typedef enum {
	CODEC_MIC_ANALOG,
	CODEC_MIC_DIGITAL,
} CODEC_MIC_Type;

/**
 * @brief CODEC initialization parameters
 */
typedef struct {
	codec_detect_cb		cb; /* codec callback */
} CODEC_InitParam;

typedef struct {
	AUDIO_CODEC_Type type;       		/*!< Type of specific audio codec    */
	uint8_t			 i2cAddr;			/*!< I2C address of audio codec   */
} CODEC_DetectParam;

/**
  * @brief  Codec Gain parameters
  */
typedef struct {
	uint8_t        speaker_double_used;    /*!< Flag of speaker case (double or single)    */
	uint8_t        double_speaker_val;     /*!< Volume gain of double speaker    */
	uint8_t        single_speaker_val;     /*!< Volume gain of single speaker    */
	CODEC_Ch	   single_speaker_ch;
	uint8_t        headset_val;            /*!< Volume gain of headset    */
	CODEC_MIC_Type mainmic_type;		   /*!< Type of main mic (analog or digital)    */
	uint8_t        mainmic_analog_val;     /*!< Analog volume gain of main mic    */
	uint8_t		   mainmic_digital_val;	   /*!< Digital volume gain of main mic    */
	uint8_t        headsetmic_val;         /*!< Volume gain of headset mic    */
} CODEC_HWParam;

/**
  * @brief  Codec device set
  */
typedef enum {
	CODEC_DEV_DISABLE	= 0,
	CODEC_DEV_ENABLE	= 1
} CODEC_DevState;

/**
  * @brief  Data format Init parameters
  */
typedef struct {
	uint32_t         sampleRate;    /*!< Sample rate for the stream data    */
	uint8_t			 isDevEnable;	/*!< Mark the device enable status    */
	uint8_t			 direction;		/*!< Mark the direction of codec data    */
	uint8_t			 mixMode;
	AUDIO_Device     audioDev;      /*!< Audio device to play or capture    */
	DAI_FmtParam     *fmtParam;     /*!< Parameters for Pcm transfer initialization    */
} DATA_Param;

typedef int32_t (*hw_write)(I2C_ID i2cId, uint16_t devAddr, uint32_t memAddr, I2C_MemAddrSize memAddrSize, uint8_t *buf, int32_t size);
typedef int32_t (*hw_read)(I2C_ID i2cId, uint16_t devAddr, uint32_t memAddr, I2C_MemAddrSize memAddrSize, uint8_t *buf, int32_t size);

/**
  * @brief  CODEC Param Init structure definition
  */
typedef struct {
	AUDIO_CODEC_Type type;       		/*!< Type of specific audio codec    */
	hw_write         write;     		/*!< I2C write function    */
	hw_read          read;        		/*!< I2C read function    */
	const CODEC_HWParam  *param;      /*!< Parameters for codec gain initialization     */
	uint8_t          i2cId;      		/*!< Index of I2C for control codec   */
	uint8_t			 i2cAddr;			/*!< I2C address of audio codec   */
	const SPK_Param  *spk_cfg;
	const LINEIN_Param  *linein_cfg;
	uint8_t			 output_stable_time;
} CODEC_Param;

HAL_Status HAL_CODEC_DeInit();
HAL_Status HAL_CODEC_Init(CODEC_InitParam *initParam);
HAL_Status HAL_CODEC_Close(uint32_t dir);
HAL_Status HAL_CODEC_Open(DATA_Param *param);
HAL_Status HAL_CODEC_VOLUME_LEVEL_Set(AUDIO_Device dev, uint8_t volume);
HAL_Status HAL_CODEC_ROUTE_Set(AUDIO_Device dev, CODEC_DevState state);
HAL_Status HAL_CODEC_Mute(AUDIO_Device dev, uint8_t mute);
HAL_Status HAL_CODEC_Trigger(AUDIO_Device dev, uint8_t on);
uint32_t HAL_CODEC_MUTE_STATUS_Get();
HAL_Status HAL_CODEC_MUTE_STATUS_Init(int status);
HAL_Status HAL_CODEC_INIT_VOLUME_Set(AUDIO_Device dev, uint8_t volume);
HAL_Status HAL_CODEC_TYPE_Get(I2C_ID i2cID, CODEC_DetectParam *detect_param, uint8_t doDetect);
HAL_Status HAL_CODEC_EQ_SCENE_Set(uint8_t scene);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_CODEC_H_ */
