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
#include "sys/io.h"
#include "kernel/os/os.h"
#include "../hal_base.h"
#include "codec.h"
#include "hal_ac102.h"

#define AC102_DBG_ON                1

#if (AC102_DBG_ON == 1)
#define AC102_DEBUG(fmt, arg...)    HAL_LOG(AC102_DBG_ON, "[AC102] "fmt, ##arg)
#else
#define AC102_DEBUG(fmt, arg...)
#endif
#define AC102_ERROR(fmt, arg...)    HAL_LOG(1, "[AC102] "fmt, ##arg)

#define	SYSCLK_11M					(11289600U)
#define	SYSCLK_12M					(12288000U)
#define AC102_VOLUME_MAX_LEVEL		VOLUME_LEVEL16

typedef enum {
	SYSCLK_DIV_1   = 1,
	SYSCLK_DIV_2   = 2,
	SYSCLK_DIV_3   = 3,
	SYSCLK_DIV_4   = 4,
	SYSCLK_DIV_6   = 6,
	SYSCLK_DIV_8   = 8,
	SYSCLK_DIV_12  = 12,
	SYSCLK_DIV_16  = 16,
	SYSCLK_DIV_24  = 24,
} SYS_CLKDIV;

typedef enum {
	EQ_TYPE_0  = 0,
	EQ_TYPE_1,
	EQ_TYPE_NULL,
} EQ_TYPE;

typedef struct {
	uint8_t regVal;
	SYS_CLKDIV clkDiv;
} CLK_DIVRegval;

typedef struct {
	uint32_t band_b0;
	uint32_t band_b1;
	uint32_t band_b2;
	uint32_t band_a1;
	uint32_t band_a2;
} band_coef;

typedef struct {
	band_coef band_1;
	band_coef band_2;
	band_coef band_3;
} EQ_Band;

typedef struct {
	EQ_TYPE scene;
	EQ_Band band;
} EQ_Scene;

static CLK_DIVRegval DivRegval[] = {
		{0, SYSCLK_DIV_1},
		{1, SYSCLK_DIV_2},
		{2, SYSCLK_DIV_3},
		{3, SYSCLK_DIV_4},
		{4, SYSCLK_DIV_6},
		{5, SYSCLK_DIV_8},
		{6, SYSCLK_DIV_12},
		{7, SYSCLK_DIV_16},
		{8, SYSCLK_DIV_24},
};

static EQ_Scene EqScene[] = {
		{EQ_TYPE_0, {{0x10000, 0x0, 0x0, 0x0, 0x0}, {0x010000, 0x0, 0x0, 0x0, 0x0}, {0x010000, 0x0, 0x0, 0x0, 0x0}}},
		/* BQ1: G=10, Fc=200, Q=20; BQ2: G=0, Fc=1000, Q=1; BQ3: G=0, Fc=10000, Q=1 */
		{EQ_TYPE_1, {{0x10034, 0xFFFE005E, 0xFF9B, 0xFFFE005E, 0xFFFCF}, {0x10000, 0xFFFE237B, 0xE0A1, 0xFFFE237B, 0xE0A1}, {0x010000, 0xFFFFA6A5, 0x5941, 0xFFFFA6A5, 0x5941}}},
};

static CODEC_MIC_Type mic_type = CODEC_MIC_ANALOG;

/*
 * Set speaker as the current output device.
 */
static void AC102_SetSpeaker()
{
	AC102_DEBUG("Route(PLAY): speaker..\n");
#ifdef DAC_ANA_REG25_EN
	/*enable DAC digital part*/
	snd_soc_update_bits(DAC_DIG_CTRL, (0x1<<DVCZCDEN)|(0x7<<DITHER_SGM)|
							(0x3<<DAC_PTN_SEL)|(0x1<<DAC_DIG_EN),
							(0x1<<DVCZCDEN)|(0x4<<DITHER_SGM)|(0x1<<DAC_DIG_EN));
	/*enable DAC analog part*/
	snd_soc_update_bits(DAC_ANA_CTRL1, (0x1<<DACEN)|(0X1<<VRDA_EN)|(0x1<<RSWITCH)|
	                         (0X1<<LOMUTE),(0x1<<DACEN)|(0X1<<VRDA_EN)|(0x1<<RSWITCH)|
	                         (0X1<<LOMUTE));
	/*enable LOUT*/
	snd_soc_update_bits(DAC_ANA_CTRL2, (0x1<<LINEOUTEN)|(0X1<<LINEODIFEN),
	                         (0x1<<LINEOUTEN)|(0X1<<LINEODIFEN));
#else
	/*reset*/
	//snd_soc_update_bits(CHIP_SOFT_RST, (0xFF<<0), (0x34<<0));

	/*disenable LOUT*/
	snd_soc_update_bits(SYS_FUNC_CTRL, (0x1<<DAC_ANA_OUT_EN),(0x0<<DAC_ANA_OUT_EN));

	/*enable DAC digital part*/
	snd_soc_update_bits(DAC_DIG_CTRL, (0x1<<DVCZCDEN)|(0x7<<DITHER_SGM)|
							(0x3<<DAC_PTN_SEL)|(0x1<<DAC_DIG_EN),
							(0x1<<DVCZCDEN)|(0x4<<DITHER_SGM)|(0x1<<DAC_DIG_EN));

	/*enable DAC analog part*/
	snd_soc_update_bits(SYS_FUNC_CTRL, (0x1<<DAC_PLAY_FUNC_EN)|
							(0x1<<ADC_REC_FUNC_EN)|(0x1<<DAC_ANA_OUT_EN),
							(0x1<<DAC_PLAY_FUNC_EN)|(0x1<<ADC_REC_FUNC_EN)|
							(0x1<<DAC_ANA_OUT_EN));

	/*enable LOUT differential mode*/
	snd_soc_update_bits(DAC_ANA_CTRL2, (0X1<<LINEODIFEN), (0X1<<LINEODIFEN));

#endif
}

