/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_
#include "eagle_soc.h"
#define PERIPHS_IO_MUX                  0x60000800

#define PERIPHS_IO_MUX_FUNC             0x13
#define PERIPHS_IO_MUX_FUNC_S           4
#define PERIPHS_IO_MUX_PULLUP           BIT7
#define PERIPHS_IO_MUX_PULLDWN          BIT6
#define PERIPHS_IO_MUX_SLEEP_PULLUP     BIT3
#define PERIPHS_IO_MUX_SLEEP_PULLDWN    BIT2
#define PERIPHS_IO_MUX_SLEEP_OE         BIT1
#define PERIPHS_IO_MUX_OE               BIT0

#define PERIPHS_IO_MUX_CONF_U           (PERIPHS_IO_MUX + 0x00)
#define SPI0_CLK_EQU_SYS_CLK                BIT8
#define SPI1_CLK_EQU_SYS_CLK                BIT9

#define PERIPHS_IO_MUX_MTDI_U           (PERIPHS_IO_MUX + 0x04)
#define FUNC_MTDI                           0
#define FUNC_I2SI_DATA                      1
#define FUNC_HSPIQ_MISO                     2
#define FUNC_GPIO12                         3
#define FUNC_UART0_DTR                      4

#define PERIPHS_IO_MUX_MTCK_U           (PERIPHS_IO_MUX + 0x08)
#define FUNC_MTCK                            0
#define FUNC_I2SI_BCK                        1
#define FUNC_HSPID_MOSI                      2
#define FUNC_GPIO13                          3
#define FUNC_UART0_CTS                       4

#define PERIPHS_IO_MUX_MTMS_U           (PERIPHS_IO_MUX + 0x0C)
#define FUNC_MTMS                           0
#define FUNC_I2SI_WS                        1
#define FUNC_HSPI_CLK                       2
#define FUNC_GPIO14                         3
#define FUNC_UART0_DSR                      4

#define PERIPHS_IO_MUX_MTDO_U           (PERIPHS_IO_MUX + 0x10)
#define FUNC_MTDO                           0
#define FUNC_I2SO_BCK                       1
#define FUNC_HSPI_CS0                       2
#define FUNC_GPIO15                         3
#define FUNC_U0RTS                          4
#define FUNC_UART0_RTS                      4

#define PERIPHS_IO_MUX_U0RXD_U          (PERIPHS_IO_MUX + 0x14)
#define FUNC_U0RXD                          0
#define FUNC_I2SO_DATA                      1
#define FUNC_GPIO3                          3
#define FUNC_CLK_XTAL_BK                    4

#define PERIPHS_IO_MUX_U0TXD_U          (PERIPHS_IO_MUX + 0x18)
#define FUNC_U0TXD                          0
#define FUNC_SPICS1                         1
#define FUNC_GPIO1                          3
#define FUNC_CLK_RTC_BK                     4

#define PERIPHS_IO_MUX_SD_CLK_U         (PERIPHS_IO_MUX + 0x1c)
#define FUNC_SDCLK                          0
#define FUNC_SPICLK                         1
#define FUNC_GPIO6                          3
#define UART1_CTS                           4

#define PERIPHS_IO_MUX_SD_DATA0_U       (PERIPHS_IO_MUX + 0x20)
#define FUNC_SDDATA0                        0
#define FUNC_SPIQ_MISO                      1
#define FUNC_GPIO7                          3
#define FUNC_U1TXD                          4
#define FUNC_UART1_TXD                      4

#define PERIPHS_IO_MUX_SD_DATA1_U       (PERIPHS_IO_MUX + 0x24)
#define FUNC_SDDATA1                        0
#define FUNC_SPID_MOSI                      1
#define FUNC_GPIO8                          3
#define FUNC_U1RXD                          4
#define FUNC_UART1_RXD                      4

#define PERIPHS_IO_MUX_SD_DATA2_U       (PERIPHS_IO_MUX + 0x28)
#define FUNC_SDDATA2                        0
#define FUNC_SPIHD                          1
#define FUNC_GPIO9                          3
#define UFNC_HSPIHD                         4

#define PERIPHS_IO_MUX_SD_DATA3_U       (PERIPHS_IO_MUX + 0x2c)
#define FUNC_SDDATA3                        0
#define FUNC_SPIWP                          1
#define FUNC_GPIO10                         3
#define FUNC_HSPIWP                         4

#define PERIPHS_IO_MUX_SD_CMD_U         (PERIPHS_IO_MUX + 0x30)
#define FUNC_SDCMD                          0
#define FUNC_SPICS0                         1
#define FUNC_GPIO11                         3
#define U1RTS                               4
#define UART1_RTS                           4

#define PERIPHS_IO_MUX_GPIO0_U          (PERIPHS_IO_MUX + 0x34)
#define FUNC_GPIO0                          0
#define FUNC_SPICS2                         1
#define FUNC_CLK_OUT                        4

#define PERIPHS_IO_MUX_GPIO2_U          (PERIPHS_IO_MUX + 0x38)
#define FUNC_GPIO2                          0
#define FUNC_I2SO_WS                        1
#define FUNC_U1TXD_BK                       2
#define FUNC_UART1_TXD_BK                   2
#define FUNC_U0TXD_BK                       4
#define FUNC_UART0_TXD_BK                   4

#define PERIPHS_IO_MUX_GPIO4_U          (PERIPHS_IO_MUX + 0x3C)
#define FUNC_GPIO4                          0
#define FUNC_CLK_XTAL                       1

#define PERIPHS_IO_MUX_GPIO5_U          (PERIPHS_IO_MUX + 0x40)
#define FUNC_GPIO5                          0
#define FUNC_CLK_RTC                        1

#define PIN_PULLUP_DIS(PIN_NAME)         CLEAR_PERI_REG_MASK(PIN_NAME, PERIPHS_IO_MUX_PULLUP)
#define PIN_PULLUP_EN(PIN_NAME)          SET_PERI_REG_MASK(PIN_NAME, PERIPHS_IO_MUX_PULLUP)

#define PIN_FUNC_SELECT(PIN_NAME, FUNC)  do { \
        CLEAR_PERI_REG_MASK(PIN_NAME, (PERIPHS_IO_MUX_FUNC << PERIPHS_IO_MUX_FUNC_S)); \
        SET_PERI_REG_MASK(PIN_NAME, (((FUNC & BIT2) << 2) | (FUNC & 0x3)) << PERIPHS_IO_MUX_FUNC_S); \
    } while (0)

#endif //_PIN_MUX_H_
