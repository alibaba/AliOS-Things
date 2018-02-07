/**
  * @file  hal_gpio.h
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

#ifndef _DRIVER_CHIP_HAL_GPIO_H_
#define _DRIVER_CHIP_HAL_GPIO_H_

#include "driver/chip/hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief GPIO control register block structure
 */
typedef struct {
    __IO uint32_t MODE[4];      /* offset: 0x00, GPIO working mode configuration register */
    __IO uint32_t DATA;         /* offset: 0x10, GPIO data register */
    __IO uint32_t DRIVING[2];   /* offset: 0x14, GPIO driving level register */
    __IO uint32_t PULL[2];      /* offset: 0x1C, GPIO pull type register */
} GPIO_CTRL_T;

/**
 * @brief GPIO port definition
 */
typedef enum {
    GPIO_PORT_A = 0U,
    GPIO_PORT_B = 1U,
    GPIO_PORT_NUM
} GPIO_Port;

/**
 * @brief GPIO working mode (function) definition
 */
typedef enum {
    GPIOx_Pn_F0_INPUT       = 0U,   /* for all GPIO pins */
    GPIOx_Pn_F1_OUTPUT      = 1U,   /* for all GPIO pins */
    GPIOx_Pn_F6_EINT        = 6U,   /* for all GPIO pins */
    GPIOx_Pn_F7_DISABLE     = 7U,   /* for all GPIO pins */

    GPIOA_P0_F2_SPI1_MOSI   = 2U,
    GPIOA_P0_F3_SD_CMD      = 3U,
    GPIOA_P0_F4_UART0_TX    = 4U,
    GPIOA_P0_F5_CSI_D0      = 5U,
    GPIOA_P0_F6_EINTA0      = 6U,

    GPIOA_P1_F2_SPI1_MISO   = 2U,
    GPIOA_P1_F3_SD_DATA0    = 3U,
    GPIOA_P1_F4_UART0_RX    = 4U,
    GPIOA_P1_F5_CSI_D1      = 5U,
    GPIOA_P1_F6_EINTA1      = 6U,

    GPIOA_P2_F2_SPI1_CLK    = 2U,
    GPIOA_P2_F3_SD_CLK      = 3U,
    GPIOA_P2_F4_I2C1_SCL    = 4U,
    GPIOA_P2_F5_CSI_D2      = 5U,
    GPIOA_P2_F6_EINTA2      = 6U,

    GPIOA_P3_F2_SPI1_CS0    = 2U,
    GPIOA_P3_F3_SD_DATA1    = 3U,
    GPIOA_P3_F4_I2C1_SDA    = 4U,
    GPIOA_P3_F5_CSI_D3      = 5U,
    GPIOA_P3_F6_EINTA3      = 6U,

    GPIOA_P4_F2_UART1_CTS   = 2U,
    GPIOA_P4_F3_SD_DATA2    = 3U,
    GPIOA_P4_F4_I2C0_SCL    = 4U,
    GPIOA_P4_F4_CSI_D4      = 5U,
    GPIOA_P4_F6_EINTA4      = 6U,

    GPIOA_P5_F2_UART1_RTS   = 2U,
    GPIOA_P5_F3_SD_DATA3    = 3U,
    GPIOA_P5_F4_I2C0_SDA    = 4U,
    GPIOA_P5_F5_CSI_D5      = 5U,
    GPIOA_P5_F6_EINTA5      = 6U,

    GPIOA_P6_F2_UART1_TX    = 2U,
    GPIOA_P6_F3_SPI1_CS1    = 3U,
    GPIOA_P6_F4_I2C0_SCL    = 4U,
    GPIOA_P6_F5_CSI_D6      = 5U,
    GPIOA_P6_F6_EINTA6      = 6U,

    GPIOA_P7_F2_UART1_RX    = 2U,
    GPIOA_P7_F3_SPI1_CS2    = 3U,
    GPIOA_P7_F4_I2C0_SDA    = 4U,
    GPIOA_P7_F5_CSI_D7      = 5U,
    GPIOA_P7_F6_EINTA7      = 6U,

    GPIOA_P8_F2_ADC_CH0     = 2U,
    GPIOA_P8_F3_PWM0_ECT0   = 3U,
    GPIOA_P8_F4_I2C1_SCL    = 4U,
    GPIOA_P8_F5_CSI_PCLK    = 5U,
    GPIOA_P8_F6_EINTA8      = 6U,

    GPIOA_P9_F2_ADC_CH1     = 2U,
    GPIOA_P9_F3_PWM1_ECT1   = 3U,
    GPIOA_P9_F4_I2C1_SDA    = 4U,
    GPIOA_P9_F5_CSI_MCLK    = 5U,
    GPIOA_P9_F6_EINTA9      = 6U,

    GPIOA_P10_F2_ADC_CH2    = 2U,
    GPIOA_P10_F3_PWM2_ECT2  = 3U,
    GPIOA_P10_F4_DMIC_CLK   = 4U,
    GPIOA_P10_F5_CSI_HSYNC  = 5U,
    GPIOA_P10_F6_EINTA10    = 6U,

    GPIOA_P11_F2_ADC_CH3    = 2U,
    GPIOA_P11_F3_PWM3_ECT3  = 3U,
    GPIOA_P11_F4_DMIC_DATA  = 4U,
    GPIOA_P11_F5_CSI_VSYNC  = 5U,
    GPIOA_P11_F6_EINTA11    = 6U,

    GPIOA_P12_F2_ADC_CH4    = 2U,
    GPIOA_P12_F3_PWM4_ECT4  = 3U,
    GPIOA_P12_F4_I2S_MCLK   = 4U,
    GPIOA_P12_F5_IR_TX      = 5U,
    GPIOA_P12_F6_EINTA12    = 6U,

    GPIOA_P13_F2_ADC_CH5    = 2U,
    GPIOA_P13_F3_PWM5_ECT5  = 3U,
    GPIOA_P13_F4_I2S_BCLK   = 4U,
    GPIOA_P13_F6_EINTA13    = 6U,

    GPIOA_P14_F2_ADC_CH6    = 2U,
    GPIOA_P14_F3_PWM6_ECT6  = 3U,
    GPIOA_P14_F4_I2S_DI     = 4U,
    GPIOA_P14_F5_IR_RX      = 5U,
    GPIOA_P14_F6_EINTA14    = 6U,

    GPIOA_P15_F2_ADC_CH7    = 2U,
    GPIOA_P15_F3_PWM7_ECT7  = 3U,
    GPIOA_P15_F4_I2S_DO     = 4U,
    GPIOA_P15_F5_UART1_CTS  = 5U,
    GPIOA_P15_F6_EINTA15    = 6U,

    GPIOA_P16_F2_IR_TX      = 2U,
    GPIOA_P16_F3_IR_RX      = 3U,
    GPIOA_P16_F4_I2S_LRCLK  = 4U,
    GPIOA_P16_F5_UART1_RTS  = 5U,
    GPIOA_P16_F6_EINTA16    = 6U,

    GPIOA_P17_F2_I2C0_SCL   = 2U,
    GPIOA_P17_F3_IR_RX      = 3U,
    GPIOA_P17_F4_I2C1_SCL   = 4U,
    GPIOA_P17_F5_UART1_TX   = 5U,
    GPIOA_P17_F6_EINTA17    = 6U,

    GPIOA_P18_F2_I2C0_SDA   = 2U,
    GPIOA_P18_F3_IR_TX      = 3U,
    GPIOA_P18_F4_I2C1_SDA   = 4U,
    GPIOA_P18_F5_UART1_RX   = 5U,
    GPIOA_P18_F6_EINTA18    = 6U,

    GPIOA_P19_F2_NUART_CTS  = 2U,
    GPIOA_P19_F3_NSWD_TMS   = 3U,
    GPIOA_P19_F4_PWM0_ECT0  = 4U,
    GPIOA_P19_F5_SPI1_MOSI  = 5U,
    GPIOA_P19_F6_EINTA19    = 6U,

    GPIOA_P20_F2_NUART_RTS  = 2U,
    GPIOA_P20_F3_NSWD_TCK   = 3U,
    GPIOA_P20_F4_PWM1_ECT1  = 4U,
    GPIOA_P20_F5_SPI1_MISO  = 5U,
    GPIOA_P20_F6_EINTA20    = 6U,

    GPIOA_P21_F2_NUART_TX   = 2U,
    GPIOA_P21_F3_DMIC_CLK   = 3U,
    GPIOA_P21_F4_PWM2_ECT2  = 4U,
    GPIOA_P21_F5_SPI1_CLK   = 5U,
    GPIOA_P21_F6_EINTA21    = 6U,

    GPIOA_P22_F2_NUART_RX   = 2U,
    GPIOA_P22_F3_DMIC_DATA  = 3U,
    GPIOA_P22_F4_PWM3_ECT3  = 4U,
    GPIOA_P22_F5_SPI1_CS0   = 5U,
    GPIOA_P22_F6_EINTA22    = 6U,

    GPIOB_P0_F2_UART0_TX    = 2U,
    GPIOB_P0_F3_JTAG_TMS    = 3U,
    GPIOB_P0_F4_PWM4_ECT4   = 4U,
    GPIOB_P0_F5_SWD_TMS     = 5U,
    GPIOB_P0_F6_EINTB0      = 6U,

    GPIOB_P1_F2_UART0_RX    = 2U,
    GPIOB_P1_F3_JTAG_TCK    = 3U,
    GPIOB_P1_F4_PWM5_ECT5   = 4U,
    GPIOB_P1_F5_SWD_TCK     = 5U,
    GPIOB_P1_F6_EINTB1      = 6U,

    GPIOB_P2_F2_SWD_TMS     = 2U,
    GPIOB_P2_F3_JTAG_TD0    = 3U,
    GPIOB_P2_F4_PWM6_ECT6   = 4U,
    GPIOB_P2_F5_FLASH_WP    = 5U,
    GPIOB_P2_F6_EINTB2      = 6U,

    GPIOB_P3_F2_SWD_TCK     = 2U,
    GPIOB_P3_F3_JTAG_TDI    = 3U,
    GPIOB_P3_F4_PWM7_ECT7   = 4U,
    GPIOB_P3_F5_FLASH_HOLD  = 5U,
    GPIOB_P3_F6_EINTB3      = 6U,

    GPIOB_P4_F2_SPI0_MOSI   = 2U,
    GPIOB_P4_F3_SD_CMD      = 3U,
    GPIOB_P4_F5_FLASH_MOSI  = 5U,
    GPIOB_P4_F6_EINTB4      = 6U,

    GPIOB_P5_F2_SPI0_MISO   = 2U,
    GPIOB_P5_F3_SD_DATA0    = 3U,
    GPIOB_P5_F5_FLASH_MISO  = 5U,
    GPIOB_P5_F6_EINTB5      = 6U,

    GPIOB_P6_F2_SPI0_CS0    = 2U,
    GPIOB_P6_F5_FLASH_CS    = 5U,
    GPIOB_P6_F6_EINTB6      = 6U,

    GPIOB_P7_F2_SPI0_CLK    = 2U,
    GPIOB_P7_F3_SD_CLK      = 3U,
    GPIOB_P7_F5_FLASH_CLK   = 5U,
    GPIOB_P7_F6_EINTB7      = 6U,
} GPIO_WorkMode;

