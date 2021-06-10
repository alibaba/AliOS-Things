/**
  ******************************************************************************
  * @file    rl6548.c
  * @author
  * @version V1.0.0
  * @date    2018-12-12
  * @brief   This file provides firmware functions to manage the following
  *          functionalities of the Audio codec peripheral:
  *           - Codec Initialization
  *           - Codec parameters configuration
  *           - SI read/write configuration
  *           - SI Clock configuration
  *
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2018, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */
#include "ameba_soc.h"
#include "rl6548.h"
#include "rl6548_eq_table.h"
/**
  * @brief  Enables or disables the specified AUDIO SI peripheral.
  * @param  new_state: new state of the SIx peripheral.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void AUDIO_SI_Cmd(u8  new_state)
{
	AUDIO_SI_TypeDef* SIx = AUDIO_SI_DEV;
	
	if (new_state == ENABLE) {
		SIx->SI_CTRLR &= ~ BIT_CTRLR_SI_DISABLE;
	} else {
		SIx->SI_CTRLR |= BIT_CTRLR_SI_DISABLE;
	}
}

/**
  * @brief	SI write codec register.
  * @param  address: codec register address.
  * @param  data: data which write to register.
  * @retval None
  */
void AUDIO_SI_WriteReg(u32 address, u32 data)
{
	u32 cnt = 0;
	AUDIO_SI_TypeDef* SIx = AUDIO_SI_DEV;
	
	SIx->SI_CTRLR = (address << 8) | (data << 16) | BIT_CTRLR_SI_WR_START;
	
	while((SIx->SI_CTRLR & BIT_CTRLR_SI_WR_START) && (++cnt) < 100000);
	
	if (cnt == 100000){
		DBG_8195A("write codec reg fail!!\n");
	}
}

/**
  * @brief	SI read codec register.
  * @param  address: codec register address.
  * @retval value of register.
  */
u16 AUDIO_SI_ReadReg(u32 address)
{
	u32 reg_value = 0;
	u32 cnt = 0;
	AUDIO_SI_TypeDef* SIx = AUDIO_SI_DEV;

	/* Configure Read command */
	SIx->SI_CTRLR = (address << 8) | BIT_CTRLR_SI_RD_START;

	do {
		reg_value = SIx->SI_CTRLR;
	/* waiting for read done */
	} while((reg_value & BIT_CTRLR_SI_RD_START) && (++cnt) < 100000);

	if (cnt == 100000){
		DBG_8195A("read codec reg fail!!\n");
		return 0;
	}

	return ((reg_value & BIT_CTRLR_SI_DATA) >> 16);
}

/**
  * @brief  Turn on or turn off the clock of register bank of audio codec.
  * @param  new_state: new state of the clock of register bank of audio codec.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void AUDIO_SI_ClkCmd(u8  new_state)
{
	AUDIO_SI_TypeDef* SIx = AUDIO_SI_DEV;
	
	if (new_state == ENABLE) {
		SIx->SI_CLK_EN |= BIT_SI_CLK_EN;
	} else {
		SIx->SI_CLK_EN &= ~ BIT_SI_CLK_EN;
	}
}

/**
  * @brief  Initialize codec peripheral.
  * @param  sample_rate: codec ADC and DAC sample rate.
  *          This parameter can be one of the following values:
  *            @arg SR_8K: sample rate is 8kHz
  *            @arg SR_16K: sample rate is 16kHz
  *            @arg SR_32K: sample rate is 32kHz
  *            @arg SR_48K: sample rate is 48kHz
  *            @arg SR_96K: sample rate is 96kHz
  *            @arg SR_44P1K: sample rate is 44.1kHz
  *            @arg SR_88P2K: sample rate is 88.2kHz
  * @param  word_len: data sample bit
  *          This parameter can be one of the following values:
  *            @arg WL_16: sample bit is 16 bit
  *            @arg WL_24: sample bit is 24 bit
  *            @arg WL_8: sample bit is 8 bit
  * @param  mono_stereo: mono channel or stereo channel
  *          This parameter can be one of the following values:
  *            @arg CH_STEREO: stereo channel, channel number is 2
  *            @arg CH_MONO: mono channel, channel number is 1
  * @param  application: application mode
  *          This parameter can be one of the following values:
  *            @arg APP_AMIC_IN: analog microphone in
  *            @arg APP_DMIC_IN: digital microphone in
  *            @arg APP_LINE_IN: line in
  *            @arg APP_LINE_OUT: line out
  *            @arg APP_DAAD_LPBK: loopback
  * @return  None
  */