/*
 * Set main mic as the current input device.
 */
static void AC102_SetMainMic()
{
	AC102_DEBUG("Route(cap): main mic..\n");
#ifdef WAIT_MABIA_STABLE
	/*wait for voltage stable*/
	snd_soc_update_bits(PWR_CTRL2, (0x1<<MBIAS_EN), (0x1<<MBIAS_EN));
	snd_soc_update_bits(ADC_ANA_CTRL1, (0X1<<ADC_GEN),(0X1<<ADC_GEN));
	OS_MSleep(200);
#endif
	/*enable MBIAS*/
	snd_soc_update_bits(PWR_CTRL2, (0x1<<ALDO_EN)|(0x1<<DLDO_EN)|(0x1<<MBIAS_EN)|
							(0x1<<VREF_EN)|(0x1<<IREF_EN), (0x1<<ALDO_EN)|(0x1<<DLDO_EN)|
							(0x1<<MBIAS_EN)|(0x1<<VREF_EN)|(0x1<<IREF_EN));

	/*enable ADC analog part*/
	snd_soc_update_bits(ADC_ANA_CTRL1, (0X1<<ADC_GEN),(0X1<<ADC_GEN));

	/*enable HPF*/
	snd_soc_update_bits(AGC_CTRL, (0X1<<HPF_EN)|(0X2<<AGC_HYS_SET),
	                         (0X1<<HPF_EN)|(0X1<<AGC_HYS_SET));

	/*enable ADC digital part*/
	snd_soc_update_bits(ADC_DIG_CTRL, (0x2<<ADOUT_DTS)|(0X1<<ADOUT_DLY_EN)|(0x1<<ADC_DIG_EN),
							(0x2<<ADOUT_DTS)|(0X1<<ADOUT_DLY_EN)|(0x1<<ADC_DIG_EN));
}

/*
 * Set digital mic as the current input device.
 */
static void AC102_SetDigitalMic()
{
	AC102_DEBUG("Route(cap): digital mic..\n");

	/*enable MBIAS*/
	snd_soc_update_bits(PWR_CTRL2, (0x1<<ALDO_EN)|(0x1<<DLDO_EN)|(0x1<<MBIAS_EN)|
							(0x1<<VREF_EN)|(0x1<<IREF_EN), (0x1<<ALDO_EN)|(0x1<<DLDO_EN)|
							(0x1<<MBIAS_EN)|(0x1<<VREF_EN)|(0x1<<IREF_EN));
	/*enable HPF*/
	snd_soc_update_bits(AGC_CTRL, (0X1<<HPF_EN)|(0X2<<AGC_HYS_SET),
	                         (0X1<<HPF_EN)|(0X1<<AGC_HYS_SET));

	/*enable ADC digital part*/
	snd_soc_update_bits(ADC_DIG_CTRL, (0x1<<DIG_MIC_EN)|(0X1<<ADC_DIG_EN),
							(0x1<<DIG_MIC_EN)|(0X1<<ADC_DIG_EN));
}

/*
 * Set audio output/input device.
 */
static int32_t AC102_SetRoute(AUDIO_Device device, CODEC_DevState state)
{
	switch (device) {
		case AUDIO_OUT_DEV_SPEAKER:
			AC102_SetSpeaker();
			break;
		case AUDIO_IN_DEV_MAINMIC:
			if(mic_type == CODEC_MIC_ANALOG)
				AC102_SetMainMic();
			else
				AC102_SetDigitalMic();
			break;
		default:
			break;
	}
	return HAL_OK;
}

