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

#include "fsl_spi.h"

/*******************************************************************************
 * Definitons
 ******************************************************************************/

/* Convert transfer count to transfer bytes. dataWidth is a
 * range <0,15>. Range <8,15> represents 2B transfer */
#define SPI_COUNT_TO_BYTES(dataWidth, count) ((count) << ((dataWidth) >> 3U))
#define SPI_BYTES_TO_COUNT(dataWidth, bytes) ((bytes) >> ((dataWidth) >> 3U))
#define SPI_SSELPOL_MASK ((SPI_CFG_SPOL0_MASK) | (SPI_CFG_SPOL1_MASK) | (SPI_CFG_SPOL2_MASK) | (SPI_CFG_SPOL3_MASK))

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*! @brief SPI internal handle pointer array */
static spi_master_handle_t *s_spiHandle[FSL_FEATURE_SOC_SPI_COUNT];

/*! @brief internal SPI config array */
static spi_config_t g_configs[FSL_FEATURE_SOC_SPI_COUNT] = {(spi_data_width_t)0};

/*! @brief Array to map SPI instance number to base address. */
static const uint32_t s_spiBaseAddrs[FSL_FEATURE_SOC_SPI_COUNT] = SPI_BASE_ADDRS;

/*! @brief IRQ name array */
static const IRQn_Type s_spiIRQ[] = SPI_IRQS;

/* @brief Array to map SPI instance number to CLOCK names */
static const clock_ip_name_t s_spiClock[] = SPI_CLOCKS;

/* @brief Array to map SPI reset. */
static const SYSCON_RSTn_t s_spiReset[2] = {kSPI0_RST_SHIFT_RSTn, kSPI1_RST_SHIFT_RSTn};

/*! @brief Typedef for spi master interrupt handler. spi master and slave handle is the same. */
typedef void (*spi_isr_t)(SPI_Type *base, spi_master_handle_t *spiHandle);

/*! @brief Pointer to master IRQ handler for each instance. */
static spi_isr_t s_spiMasterIsr;
static spi_isr_t s_spiSlaveIsr;

/* @brief Dummy data for each instance. This data is used when user's tx buffer is NULL*/
volatile uint8_t s_dummyData[FSL_FEATURE_SOC_SPI_COUNT] = {0};
/*******************************************************************************
 * Code
 ******************************************************************************/

/* Get the index corresponding to the FLEXCOMM */
uint32_t SPI_GetInstance(SPI_Type *base)
{
    assert(NULL != base);

    int i = 0U;

    for (i = 0; i < FSL_FEATURE_SOC_SPI_COUNT; i++)
    {
        if ((uint32_t)base == s_spiBaseAddrs[i])
        {
            return i;
        }
    }

    assert(false);
    return 0;
}

void SPI_SetDummyData(SPI_Type *base, uint8_t dummyData)
{
    uint32_t instance = SPI_GetInstance(base);
    s_dummyData[instance] = dummyData;
}

/* Get the information of datawidth and SSEL number. */
void *SPI_GetConfig(SPI_Type *base)
{
    int32_t instance = SPI_GetInstance(base);

    if (instance < 0)
    {
        return NULL;
    }
    return &g_configs[instance];
}

/* Get status flags from FIFO status register. */
uint32_t SPI_GetFifoStatusFlags(SPI_Type *base)
{
    uint32_t instance = SPI_GetInstance(base);
    return (VFIFO->SPI[instance].STATSPI);
}

/* Clear status flags of FIFO status register. */
void SPI_ClearFifoStatusFlags(SPI_Type *base, uint32_t mask)
{
    uint32_t instance = SPI_GetInstance(base);
    VFIFO->SPI[instance].STATSPI |= mask;
}

/* Enable FIFO interrupt from FIFO CTLSETSPI register. */
void SPI_EnableFifoInterrupts(SPI_Type *base, uint32_t irqs)
{
    uint32_t instance = SPI_GetInstance(base);
    VFIFO->SPI[instance].CTLSETSPI |= irqs;
}

/* Clear FIFO interrupt from FIFO CTLCLRSPI register. */
void SPI_DisableFifoInterrupts(SPI_Type *base, uint32_t irqs)
{
    uint32_t instance = SPI_GetInstance(base);
    VFIFO->SPI[instance].CTLCLRSPI |= irqs;
}

/* Flush the FIFO for SPI transfer. */
void SPI_FifoFlush(SPI_Type *base, uint32_t direction)
{
    uint32_t instance = SPI_GetInstance(base);

    if (kSPI_FifoTx & direction)
    {
        VFIFO->SPI[instance].CTLSETSPI |= VFIFO_SPI_CTLSETSPI_TXFLUSH_MASK;
        VFIFO->SPI[instance].CTLCLRSPI |= VFIFO_SPI_CTLCLRSPI_TXFLUSHCLR_MASK;
    }

    if (kSPI_FifoRx & direction)
    {
        VFIFO->SPI[instance].CTLSETSPI |= VFIFO_SPI_CTLSETSPI_RXFLUSH_MASK;
        VFIFO->SPI[instance].CTLCLRSPI |= VFIFO_SPI_CTLCLRSPI_RXFLUSHCLR_MASK;
    }
}

