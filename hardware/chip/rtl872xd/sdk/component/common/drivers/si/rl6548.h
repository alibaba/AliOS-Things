/**
  ******************************************************************************
  * @file    rl6548.h
  * @author
  * @version V1.0.0
  * @date    2018-12-12
  * @brief   This file contains all the functions prototypes for the audio codec firmware
  *          library.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2018, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */
  
#ifndef  __RL6548_H__
#define __RL6548_H__

/** @addtogroup AmebaD_Periph_Driver
  * @{
  */

/** @defgroup audio
  * @brief audio driver modules
  * @{
  */

/** @addtogroup audio
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * codec:
  * 	- Support anti-pop function to reduce audible pop
  * 	- Support AMIC-in, DMIC-in, Line-in and Line-out
  * 	- Sample rate: 8/16/32/44.1/48/96 KHz
  * 	- Sample bit: 16 bit, 24 bit, 8bit
  * 	- Channel number: mono or stereo
  * 	- Data format: I2S, Left justified, PCM mode A, PCM mode B, PCM mode A-N, PCM mode B-N
  * 	- Gain Control in ADC and DAC Path
  *
  * 	audio SI:
  * 	- Base Address: AUDIO_SI_DEV
  * 	- can use to configure codec register
  *
  *****************************************************************************************
  * How to use audio SI & codec API
  *****************************************************************************************
  *      To use audio codec si, the following steps are mandatory:
  *
  *      1. Open audio codec clock and function using
  *			PLLx_Set(0, ENABLE); (x is 0 or 1)
  *			RCC_PeriphClockCmd(APBPeriph_AUDIOC, APBPeriph_AUDIOC_CLOCK, ENABLE);
  *			RCC_PeriphClockCmd(APBPeriph_SPORT, APBPeriph_SPORT_CLOCK, ENABLE);
  *
  *      2. AUDIO SI enable:
  *			AUDIO_SI_Cmd(ENABLE).
  *
  *      3. Write AUDIO Codec registers using:
  *			AUDIO_SI_WriteReg(address, data);
  *
  *      4. Read AUDIO Codec registers using:
  *			AUDIO_SI_ReadReg(address);
  *
  *      @note Turn on AUDIO Codec register bank clock using:
  *			AUDIO_SI_ClkCmd(ENABLE);
  *
  *      @note Turn off AUDIO Codec register bank clock using:
  *			AUDIO_SI_ClkCmd(DISABLE);
  *
  *      To use audio codec si, the following steps are mandatory:
  *
  *      1. Codec initialize using
  *			CODEC_Init(sample_rate, word_len, mono_stereo, application);
  *
  *      2. Codec set volume using
  *			 CODEC_SetVolume(vol_lch, vol_rch);
  *
  *      3. Codec get volume using
  *			CODEC_GetVolume(*vol);
  *
  *      4. Set codec ADC and DAC sample rate using
  *			CODEC_SetSr(sample_rate);
  *
  *      5. Set codec ADC gain using
  *			CODEC_SetAdcGain(ad_gain_left, ad_gain_right);
  *
  *		 6. Set codec MIC_BIAS output voltage
  *			CODEC_SetMicBias(mic_bias);
  *
  *		 7. Codec de-initialize using
  *			CODEC_DeInit(application);
  *
  *      @note    All other functions can be used separately to modify, if needed,
  *			a specific feature of the AUDIO
  *****************************************************************************************
  * @endverbatim
  */

/* Exported constants --------------------------------------------------------*/

/** @defgroup AUDIO_Exported_Constants AUDIO Exported Constants
  * @{
  */
  
/** @defgroup CODEC_sample_rate AUDIO Codec Sample Rate
  * @{
  */
#define SR_48K			((u32)0x00000000)
#define SR_96K			((u32)0x00000001)
#define SR_32K			((u32)0x00000003)
#define SR_16K			((u32)0x00000005)
#define SR_8K			((u32)0x00000007)
#define SR_44P1K		((u32)0x00000008)
#define SR_88P2K		((u32)0x00000009)