void CODEC_Init(u32 sample_rate, u32 word_len, u32 mono_stereo, u32 application)
{
    uint32_t reg_value = 0, reg_value2 = 0, reg_value3 = 0;

	reg_value = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AUDIO_SHARE_PAD_CTRL);
	reg_value &= ~(BIT_LSYS_MASK_AC_LDO_REG << BIT_LSYS_SHIFT_AC_LDO_REG);
	//restore the pad power
	if (is_power_supply18()){
		reg_value |= ((u32)0x68) << BIT_LSYS_SHIFT_AC_LDO_REG;
	}
	else{
		reg_value |= ((u32)0x2A) << BIT_LSYS_SHIFT_AC_LDO_REG;
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AUDIO_SHARE_PAD_CTRL, reg_value);
	
    /* ================= CODEC initialize ======================== */

	if ((application&APP_DAAD_LPBK) == APP_DAAD_LPBK){
		reg_value3 = (1<<BIT_ADC_L_AD_ZDET_TOUT | 1<<BIT_ADC_L_AD_ZDET_FUNC | 2<<BIT_ADC_L_ADJ_HPF_COEF_SEL);
		AUDIO_SI_WriteReg(ADC_L_CTRL, reg_value3);
		reg_value3 = 0;
		
		reg_value3 = (1<<BIT_ADC_R_AD_ZDET_TOUT | 1<<BIT_ADC_R_AD_ZDET_FUNC | 2<<BIT_ADC_R_ADJ_HPF_COEF_SEL);
		AUDIO_SI_WriteReg(ADC_R_ADJ_D, reg_value3);
		reg_value3 = 0;
		
		reg_value = AUDIO_SI_ReadReg(ASRC_CTRL);
		reg_value |= (1 | 1<<BIT_ASRC_FTK_LOOP_EN | 1<<BIT_ASRC_256FS_SYS_SEL);
		AUDIO_SI_WriteReg(ASRC_CTRL, reg_value);	

		reg_value = AUDIO_SI_ReadReg(ASRC_CTRL);
		reg_value |= (1 << BIT_ASRC_EN);
		AUDIO_SI_WriteReg(ASRC_CTRL, reg_value);			
		
		reg_value3 = (1<<BIT_DA_L_EN | 1<<BIT_DA_R_EN | 1<<BIT_MOD_L_EN | 1<<BIT_MOD_R_EN | 1<<BIT_DA_ANA_CLK_EN | 1<<BIT_DA_FIFO_EN\
					| 1<<BIT_AD_L_EN | 1<<BIT_AD_R_EN | 1<<BIT_AD_FIFO_EN | 1<<BIT_AD_ANA_CLK_EN | 1<<BIT_AD_ANA_L_EN | 1<<BIT_AD_ANA_R_EN);		
		AUDIO_SI_WriteReg(DAC_ADC_MIC_CLK_CTRL,reg_value3);
		reg_value3 = 0;
		
		reg_value = AUDIO_SI_ReadReg(DAC_L_MUTE_CTRL);
		reg_value |= (1<<BIT_DAAD_LPBK_EN);
		AUDIO_SI_WriteReg(DAC_L_MUTE_CTRL, reg_value);
		
		reg_value = AUDIO_SI_ReadReg(ADC_DMIC_L_FILTER_CTRL);
		reg_value &= (0xffff&(~(1<<BIT_ADC_L_DMIC_MIX_MUTE)));
		reg_value |= (1<<BIT_ADC_L_AD_MIX_MUTE);	
		AUDIO_SI_WriteReg(ADC_DMIC_L_FILTER_CTRL, reg_value);
		
		reg_value = AUDIO_SI_ReadReg(ADC_DMIC_R_FILTER_CTRL);
		reg_value &= (0xffff&(~(1<<BIT_ADC_R_DMIC_MIX_MUTE)));
		reg_value |= (1<<BIT_ADC_R_AD_MIX_MUTE);		
		AUDIO_SI_WriteReg(ADC_DMIC_R_FILTER_CTRL, reg_value);	
		
		
		reg_value = AUDIO_SI_ReadReg(DAC_ADC_MIC_CLK_CTRL);
		reg_value |= (1<<BIT_DMIC_L_EN | 1<<BIT_DMIC_R_EN | 1<<BIT_DMIC_CLK_EN);
		AUDIO_SI_WriteReg(DAC_ADC_MIC_CLK_CTRL, reg_value);
		
		reg_value3 = (1 | 1<<BIT_ASRC_FTK_LOOP_EN | 1<<BIT_ASRC_256FS_SYS_SEL | 1<<BIT_ASRC_EN);
		AUDIO_SI_WriteReg(ASRC_CTRL, reg_value3);
		reg_value3 = 0;
		
		AUDIO_SI_WriteReg(DAC_ADC_SR_CTRL, (sample_rate << 4) | sample_rate);
		
		reg_value3 = (1<<BIT_I2S_RST_N_REG);
		AUDIO_SI_WriteReg(I2S_CTRL, reg_value3);
		reg_value3 = 0;
		return;
	}

	reg_value = AUDIO_SI_ReadReg(MICBST_CTRL);		//VREF voltage selection
	reg_value &= (0xffff&(~(3<<BIT_VREF_VREFSEL)));
	reg_value |= (2<<BIT_VREF_VREFSEL);
	AUDIO_SI_WriteReg(MICBST_CTRL, reg_value);

	if ((application&APP_LINE_OUT) == APP_LINE_OUT){
		reg_value = AUDIO_SI_ReadReg(DAC_ADC_MIC_CLK_CTRL); 	//dac modulation/filter/fifo on		
		reg_value |= (1<<BIT_DA_L_EN | 1<<BIT_DA_R_EN | 1<<BIT_MOD_L_EN | 1<<BIT_MOD_R_EN | 1<<BIT_DA_ANA_CLK_EN | 1<<BIT_DA_FIFO_EN);
		AUDIO_SI_WriteReg(DAC_ADC_MIC_CLK_CTRL, reg_value); 
		
		//step1
		reg_value = AUDIO_SI_ReadReg(GEN_CTRL);
		reg_value &= (1<<BIT_DTSDM_POW_L | 1<<BIT_DTSDM_POW_R);
		reg_value |= (1<<BIT_DAC_ADDACK_POW | 1<<BIT_DAC_CKXEN | 1<<BIT_DAC_L_POW | 1<<BIT_DAC_R_POW | 1<<BIT_DPRAMP_POW | 1<<BIT_DTSDM_CKXEN);
		AUDIO_SI_WriteReg(GEN_CTRL, reg_value);	
		
		reg_value3 = (3<<BIT_HPO_DPRSELL | 3<<BIT_HPO_DPRSELR | 1<< BIT_HPO_ENAL | 1<< BIT_HPO_ENAR |\
					1<<BIT_HPO_ENDPL | 1<<BIT_HPO_ENDPR | 1<<BIT_HPO_L_POW);
		AUDIO_SI_WriteReg(HPO_CTRL, reg_value3);
		reg_value3 = 0;

		reg_value = AUDIO_SI_ReadReg(HPO_MIC_CTRL);
		reg_value &= (1<<BIT_MICBIAS_POW | 3<<BIT_MICBIAS_VSET);
		reg_value |= (2<<BIT_HPO_ML | 2 <<BIT_HPO_MR | 1<<BIT_HPO_R_POW | 1<<BIT_HPO_SEL | 1<<BIT_HPO_SER | 1<<BIT_MBIAS_POW | 1<<BIT_VREF_POW);
		AUDIO_SI_WriteReg(HPO_MIC_CTRL, reg_value);
		
		reg_value3 = (1<<BIT_CKX_MICBIAS_EN);
		AUDIO_SI_WriteReg(CK_DEPOP_MICBIAS, reg_value3);
		reg_value3 = 0;
		
		//step2
		reg_value3 = (0xffff & (~(1<<BIT_HPO_CLPDPR | 1<<BIT_HPO_CLR | 1<<BIT_HPO_OPNDPL | 1<<BIT_HPO_OPNDPR)));
		AUDIO_SI_WriteReg(HPO_CTRL, reg_value3);
		reg_value3 = 0;
		
		reg_value = AUDIO_SI_ReadReg(HPO_MIC_CTRL);
		reg_value &= (1<<BIT_MICBIAS_POW | 3<<BIT_MICBIAS_VSET); 
		reg_value |= (2<<BIT_HPO_ML | 2<<BIT_HPO_MR | 1<<BIT_HPO_OPPDPR | 1<<BIT_HPO_R_POW | 1<<BIT_HPO_SEL | 1<<BIT_HPO_SER | 1<<BIT_MBIAS_POW | 1<<BIT_VREF_POW);
		AUDIO_SI_WriteReg(HPO_MIC_CTRL, reg_value);
				
		//step3
		reg_value = AUDIO_SI_ReadReg(GEN_CTRL);
		reg_value &= (1<<BIT_DTSDM_POW_L | 1<<BIT_DTSDM_POW_R);
		reg_value |= (1<<BIT_DAC_ADDACK_POW | 1<<BIT_DAC_CKXEN | 1<<BIT_DAC_L_POW | 1<<BIT_DAC_R_POW | 1<<BIT_DPRAMP_ENRAMP | 1<<BIT_DPRAMP_POW | 1<<BIT_DTSDM_CKXEN);
		AUDIO_SI_WriteReg(GEN_CTRL, reg_value);

		reg_value3 = (1<<BIT_BB_CK_DEPOP_EN | 1<<BIT_CKX_MICBIAS_EN);
		AUDIO_SI_WriteReg(CK_DEPOP_MICBIAS, reg_value3);
		reg_value3 = 0;
	}

	if (((application&APP_AMIC_IN) == APP_AMIC_IN) || ((application&APP_LINE_IN) == APP_LINE_IN)){
		reg_value = AUDIO_SI_ReadReg(HPO_MIC_CTRL);		//vref power on
		reg_value &= (0xffff & (~(1<<BIT_MICBST_ENDFL | 1<<BIT_MICBST_ENDFR )));
		reg_value |= (1<<BIT_MBIAS_POW | 1<<BIT_VREF_POW);
		AUDIO_SI_WriteReg(HPO_MIC_CTRL, reg_value);
		
		DelayUs(5);

		if ((application&APP_AMIC_IN) == APP_AMIC_IN){
			reg_value = AUDIO_SI_ReadReg(CK_DEPOP_MICBIAS);		//micbias chopper clock enable
			reg_value |= (1<<BIT_CKX_MICBIAS_EN);
			AUDIO_SI_WriteReg(CK_DEPOP_MICBIAS, reg_value);	
			
			reg_value = AUDIO_SI_ReadReg(HPO_MIC_CTRL);		//release micbias chopper clock gate and mic_bias on
			reg_value |= (1<<BIT_MICBIAS_ENCHX | 1<<BIT_MICBIAS_POW);
			AUDIO_SI_WriteReg(HPO_MIC_CTRL, reg_value);			
			DelayUs(100);

			reg_value = AUDIO_SI_ReadReg(MICBST_CTRL);		//PGA on and unmute mic
			reg_value &= (0xffff & (~(3<<BIT_MICBST_MUTE_L | 3<<BIT_MICBST_MUTE_R)));
			reg_value |= (2<<BIT_MICBST_MUTE_L | 2<<BIT_MICBST_MUTE_R | 3<<BIT_MICBST_POW);
			AUDIO_SI_WriteReg(MICBST_CTRL, reg_value);			
		}
		else{
			reg_value = AUDIO_SI_ReadReg(MICBST_CTRL);		//PGA on and unmute line-in			
			reg_value &= (0xffff & (~(3<<BIT_MICBST_GSELL | 3<<BIT_MICBST_GSELR | 3<<BIT_MICBST_MUTE_L | 3<<BIT_MICBST_MUTE_R)));
			reg_value |= (1<<BIT_MICBST_MUTE_L | 1<<BIT_MICBST_MUTE_R | 3<<BIT_MICBST_POW);
			AUDIO_SI_WriteReg(MICBST_CTRL, reg_value);	
		}
	}

	if (((application&APP_AMIC_IN) == APP_AMIC_IN) || ((application&APP_LINE_IN) == APP_LINE_IN) || ((application&APP_LINE_OUT) == APP_LINE_OUT)) { 
		DelayMs(200); 
	} 

	if ((application&APP_LINE_OUT) == APP_LINE_OUT){
		//step5
		reg_value = AUDIO_SI_ReadReg(GEN_CTRL);		
		reg_value &= (1<<BIT_DTSDM_POW_L | 1<<BIT_DTSDM_POW_R);
		reg_value |= (1<<BIT_DAC_ADDACK_POW | 1<<BIT_DAC_CKXEN | 1<<BIT_DAC_L_POW | 1<<BIT_DAC_R_POW | 1<<BIT_DTSDM_CKXEN);
		AUDIO_SI_WriteReg(GEN_CTRL, reg_value);

		reg_value3 = (3<<BIT_HPO_DPRSELL | 3<<BIT_HPO_DPRSELR | 3<<BIT_HPO_ENAL | 1<<BIT_HPO_L_POW);
		AUDIO_SI_WriteReg(HPO_CTRL, reg_value3);
		reg_value3 = 0;
		
		reg_value = AUDIO_SI_ReadReg(HPO_MIC_CTRL);
		reg_value &= (1<<BIT_MICBIAS_POW | 3<<BIT_MICBIAS_VSET);
		reg_value |= (2<<BIT_HPO_ML | 2<<BIT_HPO_MR | 2<<BIT_HPO_OPPDPR | 1<<BIT_HPO_SEL | 1<<BIT_HPO_SER | 1<<BIT_MBIAS_POW | 1<<BIT_VREF_POW);
		AUDIO_SI_WriteReg(HPO_MIC_CTRL, reg_value);
		
		reg_value3 = (1<<BIT_CKX_MICBIAS_EN);
		AUDIO_SI_WriteReg(CK_DEPOP_MICBIAS, reg_value3);
		reg_value3 = 0;
	}
	
	if (((application&APP_AMIC_IN) == APP_AMIC_IN) || ((application&APP_LINE_IN) == APP_LINE_IN)){
		reg_value = AUDIO_SI_ReadReg(GEN_CTRL);		//adc on		
		reg_value |= (1<<BIT_DAC_ADDACK_POW | 1<<BIT_DTSDM_CKXEN | 1<<BIT_DTSDM_POW_L | 1<<BIT_DTSDM_POW_R);
		AUDIO_SI_WriteReg(GEN_CTRL, reg_value);	
		DelayUs(50);
	}
	
	/* Configure ADC and DAC corresponding clock */
	reg_value3 = (1<<BIT_DA_L_EN | 1<<BIT_DA_R_EN | 1<<BIT_MOD_L_EN | 1<<BIT_MOD_R_EN | 1<<BIT_DA_ANA_CLK_EN | 1<<BIT_DA_FIFO_EN | 1<<BIT_AD_L_EN\
				| 1<<BIT_AD_R_EN | 1<<BIT_AD_FIFO_EN | 1<<BIT_AD_ANA_CLK_EN | 1<<BIT_AD_ANA_L_EN | 1<<BIT_AD_ANA_R_EN);						
	AUDIO_SI_WriteReg(DAC_ADC_MIC_CLK_CTRL, reg_value3);
	reg_value3 = 0;
	/* Left channel: Enable High pass filter enable control (filter DC), analog ADC input path mute control Left Channel */
	/* Enable ADC SRC 1st LPF control and ADC SRC 2nd LPF control, DMIC SRC 1st LPF fc:46.92kHz */
	reg_value3 = (1<<BIT_ADC_L_DMIC_LPF1ST_FC_SEL | 1<<BIT_ADC_L_AD_LPF2ND_EN | 1<<BIT_ADC_L_AD_LPF1ST_EN | 1<<BIT_ADC_L_DMIC_MIX_MUTE | 1<<BIT_ADC_L_AD_DCHPF_EN);
	
	AUDIO_SI_WriteReg(ADC_DMIC_L_FILTER_CTRL, reg_value3);
	reg_value3 = 0;	
	/* Right channel: Enable High pass filter enable control (filter DC), analog ADC input path mute control Left Channel */
	/* Enable ADC SRC 1st LPF control and ADC SRC 2nd LPF control, DMIC SRC 1st LPF fc:46.92kHz */
	reg_value3 = (1<<BIT_ADC_R_DMIC_LPF1ST_FC_SEL | 1<<BIT_ADC_R_AD_LPF2ND_EN | 1<<BIT_ADC_R_AD_LPF1ST_EN | 1<<BIT_ADC_R_DMIC_MIX_MUTE | 1<<BIT_ADC_R_AD_DCHPF_EN);	
	AUDIO_SI_WriteReg(ADC_DMIC_R_FILTER_CTRL, reg_value3);
	reg_value3 = 0;	
	
	if (((application&APP_AMIC_IN) == APP_AMIC_IN) || ((application&APP_LINE_IN) == APP_LINE_IN)){
		reg_value = AUDIO_SI_ReadReg(ADC_L_CTRL);
		reg_value2 = AUDIO_SI_ReadReg(ADC_R_ADJ_D);

		reg_value &= (0xffff&(~(7<<BIT_ADC_L_ADJ_HPF_COEF_SEL | 3<<BIT_ADC_L_AD_ZDET_FUNC)));
		reg_value |= (1<<BIT_ADC_L_ADJ_HPF_2ND_EN);
		
		reg_value2 &= (0xffff & (~(7<<BIT_ADC_R_ADJ_HPF_COEF_SEL | 3<<BIT_ADC_R_AD_ZDET_FUNC)));
		reg_value2 |= (1<<BIT_ADC_R_ADJ_HPF_2ND_EN);
		
		switch(sample_rate){
			case SR_8K:
			case SR_16K:
				break;
			case SR_32K:
				reg_value |= (1<<BIT_ADC_L_ADJ_HPF_COEF_SEL);
				reg_value2 |= (1<<BIT_ADC_R_ADJ_HPF_COEF_SEL);
				break;
			case SR_44P1K:
			case SR_48K:
				reg_value |= (2<<BIT_ADC_L_ADJ_HPF_COEF_SEL);
				reg_value2 |= (2<<BIT_ADC_R_ADJ_HPF_COEF_SEL);
				break;				
			case SR_88P2K:
			case SR_96K:
				reg_value |= (3<<BIT_ADC_L_ADJ_HPF_COEF_SEL);
				reg_value2 |= (3<<BIT_ADC_R_ADJ_HPF_COEF_SEL);
				break;
			default:
				break;					
		}

		AUDIO_SI_WriteReg(ADC_L_CTRL, reg_value);
		AUDIO_SI_WriteReg(ADC_R_ADJ_D, reg_value2);
		
		DelayMs(50);		//maybe need fine tune per board
	}
	
	reg_value3 = (1<<BIT_AUDIO_IP_TCON_EN | 1<<BIT_ASRC_FTK_LOOP_EN | 1<<BIT_ASRC_256FS_SYS_SEL | 1<<BIT_ASRC_EN);
	AUDIO_SI_WriteReg(ASRC_CTRL, reg_value3);
	reg_value3 = 0;
	
	/* ADC and DAC sample rate 16K */
	AUDIO_SI_WriteReg(DAC_ADC_SR_CTRL, (sample_rate << 4) | sample_rate);
	/* Left channel: mon DAC Lch 128fs-domain mixer sidetone path mute enable, Digital DAC & ADC loop back control, mon DAC Lch dvol mute enable */
	reg_value = AUDIO_SI_ReadReg(DAC_L_MUTE_CTRL);
	//AUDIO_SI_WriteReg(0xfb, 0x4);
	reg_value |= BIT3;
	AUDIO_SI_WriteReg(DAC_L_MUTE_CTRL, reg_value & 0xfff9);
	/* Right channel: mon DAC Rch 128fs-domain mixer sidetone path mute enable, Digital DAC & ADC loop back control, mon DAC Rch dvol mute enable */
	reg_value = AUDIO_SI_ReadReg(DAC_R_MUTE_CTRL);
	reg_value |= BIT3;
	AUDIO_SI_WriteReg(DAC_R_MUTE_CTRL, reg_value & 0xfff9);
	/* Enable control for dac filter; ALC is stereo mode */
	reg_value = AUDIO_SI_ReadReg(ALC_MIN_GAIN);	
	reg_value &= (0xffff &(~(1<<BIT_DA_STEREO_MODE_EN)));
	
	if (mono_stereo == CH_STEREO){
		reg_value |= BIT8;
	}
	AUDIO_SI_WriteReg(ALC_MIN_GAIN, reg_value);
	reg_value = AUDIO_SI_ReadReg(DAC_L_CTRL);
	reg_value &= (0xff<<BIT_DAC_L_DA_GAIN | 1<<BIT_DAC_L_DAHPF_EN | 2<<BIT_DAC_L_DA_DITHER_SEL);
	reg_value |= (2 << BIT_DAC_L_DA_ZDET_FUNC);
	
	AUDIO_SI_WriteReg(DAC_L_CTRL, reg_value);
	AUDIO_SI_WriteReg(DAC_R_CTRL, reg_value);


	if ((application&APP_DMIC_IN) == APP_DMIC_IN){
		reg_value = AUDIO_SI_ReadReg(ADC_DMIC_L_FILTER_CTRL);
		reg_value &= (0xffff & (~(1<<BIT_ADC_L_DMIC_RI_FA_SEL | 1<<BIT_ADC_L_DMIC_MIX_MUTE)));
		reg_value |= (1<<BIT_ADC_L_DMIC_RI_FA_SEL | 1<<BIT_ADC_L_DMIC_LPF2ND_EN | 1<<BIT_ADC_L_DMIC_LPF1ST_EN | 1<<BIT_ADC_L_AD_MIX_MUTE | 1<<BIT_ADC_L_AD_DCHPF_EN);
		AUDIO_SI_WriteReg(ADC_DMIC_L_FILTER_CTRL, reg_value);
		
		reg_value = AUDIO_SI_ReadReg(ADC_L_CTRL);
		reg_value |= (1<<BIT_ADC_L_ADJ_HPF_2ND_EN);
		AUDIO_SI_WriteReg(ADC_L_CTRL, reg_value);

		reg_value = AUDIO_SI_ReadReg(ADC_DMIC_R_FILTER_CTRL);
		reg_value &= (0xffff & (~(1<<BIT_ADC_R_DMIC_RI_FA_SEL | 1<<BIT_ADC_R_DMIC_MIX_MUTE)));
		reg_value |= (1<<BIT_ADC_L_DMIC_LPF2ND_EN | 1<<BIT_ADC_L_DMIC_LPF1ST_EN | 1<<BIT_ADC_L_AD_MIX_MUTE | 1<<BIT_ADC_L_AD_DCHPF_EN);
		AUDIO_SI_WriteReg(ADC_DMIC_R_FILTER_CTRL, reg_value);	//bit0: 0 means latching rising edge, whereas 1 means falling edge.
		
		reg_value = AUDIO_SI_ReadReg(ADC_R_ADJ_D);
		reg_value |= (1<<BIT_ADC_R_ADJ_HPF_2ND_EN);
		AUDIO_SI_WriteReg(ADC_R_ADJ_D, reg_value);

		reg_value = AUDIO_SI_ReadReg(DAC_ADC_SR_CTRL);
		reg_value &= (0xffff&(~(7<<BIT_DMIC_CLK_SEL)));
		reg_value |= (1<<BIT_DMIC_CLK_SEL);
		AUDIO_SI_WriteReg(DAC_ADC_SR_CTRL, reg_value);	

		reg_value = AUDIO_SI_ReadReg(DAC_ADC_MIC_CLK_CTRL);		
		reg_value |= (1<<BIT_DMIC_L_EN | 1<<BIT_DMIC_R_EN | 1<<BIT_DMIC_CLK_EN);
		AUDIO_SI_WriteReg(DAC_ADC_MIC_CLK_CTRL, reg_value);
	}

	if ((sample_rate == SR_96K) || (sample_rate == SR_88P2K)){
		reg_value = AUDIO_SI_ReadReg(DAC_ADC_SR_CTRL);
		reg_value &= (0xffff &(~(15<<BIT_ASRC_FSI_RATE_MANUAL)));
		reg_value |= (1<<BIT_ASRC_FSI_RATE_MANUAL);
		AUDIO_SI_WriteReg(DAC_ADC_SR_CTRL, reg_value);

		reg_value = AUDIO_SI_ReadReg(ASRC_CTRL);
		reg_value &= (0xffff&(~(3<<BIT_ASRC_256FS_SYS_SEL))); 
		AUDIO_SI_WriteReg(ASRC_CTRL, reg_value);		
	}
	
	reg_value = ((word_len << 4) |mono_stereo | 0x0080);
	AUDIO_SI_WriteReg(I2S_CTRL, reg_value);
}

