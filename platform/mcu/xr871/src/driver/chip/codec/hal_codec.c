/**
  * @file  hal_codec.c
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

#include "../hal_base.h"
#include "sys/io.h"
#include "string.h"
#include "pm/pm.h"
#include "codec.h"

#define CODEC_DBG_ON                0

#if (CODEC_DBG_ON == 1)
#define CODEC_DEBUG(fmt, arg...)    HAL_LOG(CODEC_DBG_ON, "[CODEC] "fmt, ##arg)
#else
#define CODEC_DEBUG(fmt, arg...)
#endif
#define CODEC_ERROR(fmt, arg...)    HAL_LOG(1, "[CODEC] "fmt, ##arg)

typedef struct {
	AUDIO_CODEC_Type     type;
	uint8_t              i2cId;
	uint8_t              devAddr;
	uint8_t              RegLength;
	uint8_t              RegValLength;
	/*dir*/
	uint8_t              playBack;
	uint8_t              record;

	struct codec_ops *   ops;
	struct codec_dai_ops *dai_ops;
	struct codec_ctl_ops *ctl_ops;
	hw_write             write;
	hw_read              read;
	uint32_t             playDev;
	uint32_t             recordDev;
	uint32_t             sampleRate;
	DAI_FmtParam         *fmtParam;
	const CODEC_HWParam *initParam;
	const SPK_Param      *spk_cfg;
	const LINEIN_Param	 *line_cfg;
	uint8_t 			 output_stable_time;
	codec_detect_cb		 cd_cb;
	HAL_Mutex            Lock;
} CODEC_Priv;

struct CODECS {
	AUDIO_CODEC_Type    type;
	CODEC      			*dev;
};

#define SPEAKER_DOUBLE_USED             1
#define D_SPEAKER_VOL                   0x10
#define S_SPEAKER_VOL                   0x10
#define HEADPHONE_VOL                   0x2b
#define MAINMIC_ANALOG_GAIN             0x3
#define MAINMIC_DIGITAL_GAIN            0x81
#define HDSETMIC_GAIN                   0x4
#define ADC_DIGITAL_GAIN                0xb0b0

/*
 * Default format initialization parameter
 */
static DAI_FmtParam gFmtParam = {
	DAIFMT_CBS_CFS,
	DAIFMT_I2S,
	DAIFMT_NB_NF,
	32,            /*16,32,64,128,256*/
	16,
	24576000,
	24576000,
	MCLK1
};

/*
 * Default gain initialization parameter
 */
static CODEC_HWParam gInitParam = {
	.speaker_double_used = SPEAKER_DOUBLE_USED,
	.double_speaker_val = D_SPEAKER_VOL,
	.single_speaker_val = S_SPEAKER_VOL,
	.single_speaker_ch = CODEC_LIFT,
	.headset_val = HEADPHONE_VOL,
	.mainmic_type = CODEC_MIC_ANALOG,
	.mainmic_analog_val = MAINMIC_ANALOG_GAIN,
	.mainmic_digital_val = MAINMIC_DIGITAL_GAIN,
	.headsetmic_val = HDSETMIC_GAIN,
};

/*
 * Default volume initialization parameter
 */
volatile uint32_t g_init_volume_value = 14;

/*
 * Default mute status initialization parameter
 */
volatile uint32_t g_init_mute_status = 0;

static CODEC_Priv gCodecPriv;
extern CODEC AC101, AC102;

static const struct CODECS codecs[] = {
	{AUDIO_CODEC_AC101, &AC101},
	{AUDIO_CODEC_AC102, &AC102},
	{AUDIO_CODEC_NONE, NULL},
};

int32_t snd_soc_read(uint32_t reg)
{
	int16_t ret = 0;
	uint8_t val[5];
	CODEC_Priv *priv = &gCodecPriv;
	int8_t regValLength = 0;

	if (priv->RegValLength == CODEC_I2C_REGVAL_LENGTH16)
		regValLength = 2;
	else if (priv->RegValLength == CODEC_I2C_REGVAL_LENGTH8)
		regValLength = 1;
	else
		return -1;

	if (priv->RegLength != CODEC_I2C_REG_LENGTH8)
		return -1;

	ret = priv->read(priv->i2cId, priv->devAddr, reg, I2C_MEMADDR_SIZE_8BIT, val, regValLength);
	if (ret != regValLength)
		return -1;

	uint16_t *reg_val = (uint16_t *)val;
	if (regValLength == CODEC_I2C_REGVAL_LENGTH16)
		return ((((*reg_val)&0xFF) << 8) |(((*reg_val)&0xFF00)>>8));
	else if(regValLength == CODEC_I2C_REGVAL_LENGTH8)
		return ((*reg_val)&0xFF);
	else
		return -1;
}

