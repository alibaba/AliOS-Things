/*
 * The Clear BSD License
 * Copyright (c) 2016, NXP Semiconductor, Inc.
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used tom  endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
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

#include "wifi_common.h"
#include "athdefs.h"

#include "fsl_spi.h"
#include "fsl_spi_dma.h"
//#include "fsl_dmamux.h"
//#include "fsl_edma.h"
//#include "fsl_dspi_edma.h"
#include "wifi_spi.h"
#include "fsl_gpio.h"

static spi_dma_handle_t g_spi_edma_m_handle;
static dma_handle_t g_m_tx_handle;
static dma_handle_t g_m_rx_handle;
static spi_dma_handle_t g_m_dma_handle;
static spi_master_handle_t g_m_handle;

static kmutex_t spi_resource_mutex;
static ksem_t spi_transfer_event;
static int32_t g_dma_chunk = 1023;
//static enum IRQn g_dma_irqs[][FSL_FEATURE_EDMA_MODULE_CHANNEL] = DMA_CHN_IRQS;
static enum IRQn g_spi_irqs[] = SPI_IRQS;
static spi_master_config_t g_spi_config;

static SPI_Type *g_spi_base = NULL;
static uint32_t g_xfer_cs = 0;
static uint32_t g_irq_threshold = 0;

extern uint32_t SPI_GetInstance(SPI_Type *base);

/*! @brief Array to map DMA instance number to IRQ number. */
static const IRQn_Type s_dmaIRQNumber[] = DMA_IRQS;

/*! @brief Array to map DMA instance number to base pointer. */
static DMA_Type *const s_dmaBases[] = DMA_BASE_PTRS;

static int32_t DMA_GetInstance(DMA_Type *base)
{
    int32_t instance;
    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < ARRAY_SIZE(s_dmaBases); instance++)
    {
        if (s_dmaBases[instance] == base)
        {
            break;
        }
    }
    assert(instance < ARRAY_SIZE(s_dmaBases));
    return instance < ARRAY_SIZE(s_dmaBases) ? instance : -1;
}

/*
 * DMA handler, release transfer semaphore
 */
static void SPI_DMA_MasterUserCallback(SPI_Type *base, spi_dma_handle_t *handle, status_t status, void *userData)
{
    /* disable DMA requests before invoke callback */
    GPIO_WritePinOutput(GPIO, 1, 15, 1);
	krhino_sem_give(&spi_transfer_event);
}

/* 
 * IRQ handler, release transfer semaphore
 */
static void SPI_MasterUserCallback(SPI_Type *base, spi_master_handle_t *handle, status_t status, void *userData)
{
    /* disable IRQ requests before invoke callback */
    GPIO_WritePinOutput(GPIO, 1, 15, 1);
	krhino_sem_give(&spi_transfer_event);
}

/*
 * Initialize SPI IRQ mode
 */
static A_STATUS WIFIDRVS_SPI_InitIRQMode(WIFIDRVS_SPI_config_t *config)
{
    uint32_t spi_id = SPI_GetInstance(config->spi.base);
    NVIC_SetPriority(g_spi_irqs[spi_id], config->irq_mode.spi_irq_prio);

    /* SPI IRQ non-blocking handle */
    SPI_MasterTransferCreateHandle(config->spi.base, &g_m_handle, SPI_MasterUserCallback, NULL);

    return A_OK;
}
 
/*
 * Initialize SPI DMA mode
 */
static A_STATUS WIFIDRVS_SPI_InitDMAMode(WIFIDRVS_SPI_config_t *config)
{
    int32_t dmaInstance;
    DMA_Init(config->dma_mode.dma_base);
    DMA_EnableChannel(config->dma_mode.dma_base, config->dma_mode.dma_rx_chnl);
    DMA_EnableChannel(config->dma_mode.dma_base, config->dma_mode.dma_tx_chnl);
    DMA_SetChannelPriority(config->dma_mode.dma_base, config->dma_mode.dma_rx_chnl, config->dma_mode.dma_rx_chnl_prio);
    DMA_SetChannelPriority(config->dma_mode.dma_base, config->dma_mode.dma_tx_chnl, config->dma_mode.dma_tx_chnl_prio);
    DMA_CreateHandle(&g_m_rx_handle, config->dma_mode.dma_base, config->dma_mode.dma_rx_chnl);
    DMA_CreateHandle(&g_m_tx_handle, config->dma_mode.dma_base, config->dma_mode.dma_tx_chnl);
    SPI_MasterTransferCreateHandleDMA(config->spi.base, &g_m_dma_handle, SPI_DMA_MasterUserCallback, NULL, &g_m_tx_handle,
                                      &g_m_rx_handle);
    dmaInstance = DMA_GetInstance(config->dma_mode.dma_base);
    NVIC_SetPriority(s_dmaIRQNumber[dmaInstance], config->dma_mode.dma_irq_prio);
    return A_OK;
}