/**
  * @brief  Set line out to differential mode.
  * @param  None
  * @return  None
  */
void CODEC_SetLineoutDifferential(){
	uint32_t reg_value = 0;
	reg_value = AUDIO_SI_ReadReg(HPO_MIC_CTRL);
	reg_value &= (0xffff&(~(1<<BIT_HPO_SEL | 1<<BIT_HPO_SER)));
	AUDIO_SI_WriteReg(HPO_MIC_CTRL, reg_value);
}


/**
  * @brief  Set codec volume by controlling mon DAC channel dvol gain.
  * @param  vol_lch: mon DAC Lch dvol gain control(0.375dB/step)
  *          This parameter can be one of the following values:
  *            @arg 8'hAF: 0dB
  *            @arg ...
  *            @arg 8'h00: -65.625dB
  * @param  vol_rch: mon DAC Rch dvol gain control(0.375dB/step)
  *          This parameter can be one of the following values:
  *            @arg 8'hAF: 0dB
  *            @arg ...
  *            @arg 8'h00: -65.625dB
  * @return  None
  */
void CODEC_SetVolume(u8 vol_lch, u8 vol_rch)
{
	u32 reg_value = 0;
	
	reg_value = AUDIO_SI_ReadReg(DAC_L_CTRL);
	reg_value &= ~0xff;
	reg_value |= vol_lch;

	AUDIO_SI_WriteReg(DAC_L_CTRL, reg_value);

	reg_value = AUDIO_SI_ReadReg(DAC_R_CTRL);
	reg_value &= ~0xff;
	reg_value |= vol_rch;

	AUDIO_SI_WriteReg(DAC_R_CTRL, reg_value);
}

