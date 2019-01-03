/***************************************************************************//**
* \file SCB3_SPI_UART_INT.c
* \version 4.0
*
* \brief
*  This file provides the source code to the Interrupt Service Routine for
*  the SCB Component in SPI and UART modes.
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

#include "SCB3_PVT.h"
#include "SCB3_SPI_UART_PVT.h"
#include "cyapicallbacks.h"

#if (SCB3_SCB_IRQ_INTERNAL)
/*******************************************************************************
* Function Name: SCB3_SPI_UART_ISR
****************************************************************************//**
*
*  Handles the Interrupt Service Routine for the SCB SPI or UART modes.
*
*******************************************************************************/
CY_ISR(SCB3_SPI_UART_ISR)
{
#if (SCB3_INTERNAL_RX_SW_BUFFER_CONST)
    uint32 locHead;
#endif /* (SCB3_INTERNAL_RX_SW_BUFFER_CONST) */

#if (SCB3_INTERNAL_TX_SW_BUFFER_CONST)
    uint32 locTail;
#endif /* (SCB3_INTERNAL_TX_SW_BUFFER_CONST) */

#ifdef SCB3_SPI_UART_ISR_ENTRY_CALLBACK
    SCB3_SPI_UART_ISR_EntryCallback();
#endif /* SCB3_SPI_UART_ISR_ENTRY_CALLBACK */

    if (NULL != SCB3_customIntrHandler)
    {
        SCB3_customIntrHandler();
    }

    #if(SCB3_CHECK_SPI_WAKE_ENABLE)
    {
        /* Clear SPI wakeup source */
        SCB3_ClearSpiExtClkInterruptSource(SCB3_INTR_SPI_EC_WAKE_UP);
    }
    #endif

    #if (SCB3_CHECK_RX_SW_BUFFER)
    {
        if (SCB3_CHECK_INTR_RX_MASKED(SCB3_INTR_RX_NOT_EMPTY))
        {
            do
            {
                /* Move local head index */
                locHead = (SCB3_rxBufferHead + 1u);

                /* Adjust local head index */
                if (SCB3_INTERNAL_RX_BUFFER_SIZE == locHead)
                {
                    locHead = 0u;
                }

                if (locHead == SCB3_rxBufferTail)
                {
                    #if (SCB3_CHECK_UART_RTS_CONTROL_FLOW)
                    {
                        /* There is no space in the software buffer - disable the
                        * RX Not Empty interrupt source. The data elements are
                        * still being received into the RX FIFO until the RTS signal
                        * stops the transmitter. After the data element is read from the
                        * buffer, the RX Not Empty interrupt source is enabled to
                        * move the next data element in the software buffer.
                        */
                        SCB3_INTR_RX_MASK_REG &= ~SCB3_INTR_RX_NOT_EMPTY;
                        break;
                    }
                    #else
                    {
                        /* Overflow: through away received data element */
                        (void) SCB3_RX_FIFO_RD_REG;
                        SCB3_rxBufferOverflow = (uint8) SCB3_INTR_RX_OVERFLOW;
                    }
                    #endif
                }
                else
                {
                    /* Store received data */
                    SCB3_PutWordInRxBuffer(locHead, SCB3_RX_FIFO_RD_REG);

                    /* Move head index */
                    SCB3_rxBufferHead = locHead;
                }
            }
            while(0u != SCB3_GET_RX_FIFO_ENTRIES);

            SCB3_ClearRxInterruptSource(SCB3_INTR_RX_NOT_EMPTY);
        }
    }
    #endif


    #if (SCB3_CHECK_TX_SW_BUFFER)
    {
        if (SCB3_CHECK_INTR_TX_MASKED(SCB3_INTR_TX_NOT_FULL))
        {
            do
            {
                /* Check for room in TX software buffer */
                if (SCB3_txBufferHead != SCB3_txBufferTail)
                {
                    /* Move local tail index */
                    locTail = (SCB3_txBufferTail + 1u);

                    /* Adjust local tail index */
                    if (SCB3_TX_BUFFER_SIZE == locTail)
                    {
                        locTail = 0u;
                    }

                    /* Put data into TX FIFO */
                    SCB3_TX_FIFO_WR_REG = SCB3_GetWordFromTxBuffer(locTail);

                    /* Move tail index */
                    SCB3_txBufferTail = locTail;
                }
                else
                {
                    /* TX software buffer is empty: complete transfer */
                    SCB3_DISABLE_INTR_TX(SCB3_INTR_TX_NOT_FULL);
                    break;
                }
            }
            while (SCB3_SPI_UART_FIFO_SIZE != SCB3_GET_TX_FIFO_ENTRIES);

            SCB3_ClearTxInterruptSource(SCB3_INTR_TX_NOT_FULL);
        }
    }
    #endif

#ifdef SCB3_SPI_UART_ISR_EXIT_CALLBACK
    SCB3_SPI_UART_ISR_ExitCallback();
#endif /* SCB3_SPI_UART_ISR_EXIT_CALLBACK */

}

#endif /* (SCB3_SCB_IRQ_INTERNAL) */


/* [] END OF FILE */