int32_t snd_soc_write(uint32_t reg, uint32_t reg_val)
{
	uint32_t ret;
	CODEC_Priv *priv = &gCodecPriv;
	uint8_t val[5];

	int8_t regValLength = 0;

	if (priv->RegLength != CODEC_I2C_REG_LENGTH8)
		return -1;

	if (priv->RegValLength == CODEC_I2C_REGVAL_LENGTH16) {
		regValLength = 2;
		val[0] = (reg_val & 0xFF00) >> 8;
		val[1] = reg_val & 0xFF;
	} else if (priv->RegValLength == CODEC_I2C_REGVAL_LENGTH8) {
		regValLength = 1;
		val[0] = reg_val & 0xFF;
	} else
		return -1;

	ret = priv->write(priv->i2cId, priv->devAddr, reg, I2C_MEMADDR_SIZE_8BIT, val, regValLength);
	if (ret != regValLength)
		return -1;
	else
		return 0;
}

/**
  * @internal
  * @brief update codec register bits.
  * @param reg: codec register.
  * @param mask: register mask.
  * @param value: new value.
  * @retval Returns 1 for change, 0 for no change, or negative error code.
  */
int32_t snd_soc_update_bits(uint32_t reg, uint32_t mask, uint32_t value)
{
	bool change;
	uint32_t old, new;
	int ret;
	ret = snd_soc_read(reg);
	if (ret < 0)
		return ret;
	old = ret;
	new = (old & ~mask) | (value & mask);
	change = old != new;
	if (change)
		ret = snd_soc_write(reg, new);
	if (ret < 0)
		return ret;
	return change;
}

/**
  * @brief Enable or Disable output device.
  * @param AUDIO_Device: audio device.
  * @param on: enable or disable.
  * @retval HAL state
  */
HAL_Status HAL_CODEC_Trigger(AUDIO_Device dev, uint8_t on)
{
	if (dev != AUDIO_OUT_DEV_HEADPHONE && dev != AUDIO_OUT_DEV_SPEAKER)
		return HAL_ERROR;
	CODEC_Priv *priv = &gCodecPriv;

	HAL_MutexLock(&priv->Lock, OS_WAIT_FOREVER);
	if (priv->spk_cfg && dev == AUDIO_OUT_DEV_SPEAKER) {
		HAL_GPIO_WritePin(priv->spk_cfg->ctrl_port,
		                  priv->spk_cfg->ctrl_pin,
		                  on ? priv->spk_cfg->ctrl_on_state :
		                       priv->spk_cfg->ctrl_off_state);
		OS_MSleep(on ? priv->spk_cfg->ctrl_on_delay : priv->spk_cfg->ctrl_off_delay);
	} else {//headphone set in codec file

		if (priv->ctl_ops->setTrigger)
			priv->ctl_ops->setTrigger(dev, on);
	}
	HAL_MutexUnlock(&priv->Lock);

	return HAL_OK;
}

/**
  * @brief Set output device mute or unmute.
  * @param AUDIO_Device: audio device.
  * @param mute: mute flag.
  * @retval HAL state
  */
HAL_Status HAL_CODEC_Mute(AUDIO_Device dev, uint8_t mute)
{
	if (dev != AUDIO_OUT_DEV_SPEAKER && dev != AUDIO_OUT_DEV_HEADPHONE)
		return HAL_ERROR;

	CODEC_Priv *priv = &gCodecPriv;

	HAL_MutexLock(&priv->Lock, OS_WAIT_FOREVER);

	if (dev == AUDIO_OUT_DEV_SPEAKER && priv->spk_cfg) {
		HAL_GPIO_WritePin(priv->spk_cfg->ctrl_port,
		                  priv->spk_cfg->ctrl_pin,
		                  mute ? priv->spk_cfg->ctrl_off_state :
		                         priv->spk_cfg->ctrl_on_state);
	}

	if (dev == AUDIO_OUT_DEV_HEADPHONE) {
	}

	HAL_MutexUnlock(&priv->Lock);

	return HAL_OK;
}

