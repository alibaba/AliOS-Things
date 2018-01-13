/*
 * Copyright (c) 2017, NXP Semiconductors, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_usart.h"

enum _usart_transfer_states
{
    kUSART_TxIdle, /* TX idle. */
    kUSART_TxBusy, /* TX busy. */
    kUSART_RxIdle, /* RX idle. */
    kUSART_RxBusy  /* RX busy. */
};

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Array of USART handle. */
static usart_handle_t *s_usartHandle[FSL_FEATURE_SOC_USART_COUNT];

/*! @brief IRQ name array */
static const IRQn_Type s_usartIRQ[] = USART_IRQS;

/*! @brief Array to map USART instance number to base address. */
static const uint32_t s_usartBaseAddrs[FSL_FEATURE_SOC_USART_COUNT] = USART_BASE_ADDRS;

/* @brief Array to map USART instance number to CLOCK names */
static const clock_ip_name_t s_usartClock[] = USART_CLOCKS;

/* Typedef for interrupt handler. */
typedef void (*usart_isr_t)(USART_Type *base, usart_handle_t *handle);

/* USART ISR for transactional APIs. */
static usart_isr_t s_usartIsr;

/*******************************************************************************
 * Code
 ******************************************************************************/

/* Get the index corresponding to the USART */
uint32_t USART_GetInstance(USART_Type *base)
{
    int i;

    for (i = 0; i < FSL_FEATURE_SOC_USART_COUNT; i++)
    {
        if ((uint32_t)base == s_usartBaseAddrs[i])
        {
            return i;
        }
    }

    assert(false);
    return 0;
}
/* Write one byte to TX register. */
void USART_WriteByte(USART_Type *base, uint8_t data)
{
    uint32_t instance = USART_GetInstance(base);
    if (USART_IsTxFifoEnable(base))
    {
        VFIFO->USART[instance].TXDATUSART = data;
    }
    else
    {
        base->TXDAT = data;
    }
}

/* Read one byte from RX register. */
uint8_t USART_ReadByte(USART_Type *base)
{
    uint32_t instance = USART_GetInstance(base);
    return ((USART_IsRxFifoEnable(base)) ? (VFIFO->USART[instance].RXDATUSART) : (base->RXDAT & 0x000000FFU));
}

/* Get system FIFO USART status flags. */
uint32_t USART_GetFifoStatusFlags(USART_Type *base)
{
    uint32_t instance = USART_GetInstance(base);
    return (VFIFO->USART[instance].STATUSART);
}

/* Clear system FIFO USART status flag. */
void USART_ClearFifoStatusFlags(USART_Type *base, uint32_t mask)
{
    uint32_t instance = USART_GetInstance(base);
    VFIFO->USART[instance].STATUSART |= mask;
}

/* Enable system FIFO interrupts for USART. */
void USART_EnableFifoInterrupts(USART_Type *base, uint32_t mask)
{
    uint32_t instance = USART_GetInstance(base);
    VFIFO->USART[instance].CTLSETUSART |= mask;
}

/* Disable FIFO interrupts for USART. */
void USART_DisableFifoInterrupts(USART_Type *base, uint32_t mask)
{
    uint32_t instance = USART_GetInstance(base);
    VFIFO->USART[instance].CTLCLRUSART |= mask;
}

/* Get enabled interrupt of FIFO for USART. */
uint32_t USART_GetEnabledFifoInterrupts(USART_Type *base)
{
    uint32_t instance = USART_GetInstance(base);
    return (VFIFO->USART[instance].CTLSETUSART);
}

/* Flush the FIFO for USART transfer. */
void USART_FifoFlush(USART_Type *base, uint32_t direction)
{
    uint32_t instance = USART_GetInstance(base);

    if (kUSART_FifoTx & direction)
    {
        VFIFO->USART[instance].CTLSETUSART |= VFIFO_USART_CTLSETUSART_TXFLUSH_MASK;
        VFIFO->USART[instance].CTLCLRUSART |= VFIFO_USART_CTLCLRUSART_TXFLUSHCLR_MASK;
    }

    if (kUSART_FifoRx & direction)
    {
        VFIFO->USART[instance].CTLSETUSART |= VFIFO_USART_CTLSETUSART_RXFLUSH_MASK;
        VFIFO->USART[instance].CTLCLRUSART |= VFIFO_USART_CTLCLRUSART_RXFLUSHCLR_MASK;
    }
}

