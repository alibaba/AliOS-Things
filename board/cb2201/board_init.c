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
 * @file     board_init.c
 * @brief    CSI Source File for board init
 * @version  V1.0
 * @date     02. June 2017
 ******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include "drv_usart.h"

#include <csi_core.h>
#include <csi_config.h>
#include "pin.h"

#include <aos/hal/flash.h>
#include <aos/kernel.h>

extern usart_handle_t console_handle;
extern void ioreuse_initial(void);

hal_logic_partition_t hal_partitions[7];

void cb2201_pinmux_config(void)
{
    //console
    drv_pinmux_config(CONSOLE_TXD, CONSOLE_TXD_FUNC);
    drv_pinmux_config(CONSOLE_RXD, CONSOLE_RXD_FUNC);

    //uart2
    drv_pinmux_config(NETM_PIN_USART_TX, NETM_PIN_USART_TX_FUNC);
    drv_pinmux_config(NETM_PIN_USART_RX, NETM_PIN_USART_RX_FUNC);

    //enc28j60
    drv_pinmux_config(ENC28J60_ETH_SPI_MISO, ENC28J60_ETH_SPI_MISO_FUNC);
    drv_pinmux_config(ENC28J60_ETH_SPI_MOSI, ENC28J60_ETH_SPI_MOSI_FUNC);
    drv_pinmux_config(ENC28J60_ETH_SPI_CS, ENC28J60_ETH_SPI_CS_FUNC);
    drv_pinmux_config(ENC28J60_ETH_SPI_SCK, ENC28J60_ETH_SPI_SCK_FUNC);
    drv_pinmux_config(ENC28J60_ETH_PIN_RST, ENC28J60_ETH_PIN_RST_FUNC);
    drv_pinmux_config(ENC28J60_ETH_PIN_INT, ENC28J60_ETH_PIN_INT_FUNC);

    //uart1
    drv_pinmux_config(AT_PIN_USART_TX, AT_TXD_FUNC);
    drv_pinmux_config(AT_PIN_USART_RX, AT_RXD_FUNC);

    //iic
    drv_pinmux_config(EXAMPLE_PIN_IIC_SDA, EXAMPLE_PIN_IIC_SDA_FUNC);
    drv_pinmux_config(EXAMPLE_PIN_IIC_SCL, EXAMPLE_PIN_IIC_SCL_FUNC);

    //adc
    drv_pinmux_config(EXAMPLE_ADC_CH12, EXAMPLE_ADC_CH12_FUNC);

    //pwm
    drv_pinmux_config(EXAMPLE_PWM_CH, EXAMPLE_PWM_CH_FUNC);

    //dht11
    drv_pinmux_config(EXAMPLE_DHT11_PIN, EXAMPLE_DHT11_PIN_FUNC);

    //led
    drv_pinmux_config(EXAMPLE_LED1_PIN1, EXAMPLE_LED1_PIN1_FUNC);
    drv_pinmux_config(EXAMPLE_LED1_PIN2, EXAMPLE_LED1_PIN2_FUNC);
    drv_pinmux_config(EXAMPLE_LED2_PIN1, EXAMPLE_LED2_PIN1_FUNC);
    drv_pinmux_config(EXAMPLE_LED2_PIN2, EXAMPLE_LED2_PIN2_FUNC);
}

void __attribute__((weak)) board_init(void)
{
    hal_partitions[0].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_partitions[0].partition_description      = "BOOTLOADER_MTB";
    hal_partitions[0].partition_start_addr       = 0x10000000;
    hal_partitions[0].partition_length           = 0x200;    //512 bytes
    hal_partitions[0].partition_options          = PAR_OPT_READ_EN;

    hal_partitions[1].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_partitions[1].partition_description      = "BOOTLOADER";
    hal_partitions[1].partition_start_addr       = 0x10000200;
    hal_partitions[1].partition_length           = 0x5600;    //22016 bytes
    hal_partitions[1].partition_options          = PAR_OPT_READ_EN;

    hal_partitions[2].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_partitions[2].partition_description      = "TEE";
    hal_partitions[2].partition_start_addr       = 0x10005800;
    hal_partitions[2].partition_length           = 0x5000;    //20K bytes
    hal_partitions[2].partition_options          = PAR_OPT_READ_EN;

    hal_partitions[3].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_partitions[3].partition_description      = "FACTORYSETTINGS";
    hal_partitions[3].partition_start_addr       = 0x1000A800;
    hal_partitions[3].partition_length           = 0x400;    //1K bytes
    hal_partitions[3].partition_options          = PAR_OPT_READ_EN;

    hal_partitions[4].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_partitions[4].partition_description      = "IMAGES_MTB";
    hal_partitions[4].partition_start_addr       = 0x1000AC00;
    hal_partitions[4].partition_length           = 0x1000;    //4K bytes
    hal_partitions[4].partition_options          = PAR_OPT_READ_EN;

    hal_partitions[5].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_partitions[5].partition_description      = "REE";
    hal_partitions[5].partition_start_addr       = 0x1000BC00;
    hal_partitions[5].partition_length           = 0x25000;    //153600 bytes
    hal_partitions[5].partition_options          = PAR_OPT_READ_EN;

#ifdef WITH_SAL
    hal_partitions[6].partition_owner            = HAL_FLASH_EMBEDDED;
    hal_partitions[6].partition_description      = "KV";
    hal_partitions[6].partition_start_addr       = 0x1003D800;
    hal_partitions[6].partition_length           = 0x800;    //2K bytes
    hal_partitions[6].partition_options          = PAR_OPT_READ_EN | PAR_OPT_WRITE_EN;
#endif

    ioreuse_initial();

    cb2201_pinmux_config();

    /* init the console*/
    console_handle = csi_usart_initialize(CONSOLE_IDX, NULL);
    /* config the UART */
    csi_usart_config(console_handle, 115200, USART_MODE_ASYNCHRONOUS, USART_PARITY_NONE, USART_STOP_BITS_1, USART_DATA_BITS_8);
}