/**
  * @brief Set current stream output device or input device.
  * @param AUDIO_Device: audio device.
  * @retval HAL state
  */
HAL_Status HAL_CODEC_ROUTE_Set(AUDIO_Device dev, CODEC_DevState state)
{
	if (!(dev & AUDIO_DEV_ALL))
		return HAL_ERROR;

	CODEC_Priv *priv = &gCodecPriv;

	HAL_MutexLock(&priv->Lock, OS_WAIT_FOREVER);
	if (priv->ctl_ops->setRoute)
		priv->ctl_ops->setRoute(dev, state);

	if (priv->spk_cfg && dev == AUDIO_OUT_DEV_SPEAKER) {
		HAL_GPIO_WritePin(priv->spk_cfg->ctrl_port,
		                  priv->spk_cfg->ctrl_pin,
		                  state == CODEC_DEV_ENABLE ?
		                  	priv->spk_cfg->ctrl_on_state :
		                  	priv->spk_cfg->ctrl_off_state);
	}
	HAL_MutexUnlock(&priv->Lock);

	return HAL_OK;
}

/**
  * @brief Set output device's volume gain when stream on.
  * @param AUDIO_Device: audio output device.
  * @param volume: the audio gain.
  * @retval HAL state
  */
HAL_Status HAL_CODEC_VOLUME_LEVEL_Set(AUDIO_Device dev, uint8_t volume)
{
	CODEC_Priv *priv = &gCodecPriv;
	if (volume > VOLUME_MAX_LEVEL)
		return HAL_INVALID;

	HAL_MutexLock(&priv->Lock, OS_WAIT_FOREVER);
	if (priv->ctl_ops->setVolume)
		priv->ctl_ops->setVolume(dev, volume);
	HAL_MutexUnlock(&priv->Lock);

	return HAL_OK;
}

/**
  * @brief Init output device's volume gain.
  * @param AUDIO_Device: audio output device.
  * @param volume: the audio gain.
  * @retval HAL state
  */
HAL_Status HAL_CODEC_INIT_VOLUME_Set(AUDIO_Device dev, uint8_t volume)
{
	CODEC_Priv *priv = &gCodecPriv;

	if (volume > VOLUME_MAX_LEVEL)
		return HAL_INVALID;

	HAL_MutexLock(&priv->Lock, OS_WAIT_FOREVER);
	g_init_volume_value = volume;
	HAL_MutexUnlock(&priv->Lock);

	return HAL_OK;
}

/**
  * @brief Init the codec mute state
  * @param status: The status of output device.
  * @retval HAL state
  */
HAL_Status HAL_CODEC_MUTE_STATUS_Init(int status)
{
	CODEC_Priv *priv = &gCodecPriv;

	HAL_MutexLock(&priv->Lock, OS_WAIT_FOREVER);
	g_init_mute_status = status;
	HAL_MutexUnlock(&priv->Lock);

	return HAL_OK;
}

/**
  * @brief  Return the codec output device(mute or unmute) state
  * @retval device state
  */
uint32_t HAL_CODEC_MUTE_STATUS_Get()
{
	return g_init_mute_status;
}

/**
  * @brief  to get the codec type (AC101 or AC102)
  * @retval device state
  */