/**
  * @}
  */

/** @defgroup CODEC_word_len AUDIO Codec Word Len
  * @{
  */
#define WL_16			((u32)0x00000000)
#define WL_24			((u32)0x00000002)	
#define WL_8				((u32)0x00000003)

/**
  * @}
  */

/** @defgroup CODEC_channel_mode AUDIO Codec Channel Mode
  * @{
  */
#define CH_STEREO		((u32)0x00000000)
#define CH_MONO			((u32)0x00000001)

/**
  * @}
  */

/** @defgroup CODEC_application_mode AUDIO CODEC Application Mode
  * @{
  */
#define APP_AMIC_IN		((u32)0x00000001)
#define APP_DMIC_IN		((u32)0x00000002)
#define APP_LINE_IN		((u32)0x00000004)
#define APP_LINE_OUT	((u32)0x00000008)
#define APP_DAAD_LPBK	((u32)0x00000010)

/**
  * @}
  */

/** @defgroup CODEC_mute_action AUDIO CODEC mute action per channel
  * @{
  */
#define MUTE_DISABLE	((u32)0x00000000)
#define MUTE_ENABLE		((u32)0x00000001)
#define MUTE_NO_ACT		((u32)0x00000002)

/**
  * @}
  */


/** @defgroup Codec_rx_channel_selection AUDIO CODEC Rx Channel Selection
  * @{
  */
#define RX_CH_LR						((u32)0x00000000)
#define RX_CH_RL						((u32)0x00000001)
#define RX_CH_LL						((u32)0x00000002)
#define RX_CH_RR						((u32)0x00000003)

/**
  * @}
  */
  
/**
  * @}
  */

/** @defgroup  Register Macro Definition and Bit Definition
  * @{
  */
#define GEN_CTRL 							0x00	
#define HPO_CTRL 							0x01
#define HPO_MIC_CTRL 						0x02
#define MICBST_CTRL 						0x03
#define DEBUG_BUS_SEL 						0x04
#define CK_DEPOP_MICBIAS 					0x0c
#define SIDETONE_CTRL 						0x0f
#define I2S_CTRL 							0x10
#define ADC_DMIC_L_FILTER_CTRL 				0x11
#define ADC_L_CTRL 							0x12
#define ADC_L_GAIN 							0x13
#define ADC_DMIC_R_FILTER_CTRL 				0x14
#define ADC_R_ADJ_D  						0x15
#define ADC_R_GAIN 							0x16
#define DAC_ADC_SR_CTRL 					0x17
#define DAC_ADC_MIC_CLK_CTRL 				0x18
#define ASRC_FTK_SDM_INI 					0x19
#define GEN_SRC_CLK_CTRL 					0x1a
#define ASRC_CTRL 							0x1b
#define ALC_ATK_CTRL 						0x1c
#define ALC_ZD_BK 							0x1d
#define ALC_DRC_CTRL 						0x1e
#define ALC_NOISE_CTRL 						0x1f
#define ALC_VAL_CTRL 						0x20
#define ALC_RATE_CTRL 						0x21
#define ALC_RC_WD_CTRL 						0x22
#define ALC_TH_CTRL 						0x23
#define ALC_ZERO_CTRL 						0x24
#define ALC_LIMITER_TH1 					0x25
#define ALC_LIMITER_TH2 					0x26
#define ALC_MIN_GAIN 						0x27
#define ALC_MODE_CTRL 						0x28
#define ALC_STATUS_REG 						0x29
#define DAC_L_SILENCE_CTRL 					0xf6
#define DAC_R_SILENCE_CTRL 					0Xf7
#define ADC_L_SILENCE_CTRL 					0xf8
#define ADC_R_SILENCE_CTRL 					0xf9
#define DAC_L_CTRL 							0xfa
#define DAC_L_MUTE_CTRL 					0xfb
#define DAC_R_CTRL 							0xfc
#define DAC_R_MUTE_CTRL 					0xfd

