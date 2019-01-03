/***************************************************************************//**
* \file SCB3_SPI_UART.c
* \version 4.0
*
* \brief
*  This file provides the source code to the API for the SCB Component in
*  SPI and UART modes.
*
* Note:
*
*******************************************************************************
* \copyright
* Copyright 2013-2017, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "SCB3_PVT.h"
#include "SCB3_SPI_UART_PVT.h"

/***************************************
*        SPI/UART Private Vars
***************************************/

#if(SCB3_INTERNAL_RX_SW_BUFFER_CONST)
    /* Start index to put data into the software receive buffer.*/
    volatile uint32 SCB3_rxBufferHead;
    /* Start index to get data from the software receive buffer.*/
    volatile uint32 SCB3_rxBufferTail;
    /**
    * \addtogroup group_globals
    * \{
    */
    /** Sets when internal software receive buffer overflow
    *  was occurred.
    */
    volatile uint8  SCB3_rxBufferOverflow;
    /** \} globals */
#endif /* (SCB3_INTERNAL_RX_SW_BUFFER_CONST) */

#if(SCB3_INTERNAL_TX_SW_BUFFER_CONST)
    /* Start index to put data into the software transmit buffer.*/
    volatile uint32 SCB3_txBufferHead;
    /* Start index to get data from the software transmit buffer.*/
    volatile uint32 SCB3_txBufferTail;
#endif /* (SCB3_INTERNAL_TX_SW_BUFFER_CONST) */

#if(SCB3_INTERNAL_RX_SW_BUFFER)
    /* Add one element to the buffer to receive full packet. One byte in receive buffer is always empty */
    volatile uint8 SCB3_rxBufferInternal[SCB3_INTERNAL_RX_BUFFER_SIZE];
#endif /* (SCB3_INTERNAL_RX_SW_BUFFER) */

#if(SCB3_INTERNAL_TX_SW_BUFFER)
    volatile uint8 SCB3_txBufferInternal[SCB3_TX_BUFFER_SIZE];
#endif /* (SCB3_INTERNAL_TX_SW_BUFFER) */