HAL_Status HAL_CODEC_TYPE_Get(I2C_ID i2cID, CODEC_DetectParam *detect_param, uint8_t doDetect)
{
	if (detect_param == NULL)
		return HAL_ERROR;

	if (!doDetect) {
		detect_param->type = gCodecPriv.type;
		detect_param->i2cAddr = gCodecPriv.devAddr;
	} else {
		uint8_t i, val[5];
		AUDIO_CODEC_Type codec_type = AUDIO_CODEC_NONE;
		for (i = 0; i < AUDIO_CODEC_NONE; i++) {
			if (HAL_I2C_Master_Receive_Mem_IT(i2cID, codecs[i].dev->devAddr, 0, I2C_MEMADDR_SIZE_8BIT, val,
							codecs[i].dev->RegValLength) == codecs[i].dev->RegValLength) {
				if (codecs[i].dev->devAddr == AC101_I2C_ADDR) {
					codec_type = AUDIO_CODEC_AC101;
				}
				else if (codecs[i].dev->devAddr == AC102_I2C_ADDR1 || codecs[i].dev->devAddr == AC102_I2C_ADDR2) {
					codec_type = AUDIO_CODEC_AC102;
				}
				detect_param->i2cAddr = codecs[i].dev->devAddr;
				break;
			} else {
				if (codecs[i].dev->devAddr == AC102_I2C_ADDR1 || codecs[i].dev->devAddr == AC102_I2C_ADDR2) {
					uint8_t addr = codecs[i].dev->devAddr;
					addr = AC102_I2C_ADDR1 ? AC102_I2C_ADDR2 : AC102_I2C_ADDR1;
					if (HAL_I2C_Master_Receive_Mem_IT(i2cID, addr, 0, I2C_MEMADDR_SIZE_8BIT, val,
							codecs[i].dev->RegValLength) == codecs[i].dev->RegValLength) {
						codecs[i].dev->devAddr = addr;
						codec_type = AUDIO_CODEC_AC102;
						detect_param->i2cAddr = addr;
						break;
					}
				}
			}
		}

		detect_param->type = codec_type;
	}

	return HAL_OK;
}

/**
  * @brief  Set audio EQ scene
  * @retval device state
  */
HAL_Status HAL_CODEC_EQ_SCENE_Set(uint8_t scene)
{
	CODEC_Priv *priv = &gCodecPriv;
	if (scene < 0)
		return HAL_INVALID;

	HAL_MutexLock(&priv->Lock, OS_WAIT_FOREVER);
	if (priv->ctl_ops->setEqScene)
		priv->ctl_ops->setEqScene(scene);
	HAL_MutexUnlock(&priv->Lock);

	return HAL_OK;
}

/**
  * @brief Open the Codec module according to the specified parameters
  *         in the DATA_Param.
  * @param  param: pointer to a DATA_Param structure that contains
  *         data format information
  * @retval HAL status
  */
HAL_Status HAL_CODEC_Open(DATA_Param *param)
{
	if (!param)
		return HAL_INVALID;

	CODEC_DEBUG("CODEC Open..\n");

	CODEC_Priv *priv = &gCodecPriv;

	HAL_MutexLock(&priv->Lock, OS_WAIT_FOREVER);
	if (!param->fmtParam)
		priv->fmtParam = &gFmtParam;
	else
		priv->fmtParam = param->fmtParam;

	if (param->sampleRate)
		priv->sampleRate = param->sampleRate;
	else
		priv->sampleRate = 48000;

	switch (param->direction) {
		case CODEC_DIR_OUT:
			priv->playBack = 1;
			break;
		case CODEC_DIR_IN:
			priv->record = 1;
			break;
		default:
			HAL_MutexUnlock(&priv->Lock);
			return HAL_INVALID;
	}

	if (priv->dai_ops->startup)
		priv->dai_ops->startup(param->audioDev);
	if (priv->dai_ops->setPll)
		priv->dai_ops->setPll(priv->fmtParam);
	if (priv->dai_ops->setClkdiv)
		priv->dai_ops->setClkdiv(priv->fmtParam,priv->sampleRate);
	if (priv->dai_ops->setFormat)
		priv->dai_ops->setFormat(priv->fmtParam);
	if (priv->ctl_ops->setAttribute)
		priv->ctl_ops->setAttribute(param->audioDev, param->mixMode);
	if (!param->isDevEnable) {
		if (priv->ctl_ops->setRoute)
			priv->ctl_ops->setRoute(param->audioDev, 1);
		if (priv->ctl_ops->setVolume && (param->audioDev == AUDIO_OUT_DEV_HEADPHONE ||
										 param->audioDev ==	AUDIO_OUT_DEV_SPEAKER))
			priv->ctl_ops->setVolume(param->audioDev, g_init_volume_value);
	}
	HAL_MutexUnlock(&priv->Lock);

	OS_MSleep(priv->output_stable_time);

	return HAL_OK;
}

/**
  * @brief Close the Codec module
  *
  * @note The module is closed at the end of transaction to avoid power consumption
  * @retval none
  */
