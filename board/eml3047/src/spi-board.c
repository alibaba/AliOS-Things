/*
 / _____)             _              | |
 ( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
 (______/|_____)_|_|_| \__)_____)\____)_| |_|
 (C)2013 Semtech

 Description: Bleeper board SPI driver implementation

 License: Revised BSD License, see LICENSE.TXT file include in the project

 Maintainer: Miguel Luis and Gregory Cristian
 */
/*******************************************************************************
 * @file    hw_spi.c
 * @author  MCD Application Team
 * @version V1.1.1
 * @date    01-June-2017
 * @brief   manages the SPI interface
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
 * All rights reserved.</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted, provided that the following conditions are met:
 *
 * 1. Redistribution of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of other
 *    contributors to this software may be used to endorse or promote products
 *    derived from this software without specific written permission.
 * 4. This software, including modifications and/or derivative works of this
 *    software, must execute solely and exclusively on microcontroller or
 *    microprocessor devices manufactured by or for STMicroelectronics.
 * 5. Redistribution and use of this software other than as permitted under
 *    this license is void and will automatically terminate your rights under
 *    this license.
 *
 * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
 * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
 * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "hw.h"
#include "utilities.h"
#include "spi.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/**
 * @brief Calculates Spi Divisor based on Spi Frequency and Mcu Frequency
 *
 * @param [IN] Spi Frequency
 * @retval Spi divisor
 */
LL_SPI_InitTypeDef  SpiHandle[2];

/* Exported functions ---------------------------------------------------------*/

void SpiInit( Spi_t *obj, SpiId_t spiId, PinNames mosi, PinNames miso, PinNames sclk, PinNames nss )
{
    CRITICAL_SECTION_BEGIN( );

    obj->SpiId = spiId;

    LL_APB2_GRP1_EnableClock( LL_APB2_GRP1_PERIPH_SPI1 );
    GpioInit(&obj->Sclk, sclk, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0);
    GpioInit(&obj->Miso, miso, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0);
    GpioInit(&obj->Mosi, mosi, PIN_ALTERNATE_FCT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0);
    GpioInit(&obj->Nss, nss, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1);

    SpiFormat( obj, SPI_DATASIZE_8BIT, SPI_POLARITY_LOW, SPI_PHASE_1EDGE, 0 );

    SpiFrequency( obj, 10000000 );

    LL_SPI_Init(SPI1,  &SpiHandle[spiId] );

    CRITICAL_SECTION_END( );
}

void SpiDeInit( Spi_t *obj )
{

    LL_SPI_DeInit( SPI1 );
    /*##-1- Reset peripherals ####*/
    LL_APB2_GRP1_ForceReset( LL_APB2_GRP1_PERIPH_SPI1 );
    LL_APB2_GRP1_ReleaseReset( LL_APB2_GRP1_PERIPH_SPI1 );
    /*##-2- Configure the SPI GPIOs */
    HW_SPI_IoDeInit( );
}

void SpiFormat( Spi_t *obj, int8_t bits, int8_t cpol, int8_t cpha, int8_t slave )
{
    SpiHandle[obj->SpiId].TransferDirection = LL_SPI_FULL_DUPLEX;
    if( bits == SPI_DATASIZE_8BIT )
    {
        SpiHandle[obj->SpiId].DataWidth = LL_SPI_DATAWIDTH_8BIT;
    }
    else
    {
        SpiHandle[obj->SpiId].DataWidth = LL_SPI_DATAWIDTH_16BIT;
    }
    SpiHandle[obj->SpiId].ClockPolarity = cpol;
    SpiHandle[obj->SpiId].ClockPhase = cpha;
    SpiHandle[obj->SpiId].BitOrder = LL_SPI_MSB_FIRST;
    SpiHandle[obj->SpiId].CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    SpiHandle[obj->SpiId].NSS = LL_SPI_NSS_SOFT;

    SpiFrequency(obj, 10000000);

    if( slave == 0 )
    {
        SpiHandle[obj->SpiId].Mode = LL_SPI_MODE_MASTER;
    }
    else
    {
        SpiHandle[obj->SpiId].Mode = LL_SPI_MODE_SLAVE;
    }
}

void SpiFrequency( Spi_t *obj, uint32_t hz )
{
    uint32_t divisor = 0;
    uint32_t SysClkTmp = SystemCoreClock;
    uint32_t baudRate;

    while ( SysClkTmp > hz )
    {
        divisor++;
        SysClkTmp = (SysClkTmp >> 1);

        if ( divisor >= 7 )
        {
            break;
        }
    }

    assert_param( ((SPI_CR1_BR_0 == (0x1U << SPI_CR1_BR_Pos)) &&
                  (SPI_CR1_BR_1 == (0x2U << SPI_CR1_BR_Pos)) &&
                  (SPI_CR1_BR_2 == (0x4U << SPI_CR1_BR_Pos))) );
    baudRate = divisor << SPI_CR1_BR_Pos;

    return SpiHandle[obj->SpiId].BaudRate = baudRate;
}

uint16_t SpiInOut( Spi_t *obj, uint16_t outData )
{
    uint16_t rx_data;

    /* Check if the SPI is already enabled. Enable otherwise */
    if ( LL_SPI_IsEnabled( SPI1 ) == RESET )
    {
        LL_SPI_Enable( SPI1 );
    }

    /* Transmit data */
    LL_SPI_TransmitData8( SPI1, outData );

    /* Wait until RXNE flag is set */
    while ( LL_SPI_IsActiveFlag_RXNE( SPI1 ) == RESET )
    {
        ;
    }

    /* Receive data */
    rx_data = LL_SPI_ReceiveData8( SPI1 );

    /* Wait until Busy flag is reset before disabling SPI */
    while ( LL_SPI_IsActiveFlag_BSY( SPI1 ) != RESET )
    {
        ;
    }

    return rx_data;
}