/**
  * @brief  Get codec mon DAC channel gain control.
  * @param  vol: mon DAC DAC channel dvol gain
  * @note vol value high 8 bits is rch gain, low 8 bits are lch gain
  * @return  None
  */
void CODEC_GetVolume(u16 *vol)
{
	u8 vol_lch = 0;
	u8 vol_rch = 0;
	
	vol_lch = AUDIO_SI_ReadReg(DAC_L_CTRL) & 0xff;
	vol_rch = AUDIO_SI_ReadReg(DAC_R_CTRL) & 0xff;
	
	*vol = (vol_rch << 8) | vol_lch; 
}

/**
  * @brief  Set codec ADC and DAC sample rate.
  * @param  sample_rate: codec ADC and DAC sample rate.
  *          This parameter can be one of the following values:
  *            @arg SR_8K: sample rate is 8kHz
  *            @arg SR_16K: sample rate is 16kHz
  *            @arg SR_32K: sample rate is 32kHz
  *            @arg SR_48K: sample rate is 48kHz
  *            @arg SR_96K: sample rate is 96kHz
  *            @arg SR_44P1K: sample rate is 44.1kHz
  *            @arg SR_88P2K: sample rate is 88.2kHz
  * @return  None
  */
void CODEC_SetSr(u32 sample_rate){
	u32 reg_value = 0;
	
	if ((sample_rate == SR_96K) || (sample_rate == SR_88P2K)){
		reg_value = AUDIO_SI_ReadReg(DAC_ADC_SR_CTRL);
		reg_value &= (7<<BIT_DMIC_CLK_SEL | 1<<BIT_ASRC_FSI_GATING_EN);
		reg_value |= (0x0800|(sample_rate << 4) | sample_rate);
		AUDIO_SI_WriteReg(DAC_ADC_SR_CTRL, reg_value);
		
		reg_value = AUDIO_SI_ReadReg(ASRC_CTRL);	
		reg_value &= (0xffff & (~(3<<BIT_ASRC_256FS_SYS_SEL)));
		AUDIO_SI_WriteReg(ASRC_CTRL, reg_value);		
	}
	else{
		reg_value = AUDIO_SI_ReadReg(DAC_ADC_SR_CTRL);
		reg_value &= (7<<BIT_DMIC_CLK_SEL | 1<<BIT_ASRC_FSI_GATING_EN);
		reg_value |= ((sample_rate << 4) | sample_rate);

		AUDIO_SI_WriteReg(DAC_ADC_SR_CTRL, reg_value);

		reg_value = AUDIO_SI_ReadReg(ASRC_CTRL);
		reg_value &= (0xffff & (~(3<<BIT_ASRC_256FS_SYS_SEL)));
		reg_value |=(1<<BIT_ASRC_256FS_SYS_SEL);
		AUDIO_SI_WriteReg(ASRC_CTRL, reg_value);			
	}

}