/**
 * @brief GPIO driving level definition
 */
typedef enum {
    GPIO_DRIVING_LEVEL_0 = 0U,
    GPIO_DRIVING_LEVEL_1 = 1U,
    GPIO_DRIVING_LEVEL_2 = 2U,
    GPIO_DRIVING_LEVEL_3 = 3U
} GPIO_DrivingLevel;

/**
 * @brief GPIO pull type definition
 */
typedef enum {
    GPIO_PULL_NONE = 0U,
    GPIO_PULL_UP   = 1U,
    GPIO_PULL_DOWN = 2U
} GPIO_PullType;

/**
 * @brief GPIO interrupt register block structure
 */
typedef struct {
    __IO uint32_t IRQ_MODE[4];  /* offset: 0x00, GPIO interrupt configuration register */
    __IO uint32_t IRQ_EN;       /* offset: 0x10, GPIO interrupt enable register */
    __IO uint32_t IRQ_STATUS;   /* offset: 0x14, GPIO interrupt status register */
    __IO uint32_t IRQ_DEBOUNCE; /* offset: 0x18, GPIO interrupt debounce register */
} GPIO_IRQ_T;

/**
 * @brief GPIO interrupt trigger event definition
 */
typedef enum {
    GPIO_IRQ_EVT_RISING_EDGE  = 0U,
    GPIO_IRQ_EVT_FALLING_EDGE = 1U,
    GPIO_IRQ_EVT_HIGH_LEVEL   = 2U,
    GPIO_IRQ_EVT_LOW_LEVEL    = 3U,
    GPIO_IRQ_EVT_BOTH_EDGE    = 4U
} GPIO_IrqEvent;

