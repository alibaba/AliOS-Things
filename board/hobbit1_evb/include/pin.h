/*
 * Copyright (C) 2017 C-SKY Microsystems Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/******************************************************************************
 * @file     pin.h
 * @brief    header File for pin definition
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#ifndef HOBBIT1_2_PIN_H
#define HOBBIT1_2_PIN_H

#include <stdint.h>
#include "pin_name.h"

#define CONSOLE_TXD       PA10_ADC2_TXD0//PA2_TXD0_SPI0MISO
#define CONSOLE_RXD       PA11_ACMP0N_ADC3_RXD0//PA3_RXD0_SPI0MOSI
#define CONSOLE_IDX        0

#define UART_TXD1     PA17_TXD1_ADC9//PA21_SPI1SCK_TRIG1_TXD1
#define UART_RXD1     PA16_RXD1_ADC8//PA20_SPI1SSN2_TRIG0_RXD1

#define UART_TXD2       PA24_TXD2_I2SMCLK_SPI1SSN0
#define UART_RXD2       PA25_RXD2_I2SSCK_SPI1SSN1

#define SPI0_TX     PA2_TXD0_SPI0MISO
#define SPI0_RX     PA3_RXD0_SPI0MOSI
#define SPI0_CLK    PA4_CTS0_PWM0_SPI0SCK_TRIG0
#define SPI0_CS     PA5_RTS0_PWM1_SPI0SSN_TRIG1

#define SPI1_TX     PA22_SPI1MISO_PWM0_ADC10
#define SPI1_RX     PA23_SPI1MOSI_PWM1_ADC11
#define SPI1_CLK    PA21_SPI1SCK_TRIG1_TXD1
#define SPI1_CS     PA18_SPI1SSN0_ACMP0O
#define TEST_SPI_IDX    1
#define IIC_SCL0   PA6_SPI0MOSI_PWM6_SCL0
#define IIC_SDA0   PA7_SPI0SSN_PWM7_SDA0

#define IIC_SCL1   PC0_SCL1_CTS1_PWM10_ADC14
#define IIC_SDA1   PC1_SDA1_RTS1_PWM11_ADC15
#define TEST_IIC_IDX 1

#define UART_PINS  { {PA2_TXD0_SPI0MISO,PA3_RXD0_SPI0MOSI},\
        {PA21_SPI1SCK_TRIG1_TXD1,PA20_SPI1SSN2_TRIG0_RXD1},\
        {PA24_TXD2_I2SMCLK_SPI1SSN0,PA25_RXD2_I2SSCK_SPI1SSN1} }

#define UART_PINS  { {PA2_TXD0_SPI0MISO,PA3_RXD0_SPI0MOSI},\
        {PA21_SPI1SCK_TRIG1_TXD1,PA20_SPI1SSN2_TRIG0_RXD1},\
        {PA24_TXD2_I2SMCLK_SPI1SSN0,PA25_RXD2_I2SSCK_SPI1SSN1} }

#define PWM_CH0     PA4_CTS0_PWM0_SPI0SCK_TRIG0
#define PWM_CH1     PA5_RTS0_PWM1_SPI0SSN_TRIG1
#define PWM_CH2     PB0_SCL0_PWM2_I2SMCLK
#define PWM_CH3     PB1_SDA0_PWM3_I2SSCK
#define PWM_CH4     PB2_SPI0SCK_PWM4_I2SWS
#define PWM_CH5     PB3_SPI0MISO_PWM5_I2SSD

#define GPIO_EXAMPLE_PORT   PORTA
#define GPIO_EXAMPLE_PIN    PA17_TXD1_ADC9
#define CTS_GPIO_TEST_PIN   PA18_SPI1SSN0_ACMP0O
#define CTS_GPIO_TEST_PORT  PORTA
#define EXAMPLE_BOARD_GPIO_PIN_NAME "TX1"
#define CTS_BOARD_GPIO_PIN_NAME     "PA18"

/****Ethernet Drvier****/
#define PIN_ETH_INT     PA20_SPI1SSN2_TRIG0_RXD1
#define PIN_ETH_RST     PA5_RTS0_PWM1_SPI0SSN_TRIG1
#define ETH_SPI_TX      PB3_SPI0MISO_PWM5_I2SSD//PA2_TXD0_SPI0MISO
#define ETH_SPI_RX      PA6_SPI0MOSI_PWM6_SCL0//PA3_RXD0_SPI0MOSI
#define ETH_SPI_CLK     PB2_SPI0SCK_PWM4_I2SWS//PA4_CTS0_PWM0_SPI0SCK_TRIG0
#define ETH_SPI_CS      PA7_SPI0SSN_PWM7_SDA0//PA5_RTS0_PWM1_SPI0SSN_TRIG1
#define ETH_SPI_IDX     0

#define ADC_PIN0     PA8_WKUP_ADC0_ACMP0P
#define ADC_PIN1     PA9_BOOT_ADC1_PWMFAULT
#define ADC_PIN2     PA10_ADC2_TXD0
#define ADC_PIN3     PA11_ACMP0N_ADC3_RXD0
#define ADC_PIN4     PA12_PWM8_TCK_ADC4
#define ADC_PIN5     PA13_PWM9_TMS_ADC5
#define ADC_PIN6     PA14_PWM10_ADC6
#define ADC_PIN7     PA15_PWM11_ADC7
#define ADC_PIN8     PA16_RXD1_ADC8
#define ADC_PIN9     PA17_TXD1_ADC9
#define ADC_PIN10    PA22_SPI1MISO_PWM0_ADC10
#define ADC_PIN11    PA23_SPI1MOSI_PWM1_ADC11
#define ADC_PIN12    PA26_CTS2_I2SWS_ADC12
#define ADC_PIN13    PA27_RTS2_I2SSD_ADC13
#define ADC_PIN14    PC0_SCL1_CTS1_PWM10_ADC14
#define ADC_PIN15    PC1_SDA1_RTS1_PWM11_ADC15

#define ADC_PINS_MAX_NUM    16

#endif /* HOBBIT1_2_PIN_H */
