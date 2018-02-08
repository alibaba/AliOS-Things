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
#include "hal_ac101.h"

#define AC101_DBG_ON                1

#if (AC101_DBG_ON == 1)
#define AC101_DEBUG(fmt, arg...)    HAL_LOG(AC101_DBG_ON, "[AC101] "fmt, ##arg)
#else
#define AC101_DEBUG(fmt, arg...)
#endif
#define AC101_ERROR(fmt, arg...)    HAL_LOG(1, "[AC101] "fmt, ##arg)

/* PLL divisors */
typedef struct {
	uint32_t        pll_in;
	uint32_t        pll_out;
	uint32_t        m;
	uint32_t        n_i;
	uint32_t        n_f;
} PLL_Div;

typedef struct {
	uint32_t        samplerate;
	uint32_t        aif1_bclk_div;
	uint32_t        aif1_srbit;
} AIF1_Fs;

typedef struct {
	uint32_t        aif1_lrlk_div;
	uint32_t        aif1_lrlk_bit;
} AIF1_Lrck;

typedef struct {
	uint32_t        aif1_wsize_val;
	uint32_t        aif1_wsize_bit;
} AIF1_WordSize;

#define AGC_ENABLE              0
#define DRC_ENABLE              0
#define AC101_I2C_ADDR          0x1a

/*
 * Note : pll code from original tdm/i2s driver.
 * 	  freq_out = freq_in * N/(m*(2k+1)) , k=1,N=N_i+N_f,N_f=factor*0.2;
 */
static const PLL_Div codec_pll_div[] = {
	{128000, 22579200, 1, 529, 1},
	{192000, 22579200, 1, 352, 4},
	{256000, 22579200, 1, 264, 3},
	{384000, 22579200, 1, 176, 2},/*((176+2*0.2)*6000000)/(38*(2*1+1))*/
	{6000000, 22579200, 38, 429, 0},/*((429+0*0.2)*6000000)/(38*(2*1+1))*/
	{13000000, 22579200, 19, 99, 0},
	{19200000, 22579200, 25, 88, 1},
	{128000, 24576000, 1, 576, 0},
	{192000, 24576000, 1, 384, 0},
	{256000, 24576000, 1, 288, 0},
	{384000, 24576000, 1, 192, 0},
	{2048000, 24576000, 1, 36, 0},
	{6000000, 24576000, 25, 307, 1},
	{13000000, 24576000, 42, 238, 1},
	{19200000, 24576000, 25, 88, 1},
};

static const AIF1_Fs codec_aif1_fs[] = {
	{44100, 4, 7},
	{48000, 4, 8},
	{8000, 9, 0},
	{11025, 8, 1},
	{12000, 8, 2},
	{16000, 7, 3},
	{22050, 6, 4},
	{24000, 6, 5},
	{32000, 5, 6},
	{96000, 2, 9},
	{192000, 1, 10},
};

static const AIF1_Lrck codec_aif1_lrck[] = {
	{16, 0},
	{32, 1},
	{64, 2},
	{128, 3},
	{256, 4},
};

static const AIF1_WordSize codec_aif1_wsize[] = {
	{8, 0},
	{16, 1},
	{20, 2},
	{24, 3},
};

static Volume spk_vol[] = {
	{VOLUME_LEVEL0, 0},
	{VOLUME_LEVEL1, 1},
	{VOLUME_LEVEL2, 2},
	{VOLUME_LEVEL3, 3},
	{VOLUME_LEVEL4, 4},
	{VOLUME_LEVEL5, 5},
	{VOLUME_LEVEL6, 6},
	{VOLUME_LEVEL7, 7},
	{VOLUME_LEVEL8, 8},
	{VOLUME_LEVEL9, 9},
	{VOLUME_LEVEL10, 10},
	{VOLUME_LEVEL11, 11},
	{VOLUME_LEVEL12, 12},
	{VOLUME_LEVEL13, 13},
	{VOLUME_LEVEL14, 14},
	{VOLUME_LEVEL15, 15},
	{VOLUME_LEVEL16, 16},
	{VOLUME_LEVEL17, 17},
	{VOLUME_LEVEL18, 18},
	{VOLUME_LEVEL19, 19},
	{VOLUME_LEVEL20, 20},
	{VOLUME_LEVEL21, 21},
	{VOLUME_LEVEL22, 22},
	{VOLUME_LEVEL23, 23},
	{VOLUME_LEVEL24, 24},
	{VOLUME_LEVEL25, 25},
	{VOLUME_LEVEL26, 26},
	{VOLUME_LEVEL27, 27},
	{VOLUME_LEVEL28, 28},
	{VOLUME_LEVEL29, 29},
	{VOLUME_LEVEL30, 30},
	{VOLUME_LEVEL31, 31},
};

static Volume phone_vol[] = {
	{VOLUME_LEVEL0, 0},
	{VOLUME_LEVEL1, 2},
	{VOLUME_LEVEL2, 4},
	{VOLUME_LEVEL3, 6},
	{VOLUME_LEVEL4, 8},
	{VOLUME_LEVEL5, 10},
	{VOLUME_LEVEL6, 12},
	{VOLUME_LEVEL7, 14},
	{VOLUME_LEVEL8, 16},
	{VOLUME_LEVEL9, 18},
	{VOLUME_LEVEL10, 20},
	{VOLUME_LEVEL11, 22},
	{VOLUME_LEVEL12, 24},
	{VOLUME_LEVEL13, 26},
	{VOLUME_LEVEL14, 28},
	{VOLUME_LEVEL15, 30},
	{VOLUME_LEVEL16, 32},
	{VOLUME_LEVEL17, 34},
	{VOLUME_LEVEL18, 36},
	{VOLUME_LEVEL19, 38},
	{VOLUME_LEVEL20, 40},
	{VOLUME_LEVEL21, 42},
	{VOLUME_LEVEL22, 44},
	{VOLUME_LEVEL23, 46},
	{VOLUME_LEVEL24, 48},
	{VOLUME_LEVEL25, 50},
	{VOLUME_LEVEL26, 52},
	{VOLUME_LEVEL27, 54},
	{VOLUME_LEVEL28, 56},
	{VOLUME_LEVEL29, 58},
	{VOLUME_LEVEL30, 60},
	{VOLUME_LEVEL31, 63},
};

static uint8_t speaker_double_used = 0;