/* GPIO interrupt debounce value */
#define GPIO_IRQ_DEB_CLK_SCALE_SHIFT    4   /* R/W */
#define GPIO_IRQ_DEB_CLK_SCALE_MASK     (0x7U << GPIO_IRQ_DEB_CLK_SCALE_SHIFT)

#define GPIO_IRQ_DEB_CLK_SRC_SHIFT      0   /* R/W */
#define GPIO_IRQ_DEB_CLK_SRC_MASK       (0x1U << GPIO_IRQ_DEB_CLK_SRC_SHIFT)
typedef enum {
    GPIO_IRQ_DEB_CLK_SRC_LFCLK = 0U,
    GPIO_IRQ_DEB_CLK_SRC_HFCLK = 1U
} GPIO_IrqDebounceClkSrc;

/******************************************************************************/

/**
 * @brief GPIO pin state definition
 */
typedef enum {
    GPIO_PIN_LOW  = 0,
    GPIO_PIN_HIGH = 1,
} GPIO_PinState;

/**
 * @brief GPIO pin number definition
 *     - GPIOA: pin [0:23]
 *     - GPIOB: pin [0:7]
 */
typedef enum {
    GPIO_PIN_0  = 0U,
    GPIO_PIN_1  = 1U,
    GPIO_PIN_2  = 2U,
    GPIO_PIN_3  = 3U,
    GPIO_PIN_4  = 4U,
    GPIO_PIN_5  = 5U,
    GPIO_PIN_6  = 6U,
    GPIO_PIN_7  = 7U,
    GPIO_PIN_8  = 8U,
    GPIO_PIN_9  = 9U,
    GPIO_PIN_10 = 10U,
    GPIO_PIN_11 = 11U,
    GPIO_PIN_12 = 12U,
    GPIO_PIN_13 = 13U,
    GPIO_PIN_14 = 14U,
    GPIO_PIN_15 = 15U,
    GPIO_PIN_16 = 16U,
    GPIO_PIN_17 = 17U,
    GPIO_PIN_18 = 18U,
    GPIO_PIN_19 = 19U,
    GPIO_PIN_20 = 20U,
    GPIO_PIN_21 = 21U,
    GPIO_PIN_22 = 22U,

    GPIOA_PIN_NUM = 23U,
    GPIOB_PIN_NUM = 8U
} GPIO_Pin;

