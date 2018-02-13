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

#include <string.h>
#include "../board_debug.h"
#include "board_config.h"
#include "driver/chip/hal_codec.h"

#define BOARD_SWD_EN	1

static const GPIO_PinMuxParam g_pinmux_uart0[] = {
	{ GPIO_PORT_B, GPIO_PIN_0,  { GPIOB_P0_F2_UART0_TX,   GPIO_DRIVING_LEVEL_1, GPIO_PULL_UP } }, /* TX */
	{ GPIO_PORT_B, GPIO_PIN_1,  { GPIOB_P1_F2_UART0_RX,   GPIO_DRIVING_LEVEL_1, GPIO_PULL_UP } }, /* RX */
};

static const GPIO_PinMuxParam g_pinmux_uart1[] = {
	{ GPIO_PORT_A, GPIO_PIN_17, { GPIOA_P17_F5_UART1_TX,  GPIO_DRIVING_LEVEL_1, GPIO_PULL_UP } }, /* TX */
	{ GPIO_PORT_A, GPIO_PIN_18, { GPIOA_P18_F5_UART1_RX,  GPIO_DRIVING_LEVEL_1, GPIO_PULL_UP } }, /* RX */
};

static const GPIO_PinMuxParam g_pinmux_dmic[] = {
	{ GPIO_PORT_A, GPIO_PIN_21, { GPIOA_P21_F3_DMIC_CLK,  GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_22, { GPIOA_P22_F3_DMIC_DATA, GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
};

static const GPIO_PinMuxParam g_pinmux_i2s[] = {
	{ GPIO_PORT_A, GPIO_PIN_12, { GPIOA_P12_F4_I2S_MCLK,  GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_13, { GPIOA_P13_F4_I2S_BCLK,  GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_14, { GPIOA_P14_F4_I2S_DI,    GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_15, { GPIOA_P15_F4_I2S_DO,    GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_16, { GPIOA_P16_F4_I2S_LRCLK, GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
};

static const GPIO_PinMuxParam g_pinmux_irrx[] = {
	{ GPIO_PORT_A, GPIO_PIN_17, { GPIOA_P17_F3_IR_RX,     GPIO_DRIVING_LEVEL_1, GPIO_PULL_UP } },
};

static const GPIO_PinMuxParam g_pinmux_irtx[] = {
	{ GPIO_PORT_A, GPIO_PIN_18, { GPIOA_P18_F3_IR_TX,     GPIO_DRIVING_LEVEL_1, GPIO_PULL_DOWN } },
};

static const GPIO_PinMuxParam g_pinmux_i2c0[] = {
	{ GPIO_PORT_A, GPIO_PIN_4,  { GPIOA_P4_F4_I2C0_SCL,   GPIO_DRIVING_LEVEL_1, GPIO_PULL_UP } },
	{ GPIO_PORT_A, GPIO_PIN_5,  { GPIOA_P5_F4_I2C0_SDA,   GPIO_DRIVING_LEVEL_1, GPIO_PULL_UP } },
};

static const GPIO_PinMuxParam g_pinmux_i2c1[] = {
	{ GPIO_PORT_A, GPIO_PIN_17, { GPIOA_P17_F4_I2C1_SCL,  GPIO_DRIVING_LEVEL_1, GPIO_PULL_UP } },
	{ GPIO_PORT_A, GPIO_PIN_18, { GPIOA_P18_F4_I2C1_SDA,  GPIO_DRIVING_LEVEL_1, GPIO_PULL_UP } },
};

static const GPIO_PinMuxParam g_pinmux_adc[] = {
	{ GPIO_PORT_A, GPIO_PIN_8,  { GPIOA_P8_F2_ADC_CH0,  GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_9,  { GPIOA_P9_F2_ADC_CH1,  GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_10, { GPIOA_P10_F2_ADC_CH2, GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_11, { GPIOA_P11_F2_ADC_CH3, GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_12, { GPIOA_P12_F2_ADC_CH4, GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_13, { GPIOA_P13_F2_ADC_CH5, GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_14, { GPIOA_P14_F2_ADC_CH6, GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_15, { GPIOA_P15_F2_ADC_CH7, GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
};

static const GPIO_PinMuxParam g_pinmux_spi0[] = {
	{ GPIO_PORT_B, GPIO_PIN_4,  { GPIOB_P4_F2_SPI0_MOSI,  GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_B, GPIO_PIN_5,  { GPIOB_P5_F2_SPI0_MISO,  GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_B, GPIO_PIN_7,  { GPIOB_P7_F2_SPI0_CLK,   GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
};

static const GPIO_PinMuxParam g_pinmux_spi0_cs0[] = {
	{ GPIO_PORT_B, GPIO_PIN_6,  { GPIOB_P6_F2_SPI0_CS0,   GPIO_DRIVING_LEVEL_1, GPIO_PULL_UP } },
};

static const GPIO_PinMuxParam g_pinmux_spi1[] = {
	{ GPIO_PORT_A, GPIO_PIN_0,  { GPIOA_P0_F2_SPI1_MOSI,  GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_1,  { GPIOA_P1_F2_SPI1_MISO,  GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_2,  { GPIOA_P2_F2_SPI1_CLK,   GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
};

static const GPIO_PinMuxParam g_pinmux_spi1_cs0[] = {
	{ GPIO_PORT_A, GPIO_PIN_3,  { GPIOA_P3_F2_SPI1_CS0,   GPIO_DRIVING_LEVEL_1, GPIO_PULL_UP } },
};

static const GPIO_PinMuxParam g_pinmux_spi1_cs1[] = {
	{ GPIO_PORT_A, GPIO_PIN_6,  { GPIOA_P6_F3_SPI1_CS1,   GPIO_DRIVING_LEVEL_1, GPIO_PULL_UP } },
};

static const GPIO_PinMuxParam g_pinmux_spi1_cs2[] = {
	{ GPIO_PORT_A, GPIO_PIN_7,  { GPIOA_P7_F3_SPI1_CS2,   GPIO_DRIVING_LEVEL_1, GPIO_PULL_UP } },
};

static const GPIO_PinMuxParam g_pinmux_flashc[] = {
	{ GPIO_PORT_B, GPIO_PIN_4,  { GPIOB_P4_F5_FLASH_MOSI, GPIO_DRIVING_LEVEL_3, GPIO_PULL_NONE } },
	{ GPIO_PORT_B, GPIO_PIN_5,  { GPIOB_P5_F5_FLASH_MISO, GPIO_DRIVING_LEVEL_3, GPIO_PULL_NONE } },
	{ GPIO_PORT_B, GPIO_PIN_7,  { GPIOB_P7_F5_FLASH_CLK,  GPIO_DRIVING_LEVEL_3, GPIO_PULL_NONE } },
	{ GPIO_PORT_B, GPIO_PIN_6,  { GPIOB_P6_F5_FLASH_CS,   GPIO_DRIVING_LEVEL_3, GPIO_PULL_UP   } },
#if (!BOARD_SWD_EN)
	{ GPIO_PORT_B, GPIO_PIN_2,  { GPIOB_P2_F5_FLASH_WP,   GPIO_DRIVING_LEVEL_3, GPIO_PULL_UP   } },
	{ GPIO_PORT_B, GPIO_PIN_3,  { GPIOB_P3_F5_FLASH_HOLD, GPIO_DRIVING_LEVEL_3, GPIO_PULL_UP   } },
#endif
};

/* flash */
static const FlashBoardCfg g_flash_cfg[] = {
	{
		.type = FLASH_DRV_FLASHC,
		.mode = FLASH_READ_DUAL_O_MODE,
		.flashc.clk = (48 * 1000 * 1000),
	},
};

static const GPIO_PinMuxParam g_pinmux_pwm[] = {
	{ GPIO_PORT_A, GPIO_PIN_8,  { GPIOA_P8_F3_PWM0_ECT0,  GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_9,  { GPIOA_P9_F3_PWM1_ECT1,  GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_10, { GPIOA_P10_F3_PWM2_ECT2, GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_11, { GPIOA_P11_F3_PWM3_ECT3, GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_12, { GPIOA_P12_F3_PWM4_ECT4, GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_13, { GPIOA_P13_F3_PWM5_ECT5, GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_14, { GPIOA_P14_F3_PWM6_ECT6, GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_15, { GPIOA_P15_F3_PWM7_ECT7, GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
};

#define BOARD_SD0_DATA_BITS   	1
#define BOARD_SD0_DET_VALID   	0
#define BOARD_SD0_DET_PORT    	GPIO_PORT_A
#define BOARD_SD0_DET_PIN     	GPIO_PIN_3
#define BOARD_SD0_DET_PIN_MODE	GPIOA_P3_F6_EINTA3

static const GPIO_PinMuxParam g_pinmux_sd0[BOARD_SD0_DATA_BITS + 2] = {
	{ GPIO_PORT_A, GPIO_PIN_0,  { GPIOA_P0_F3_SD_CMD,     GPIO_DRIVING_LEVEL_2, GPIO_PULL_UP } },	/* CMD */
	{ GPIO_PORT_A, GPIO_PIN_2,  { GPIOA_P2_F3_SD_CLK,     GPIO_DRIVING_LEVEL_2, GPIO_PULL_UP } },	/* CLK */
	{ GPIO_PORT_A, GPIO_PIN_1,  { GPIOA_P1_F3_SD_DATA0,   GPIO_DRIVING_LEVEL_2, GPIO_PULL_UP } },	/* D0 */
//	{ GPIO_PORT_A, GPIO_PIN_3,  { GPIOA_P3_F3_SD_DATA1,   GPIO_DRIVING_LEVEL_2, GPIO_PULL_UP } },	/* D1 */
//	{ GPIO_PORT_A, GPIO_PIN_4,  { GPIOA_P4_F3_SD_DATA2,   GPIO_DRIVING_LEVEL_2, GPIO_PULL_UP } },	/* D2 */
//	{ GPIO_PORT_A, GPIO_PIN_5,  { GPIOA_P5_F3_SD_DATA3,   GPIO_DRIVING_LEVEL_2, GPIO_PULL_NONE } },	/* D3 */
};

static const GPIO_PinMuxParam g_pinmux_sd0_det[] = {
	{ BOARD_SD0_DET_PORT, BOARD_SD0_DET_PIN, { BOARD_SD0_DET_PIN_MODE,  GPIO_DRIVING_LEVEL_2, GPIO_PULL_NONE } },	/* DET */
};

static const HAL_SDCGPIOCfg g_sd0_cfg = {
	.data_bits       = BOARD_SD0_DATA_BITS,
	.has_detect_gpio = BOARD_SD0_DET_VALID,
	.detect_port     = BOARD_SD0_DET_PORT,
	.detect_pin      = BOARD_SD0_DET_PIN
};

#define BOARD_SPK_PORT    GPIO_PORT_A
#define BOARD_SPK_PIN     GPIO_PIN_3

static const GPIO_PinMuxParam g_pinmux_spk[] = {
	{ BOARD_SPK_PORT, BOARD_SPK_PIN, { GPIOx_Pn_F1_OUTPUT,   GPIO_DRIVING_LEVEL_1, GPIO_PULL_UP } },
};

static const SPK_Param g_spk_cfg = {
	.ctrl_port      = BOARD_SPK_PORT,
	.ctrl_pin       = BOARD_SPK_PIN,
	.ctrl_on_state  = GPIO_PIN_HIGH,
	.ctrl_off_state = GPIO_PIN_LOW
};

GPIO_PinMuxParam g_pinmux_csi[] = {
	{ GPIO_PORT_A, GPIO_PIN_0,  { GPIOA_P0_F5_CSI_D0,     GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_1,  { GPIOA_P1_F5_CSI_D1,     GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_2,  { GPIOA_P2_F5_CSI_D2,     GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_3,  { GPIOA_P3_F5_CSI_D3,     GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_4,  { GPIOA_P4_F4_CSI_D4,     GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_5,  { GPIOA_P5_F5_CSI_D5,     GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_6,  { GPIOA_P6_F5_CSI_D6,     GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_7,  { GPIOA_P7_F5_CSI_D7,     GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_8,  { GPIOA_P8_F5_CSI_PCLK,   GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_9,  { GPIOA_P9_F5_CSI_MCLK,   GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_10, { GPIOA_P10_F5_CSI_HSYNC, GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
	{ GPIO_PORT_A, GPIO_PIN_11, { GPIOA_P11_F5_CSI_VSYNC, GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE } },
};

struct board_pinmux_info {
	const GPIO_PinMuxParam *pinmux;
	uint32_t count;
};

#define BOARD_PINMUX_INFO_MAX	2

static HAL_Status board_get_pinmux_info(uint32_t major, uint32_t minor, uint32_t param,
                                        struct board_pinmux_info info[])
{
	HAL_Status ret = HAL_OK;

	switch (major) {
	case HAL_DEV_MAJOR_UART:
		if (minor == UART0_ID) {
			info[0].pinmux = g_pinmux_uart0;
			info[0].count = HAL_ARRAY_SIZE(g_pinmux_uart0);
		} else if (minor == UART1_ID) {
			info[0].pinmux = g_pinmux_uart1;
			info[0].count = HAL_ARRAY_SIZE(g_pinmux_uart1);
		} else {
			ret = HAL_INVALID;
		}
		break;
	case HAL_DEV_MAJOR_I2C:
		if (minor == I2C0_ID) {
			info[0].pinmux = g_pinmux_i2c0;
			info[0].count = HAL_ARRAY_SIZE(g_pinmux_i2c0);
		} else if (minor == I2C1_ID) {
			info[0].pinmux = g_pinmux_i2c1;
			info[0].count = HAL_ARRAY_SIZE(g_pinmux_i2c1);
		} else {
			ret = HAL_INVALID;
		}
		break;
	case HAL_DEV_MAJOR_SPI:
		if (minor == SPI0) {
			info[0].pinmux = g_pinmux_spi0;
			info[0].count = HAL_ARRAY_SIZE(g_pinmux_spi0);
			info[1].pinmux = g_pinmux_spi0_cs0;
			info[1].count = HAL_ARRAY_SIZE(g_pinmux_spi0_cs0);
		} else if (minor == SPI1) {
			info[0].pinmux = g_pinmux_spi1;
			info[0].count = HAL_ARRAY_SIZE(g_pinmux_spi1);
			switch (param) {
			case SPI_TCTRL_SS_SEL_SS0:
				info[1].pinmux = g_pinmux_spi1_cs0;
				info[1].count = HAL_ARRAY_SIZE(g_pinmux_spi1_cs0);
				break;
			case SPI_TCTRL_SS_SEL_SS1:
				info[1].pinmux = g_pinmux_spi1_cs1;
				info[1].count = HAL_ARRAY_SIZE(g_pinmux_spi1_cs1);
				break;
			case SPI_TCTRL_SS_SEL_SS2:
				info[1].pinmux = g_pinmux_spi1_cs2;
				info[1].count = HAL_ARRAY_SIZE(g_pinmux_spi1_cs2);
				break;
			default:
				ret = HAL_INVALID;
				break;
			}
		} else {
			ret = HAL_INVALID;
		}
		break;
	case HAL_DEV_MAJOR_IRRX:
		info[0].pinmux = g_pinmux_irrx;
		info[0].count = HAL_ARRAY_SIZE(g_pinmux_irrx);
		break;
	case HAL_DEV_MAJOR_IRTX:
		info[0].pinmux = g_pinmux_irtx;
		info[0].count = HAL_ARRAY_SIZE(g_pinmux_irtx);
		break;
	case HAL_DEV_MAJOR_I2S:
		info[0].pinmux = g_pinmux_i2s;
		info[0].count = HAL_ARRAY_SIZE(g_pinmux_i2s);
		break;
	case HAL_DEV_MAJOR_DMIC:
		info[0].pinmux = g_pinmux_dmic;
		info[0].count = HAL_ARRAY_SIZE(g_pinmux_dmic);
		break;
	case HAL_DEV_MAJOR_ADC:
		if (minor < HAL_ARRAY_SIZE(g_pinmux_adc)) {
			info[0].pinmux = &g_pinmux_adc[minor];
			info[0].count = 1;
		} else {
			ret = HAL_INVALID;
		}
		break;
	case HAL_DEV_MAJOR_PWM:
		if (minor < HAL_ARRAY_SIZE(g_pinmux_pwm)) {
			info[0].pinmux = &g_pinmux_pwm[minor];
			info[0].count = 1;
		} else if (minor != ADC_CHANNEL_8) {
			ret = HAL_INVALID;
		}
		break;
	case HAL_DEV_MAJOR_FLASHC:
		info[0].pinmux = g_pinmux_flashc;
		info[0].count = HAL_ARRAY_SIZE(g_pinmux_flashc);
		break;
	case HAL_DEV_MAJOR_SDC:
		info[0].pinmux = g_pinmux_sd0;
		info[0].count = HAL_ARRAY_SIZE(g_pinmux_sd0);
		info[1].pinmux = g_pinmux_sd0_det;
		info[1].count = 1;
		break;
	case HAL_DEV_MAJOR_CSI:
		info[0].pinmux = g_pinmux_csi;
		info[0].count = HAL_ARRAY_SIZE(g_pinmux_csi);
		break;
	case HAL_DEV_MAJOR_AUDIO_CODEC:
		info[0].pinmux = g_pinmux_spk;
		info[0].count = HAL_ARRAY_SIZE(g_pinmux_spk);
		break;
	default:
		BOARD_ERR("unknow major %u\n", major);
		ret = HAL_INVALID;
	}

	return ret;
}

static HAL_Status board_get_cfg(uint32_t major, uint32_t minor, uint32_t param)
{
	HAL_Status ret = HAL_OK;

	switch (major) {
	case HAL_DEV_MAJOR_SDC:
		*((HAL_SDCGPIOCfg **)param) = (HAL_SDCGPIOCfg *)&g_sd0_cfg;
		break;
	case HAL_DEV_MAJOR_FLASH:
		if (minor <= (sizeof(g_flash_cfg) / sizeof(FlashBoardCfg)))
			*((FlashBoardCfg **)param) = (FlashBoardCfg *)&g_flash_cfg[minor];
		else
			*((FlashBoardCfg **)param) = NULL;
		break;
	case HAL_DEV_MAJOR_AUDIO_CODEC:
		*((SPK_Param **)param) = (SPK_Param *)&g_spk_cfg;
		break;
	default:
		BOARD_ERR("unknow major %u\n", major);
		ret = HAL_INVALID;
	}

	return ret;
}

HAL_Status board_ioctl(HAL_BoardIoctlReq req, uint32_t param0, uint32_t param1)
{
	HAL_Status ret = HAL_OK;
	uint32_t major, minor, i;
	struct board_pinmux_info info[BOARD_PINMUX_INFO_MAX];

	switch (req) {
	case HAL_BIR_PINMUX_INIT:
	case HAL_BIR_PINMUX_DEINIT:
		memset(info, 0, sizeof(info));
		major = HAL_DEV_MAJOR((HAL_Dev_t)param0);
		minor = HAL_DEV_MINOR((HAL_Dev_t)param0);
		ret = board_get_pinmux_info(major, minor, param1, info);
		if (major == HAL_DEV_MAJOR_SDC) {
			if (param1 == SDCGPIO_BAS) {
				board_pinmux_cfg(req, info[0].pinmux, info[0].count);
			} else if (param1 == SDCGPIO_DET) {
				board_pinmux_cfg(req, info[1].pinmux, info[1].count);
			}
		} else {
			for (i = 0; i < BOARD_PINMUX_INFO_MAX; ++i) {
				if (info[i].pinmux != NULL && info[i].count != 0) {
					board_pinmux_cfg(req, info[i].pinmux, info[i].count);
				} else {
					break;
				}
			}
		}
		break;
	case HAL_BIR_CHIP_CLOCK_INIT:
		board_chip_clock_init();
		break;
	case HAL_BIR_GET_CFG:
		major = HAL_DEV_MAJOR((HAL_Dev_t)param0);
		minor = HAL_DEV_MINOR((HAL_Dev_t)param0);
		ret = board_get_cfg(major, minor, param1);
		break;
	default:
		BOARD_ERR("req %d not suppport\n", req);
		ret = HAL_INVALID;
		break;
	}

	if (ret != HAL_OK) {
		BOARD_ERR("req %d, param0 %#x, param1 %#x, ret %d\n", req, param0, param1, ret);
	}

	return ret;
}