static void agc_config()
{
	snd_soc_update_bits(0xb4, (0x3<<6), (0x3<<6));
	snd_soc_update_bits(0x84, (0x3f<<8)|(0xff<<0), (0x31<<8)|(0x28<<0));
	snd_soc_update_bits(0x85, (0x3f<<8)|(0xff<<0), (0x31<<8)|(0x28<<0));
	snd_soc_update_bits(0x8a, (0x7fff<<0), (0x24<<0));
	snd_soc_update_bits(0x8b, (0x7fff<<0), 0x2<<0);
	snd_soc_update_bits(0x8c, (0x7fff<<0), 0x24<<0);
	snd_soc_update_bits(0x8d, (0x7fff<<0),(0x2<<0));
	snd_soc_update_bits(0x8e, (0x1f<<8)|(0x1f<<0),(0xf<<8)|(0xf<<0));
	snd_soc_update_bits(0x93, (0x7ff<<0),(0xfc<<0));
	snd_soc_write(0x94, 0xabb3);
}

static void drc_config()
{
	snd_soc_update_bits(0xa3, (0x7ff<<0),(1<<0));
	snd_soc_write(0xa4, 0x2baf);
	snd_soc_update_bits(0xa5, (0x7ff<<0),(1<<0));
	snd_soc_write(0xa6, 0x2baf);
	snd_soc_update_bits(0xa7, (0x7ff<<0),(0<<0));
	snd_soc_write(0xa8, 0x44a);
	snd_soc_update_bits(0xa9, (0x7ff<<0),(0<<0));
	snd_soc_write(0xaa, 0x1e06);
	snd_soc_update_bits(0xab, (0x7ff<<0),(0x352<<0));
	snd_soc_write(0xac, 0x6910);
	snd_soc_update_bits(0xad, (0x7ff<<0),(0x77a<<0));
	snd_soc_write(0xae, 0xaaaa);
	snd_soc_update_bits(0xaf, (0x7ff<<0),(0x2de<<0));
	snd_soc_write(0xb0, 0xc982);
	snd_soc_write(0x16, 0x9f9f);
}

static void agc_enable(bool on)
{
	if (on) {
		snd_soc_update_bits(MOD_CLK_ENA, (0x1<<7),(0x1<<7));
		snd_soc_update_bits(MOD_RST_CTRL, (0x1<<7),(0x1<<7));

		snd_soc_update_bits(0x82, (0xf<<0)|(0x7<<12),(0x6<<0)|(0x7<<12));
		snd_soc_update_bits(0x83, (0xf<<0)|(0x7<<12),(0x6<<0)|(0x7<<12));
	} else {
		snd_soc_update_bits(MOD_CLK_ENA, (0x1<<7),(0x0<<7));
		snd_soc_update_bits(MOD_RST_CTRL, (0x1<<7),(0x0<<7));

		snd_soc_update_bits(0x82, (0xf<<0)|(0x7<<12),(0x0<<0)|(0x0<<12));
		snd_soc_update_bits(0x83, (0xf<<0)|(0x7<<12),(0x0<<0)|(0x0<<12));
	}
}

static void drc_enable(bool on)
{
	if (on) {
		snd_soc_write(0xb5, 0x80);
		snd_soc_update_bits(MOD_CLK_ENA, (0x1<<6),(0x1<<6));
		snd_soc_update_bits(MOD_RST_CTRL, (0x1<<6),(0x1<<6));
		snd_soc_update_bits(0xa0, (0x7<<0),(0x7<<0));
	} else {
		snd_soc_write(0xb5, 0x0);
		snd_soc_update_bits(MOD_CLK_ENA, (0x1<<6),(0x0<<6));
		snd_soc_update_bits(MOD_RST_CTRL, (0x1<<6),(0x0<<6));
		snd_soc_update_bits(0xa0, (0x7<<0),(0x0<<0));
	}
}

/*
 * Set clock split ratio according to the pcm parameter.
 */
static int32_t AC101_SetClkdiv(DAI_FmtParam *fmtParam,uint32_t sampleRate)
{
	uint32_t i = 0;
	uint32_t aif1_word_size = fmtParam->wordWidth;
	uint32_t aif1_lrlk_div = 2*fmtParam->slotWidth;

	/*set BCLK/LRCK ratio*/
	for (i = 0; i < ARRAY_SIZE(codec_aif1_lrck); i++) {
		if (codec_aif1_lrck[i].aif1_lrlk_div == aif1_lrlk_div) {
			snd_soc_update_bits(AIF1_CLK_CTRL, (0x7<<AIF1_LRCK_DIV),
			                        ((codec_aif1_lrck[i].aif1_lrlk_bit)<<AIF1_LRCK_DIV));
			break;
		}
	}
	for (i = 0; i < ARRAY_SIZE(codec_aif1_fs); i++) {
		if (codec_aif1_fs[i].samplerate ==  sampleRate) {
			snd_soc_update_bits(AIF_SR_CTRL, (0xf<<AIF1_FS),
			                      ((codec_aif1_fs[i].aif1_srbit)<<AIF1_FS));
			snd_soc_update_bits(AIF1_CLK_CTRL, (0xf<<AIF1_BCLK_DIV),
			                      ((codec_aif1_fs[i].aif1_bclk_div)<<AIF1_BCLK_DIV));
			break;
		}
	}

	for (i = 0; i < ARRAY_SIZE(codec_aif1_wsize); i++) {
		if (codec_aif1_wsize[i].aif1_wsize_val == aif1_word_size) {
			snd_soc_update_bits(AIF1_CLK_CTRL, (0x3<<AIF1_WORK_SIZ),
			                      ((codec_aif1_wsize[i].aif1_wsize_bit)<<AIF1_WORK_SIZ));
			break;
		}
	}
	return 0;
}

/*
 * Set the codec FLL.
 */