/**
 * @brief Wakeup I/O of GPIO port A definition
 */
typedef enum {
    WAKEUP_IO0 = GPIO_PIN_4,
    WAKEUP_IO1 = GPIO_PIN_5,
    WAKEUP_IO2 = GPIO_PIN_6,
    WAKEUP_IO3 = GPIO_PIN_7,
    WAKEUP_IO4 = GPIO_PIN_17,
    WAKEUP_IO5 = GPIO_PIN_18,
    WAKEUP_IO6 = GPIO_PIN_19,
    WAKEUP_IO7 = GPIO_PIN_20,
    WAKEUP_IO8 = GPIO_PIN_21,
    WAKEUP_IO9 = GPIO_PIN_22,
    WAKEUP_IO_MAX = 10, /* keep last */
} WAKEUP_IO;

/**
 * @brief GPIO initialization parameters
 */
typedef struct {
    GPIO_WorkMode       mode;
    GPIO_DrivingLevel   driving;
    GPIO_PullType       pull;
} GPIO_InitParam;

/**
 * @brief GPIO pinmux configuration parameters
 */
typedef struct {
    GPIO_Port       port;
    GPIO_Pin        pin;
    GPIO_InitParam  config;
} GPIO_PinMuxParam;

/** @brief Type define of GPIO IRQ callback function */
typedef void (*GPIO_IRQCallback) (void *arg);

/**
 * @brief GPIO interrupt enable parameters
 */
typedef struct {
    GPIO_IrqEvent       event;
    GPIO_IRQCallback    callback;
    void               *arg;
} GPIO_IrqParam;

void HAL_GPIO_Init(GPIO_Port port, GPIO_Pin pin, const GPIO_InitParam *param);
void HAL_GPIO_DeInit(GPIO_Port port, GPIO_Pin pin);

void HAL_GPIO_WritePin(GPIO_Port port, GPIO_Pin pin, GPIO_PinState state);
GPIO_PinState HAL_GPIO_ReadPin(GPIO_Port port, GPIO_Pin pin);

void HAL_GPIO_WritePort(GPIO_Port port, uint32_t portMask);
uint32_t HAL_GPIO_ReadPort(GPIO_Port port);

void HAL_GPIO_EnableIRQ(GPIO_Port port, GPIO_Pin pin, const GPIO_IrqParam *param);
void HAL_GPIO_DisableIRQ(GPIO_Port port, GPIO_Pin pin);

void HAL_GPIO_PinMuxConfig(const GPIO_PinMuxParam *param, uint32_t count);
void HAL_GPIO_PinMuxDeConfig(const GPIO_PinMuxParam *param, uint32_t count);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_GPIO_H_ */
