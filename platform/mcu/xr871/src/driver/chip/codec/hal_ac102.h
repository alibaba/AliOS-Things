/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *	 notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *	 notice, this list of conditions and the following disclaimer in the
 *	 documentation and/or other materials provided with the
 *	 distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *	 its contributors may be used to endorse or promote products derived
 *	 from this software without specific prior written permission.
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

#ifndef __HAL_AC102_H_
#define __HAL_AC102_H_

#ifdef __cplusplus
 extern "C" {
#endif

/*
 * Register Information.
 */
#define CHIP_SOFT_RST				0x0
#define PWR_CTRL1                   0x1
#define PWR_CTRL2                   0x2
#define SYS_FUNC_CTRL               0x3
#define ADC_CLK_SET                 0x4
#define DAC_CLK_SET					0x5
#define SYS_CLK_ENA                 0x6

#define I2S_CTRL                 	0x7
#define I2S_BCLK_CTRL				0x8
#define I2S_LRCK_CTRL1				0x9
#define I2S_LRCK_CTRL2				0x0A
#define I2S_FMT_CTRL1				0x0B
#define I2S_FMT_CTRL2				0x0C
#define I2S_FMT_CTRL3				0x0D
#define I2S_SLOT_CTRL				0x0E
#define I2S_TX_CTRL                 0x0F
#define I2S_TX_CHMP_CTRL            0x11
#define I2S_TX_MIX_SRC				0x13
#define I2S_RX_CHMP_CTRL			0x16
#define I2S_RX_MIX_SRC				0x18

#define ADC_DIG_CTRL				0x19
#define ADC_DVC						0x1A

#define DAC_DIG_CTRL                0x1B
#define DAC_DVC						0x1C
#define DAC_MIX_SRC					0x1D
#define DIG_PADDRV_CTRL				0x1F
#define ADC_ANA_CTRL1				0x20
#define ADC_ANA_CTRL2				0x21
#define ADC_ANA_CTRL3				0x22
#define ADC_ANA_CTRL4				0x23
#define ADC_ANA_CTRL5				0x24
#define DAC_ANA_CTRL1				0x25

#define DAC_ANA_CTRL2				0x26
#define DAC_ANA_CTRL3				0x27
#define DAC_ANA_CTRL4				0x28

#define AGC_STA            			0x30
#define AGC_CTRL                   	0x31
#define AGC_DEBT            		0x32
#define AGC_TGLVL                  	0x33
#define AGC_MAXG                	0x34
#define AGC_AVGC1                	0x35
#define AGC_AVGC2                   0x36
#define AGC_AVGC3                  	0x37
#define AGC_AVGC4                  	0x38
#define AGC_DECAYT1                 0x39
#define AGC_DECAYT2                 0x3A
#define AGC_ATTACKT1                0x3B
#define AGC_ATTACKT2                0x3C
#define AGC_NTH                   	0x3D
#define AGC_NAVGC1                  0x3E
#define AGC_NAVGC2                  0x3F
#define AGC_NAVGC3                  0x40
#define AGC_NAVGC4                  0x41

#define HPF_COEF1                	0x42
#define HPF_COEF2                 	0x43
#define HPF_COEF3                   0x44
#define HPF_COEF4                  	0x45

#define AGC_OPT                  	0x46

#define EQ_CTRL                  	0x4F
#define EQ1_B0_H                   	0x50
#define EQ1_B0_M					0x51
#define EQ1_B0_L                  	0x52
#define EQ1_B1_H                   	0x53
#define EQ1_B1_M					0x54
#define EQ1_B1_L                  	0x55
#define EQ1_B2_H                   	0x56
#define EQ1_B2_M					0x57
#define EQ1_B2_L                  	0x58
#define EQ1_A1_H                   	0x59
#define EQ1_A1_M					0x5A
#define EQ1_A1_L                  	0x5B
#define EQ1_A2_H                   	0x5C
#define EQ1_A2_M					0x5D
#define EQ1_A2_L					0x5E

#define EQ2_B0_H                  	0x60
#define EQ2_B0_M                   	0x61
#define EQ2_B0_L					0x62
#define EQ2_B1_H                  	0x63
#define EQ2_B1_M                   	0x64
#define EQ2_B1_L					0x65
#define EQ2_B2_H                 	0x66
#define EQ2_B2_M                   	0x67
#define EQ2_B2_L					0x68
#define EQ2_A1_H                  	0x69
#define EQ2_A1_M                   	0x6A
#define EQ2_A1_L					0x6B
#define EQ2_A2_H                  	0x6C
#define EQ2_A2_M                   	0x6D
#define EQ2_A2_L					0x6E

#define EQ3_B0_H                  	0x70
#define EQ3_B0_M                   	0x71
#define EQ3_B0_L					0x72
#define EQ3_B1_H                  	0x73
#define BQ3_B1_M                   	0x74
#define EQ3_B1_L					0x75
#define EQ3_B2_H                  	0x76
#define EQ3_B2_M                   	0x77
#define EQ3_B2_L					0x78
#define EQ3_A1_H                  	0x79
#define EQ3_A1_M                   	0x7A
#define EQ3_A1_L					0x7B
#define EQ3_A2_H                  	0x7C
#define EQ3_A2_M                   	0x7D
#define EQ3_A2_L					0x7E

/* PWR_CTRL1 */
#define ALDO_VCTRL                  5
#define DLDO_VCTRL                  2
#define MBIAS_VCTRL                 0

/* PWR_CTRL2 */
#define IREF_CTRL                  	5
#define ALDO_EN                  	4
#define DLDO_EN                 	3
#define MBIAS_EN                  	2
#define VREF_EN                  	1
#define IREF_EN                 	0

/* SYS_FUNC_CTRL */
#define VREF_SPUP_STA               6
#define DAC_PLAY_FUNC_EN            5
#define ADC_REC_FUNC_EN             4
#define AGC_GEN                 	2
#define MCLK_AUTO_DET_EN            1
#define DAC_ANA_OUT_EN            	0

/* ADC_CLK_SET */
#define NADC                 		0 //[3:0]

/* DAC_CLK_SET */
#define NDAC                 		0 //[3:0]

/* SYS_CLK_ENA */
#define SYSCLK_EN					5
#define I2S_CLK_EN					4
#define EQ_CLK_EN					3
#define DAC_Digital_CLK_EN			2
#define AGC_HPF_CLK_EN				1
#define ADC_Digital_CLK_EN			0

/* I2S_CTRL */
#define BCLK_IOEN					7
#define LRCK_IOEN					6
#define SDO_EN						4
#define TXEN						2
#define RXEN						1
#define I2SGEN						0

/* I2S_BCLK_CTRL */
#define EDGE_TRANSFER				5
#define BCLK_POLARITY				4
#define BCLKDIV						0

/* I2S_LRCK_CTRL1 */
#define LRCK_POLARITY				4
#define LRCK_PERIODH				0

/* I2S_LRCK_CTRL2 */
#define LRCK_PERIODL				0

/* I2S_FMT_CTRL1 */
#define ENCD_SEL					6
#define MODE_SEL					4
#define OFFSET						2
#define TX_SLOT_HIZ					1
#define TX_STATE					0

/* I2S_FMT_CTRL2 */
#define SW							4
#define SR							0

/* I2S_FMT_CTRL3 */
#define TX_MLS						7
#define SEXT						5
#define OUT_MUTE					3
#define LRCK_WIDTH					2
#define TX_PDM						0

/* I2S_SLOT_CTRL */
#define RX_CHSEL					2
#define TX_CHSEL					0

/* I2S_TX_CTRL */
#define TX_CHEN						0

/* I2S_TX_CHMP_CTRL */
#define TX_CH4_MAP					3
#define TX_CH3_MAP					2
#define TX_CH2_MAP					1
#define TX_CH1_MAP					0

/* I2S_TX_MIX_SRC */
#define TX_MIXR_GAIN                6
#define TX_MIXL_GAIN                4
#define TX_MIXR_SRC                 2
#define TX_MIXL_SRC                 0

/* I2S_RX_CHMP_CTRL */
#define RX_CH4_MAP					6
#define RX_CH3_MAP					4
#define RX_CH2_MAP					2
#define RX_CH1_MAP					0

/* I2S_RX_MIX_SRC */
#define RX_MIX_GAIN                 2
#define RX_MIX_SRC                	0

/* ADC_DIG_CTRL */
#define DIG_MIC_EN                  7
#define DIG_MIC_OSR                 6
#define ADC_PTN_SEL                 4
#define ADOUT_DTS                   2
#define ADOUT_DLY_EN                1
#define ADC_DIG_EN                 	0

/* ADC_DVC */
#define ADC_DVC_VOL					0

/* DAC_DIG_CTRL */
#define DVCZCDEN                    6
#define DITHER_SGM                  3
#define DAC_PTN_SEL                 1
#define DAC_DIG_EN                  0

/* DAC_DVC */
#define DAC_DVC_VOL					0

/* DAC_MIX_SRC */
#define DAC_MIX_GAIN                2
#define DAC_MIX_SRC_                 0

/* DIG_PADDRV_CTRL */
#define PAD_FOR_SDOUT               4
#define PAD_FOR_LRCK                2
#define PAD_FOR_BCLK               	0

/* ADC_ANA_CTRL1 */
#define PGA_GAIN_CTRL               3
#define PGA_CTRL_RCM                1
#define ADC_GEN                    	0

/* ADC_ANA_CTRL2 */
#define DSM_DISABLE               	7
#define SEL_OUT_EDGE                6
#define DSM_VREFP_LPMODE            5
#define DSM_VREFP_OUTCTRL           3
#define DITHER_LEVEL_CTRL           0

/* ADC_ANA_CTRL3 */
#define DSM_DITHER_EN               7
#define DSM_DEMOFF                	6
#define PGA_OI_NM_CTRL            	3
#define PGA_OI_M_CTRL           	0

/* ADC_ANA_CTRL4 */
#define PGA_IN_VCM_CTRL             6
#define PGA_NMAMP_IB_SEL            3
#define PGA_MAMP_IB_SEL           	0

/* ADC_ANA_CTRL5 */
#define DSM_OTA_IB_SEL              5
#define DSM_COMP_IB_SEL             2
#define SM_OTA_CTRL           		0

/* DAC_ANA_CTRL1 */
#define DACEN                      	7
#define VRDA_EN                     5
#define RAMP_EN                     4
#define RSWITCH                     3
#define LOMUTE                      2

/* DAC_ANA_CTRL2 */
#define LINEOUTEN                   5
#define LINEODIFEN                  4
#define LINEOAMPGAIN                0

/* DAC_ANA_CTRL3 */
#define DACOPBC                   	5
#define LINEOPBC                  	2
#define BCEN                		1

/* DAC_ANA_CTRL4 */
#define RAMP_TIME                   4
#define ATTSTEP                  	2
#define LOUTAUTOATT                	1
#define	LOUTVOLZCDEN				0

/* AGC_CTRL */
#define AGC_SAT_FLAG                6
#define AGC_NTHR_FLAG               5
#define AGC_EN			            4
#define	HPF_EN						3
#define NOISE_DETECT_EN		        2
#define	AGC_HYS_SET					0

/* EQ_CTRL */
#define EQ_EN						0

#ifdef __cplusplus
}
#endif

#endif /*_DRIVER_CHIP_HAL_AC101_H_*/