/**
  * @brief  Set codec ADC and DAC channel number.
  * @param  mono_stereo: codec ADC and DAC channel number.
  *          This parameter can be one of the following values:
  *            @arg CH_MONO: mono
  *            @arg CH_STEREO: stereo
  * @return  None
  */
void CODEC_SetCh(u32 mono_stereo){
	u32 reg_value = 0;

	reg_value = AUDIO_SI_ReadReg(I2S_CTRL);
	reg_value &= (0xffff & ~(1<<BIT_EN_I2S_MONO));
	reg_value |= mono_stereo;

	AUDIO_SI_WriteReg(I2S_CTRL, reg_value);	

	reg_value = AUDIO_SI_ReadReg(ALC_MIN_GAIN);
	reg_value &= (0xffff & ~(1<<BIT_DA_STEREO_MODE_EN));
	if (mono_stereo == CH_STEREO){
		reg_value |= BIT8;
	}

	AUDIO_SI_WriteReg(ALC_MIN_GAIN, reg_value);
}


/**
  * @brief  Set codec ADC gain.
  * @param  ad_gain_left: ADC left channel digital volume gain
  *          This parameter can be one of the following values:
  *            @arg 7'h00: -17.625dB
  *            @arg ...
  *            @arg 7'h2f: 0dB
  *            @arg 7'h30: 0.375dB
  *            @arg ...
  *            @arg 7'h7f: 30dB
  * @param  ad_gain_right: ADC right channel digital volume gain
  *          This parameter can be one of the following values:
  *            @arg 7'h00: -17.625dB
  *            @arg ...
  *            @arg 7'h2f: 0dB
  *            @arg 7'h30: 0.375dB
  *            @arg ...
  *            @arg 7'h7f: 30dB
  * @note ADC digital volume is -17.625dB~+30dB in 0.375 dB step.
  * @return  None
  */
