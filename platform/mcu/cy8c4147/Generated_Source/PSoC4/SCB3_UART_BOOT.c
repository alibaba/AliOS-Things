/***************************************************************************//**
* \file SCB3_UART_BOOT.c
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

#include "SCB3_BOOT.h"
#include "SCB3_SPI_UART.h"

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (SCB3_UART_BTLDR_COMM_ENABLED)

/*******************************************************************************
* Function Name: SCB3_UartCyBtldrCommStart
****************************************************************************//**
*
*  Starts the UART component.
*
*******************************************************************************/
void SCB3_UartCyBtldrCommStart(void)
{
    SCB3_Start();
}


/*******************************************************************************
* Function Name: SCB3_UartCyBtldrCommStop
****************************************************************************//**
*
*  Disables the UART component.
*
*******************************************************************************/
void SCB3_UartCyBtldrCommStop(void)
{
    SCB3_Stop();
}


/*******************************************************************************
* Function Name: SCB3_UartCyBtldrCommReset
****************************************************************************//**
*
*  Resets the receive and transmit communication buffers.
*
*******************************************************************************/
void SCB3_UartCyBtldrCommReset(void)
{
    /* Clear RX and TX buffers */
    SCB3_SpiUartClearRxBuffer();
    SCB3_SpiUartClearTxBuffer();
}


/*******************************************************************************
* Function Name: SCB3_UartCyBtldrCommRead
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
cystatus SCB3_UartCyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut)
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
            if (0u != SCB3_SpiUartGetRxBufferSize())
            {
                /* Wait for end of packet */
                do
                {
                    byteCount = SCB3_SpiUartGetRxBufferSize();
                    CyDelayUs(SCB3_UART_BYTE_TO_BYTE);
                }
                while (byteCount != SCB3_SpiUartGetRxBufferSize());

                byteCount = SCB3_BYTES_TO_COPY(byteCount, size);
                *count = (uint16) byteCount;
                status = CYRET_SUCCESS;

                break;
            }

            CyDelay(SCB3_WAIT_1_MS);
            --timeoutMs;
        }
        while (0u != timeoutMs);

        /* Get data from RX buffer into bootloader buffer */
        for (i = 0u; i < byteCount; ++i)
        {
            pData[i] = (uint8) SCB3_SpiUartReadRxData();
        }
    }

    return (status);
}


/*******************************************************************************
* Function Name: SCB3_UartCyBtldrCommWrite
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
cystatus SCB3_UartCyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut)
{
    cystatus status;

    status = CYRET_BAD_PARAM;

    if ((NULL != pData) && (size > 0u))
    {
        /* Transmit data. This function does not wait until data is sent. */
        SCB3_SpiUartPutArray(pData, (uint32) size);

        *count = size;
        status = CYRET_SUCCESS;

        if (0u != timeOut)
        {
            /* Suppress compiler warning */
        }
    }

    return (status);
}

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (SCB3_UART_BTLDR_COMM_ENABLED) */


/* [] END OF FILE */