/* Enable USART FIFO function. */
void USART_EnableFifo(USART_Type *base, const usart_fifo_config_t *config)
{
    uint32_t instance = 0U;

    instance = USART_GetInstance(base);

    /* Enable the system FIFO clock. */
    CLOCK_EnableClock(kCLOCK_Fifo);

    /* Enable the TX/RX FIFO mode. */
    switch (instance)
    {
        case 0:
            SYSCON->FIFOCTRL &= ~(SYSCON_FIFOCTRL_U0TXFIFOEN_MASK | SYSCON_FIFOCTRL_U0RXFIFOEN_MASK);
            SYSCON->FIFOCTRL |=
                SYSCON_FIFOCTRL_U0TXFIFOEN(config->enableTxFifo) | SYSCON_FIFOCTRL_U0RXFIFOEN(config->enableRxFifo);
            break;
        case 1:
            SYSCON->FIFOCTRL &= ~(SYSCON_FIFOCTRL_U1TXFIFOEN_MASK | SYSCON_FIFOCTRL_U1RXFIFOEN_MASK);
            SYSCON->FIFOCTRL |=
                SYSCON_FIFOCTRL_U1TXFIFOEN(config->enableTxFifo) | SYSCON_FIFOCTRL_U1RXFIFOEN(config->enableRxFifo);
            break;
        case 2:
            SYSCON->FIFOCTRL &= ~(SYSCON_FIFOCTRL_U2TXFIFOEN_MASK | SYSCON_FIFOCTRL_U2RXFIFOEN_MASK);
            SYSCON->FIFOCTRL |=
                SYSCON_FIFOCTRL_U2TXFIFOEN(config->enableTxFifo) | SYSCON_FIFOCTRL_U2RXFIFOEN(config->enableRxFifo);
            break;
        case 3:
            SYSCON->FIFOCTRL &= ~(SYSCON_FIFOCTRL_U3TXFIFOEN_MASK | SYSCON_FIFOCTRL_U3RXFIFOEN_MASK);
            SYSCON->FIFOCTRL |=
                SYSCON_FIFOCTRL_U3TXFIFOEN(config->enableTxFifo) | SYSCON_FIFOCTRL_U3RXFIFOEN(config->enableRxFifo);
            break;
        default:
            break;
    }
    /* Pause the USART FIFO for setting. */
    while (!((VFIFO->FIFOCTLUSART & VFIFO_FIFOCTLUSART_RXPAUSED_MASK) &&
             (VFIFO->FIFOCTLUSART & VFIFO_FIFOCTLUSART_TXPAUSED_MASK)))
    {
        VFIFO->FIFOCTLUSART |= (VFIFO_FIFOCTLUSART_RXPAUSE_MASK | VFIFO_FIFOCTLUSART_TXPAUSE_MASK);
    }
    /* Flush the  TX and RX FIFO buffer. */
    USART_FifoFlush(base, kUSART_FifoTx | kUSART_FifoRx);
    /* Set the TX and RX FIFO size. */
    VFIFO->FIFOCFGUSART[instance] =
        VFIFO_FIFOCFGUSART_RXSIZE(config->rxFifoSize) | VFIFO_FIFOCFGUSART_TXSIZE(config->txFifoSize);
    /* Set the TX and RX FIFO threshold size. */
    VFIFO->USART[instance].CFGUSART &= ~(VFIFO_USART_CFGUSART_RXTHRESHOLD_MASK | VFIFO_USART_CFGUSART_TXTHRESHOLD_MASK);
    VFIFO->USART[instance].CFGUSART |= VFIFO_USART_CFGUSART_RXTHRESHOLD(config->rxFifoThreshold) |
                                       VFIFO_USART_CFGUSART_TXTHRESHOLD(config->txFifoThreshold);
    /* Update all the TX and RX fifo size. */
    VFIFO->FIFOUPDATEUSART |=
        VFIFO_FIFOUPDATEUSART_USART0RXUPDATESIZE_MASK | VFIFO_FIFOUPDATEUSART_USART1RXUPDATESIZE_MASK |
        VFIFO_FIFOUPDATEUSART_USART2RXUPDATESIZE_MASK | VFIFO_FIFOUPDATEUSART_USART3RXUPDATESIZE_MASK |
        VFIFO_FIFOUPDATEUSART_USART0TXUPDATESIZE_MASK | VFIFO_FIFOUPDATEUSART_USART1TXUPDATESIZE_MASK |
        VFIFO_FIFOUPDATEUSART_USART2TXUPDATESIZE_MASK | VFIFO_FIFOUPDATEUSART_USART3TXUPDATESIZE_MASK;
    /* Unpause the system FIFO for transfer. */
    VFIFO->FIFOCTLUSART &= ~(VFIFO_FIFOCTLUSART_RXPAUSE_MASK | VFIFO_FIFOCTLUSART_TXPAUSE_MASK);
}

void USART_DisableFifo(USART_Type *base)
{
    assert(NULL != base);
    uint32_t instance = 0U;

    instance = USART_GetInstance(base);

    /* Pause the USART FIFO for setting. */
    while (!((VFIFO->FIFOCTLUSART & VFIFO_FIFOCTLUSART_RXPAUSED_MASK) &&
             (VFIFO->FIFOCTLUSART & VFIFO_FIFOCTLUSART_TXPAUSED_MASK)))
    {
        VFIFO->FIFOCTLUSART |= (VFIFO_FIFOCTLUSART_RXPAUSE_MASK | VFIFO_FIFOCTLUSART_TXPAUSE_MASK);
    }
    /* Flush the  TX and RX FIFO buffer. */
    USART_FifoFlush(base, kUSART_FifoTx | kUSART_FifoRx);
    /* Set the TX and RX FIFO size. */
    VFIFO->FIFOCFGUSART[instance] = VFIFO_FIFOCFGUSART_RXSIZE(0U) | VFIFO_FIFOCFGUSART_TXSIZE(0U);
    /* Set the TX and RX FIFO threshold size. */
    VFIFO->USART[instance].CFGUSART &= ~(VFIFO_USART_CFGUSART_RXTHRESHOLD_MASK | VFIFO_USART_CFGUSART_TXTHRESHOLD_MASK);
    /* Disable all FIFO interrupts. */
    USART_DisableFifoInterrupts(base, kUSART_FifoAllinterruptEnable);
    /* Clear all FIFO status flags, only receive timeout and bus error flag can be cleared. */
    USART_ClearFifoStatusFlags(base, kUSART_RxFifoTimeOutFlag | kUSART_FifoBusErrorFlag);

    /* Unpause the system FIFO for transfer. */
    VFIFO->FIFOCTLUSART &= ~(VFIFO_FIFOCTLUSART_RXPAUSE_MASK | VFIFO_FIFOCTLUSART_TXPAUSE_MASK);

    /* Disable the TX/RX FIFO mode. */
    switch (instance)
    {
        case 0:
            SYSCON->FIFOCTRL &= ~(SYSCON_FIFOCTRL_U0TXFIFOEN_MASK | SYSCON_FIFOCTRL_U0RXFIFOEN_MASK);
            break;
        case 1:
            SYSCON->FIFOCTRL &= ~(SYSCON_FIFOCTRL_U1TXFIFOEN_MASK | SYSCON_FIFOCTRL_U1RXFIFOEN_MASK);
            break;
        case 2:
            SYSCON->FIFOCTRL &= ~(SYSCON_FIFOCTRL_U2TXFIFOEN_MASK | SYSCON_FIFOCTRL_U2RXFIFOEN_MASK);
            break;
        case 3:
            SYSCON->FIFOCTRL &= ~(SYSCON_FIFOCTRL_U3TXFIFOEN_MASK | SYSCON_FIFOCTRL_U3RXFIFOEN_MASK);
            break;
        default:
            break;
    }
}