int32_t AC102_SetVolume(AUDIO_Device dev, uint8_t volume)
{
	AC102_DEBUG("[set volume] dev(%d) volume(%d)..\n", (int)dev, (int)volume);
	if (volume > AC102_VOLUME_MAX_LEVEL) {
		AC102_DEBUG("[set volume] Wrong volume..\n");
		return HAL_INVALID;
	}

	uint32_t volume_val = volume;
	if (volume_val == 0) {
		snd_soc_update_bits(DAC_DVC, (0xFF<<DAC_DVC_VOL), (0x0<<DAC_DVC_VOL));
	} else {
		switch (dev) {
			case AUDIO_OUT_DEV_SPEAKER:
				snd_soc_update_bits(DAC_DVC, (0xFF<<DAC_DVC_VOL), (0x81<<DAC_DVC_VOL));
				snd_soc_update_bits(DAC_ANA_CTRL2, (0xF<<LINEOAMPGAIN), ((0x10 - volume_val)<<LINEOAMPGAIN));
				break;
			default:
				AC102_DEBUG("[set volume] Wrong audio out device..\n");
				return HAL_INVALID;
		}
	}

	return HAL_OK;
}

int32_t AC102_SetTrigger(AUDIO_Device dev, uint8_t on)
{
	return HAL_OK;
}

int32_t AC102_Attribute(AUDIO_Device dev, uint32_t param)
{
	return HAL_OK;
}

int32_t AC102_SetEqScene(uint8_t scene)
{
	EQ_Scene *eqScene = EqScene;

	do {
	   if (eqScene->scene == scene) {
	   		AC102_DEBUG("[set EqScene] scene(%u)..\n", scene);
	   		uint8_t i, j, reg_base;
			uint32_t *reg_val = &eqScene->band.band_1.band_b0;
	   		for (i = 0, reg_base = EQ1_B0_H; i < 3; i++, reg_base += 0x10) {
				for (j = 0; j < 5; j++) {
					snd_soc_update_bits(reg_base++, (0x7<<0), (((*reg_val >> 16) & 0x7)<<0));
					snd_soc_update_bits(reg_base++, (0xFF<<0), (((*reg_val >> 8) & 0xFF)<<0));
					snd_soc_update_bits(reg_base++, (0xFF<<0), ((*reg_val & 0xFF)<<0));
					reg_val++;
				}
			}
			break;
	   }
		if (eqScene->scene == (EQ_TYPE_NULL - 1))
			break;

	   eqScene++;
	} while (eqScene->scene < EQ_TYPE_NULL);
	snd_soc_update_bits(EQ_CTRL, (0x01<<EQ_EN), ((scene == 0 ? 0 :1)<<EQ_EN));

	return HAL_OK;
}

int32_t AC102_SetPll(DAI_FmtParam *fmtParam)
{
	return HAL_OK;
}

int32_t AC102_SetClkdiv(DAI_FmtParam *fmtParam,uint32_t sampleRate)
{
	uint8_t nadc, ndac;
	uint32_t sysclk;
	sysclk = ((sampleRate % 1000) != 0) ? SYSCLK_11M : SYSCLK_12M;
	nadc = ndac = sysclk / (128 * sampleRate);

	CLK_DIVRegval *divRegval = DivRegval;
	do {
	   if (divRegval->clkDiv == nadc) {
			snd_soc_update_bits(ADC_CLK_SET, (0xF<<NADC), (divRegval->regVal<<NADC));
			snd_soc_update_bits(DAC_CLK_SET, (0xF<<NDAC), (divRegval->regVal<<NDAC));
			break;
	   }
	   divRegval++;
	} while (divRegval->clkDiv < SYSCLK_DIV_24);

	return HAL_OK;
}

int32_t AC102_SetFotmat(DAI_FmtParam *fmtParam)
{
	return HAL_OK;
}