#if(SCB3_RX_DIRECTION)
    /*******************************************************************************
    * Function Name: SCB3_SpiUartReadRxData
    ****************************************************************************//**
    *
    *  Retrieves the next data element from the receive buffer.
    *   - RX software buffer is disabled: Returns data element retrieved from
    *     RX FIFO. Undefined data will be returned if the RX FIFO is empty.
    *   - RX software buffer is enabled: Returns data element from the software
    *     receive buffer. Zero value is returned if the software receive buffer
    *     is empty.
    *
    * \return
    *  Next data element from the receive buffer. 
    *  The amount of data bits to be received depends on RX data bits selection 
    *  (the data bit counting starts from LSB of return value).
    *
    * \globalvars
    *  SCB3_rxBufferHead - the start index to put data into the 
    *  software receive buffer.
    *  SCB3_rxBufferTail - the start index to get data from the 
    *  software receive buffer.
    *
    *******************************************************************************/
    uint32 SCB3_SpiUartReadRxData(void)
    {
        uint32 rxData = 0u;

    #if (SCB3_INTERNAL_RX_SW_BUFFER_CONST)
        uint32 locTail;
    #endif /* (SCB3_INTERNAL_RX_SW_BUFFER_CONST) */

        #if (SCB3_CHECK_RX_SW_BUFFER)
        {
            if (SCB3_rxBufferHead != SCB3_rxBufferTail)
            {
                /* There is data in RX software buffer */

                /* Calculate index to read from */
                locTail = (SCB3_rxBufferTail + 1u);

                if (SCB3_INTERNAL_RX_BUFFER_SIZE == locTail)
                {
                    locTail = 0u;
                }

                /* Get data from RX software buffer */
                rxData = SCB3_GetWordFromRxBuffer(locTail);

                /* Change index in the buffer */
                SCB3_rxBufferTail = locTail;

                #if (SCB3_CHECK_UART_RTS_CONTROL_FLOW)
                {
                    /* Check if RX Not Empty is disabled in the interrupt */
                    if (0u == (SCB3_INTR_RX_MASK_REG & SCB3_INTR_RX_NOT_EMPTY))
                    {
                        /* Enable RX Not Empty interrupt source to continue
                        * receiving data into software buffer.
                        */
                        SCB3_INTR_RX_MASK_REG |= SCB3_INTR_RX_NOT_EMPTY;
                    }
                }
                #endif

            }
        }
        #else
        {
            /* Read data from RX FIFO */
            rxData = SCB3_RX_FIFO_RD_REG;
        }
        #endif

        return (rxData);
    }


    /*******************************************************************************
    * Function Name: SCB3_SpiUartGetRxBufferSize
    ****************************************************************************//**
    *
    *  Returns the number of received data elements in the receive buffer.
    *   - RX software buffer disabled: returns the number of used entries in
    *     RX FIFO.
    *   - RX software buffer enabled: returns the number of elements which were
    *     placed in the receive buffer. This does not include the hardware RX FIFO.
    *
    * \return
    *  Number of received data elements.
    *
    * \globalvars
    *  SCB3_rxBufferHead - the start index to put data into the 
    *  software receive buffer.
    *  SCB3_rxBufferTail - the start index to get data from the 
    *  software receive buffer.
    *
    *******************************************************************************/
    uint32 SCB3_SpiUartGetRxBufferSize(void)
    {
        uint32 size;
    #if (SCB3_INTERNAL_RX_SW_BUFFER_CONST)
        uint32 locHead;
    #endif /* (SCB3_INTERNAL_RX_SW_BUFFER_CONST) */

        #if (SCB3_CHECK_RX_SW_BUFFER)
        {
            locHead = SCB3_rxBufferHead;

            if(locHead >= SCB3_rxBufferTail)
            {
                size = (locHead - SCB3_rxBufferTail);
            }
            else
            {
                size = (locHead + (SCB3_INTERNAL_RX_BUFFER_SIZE - SCB3_rxBufferTail));
            }
        }
        #else
        {
            size = SCB3_GET_RX_FIFO_ENTRIES;
        }
        #endif

        return (size);
    }


    /*******************************************************************************
    * Function Name: SCB3_SpiUartClearRxBuffer
    ****************************************************************************//**
    *
    *  Clears the receive buffer and RX FIFO.
    *
    * \globalvars
    *  SCB3_rxBufferHead - the start index to put data into the 
    *  software receive buffer.
    *  SCB3_rxBufferTail - the start index to get data from the 
    *  software receive buffer.
    *
    *******************************************************************************/
    void SCB3_SpiUartClearRxBuffer(void)
    {
        #if (SCB3_CHECK_RX_SW_BUFFER)
        {
            /* Lock from component interruption */
            SCB3_DisableInt();

            /* Flush RX software buffer */
            SCB3_rxBufferHead = SCB3_rxBufferTail;
            SCB3_rxBufferOverflow = 0u;

            SCB3_CLEAR_RX_FIFO;
            SCB3_ClearRxInterruptSource(SCB3_INTR_RX_ALL);

            #if (SCB3_CHECK_UART_RTS_CONTROL_FLOW)
            {
                /* Enable RX Not Empty interrupt source to continue receiving
                * data into software buffer.
                */
                SCB3_INTR_RX_MASK_REG |= SCB3_INTR_RX_NOT_EMPTY;
            }
            #endif
            
            /* Release lock */
            SCB3_EnableInt();
        }
        #else
        {
            SCB3_CLEAR_RX_FIFO;
        }
        #endif
    }

#endif /* (SCB3_RX_DIRECTION) */