HAL_Status HAL_CODEC_Close(uint32_t dir)
{
	CODEC_Priv *priv = &gCodecPriv;

	CODEC_DEBUG("CODEC Close..\n");

	HAL_MutexLock(&priv->Lock, OS_WAIT_FOREVER);
	if (dir == 0) {
		priv->playBack = 0;
	} else {
		priv->record = 0;
	}
	if (priv->dai_ops->shutDown)
		priv->dai_ops->shutDown(priv->playBack | dir, priv->record | !dir);
	HAL_MutexUnlock(&priv->Lock);

	return HAL_OK;
}

#ifdef CONFIG_PM
static int codec_suspend(struct soc_device *dev, enum suspend_state_t state)
{
	CODEC_Priv *priv = &gCodecPriv;

	switch (state) {
		case PM_MODE_SLEEP:
		case PM_MODE_STANDBY:
		case PM_MODE_HIBERNATION:
		case PM_MODE_POWEROFF:

			HAL_MutexLock(&priv->Lock, OS_WAIT_FOREVER);

			if (priv->ops->setInitParam)
				priv->ops->setInitParam(HAL_CODEC_DEINIT,NULL);
			if (priv->ops->jackDetect)
				priv->ops->jackDetect(HAL_CODEC_DEINIT,NULL);
			if (priv->ops->setSysClk)
				priv->ops->setSysClk(HAL_CODEC_DEINIT,NULL);
			if (priv->ops->setPower)
				priv->ops->setPower(HAL_CODEC_DEINIT,NULL);
			HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT,
			               HAL_MKDEV(HAL_DEV_MAJOR_AUDIO_CODEC, 0), 0);

			HAL_MutexUnlock(&priv->Lock);

			break;
		default:
			break;
	}
	return 0;
}

static int codec_resume(struct soc_device *dev, enum suspend_state_t state)
{
	CODEC_Priv *priv = &gCodecPriv;
	switch (state) {
		case PM_MODE_SLEEP:
		case PM_MODE_STANDBY:
		case PM_MODE_HIBERNATION:

			HAL_MutexLock(&priv->Lock, OS_WAIT_FOREVER);

			if (priv->ops->setPower)
				priv->ops->setPower(HAL_CODEC_INIT,NULL);
			if (priv->ops->setSysClk)
				priv->ops->setSysClk(HAL_CODEC_INIT,NULL);
			if (priv->ops->setInitParam)
				priv->ops->setInitParam(HAL_CODEC_INIT,(void *)priv->initParam);
			if (priv->ops->jackDetect)
				priv->ops->jackDetect(HAL_CODEC_INIT,NULL);
			HAL_BoardIoctl(HAL_BIR_PINMUX_INIT,
			               HAL_MKDEV(HAL_DEV_MAJOR_AUDIO_CODEC, 0), 0);

			HAL_MutexUnlock(&priv->Lock);
			break;

		default:
			break;
	}
	return 0;
}

static struct soc_device_driver codec_drv = {
	.name = "CODEC",
	.suspend = codec_suspend,
	.resume = codec_resume,
};

static struct soc_device codec_dev = {
	.name = "CODEC",
	.driver = &codec_drv,
};

#define CODEC_DEV (&codec_dev)
#endif

__nonxip_text
static void __linein_cd_irq(void *arg)
{
	CODEC_Priv *priv = (CODEC_Priv *)arg;

	if (priv->line_cfg && priv->cd_cb) {
		GPIO_PinState state = HAL_GPIO_ReadPin(priv->line_cfg->detect_port,
		                                       priv->line_cfg->detect_pin);
		priv->cd_cb((state == priv->line_cfg->insert_state) ? 1 : 0);
	}
}

/**
  * @brief Initializes the CODEC according to the specified parameters
  *         in the CODEC_Param.
  * @param  param: pointer to a CODEC_Param structure that contains
  *         the configuration information for CODEC module
  * @retval HAL status
  */