static int32_t AC101_SetPll(DAI_FmtParam *fmtParam)
{
	uint32_t i = 0;
	uint32_t m = 0;
	uint32_t n_i = 0;
	uint32_t n_f = 0;
	if ((fmtParam->freqIn < 128000) || (fmtParam->freqIn > 24576000)) {
		return HAL_INVALID;
	} else if ((fmtParam->freqIn == 24576000) || (fmtParam->freqIn == 22579200)) {
		switch (fmtParam->pllId) {
			case MCLK1:
				/*select aif1 clk source from mclk1 and enable aif1*/
				snd_soc_update_bits(SYSCLK_CTRL, (0x3<<AIF1CLK_SRC)|(0x1<<AIF1CLK_ENA)|
				                       (0x1<<SYSCLK_SRC)|(0x1<<SYSCLK_ENA),(0x0<<AIF1CLK_SRC)|
				                       (0x1<<AIF1CLK_ENA)|(0x0<<SYSCLK_SRC)|(0x1<<SYSCLK_ENA));
				/*enable moule clk*/
				snd_soc_update_bits(MOD_CLK_ENA, (0x1<<MOD_CLK_AIF1), (0x1<<MOD_CLK_AIF1));

				/*reset module clk*/
				snd_soc_update_bits(MOD_RST_CTRL, (0x1<<MOD_RESET_AIF1), (0x1<<MOD_RESET_AIF1));
				break;
			default:
				return HAL_INVALID;
		}
		return HAL_OK;
	}
	switch (fmtParam->pllId) {
		case MCLK1:
			/*pll source from MCLK1*/
			snd_soc_update_bits(SYSCLK_CTRL, (0x3<<PLLCLK_SRC), (0x0<<PLLCLK_SRC));
			break;
		case BCLK1:
			/*pll source from BCLK1*/
			snd_soc_update_bits(SYSCLK_CTRL, (0x3<<PLLCLK_SRC), (0x2<<PLLCLK_SRC));
			break;
		default:
			return HAL_INVALID;
	}
	/* freq_out = freq_in * n/(m*(2k+1)) , k=1,N=N_i+N_f */
	for (i = 0; i < ARRAY_SIZE(codec_pll_div); i++) {
		if ((codec_pll_div[i].pll_in == fmtParam->freqIn) && (codec_pll_div[i].pll_out == fmtParam->freqOut)) {
			m = codec_pll_div[i].m;
			n_i = codec_pll_div[i].n_i;
			n_f = codec_pll_div[i].n_f;
			break;
		}
	}
	/*config pll m*/
	snd_soc_update_bits(PLL_CTRL1, (0x3f<<PLL_POSTDIV_M), (m<<PLL_POSTDIV_M));
	/*config pll n*/
	snd_soc_update_bits(PLL_CTRL2, (0x3ff<<PLL_PREDIV_NI)|(0x7<<PLL_POSTDIV_NF)|
	                        (0x1<<PLL_EN), (n_i<<PLL_PREDIV_NI)|(n_f<<PLL_POSTDIV_NF)|(1<<PLL_EN));
	/*enable pll,AIF1,SYSCLK*/
	snd_soc_update_bits(SYSCLK_CTRL, (0x1<<PLLCLK_ENA)|(0x3<<AIF1CLK_SRC)|(0x1<<AIF1CLK_ENA)|
	                        (0x1<<SYSCLK_SRC)|(0x1<<SYSCLK_ENA),(1<<PLLCLK_ENA)|
	                        (0x3<<AIF1CLK_SRC)|(0x1<<AIF1CLK_ENA)|(0x0<<SYSCLK_SRC)|(0x1<<SYSCLK_ENA));
	/*enable moule clk*/
	snd_soc_update_bits(MOD_CLK_ENA, (0x1<<MOD_CLK_AIF1), (0x1<<MOD_CLK_AIF1));
	/*reset module clk*/
	snd_soc_update_bits(MOD_RST_CTRL, (0x1<<MOD_RESET_AIF1), (0x1<<MOD_RESET_AIF1));

	return HAL_OK;
}

/*
 * Set codec DAI configuration.
 */
static int32_t AC101_SetFotmat(DAI_FmtParam *fmtParam)
{
	int32_t reg_val;
	/*
         * 	master or slave selection
         *	0 = Master mode
         *	1 = Slave mode
         */
	reg_val = snd_soc_read(AIF1_CLK_CTRL);
	reg_val &=~(0x1<<AIF1_MSTR_MOD);
	switch(fmtParam->clkMode){
		case DAIFMT_CBM_CFM:   /* codec clk & frm master, ap is slave*/
			reg_val |= (0x0<<AIF1_MSTR_MOD);
			break;
		case DAIFMT_CBS_CFS:   /* codec clk & frm slave,ap is master*/
			reg_val |= (0x1<<AIF1_MSTR_MOD);
			break;
		default:
			return HAL_INVALID;
	}
	snd_soc_write(AIF1_CLK_CTRL, reg_val);

	/* i2s mode selection */
	reg_val = snd_soc_read(AIF1_CLK_CTRL);
	reg_val&=~(3<<AIF1_DATA_FMT);
	switch(fmtParam->transferFormat){
		case DAIFMT_I2S:        /* I2S1 mode */
			reg_val |= (0x0<<AIF1_DATA_FMT);
			break;
		case DAIFMT_RIGHT_J:    /* Right Justified mode */
			reg_val |= (0x2<<AIF1_DATA_FMT);
			break;
		case DAIFMT_LEFT_J:     /* Left Justified mode */
			reg_val |= (0x1<<AIF1_DATA_FMT);
			break;
		case DAIFMT_DSP_A:      /* L reg_val msb after FRM LRC */
			reg_val |= (0x3<<AIF1_DATA_FMT);
			break;
		default:
			return HAL_INVALID;
	}
	snd_soc_write(AIF1_CLK_CTRL, reg_val);

	/* DAI signal inversions */
	reg_val = snd_soc_read(AIF1_CLK_CTRL);
	switch(fmtParam->signalInterval){
		case DAIFMT_NB_NF:     /* normal bit clock + nor frame */
			reg_val &= ~(0x1<<AIF1_LRCK_INV);
			reg_val &= ~(0x1<<AIF1_BCLK_INV);
			break;
		case DAIFMT_NB_IF:     /* normal bclk + inv frm */
			reg_val |= (0x1<<AIF1_LRCK_INV);
			reg_val &= ~(0x1<<AIF1_BCLK_INV);
			break;
		case DAIFMT_IB_NF:     /* invert bclk + nor frm */
			reg_val &= ~(0x1<<AIF1_LRCK_INV);
			reg_val |= (0x1<<AIF1_BCLK_INV);
			break;
		case DAIFMT_IB_IF:     /* invert bclk + inv frm */
			reg_val |= (0x1<<AIF1_LRCK_INV);
			reg_val |= (0x1<<AIF1_BCLK_INV);
			break;
	}
	snd_soc_write(AIF1_CLK_CTRL, reg_val);
	return HAL_OK;
}