#define DAC_L_BQ_EQ_CTRL 					0x2a
#define DAC_L_BIQUAD_H0_1L 					0x2b
#define DAC_L_BIQUAD_H0_1H 					0x2c
#define DAC_L_BIQUAD_B1_1L 					0x2d
#define DAC_L_BIQUAD_B1_1H 					0x2e
#define DAC_L_BIQUAD_B2_1L 					0x2f
#define DAC_L_BIQUAD_B2_1H 					0x30
#define DAC_L_BIQUAD_A1_1L 					0x31
#define DAC_L_BIQUAD_A1_1H 					0x32
#define DAC_L_BIQUAD_A2_1L 					0x33
#define DAC_L_BIQUAD_A2_1H 					0x34
#define DAC_L_BIQUAD_H0_2L 					0x35
#define DAC_L_BIQUAD_H0_2H 					0x36
#define DAC_L_BIQUAD_B1_2L 					0x37
#define DAC_L_BIQUAD_B1_2H 					0x38
#define DAC_L_BIQUAD_B2_2L 					0x39
#define DAC_L_BIQUAD_B2_2H 					0x3a
#define DAC_L_BIQUAD_A1_2L 					0x3b
#define DAC_L_BIQUAD_A1_2H 					0x3c
#define DAC_L_BIQUAD_A2_2L 					0x3d
#define DAC_L_BIQUAD_A2_2H 					0x3e
#define DAC_L_BIQUAD_H0_3L 					0x3f
#define DAC_L_BIQUAD_H0_3H 					0x40
#define DAC_L_BIQUAD_B1_3L 					0x41
#define DAC_L_BIQUAD_B1_3H 					0x42
#define DAC_L_BIQUAD_B2_3L 					0x43
#define DAC_L_BIQUAD_B2_3H 					0x44
#define DAC_L_BIQUAD_A1_3L 					0x45
#define DAC_L_BIQUAD_A1_3H 					0x46
#define DAC_L_BIQUAD_A2_3L 					0x47
#define DAC_L_BIQUAD_A2_3H 					0x48
#define DAC_L_BIQUAD_H0_4L 					0x49
#define DAC_L_BIQUAD_H0_4H 					0x4a
#define DAC_L_BIQUAD_B1_4L 					0x4b
#define DAC_L_BIQUAD_B1_4H					0x4c
#define DAC_L_BIQUAD_B2_4L 					0x4d
#define DAC_L_BIQUAD_B2_4H 					0x4e
#define DAC_L_BIQUAD_A1_4L 					0x4f
#define DAC_L_BIQUAD_A1_4H 					0x50
#define DAC_L_BIQUAD_A2_4L 					0x51
#define DAC_L_BIQUAD_A2_4H 					0x52
#define DAC_L_BIQUAD_H0_5L 					0x53
#define DAC_L_BIQUAD_H0_5H 					0x54
#define DAC_L_BIQUAD_B1_5L 					0x55
#define DAC_L_BIQUAD_B1_5H 					0x56
#define DAC_L_BIQUAD_B2_5L 					0x57
#define DAC_L_BIQUAD_B2_5H 					0x58
#define DAC_L_BIQUAD_A1_5L 					0x59 
#define DAC_L_BIQUAD_A1_5H 					0x5a
#define DAC_L_BIQUAD_A2_5L 					0x5b
#define DAC_L_BIQUAD_A2_5H 					0x5c