/* Enable SPI FIFO function. */
void SPI_EnableFifo(SPI_Type *base, const spi_fifo_config_t *config)
{
    uint32_t instance = 0U;
    assert((config->enableRxFifo) || (config->enableTxFifo));

    instance = SPI_GetInstance(base);

    /* Enable the system FIFO clock. */
    CLOCK_EnableClock(kCLOCK_Fifo);

    /* Configure the FIFOCTRL register to enable the SPI TX/RX FIFO . */
    switch (instance)
    {
        case 0:
            SYSCON->FIFOCTRL &= ~(SYSCON_FIFOCTRL_SPI0TXFIFOEN_MASK | SYSCON_FIFOCTRL_SPI0RXFIFOEN_MASK);
            SYSCON->FIFOCTRL |=
                SYSCON_FIFOCTRL_SPI0TXFIFOEN(config->enableTxFifo) | SYSCON_FIFOCTRL_SPI0RXFIFOEN(config->enableRxFifo);
            break;
        case 1:
            SYSCON->FIFOCTRL &= ~(SYSCON_FIFOCTRL_SPI1TXFIFOEN_MASK | SYSCON_FIFOCTRL_SPI1RXFIFOEN_MASK);
            SYSCON->FIFOCTRL |=
                SYSCON_FIFOCTRL_SPI1TXFIFOEN(config->enableTxFifo) | SYSCON_FIFOCTRL_SPI1RXFIFOEN(config->enableRxFifo);
            break;
        default:
            break;
    }
    /* Pause the SPI FIFO for setting. */
    while (
        !((VFIFO->FIFOCTLSPI & VFIFO_FIFOCTLSPI_RXPAUSED_MASK) && (VFIFO->FIFOCTLSPI & VFIFO_FIFOCTLSPI_TXPAUSED_MASK)))
    {
        VFIFO->FIFOCTLSPI |= (VFIFO_FIFOCTLSPI_RXPAUSE_MASK | VFIFO_FIFOCTLSPI_TXPAUSE_MASK);
    }
    /* Flush the  TX and RX FIFO buffer. */
    SPI_FifoFlush(base, kSPI_FifoTx | kSPI_FifoRx);
    /* Set the TX and RX FIFO size. */
    VFIFO->FIFOCFGSPI[instance] =
        VFIFO_FIFOCFGSPI_RXSIZE(config->rxFifoSize) | VFIFO_FIFOCFGSPI_TXSIZE(config->txFifoSize);
    /* Update all the SPI FIFO size. */
    VFIFO->FIFOUPDATESPI |= VFIFO_FIFOUPDATESPI_SPI0RXUPDATESIZE_MASK | VFIFO_FIFOUPDATESPI_SPI0TXUPDATESIZE_MASK |
                            VFIFO_FIFOUPDATESPI_SPI1RXUPDATESIZE_MASK | VFIFO_FIFOUPDATESPI_SPI1TXUPDATESIZE_MASK;
    /* Set the TX and RX FIFO threshold size. */
    VFIFO->SPI[instance].CFGSPI &= ~(VFIFO_SPI_CFGSPI_RXTHRESHOLD_MASK | VFIFO_SPI_CFGSPI_TXTHRESHOLD_MASK);
    VFIFO->SPI[instance].CFGSPI |=
        VFIFO_SPI_CFGSPI_RXTHRESHOLD(config->rxFifoThreshold) | VFIFO_SPI_CFGSPI_TXTHRESHOLD(config->txFifoThreshold);
    /* Unpause the system FIFO for transfer. */
    VFIFO->FIFOCTLSPI &= ~(VFIFO_FIFOCTLSPI_RXPAUSE_MASK | VFIFO_FIFOCTLSPI_TXPAUSE_MASK);
}