/* Check if TX FIFO enabled. */
bool USART_IsTxFifoEnable(USART_Type *base)
{
    uint32_t instance = USART_GetInstance(base);
    uint32_t fifosize = VFIFO->FIFOCFGUSART[instance];

    /* Check if system FIFO control TX is enabled. */
    switch (instance)
    {
        case 0:
            if (SYSCON->FIFOCTRL & SYSCON_FIFOCTRL_U0TXFIFOEN_MASK)
            {
                break;
            }
            else
            {
                return false;
            }
        case 1:
            if (SYSCON->FIFOCTRL & SYSCON_FIFOCTRL_U1TXFIFOEN_MASK)
            {
                break;
            }
            else
            {
                return false;
            }
        case 2:
            if (SYSCON->FIFOCTRL & SYSCON_FIFOCTRL_U2TXFIFOEN_MASK)
            {
                break;
            }
            else
            {
                return false;
            }
        case 3:
            if (SYSCON->FIFOCTRL & SYSCON_FIFOCTRL_U3TXFIFOEN_MASK)
            {
                break;
            }
            else
            {
                return false;
            }
        default:
            break;
    }

    /* Check if TX FIFO size is available. */
    return (fifosize & VFIFO_FIFOCFGUSART_TXSIZE_MASK) ? (true) : (false);
}

/* Check if RX FIFO enabled. */
bool USART_IsRxFifoEnable(USART_Type *base)
{
    uint32_t instance = USART_GetInstance(base);
    uint32_t fifosize = VFIFO->FIFOCFGUSART[instance];

    /* Check if system FIFO control RX is enabled. */
    switch (instance)
    {
        case 0:
            if (SYSCON->FIFOCTRL & SYSCON_FIFOCTRL_U0RXFIFOEN_MASK)
            {
                break;
            }
            else
            {
                return false;
            }
        case 1:
            if (SYSCON->FIFOCTRL & SYSCON_FIFOCTRL_U1RXFIFOEN_MASK)
            {
                break;
            }
            else
            {
                return false;
            }
        case 2:
            if (SYSCON->FIFOCTRL & SYSCON_FIFOCTRL_U2RXFIFOEN_MASK)
            {
                break;
            }
            else
            {
                return false;
            }
        case 3:
            if (SYSCON->FIFOCTRL & SYSCON_FIFOCTRL_U3RXFIFOEN_MASK)
            {
                break;
            }
            else
            {
                return false;
            }
        default:
            break;
    }

    /* Check if RX FIFO size is available. */
    return (fifosize & VFIFO_FIFOCFGUSART_RXSIZE_MASK) ? (true) : (false);
}

size_t USART_TransferGetRxRingBufferLength(usart_handle_t *handle)
{
    size_t size = 0U;

    /* Check arguments */
    assert(NULL != handle);

    if (handle->rxRingBufferTail > handle->rxRingBufferHead)
    {
        size = (size_t)(handle->rxRingBufferHead + handle->rxRingBufferSize - handle->rxRingBufferTail);
    }
    else
    {
        size = (size_t)(handle->rxRingBufferHead - handle->rxRingBufferTail);
    }
    return size;
}

static bool USART_TransferIsRxRingBufferFull(usart_handle_t *handle)
{
    bool full = 0U;

    /* Check arguments */
    assert(NULL != handle);

    if (USART_TransferGetRxRingBufferLength(handle) == (handle->rxRingBufferSize - 1U))
    {
        full = true;
    }
    else
    {
        full = false;
    }
    return full;
}

void USART_TransferStartRingBuffer(USART_Type *base, usart_handle_t *handle, uint8_t *ringBuffer, size_t ringBufferSize)
{
    /* Check arguments */
    assert(NULL != base);
    assert(NULL != handle);
    assert(NULL != ringBuffer);

    /* Setup the ringbuffer address */
    handle->rxRingBuffer = ringBuffer;
    handle->rxRingBufferSize = ringBufferSize;
    handle->rxRingBufferHead = 0U;
    handle->rxRingBufferTail = 0U;

    /* Ring buffer is ready we can start receiving data */
    if (handle->isRxFifoEnabled)
    {
        /* Start RX threshold interrupt for ring buffer to receive data. */
        USART_EnableFifoInterrupts(base, kUSART_RxFifoThresholdInterruptEnable);
    }
    else
    {
        /* Start receive data read interrupt and reveive overrun interrupt. */
        USART_EnableInterrupts(base, kUSART_RxReadyInterruptEnable | kUSART_HardwareOverRunInterruptEnable);
    }
}

void USART_TransferStopRingBuffer(USART_Type *base, usart_handle_t *handle)
{
    /* Check arguments */
    assert(NULL != base);
    assert(NULL != handle);

    /* If USART is in idle state, dsiable the interrupts for ring buffer. */
    if (handle->rxState == kUSART_RxIdle)
    {
        if (handle->isRxFifoEnabled)
        {
            /* Disable FIFO RX threshold interrupt. */
            USART_DisableFifoInterrupts(base, kUSART_RxFifoThresholdInterruptEnable);
        }
        else
        {
            USART_DisableInterrupts(base, kUSART_RxReadyInterruptEnable | kUSART_HardwareOverRunInterruptEnable);
        }
    }
    handle->rxRingBuffer = NULL;
    handle->rxRingBufferSize = 0U;
    handle->rxRingBufferHead = 0U;
    handle->rxRingBufferTail = 0U;
}