/*
 * Set headphone as the current output device.
 */
static void AC101_SetHeadphone()
{
	AC101_DEBUG("Route(PLAY): Headphone..\n");
	/*open aif1 DAC channel slot0 switch*/
	snd_soc_update_bits(AIF1_DACDAT_CTRL, (0x1<<AIF1_DA0L_ENA)|(0x1<<AIF1_DA0R_ENA)|
	                        (0x3<<AIF1_DA0L_SRC)|(0x3<<AIF1_DA0R_SRC),(0x1<<AIF1_DA0L_ENA)|
	                        (0x1<<AIF1_DA0R_ENA)|(0x0<<AIF1_DA0L_SRC)|(0x0<<AIF1_DA0R_SRC));
	/*select  DAC mixer source*/
	snd_soc_update_bits(DAC_MXR_SRC, (0x1<<DACL_MXR_AIF1_DA0L)|(0x1<<DACR_MXR_AIF1_DA0R),
	                        (0x1<<DACL_MXR_AIF1_DA0L)|(0x1<<DACR_MXR_AIF1_DA0R));

	snd_soc_update_bits(MOD_CLK_ENA, (0x1<<MOD_CLK_DAC_DIG), (0x1<<MOD_CLK_DAC_DIG));
	snd_soc_update_bits(MOD_RST_CTRL, (0x1<<MOD_RESET_DAC_DIG), (0x1<<MOD_RESET_DAC_DIG));

	/*enable digital DAC*/
	snd_soc_update_bits(DAC_DIG_CTRL, (0x1<<ENDA), (0x1<<ENDA));
	/*enable L/R internal analog channel/output mixer*/
	snd_soc_update_bits(OMIXER_DACA_CTRL, (0x1<<DACAREN)|(0x1<<DACALEN)|(0xf<<HPOUTPUTENABLE)|
	                       (0x1<<LMIXEN)|(0X1<<RMIXEN),(0x1<<DACAREN)|(0x1<<DACALEN)|
	                       (0xf<<HPOUTPUTENABLE)|(0x0<<LMIXEN)|(0X0<<RMIXEN));
	/*disable speaker*/
	snd_soc_update_bits(SPKOUT_CTRL, (0x1<<RSPK_EN)|(0x1<<LSPK_EN), (0x0<<RSPK_EN)|(0x0<<LSPK_EN));

	snd_soc_update_bits(HPOUT_CTRL, (0x1<<HPPA_EN)|(0x1<<RHPPA_MUTE)|(0x1<<LHPPA_MUTE)|(0x1<<RHPS)|(0x1<<LHPS),
	                       (0x1<<HPPA_EN)|(0x1<<RHPPA_MUTE)|(0x1<<LHPPA_MUTE)|(0x0<<RHPS)|(0x0<<LHPS));
}

/*
 * Set speaker as the current output device.
 */
static void AC101_SetSpeaker()
{
	AC101_DEBUG("Route(PLAY): speaker..\n");
	/*open aif1 DAC channel slot0 switch*/
	snd_soc_update_bits(AIF1_DACDAT_CTRL, (0x1<<AIF1_DA0L_ENA)|(0x1<<AIF1_DA0R_ENA)|
	                        (0x3<<AIF1_DA0L_SRC)|(0x3<<AIF1_DA0R_SRC),(0x1<<AIF1_DA0L_ENA)|
	                        (0x1<<AIF1_DA0R_ENA)|(0x0<<AIF1_DA0L_SRC)|(0x0<<AIF1_DA0R_SRC));
	/*select  DAC mixer source*/
	snd_soc_update_bits(DAC_MXR_SRC, (0x1<<DACL_MXR_AIF1_DA0L)|(0x1<<DACR_MXR_AIF1_DA0R),
	                        (0x1<<DACL_MXR_AIF1_DA0L)|(0x1<<DACR_MXR_AIF1_DA0R));
	/*enable digital DAC*/
	snd_soc_update_bits(DAC_DIG_CTRL, (0x1<<ENDA), (0x1<<ENDA));
	/*enable analog DAC/enable output mixer*/
	snd_soc_update_bits(OMIXER_DACA_CTRL, (0x1<<DACALEN)|(0X1<<DACAREN)|(0x1<<LMIXEN)|
	                         (0X1<<RMIXEN) |(0xf<<HPOUTPUTENABLE),(0x1<<DACALEN)|
	                         (0X1<<DACAREN)|(0x1<<LMIXEN)|(0X1<<RMIXEN)|(0xf<<HPOUTPUTENABLE));

	snd_soc_update_bits(MOD_CLK_ENA, (0x1<<MOD_CLK_DAC_DIG), (0x1<<MOD_CLK_DAC_DIG));
	snd_soc_update_bits(MOD_RST_CTRL, (0x1<<MOD_RESET_DAC_DIG), (0x1<<MOD_RESET_DAC_DIG));

	/*select output mixer source select*/
	snd_soc_update_bits(OMIXER_SR, (0x1<<RMIXMUTEDACR)|(0x1<<LMIXMUTEDACL),
	                          (0x1<<RMIXMUTEDACR)|(0x1<<LMIXMUTEDACL));
	/*disable headphone power amplifier/mute l/r headphone pa*/
	snd_soc_update_bits(HPOUT_CTRL, (0x1<<RHPPA_MUTE)|(0x1<<LHPPA_MUTE)|0x1<<HPPA_EN,
	                          (0x0<<RHPPA_MUTE)|(0x0<<LHPPA_MUTE)|(0x0<<HPPA_EN));

	if (speaker_double_used) {
		snd_soc_update_bits(SPKOUT_CTRL, (0x1<<RSPKS)|(0x1<<LSPKS)|(0x1<<RSPK_EN)|(0x1<<LSPK_EN),
		                       (0x0<<RSPKS)|(0x0<<LSPKS)|(0x1<<RSPK_EN)|(0x1<<LSPK_EN));
	} else {
		snd_soc_update_bits(SPKOUT_CTRL, (0x1<<LSPKS)|(0x1<<RSPK_EN)|(0x1<<LSPK_EN),
		                       (0x1<<LSPKS)|(0x1<<LSPK_EN));
	}
}

/*
 * Set main mic as the current input device.
 */