#define DAC_R_BQ_EQ_CONTROL 				0x5d
#define DAC_R_BIQUAD_H0_1L 					0x5e
#define DAC_R_BIQUAD_H0_1H 					0x5f
#define DAC_R_BIQUAD_B1_1L 					0x60
#define DAC_R_BIQUAD_B1_1H 					0x61
#define DAC_R_BIQUAD_B2_1L 					0x62
#define DAC_R_BIQUAD_B2_1H 					0x63
#define DAC_R_BIQUAD_A1_1L 					0x64
#define DAC_R_BIQUAD_A1_1H 					0x65
#define DAC_R_BIQUAD_A2_1L 					0x66
#define DAC_R_BIQUAD_A2_1H 					0x67
#define DAC_R_BIQUAD_H0_2L 					0x68
#define DAC_R_BIQUAD_H0_2H 					0x69
#define DAC_R_BIQUAD_B1_2L 					0x6a
#define DAC_R_BIQUAD_B1_2H 					0x6b
#define DAC_R_BIQUAD_B2_2L 					0x6c
#define DAC_R_BIQUAD_B2_2H 					0x6d
#define DAC_R_BIQUAD_A1_2L 					0x6e
#define DAC_R_BIQUAD_A1_2H 					0x6f
#define DAC_R_BIQUAD_A2_2L 					0x70
#define DAC_R_BIQUAD_A2_2H 					0x71
#define DAC_R_BIQUAD_H0_3L 					0x72
#define DAC_R_BIQUAD_H0_3H 					0x73
#define DAC_R_BIQUAD_B1_3L 					0x74
#define DAC_R_BIQUAD_B1_3H 					0x75
#define DAC_R_BIQUAD_B2_3L 					0x76
#define DAC_R_BIQUAD_B2_3H 					0x77
#define DAC_R_BIQUAD_A1_3L 					0x78
#define DAC_R_BIQUAD_A1_3H 					0x79
#define DAC_R_BIQUAD_A2_3L 					0x7a
#define DAC_R_BIQUAD_A2_3H 					0x7b
#define DAC_R_BIQUAD_H0_4L 					0x7c
#define DAC_R_BIQUAD_H0_4H 					0x7d
#define DAC_R_BIQUAD_B1_4L 					0x7e
#define DAC_R_BIQUAD_B1_4H 					0x7f
#define DAC_R_BIQUAD_B2_4L 					0x80
#define DAC_R_BIQUAD_B2_4H 					0x81
#define DAC_R_BIQUAD_A1_4L 					0x82
#define DAC_R_BIQUAD_A1_4H 					0x83
#define DAC_R_BIQUAD_A2_4L 					0x84
#define DAC_R_BIQUAD_A2_4H 					0x85
#define DAC_R_BIQUAD_H0_5L 					0x86
#define DAC_R_BIQUAD_H0_5H 					0x87
#define DAC_R_BIQUAD_B1_5L 					0x88
#define DAC_R_BIQUAD_B1_5H 					0x89
#define DAC_R_BIQUAD_B2_5L 					0x8a
#define DAC_R_BIQUAD_B2_5H 					0x8b
#define DAC_R_BIQUAD_A1_5L 					0x8c 
#define DAC_R_BIQUAD_A1_5H 					0x8d
#define DAC_R_BIQUAD_A2_5L 					0x8e
#define DAC_R_BIQUAD_A2_5H 					0x8f