status_t USART_Init(USART_Type *base, const usart_config_t *config, uint32_t srcClock_Hz)
{
    int result = 0U;

    /* Check arguments */
    assert(!((NULL == base) || (NULL == config) || (0 == srcClock_Hz)));
    if ((NULL == base) || (NULL == config) || (0 == srcClock_Hz))
    {
        return kStatus_InvalidArgument;
    }

    /* Enable usart clock and FRG clock */
    CLOCK_EnableClock(s_usartClock[USART_GetInstance(base)]);
    CLOCK_EnableClock(kCLOCK_Frg);

    /* Setup configuration and enable USART to configure other register. */
    base->CFG = USART_CFG_PARITYSEL(config->parityMode) | USART_CFG_STOPLEN(config->stopBitCount) |
                USART_CFG_DATALEN(config->bitCountPerChar) | USART_CFG_LOOP(config->loopback) | USART_CFG_ENABLE_MASK;

    /* Setup baudrate */
    result = USART_SetBaudRate(base, config->baudRate_Bps, srcClock_Hz);
    if (kStatus_Success != result)
    {
        return result;
    }

    /* Check if FIFO is enabled from extern configuration. */
    if ((config->fifoConfig.enableTxFifo) || (config->fifoConfig.enableRxFifo))
    {
        USART_EnableFifo(base, &config->fifoConfig);
    }
    /* Setup the USART transmit and receive. */
    USART_EnableTx(base, config->enableTx);
    USART_EnableRx(base, config->enableRx);

    return kStatus_Success;
}

void USART_Deinit(USART_Type *base)
{
    /* Check arguments */
    assert(NULL != base);
    /* Wait for data transmit complete. */
    while (!(base->STAT & USART_STAT_TXIDLE_MASK))
    {
    }
    if ((USART_IsTxFifoEnable(base)) || (USART_IsRxFifoEnable(base)))
    {
        USART_DisableFifo(base);
    }
    /* Disable the USART module. */
    base->CFG &= ~(USART_CFG_ENABLE_MASK);
    /* Disbable the clock for usart */
    CLOCK_DisableClock(s_usartClock[USART_GetInstance(base)]);
}

void USART_GetDefaultConfig(usart_config_t *config)
{
    /* Check arguments */
    assert(NULL != config);

    /* Set always all members ! */
    config->baudRate_Bps = 115200U;
    config->parityMode = kUSART_ParityDisabled;
    config->stopBitCount = kUSART_OneStopBit;
    config->bitCountPerChar = kUSART_8BitsPerChar;
    config->loopback = false;
    config->enableRx = false;
    config->enableTx = false;
    config->fifoConfig.enableTxFifo = false;
    config->fifoConfig.enableRxFifo = false;
    config->fifoConfig.rxFifoSize = 16U;
    config->fifoConfig.txFifoSize = 16U;
    config->fifoConfig.txFifoThreshold = 0U;
    config->fifoConfig.rxFifoThreshold = 0U;
}

status_t USART_SetBaudRate(USART_Type *base, uint32_t baudrate_Bps, uint32_t srcClock_Hz)
{
    uint32_t best_diff = (uint32_t)-1, best_osrval = 0xf, best_brgval = (uint32_t)-1;
    uint32_t osrval, brgval, diff, baudrate;

    /* check arguments */
    assert(!((NULL == base) || (0 == baudrate_Bps) || (0 == srcClock_Hz)));
    if ((NULL == base) || (0 == baudrate_Bps) || (0 == srcClock_Hz))
    {
        return kStatus_InvalidArgument;
    }

    /*
     * Smaller values of OSR can make the sampling position within a data bit less accurate and may
     * potentially cause more noise errors or incorrect data.
     */
    for (osrval = best_osrval; osrval >= 8; osrval--)
    {
        brgval = (srcClock_Hz / ((osrval + 1) * baudrate_Bps)) - 1;
        if (brgval > 0xFFFF)
        {
            continue;
        }
        baudrate = srcClock_Hz / ((osrval + 1) * (brgval + 1));
        diff = baudrate_Bps < baudrate ? baudrate - baudrate_Bps : baudrate_Bps - baudrate;
        if (diff < best_diff)
        {
            best_diff = diff;
            best_osrval = osrval;
            best_brgval = brgval;
        }
    }

    /* value over range */
    if (best_brgval > 0xFFFF)
    {
        return kStatus_USART_BaudrateNotSupport;
    }

    base->OSR = best_osrval;
    base->BRG = best_brgval;
    return kStatus_Success;
}

void USART_WriteBlocking(USART_Type *base, const uint8_t *data, size_t length)
{
    uint32_t instance = 0U;
    /* Check arguments */
    assert(!((NULL == base) || (NULL == data)));
    if ((NULL == base) || (NULL == data))
    {
        return;
    }

    instance = USART_GetInstance(base);

    for (; length > 0; length--)
    {
        if (USART_IsTxFifoEnable(base))
        {
            /* Loop until TX FIFO is empty for new data */
            while (!(VFIFO->USART[instance].STATUSART & VFIFO_USART_STATUSART_TXEMPTY_MASK))
            {
            }
            VFIFO->USART[instance].TXDATUSART = *data;
        }
        else
        {
            /* Wait for TX is ready to transmit new data. */
            while (!(base->STAT & USART_STAT_TXRDY_MASK))
            {
            }
            base->TXDAT = *data;
        }
        data++;
    }
    /* Wait to finish transfer */
    while (!(base->STAT & USART_STAT_TXIDLE_MASK))
    {
    }
}