static void AC101_SetMainMic()
{
	AC101_DEBUG("Route(cap): main mic..\n");
	snd_soc_update_bits(MOD_CLK_ENA, (0x1<<MOD_CLK_ADC_DIG), (0x1<<MOD_CLK_ADC_DIG));
	snd_soc_update_bits(MOD_RST_CTRL, (0x1<<MOD_RESET_ADC_DIG), (0x1<<MOD_RESET_ADC_DIG));

	snd_soc_update_bits(ADC_SRCBST_CTRL, (0x1<<MIC1AMPEN)|(0x1<<MIC2AMPEN),
	                       (0x1<<MIC1AMPEN)|(0x0<<MIC2AMPEN));
	/*select  adc mixer source*/
	snd_soc_update_bits(ADC_SRC, (0x1<<LADCMIXMUTEMIC2BOOST)|(0x1<<RADCMIXMUTEMIC2BOOST)
	                       |(0x1<<LADCMIXMUTEMIC1BOOST)|(0x1<<RADCMIXMUTEMIC1BOOST),
	                      (0x1<<LADCMIXMUTEMIC1BOOST)|(0x1<<RADCMIXMUTEMIC1BOOST));
	/*ADC L/R channel Enable*/
	snd_soc_update_bits(ADC_APC_CTRL,  (0x1<<ADCREN)|(0x1<<ADCLEN)|(0x1<<MBIASEN),
	                      (0x1<<ADCREN)|(0x1<<ADCLEN)|(0x1<<MBIASEN));
	snd_soc_update_bits(ADC_DIG_CTRL, (0x1<<ENAD)|(0x1<<ENDM), (0x1<<ENAD)|(0x0<<ENDM));
	/* select AIF1 output  mixer source*/
	snd_soc_update_bits(AIF1_MXR_SRC, (0x1<<AIF1_AD0R_ADCR_MXR)|(0x1<<AIF1_AD0L_ADCL_MXR),
	                      (0x1<<AIF1_AD0R_ADCR_MXR)|(0x1<<AIF1_AD0L_ADCL_MXR));
	/*open ADC channel slot0 switch*/
	snd_soc_update_bits(AIF1_ADCDAT_CTRL, (0x1<<AIF1_AD0L_ENA)|(0x1<<AIF1_AD0R_ENA),
	                      (0x1<<AIF1_AD0L_ENA)|(0x1<<AIF1_AD0R_ENA));
#if 0
        /*open aif1 DAC channel slot0 switch*/
      //  snd_soc_update_bits(AIF1_DACDAT_CTRL, (0x1<<AIF1_DA0L_ENA)|(0x1<<AIF1_DA0R_ENA)|
        //			(0x3<<AIF1_DA0L_SRC)|(0x3<<AIF1_DA0R_SRC),(0x1<<AIF1_DA0L_ENA)|
        //			(0x1<<AIF1_DA0R_ENA)|(0x0<<AIF1_DA0L_SRC)|(0x0<<AIF1_DA0R_SRC));
        /*select  DAC mixer source*/
        snd_soc_update_bits(DAC_MXR_SRC, (0x1<<DACL_MXR_ADCL)|(0x1<<DACR_MXR_SR),
                        	(0x1<<DACL_MXR_ADCL)|(0x1<<DACR_MXR_SR));

	snd_soc_update_bits(MOD_CLK_ENA, (0x1<<MOD_CLK_DAC_DIG), (0x1<<MOD_CLK_DAC_DIG));
	snd_soc_update_bits(MOD_RST_CTRL, (0x1<<MOD_RESET_DAC_DIG), (0x1<<MOD_RESET_DAC_DIG));

        /*enable digital DAC*/
        snd_soc_update_bits(DAC_DIG_CTRL, (0x1<<ENDA), (0x1<<ENDA));

        /*enable L/R internal analog channel/output mixer*/
        snd_soc_update_bits(OMIXER_DACA_CTRL, (0x1<<DACAREN)|(0x1<<DACALEN)|(0xf<<HPOUTPUTENABLE)|
        			(0x1<<LMIXEN)|(0X1<<RMIXEN),(0x1<<DACAREN)|(0x1<<DACALEN)|
        			(0xf<<HPOUTPUTENABLE)|(0x1<<LMIXEN)|(0X1<<RMIXEN));

        /*select output mixer source select*/
        snd_soc_update_bits(OMIXER_SR, (0x1<<RMIXMUTEDACR)|(0x1<<LMIXMUTEDACL),
                        	(0x1<<RMIXMUTEDACR)|(0x1<<LMIXMUTEDACL));


	/*disable speaker*/
        snd_soc_update_bits(SPKOUT_CTRL, (0x1<<RSPK_EN)|(0x1<<LSPK_EN), (0x0<<RSPK_EN)|(0x0<<LSPK_EN));

        snd_soc_update_bits(HPOUT_CTRL, (0x1<<HPPA_EN)|(0x1<<RHPPA_MUTE)|(0x1<<LHPPA_MUTE)|(0x1<<RHPS)|(0x1<<LHPS),
                        	(0x1<<HPPA_EN)|(0x1<<RHPPA_MUTE)|(0x1<<LHPPA_MUTE)|(0x1<<RHPS)|(0x1<<LHPS));

#endif
}

/*
 * Set headset mic as the current input device.
 */