HAL_Status HAL_CODEC_Init(CODEC_InitParam *initParam)
{
	HAL_Status sta;
	const CODEC_Param *param = NULL;

	sta = HAL_BoardIoctl(HAL_BIR_GET_CFG, HAL_MKDEV(HAL_DEV_MAJOR_AUDIO_CODEC, 0), (uint32_t)&param);
	if (sta != HAL_OK) {
		CODEC_ERROR("%s, %d get cfg error\n", __func__, __LINE__);
		return -1;
	}

	if (!param)
		return HAL_INVALID;

	CODEC_DEBUG("CODEC INIT..\n");

	CODEC_Priv *priv = &gCodecPriv;
	memset(priv, 0, sizeof(*priv));
	/* set i2s read/write interface */
	priv->type = param->type;
	priv->read= param->read;
	priv->write = param->write;
	priv->i2cId = param->i2cId;
	priv->devAddr = param->i2cAddr;

	if (!param->param && (param->type == AUDIO_CODEC_AC101)) {
		priv->initParam = &gInitParam;
	} else
		priv->initParam = param->param;

	int i = 0;
	CODECP codec = NULL;
	for (i = 0; i < AUDIO_CODEC_NONE; i++) {
		if (param->type == codecs[i].type) {
			codec = codecs[i].dev;
			break;
		}
	}
	if (!codec)
		return HAL_ERROR;

	CODEC_DEBUG("CODEC: %s\n", codec->type == AUDIO_CODEC_AC101 ? "ac101" : "ac102");

	HAL_MutexInit(&priv->Lock);
	priv->ops = codec->ops;
	priv->dai_ops = codec->dai_ops;
	priv->ctl_ops = codec->ctl_ops;
	priv->RegValLength = codec->RegValLength;
	priv->RegLength = codec->RegLength;
	priv->cd_cb = initParam->cb;

	HAL_MutexLock(&priv->Lock, OS_WAIT_FOREVER);
	if (priv->ops->setPower)
		priv->ops->setPower(HAL_CODEC_INIT,NULL);
	if (priv->ops->setSysClk)
		priv->ops->setSysClk(HAL_CODEC_INIT,NULL);

	if (priv->ops->setInitParam)
		priv->ops->setInitParam(HAL_CODEC_INIT,(void *)priv->initParam);

	if (priv->ops->jackDetect)
		priv->ops->jackDetect(HAL_CODEC_INIT,NULL);
#ifdef CONFIG_PM
	pm_register_ops(CODEC_DEV);
#endif
	HAL_MutexUnlock(&priv->Lock);

	priv->line_cfg = param->linein_cfg;
	priv->spk_cfg = param->spk_cfg;
	priv->output_stable_time = param->output_stable_time;

	if (priv->spk_cfg || priv->line_cfg) {
		HAL_BoardIoctl(HAL_BIR_PINMUX_INIT,
	                   HAL_MKDEV(HAL_DEV_MAJOR_AUDIO_CODEC, 0), 0);
	}

	if (priv->spk_cfg) {
		HAL_GPIO_WritePin(priv->spk_cfg->ctrl_port, priv->spk_cfg->ctrl_pin,
							priv->spk_cfg->ctrl_off_state);
	}

	if (priv->line_cfg && priv->cd_cb) {
		GPIO_IrqParam Irq_param;
		Irq_param.event = GPIO_IRQ_EVT_BOTH_EDGE;
		Irq_param.callback = __linein_cd_irq;
		Irq_param.arg = priv;
		HAL_GPIO_EnableIRQ(priv->line_cfg->detect_port, priv->line_cfg->detect_pin, &Irq_param);
	}

	return HAL_OK;
}

/**
  * @brief DeInitializes the CODEC peripheral(power, clk, jack)
  * @retval HAL status
  */
HAL_Status HAL_CODEC_DeInit()
{
	CODEC_Priv *priv = &gCodecPriv;

	HAL_MutexLock(&priv->Lock, OS_WAIT_FOREVER);
	if (priv->ops->setPower)
		priv->ops->setPower(HAL_CODEC_DEINIT,NULL);

	if (priv->ops->setSysClk)
		priv->ops->setSysClk(HAL_CODEC_DEINIT,NULL);

	if (priv->ops->setInitParam)
		priv->ops->setInitParam(HAL_CODEC_DEINIT,NULL);

	if (priv->ops->jackDetect)
		priv->ops->jackDetect(HAL_CODEC_DEINIT,NULL);
	HAL_MutexUnlock(&priv->Lock);

	HAL_MutexDeinit(&priv->Lock);
	return HAL_OK;
}
