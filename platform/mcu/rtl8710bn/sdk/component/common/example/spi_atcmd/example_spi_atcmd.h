/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/

#ifndef __EXAMPLE_SPI_ATCMD_H__
#define __EXAMPLE_SPI_ATCMD_H__

#if CONFIG_EXAMPLE_SPI_ATCMD

#define SPI0_MOSI  PC_2
#define SPI0_MISO  PC_3
#define SPI0_SCLK  PC_1
#define SPI0_CS    PC_0

#define GPIO_CS    PA_3
#define GPIO_HRDY  PA_1
#define GPIO_SYNC  PB_3

void example_spi_atcmd(void);

#endif // #if CONFIG_EXAMPLE_SPI_ATCMD

#endif // #ifndef __EXAMPLE_SPI_ATCMD_H__