void CODEC_SetAdcGain(u32 ad_gain_left, u32 ad_gain_right)
{
	u32 reg_value = 0;

	reg_value = AUDIO_SI_ReadReg(ADC_L_GAIN);
	reg_value &= ~(0x7f<<ADC_L_AD_GAIN);
	reg_value |= (ad_gain_left << ADC_L_AD_GAIN);
	
	AUDIO_SI_WriteReg(ADC_L_GAIN, reg_value);
	
	reg_value = AUDIO_SI_ReadReg(ADC_R_GAIN);
	reg_value &= ~(0x7f<<ADC_R_AD_GAIN);
	reg_value |= (ad_gain_left << ADC_R_AD_GAIN);
	AUDIO_SI_WriteReg(ADC_R_GAIN, reg_value);	
}

/**
  * @brief  Set codec amic boost.
  * @param  amic_bst_left: AMIC left channel boost gain
  *          This parameter can be one of the following values:
  *            @arg 2'b00: 0dB
  *            @arg 2'b01: 20dB
  *            @arg 2'b10: 30dB
  *            @arg 2'b11: 40dB
  * @param  amic_bst_right: AMIC right channel boost gain
  *          This parameter can be one of the following values:
  *            @arg 2'b00: 0dB
  *            @arg 2'b01: 20dB
  *            @arg 2'b10: 30dB
  *            @arg 2'b11: 40dB
  * @return  None
  */
void CODEC_SetAmicBst(u32 amic_bst_left, u32 amic_bst_right)
{
	u32 reg_value = 0;

	reg_value = AUDIO_SI_ReadReg(MICBST_CTRL);
	reg_value &= ~(15<<BIT_MICBST_GSELL);
	reg_value |= ((amic_bst_left&0x03)|((amic_bst_right&0x03)<<2));

	AUDIO_SI_WriteReg(MICBST_CTRL, reg_value);
}

/**
  * @brief  Set codec dmic boost.
  * @param  dmic_bst_left: DMIC left channel boost gain
  *          This parameter can be one of the following values:
  *            @arg 2'b00: 0dB
  *            @arg 2'b01: 12dB
  *            @arg 2'b10: 24dB
  *            @arg 2'b11: 36dB
  * @param  dmic_bst_right: DMIC right channel boost gain
  *          This parameter can be one of the following values:
  *            @arg 2'b00: 0dB
  *            @arg 2'b01: 12dB
  *            @arg 2'b10: 24dB
  *            @arg 2'b11: 36dB
  * @return  None
  */
void CODEC_SetDmicBst(u32 dmic_bst_left, u32 dmic_bst_right)
{
	u32 reg_value = 0;

	reg_value = AUDIO_SI_ReadReg(ADC_L_CTRL);
	reg_value &= ~(3<<BIT_ADC_L_DMIC_BOOST_GAIN);
	reg_value |= (dmic_bst_left&0x03)<<BIT_ADC_L_DMIC_BOOST_GAIN;

	AUDIO_SI_WriteReg(ADC_L_CTRL, reg_value);
	
	reg_value = AUDIO_SI_ReadReg(ADC_R_ADJ_D);
	reg_value &= ~(3<<BIT_ADC_R_DMIC_BOOST_GAIN);
	
	reg_value |= (dmic_bst_right&0x03)<<BIT_ADC_R_DMIC_BOOST_GAIN;
	AUDIO_SI_WriteReg(ADC_R_ADJ_D, reg_value);
}

