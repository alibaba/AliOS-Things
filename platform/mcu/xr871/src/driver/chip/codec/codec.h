/**
  * @file  codec.h
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

#ifndef _DRIVER_CHIP_CODEC_H_
#define _DRIVER_CHIP_CODEC_H_

#include "driver/chip/hal_codec.h"

#ifdef __cplusplus
 extern "C" {
#endif

typedef struct {
	uint8_t level;
	uint8_t reg_val;
} Volume;

typedef enum {
	HAL_CODEC_INIT      = 0,
	HAL_CODEC_DEINIT
} CODEC_Req;

/**
  * @brief DAI pll/clock/format configuration, all optional..
  *        Called by codec drivers.
  */
struct codec_dai_ops {
	int32_t (*startup)(AUDIO_Device device);
	int32_t (*setPll)(DAI_FmtParam *fmtParam);
	int32_t (*setClkdiv)(DAI_FmtParam *fmtParam,uint32_t sampleRate);
	int32_t (*setFormat)(DAI_FmtParam *fmtParam);
	int32_t (*shutDown)(bool playOn, bool recordOn);
};

/**
  * @brief Stream path/volume/trigger configuration, all optional.
  *        Called by codec drivers.
  */
struct codec_ctl_ops {
	int32_t (*setRoute)(AUDIO_Device device);
	int32_t (*setVolume)(AUDIO_Device dev, uint32_t volume);
	int32_t (*setTrigger)(AUDIO_Device dev, uint32_t on);
};

/**
  * @brief Stream path/volume/trigger configuration, all optional.
  *        Called by codec drivers.
  */
struct codec_ops {
	int32_t (*setPower)(CODEC_Req req, void *arg);
	int32_t (*setSysClk)(CODEC_Req req, void *arg);
	int32_t (*setInitParam)(CODEC_Req req, void *arg);
	int32_t (*jackDetect)(CODEC_Req req, void *arg);
};

/**
  * @brief  Codec info structure definition
  */
typedef struct {
	uint8_t                name[10];     /*!< Name of codec    */
	uint8_t                devAddr;      /*!< Address of I2C device    */
	uint8_t                RegLength;    /*!< I2C registers length    */
	uint8_t                RegValLength; /*!< I2C registers values length    */
	struct codec_ops       *ops;         /*!< Init the low level hardware : GPIO, CLOCK, POWER, JACK...etc     */
	struct codec_dai_ops   *dai_ops;     /*!< Init codec pcm interface     */
	struct codec_ctl_ops   *ctl_ops;     /*!< Config stream volume and path    */
} CODEC,*CODECP;

int32_t snd_soc_read(uint32_t reg);
int32_t snd_soc_write(uint32_t reg, uint32_t reg_val);
int32_t snd_soc_update_bits(uint32_t reg, uint32_t mask,uint32_t value);

#define CODEC_I2C_REG_LENGTH8               1
#define CODEC_I2C_REGVAL_LENGTH8            1
#define CODEC_I2C_REGVAL_LENGTH16           2

#define MCLK1                               1
#define BCLK1                               2

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_CODEC_H_ */