status_t USART_ReadBlocking(USART_Type *base, uint8_t *data, size_t length)
{
    uint32_t status;
    uint32_t instance = 0U;

    /* Check arguments */
    assert(!((NULL == base) || (NULL == data)));
    if ((NULL == base) || (NULL == data))
    {
        return kStatus_InvalidArgument;
    }

    instance = USART_GetInstance(base);

    for (; length > 0; length--)
    {
        if (USART_IsRxFifoEnable(base))
        {
            /* Wait for RX FIFO is not empty. */
            while ((VFIFO->USART[instance].STATUSART & VFIFO_USART_STATUSART_RXEMPTY_MASK))
            {
            }
            /* Check FIFO receive status. */
            status = VFIFO->USART[instance].STATUSART;
            if (status & VFIFO_USART_STATUSART_BUSERR_MASK)
            {
                return kStatus_USART_FifoBusError;
            }
            status = VFIFO->USART[instance].RXDATSTATUSART;
            if (status & VFIFO_USART_RXDATSTATUSART_FRAMERR_MASK)
            {
                return kStatus_USART_FramingError;
            }
            if (status & VFIFO_USART_RXDATSTATUSART_PARITYERR_MASK)
            {
                return kStatus_USART_ParityError;
            }
            if (status & VFIFO_USART_RXDATSTATUSART_RXNOISE_MASK)
            {
                return kStatus_USART_NoiseError;
            }
            *data = (status & VFIFO_USART_RXDATSTATUSART_RXDAT_MASK);
        }
        else
        {
            /* loop until receive is ready to read */
            while (!(base->STAT & USART_STAT_RXRDY_MASK))
            {
            }

            /* Check receive status */
            status = base->STAT;

            if (status & USART_STAT_FRAMERRINT_MASK)
            {
                base->STAT |= USART_STAT_FRAMERRINT_MASK;
                return kStatus_USART_FramingError;
            }
            if (status & USART_STAT_PARITYERRINT_MASK)
            {
                base->STAT |= USART_STAT_PARITYERRINT_MASK;
                return kStatus_USART_ParityError;
            }
            if (status & USART_STAT_RXNOISEINT_MASK)
            {
                base->STAT |= USART_STAT_RXNOISEINT_MASK;
                return kStatus_USART_NoiseError;
            }
            if (base->STAT & USART_STAT_OVERRUNINT_MASK)
            {
                base->STAT |= USART_STAT_OVERRUNINT_MASK;
                return kStatus_USART_HardwareOverrun;
            }
            *data = base->RXDAT;
        }
        data++;
    }
    return 0;
}

status_t USART_TransferCreateHandle(USART_Type *base,
                                    usart_handle_t *handle,
                                    usart_transfer_callback_t callback,
                                    void *userData)
{
    int32_t instance = 0;

    /* Check 'base' */
    assert(!((NULL == base) || (NULL == handle)));
    if ((NULL == base) || (NULL == handle))
    {
        return kStatus_InvalidArgument;
    }

    instance = USART_GetInstance(base);

    memset(handle, 0, sizeof(*handle));
    /* Set the TX/RX state. */
    handle->rxState = kUSART_RxIdle;
    handle->txState = kUSART_TxIdle;
    /* Set the callback and user data. */
    handle->callback = callback;
    handle->userData = userData;
    /* Check if TX/RX FIFO are enabled. */
    handle->isTxFifoEnabled = USART_IsTxFifoEnable(base);
    handle->isRxFifoEnabled = USART_IsRxFifoEnable(base);
    if ((handle->isTxFifoEnabled) || (handle->isRxFifoEnabled))
    {
        handle->rxFifoThreshold = (VFIFO->USART[instance].CFGUSART & VFIFO_USART_CFGUSART_RXTHRESHOLD_MASK) >>
                                  VFIFO_USART_CFGUSART_RXTHRESHOLD_SHIFT;
        handle->txFifoThreshold = (VFIFO->USART[instance].CFGUSART & VFIFO_USART_CFGUSART_TXTHRESHOLD_MASK) >>
                                  VFIFO_USART_CFGUSART_TXTHRESHOLD_SHIFT;
    }
    /* Store the handle data to global variables. */
    s_usartHandle[instance] = handle;
    /* Mapping the interrupt function. */
    s_usartIsr = USART_TransferHandleIRQ;

    /* Enable interrupt in NVIC. */
    EnableIRQ(s_usartIRQ[instance]);

    return kStatus_Success;
}

status_t USART_TransferSendNonBlocking(USART_Type *base, usart_handle_t *handle, usart_transfer_t *xfer)
{
    /* Check arguments */
    assert(!((NULL == base) || (NULL == handle) || (NULL == xfer)));
    if ((NULL == base) || (NULL == handle) || (NULL == xfer))
    {
        return kStatus_InvalidArgument;
    }
    /* Check xfer members */
    assert(!((0 == xfer->dataSize) || (NULL == xfer->data)));
    if ((0 == xfer->dataSize) || (NULL == xfer->data))
    {
        return kStatus_InvalidArgument;
    }
    uint32_t instance = USART_GetInstance(base);

    /* Return error if current TX busy. */
    if (kUSART_TxBusy == handle->txState)
    {
        return kStatus_USART_TxBusy;
    }
    else
    {
        handle->txData = xfer->data;
        handle->txDataSize = xfer->dataSize;
        handle->txDataSizeAll = xfer->dataSize;
        handle->txState = kUSART_TxBusy;
        /* Check if TX/RX FIFO are enabled. */
        handle->isTxFifoEnabled = USART_IsTxFifoEnable(base);
        handle->isRxFifoEnabled = USART_IsRxFifoEnable(base);
        if ((handle->isTxFifoEnabled) || (handle->isRxFifoEnabled))
        {
            handle->rxFifoThreshold = (VFIFO->USART[instance].CFGUSART & VFIFO_USART_CFGUSART_RXTHRESHOLD_MASK) >>
                                      VFIFO_USART_CFGUSART_RXTHRESHOLD_SHIFT;
            handle->txFifoThreshold = (VFIFO->USART[instance].CFGUSART & VFIFO_USART_CFGUSART_TXTHRESHOLD_MASK) >>
                                      VFIFO_USART_CFGUSART_TXTHRESHOLD_SHIFT;
        }
        /* Enable transmiter interrupt. */
        if (handle->isTxFifoEnabled)
        {
            /* Start system fifo usart tx threshold interrupt. */
            USART_EnableFifoInterrupts(base, kUSART_TxFifoThresholdInterruptEnable);
        }
        else
        {
            USART_EnableInterrupts(base, kUSART_TxReadyInterruptEnable);
            /* Clear transmit disable bit. */
            base->CTL &= ~USART_CTL_TXDIS_MASK;
        }
    }
    return kStatus_Success;
}