/**
  * @brief  Set MIC_BIAS output voltage.
  * @param  mic_bias: micphone bias voltage setting
  *          This parameter can be one of the following values:
  *            @arg 2'b00: 0.90*AVCC
  *            @arg 2'b01: 0.86*AVCC
  *            @arg 2'b10: 0.75*AVCC
  *            @arg 2'b11: reserved
  * @note AVCC is 2.8V@3.3V operation mode or 1.7V@1.8V operation mode
  * @return  None
  */
void CODEC_SetMicBias(u8 mic_bias)
{
	u32 reg_value = 0;
	reg_value = AUDIO_SI_ReadReg(HPO_MIC_CTRL);
	reg_value &= ~(3<<BIT_MICBIAS_VSET);
	reg_value |= ((mic_bias&0x03) << BIT_MICBIAS_VSET);
	AUDIO_SI_WriteReg(HPO_MIC_CTRL, reg_value);		
}

/**
  * @brief  mute or unmute per ad channel.
  * @param  mute_lch: mute option for left ad channel
  *			 This parameter can be one of the following values:
  *			   @arg MUTE_DISABLE: unmute
  *			   @arg MUTE_ENABLE: mute
  *			   @arg MUTE_NO_ACT: no modification
  * @param  mute_rch: mute option for right ad channel
  *			 This parameter can be one of the following values:
  *			   @arg MUTE_DISABLE: unmute
  *			   @arg MUTE_ENABLE: mute
  *			   @arg MUTE_NO_ACT: no modification 
  * @return  None
  */
void CODEC_MuteRecord(u32 mute_lch, u32 mute_rch)
{
	u32 reg_value = 0;

	if (mute_lch == MUTE_ENABLE){
		reg_value = AUDIO_SI_ReadReg(ADC_L_CTRL);		//left ad mute
		reg_value |= (1<<BIT_ADC_L_AD_MUTE);
		AUDIO_SI_WriteReg(ADC_L_CTRL, reg_value);				
	}
	else if (mute_lch == MUTE_DISABLE){
		reg_value = AUDIO_SI_ReadReg(ADC_L_CTRL);		//left ad unmute
		reg_value &= (0xffff & (~(1<<BIT_ADC_L_AD_MUTE)));
		AUDIO_SI_WriteReg(ADC_L_CTRL, reg_value);		
	}

	if (mute_rch == MUTE_ENABLE){
		reg_value = AUDIO_SI_ReadReg(ADC_R_ADJ_D);		//right ad mute
		reg_value |= (1<<BIT_ADC_R_AD_MUTE);
		AUDIO_SI_WriteReg(ADC_R_ADJ_D, reg_value);		
	}
	else if (mute_rch == MUTE_DISABLE){
		reg_value = AUDIO_SI_ReadReg(ADC_R_ADJ_D);		//right ad unmute		
		reg_value &= (0xffff & (~(1<<BIT_ADC_R_AD_MUTE)));
		AUDIO_SI_WriteReg(ADC_R_ADJ_D, reg_value);
	}
	return;
}

/**
  * @brief  mute or unmute per da channel.
  * @param  mute_lch: mute option for left da channel
  *			 This parameter can be one of the following values:
  *			   @arg MUTE_DISABLE: unmute
  *			   @arg MUTE_ENABLE: mute
  *			   @arg MUTE_NO_ACT: no modification
  * @param  mute_rch: mute option for right da channel
  *			 This parameter can be one of the following values:
  *			   @arg MUTE_DISABLE: unmute
  *			   @arg MUTE_ENABLE: mute
  *			   @arg MUTE_NO_ACT: no modification 
  * @return  None
  */
void CODEC_MutePlay(u32 mute_lch, u32 mute_rch)
{
	u32 reg_value = 0;

	if (mute_lch == MUTE_ENABLE){
		reg_value = AUDIO_SI_ReadReg(DAC_L_MUTE_CTRL); 	//left da mut
		reg_value |= (1<<BIT_DAC_L_DA_MUTE);
		AUDIO_SI_WriteReg(DAC_L_MUTE_CTRL, reg_value);		
	}
	else if (mute_lch == MUTE_DISABLE){
		reg_value = AUDIO_SI_ReadReg(DAC_L_MUTE_CTRL); 	//left da unmute
		reg_value &= (0xffff & (~(1<<BIT_DAC_L_DA_MUTE)));
		AUDIO_SI_WriteReg(DAC_L_MUTE_CTRL, reg_value); 
	}
	
	if (mute_rch == MUTE_ENABLE){
		reg_value = AUDIO_SI_ReadReg(DAC_R_MUTE_CTRL); 	//right da mute
		reg_value |= (1<<BIT_DAC_R_DA_MUTE);
		AUDIO_SI_WriteReg(DAC_R_MUTE_CTRL, reg_value); 		
	}
	else if (mute_rch == MUTE_DISABLE){
		reg_value = AUDIO_SI_ReadReg(DAC_R_MUTE_CTRL); 	//right da unmute		
		reg_value &= (0xffff & (~(1<<BIT_DAC_R_DA_MUTE)));
		AUDIO_SI_WriteReg(DAC_R_MUTE_CTRL, reg_value); 	
	}
	return;
}

/**
  * @brief  De-initialize codec peripheral.
  * @param  application: application mode
  *          This parameter can be one of the following values:
  *            @arg APP_AMIC_IN: analog microphone in
  *            @arg APP_DMIC_IN: digital microphone in
  *            @arg APP_LINE_IN: line in
  *            @arg APP_LINE_OUT: line out
  * @return  None
  */
