/***************************************************************************//**
* \file UART_1_UART_BOOT.c
* \version 4.0
*
* \brief
*  This file provides the source code of the bootloader communication APIs
*  for the SCB Component UART mode.
*
* Note:
*
********************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "UART_1_BOOT.h"
#include "UART_1_SPI_UART.h"

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (UART_1_UART_BTLDR_COMM_ENABLED)

/*******************************************************************************
* Function Name: UART_1_UartCyBtldrCommStart
****************************************************************************//**
*
*  Starts the UART component.
*
*******************************************************************************/
void UART_1_UartCyBtldrCommStart(void)
{
    UART_1_Start();
}


/*******************************************************************************
* Function Name: UART_1_UartCyBtldrCommStop
****************************************************************************//**
*
*  Disables the UART component.
*
*******************************************************************************/
void UART_1_UartCyBtldrCommStop(void)
{
    UART_1_Stop();
}


/*******************************************************************************
* Function Name: UART_1_UartCyBtldrCommReset
****************************************************************************//**
*
*  Resets the receive and transmit communication buffers.
*
*******************************************************************************/
void UART_1_UartCyBtldrCommReset(void)
{
    /* Clear RX and TX buffers */
    UART_1_SpiUartClearRxBuffer();
    UART_1_SpiUartClearTxBuffer();
}


/*******************************************************************************
* Function Name: UART_1_UartCyBtldrCommRead
****************************************************************************//**
*
*  Allows the caller to read data from the bootloader host (the host writes the
*  data). The function handles polling to allow a block of data to be completely
*  received from the host device.
*
*  \param pData: Pointer to storage for the block of data to be read from the
*   bootloader host
*  \param size: Number of bytes to be read.
*  \param count: Pointer to the variable to write the number of bytes actually
*   read.
*  \param timeOut Number of units in 10 ms to wait before returning
*   because of a timeout.
*
*  \return
*   Returns CYRET_SUCCESS if no problem was encountered or returns the value
*   that best describes the problem. For more information refer to the
*   "Return Codes" section of the System Reference Guide.
*
*******************************************************************************/
cystatus UART_1_UartCyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut)
{
    cystatus status;
    uint32 byteCount;
    uint32 timeoutMs;
    uint32 i;

    status = CYRET_BAD_PARAM;

    if ((NULL != pData) && (size > 0u))
    {
        status = CYRET_TIMEOUT;
        timeoutMs = ((uint32) 10u * timeOut); /* Convert from 10mS check to 1mS checks */

        /* Wait with timeout 1mS for packet end */
        byteCount = 0u;
        do
        {
            /* Check packet start */
            if (0u != UART_1_SpiUartGetRxBufferSize())
            {
                /* Wait for end of packet */
                do
                {
                    byteCount = UART_1_SpiUartGetRxBufferSize();
                    CyDelayUs(UART_1_UART_BYTE_TO_BYTE);
                }
                while (byteCount != UART_1_SpiUartGetRxBufferSize());

                byteCount = UART_1_BYTES_TO_COPY(byteCount, size);
                *count = (uint16) byteCount;
                status = CYRET_SUCCESS;

                break;
            }

            CyDelay(UART_1_WAIT_1_MS);
            --timeoutMs;
        }
        while (0u != timeoutMs);

        /* Get data from RX buffer into bootloader buffer */
        for (i = 0u; i < byteCount; ++i)
        {
            pData[i] = (uint8) UART_1_SpiUartReadRxData();
        }
    }

    return (status);
}


/*******************************************************************************
* Function Name: UART_1_UartCyBtldrCommWrite
****************************************************************************//**
*
*  Allows the caller to write data to the bootloader host (the host reads the
*  data). The function does not use timeout and returns after data has been
*  copied into the transmit buffer. The data transmission starts immediately
*  after the first data element is written into the buffer and lasts until all
*  data elements from the buffer are sent.
*
*  \param pData: Pointer to the block of data to be written to the bootloader
*   host.
*  \param size: Number of bytes to be written.
*  \param count: Pointer to the variable to write the number of bytes actually
*   written.
*  \param timeOut: The timeout is not used by this function.
*   The function returns as soon as data is copied into the transmit buffer.
*
*  \return
*   Returns CYRET_SUCCESS if no problem was encountered or returns the value
*   that best describes the problem. For more information refer to the
*   "Return Codes" section of the System Reference Guide.
*
*******************************************************************************/
cystatus UART_1_UartCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut)
{
    cystatus status;

    status = CYRET_BAD_PARAM;

    if ((NULL != pData) && (size > 0u))
    {
        /* Transmit data. This function does not wait until data is sent. */
        UART_1_SpiUartPutArray(pData, (uint32) size);

        *count = size;
        status = CYRET_SUCCESS;

        if (0u != timeOut)
        {
            /* Suppress compiler warning */
        }
    }

    return (status);
}

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (UART_1_UART_BTLDR_COMM_ENABLED) */


/* [] END OF FILE */