/* Disable system FIFO. */
void SPI_DisableFifo(SPI_Type *base)
{
    assert(NULL != base);
    uint32_t instance = SPI_GetInstance(base);

    /* Pause the SPI FIFO for setting. */
    while (
        !((VFIFO->FIFOCTLSPI & VFIFO_FIFOCTLSPI_RXPAUSED_MASK) && (VFIFO->FIFOCTLSPI & VFIFO_FIFOCTLSPI_TXPAUSED_MASK)))
    {
        VFIFO->FIFOCTLSPI |= (VFIFO_FIFOCTLSPI_RXPAUSE_MASK | VFIFO_FIFOCTLSPI_TXPAUSE_MASK);
    }

    /* Flush the  TX and RX FIFO buffer. */
    SPI_FifoFlush(base, kSPI_FifoTx | kSPI_FifoRx);
    /* Set the TX and RX FIFO size to zero. */
    VFIFO->FIFOCFGSPI[instance] = VFIFO_FIFOCFGSPI_RXSIZE(0U) | VFIFO_FIFOCFGSPI_TXSIZE(0U);
    /* Update all the SPI FIFO size. */
    VFIFO->FIFOUPDATESPI |= VFIFO_FIFOUPDATESPI_SPI0RXUPDATESIZE_MASK | VFIFO_FIFOUPDATESPI_SPI0TXUPDATESIZE_MASK |
                            VFIFO_FIFOUPDATESPI_SPI1RXUPDATESIZE_MASK | VFIFO_FIFOUPDATESPI_SPI1TXUPDATESIZE_MASK;
    /* Disable all FIFO interrupts. */
    SPI_DisableFifoInterrupts(base, kSPI_AllFifoInterruptEnable);
    /* Clear all FIFO status flags, only receive timeout and bus error flag can be cleared. */
    SPI_ClearFifoStatusFlags(base, kSPI_RxFifoTimeOutFlag | kSPI_FifoBusErrorFlag);
    /* Unpause the system FIFO for transfer. */
    VFIFO->FIFOCTLSPI &= ~(VFIFO_FIFOCTLSPI_RXPAUSE_MASK | VFIFO_FIFOCTLSPI_TXPAUSE_MASK);

    /* Disable the TX/RX FIFO for tansfer. */
    switch (instance)
    {
        case 0:
            SYSCON->FIFOCTRL &= ~(SYSCON_FIFOCTRL_SPI0TXFIFOEN_MASK | SYSCON_FIFOCTRL_SPI0RXFIFOEN_MASK);
            break;
        case 1:
            SYSCON->FIFOCTRL &= ~(SYSCON_FIFOCTRL_SPI1TXFIFOEN_MASK | SYSCON_FIFOCTRL_SPI1RXFIFOEN_MASK);
            break;
        default:
            break;
    }
}
/* Check if TX FIFO enabled. */
bool SPI_IsTxFifoEnabled(SPI_Type *base)
{
    uint32_t instance = SPI_GetInstance(base);
    uint32_t fifosize = VFIFO->FIFOCFGSPI[instance];

    /* Check if system FIFO control TX is enabled. */
    switch (instance)
    {
        case 0:
            if (SYSCON->FIFOCTRL & SYSCON_FIFOCTRL_SPI0TXFIFOEN_MASK)
            {
                break;
            }
            else
            {
                return false;
            }
        case 1:
            if (SYSCON->FIFOCTRL & SYSCON_FIFOCTRL_SPI1TXFIFOEN_MASK)
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
    return (fifosize & VFIFO_FIFOCFGSPI_TXSIZE_MASK) ? (true) : (false);
}

/* Check if RX FIFO enabled. */
bool SPI_IsRxFifoEnabled(SPI_Type *base)
{
    uint32_t instance = SPI_GetInstance(base);
    uint32_t fifosize = VFIFO->FIFOCFGSPI[instance];

    /* Check if system FIFO control RX is enabled. */
    switch (instance)
    {
        case 0:
            if (SYSCON->FIFOCTRL & SYSCON_FIFOCTRL_SPI0RXFIFOEN_MASK)
            {
                break;
            }
            else
            {
                return false;
            }
        case 1:
            if (SYSCON->FIFOCTRL & SYSCON_FIFOCTRL_SPI1RXFIFOEN_MASK)
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
    return (fifosize & VFIFO_FIFOCFGSPI_RXSIZE_MASK) ? (true) : (false);
}

static void SPI_SendTransfer(SPI_Type *base, spi_master_handle_t *handle)
{
    uint32_t tx_ctrl = 0U, last_ctrl = 0U;
    uint32_t tmp32 = 0U;
    uint32_t instance = SPI_GetInstance(base);

    /* Select slave to talk with */
    tx_ctrl |= (SPI_DEASSERT_ALL & (~SPI_DEASSERT_SSELNUM(g_configs[instance].sselNum)));
    /* Set width of data - range asserted at entry */
    tx_ctrl |= SPI_TXDATCTL_LEN(handle->dataWidth);
    /* If the control flag is different with configuration. */
    if (handle->configFlags !=
        (base->TXCTL & SPI_TXDATCTL_RXIGNORE_MASK & SPI_TXDATCTL_EOT_MASK & SPI_TXDATCTL_EOF_MASK))
    {
        /* Delay end of frame.*/
        tx_ctrl |= (handle->configFlags & (uint32_t)kSPI_FrameDelay) ? (uint32_t)kSPI_FrameDelay : 0;
        /* Ignore the receive.*/
        tx_ctrl |= (handle->configFlags & SPI_TXDATCTL_RXIGNORE_MASK);
    }
    last_ctrl = tx_ctrl;
    if (handle->configFlags & SPI_TXDATCTL_EOT_MASK)
    {
        last_ctrl |= SPI_TXDATCTL_EOT_MASK;
    }

    if (handle->isTxFifoEnabled)
    {
        /* Fill data to tx fifo buffer until the tx threshold reached flag cleard. */
        while ((VFIFO->SPI[instance].STATSPI & VFIFO_SPI_STATSPI_TXTH_MASK) && (handle->txRemainingBytes))
        {
            if (handle->txData)
            {
                tmp32 = *(handle->txData++);
                handle->txRemainingBytes--;
                if (handle->dataWidth > 7U)
                {
                    tmp32 |= ((uint32_t)(*(handle->txData++)) << 8U);
                    handle->txRemainingBytes--;
                }
            }
            else
            {
                tmp32 = ((uint32_t)s_dummyData[instance] << 8U) | s_dummyData[instance];
                handle->txRemainingBytes--;
                if (handle->dataWidth > 7U)
                {
                    handle->txRemainingBytes--;
                }
            }
            /* If this transmit is the last to send, Set the control bits and disable Tx interrupt. */
            if (handle->txRemainingBytes == 0U)
            {
                VFIFO->SPI[instance].TXDATSPI = (tmp32 | last_ctrl);
                SPI_DisableFifoInterrupts(base, kSPI_TxFifoThresholdInterruptEnable);
            }
            else
            {
                VFIFO->SPI[instance].TXDATSPI = (tmp32 | tx_ctrl);
            }
        }
    }
    else
    {
        /* If transmit is ready, write data once to TXDATCTL register. */
        if ((kSPI_TxReadyFlag & SPI_GetStatusFlags(base)) && (handle->txRemainingBytes))
        {
            if (handle->txData)
            {
                tmp32 = *(handle->txData++);
                handle->txRemainingBytes--;
                if (handle->dataWidth > 7U)
                {
                    tmp32 |= ((uint32_t)(*(handle->txData++)) << 8U);
                    handle->txRemainingBytes--;
                }
            }
            else
            {
                tmp32 = ((uint32_t)s_dummyData[instance] << 8U) | s_dummyData[instance];
                handle->txRemainingBytes--;
                if (handle->dataWidth > 7U)
                {
                    handle->txRemainingBytes--;
                }
            }
            /* If this transmit is the last to send, Set the control bits and disable Tx interrupt. */
            if (handle->txRemainingBytes == 0U)
            {
                base->TXDATCTL = (tmp32 | last_ctrl);
                SPI_DisableInterrupts(base, kSPI_TxReadyInterruptEnable);
            }
            base->TXDATCTL = (tmp32 | tx_ctrl);
        }
    }
}

static void SPI_ReceiveTransfer(SPI_Type *base, spi_master_handle_t *handle)
{
    uint32_t tmp32 = 0U;
    uint32_t instance = SPI_GetInstance(base);

    if (handle->isRxFifoEnabled)
    {
        /* Read data from fifo buffer until the rx fifo is empty or to receive bytes is zero. */
        while ((!(VFIFO->SPI[instance].STATSPI & VFIFO_SPI_STATSPI_RXEMPTY_MASK)) && (handle->rxRemainingBytes))
        {
            tmp32 = VFIFO->SPI[instance].RXDATSPI;
            /* Check If receive buffer is NULL. */
            if (handle->rxData)
            {
                *(handle->rxData++) = tmp32;
                if (handle->dataWidth > 7U)
                {
                    *(handle->rxData++) = (tmp32 >> 8U);
                }
            }
            handle->rxRemainingBytes--;
            if (handle->dataWidth > 7U)
            {
                handle->rxRemainingBytes--;
            }
        }
    }
    else
    {
        /* If receive is ready, read data from RXDAT register. */
        if ((kSPI_RxReadyFlag & SPI_GetStatusFlags(base)) && (handle->rxRemainingBytes))
        {
            tmp32 = (base->RXDAT & 0x0000FFFFU);
            /* Check If receive buffer is NULL. */
            if (handle->rxData)
            {
                *(handle->rxData++) = tmp32;
                if (handle->dataWidth > 7U)
                {
                    *(handle->rxData++) = (tmp32 >> 8U);
                }
            }
            handle->rxRemainingBytes--;
            if (handle->dataWidth > 7U)
            {
                handle->rxRemainingBytes--;
            }
        }
    }
}

/* Set delay time for SPI transfer. */
void SPI_SetTransferDelay(SPI_Type *base, const spi_delay_config_t *config)
{
    assert(NULL != config);
    /* Clear the DLY register. */
    base->DLY &=
        ~(SPI_DLY_PRE_DELAY_MASK | SPI_DLY_POST_DELAY_MASK | SPI_DLY_FRAME_DELAY_MASK | SPI_DLY_TRANSFER_DELAY_MASK);
    /* Set the delay configuration. */
    base->DLY |= (SPI_DLY_PRE_DELAY(config->preDelay) | SPI_DLY_POST_DELAY(config->postDelay) |
                  SPI_DLY_FRAME_DELAY(config->frameDelay) | SPI_DLY_TRANSFER_DELAY(config->transferDelay));
}
void SPI_MasterGetDefaultConfig(spi_master_config_t *config)
{
    assert(NULL != config);

    config->enableLoopback = false;
    config->enableMaster = true;
    config->polarity = kSPI_ClockPolarityActiveHigh;
    config->phase = kSPI_ClockPhaseFirstEdge;
    config->direction = kSPI_MsbFirst;
    config->baudRate_Bps = 500000U;
    config->dataWidth = kSPI_Data8Bits;
    config->sselNum = kSPI_Ssel0;
    config->sselPol = kSPI_SpolActiveAllLow;
    config->fifoConfig.enableRxFifo = false;
    config->fifoConfig.enableTxFifo = false;
    config->fifoConfig.rxFifoSize = 8U;
    config->fifoConfig.txFifoSize = 8U;
    config->fifoConfig.rxFifoThreshold = 0U;
    config->fifoConfig.txFifoThreshold = 1U;
    config->delayConfig.frameDelay = 0U;
    config->delayConfig.postDelay = 0U;
    config->delayConfig.preDelay = 0U;
    config->delayConfig.transferDelay = 0U;
}

status_t SPI_MasterInit(SPI_Type *base, const spi_master_config_t *config, uint32_t srcClock_Hz)
{
    int32_t result = 0, instance = 0;
    uint32_t tmp = 0U;

    /* assert params */
    assert(!((NULL == base) || (NULL == config) || (0 == srcClock_Hz)));
    if ((NULL == base) || (NULL == config) || (0 == srcClock_Hz))
    {
        return kStatus_InvalidArgument;
    }

    /* Get instance number */
    instance = SPI_GetInstance(base);
    assert(instance >= 0);

    /* Config FIFOs if the system FIFO is enabled.
     * Note: The FIFO should be enabled before the baudrate has been configured.
     */
    if ((config->fifoConfig.enableRxFifo) || (config->fifoConfig.enableTxFifo))
    {
        SPI_EnableFifo(base, &(config->fifoConfig));
    }

    /* Enable clock. */
    CLOCK_EnableClock(s_spiClock[instance]);
    /* Reset spi periphera. */
    RESET_PeripheralReset(s_spiReset[instance]);

    /* set divider */
    result = SPI_MasterSetBaud(base, config->baudRate_Bps, srcClock_Hz);
    if (kStatus_Success != result)
    {
        return result;
    }

    /* configure SPI mode */
    tmp &= ~(SPI_CFG_MASTER_MASK | SPI_CFG_LSBF_MASK | SPI_CFG_CPHA_MASK | SPI_CFG_CPOL_MASK | SPI_CFG_LOOP_MASK |
             SPI_CFG_ENABLE_MASK | SPI_SSELPOL_MASK);
    /* phase */
    tmp |= SPI_CFG_CPHA(config->phase);
    /* polarity */
    tmp |= SPI_CFG_CPOL(config->polarity);
    /* direction */
    tmp |= SPI_CFG_LSBF(config->direction);
    /* master mode */
    tmp |= SPI_CFG_MASTER(1);
    /* loopback */
    tmp |= SPI_CFG_LOOP(config->enableLoopback);
    /* configure active level for all CS */
    tmp |= ((uint32_t)config->sselPol & (SPI_SSELPOL_MASK));
    base->CFG = tmp;

    /* Set delay configuration. */
    SPI_SetTransferDelay(base, &(config->delayConfig));

    /* store configuration */
    g_configs[instance].dataWidth = config->dataWidth;
    g_configs[instance].sselNum = config->sselNum;

    SPI_SetDummyData(base, (uint8_t)SPI_DUMMYDATA);

    /* Enable the SPI module. */
    SPI_Enable(base, config->enableMaster);
    return kStatus_Success;
}

void SPI_SlaveGetDefaultConfig(spi_slave_config_t *config)
{
    assert(NULL != config);

    config->enableSlave = true;
    config->polarity = kSPI_ClockPolarityActiveHigh;
    config->phase = kSPI_ClockPhaseFirstEdge;
    config->direction = kSPI_MsbFirst;
    config->dataWidth = kSPI_Data8Bits;
    config->sselPol = kSPI_SpolActiveAllLow;
    config->fifoConfig.enableRxFifo = false;
    config->fifoConfig.enableTxFifo = false;
    config->fifoConfig.rxFifoSize = 8U;
    config->fifoConfig.txFifoSize = 8U;
    config->fifoConfig.rxFifoThreshold = 0U;
    config->fifoConfig.txFifoThreshold = 0U;
}

status_t SPI_SlaveInit(SPI_Type *base, const spi_slave_config_t *config)
{
    int32_t instance = 0U;
    uint32_t tmp = 0U;

    /* assert params */
    assert(!((NULL == base) || (NULL == config)));
    if ((NULL == base) || (NULL == config))
    {
        return kStatus_InvalidArgument;
    }

    /* Get the instance of SPI. */
    instance = SPI_GetInstance(base);

    /* Config FIFOs if the system FIFO is enabled. */
    if ((config->fifoConfig.enableRxFifo) || (config->fifoConfig.enableTxFifo))
    {
        SPI_EnableFifo(base, &(config->fifoConfig));
    }

    /* Enable clock. */
    CLOCK_EnableClock(s_spiClock[instance]);
    /* Reset spi periphera. */
    RESET_PeripheralReset(s_spiReset[instance]);

    /* configure SPI mode */
    tmp &= ~(SPI_CFG_MASTER_MASK | SPI_CFG_LSBF_MASK | SPI_CFG_CPHA_MASK | SPI_CFG_CPOL_MASK | SPI_CFG_ENABLE_MASK |
             SPI_SSELPOL_MASK);
    /* phase */
    tmp |= SPI_CFG_CPHA(config->phase);
    /* polarity */
    tmp |= SPI_CFG_CPOL(config->polarity);
    /* direction */
    tmp |= SPI_CFG_LSBF(config->direction);
    /* configure active level for all CS */
    tmp |= ((uint32_t)config->sselPol & (SPI_SSELPOL_MASK));
    base->CFG = tmp;

    /* store configuration */
    g_configs[instance].dataWidth = config->dataWidth;

    SPI_SetDummyData(base, (uint8_t)SPI_DUMMYDATA);

    /* Enable the SPI module. */
    SPI_Enable(base, config->enableSlave);
    return kStatus_Success;
}

void SPI_Deinit(SPI_Type *base)
{
    /* Assert arguments */
    assert(NULL != base);
    uint32_t instance = SPI_GetInstance(base);

    /* Disable FIFO if enabled. */
    if ((SPI_IsTxFifoEnabled(base)) || (SPI_IsTxFifoEnabled(base)))
    {
        SPI_DisableFifo(base);
    }

    /* Disable SPI module before shutting down the clock. */
    SPI_Enable(base, false);
    /* Disable clock. */
    CLOCK_DisableClock(s_spiClock[instance]);
}

status_t SPI_MasterSetBaud(SPI_Type *base, uint32_t baudrate_Bps, uint32_t srcClock_Hz)
{
    uint32_t tmp;

    /* assert params */
    assert(!((NULL == base) || (0 == baudrate_Bps) || (0 == srcClock_Hz)));
    if ((NULL == base) || (0 == baudrate_Bps) || (0 == srcClock_Hz))
    {
        return kStatus_InvalidArgument;
    }

    /* calculate baudrate */
    tmp = (srcClock_Hz / baudrate_Bps) - 1;
    if (tmp > 0xFFFF)
    {
        return kStatus_SPI_BaudrateNotSupport;
    }
    base->DIV &= ~SPI_DIV_DIVVAL_MASK;
    base->DIV |= SPI_DIV_DIVVAL(tmp);
    return kStatus_Success;
}

void SPI_WriteData(SPI_Type *base, uint16_t data, uint32_t configFlags)
{
    uint32_t control = 0;
    int32_t instance;

    /* check params */
    assert(NULL != base);
    /* get and check instance */
    instance = SPI_GetInstance(base);
    assert(!(instance < 0));
    if (instance < 0)
    {
        return;
    }

    /* Set data width */
    control |= SPI_TXDATCTL_LEN(g_configs[instance].dataWidth);
    /* Set sssel */
    control |= (SPI_DEASSERT_ALL & (~SPI_DEASSERT_SSELNUM(g_configs[instance].sselNum)));
    /* Mask configFlags */
    control |= (configFlags & (SPI_TXDATCTL_EOT_MASK | SPI_TXDATCTL_EOF_MASK | SPI_TXDATCTL_RXIGNORE_MASK));
    /* Control should not affect lower 16 bits */
    assert(!(control & 0xFFFF));

    if (SPI_IsTxFifoEnabled(base))
    {
        VFIFO->SPI[instance].TXDATSPI = data | control;
    }
    else
    {
        base->TXDATCTL = (data | control);
    }
}

uint32_t SPI_ReadData(SPI_Type *base)
{
    assert(NULL != base);
    uint32_t instance = SPI_GetInstance(base);

    if (SPI_IsRxFifoEnabled(base))
    {
        return (VFIFO->SPI[instance].RXDATSPI);
    }
    else
    {
        return (base->RXDAT & 0x0000FFFFU);
    }
}
status_t SPI_MasterTransferCreateHandle(SPI_Type *base,
                                        spi_master_handle_t *handle,
                                        spi_master_callback_t callback,
                                        void *userData)
{
    int32_t instance = 0;

    /* check 'base' */
    assert(!(NULL == base));
    if (NULL == base)
    {
        return kStatus_InvalidArgument;
    }
    /* check 'handle' */
    assert(!(NULL == handle));
    if (NULL == handle)
    {
        return kStatus_InvalidArgument;
    }
    /* Get SPI instance by 'base' param */
    instance = SPI_GetInstance(base);
    if (instance < 0)
    {
        return kStatus_InvalidArgument;
    }

    memset(handle, 0, sizeof(*handle));

    handle->dataWidth = g_configs[instance].dataWidth;
    /* in slave mode, the sselNum is not important */
    handle->sselNum = g_configs[instance].sselNum;
    handle->isRxFifoEnabled = SPI_IsRxFifoEnabled(base);
    handle->isTxFifoEnabled = SPI_IsTxFifoEnabled(base);
    handle->rxFifoThreshold = SPI_FIFO_GETRXTHRESHOLD(base);
    handle->txFifoThreshold = SPI_FIFO_GETTXTHRESHOLD(base);
    handle->callback = callback;
    handle->userData = userData;

    s_spiHandle[instance] = handle;
    s_spiMasterIsr = SPI_MasterTransferHandleIRQ;
    /* Enable SPI NVIC */
    EnableIRQ(s_spiIRQ[instance]);

    return kStatus_Success;
}

status_t SPI_SlaveTransferCreateHandle(SPI_Type *base,
                                       spi_slave_handle_t *handle,
                                       spi_slave_callback_t callback,
                                       void *userData)
{
    SPI_MasterTransferCreateHandle(base, handle, callback, userData);
    s_spiSlaveIsr = SPI_SlaveTransferHandleIRQ;
    return kStatus_Success;
}

status_t SPI_MasterTransferBlocking(SPI_Type *base, spi_transfer_t *xfer)
{
    int32_t instance;
    uint32_t tx_ctrl = 0, last_ctrl = 0;
    uint32_t tmp32, remainingBytes, dataWidth;
    bool isTxFifoEnabled = SPI_IsTxFifoEnabled(base);
    bool isRxFifoEnabled = SPI_IsRxFifoEnabled(base);

    /* Check params */
    assert(!((NULL == base) || (NULL == xfer) || ((NULL == xfer->txData) && (NULL == xfer->rxData))));
    if ((NULL == base) || (NULL == xfer) || ((NULL == xfer->txData) && (NULL == xfer->rxData)))
    {
        return kStatus_InvalidArgument;
    }
    remainingBytes = xfer->dataSize;

    instance = SPI_GetInstance(base);
    assert(instance >= 0);
    dataWidth = g_configs[instance].dataWidth;

    /* DataSize (in bytes) is not aligned to 16bit (2B) transfer */
    if ((dataWidth > kSPI_Data8Bits) && (xfer->dataSize & 0x1))
    {
        return kStatus_InvalidArgument;
    }

    /* Select slave to talk with */
    tx_ctrl |= (SPI_DEASSERT_ALL & (~SPI_DEASSERT_SSELNUM(g_configs[instance].sselNum)));
    /* Set width of data - range asserted at entry */
    tx_ctrl |= SPI_TXDATCTL_LEN(dataWidth);
    /* Delay end of transfer */
    tx_ctrl |= (xfer->configFlags & (uint32_t)kSPI_FrameDelay) ? (uint32_t)kSPI_FrameDelay : 0;
    /* Ignore the receive.*/
    tx_ctrl |= (xfer->configFlags & SPI_TXDATCTL_RXIGNORE_MASK);

    last_ctrl = tx_ctrl;
    /* End of transfer */
    last_ctrl |= (xfer->configFlags & (uint32_t)kSPI_FrameAssert) ? (uint32_t)kSPI_FrameAssert : 0;

    if (!isTxFifoEnabled)
    {
        base->TXCTL = tx_ctrl;
    }

    /* Last index of loop */
    while (remainingBytes)
    {
        tmp32 = 0U;
        /* Wait for Transmit is ready. */
        if (isTxFifoEnabled)
        {
            while (!(VFIFO->SPI[instance].STATSPI & VFIFO_SPI_STATSPI_TXEMPTY_MASK))
            {
            }
        }
        else
        {
            while (!(base->STAT & SPI_STAT_TXRDY_MASK))
            {
            }
        }
        /* If txdata is not NULL. */
        if (xfer->txData)
        {
            tmp32 = *(xfer->txData++);
            if (dataWidth > 7U)
            {
                tmp32 |= ((uint32_t)(*(xfer->txData++))) << 8U;
            }
        }
        else
        {
            tmp32 = ((uint32_t)s_dummyData[instance] << 8U) | s_dummyData[instance];
        }
        if ((dataWidth > 7) ? (remainingBytes == 2U) : (remainingBytes == 1U))
        {
            if (isTxFifoEnabled)
            {
                VFIFO->SPI[instance].TXDATSPI = (tmp32 | last_ctrl);
            }
            else
            {
                base->TXDATCTL = (tmp32 | last_ctrl);
            }
        }
        else
        {
            if (isTxFifoEnabled)
            {
                VFIFO->SPI[instance].TXDATSPI = (tmp32 | tx_ctrl);
            }
            else
            {
                /* Write data to the Transmit register. */
                base->TXDAT = tmp32;
            }
        }

        /* Read data from the receive register. */
        if (isRxFifoEnabled)
        {
            while ((VFIFO->SPI[instance].STATSPI & VFIFO_SPI_STATSPI_RXEMPTY_MASK))
            {
            }
            tmp32 = VFIFO->SPI[instance].RXDATSPI;
        }
        else
        {
            while (!(base->STAT & SPI_STAT_RXRDY_MASK))
            {
            }
            tmp32 = base->RXDAT;
        }
        /* If receive buffer is NULL. */
        if (xfer->rxData)
        {
            *(xfer->rxData++) = tmp32;
            if (dataWidth > 7U)
            {
                *(xfer->rxData++) = (tmp32 >> 8U);
            }
        }

        remainingBytes--;
        if (dataWidth > 7U)
        {
            remainingBytes--;
        }
    }

    return kStatus_Success;
}

status_t SPI_MasterTransferNonBlocking(SPI_Type *base, spi_master_handle_t *handle, spi_transfer_t *xfer)
{
    /* check params */
    assert(
        !((NULL == base) || (NULL == handle) || (NULL == xfer) || ((NULL == xfer->txData) && (NULL == xfer->rxData))));
    if ((NULL == base) || (NULL == handle) || (NULL == xfer) || ((NULL == xfer->txData) && (NULL == xfer->rxData)))
    {
        return kStatus_InvalidArgument;
    }

    /* dataSize (in bytes) is not aligned to 16bit (2B) transfer */
    if ((handle->dataWidth > kSPI_Data8Bits) && (xfer->dataSize & 0x1))
    {
        return kStatus_InvalidArgument;
    }

    /* Check if SPI is busy */
    if (handle->state == kStatus_SPI_Busy)
    {
        return kStatus_SPI_Busy;
    }

    /* Set the handle information */
    handle->txData = xfer->txData;
    handle->rxData = xfer->rxData;
    /* Set count */
    handle->txRemainingBytes = xfer->dataSize;
    handle->rxRemainingBytes = xfer->dataSize;
    handle->totalByteCount = xfer->dataSize;
    /* Other options */
    handle->configFlags = xfer->configFlags;
    /* Set the SPI state to busy */
    handle->state = kStatus_SPI_Busy;
    /* FIFO options. */
    if (SPI_IsTxFifoEnabled(base))
    {
        SPI_EnableFifoInterrupts(base, kSPI_TxFifoThresholdInterruptEnable);
    }
    else
    {
        /* Enable generating txIRQ, first transfer is fired by TX ready interrupt. */
        SPI_EnableInterrupts(base, kSPI_TxReadyInterruptEnable);
    }
    if (SPI_IsRxFifoEnabled(base))
    {
        SPI_EnableFifoInterrupts(base, kSPI_RxFifoThresholdInterruptEnable);
    }
    else
    {
        /* Enable generating rxIRQ. */
        SPI_EnableInterrupts(base, kSPI_RxReadyInterruptEnable);
    }
    return kStatus_Success;
}

status_t SPI_MasterHalfDuplexTransferBlocking(SPI_Type *base, spi_half_duplex_transfer_t *xfer)
{
    assert(xfer);

    spi_transfer_t tempXfer = {0};
    status_t status;

    if (xfer->isTransmitFirst)
    {
        tempXfer.txData = xfer->txData;
        tempXfer.rxData = NULL;
        tempXfer.dataSize = xfer->txDataSize;
    }
    else
    {
        tempXfer.txData = NULL;
        tempXfer.rxData = xfer->rxData;
        tempXfer.dataSize = xfer->rxDataSize;
    }
    /* If the pcs pin keep assert between transmit and receive. */
    if (xfer->isPcsAssertInTransfer)
    {
        tempXfer.configFlags = (xfer->configFlags) & (uint32_t)(~kSPI_FrameAssert);
    }
    else
    {
        tempXfer.configFlags = (xfer->configFlags) | kSPI_FrameAssert;
    }

    status = SPI_MasterTransferBlocking(base, &tempXfer);

    if (status != kStatus_Success)
    {
        return status;
    }

    if (xfer->isTransmitFirst)
    {
        tempXfer.txData = NULL;
        tempXfer.rxData = xfer->rxData;
        tempXfer.dataSize = xfer->rxDataSize;
    }
    else
    {
        tempXfer.txData = xfer->txData;
        tempXfer.rxData = NULL;
        tempXfer.dataSize = xfer->txDataSize;
    }
    tempXfer.configFlags = xfer->configFlags;

    /* SPI transfer blocking. */
    status = SPI_MasterTransferBlocking(base, &tempXfer);

    return status;
}

status_t SPI_MasterHalfDuplexTransferNonBlocking(SPI_Type *base,
                                                 spi_master_handle_t *handle,
                                                 spi_half_duplex_transfer_t *xfer)
{
    assert(xfer);
    assert(handle);
    spi_transfer_t tempXfer = {0};
    status_t status;

    if (xfer->isTransmitFirst)
    {
        tempXfer.txData = xfer->txData;
        tempXfer.rxData = NULL;
        tempXfer.dataSize = xfer->txDataSize;
    }
    else
    {
        tempXfer.txData = NULL;
        tempXfer.rxData = xfer->rxData;
        tempXfer.dataSize = xfer->rxDataSize;
    }
    /* If the pcs pin keep assert between transmit and receive. */
    if (xfer->isPcsAssertInTransfer)
    {
        tempXfer.configFlags = (xfer->configFlags) & (uint32_t)(~kSPI_FrameAssert);
    }
    else
    {
        tempXfer.configFlags = (xfer->configFlags) | kSPI_FrameAssert;
    }

    status = SPI_MasterTransferBlocking(base, &tempXfer);
    if (status != kStatus_Success)
    {
        return status;
    }

    if (xfer->isTransmitFirst)
    {
        tempXfer.txData = NULL;
        tempXfer.rxData = xfer->rxData;
        tempXfer.dataSize = xfer->rxDataSize;
    }
    else
    {
        tempXfer.txData = xfer->txData;
        tempXfer.rxData = NULL;
        tempXfer.dataSize = xfer->txDataSize;
    }
    tempXfer.configFlags = xfer->configFlags;

    status = SPI_MasterTransferNonBlocking(base, handle, &tempXfer);

    return status;
}

status_t SPI_SlaveTransferNonBlocking(SPI_Type *base, spi_slave_handle_t *handle, spi_transfer_t *xfer)
{
    status_t status = 0U;

    status = SPI_MasterTransferNonBlocking(base, handle, xfer);
    if (kStatus_Success != status)
    {
        return status;
    }
    s_spiSlaveIsr = SPI_SlaveTransferHandleIRQ;

    return kStatus_Success;
}
status_t SPI_MasterTransferGetCount(SPI_Type *base, spi_master_handle_t *handle, size_t *count)
{
    assert(NULL != handle);

    if (!count)
    {
        return kStatus_InvalidArgument;
    }

    /* Catch when there is not an active transfer. */
    if (handle->state != kStatus_SPI_Busy)
    {
        *count = 0;
        return kStatus_NoTransferInProgress;
    }

    *count = handle->totalByteCount - handle->rxRemainingBytes;
    return kStatus_Success;
}

void SPI_MasterTransferAbort(SPI_Type *base, spi_master_handle_t *handle)
{
    assert(NULL != handle);

    /* Disable interrupt requests*/
    if (SPI_IsTxFifoEnabled(base))
    {
        SPI_DisableFifoInterrupts(base, kSPI_TxFifoThresholdInterruptEnable);
    }
    else
    {
        SPI_DisableInterrupts(base, kSPI_TxReadyInterruptEnable);
    }
    if (SPI_IsRxFifoEnabled(base))
    {
        SPI_DisableFifoInterrupts(base, kSPI_RxFifoThresholdInterruptEnable);
    }
    else
    {
        SPI_DisableInterrupts(base, kSPI_RxReadyInterruptEnable);
    }

    handle->state = kStatus_SPI_Idle;
    handle->txRemainingBytes = 0;
    handle->rxRemainingBytes = 0;
}

void SPI_MasterTransferHandleIRQ(SPI_Type *base, spi_master_handle_t *handle)
{
    assert((NULL != base) && (NULL != handle));
    uint32_t instance = SPI_GetInstance(base);

    /* IRQ behaviour:
     * - first interrupt is triggered by transmit ready interrupt or tx threshold reached if
     *   FIFO is enabled. The transfer function then tries read data and transmit data
     *   interleaved that results to strategy to process as many items as possible.
     * - last interrupt is triggered by receive data ready. The last state is
     *   known by empty rxBuffer and txBuffer. If there is nothing to receive
     *   or send - both operations have been finished and interrupts can be disabled.
     * - Note(case with the fifo used): Because the SPI0 and SPI1 share the same tx buffer and
     *   rx buffer, So the threshold interrupt may occured but the data in the buffer
     *   for SPIx may not reach the threshold separately if SPI0 and SPI1 are receiving
     *   data at the same time. The similar issue may happen when SPI0 and SPI1 are sending
     *   data at the same time with the FIFO used.
     */

    /* Data to send or read or expected to receive */
    if (handle->rxRemainingBytes)
    {
        SPI_ReceiveTransfer(base, handle);
    }
    if (handle->txRemainingBytes)
    {
        SPI_SendTransfer(base, handle);
    }
    if ((0U == handle->txRemainingBytes) && (0U == handle->rxRemainingBytes))
    {
        if (SPI_IsRxFifoEnabled(base))
        {
            SPI_EnableFifoInterrupts(base, kSPI_RxFifoThresholdInterruptEnable);
        }
        else
        {
            /* Disable TX and RX interrupt. */
            SPI_DisableInterrupts(base, kSPI_RxReadyInterruptEnable);
        }
        /* Set transfer state to idle. */
        handle->state = kStatus_SPI_Idle;
        /* If callback is not NULL, call this function. */
        if (handle->callback)
        {
            (handle->callback)(base, handle, handle->state, handle->userData);
        }
    }
    /* Check when the last receive, if the rx threshold can be triggered with the fifo enabled. */
    if (SPI_IsRxFifoEnabled(base))
    {
        /* If the bytes to receive is not larger then the rx threshold, the interrupt will not be triggerred.
         * So need to reload the rx threshold value to trigger the last receive.
         */
        if ((handle->rxRemainingBytes != 0U) && (handle->rxRemainingBytes <= handle->rxFifoThreshold))
        {
            VFIFO->SPI[instance].CFGSPI = ((VFIFO->SPI[instance].CFGSPI & ~VFIFO_SPI_CFGSPI_RXTHRESHOLD_MASK) |
                                           VFIFO_SPI_CFGSPI_RXTHRESHOLD(handle->rxRemainingBytes - 1));
        }
        /* If the rx threshold value in the register is not the default value in handle. Reload it to default value. */
        if ((handle->rxRemainingBytes == 0U) && (SPI_FIFO_GETRXTHRESHOLD(base) != handle->rxFifoThreshold))
        {
            VFIFO->SPI[instance].CFGSPI = ((VFIFO->SPI[instance].CFGSPI & ~VFIFO_SPI_CFGSPI_RXTHRESHOLD_MASK) |
                                           VFIFO_SPI_CFGSPI_RXTHRESHOLD(handle->rxFifoThreshold));
        }
    }
}

void SPI_SlaveTransferHandleIRQ(SPI_Type *base, spi_slave_handle_t *handle)
{
    assert((NULL != base) && (NULL != handle));
    uint32_t instance = SPI_GetInstance(base);

    /* IRQ behaviour:
     * - first interrupt is triggered by transmit ready interrupt or tx threshold reached if
     *   FIFO is enabled. The transfer function then tries read data and transmit data
     *   interleaved that results to strategy to process as many items as possible.
     * - last interrupt is triggered by receive data ready. The last state is
     *   known by empty rxBuffer and txBuffer. If there is nothing to receive
     *   or send - both operations have been finished and interrupts can be disabled.
     * - Note(case with the fifo used): Because the SPI0 and SPI1 share the same tx buffer and
     *   rx buffer, So the threshold interrupt may occured but the data in the buffer
     *   for SPIx may not reach the threshold separately if SPI0 and SPI1 are receiving
     *   data at the same time. The similar issue may happen when SPI0 and SPI1 are sending
     *   data at the same time with the FIFO used.
     */

    /* Data to send or read or expected to receive */
    if (handle->rxRemainingBytes)
    {
        SPI_ReceiveTransfer(base, handle);
    }
    if (handle->txRemainingBytes)
    {
        SPI_SendTransfer(base, handle);
    }

    if ((0U == handle->txRemainingBytes) && (0U == handle->rxRemainingBytes))
    {
        if (SPI_IsRxFifoEnabled(base))
        {
            SPI_EnableFifoInterrupts(base, kSPI_RxFifoThresholdInterruptEnable);
        }
        else
        {
            /* Disable TX and RX interrupt. */
            SPI_DisableInterrupts(base, kSPI_RxReadyInterruptEnable);
        }
        /* Set transfer state to idle. */
        handle->state = kStatus_SPI_Idle;
        /* If callback is not NULL, call this function. */
        if (handle->callback)
        {
            (handle->callback)(base, handle, handle->state, handle->userData);
        }
    }

    /* Check when the last receive, if the rx threshold can be triggered with the fifo enabled. */
    if (SPI_IsRxFifoEnabled(base))
    {
        /* If the bytes to receive is not larger then the rx threshold, the interrupt will not be triggerred.
         * So need to reload the rx threshold value to trigger the last receive.
         */
        if ((handle->rxRemainingBytes != 0U) && (handle->rxRemainingBytes <= handle->rxFifoThreshold))
        {
            VFIFO->SPI[instance].CFGSPI = ((VFIFO->SPI[instance].CFGSPI & ~VFIFO_SPI_CFGSPI_RXTHRESHOLD_MASK) |
                                           VFIFO_SPI_CFGSPI_RXTHRESHOLD(handle->rxRemainingBytes - 1));
        }
        /* If the rx threshold value in the register is not the default value in handle. Reload it to default value. */
        if ((handle->rxRemainingBytes == 0U) && (SPI_FIFO_GETRXTHRESHOLD(base) != handle->rxFifoThreshold))
        {
            VFIFO->SPI[instance].CFGSPI = ((VFIFO->SPI[instance].CFGSPI & ~VFIFO_SPI_CFGSPI_RXTHRESHOLD_MASK) |
                                           VFIFO_SPI_CFGSPI_RXTHRESHOLD(handle->rxFifoThreshold));
        }
    }
}

static void SPI_CommonIRQHandler(SPI_Type *base, void *param)
{
    if (SPI_IsMaster(base))
    {
        s_spiMasterIsr(base, (spi_master_handle_t *)param);
    }
    else
    {
        s_spiSlaveIsr(base, (spi_slave_handle_t *)param);
    }
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

#if defined(SPI0)
void SPI0_DriverIRQHandler(void)
{
    assert(s_spiHandle[0]);
    SPI_CommonIRQHandler(SPI0, s_spiHandle[0]);
}
#endif

#if defined(SPI1)
void SPI1_DriverIRQHandler(void)
{
    assert(s_spiHandle[1]);
    SPI_CommonIRQHandler(SPI1, s_spiHandle[1]);
}
#endif