void CODEC_DeInit(u32 application)
{
	u32 reg_value = 0;
	
	if (((application&APP_AMIC_IN) == APP_AMIC_IN) || ((application&APP_LINE_IN) == APP_LINE_IN)){
#if 0
		reg_value = AUDIO_SI_ReadReg(ADC_DMIC_L_FILTER_CTRL);		//left ad filter off
		reg_value &= ~0x4000;
		AUDIO_SI_WriteReg(ADC_DMIC_L_FILTER_CTRL, reg_value);	
		
		reg_value = AUDIO_SI_ReadReg(ADC_DMIC_R_FILTER_CTRL);		//right ad filter off		
		reg_value &= ~0x4000;
		AUDIO_SI_WriteReg(ADC_DMIC_R_FILTER_CTRL, reg_value);		
#endif		
		reg_value = AUDIO_SI_ReadReg(GEN_CTRL);		//adc off
		reg_value &= ~(1<<BIT_DTSDM_POW_L | 1<<BIT_DTSDM_POW_R);
		AUDIO_SI_WriteReg(GEN_CTRL, reg_value);	
		
		reg_value = AUDIO_SI_ReadReg(MICBST_CTRL);		//PGA off and path mute		
		reg_value &= ~(3<<BIT_MICBST_POW);
		reg_value |= (3<<BIT_MICBST_MUTE_L | 3<<BIT_MICBST_MUTE_R);
		AUDIO_SI_WriteReg(MICBST_CTRL, reg_value);	
		
		if ((application&APP_AMIC_IN) == APP_AMIC_IN){
			reg_value = AUDIO_SI_ReadReg(HPO_MIC_CTRL);		//mic_bias off
			reg_value &= ~(1<<BIT_MICBIAS_POW);
			AUDIO_SI_WriteReg(HPO_MIC_CTRL, reg_value);				
		}
	}

	if ((application&APP_DMIC_IN) == APP_DMIC_IN){
		reg_value = AUDIO_SI_ReadReg(DAC_ADC_MIC_CLK_CTRL);//dmic clock off
		reg_value &= ~(1<<BIT_DMIC_L_EN | 1<<BIT_DMIC_R_EN | 1<<BIT_DMIC_CLK_EN);
		AUDIO_SI_WriteReg(DAC_ADC_MIC_CLK_CTRL, reg_value);
	}

	if ((application&APP_LINE_OUT) == APP_LINE_OUT){
		reg_value = AUDIO_SI_ReadReg(HPO_CTRL);		//amplifier off
		reg_value &= ~(1<<BIT_HPO_ENAL | 1<<BIT_HPO_ENAR);
		AUDIO_SI_WriteReg(HPO_CTRL, reg_value);
		
		reg_value = AUDIO_SI_ReadReg(HPO_MIC_CTRL);		//path mute and right channel off
		reg_value &= ~(1<<BIT_HPO_R_POW);
		reg_value |= (3<< BIT_HPO_ML | 3<<BIT_HPO_MR);
		AUDIO_SI_WriteReg(HPO_MIC_CTRL, reg_value);

		reg_value = AUDIO_SI_ReadReg(HPO_CTRL);		//left channel off and amplifier off
		reg_value &= ~(1<<BIT_HPO_L_POW);
		AUDIO_SI_WriteReg(HPO_CTRL, reg_value);		

		reg_value = AUDIO_SI_ReadReg(GEN_CTRL);		//dac off
		reg_value &= ~(1<<BIT_DAC_L_POW | 1<<BIT_DAC_R_POW);
		AUDIO_SI_WriteReg(GEN_CTRL, reg_value);		

		reg_value = AUDIO_SI_ReadReg(DAC_ADC_MIC_CLK_CTRL);		//dac modulation/filter/fifo reset		
		reg_value &= ~(1<<BIT_DA_L_EN | 1<<BIT_DA_R_EN | 1<<BIT_MOD_L_EN | 1<<BIT_MOD_R_EN | 1<<BIT_DA_ANA_CLK_EN | 1<<BIT_DA_FIFO_EN);
		AUDIO_SI_WriteReg(DAC_ADC_MIC_CLK_CTRL, reg_value);			
	}
}

/**
  * @brief  Config codec DA path EQ parameters.
  * @param  sample_rate: codec DAC sample rate.
  *          This parameter can be one of the following values:
  *            @arg SR_8K: sample rate is 8kHz
  *            @arg SR_16K: sample rate is 16kHz
  *            @arg SR_48K: sample rate is 48kHz
  *            @arg SR_44P1K: sample rate is 44.1kHz
  * @return  None
  */
void CODEC_DacEqConfig(u32 sample_rate)
{
	u32 cnt;
	u32 eq_en_num;
	u32 *eq_param;

	switch (sample_rate){
		case SR_44P1K:
			eq_en_num = eq_44p1k_en_num;
			eq_param = (u32 *)eq_param_44p1k;
			break;
		case SR_48K:
			eq_en_num = eq_48k_en_num;
			eq_param = (u32 *)eq_param_48k;
			break;
		case SR_8K:
			eq_en_num = eq_8k_en_num;
			eq_param = (u32 *)eq_param_8k;
			break;
		case SR_16K:
			eq_en_num = eq_16k_en_num;
			eq_param = (u32 *)eq_param_16k;
			break;
		default:
			eq_en_num = 0;
			break;
	}
	
	if (!eq_en_num){
		return;
	}
	for(cnt=0; cnt<eq_en_num*5; cnt++){
		AUDIO_SI_WriteReg(DAC_L_BIQUAD_H0_1L + cnt*2, (u16)(eq_param[cnt]));
		AUDIO_SI_WriteReg(DAC_L_BIQUAD_H0_1H + cnt*2, (u16)((eq_param[cnt])>>16));
	}
	for(cnt=0; cnt<eq_en_num*5; cnt++){
		AUDIO_SI_WriteReg(DAC_R_BIQUAD_H0_1L + cnt*2, (u16)(eq_param[cnt]));
		AUDIO_SI_WriteReg(DAC_R_BIQUAD_H0_1H + cnt*2, (u16)((eq_param[cnt])>>16));
	}
	
	AUDIO_SI_WriteReg(DAC_L_BQ_EQ_CTRL, 0x0);
	AUDIO_SI_WriteReg(DAC_L_BQ_EQ_CTRL, 0x1);
	AUDIO_SI_WriteReg(DAC_R_BQ_EQ_CONTROL, 0x0);
	AUDIO_SI_WriteReg(DAC_R_BQ_EQ_CONTROL, 0x1);
}

/**
  * @brief  Enable ALC function and set limiter threshold value
  * @param  limiter_val: ALC main-limiter threshold level (at amplitude domain) control
  *          This parameter can be one of the following values:
  *            @arg 32'h00: 0dB
  *            @arg 32'h01: -0.375dB
  *            @arg 32'h02: -0.750dB
  *            @arg ...
  *            @arg 32'h3F: -23.625dB
  * @return  None
  */
void CODEC_SetALC(u32 limiter_val) 
{
	u32 reg_value = AUDIO_SI_ReadReg(ALC_DRC_CTRL);
	reg_value |= (1 | 1 << 6);
	AUDIO_SI_WriteReg(ALC_DRC_CTRL, reg_value);

	reg_value = AUDIO_SI_ReadReg(ALC_RATE_CTRL);
	reg_value |= (limiter_val << 10);
	AUDIO_SI_WriteReg(ALC_RATE_CTRL, reg_value);
}

/**
  * @brief  De-initialize ALC function.
  * @param  None
  * @return  None
  */
void CODEC_ALC_deinit() 
{
	u32 reg_value = AUDIO_SI_ReadReg(ALC_DRC_CTRL);
	reg_value &= (~(1 | 1 << 6));
	AUDIO_SI_WriteReg(ALC_DRC_CTRL, reg_value);
}

/******************* (C) COPYRIGHT 2018 Realtek Semiconductor *****END OF FILE****/