#if(SCB3_TX_DIRECTION)
    /*******************************************************************************
    * Function Name: SCB3_SpiUartWriteTxData
    ****************************************************************************//**
    *
    *  Places a data entry into the transmit buffer to be sent at the next available
    *  bus time.
    *  This function is blocking and waits until there is space available to put the
    *  requested data in the transmit buffer.
    *
    *  \param txDataByte: the data to be transmitted.
    *   The amount of data bits to be transmitted depends on TX data bits selection 
    *   (the data bit counting starts from LSB of txDataByte).
    *
    * \globalvars
    *  SCB3_txBufferHead - the start index to put data into the 
    *  software transmit buffer.
    *  SCB3_txBufferTail - start index to get data from the software
    *  transmit buffer.
    *
    *******************************************************************************/
    void SCB3_SpiUartWriteTxData(uint32 txData)
    {
    #if (SCB3_INTERNAL_TX_SW_BUFFER_CONST)
        uint32 locHead;
    #endif /* (SCB3_INTERNAL_TX_SW_BUFFER_CONST) */

        #if (SCB3_CHECK_TX_SW_BUFFER)
        {
            /* Put data directly into the TX FIFO */
            if ((SCB3_txBufferHead == SCB3_txBufferTail) &&
                (SCB3_SPI_UART_FIFO_SIZE != SCB3_GET_TX_FIFO_ENTRIES))
            {
                /* TX software buffer is empty: put data directly in TX FIFO */
                SCB3_TX_FIFO_WR_REG = txData;
            }
            /* Put data into TX software buffer */
            else
            {
                /* Head index to put data */
                locHead = (SCB3_txBufferHead + 1u);

                /* Adjust TX software buffer index */
                if (SCB3_TX_BUFFER_SIZE == locHead)
                {
                    locHead = 0u;
                }

                /* Wait for space in TX software buffer */
                while (locHead == SCB3_txBufferTail)
                {
                }

                /* TX software buffer has at least one room */

                /* Clear old status of INTR_TX_NOT_FULL. It sets at the end of transfer when TX FIFO is empty. */
                SCB3_ClearTxInterruptSource(SCB3_INTR_TX_NOT_FULL);

                SCB3_PutWordInTxBuffer(locHead, txData);

                SCB3_txBufferHead = locHead;

                /* Check if TX Not Full is disabled in interrupt */
                if (0u == (SCB3_INTR_TX_MASK_REG & SCB3_INTR_TX_NOT_FULL))
                {
                    /* Enable TX Not Full interrupt source to transmit from software buffer */
                    SCB3_INTR_TX_MASK_REG |= (uint32) SCB3_INTR_TX_NOT_FULL;
                }
            }
        }
        #else
        {
            /* Wait until TX FIFO has space to put data element */
            while (SCB3_SPI_UART_FIFO_SIZE == SCB3_GET_TX_FIFO_ENTRIES)
            {
            }

            SCB3_TX_FIFO_WR_REG = txData;
        }
        #endif
    }


    /*******************************************************************************
    * Function Name: SCB3_SpiUartPutArray
    ****************************************************************************//**
    *
    *  Places an array of data into the transmit buffer to be sent.
    *  This function is blocking and waits until there is a space available to put
    *  all the requested data in the transmit buffer. The array size can be greater
    *  than transmit buffer size.
    *
    * \param wrBuf: pointer to an array of data to be placed in transmit buffer. 
    *  The width of the data to be transmitted depends on TX data width selection 
    *  (the data bit counting starts from LSB for each array element).
    * \param count: number of data elements to be placed in the transmit buffer.
    *
    * \globalvars
    *  SCB3_txBufferHead - the start index to put data into the 
    *  software transmit buffer.
    *  SCB3_txBufferTail - start index to get data from the software
    *  transmit buffer.
    *
    *******************************************************************************/
    void SCB3_SpiUartPutArray(const uint8 wrBuf[], uint32 count)
    {
        uint32 i;

        for (i=0u; i < count; i++)
        {
            SCB3_SpiUartWriteTxData((uint32) wrBuf[i]);
        }
    }


    /*******************************************************************************
    * Function Name: SCB3_SpiUartGetTxBufferSize
    ****************************************************************************//**
    *
    *  Returns the number of elements currently in the transmit buffer.
    *   - TX software buffer is disabled: returns the number of used entries in
    *     TX FIFO.
    *   - TX software buffer is enabled: returns the number of elements currently
    *     used in the transmit buffer. This number does not include used entries in
    *     the TX FIFO. The transmit buffer size is zero until the TX FIFO is
    *     not full.
    *
    * \return
    *  Number of data elements ready to transmit.
    *
    * \globalvars
    *  SCB3_txBufferHead - the start index to put data into the 
    *  software transmit buffer.
    *  SCB3_txBufferTail - start index to get data from the software
    *  transmit buffer.
    *
    *******************************************************************************/
    uint32 SCB3_SpiUartGetTxBufferSize(void)
    {
        uint32 size;
    #if (SCB3_INTERNAL_TX_SW_BUFFER_CONST)
        uint32 locTail;
    #endif /* (SCB3_INTERNAL_TX_SW_BUFFER_CONST) */

        #if (SCB3_CHECK_TX_SW_BUFFER)
        {
            /* Get current Tail index */
            locTail = SCB3_txBufferTail;

            if (SCB3_txBufferHead >= locTail)
            {
                size = (SCB3_txBufferHead - locTail);
            }
            else
            {
                size = (SCB3_txBufferHead + (SCB3_TX_BUFFER_SIZE - locTail));
            }
        }
        #else
        {
            size = SCB3_GET_TX_FIFO_ENTRIES;
        }
        #endif

        return (size);
    }


    /*******************************************************************************
    * Function Name: SCB3_SpiUartClearTxBuffer
    ****************************************************************************//**
    *
    *  Clears the transmit buffer and TX FIFO.
    *
    * \globalvars
    *  SCB3_txBufferHead - the start index to put data into the 
    *  software transmit buffer.
    *  SCB3_txBufferTail - start index to get data from the software
    *  transmit buffer.
    *
    *******************************************************************************/
    void SCB3_SpiUartClearTxBuffer(void)
    {
        #if (SCB3_CHECK_TX_SW_BUFFER)
        {
            /* Lock from component interruption */
            SCB3_DisableInt();

            /* Flush TX software buffer */
            SCB3_txBufferHead = SCB3_txBufferTail;

            SCB3_INTR_TX_MASK_REG &= (uint32) ~SCB3_INTR_TX_NOT_FULL;
            SCB3_CLEAR_TX_FIFO;
            SCB3_ClearTxInterruptSource(SCB3_INTR_TX_ALL);

            /* Release lock */
            SCB3_EnableInt();
        }
        #else
        {
            SCB3_CLEAR_TX_FIFO;
        }
        #endif
    }