/*
 * Initialize SPI peripheral
 */
static A_STATUS WIFIDRVS_SPI_InitPeriph(
    SPI_Type *base,
    uint32_t src_clk_hz,
    uint32_t xfer_cs,
    spi_master_config_t *user_config
)
{
    assert(NULL != base);
    assert(NULL != user_config);

    krhino_mutex_create(&spi_resource_mutex, "spi_resource_mutex");

    krhino_sem_create(&spi_transfer_event, "spi_transfer_event", 0);

    /* DSPI init */
    SPI_MasterInit(base, &g_spi_config, src_clk_hz);

    return A_OK;
}

/*
 * Transfer data in DMA mode
 */
static A_STATUS WIFIDRVS_SPI_DMA_Transfer(spi_transfer_t *transfer)
{
    assert(NULL != transfer);
    GPIO_WritePinOutput(GPIO, 1, 15, 0);
    status_t result = SPI_MasterTransferDMA(g_spi_base, &g_m_dma_handle, transfer);
    if (kStatus_Success != result)
    {
        assert(0);
        return A_ERROR;
    }
    /* semaphore is released in callback fn */
    if (RHINO_SUCCESS != krhino_sem_take(&spi_transfer_event, RHINO_WAIT_FOREVER))
    {
        assert(0);
        return A_ERROR;
    }

//    SPI_EnableTxDMA(g_spi_base, false);
//    SPI_EnableRxDMA(g_spi_base, false);

    return A_OK;
}

/*
 * Transfer data in IRQ mode
 */
static A_STATUS WIFIDRVS_SPI_IRQ_Transfer(spi_transfer_t *transfer)
{
    assert(NULL != transfer);

    GPIO_WritePinOutput(GPIO, 1, 15, 0);
    status_t result = SPI_MasterTransferNonBlocking(g_spi_base, &g_m_handle, transfer);
    if (kStatus_Success != result)
    {
        assert(0);
        return A_ERROR;
    }
    if (RHINO_SUCCESS != krhino_sem_take(&spi_transfer_event,RHINO_WAIT_FOREVER))
    {
        assert(0);
        result = A_ERROR;
    }
    return A_OK;
}

/*
 * Transfer data
 */
static A_STATUS WIFIDRVS_SPI_Transfer(spi_transfer_t *transfer)
{
    A_STATUS result = A_OK;
    int32_t to_transfer;

    /* NOTE: following code expects that SDK drivers do not
     * modify members of 'transfer' argument */
    for (to_transfer = transfer->dataSize; to_transfer > 0;)
    {
        if (to_transfer < g_irq_threshold)
//        if (1)
        {
            /* DMA is unefficient for small amount of data, so use IRQ mode.
             * IRQ mode can transfer unlimited number of data */
            transfer->dataSize = to_transfer;
            transfer->configFlags |= kSPI_FrameAssert;
            transfer->configFlags |= SPI_TXDATCTL_EOT_MASK;
            result = WIFIDRVS_SPI_IRQ_Transfer(transfer);
            if (A_OK != result)
                break;
            to_transfer = 0;
        }
        else
        {
            /* DMA has bug !!!!!!!!!!!!!!!!!!! */
            /* DSPI over EDMA can transfer only limited number of bytes
             * so we have to split transfer into chunks */
            //transfer->dataSize = to_transfer < g_dma_chunk ? to_transfer : g_dma_chunk;
            // TODO: need to verify behaviour when transfer is splitted in default continuous mode
            // TODO: allows CS for every word ?
            if (to_transfer < g_dma_chunk)
            {
                transfer->dataSize = to_transfer;
                transfer->configFlags |= kSPI_FrameAssert;
            }
            else
            {
                transfer->dataSize = g_dma_chunk;
            }
            result = WIFIDRVS_SPI_DMA_Transfer(transfer);
            if (A_OK != result)
                break;
            to_transfer -= transfer->dataSize;
            /* recalculate rx/rx offsets */
            if (NULL != transfer->txData)
            {
                transfer->txData += transfer->dataSize;
            }
            if (NULL != transfer->rxData)
            {
                transfer->rxData += transfer->dataSize;
            }
        }
    }

    return result;
}

/*!
 * @brief Initialize SPI driver
 */