static void AC101_SetHeadphoneMic()
{
	AC101_DEBUG("Route(cap): Headset mic..\n");
	snd_soc_update_bits(MOD_CLK_ENA, (0x1<<MOD_CLK_ADC_DIG), (0x1<<MOD_CLK_ADC_DIG));
	snd_soc_update_bits(MOD_RST_CTRL, (0x1<<MOD_RESET_ADC_DIG), (0x1<<MOD_RESET_ADC_DIG));
	/*select  mic1/2 boost control*/
	snd_soc_update_bits(ADC_SRCBST_CTRL, (0x1<<MIC1AMPEN)|(0x1<<MIC2AMPEN),
	                        (0x0<<MIC1AMPEN)|(0x1<<MIC2AMPEN));
	snd_soc_update_bits(ADC_SRC, (0x1<<LADCMIXMUTEMIC2BOOST)|(0x1<<RADCMIXMUTEMIC2BOOST)|
	                         (0x1<<LADCMIXMUTEMIC1BOOST)|(0x1<<RADCMIXMUTEMIC1BOOST),
	                         (0x1<<LADCMIXMUTEMIC2BOOST)|(0x1<<RADCMIXMUTEMIC2BOOST)|
	                         (0x0<<LADCMIXMUTEMIC1BOOST)|(0x0<<RADCMIXMUTEMIC1BOOST));
	/*ADC L/R channel Enable*/
	snd_soc_update_bits(ADC_APC_CTRL, (0x1<<ADCREN)|(0x1<<ADCLEN)|(0x1<<HBIASEN),
	                         (0x1<<ADCREN)|(0x1<<ADCLEN)|(0x1<<HBIASEN));

	snd_soc_update_bits(ADC_DIG_CTRL, (0x1<<ENAD)|(0x1<<ENDM), (0x1<<ENAD)|(0x0<<ENDM));

	/* select AIF1 output  mixer source*/
	snd_soc_update_bits(AIF1_MXR_SRC, (0x1<<AIF1_AD0R_ADCR_MXR)|(0x1<<AIF1_AD0L_ADCL_MXR),
	                        (0x1<<AIF1_AD0R_ADCR_MXR)|(0x1<<AIF1_AD0L_ADCL_MXR));
	/*open ADC channel slot0 switch*/
	snd_soc_update_bits(AIF1_ADCDAT_CTRL, (0x1<<AIF1_AD0L_ENA)|(0x1<<AIF1_AD0R_ENA),
	                        (0x1<<AIF1_AD0L_ENA)|(0x1<<AIF1_AD0R_ENA));

	snd_soc_update_bits(ADC_APC_CTRL, (0x7<<0), (0x7<<0));
#if 0
        /*select  DAC mixer source*/
        snd_soc_update_bits(DAC_MXR_SRC, (0x1<<DACL_MXR_ADCL)|(0x1<<DACR_MXR_SR),
                        	(0x1<<DACL_MXR_ADCL)|(0x1<<DACR_MXR_SR));

	snd_soc_update_bits(MOD_CLK_ENA, (0x1<<MOD_CLK_DAC_DIG), (0x1<<MOD_CLK_DAC_DIG));
	snd_soc_update_bits(MOD_RST_CTRL, (0x1<<MOD_RESET_DAC_DIG), (0x1<<MOD_RESET_DAC_DIG));

        /*enable digital DAC*/
        snd_soc_update_bits(DAC_DIG_CTRL, (0x1<<ENDA), (0x1<<ENDA));

        /*enable L/R internal analog channel/output mixer*/
        snd_soc_update_bits(OMIXER_DACA_CTRL, (0x1<<DACAREN)|(0x1<<DACALEN)|(0xf<<HPOUTPUTENABLE)|
        			(0x1<<LMIXEN)|(0X1<<RMIXEN),(0x1<<DACAREN)|(0x1<<DACALEN)|
        			(0xf<<HPOUTPUTENABLE)|(0x1<<LMIXEN)|(0X1<<RMIXEN));

        /*select output mixer source select*/
        snd_soc_update_bits(OMIXER_SR, (0x1<<RMIXMUTEDACR)|(0x1<<LMIXMUTEDACL),
                        	(0x1<<RMIXMUTEDACR)|(0x1<<LMIXMUTEDACL));

        snd_soc_update_bits(HPOUT_CTRL, (0x1<<HPPA_EN)|(0x1<<RHPPA_MUTE)|(0x1<<LHPPA_MUTE)|(0x1<<RHPS)|(0x1<<LHPS),
                        	(0x1<<HPPA_EN)|(0x1<<RHPPA_MUTE)|(0x1<<LHPPA_MUTE)|(0x0<<RHPS)|(0x0<<LHPS));

        snd_soc_update_bits(OMIXER_SR, (0x1<<RMIXMUTEMIC2BOOST)|(0x1<<LMIXMUTEMIC2BOOST),
                        	(0x1<<RMIXMUTEMIC2BOOST)|(0x1<<LMIXMUTEMIC2BOOST));

	//snd_soc_write(0x4a, 0x40);
#endif
}

/*
 * Set codec initialization parameter.
 */
HAL_Status AC101_Setcfg(CODEC_InitParam *param)
{
	if (!param)
		return HAL_INVALID;

	if (param->speaker_double_used) {
		snd_soc_update_bits(SPKOUT_CTRL, (0x1f<<SPK_VOL), (param->double_speaker_val<<SPK_VOL));
	} else {
		snd_soc_update_bits(SPKOUT_CTRL, (0x1f<<SPK_VOL), (param->single_speaker_val<<SPK_VOL));
	}
	speaker_double_used = param->speaker_double_used;

	snd_soc_update_bits(HPOUT_CTRL, (0x3f<<HP_VOL), (param->headset_val<<HP_VOL));
	snd_soc_update_bits(ADC_SRCBST_CTRL, (0x7<<ADC_MIC1G), (param->mainmic_val<<ADC_MIC1G));
	snd_soc_update_bits(ADC_SRCBST_CTRL, (0x7<<ADC_MIC2G), (param->headsetmic_val<<ADC_MIC2G));

	if (AGC_ENABLE) {
		AC101_DEBUG("use agc..\n");
		agc_config();
		agc_enable(1);
	}
	if (DRC_ENABLE) {
		AC101_DEBUG("use drc..\n");
		drc_config();
		drc_enable(1);
	}

	/*headphone calibration clock frequency select*/
	snd_soc_update_bits(SPKOUT_CTRL, (0x7<<HPCALICKS), (0x7<<HPCALICKS));
	/*enable this bit to prevent leakage from ldoin*/
	snd_soc_update_bits(ADDA_TUNE3, (0x1<<OSCEN), (0x1<<OSCEN));
	snd_soc_update_bits(ADDA_TUNE1, (0x1<<8), (0x0<<8));
	snd_soc_write(DAC_VOL_CTRL, 0xA0A0);
	return HAL_OK;
}

/*
 * Set audio output/input device.
 */
static int32_t AC101_SetRoute(AUDIO_Device device)
{
	switch(device) {
		case AUDIO_DEVICE_HEADPHONE:
			AC101_SetHeadphone();
			break;
		case AUDIO_DEVICE_SPEAKER:
			AC101_SetSpeaker();
			break;
		case AUDIO_DEVICE_MAINMIC:
			AC101_SetMainMic();
			break;
		case AUDIO_DEVICE_HEADPHONEMIC:
			AC101_SetHeadphoneMic();
			break;
		default:
			break;
	}
	return HAL_OK;
}

/*
 * Set audio output device volume gain.
 */
