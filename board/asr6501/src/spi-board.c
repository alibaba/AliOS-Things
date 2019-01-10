/*!
 * \file      spi-board.c
 *
 * \brief     Target board SPI driver implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#include <project.h>
#include "utilities.h"
#include "board.h"
#include "debug.h"
#include "gpio.h"
#include "spi-board.h"

//#include "asr_project.h"

void SpiInit( )
{
	SPI_1_Start();
}

void SpiDeInit( Spi_t *obj )
{
	SPI_1_Stop();
}

void SpiFormat( Spi_t *obj, int8_t bits, int8_t cpol, int8_t cpha, int8_t slave )
{
	
}

void SpiFrequency( Spi_t *obj, uint32_t hz )
{
}

#define SPI_RX_TIMEOUT	500
uint16_t SpiInOut( Spi_t *obj, uint16_t outData )
{
	uint32 data;
	uint32 timeout = 0;
	
	while(SPI_1_SpiUartGetTxBufferSize() != 0);
	SPI_1_SpiUartWriteTxData(outData);
//	trace("Send Data 0x%x...\n", outData);
	
	while(SPI_1_SpiUartGetRxBufferSize() == 0)
	{
		timeout++;
		if(timeout > SPI_RX_TIMEOUT )break;
	}
	if(timeout > SPI_RX_TIMEOUT )
		DBG_PRINTF("Receive Data timeout!.\r\n");
	else
	{
		data = SPI_1_SpiUartReadRxData();
//		trace("Receive Data:0x%x.\n", data);
        return data;
	}
    
    return -1;
}