A_STATUS WIFIDRVS_SPI_Init(WIFIDRVS_SPI_config_t *config)
{
    /* No SPI base address, invalid config*/
    assert(!((NULL == config) || (NULL == config->spi.base)));
    if ((NULL == config) || (NULL == config->spi.base)) return A_ERROR;

    /* IRQ mode only - set threshold to max value */
    if ((config->irq_mode.enabled) && (!config->dma_mode.enabled))
    {
        g_irq_threshold = (uint32_t)-1;
    }
    /* DMA mode only - set threshold to 0 */
    else if ((!config->irq_mode.enabled) && (config->dma_mode.enabled))
    {
        g_irq_threshold = 0;
    }
    /* DMA and IRQ mode - set user defined value  */
    else if ((config->irq_mode.enabled) && (config->dma_mode.enabled))
    {
        g_irq_threshold = config->spi.irq_threshold;
    }
    /* Neither of modes is enabled, return error  */
    else
    {
        return A_ERROR;
    }

    /* Prepare driver internal context */
    g_spi_base = config->spi.base;
    g_xfer_cs = config->spi.xfer_cs;
    g_spi_config = config->spi.config;

    /* Initialize SPI peripheral */
    WIFIDRVS_SPI_InitPeriph(config->spi.base, config->spi.clk_hz, config->spi.xfer_cs, &config->spi.config);

    /* Enable IRQ mode */
    if (config->irq_mode.enabled)
    {
        WIFIDRVS_SPI_InitIRQMode(config);
    }

    /* Enable DMA mode */
    if (config->dma_mode.enabled)
    {
        WIFIDRVS_SPI_InitDMAMode(config);
    }

    return A_OK;
}

/*!
 * @brief Deinitialize SPI driver
 */
A_STATUS WIFIDRVS_SPI_Deinit(WIFIDRVS_SPI_config_t *config)
{
    assert(!(NULL == config));
    if (NULL == config) return A_ERROR;

    if (NULL == config->spi.base) return A_ERROR;
    SPI_Deinit(config->spi.base);

    return A_OK;
}

/*!
 * @brief Return default configuration
 */
A_STATUS WIFIDRVS_SPI_GetDefaultConfig(WIFIDRVS_SPI_config_t *config)
{
    assert(!(NULL == config));
    if (NULL == config) return A_ERROR;

    memset(config, 0, sizeof(*config));
    config->dma_mode.dma_rx_chnl = -1;
    config->dma_mode.dma_tx_chnl = -1;

    return A_OK;
}

/*!
 * @brief Return default SPI peripheral settings
 */
A_STATUS WIFIDRVS_SPI_GetSPIConfig(spi_master_config_t *user_config, uint32_t baudrate, spi_ssel_t cs)
{
    assert(!(NULL == user_config));
    if (NULL == user_config) return A_ERROR;

    memset(user_config, 0, sizeof(*user_config));
    SPI_MasterGetDefaultConfig(user_config);

    user_config->polarity = kSPI_ClockPolarityActiveLow;
    user_config->phase = kSPI_ClockPhaseSecondEdge;
    user_config->direction = kSPI_MsbFirst;
    user_config->baudRate_Bps = baudrate;
    user_config->dataWidth = kSPI_Data8Bits;
    user_config->sselNum = cs;
    user_config->sselPol = kSPI_SpolActiveAllLow;

    return A_OK;
}

/*!
 * @brief WiFi SPI transfer SPI
 */
A_STATUS WIFIDRVS_SPI_InOutToken(uint32_t OutToken, uint8_t DataSize, uint32_t *pInToken)
{
    A_STATUS result;
    spi_transfer_t transfer = {0};

    transfer.txData = (uint8_t *)&OutToken;
    transfer.rxData = (uint8_t *)pInToken;
    transfer.dataSize = DataSize;
    transfer.configFlags = g_xfer_cs;

    /* Protect transmit by spi_resource_mutex */
	
    if (RHINO_SUCCESS != krhino_mutex_lock(&spi_resource_mutex, RHINO_WAIT_FOREVER))
    {
        return A_ERROR;
    }
    result = WIFIDRVS_SPI_Transfer(&transfer);

    krhino_mutex_unlock(&spi_resource_mutex);
    return result;
}

/*!
 * @brief WiFi SPI transfer SPI
 */
A_STATUS WIFIDRVS_SPI_InOutBuffer(uint8_t *pBuffer, uint16_t length, uint8_t doRead, boolean sync)
{
    A_STATUS result;
    spi_transfer_t transfer = {0};

    if (doRead)
    {
        transfer.txData = NULL;
        transfer.rxData = pBuffer;
    }
    else
    {
        transfer.txData = pBuffer;
        transfer.rxData = NULL;
    }
    transfer.dataSize = length;
    transfer.configFlags = g_xfer_cs;

    /* Protect transmit by spi_resource_mutex */
    if (RHINO_SUCCESS != krhino_mutex_lock(&spi_resource_mutex, RHINO_WAIT_FOREVER))
    {
        return A_ERROR;
    }
    result = WIFIDRVS_SPI_Transfer(&transfer);
    krhino_mutex_unlock(&spi_resource_mutex);
    return result;
}