#define ADC_L_BQ_EQ_CONTROL 				0x90
#define ADC_L_BIQUAD_H0_1L 					0x91
#define ADC_L_BIQUAD_H0_1H 					0x92
#define ADC_L_BIQUAD_B1_1L 					0x93
#define ADC_L_BIQUAD_B1_1H 					0x94
#define ADC_L_BIQUAD_B2_1L 					0x95
#define ADC_L_BIQUAD_B2_1H 					0x96
#define ADC_L_BIQUAD_A1_1L 					0x97
#define ADC_L_BIQUAD_A1_1H 					0x98
#define ADC_L_BIQUAD_A2_1L 					0x99
#define ADC_L_BIQUAD_A2_1H 					0x9a
#define ADC_L_BIQUAD_H0_2L 					0x9b
#define ADC_L_BIQUAD_H0_2H 					0x9c
#define ADC_L_BIQUAD_B1_2L 					0x9d
#define ADC_L_BIQUAD_B1_2H 					0x9e
#define ADC_L_BIQUAD_B2_2L 					0x9f
#define ADC_L_BIQUAD_B2_2H 					0xa0
#define ADC_L_BIQUAD_A1_2L 					0xa1
#define ADC_L_BIQUAD_A1_2H 					0xa2
#define ADC_L_BIQUAD_A2_2L 					0xa3
#define ADC_L_BIQUAD_A2_2H 					0xa4
#define ADC_L_BIQUAD_H0_3L 					0xa5
#define ADC_L_BIQUAD_H0_3H 					0xa6
#define ADC_L_BIQUAD_B1_3L 					0xa7
#define ADC_L_BIQUAD_B1_3H 					0xa8
#define ADC_L_BIQUAD_B2_3L 					0xa9
#define ADC_L_BIQUAD_B2_3H 					0xaa
#define ADC_L_BIQUAD_A1_3L 					0xab
#define ADC_L_BIQUAD_A1_3H 					0xac
#define ADC_L_BIQUAD_A2_3L 					0xad
#define ADC_L_BIQUAD_A2_3H 					0xae
#define ADC_L_BIQUAD_H0_4L 					0xaf
#define ADC_L_BIQUAD_H0_4H 					0xb0
#define ADC_L_BIQUAD_B1_4L 					0xb1
#define ADC_L_BIQUAD_B1_4H 					0xb2
#define ADC_L_BIQUAD_B2_4L 					0xb3
#define ADC_L_BIQUAD_B2_4H 					0xb4
#define ADC_L_BIQUAD_A1_4L 					0xb5
#define ADC_L_BIQUAD_A1_4H 					0xb6
#define ADC_L_BIQUAD_A2_4L 					0xb7
#define ADC_L_BIQUAD_A2_4H 					0xb8
#define ADC_L_BIQUAD_H0_5L 					0xb9
#define ADC_L_BIQUAD_H0_5H 					0xba
#define ADC_L_BIQUAD_B1_5L 					0xbb
#define ADC_L_BIQUAD_B1_5H 					0xbc
#define ADC_L_BIQUAD_B2_5L 					0xbd
#define ADC_L_BIQUAD_B2_5H 					0xbe
#define ADC_L_BIQUAD_A1_5L 					0xbf 
#define ADC_L_BIQUAD_A1_5H 					0xc0
#define ADC_L_BIQUAD_A2_5L 					0xc1
#define ADC_L_BIQUAD_A2_5H 					0xc2

