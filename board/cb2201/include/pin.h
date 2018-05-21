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
#include "pinmux.h"

#define CONSOLE_TXD         PA10//PA2//PA2_TXD0_SPI0MISO
#define CONSOLE_RXD         PA11//PA3
#define CONSOLE_TXD_FUNC    PA10_UART0_TX
#define CONSOLE_RXD_FUNC    PA11_UART0_RX
#define CONSOLE_IDX        0

/****Netm Drvier****/
#define NETM_USART_IDX              2
#define NETM_PIN_USART_TX           PA24
#define NETM_PIN_USART_RX           PA25
#define NETM_PIN_USART_TX_FUNC      PA24_UART2_TX
#define NETM_PIN_USART_RX_FUNC      PA25_UART2_RX

/****Ethernet Drvier****/
#define ENC28J60_ETH_SPI_IDX        0
#define ENC28J60_ETH_SPI_MISO       PB3
#define ENC28J60_ETH_SPI_MOSI       PA6
#define ENC28J60_ETH_SPI_SCK        PB2
#define ENC28J60_ETH_SPI_CS         PA7
#define ENC28J60_ETH_PIN_INT        PA20
#define ENC28J60_ETH_PIN_RST        PA5
#define ENC28J60_ETH_SPI_MISO_FUNC  PB3_SPI0_MISO
#define ENC28J60_ETH_SPI_MOSI_FUNC  PA6_SPI0_MOSI
#define ENC28J60_ETH_SPI_SCK_FUNC   PB2_SPI0_CLK
#define ENC28J60_ETH_SPI_CS_FUNC    PIN_FUNC_GPIO
#define ENC28J60_ETH_PIN_RST_FUNC   PIN_FUNC_GPIO
#define ENC28J60_ETH_PIN_INT_FUNC   PIN_FUNC_GPIO

/****AT Module ****/
#define AT_UART_IDX                 1
#define AT_PIN_USART_TX             PA17
#define AT_PIN_USART_RX             PA16
#define AT_TXD_FUNC                 PA17_UART1_TX
#define AT_RXD_FUNC                 PA16_UART1_RX

//adc
#define EXAMPLE_ADC_CH0             PA8
#define EXAMPLE_ADC_CH0_FUNC        PA8_ADC_A0
#define EXAMPLE_ADC_CH12            PA26
#define EXAMPLE_ADC_CH12_FUNC       PA26_ADC_A12

/****Sensor Drvier****/
#define EXAMPLE_DHT11_PIN            PA4
#define EXAMPLE_DHT11_PIN_FUNC       PIN_FUNC_GPIO

#define EXAMPLE_DHT22_PIN            PA5
#define EXAMPLE_DHT22_PIN_FUNC       PIN_FUNC_GPIO

#define EXAMPLE_LED1_PIN1            PA12
#define EXAMPLE_LED1_PIN1_FUNC       PIN_FUNC_GPIO

#define EXAMPLE_LED1_PIN2            PA13
#define EXAMPLE_LED1_PIN2_FUNC       PIN_FUNC_GPIO

#define EXAMPLE_LED2_PIN1            PA14
#define EXAMPLE_LED2_PIN1_FUNC       PIN_FUNC_GPIO

#define EXAMPLE_LED2_PIN2            PA15
#define EXAMPLE_LED2_PIN2_FUNC       PIN_FUNC_GPIO

#define SENSOR_IIC_IDX               1

#define EXAMPLE_IIC_IDX             1
#define EXAMPLE_PIN_IIC_SDA         PC1
#define EXAMPLE_PIN_IIC_SCL         PC0
#define EXAMPLE_PIN_IIC_SDA_FUNC    PC1_I2C1_SDA
#define EXAMPLE_PIN_IIC_SCL_FUNC    PC0_I2C1_SCL

#define EXAMPLE_PWM_CH              PA4
#define EXAMPLE_PWM_CH_FUNC         PA4_PWM_CH0

#endif /* HOBBIT1_2_PIN_H */