void USART_TransferAbortSend(USART_Type *base, usart_handle_t *handle)
{
    assert(NULL != handle);

    /* Disable interrupts */
    if (handle->isTxFifoEnabled)
    {
        /* Disable system fifo usart tx threshold interrupt. */
        USART_DisableFifoInterrupts(base, kUSART_TxFifoThresholdInterruptEnable);
    }
    else
    {
        USART_DisableInterrupts(base, kUSART_TxReadyInterruptEnable);
    }
    /* Disable transmit after data being transmitted. */
    USART_EnableTx(base, false);

    handle->txDataSize = 0;
    handle->txState = kUSART_TxIdle;
}

status_t USART_TransferGetSendCount(USART_Type *base, usart_handle_t *handle, uint32_t *count)
{
    assert(NULL != handle);
    assert(NULL != count);

    if (kUSART_TxIdle == handle->txState)
    {
        return kStatus_NoTransferInProgress;
    }

    *count = handle->txDataSizeAll - handle->txDataSize;

    return kStatus_Success;
}

status_t USART_TransferReceiveNonBlocking(USART_Type *base,
                                          usart_handle_t *handle,
                                          usart_transfer_t *xfer,
                                          size_t *receivedBytes)
{
    uint32_t i;
    /* How many bytes to copy from ring buffer to user memory. */
    size_t bytesToCopy = 0U;
    /* How many bytes to receive. */
    size_t bytesToReceive;
    /* How many bytes currently have received. */
    size_t bytesCurrentReceived;
    uint32_t instance = USART_GetInstance(base);

    /* Check arguments */
    assert(!((NULL == base) || (NULL == handle) || (NULL == xfer)));
    if ((NULL == base) || (NULL == handle) || (NULL == xfer))
    {
        return kStatus_InvalidArgument;
    }
    /* Check xfer members */
    assert(!((0 == xfer->dataSize) || (NULL == xfer->data)));
    if ((0 == xfer->dataSize) || (NULL == xfer->data))
    {
        return kStatus_InvalidArgument;
    }

    /* How to get data:
       1. If RX ring buffer is not enabled, then save xfer->data and xfer->dataSize
          to uart handle, enable interrupt to store received data to xfer->data. When
          all data received, trigger callback.
       2. If RX ring buffer is enabled and not empty, get data from ring buffer first.
          If there are enough data in ring buffer, copy them to xfer->data and return.
          If there are not enough data in ring buffer, copy all of them to xfer->data,
          save the xfer->data remained empty space to uart handle, receive data
          to this empty space and trigger callback when finished. */
    if (kUSART_RxBusy == handle->rxState)
    {
        return kStatus_USART_RxBusy;
    }
    else
    {
        /* Check if TX/RX FIFO are enabled. */
        handle->isTxFifoEnabled = USART_IsTxFifoEnable(base);
        handle->isRxFifoEnabled = USART_IsRxFifoEnable(base);
        if ((handle->isTxFifoEnabled) || (handle->isRxFifoEnabled))
        {
            handle->rxFifoThreshold = (VFIFO->USART[instance].CFGUSART & VFIFO_USART_CFGUSART_RXTHRESHOLD_MASK) >>
                                      VFIFO_USART_CFGUSART_RXTHRESHOLD_SHIFT;
            handle->txFifoThreshold = (VFIFO->USART[instance].CFGUSART & VFIFO_USART_CFGUSART_TXTHRESHOLD_MASK) >>
                                      VFIFO_USART_CFGUSART_TXTHRESHOLD_SHIFT;
        }

        bytesToReceive = xfer->dataSize;
        bytesCurrentReceived = 0U;
        /* If RX ring buffer is used. */
        if (handle->rxRingBuffer)
        {
            /* Disable USART receive IRQ, protect ring buffer. */
            if (handle->isRxFifoEnabled)
            {
                /* Disable system fifo usart rx threshold interrupt. */
                USART_DisableFifoInterrupts(base, kUSART_RxFifoThresholdInterruptEnable);
            }
            else
            {
                USART_DisableInterrupts(base, kUSART_RxReadyInterruptEnable);
            }
            /* How many bytes in RX ring buffer currently. */
            bytesToCopy = USART_TransferGetRxRingBufferLength(handle);
            if (bytesToCopy)
            {
                bytesToCopy = MIN(bytesToReceive, bytesToCopy);
                bytesToReceive -= bytesToCopy;
                /* Copy data from ring buffer to user memory. */
                for (i = 0U; i < bytesToCopy; i++)
                {
                    xfer->data[bytesCurrentReceived++] = handle->rxRingBuffer[handle->rxRingBufferTail];
                    /* Wrap to 0. Not use modulo (%) because it might be large and slow. */
                    if (handle->rxRingBufferTail + 1U == handle->rxRingBufferSize)
                    {
                        handle->rxRingBufferTail = 0U;
                    }
                    else
                    {
                        handle->rxRingBufferTail++;
                    }
                }
            }
            /* If ring buffer does not have enough data, still need to read more data. */
            if (bytesToReceive)
            {
                /* No data in ring buffer, save the request to UART handle. */
                handle->rxData = xfer->data + bytesCurrentReceived;
                handle->rxDataSize = bytesToReceive;
                handle->rxDataSizeAll = bytesToReceive;
                handle->rxState = kUSART_RxBusy;
            }
            /* Enable USART receive interrupt for next transfer. */
            if (handle->isRxFifoEnabled)
            {
                /* Enable system fifo usart rx threshold interrupt. */
                USART_EnableFifoInterrupts(base, kUSART_RxFifoThresholdInterruptEnable);
            }
            else
            {
                USART_EnableInterrupts(base, kUSART_RxReadyInterruptEnable);
            }
            /* Call user callback since all data are received. */
            if (0 == bytesToReceive)
            {
                if (handle->callback)
                {
                    handle->callback(base, handle, kStatus_USART_RxIdle, handle->userData);
                }
            }
        }
        /* Ring buffer not used. */
        else
        {
            handle->rxData = xfer->data + bytesCurrentReceived;
            handle->rxDataSize = bytesToReceive;
            handle->rxDataSizeAll = bytesToReceive;
            handle->rxState = kUSART_RxBusy;

            /* Enable RX interrupt. */
            if (handle->isRxFifoEnabled)
            {
                USART_EnableFifoInterrupts(base, kUSART_RxFifoThresholdInterruptEnable);
            }
            else
            {
                USART_EnableInterrupts(base, kUSART_RxReadyInterruptEnable | kUSART_HardwareOverRunInterruptEnable);
            }
        }
        /* Return the how many bytes have read. */
        if (receivedBytes)
        {
            *receivedBytes = bytesCurrentReceived;
        }
    }
    return kStatus_Success;
}