#define ADC_R_BQ_EQ_CONTROL 				0xc3
#define ADC_R_BIQUAD_H0_1L 					0xc4
#define ADC_R_BIQUAD_H0_1H 					0xc5
#define ADC_R_BIQUAD_B1_1L 					0xc6
#define ADC_R_BIQUAD_B1_1H 					0xc7
#define ADC_R_BIQUAD_B2_1L 					0xc8
#define ADC_R_BIQUAD_B2_1H 					0xc9
#define ADC_R_BIQUAD_A1_1L 					0xca
#define ADC_R_BIQUAD_A1_1H 					0xcb
#define ADC_R_BIQUAD_A2_1L 					0xcc
#define ADC_R_BIQUAD_A2_1H 					0xcd
#define ADC_R_BIQUAD_H0_2L 					0xce
#define ADC_R_BIQUAD_H0_2H 					0xcf
#define ADC_R_BIQUAD_B1_2L 					0xd0
#define ADC_R_BIQUAD_B1_2H 					0xd1
#define ADC_R_BIQUAD_B2_2L 					0xd2
#define ADC_R_BIQUAD_B2_2H 					0xd3
#define ADC_R_BIQUAD_A1_2L 					0xd4
#define ADC_R_BIQUAD_A1_2H 					0xd5
#define ADC_R_BIQUAD_A2_2L 					0xd6
#define ADC_R_BIQUAD_A2_2H 					0xd7
#define ADC_R_BIQUAD_H0_3L 					0xd8
#define ADC_R_BIQUAD_H0_3H 					0xd9
#define ADC_R_BIQUAD_B1_3L 					0xda
#define ADC_R_BIQUAD_B1_3H 					0xdb
#define ADC_R_BIQUAD_B2_3L 					0xdc
#define ADC_R_BIQUAD_B2_3H 					0xdd
#define ADC_R_BIQUAD_A1_3L 					0xde
#define ADC_R_BIQUAD_A1_3H 					0xdf
#define ADC_R_BIQUAD_A2_3L 					0xe0
#define ADC_R_BIQUAD_A2_3H 					0xe1
#define ADC_R_BIQUAD_H0_4L 					0xe2
#define ADC_R_BIQUAD_H0_4H 					0xe3
#define ADC_R_BIQUAD_B1_4L 					0xe4
#define ADC_R_BIQUAD_B1_4H 					0xe5
#define ADC_R_BIQUAD_B2_4L 					0xe6
#define ADC_R_BIQUAD_B2_4H 					0xe7
#define ADC_R_BIQUAD_A1_4L 					0xe8
#define ADC_R_BIQUAD_A1_4H 					0xe9
#define ADC_R_BIQUAD_A2_4L 					0xea
#define ADC_R_BIQUAD_A2_4H 					0xeb
#define ADC_R_BIQUAD_H0_5L 					0xec
#define ADC_R_BIQUAD_H0_5H 					0xed
#define ADC_R_BIQUAD_B1_5L 					0xee
#define ADC_R_BIQUAD_B1_5H 					0xef
#define ADC_R_BIQUAD_B2_5L 					0xf0
#define ADC_R_BIQUAD_B2_5H 					0xf1
#define ADC_R_BIQUAD_A1_5L 					0xf2 
#define ADC_R_BIQUAD_A1_5H 					0xf3
#define ADC_R_BIQUAD_A2_5L 					0xf4
#define ADC_R_BIQUAD_A2_5H 					0xf5

//0x00
#define BIT_DAC_ADDACK_POW 					0
#define BIT_DAC_CKXEN 						1
#define BIT_DAC_CKXSEL 						2
#define BIT_DAC_L_POW 						3
#define BIT_DAC_R_POW 						4 
#define BIT_DPRAMP_CSEL 					5
#define BIT_DPRAMP_ENRAMP 					7
#define BIT_DPRAMP_POW 						8
#define BIT_DTSDM_CKXEN 					9
#define BIT_DTSDM_POW_L 					10 
#define BIT_DTSDM_POW_R 					11
#define BIT_HPO_CLL 						12
#define BIT_HPO_CLNDPL 						13
#define BIT_HPO_CLNDPR 						14
#define BIT_HPO_CLPDPL 						15

//0x01
#define BIT_HPO_CLPDPR 						0
#define BIT_HPO_CLR 						1
#define BIT_HPO_DPRSELL 					2
#define BIT_HPO_DPRSELR 					4
#define BIT_HPO_ENAL 						6
#define BIT_HPO_ENAR 						7 
#define BIT_HPO_ENDPL 						8
#define BIT_HPO_ENDPR 						9
#define BIT_HPO_L_POW 						10
#define BIT_HPO_MDPL 						11
#define BIT_HPO_MDPR 						12
#define BIT_HPO_OPNDPL 						13
#define BIT_HPO_OPNDPR 						14 
#define BIT_HPO_OPPDPL 						15


//0x02
#define BIT_HPO_ML 							0
#define BIT_HPO_MR 							2
#define BIT_HPO_OPPDPR 						4
#define BIT_HPO_R_POW 						5
#define BIT_HPO_SEL 						6
#define BIT_HPO_SER 						7
#define BIT_MBIAS_POW 						8
#define BIT_MICBIAS_ENCHX 					9
#define BIT_MICBIAS_POW 					10
#define BIT_MICBIAS_VSET 					11
#define BIT_MICBST_ENDFL 					13
#define BIT_MICBST_ENDFR 					14
#define BIT_VREF_POW 						15