static int32_t AC101_SetVolume( AUDIO_Device dev,uint32_t volume)
{
	AC101_DEBUG("[set volume] dev(%d) volume(%d)..\n", (int)dev, (int)volume);
	if (volume > VOLUME_MAX_LEVEL)
		return -1;
	uint32_t volume_val = 0;
	switch (dev) {
		case AUDIO_DEVICE_HEADPHONE:
			volume_val = phone_vol[volume].reg_val;
			snd_soc_update_bits(HPOUT_CTRL, (0x3f<<HP_VOL), (volume_val<<HP_VOL));
			break;
		case AUDIO_DEVICE_SPEAKER:
			volume_val = spk_vol[volume].reg_val;
			snd_soc_update_bits(SPKOUT_CTRL, (0x1f<<SPK_VOL), (volume_val<<SPK_VOL));
			break;
		default:
			AC101_DEBUG("[set volume] Wrong audio out device..\n");
			return -1;
	}
	return 0;
}

/*
 * Trigger output device avoid pops.
 */
static int32_t AC101_SetTrigger( AUDIO_Device dev,uint32_t on)
{
	if (AUDIO_DEVICE_HEADPHONE != dev)
		return -1;

	if (on) {

	} else {

	}
	return 0;
}

/*
 * Deinit codec hardware when audio stream stop.
 */
static int32_t AC101_ShutDown(bool playOn, bool recordOn)
{
	if (playOn == 0 && recordOn == 0) {
		snd_soc_update_bits(AIF1_DACDAT_CTRL, (0x1<<AIF1_DA0L_ENA)|(0x1<<AIF1_DA0R_ENA),
		                       (0x0<<AIF1_DA0L_ENA)|(0x0<<AIF1_DA0R_ENA));
		/*enable digital DAC*/
		snd_soc_update_bits(DAC_DIG_CTRL, (0x1<<ENDA), (0x0<<ENDA));
		snd_soc_update_bits(DAC_MXR_SRC, (0x1<<DACL_MXR_AIF1_DA0L)|(0x1<<DACR_MXR_AIF1_DA0R),
		                        (0x0<<DACL_MXR_AIF1_DA0L)|(0x0<<DACR_MXR_AIF1_DA0R));
		snd_soc_update_bits(OMIXER_DACA_CTRL, (0x1<<DACAREN)|(0x1<<DACALEN)|(0xf<<HPOUTPUTENABLE)|
		                        (0x1<<LMIXEN)|(0X1<<RMIXEN),(0x0<<DACAREN)|(0x0<<DACALEN)|
		                        (0x0<<HPOUTPUTENABLE)|(0x0<<LMIXEN)|(0X0<<RMIXEN));
		snd_soc_update_bits(SPKOUT_CTRL, (0x1<<RSPK_EN)|(0x1<<LSPK_EN), (0x0<<RSPK_EN)|(0x0<<LSPK_EN));
		snd_soc_update_bits(OMIXER_SR, (0x1<<RMIXMUTEDACR)|(0x1<<LMIXMUTEDACL),
		                        (0x0<<RMIXMUTEDACR)|(0x0<<LMIXMUTEDACL));
		snd_soc_update_bits(HPOUT_CTRL, (0x1<<HPPA_EN)|(0x1<<RHPPA_MUTE)|(0x1<<LHPPA_MUTE),
		                        (0x0<<HPPA_EN)|(0x0<<RHPPA_MUTE)|(0x0<<LHPPA_MUTE));

		snd_soc_update_bits(ADC_SRCBST_CTRL, (0x1<<MIC1AMPEN)|(0x1<<MIC2AMPEN),
		                        (0x0<<MIC1AMPEN)|(0x0<<MIC2AMPEN));
		snd_soc_update_bits(ADC_SRC, 0xFFFF,0);

		snd_soc_update_bits(ADC_APC_CTRL,  (0x1<<ADCREN)|(0x1<<ADCLEN)|(0x1<<MBIASEN),
		                        (0x0<<ADCREN)|(0x0<<ADCLEN)|(0x0<<MBIASEN));
		snd_soc_update_bits(ADC_DIG_CTRL, (0x1<<ENAD)|(0x1<<ENDM), (0x0<<ENAD)|(0x0<<ENDM));

		snd_soc_update_bits(AIF1_ADCDAT_CTRL, (0x1<<AIF1_AD0L_ENA)|(0x1<<AIF1_AD0R_ENA),
		                        (0x0<<AIF1_AD0L_ENA)|(0x0<<AIF1_AD0R_ENA));
		snd_soc_update_bits(PLL_CTRL1, (0x1<<PLL_EN), (0x0<<PLL_EN));

		snd_soc_update_bits(SYSCLK_CTRL, (0x1<<PLLCLK_ENA)|(0x1<<AIF1CLK_ENA)|(0x1<<SYSCLK_ENA),
		                        (0x0<<PLLCLK_ENA)|(0x0<<AIF1CLK_ENA)|(0x0<<SYSCLK_ENA));

		snd_soc_update_bits(MOD_CLK_ENA, (0x1<<MOD_CLK_AIF1)|(0x1<<MOD_CLK_ADC_DIG)|(0x1<<MOD_CLK_DAC_DIG),
		                        (0x0<<MOD_CLK_AIF1)|(0x0<<MOD_CLK_ADC_DIG)|(0x0<<MOD_CLK_DAC_DIG));

		snd_soc_update_bits(MOD_RST_CTRL, (0x1<<MOD_RESET_AIF1)|(0x1<<MOD_RESET_ADC_DIG)|(0x1<<MOD_RESET_DAC_DIG),
		                        (0x0<<MOD_RESET_AIF1)|(0x0<<MOD_RESET_ADC_DIG)|(0x0<<MOD_RESET_DAC_DIG));

	} else if (playOn == 0) {

		snd_soc_update_bits(AIF1_DACDAT_CTRL, (0x1<<AIF1_DA0L_ENA)|(0x1<<AIF1_DA0R_ENA),
		                         (0x0<<AIF1_DA0L_ENA)|(0x0<<AIF1_DA0R_ENA));
		/*enable digital DAC*/
		snd_soc_update_bits(DAC_DIG_CTRL, (0x1<<ENDA), (0x0<<ENDA));
		snd_soc_update_bits(DAC_MXR_SRC, (0x1<<DACL_MXR_AIF1_DA0L)|(0x1<<DACR_MXR_AIF1_DA0R),
		                         (0x0<<DACL_MXR_AIF1_DA0L)|(0x0<<DACR_MXR_AIF1_DA0R));
		snd_soc_update_bits(OMIXER_DACA_CTRL, (0x1<<DACAREN)|(0x1<<DACALEN)|(0xf<<HPOUTPUTENABLE)|
		                         (0x1<<LMIXEN)|(0X1<<RMIXEN),(0x0<<DACAREN)|(0x0<<DACALEN)|
		                         (0x0<<HPOUTPUTENABLE)|(0x0<<LMIXEN)|(0X0<<RMIXEN));
		snd_soc_update_bits(OMIXER_SR, (0x1<<RMIXMUTEDACR)|(0x1<<LMIXMUTEDACL),
		                         (0x0<<RMIXMUTEDACR)|(0x0<<LMIXMUTEDACL));
		/*disable speaker*/
		snd_soc_update_bits(SPKOUT_CTRL, (0x1<<RSPK_EN)|(0x1<<LSPK_EN), (0x0<<RSPK_EN)|(0x0<<LSPK_EN));

		snd_soc_update_bits(HPOUT_CTRL, (0x1<<HPPA_EN)|(0x1<<RHPPA_MUTE)|(0x1<<LHPPA_MUTE),
		                          (0x0<<HPPA_EN)|(0x0<<RHPPA_MUTE)|(0x0<<LHPPA_MUTE));

		snd_soc_update_bits(MOD_CLK_ENA, (0x1<<MOD_CLK_DAC_DIG), (0x0<<MOD_CLK_DAC_DIG));
		snd_soc_update_bits(MOD_RST_CTRL, (0x1<<MOD_RESET_DAC_DIG), (0x0<<MOD_RESET_DAC_DIG));

	} else if (recordOn == 0) {
		snd_soc_update_bits(SPKOUT_CTRL, (0x1<<RSPK_EN)|(0x1<<LSPK_EN), (0x0<<RSPK_EN)|(0x0<<LSPK_EN));

		snd_soc_update_bits(HPOUT_CTRL, (0x1<<HPPA_EN)|(0x1<<RHPPA_MUTE)|(0x1<<LHPPA_MUTE),
		                         (0x0<<HPPA_EN)|(0x0<<RHPPA_MUTE)|(0x0<<LHPPA_MUTE));

		snd_soc_update_bits(ADC_SRCBST_CTRL, (0x1<<MIC1AMPEN)|(0x1<<MIC2AMPEN),
		                         (0x0<<MIC1AMPEN)|(0x0<<MIC2AMPEN));

		snd_soc_update_bits(ADC_APC_CTRL,  (0x1<<ADCREN)|(0x1<<ADCLEN)|(0x1<<MBIASEN),
		                         (0x0<<ADCREN)|(0x0<<ADCLEN)|(0x0<<MBIASEN));
		snd_soc_update_bits(ADC_DIG_CTRL, (0x1<<ENAD)|(0x1<<ENDM), (0x0<<ENAD)|(0x0<<ENDM));

		snd_soc_update_bits(AIF1_ADCDAT_CTRL, (0x1<<AIF1_AD0L_ENA)|(0x1<<AIF1_AD0R_ENA),
		                         (0x0<<AIF1_AD0L_ENA)|(0x0<<AIF1_AD0R_ENA));
		snd_soc_update_bits(MOD_CLK_ENA, (0x1<<MOD_CLK_ADC_DIG), (0x0<<MOD_CLK_ADC_DIG));
		snd_soc_update_bits(MOD_RST_CTRL, (0x1<<MOD_RESET_ADC_DIG), (0x0<<MOD_RESET_ADC_DIG));
	} else {
		return HAL_INVALID;
	}

	if (AGC_ENABLE && recordOn == 0) {
		agc_enable(0);
	}
	if (DRC_ENABLE && playOn == 0) {
		drc_enable(0);
	}
	return HAL_OK;
}