int32_t AC102_ShutDown(bool playOn, bool recordOn)
{
	if (playOn == 0 && recordOn == 0) {
		snd_soc_update_bits(DAC_DIG_CTRL, (0x1<<DAC_DIG_EN), (0x0<<DAC_DIG_EN));
		#ifdef DAC_ANA_REG25_EN
		snd_soc_update_bits(DAC_ANA_CTRL1, (0x1<<DACEN)|(0X1<<LOMUTE),
								(0x0<<DACEN)|(0X0<<LOMUTE));
		#else
		/*enable DAC analog part*/
		snd_soc_update_bits(SYS_FUNC_CTRL, (0x1<<DAC_PLAY_FUNC_EN)|
								(0x1<<ADC_REC_FUNC_EN)|(0x1<<AGC_GEN)|(0x1<<DAC_ANA_OUT_EN),
								(0x1<<DAC_PLAY_FUNC_EN)|(0x1<<ADC_REC_FUNC_EN)|
								(0x1<<AGC_GEN)|(0x0<<DAC_ANA_OUT_EN));
		#endif

		snd_soc_update_bits(PWR_CTRL2, (0x1<<MBIAS_EN), (0x0<<MBIAS_EN));
		snd_soc_update_bits(ADC_DIG_CTRL, (0x1<<ADC_DIG_EN),(0x0<<ADC_DIG_EN));
		snd_soc_update_bits(ADC_ANA_CTRL1, (0X1<<ADC_GEN), (0X0<<ADC_GEN));

	} else if (playOn == 0) {
		snd_soc_update_bits(DAC_DIG_CTRL, (0x1<<DAC_DIG_EN), (0x0<<DAC_DIG_EN));
		#ifdef DAC_ANA_REG25_EN
		snd_soc_update_bits(DAC_ANA_CTRL1, (0x1<<DACEN)|(0X1<<LOMUTE),
								(0x0<<DACEN)|(0X0<<LOMUTE));
		#else
		/*enable DAC analog part*/
		snd_soc_update_bits(SYS_FUNC_CTRL, (0x1<<VREF_SPUP_STA)|(0x1<<DAC_PLAY_FUNC_EN)|
								(0x1<<ADC_REC_FUNC_EN)|(0x1<<AGC_GEN)|(0x1<<DAC_ANA_OUT_EN),
								(0x0<<VREF_SPUP_STA)|(0x1<<DAC_PLAY_FUNC_EN)|(0x1<<ADC_REC_FUNC_EN)|
								(0x1<<AGC_GEN)|(0x0<<DAC_ANA_OUT_EN));
		#endif
	} else if (recordOn == 0) {
		snd_soc_update_bits(PWR_CTRL2, (0x1<<MBIAS_EN), (0x0<<MBIAS_EN));
		snd_soc_update_bits(ADC_DIG_CTRL, (0x1<<ADC_DIG_EN),(0x0<<ADC_DIG_EN));
		snd_soc_update_bits(ADC_ANA_CTRL1, (0X1<<ADC_GEN), (0X0<<ADC_GEN));
	} else {
		return HAL_INVALID;
	}
	return HAL_OK;
}

int32_t AC102_SetPower(CODEC_Req req, void *arg)
{
	return HAL_OK;
}

int32_t AC102_SetSysClk(CODEC_Req req, void *arg)
{
	return HAL_OK;
}

int32_t AC102_SetInitParam(CODEC_Req req, void *arg)
{
	snd_soc_update_bits(SYS_FUNC_CTRL, (0x1<<DAC_ANA_OUT_EN),(0x0<<DAC_ANA_OUT_EN));
	snd_soc_update_bits(CHIP_SOFT_RST, (0xFF<<0), (0x34<<0));

	if (req == HAL_CODEC_INIT) {
		CODEC_HWParam *param = (CODEC_HWParam *)arg;
		if (param) {
			mic_type = param->mainmic_type;
			snd_soc_update_bits(ADC_ANA_CTRL1, (0x1F<<PGA_GAIN_CTRL),
                         (param->mainmic_analog_val<<PGA_GAIN_CTRL));
			snd_soc_update_bits(ADC_DVC, (0xFF<<ADC_DVC_VOL), (param->mainmic_digital_val<<ADC_DVC_VOL));
		}
	}

	return HAL_OK;
}

int32_t AC102_JackDetect(CODEC_Req req, void *arg)
{
	return HAL_OK;
}

const struct codec_ctl_ops ac102_ctl_ops =  {
	.setRoute  	 	= AC102_SetRoute,
	.setVolume  	= AC102_SetVolume,
	.setTrigger 	= AC102_SetTrigger,
	.setEqScene 	= AC102_SetEqScene,
	.setAttribute	= AC102_Attribute,
};

const struct codec_dai_ops ac102_dai_ops =  {
	.setPll     = AC102_SetPll,
	.setClkdiv  = AC102_SetClkdiv,
	.setFormat  = AC102_SetFotmat,
	.shutDown   = AC102_ShutDown,
};

const struct codec_ops ac102_ops =  {
	.setPower      = AC102_SetPower,
	.setSysClk     = AC102_SetSysClk,
	.setInitParam  = AC102_SetInitParam,
	.jackDetect    = AC102_JackDetect,
};

CODEC AC102 = {AUDIO_CODEC_AC102,AC102_I2C_ADDR1,CODEC_I2C_REG_LENGTH8,CODEC_I2C_REGVAL_LENGTH8,
                    (struct codec_ops *)&ac102_ops,(struct codec_dai_ops *)&ac102_dai_ops, (struct codec_ctl_ops*)&ac102_ctl_ops};