//0x03
#define BIT_MICBST_GSELL 					0
#define BIT_MICBST_GSELR 					2
#define BIT_MICBST_MUTE_L 					4
#define BIT_MICBST_MUTE_R 					6 
#define BIT_MICBST_POW 						8
#define BIT_VREF_VREFSEL 					10

//0x0c
#define BIT_SEL_BB_CK_DEPOP 				0
#define BIT_MICBIAS_OC 						2
#define BIT_BB_CK_DEPOP_EN 					3
#define BIT_CKX_MICBIAS_EN 					4

//0x10
#define BIT_EN_I2S_MONO 					0
#define BIT_I2S_EN_PCM_N_MODE 				1
#define BIT_I2S_DATA_FORMAT_SEL 			2
#define BIT_I2S_DATA_LEN_SEL 				4
#define BIT_INV_I2S_SCLK 					6
#define BIT_I2S_RST_N_REG 					7
#define BIT_SEL_I2S_RX_CH 					8
#define BIT_SEL_I2S_TX_CH 					10
#define BIT_STEREO_I2S_SELF_LPBK_EN 		12

//0x11
#define BIT_ADC_L_DMIC_RI_FA_SEL 			0
#define BIT_ADC_L_DMIC_LPF2ND_EN 			3
#define BIT_ADC_L_DMIC_LPF1ST_EN 			4
#define BIT_ADC_L_DMIC_LPF1ST_FC_SEL 		5
#define BIT_ADC_L_AD_LPF2ND_EN 				8
#define BIT_ADC_L_AD_LPF1ST_EN 				9
#define BIT_ADC_L_AD_LPF1ST_FC_SEL 			10
#define BIT_ADC_L_AD_MIX_MUTE 				12 
#define BIT_ADC_L_DMIC_MIX_MUTE 			13
#define BIT_ADC_L_AD_DCHPF_EN  				14


//0x12
#define BIT_ADC_L_AD_COMP_GAIN 				0
#define BIT_ADC_L_ADJ_HPF_2ND_EN 			2
#define BIT_ADC_L_ADJ_HPF_COEF_SEL 			3
#define BIT_ADC_L_DMIC_BOOST_GAIN 			6
#define BIT_ADC_L_AD_MUTE 					8
#define BIT_ADC_L_AD_ZDET_FUNC 				9
#define BIT_ADC_L_AD_ZDET_TOUT 				11

//0x13
#define ADC_L_ADJ_HPF_COEF_NUM 				0
#define ADC_L_AD_GAIN 						6



//0x14 
#define BIT_ADC_R_DMIC_RI_FA_SEL 			0
#define BIT_ADC_R_DMIC_LPF2ND_EN 			3
#define BIT_ADC_R_DMIC_LPF1ST_EN 			4
#define BIT_ADC_R_DMIC_LPF1ST_FC_SEL 		5
#define BIT_ADC_R_AD_LPF2ND_EN 				8
#define BIT_ADC_R_AD_LPF1ST_EN 				9
#define BIT_ADC_R_AD_LPF1ST_FC_SEL 			10
#define BIT_ADC_R_AD_MIX_MUTE 				12
#define BIT_ADC_R_DMIC_MIX_MUTE 			13
#define BIT_ADC_R_AD_DCHPF_EN 				14

//0x15
#define BIT_ADC_R_AD_COMP_GAIN 				0
#define BIT_ADC_R_ADJ_HPF_2ND_EN 			2
#define BIT_ADC_R_ADJ_HPF_COEF_SEL 			3
#define BIT_ADC_R_DMIC_BOOST_GAIN 			6
#define BIT_ADC_R_AD_MUTE 					8
#define BIT_ADC_R_AD_ZDET_FUNC 				9
#define BIT_ADC_R_AD_ZDET_TOUT 				11

//0x16
#define ADC_R_ADJ_HPF_COEF_NUM 				0
#define ADC_R_AD_GAIN 						6

