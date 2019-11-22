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

#define UART_TXD1     PA17_TXD1_ADC9//PA21_SPI1SCK_TRIG1_TXD1
#define UART_RXD1     PA16_RXD1_ADC8//PA20_SPI1SSN2_TRIG0_RXD1

#define UART_TXD2       PA24_TXD2_I2SMCLK_SPI1SSN0
#define UART_RXD2       PA25_RXD2_I2SSCK_SPI1SSN1
#define UART_CTS2       PA26_CTS2_I2SWS_ADC12
#define UART_RTS2       PA27_RTS2_I2SSD_ADC13

#define SPI0_TX     PB3_SPI0MISO_PWM5_I2SSD//PA2_TXD0_SPI0MISO
#define SPI0_RX     PA6_SPI0MOSI_PWM6_SCL0//PA3_RXD0_SPI0MOSI
#define SPI0_CLK    PB2_SPI0SCK_PWM4_I2SWS//PA4_CTS0_PWM0_SPI0SCK_TRIG0
#define SPI0_CS     PA7_SPI0SSN_PWM7_SDA0//PA5_RTS0_PWM1_SPI0SSN_TRIG1

#define SPI1_TX     PA22_SPI1MISO_PWM0_ADC10
#define SPI1_RX     PA23_SPI1MOSI_PWM1_ADC11
#define SPI1_CLK    PA21_SPI1SCK_TRIG1_TXD1
#define SPI1_CS     PA18_SPI1SSN0_ACMP0O

#define IIC_SCL0   PA6_SPI0MOSI_PWM6_SCL0
#define IIC_SDA0   PA7_SPI0SSN_PWM7_SDA0

#define IIC_SCL1   PC0_SCL1_CTS1_PWM10_ADC14
#define IIC_SDA1   PC1_SDA1_RTS1_PWM11_ADC15

#define IIC_PINS    { {PA6_SPI0MOSI_PWM6_SCL0,PA7_SPI0SSN_PWM7_SDA0}, \
                      {PC0_SCL1_CTS1_PWM10_ADC14,PC1_SDA1_RTS1_PWM11_ADC15} }

#define UART_PINS  { {0/*CONSOLE_TXD*/,0/*CONSOLE_RXD*/},\
        {UART_TXD1,UART_RXD1},\
        {UART_TXD2,UART_RXD2} }

#define PIN_ETH_INT         PA20_SPI1SSN2_TRIG0_RXD1
#define PIN_ETH_RST         PA5_RTS0_PWM1_SPI0SSN_TRIG1

#define ETH_SPI_TX  SPI0_TX
#define ETH_SPI_RX  SPI0_RX
#define ETH_SPI_CLK SPI0_CLK
#define ETH_SPI_CS  SPI0_CS

#define PIN_SENSOR_SPEED   PA18_SPI1SSN0_ACMP0O

#define PWM_CH0     PA4_CTS0_PWM0_SPI0SCK_TRIG0
#define PWM_CH1     PA5_RTS0_PWM1_SPI0SSN_TRIG1
#define PWM_CH2     PB0_SCL0_PWM2_I2SMCLK
#define PWM_CH3     PB1_SDA0_PWM3_I2SSCK
#define PWM_CH4     PB2_SPI0SCK_PWM4_I2SWS
#define PWM_CH5     PB3_SPI0MISO_PWM5_I2SSD

#define GPIO_EXAMPLE_PORT   PORTB
#define GPIO_EXAMPLE_PIN    PB0_SCL0_PWM2_I2SMCLK
#define CTS_GPIO_TEST_PIN   PA7_SPI0SSN_PWM7_SDA0
#define CTS_GPIO_TEST_PORT  PORTA
#define EXAMPLE_BOARD_GPIO_PIN_NAME "J14 B8"
#define CTS_BOARD_GPIO_PIN_NAME     "J13 B10"
//need to redefine
#define WIND_SPD_DIR_TX2 PA17_TXD1_ADC9
#define WIND_SPD_DIR_RX2 PA16_RXD1_ADC8

#define TEMP_HUMI_RX0 PC0_SCL1_CTS1_PWM10_ADC14
#define TEMP_HUMI_CS0 PC1_SDA1_RTS1_PWM11_ADC15


#ifdef CONFIG_NETM_FLOW_CONTROL
#define NETM_RTS       PA27_RTS2_I2SSD_ADC13
#define NETM_CTS       PA26_CTS2_I2SWS_ADC12
#endif

#define NETM_TXD       PA24_TXD2_I2SMCLK_SPI1SSN0
#define NETM_RXD       PA25_RXD2_I2SSCK_SPI1SSN1

#endif /* HOBBIT1_2_PIN_H */
