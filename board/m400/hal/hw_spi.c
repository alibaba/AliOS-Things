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
static uint32_t SpiFrequency( uint32_t hz );

/* Exported functions ---------------------------------------------------------*/

void HW_SPI_Init( void )
{
    /*##-1- Configure the SPI peripheral */
    /* Set the SPI parameters */
    LL_APB2_GRP1_EnableClock( LL_APB2_GRP1_PERIPH_SPI1 );

    LL_SPI_InitTypeDef SPI_InitStruct;
    SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
    SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
    SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
    SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
    SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
    SPI_InitStruct.BaudRate = SpiFrequency( 10000000 );
    SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
    SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    if ( LL_SPI_Init( SPI1, &SPI_InitStruct ) != SUCCESS )
    {
        /* Initialization Error */
        Error_Handler( );
    }

    /*##-2- Configure the SPI GPIOs */
    HW_SPI_IoInit( );
}

void HW_SPI_DeInit( void )
{

    LL_SPI_DeInit( SPI1 );
    /*##-1- Reset peripherals ####*/
    LL_APB2_GRP1_ForceReset( LL_APB2_GRP1_PERIPH_SPI1 );
    LL_APB2_GRP1_ReleaseReset( LL_APB2_GRP1_PERIPH_SPI1 );
    /*##-2- Configure the SPI GPIOs */
    HW_SPI_IoDeInit( );
}

void HW_SPI_IoInit( void )
{
    GPIO_InitTypeDef initStruct;

    initStruct.Mode = GPIO_MODE_AF_PP;
    initStruct.Pull = GPIO_PULLUP;//GPIO_PULLDOWN
    initStruct.Speed = GPIO_SPEED_HIGH;
    initStruct.Alternate = SPI_AF;

    HW_GPIO_Init( RADIO_SCLK_PORT, RADIO_SCLK_PIN, &initStruct );
    HW_GPIO_Init( RADIO_MISO_PORT, RADIO_MISO_PIN, &initStruct );
    HW_GPIO_Init( RADIO_MOSI_PORT, RADIO_MOSI_PIN, &initStruct );

    initStruct.Mode = GPIO_MODE_OUTPUT_PP;
    initStruct.Pull = GPIO_PULLUP;

    HW_GPIO_Init( RADIO_NSS_PORT, RADIO_NSS_PIN, &initStruct );

    HW_GPIO_Write( RADIO_NSS_PORT, RADIO_NSS_PIN, 1 );
}

void HW_SPI_IoDeInit( void )
{
    GPIO_InitTypeDef initStruct;

    initStruct.Mode = GPIO_MODE_OUTPUT_PP;
    initStruct.Pull = GPIO_NOPULL;
    initStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HW_GPIO_Init( RADIO_MOSI_PORT, RADIO_MOSI_PIN, &initStruct );
    HW_GPIO_Write( RADIO_MOSI_PORT, RADIO_MOSI_PIN, 0 );

    initStruct.Pull = GPIO_PULLDOWN;
    HW_GPIO_Init( RADIO_MISO_PORT, RADIO_MISO_PIN, &initStruct );
    HW_GPIO_Write( RADIO_MISO_PORT, RADIO_MISO_PIN, 0 );

    initStruct.Pull = GPIO_NOPULL;
    HW_GPIO_Init( RADIO_SCLK_PORT, RADIO_SCLK_PIN, &initStruct );
    HW_GPIO_Write( RADIO_SCLK_PORT, RADIO_SCLK_PIN, 0 );

    initStruct.Pull = GPIO_PULLUP;
    HW_GPIO_Init( RADIO_NSS_PORT, RADIO_NSS_PIN, &initStruct );
    HW_GPIO_Write( RADIO_NSS_PORT, RADIO_NSS_PIN, 1 );

    initStruct.Mode = GPIO_MODE_ANALOG;
    initStruct.Pull = GPIO_NOPULL;
    HW_GPIO_Init( RADIO_RESET_PORT, RADIO_RESET_PIN, &initStruct);
}

uint16_t HW_SPI_InOut( uint16_t txData )
{
    uint16_t rx_data;
    uint32_t tick;

    /* Check if the SPI is already enabled. Enable otherwise */
    if ( LL_SPI_IsEnabled( SPI1 ) == RESET )
    {
        LL_SPI_Enable( SPI1 );
    }

    /* Transmit data */
    LL_SPI_TransmitData8( SPI1, txData );

    tick = HAL_GetTick();
    /* Wait until RXNE flag is set */
    while ( LL_SPI_IsActiveFlag_RXNE( SPI1 ) == RESET )
    {
        if (HAL_GetTick() - tick > 100)
        {
            break;
        }
    }

    /* Receive data */
    rx_data = LL_SPI_ReceiveData8( SPI1 );

    tick = HAL_GetTick();
    /* Wait until Busy flag is reset before disabling SPI */
    while ( LL_SPI_IsActiveFlag_BSY( SPI1 ) != RESET )
    {
        if (HAL_GetTick() - tick > 100)
        {
            break;
        }
    }

    return rx_data;
}

/* Private functions ---------------------------------------------------------*/

static uint32_t SpiFrequency( uint32_t hz )
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

    return baudRate;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