//0x17
#define BIT_DAC_SAMPLE_RATE 				0
#define BIT_ADC_SAMPLE_RATE 				4
#define BIT_DMIC_CLK_SEL 					8 
#define BIT_ASRC_FSI_RATE_MANUAL 			11
#define BIT_ASRC_FSI_GATING_EN 				15

//0x18
#define BIT_DA_L_EN 						0
#define BIT_DA_R_EN 						1
#define BIT_MOD_L_EN 						2
#define BIT_MOD_R_EN 						3
#define BIT_DA_ANA_CLK_EN 					4
#define BIT_DA_FIFO_EN 						5
#define BIT_ST_EN 							6
#define BIT_AD_L_EN 						7
#define BIT_AD_R_EN 						8
#define BIT_AD_FIFO_EN 						9
#define BIT_AD_ANA_CLK_EN 					10
#define BIT_AD_ANA_L_EN 					11
#define BIT_AD_ANA_R_EN 					12
#define BIT_DMIC_L_EN 						13
#define BIT_DMIC_R_EN 						14
#define BIT_DMIC_CLK_EN 					15

//0x1b
#define BIT_AUDIO_IP_TCON_EN 				0
#define BIT_ASRC_FTK_LOOP_EN 				1
#define BIT_ASRC_256FS_SYS_SEL 				2
#define BIT_ASRC_EN 						4
#define BIT_SIDETONE_IN_SEL 				5

//0x27
#define BIT_ALC_MIN_GAIN  					0
#define BIT_DA_STEREO_MODE_EN 				8

//0xfa
#define BIT_DAC_L_DA_GAIN 					0
#define BIT_DAC_L_DAHPF_EN 					8
#define BIT_DAC_L_DA_DITHER_SEL 			9
#define BIT_DAC_L_DA_ZDET_FUNC 				11
#define BIT_DAC_L_DA_ZDET_TOUT 				13
#define BIT_DAC_L_DMIX_IN_SEL 				15

//0xfb 
#define BIT_DAC_L_DA_MUTE 					0
#define BIT_DAAD_LPBK_EN 					1
#define BIT_DAC_L_DMIX_MUTE_128FS_DA 		2
#define BIT_DAC_L_DMIX_MUTE_128FS_SIDETONE 	3

//0xfd
#define BIT_DAC_R_DA_MUTE 					0
#define BIT_DAC_R_DMIX_MUTE_128FS_DA 		2
#define BIT_DAC_R_DMIX_MUTE_128FS_SIDETONE 	3


/** @defgroup AUDIO_Exported_Functions AUDIO Exported Functions
  * @{
  */
/** @defgroup AUDIO_SI_functions 
  * @{
  */
void AUDIO_SI_Cmd(u8  new_state);
void AUDIO_SI_WriteReg(u32 address, u32 data);
u16 AUDIO_SI_ReadReg(u32 address);
void AUDIO_SI_ClkCmd(u8  new_state);
/**
  * @}
  */

/** @defgroup AUDIO_codec_functions
  * @{
  */
void CODEC_Init(u32 sample_rate, u32 word_len, u32 mono_stereo, u32 application);
void CODEC_SetVolume(u8 vol_lch, u8 vol_rch);
void CODEC_GetVolume(u16 *vol);
void CODEC_SetSr(u32 sample_rate);
void CODEC_SetAdcGain(u32 ad_gain_left, u32 ad_gain_right);
void CODEC_SetAmicBst(u32 amic_bst_left, u32 amic_bst_right);
void CODEC_SetDmicBst(u32 dmic_bst_left, u32 dmic_bst_right);
void CODEC_SetMicBias(u8 mic_bias);
void CODEC_MuteRecord(u32 mute_lch, u32 mute_rch);
void CODEC_MutePlay(u32 mute_lch, u32 mute_rch);
void CODEC_DeInit(u32 application);
void CODEC_DacEqConfig(u32 sample_rate);
void CODEC_SetALC(u32 limiter_val);
void CODEDC_ALC_deinit();
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
#endif

/******************* (C) COPYRIGHT 2018 Realtek Semiconductor *****END OF FILE****/