void USART_TransferAbortReceive(USART_Type *base, usart_handle_t *handle)
{
    assert(NULL != handle);

    /* Only abort the receive to handle->rxData, the RX ring buffer is still working. */
    if (!handle->rxRingBuffer)
    {
        /* Disable interrupts */
        if (handle->isRxFifoEnabled)
        {
            USART_DisableFifoInterrupts(base, kUSART_RxFifoThresholdInterruptEnable);
        }
        else
        {
            USART_DisableInterrupts(base, kUSART_RxReadyInterruptEnable | kUSART_HardwareOverRunInterruptEnable);
        }
    }

    handle->rxDataSize = 0U;
    handle->rxState = kUSART_RxIdle;
}

status_t USART_TransferGetReceiveCount(USART_Type *base, usart_handle_t *handle, uint32_t *count)
{
    assert(NULL != handle);
    assert(NULL != count);

    if (kUSART_RxIdle == handle->rxState)
    {
        return kStatus_NoTransferInProgress;
    }

    *count = handle->rxDataSizeAll - handle->rxDataSize;

    return kStatus_Success;
}

void USART_TransferHandleIRQ(USART_Type *base, usart_handle_t *handle)
{
    /* Check arguments */
    assert((NULL != base) && (NULL != handle));

    bool receiveEnabled = (handle->rxDataSize) || (handle->rxRingBuffer);
    bool sendEnabled = handle->txDataSize;
    uint32_t instance = USART_GetInstance(base);

    /* If RX overrun. */
    if (base->STAT & USART_STAT_OVERRUNINT_MASK)
    {
        /* Clear rx error state. */
        base->STAT |= USART_STAT_OVERRUNINT_MASK;
        /* Trigger callback. */
        if (handle->callback)
        {
            handle->callback(base, handle, kStatus_USART_RxError, handle->userData);
        }
    }
    if ((receiveEnabled &&
         ((handle->isRxFifoEnabled) ? (VFIFO->USART[instance].INTSTATUSART & VFIFO_USART_INTSTATUSART_RXTH_MASK) :
                                      (base->STAT & USART_STAT_RXRDY_MASK))) ||
        (sendEnabled &&
         ((handle->isTxFifoEnabled) ? (VFIFO->USART[instance].INTSTATUSART & VFIFO_USART_INTSTATUSART_TXTH_MASK) :
                                      (base->STAT & USART_STAT_TXRDY_MASK))))
    {
        /* Receive data */
        if ((receiveEnabled) &&
            ((handle->isRxFifoEnabled) ? (VFIFO->USART[instance].INTSTATUSART & VFIFO_USART_INTSTATUSART_RXTH_MASK) :
                                         (base->STAT & USART_STAT_RXRDY_MASK)))
        {
            /* Receive to app bufffer if app buffer is present */
            if ((handle->rxDataSize))
            {
                if (handle->isRxFifoEnabled)
                {
                    uint8_t dataToReceive = handle->rxFifoThreshold + 1;
                    do
                    {
                        *handle->rxData = VFIFO->USART[instance].RXDATUSART;
                        handle->rxDataSize--;
                        handle->rxData++;
                        receiveEnabled = ((handle->rxDataSize != 0) || (handle->rxRingBuffer));
                    } while ((--dataToReceive) && (handle->rxDataSize));
                }
                else
                {
                    *handle->rxData = base->RXDAT;
                    handle->rxDataSize--;
                    handle->rxData++;
                    receiveEnabled = ((handle->rxDataSize != 0) || (handle->rxRingBuffer));
                }

                if (!handle->rxDataSize)
                {
                    if (!handle->rxRingBuffer)
                    {
                        /* If system FIFO enabled. */
                        if (handle->isRxFifoEnabled)
                        {
                            /* Disable system fifo usart rx threshold interrupt. */
                            USART_DisableFifoInterrupts(base, kUSART_RxFifoThresholdInterruptEnable);
                        }
                        else
                        {
                            USART_DisableInterrupts(
                                base, kUSART_RxReadyInterruptEnable | kUSART_HardwareOverRunInterruptEnable);
                        }
                    }
                    handle->rxState = kUSART_RxIdle;
                    if (handle->callback)
                    {
                        handle->callback(base, handle, kStatus_USART_RxIdle, handle->userData);
                    }
                }
            }
            /* Otherwise receive to ring buffer if ring buffer is present */
            else
            {
                if (handle->rxRingBuffer)
                {
                    uint8_t count = (handle->isRxFifoEnabled) ? (handle->rxFifoThreshold + 1) : (1U);
                    /* If RX ring buffer is full, trigger callback to notify over run. */
                    while (count--)
                    {
                        if (USART_TransferIsRxRingBufferFull(handle))
                        {
                            if (handle->callback)
                            {
                                handle->callback(base, handle, kStatus_USART_RxRingBufferOverrun, handle->userData);
                            }
                        }
                        /* If ring buffer is still full after callback function, the oldest data is overrided. */
                        if (USART_TransferIsRxRingBufferFull(handle))
                        {
                            /* Increase handle->rxRingBufferTail to make room for new data. */
                            if (handle->rxRingBufferTail + 1U == handle->rxRingBufferSize)
                            {
                                handle->rxRingBufferTail = 0U;
                            }
                            else
                            {
                                handle->rxRingBufferTail++;
                            }
                        }
                        /* Read data. */
                        if (handle->isRxFifoEnabled)
                        {
                            handle->rxRingBuffer[handle->rxRingBufferHead] = VFIFO->USART[instance].RXDATUSART;
                        }
                        else
                        {
                            handle->rxRingBuffer[handle->rxRingBufferHead] = base->RXDAT;
                        }
                        /* Increase handle->rxRingBufferHead. */
                        if (handle->rxRingBufferHead + 1U == handle->rxRingBufferSize)
                        {
                            handle->rxRingBufferHead = 0U;
                        }
                        else
                        {
                            handle->rxRingBufferHead++;
                        }
                    }
                }
            }
        }
        /* Send data */
        if (sendEnabled &&
            ((handle->isTxFifoEnabled) ? (VFIFO->USART[instance].INTSTATUSART & VFIFO_USART_INTSTATUSART_TXTH_MASK) :
                                         (base->STAT & USART_STAT_TXRDY_MASK)))
        {
            /* Send data. */
            if (handle->isTxFifoEnabled)
            {
                uint8_t dataToSend = (handle->txFifoThreshold) ? (handle->txFifoThreshold) : (1U);
                do
                {
                    VFIFO->USART[instance].TXDATUSART = *handle->txData;
                    handle->txDataSize--;
                    handle->txData++;
                    sendEnabled = handle->txDataSize;
                } while ((--dataToSend) && (sendEnabled));
            }
            else
            {
                base->TXDAT = *handle->txData;
                handle->txDataSize--;
                handle->txData++;
                sendEnabled = handle->txDataSize != 0;
            }

            if (!sendEnabled)
            {
                /* Disable interrupts */
                if (handle->isTxFifoEnabled)
                {
                    USART_DisableFifoInterrupts(base, kUSART_TxFifoThresholdInterruptEnable);
                }
                else
                {
                    USART_DisableInterrupts(base, kUSART_TxReadyInterruptEnable);
                }
                handle->txState = kUSART_TxIdle;
                if (handle->callback)
                {
                    handle->callback(base, handle, kStatus_USART_TxIdle, handle->userData);
                }
            }
        }
    }

    /* Ring buffer is not used */
    if ((NULL == handle->rxRingBuffer) && (handle->isRxFifoEnabled))
    {
        /* Restore if rx transfer ends and rxLevel is different from default value */
        if ((handle->rxDataSize == 0) && (((VFIFO->USART[instance].CFGUSART & VFIFO_USART_CFGUSART_RXTHRESHOLD_MASK) >>
                                           VFIFO_USART_CFGUSART_RXTHRESHOLD_SHIFT) != handle->rxFifoThreshold))
        {
            VFIFO->USART[instance].CFGUSART =
                ((VFIFO->USART[instance].CFGUSART & (~VFIFO_USART_CFGUSART_RXTHRESHOLD_MASK)) |
                 VFIFO_USART_CFGUSART_RXTHRESHOLD(handle->rxFifoThreshold));
        }
        /* Decrease level if rx transfer is bellow */
        if ((handle->rxDataSize != 0) &&
            (handle->rxDataSize <= (VFIFO->USART[instance].CFGUSART & VFIFO_USART_CFGUSART_RXTHRESHOLD_MASK) >>
             VFIFO_USART_CFGUSART_RXTHRESHOLD_SHIFT))
        {
            VFIFO->USART[instance].CFGUSART =
                ((VFIFO->USART[instance].CFGUSART & (~VFIFO_USART_CFGUSART_RXTHRESHOLD_MASK)) |
                 VFIFO_USART_CFGUSART_RXTHRESHOLD(handle->rxDataSize - 1));
        }
    }
}

#if defined(USART0)
void USART0_DriverIRQHandler(void)
{
    s_usartIsr(USART0, s_usartHandle[0]);
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(USART1)
void USART1_DriverIRQHandler(void)
{
    s_usartIsr(USART1, s_usartHandle[1]);
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(USART2)
void USART2_DriverIRQHandler(void)
{
    s_usartIsr(USART2, s_usartHandle[2]);
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif

#if defined(USART3)
void USART3_DriverIRQHandler(void)
{
    s_usartIsr(USART3, s_usartHandle[3]);
    /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
      exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}
#endif