#endif /* (SCB3_TX_DIRECTION) */


/*******************************************************************************
* Function Name: SCB3_SpiUartDisableIntRx
****************************************************************************//**
*
*  Disables the RX interrupt sources.
*
*  \return
*   Returns the RX interrupt sources enabled before the function call.
*
*******************************************************************************/
uint32 SCB3_SpiUartDisableIntRx(void)
{
    uint32 intSource;

    intSource = SCB3_GetRxInterruptMode();

    SCB3_SetRxInterruptMode(SCB3_NO_INTR_SOURCES);

    return (intSource);
}


/*******************************************************************************
* Function Name: SCB3_SpiUartDisableIntTx
****************************************************************************//**
*
*  Disables TX interrupt sources.
*
*  \return
*   Returns TX interrupt sources enabled before function call.
*
*******************************************************************************/
uint32 SCB3_SpiUartDisableIntTx(void)
{
    uint32 intSourceMask;

    intSourceMask = SCB3_GetTxInterruptMode();

    SCB3_SetTxInterruptMode(SCB3_NO_INTR_SOURCES);

    return (intSourceMask);
}


#if(SCB3_SCB_MODE_UNCONFIG_CONST_CFG)
    /*******************************************************************************
    * Function Name: SCB3_PutWordInRxBuffer
    ****************************************************************************//**
    *
    *  Stores a byte/word into the RX buffer.
    *  Only available in the Unconfigured operation mode.
    *
    *  \param index:      index to store data byte/word in the RX buffer.
    *  \param rxDataByte: byte/word to store.
    *
    *******************************************************************************/
    void SCB3_PutWordInRxBuffer(uint32 idx, uint32 rxDataByte)
    {
        /* Put data in buffer */
        if (SCB3_ONE_BYTE_WIDTH == SCB3_rxDataBits)
        {
            SCB3_rxBuffer[idx] = ((uint8) rxDataByte);
        }
        else
        {
            SCB3_rxBuffer[(uint32)(idx << 1u)]      = LO8(LO16(rxDataByte));
            SCB3_rxBuffer[(uint32)(idx << 1u) + 1u] = HI8(LO16(rxDataByte));
        }
    }


    /*******************************************************************************
    * Function Name: SCB3_GetWordFromRxBuffer
    ****************************************************************************//**
    *
    *  Reads byte/word from RX buffer.
    *  Only available in the Unconfigured operation mode.
    *
    *  \return
    *   Returns byte/word read from RX buffer.
    *
    *******************************************************************************/
    uint32 SCB3_GetWordFromRxBuffer(uint32 idx)
    {
        uint32 value;

        if (SCB3_ONE_BYTE_WIDTH == SCB3_rxDataBits)
        {
            value = SCB3_rxBuffer[idx];
        }
        else
        {
            value  = (uint32) SCB3_rxBuffer[(uint32)(idx << 1u)];
            value |= (uint32) ((uint32)SCB3_rxBuffer[(uint32)(idx << 1u) + 1u] << 8u);
        }

        return (value);
    }


    /*******************************************************************************
    * Function Name: SCB3_PutWordInTxBuffer
    ****************************************************************************//**
    *
    *  Stores byte/word into the TX buffer.
    *  Only available in the Unconfigured operation mode.
    *
    *  \param idx:        index to store data byte/word in the TX buffer.
    *  \param txDataByte: byte/word to store.
    *
    *******************************************************************************/
    void SCB3_PutWordInTxBuffer(uint32 idx, uint32 txDataByte)
    {
        /* Put data in buffer */
        if (SCB3_ONE_BYTE_WIDTH == SCB3_txDataBits)
        {
            SCB3_txBuffer[idx] = ((uint8) txDataByte);
        }
        else
        {
            SCB3_txBuffer[(uint32)(idx << 1u)]      = LO8(LO16(txDataByte));
            SCB3_txBuffer[(uint32)(idx << 1u) + 1u] = HI8(LO16(txDataByte));
        }
    }


    /*******************************************************************************
    * Function Name: SCB3_GetWordFromTxBuffer
    ****************************************************************************//**
    *
    *  Reads byte/word from the TX buffer.
    *  Only available in the Unconfigured operation mode.
    *
    *  \param idx: index to get data byte/word from the TX buffer.
    *
    *  \return
    *   Returns byte/word read from the TX buffer.
    *
    *******************************************************************************/
    uint32 SCB3_GetWordFromTxBuffer(uint32 idx)
    {
        uint32 value;

        if (SCB3_ONE_BYTE_WIDTH == SCB3_txDataBits)
        {
            value = (uint32) SCB3_txBuffer[idx];
        }
        else
        {
            value  = (uint32) SCB3_txBuffer[(uint32)(idx << 1u)];
            value |= (uint32) ((uint32) SCB3_txBuffer[(uint32)(idx << 1u) + 1u] << 8u);
        }

        return (value);
    }

#endif /* (SCB3_SCB_MODE_UNCONFIG_CONST_CFG) */


/* [] END OF FILE */