/*
 * Set/reset power for the codec.
 */
static int32_t AC101_SetPower(CODEC_Req req, void *arg)
{
	if (req == HAL_CODEC_INIT) {

	} else {

	}
	return HAL_OK;
}

/*
 * Set/reset sysclk for the codec.
 */
static int32_t AC101_SetSysClk(CODEC_Req req, void *arg)
{
	if (req == HAL_CODEC_INIT) {

	} else {

	}
	return HAL_OK;
}

/*
 * Set/reset the necessary initialization value for the codec.
 */
static int32_t AC101_SetInitParam(CODEC_Req req, void *arg)
{
	if (req == HAL_CODEC_INIT) {
		CODEC_InitParam *param = (CODEC_InitParam *)arg;
		AC101_Setcfg(param);
	} else {
		if (AGC_ENABLE)
			agc_enable(0);

		if (DRC_ENABLE)
			drc_enable(0);
		#if 0
		int i = 0;
		for (i = 0; i < 100; i++) {
			if (i % 4 == 3)
				printf("  reg:0x%x val: 0x%x\n",i,(int)snd_soc_read(i));
			else
				printf("  reg:0x%x val: 0x%x",i,(int)snd_soc_read(i));
		}
		#endif
	}
	return HAL_OK;
}

/*
 * Init/deinit jack detection.
 */
static int32_t AC101_JackDetect(CODEC_Req req, void *arg)
{
	if (req == HAL_CODEC_INIT) {

	} else {

	}
	return HAL_OK;
}

const struct codec_ctl_ops ac101_ctl_ops =  {
	.setRoute   = AC101_SetRoute,
	.setVolume  = AC101_SetVolume,
	.setTrigger = AC101_SetTrigger,
};

const struct codec_dai_ops ac101_dai_ops =  {
	.setPll     = AC101_SetPll,
	.setClkdiv  = AC101_SetClkdiv,
	.setFormat  = AC101_SetFotmat,
	.shutDown   = AC101_ShutDown,
};

const struct codec_ops ac101_ops =  {
	.setPower      = AC101_SetPower,
	.setSysClk     = AC101_SetSysClk,
	.setInitParam  = AC101_SetInitParam,
	.jackDetect    = AC101_JackDetect,
};

CODEC AC101 = {"ac101",AC101_I2C_ADDR,CODEC_I2C_REG_LENGTH8,CODEC_I2C_REGVAL_LENGTH16, \
                    (struct codec_ops *)&ac101_ops,(struct codec_dai_ops *)&ac101_dai_ops, (struct codec_ctl_ops*)&ac101_ctl_ops};
