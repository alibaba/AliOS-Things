/*
 * Copyright (c) 2013-2016 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.

 * Copyright (c) 2016, Freescale Semiconductor, Inc.
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
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
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

#include "fsl_usart_cmsis.h"

#define ARM_USART_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(2, 0)

/*
 * ARMCC does not support split the data section automatically, so the driver
 * needs to split the data to separate sections explicitly, to reduce codesize.
 */
#if defined(__CC_ARM)
#define ARMCC_SECTION(section_name) __attribute__((section(section_name)))
#endif

typedef const struct _cmsis_usart_resource
{
    USART_Type *base;          /*!< usart peripheral base address.       */
    uint32_t (*GetFreq)(void); /*!< Function to get the clock frequency. */
} cmsis_usart_resource_t;

typedef struct _cmsis_usart_non_blocking_driver_state
{
    cmsis_usart_resource_t *resource; /*!< Basic usart resource.      */
    usart_handle_t *handle;           /*!< Interupt transfer handle.  */
    ARM_USART_SignalEvent_t cb_event; /*!< Callback function.         */
    bool isInitialized;               /*!< Is initialized or not.     */
    bool isPowerOn;                   /*!< Is power on or not.        */
    bool isConfigured;                /*!< Configured to work or not. */
} cmsis_usart_non_blocking_driver_state_t;

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)
typedef const struct _cmsis_usart_dma_resource
{
    DMA_Type *txDmaBase;   /*!< DMA peripheral base address for TX.   */
    uint32_t txDmaChannel; /*!< DMA channel for usart TX.             */

    DMA_Type *rxDmaBase;   /*!< DMA peripheral base address for RX.   */
    uint32_t rxDmaChannel; /*!< DMA channel for usart RX.             */
} cmsis_usart_dma_resource_t;

typedef struct _cmsis_usart_dma_driver_state
{
    cmsis_usart_resource_t *resource;        /*!< usart basic resource.       */
    cmsis_usart_dma_resource_t *dmaResource; /*!< usart DMA resource.         */
    usart_dma_handle_t *handle;              /*!< usart DMA transfer handle.  */
    dma_handle_t *rxHandle;                  /*!< DMA RX handle.              */
    dma_handle_t *txHandle;                  /*!< DMA TX handle.              */
    ARM_USART_SignalEvent_t cb_event;        /*!< Callback function.          */
    bool isInitialized;                      /*!< Is initialized or not.      */
    bool isPowerOn;                          /*!< Is power on or not.         */
    bool isConfigured;                       /*!< Configured to work or not.  */
} cmsis_usart_dma_driver_state_t;
#endif /* FSL_FEATURE_SOC_DMA_COUNT */

enum _usart_transfer_states
{
    kUSART_TxIdle, /*!< TX idle. */
    kUSART_TxBusy, /*!< TX busy. */
    kUSART_RxIdle, /*!< RX idle. */
    kUSART_RxBusy  /*!< RX busy. */
};

/* Driver Version */
static const ARM_DRIVER_VERSION s_usartDriverVersion = {ARM_USART_API_VERSION, ARM_USART_DRV_VERSION};

static const ARM_USART_CAPABILITIES s_usartDriverCapabilities = {
    1, /* supports usart (Asynchronous) mode */
    0, /* supports Synchronous Master mode */
    0, /* supports Synchronous Slave mode */
    0, /* supports usart Single-wire mode */
    0, /* supports usart IrDA mode */
    0, /* supports usart Smart Card mode */
    0, /* Smart Card Clock generator */
    0, /* RTS Flow Control available */
    0, /* CTS Flow Control available */
    0, /* Transmit completed event: \ref ARM_USART_EVENT_TX_COMPLETE */
    0, /* Signal receive character timeout event: \ref ARM_USART_EVENT_RX_TIMEOUT */
    0, /* RTS Line: 0=not available, 1=available */
    0, /* CTS Line: 0=not available, 1=available */
    0, /* DTR Line: 0=not available, 1=available */
    0, /* DSR Line: 0=not available, 1=available */
    0, /* DCD Line: 0=not available, 1=available */
    0, /* RI Line: 0=not available, 1=available */
    0, /* Signal CTS change event: \ref ARM_USART_EVENT_CTS */
    0, /* Signal DSR change event: \ref ARM_USART_EVENT_DSR */
    0, /* Signal DCD change event: \ref ARM_USART_EVENT_DCD */
    0, /* Signal RI change event: \ref ARM_USART_EVENT_RI */
};

/*
 * Common control function used by usart_NonBlockingControl/usart_DmaControl/usart_EdmaControl
 */
static int32_t USART_CommonControl(uint32_t control, uint32_t arg, cmsis_usart_resource_t *resource, bool *isConfigured)
{
    usart_config_t config;

    USART_GetDefaultConfig(&config);

    switch (control & ARM_USART_CONTROL_Msk)
    {
        case ARM_USART_MODE_ASYNCHRONOUS:
            /* USART Baudrate */
            config.baudRate_Bps = arg;
            break;

        /* TX/RX IO is controlled in application layer. */
        case ARM_USART_CONTROL_TX:
            if (arg)
            {
                config.enableTx = true;
            }
            else
            {
                config.enableTx = false;
            }
            return ARM_DRIVER_OK;

        case ARM_USART_CONTROL_RX:
            if (arg)
            {
                config.enableRx = true;
            }
            else
            {
                config.enableRx = false;
            }

            return ARM_DRIVER_OK;

        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    switch (control & ARM_USART_PARITY_Msk)
    {
        case ARM_USART_PARITY_NONE:
            config.parityMode = kUSART_ParityDisabled;
            break;
        case ARM_USART_PARITY_EVEN:
            config.parityMode = kUSART_ParityEven;
            break;
        case ARM_USART_PARITY_ODD:
            config.parityMode = kUSART_ParityOdd;
            break;
        default:
            return ARM_USART_ERROR_PARITY;
    }

    switch (control & ARM_USART_STOP_BITS_Msk)
    {
        case ARM_USART_STOP_BITS_1:
            /* The GetDefaultConfig has already set for this case. */
            break;
        case ARM_USART_STOP_BITS_2:
            config.stopBitCount = kUSART_TwoStopBit;
            break;
        default:
            return ARM_USART_ERROR_STOP_BITS;
    }

    /* If usart is already configured, deinit it first. */
    if (*isConfigured)
    {
        USART_Deinit(resource->base);
        *isConfigured = false;
    }

    config.enableTx = true;
    config.enableRx = true;

    if (kStatus_USART_BaudrateNotSupport == USART_Init(resource->base, &config, resource->GetFreq()))
    {
        return ARM_USART_ERROR_BAUDRATE;
    }

    *isConfigured = true;

    return ARM_DRIVER_OK;
}

void KSDK_USART_NonBlockingCallback(USART_Type *base, usart_handle_t *handle, status_t status, void *userData)
{
    uint32_t event;

    if (kStatus_USART_TxIdle == status)
    {
        event = ARM_USART_EVENT_SEND_COMPLETE;
    }
    else if (kStatus_USART_RxIdle == status)
    {
        event = ARM_USART_EVENT_RECEIVE_COMPLETE;
    }

    /* User data is actually CMSIS driver callback. */
    if (userData)
    {
        ((ARM_USART_SignalEvent_t)userData)(event);
    }
}

static ARM_DRIVER_VERSION USARTx_GetVersion(void)
{
    return s_usartDriverVersion;
}

static ARM_USART_CAPABILITIES USARTx_GetCapabilities(void)
{
    return s_usartDriverCapabilities;
}

static int32_t USART_NonBlockingInitialize(ARM_USART_SignalEvent_t cb_event,
                                           cmsis_usart_non_blocking_driver_state_t *usart)
{
    if (usart->isInitialized)
    {
        /* Driver is already initialized */
        return ARM_DRIVER_OK;
    }

    usart->cb_event = cb_event;
    usart->isInitialized = true;

    return ARM_DRIVER_OK;
}

static int32_t USART_NonBlockingUninitialize(cmsis_usart_non_blocking_driver_state_t *usart)
{
    usart->isInitialized = false;
    return ARM_DRIVER_OK;
}

static int32_t USART_NonBlockingPowerControl(ARM_POWER_STATE state, cmsis_usart_non_blocking_driver_state_t *usart)
{
    usart_config_t config;

    switch (state)
    {
        case ARM_POWER_OFF:
            if (usart->isPowerOn)
            {
                USART_Deinit(usart->resource->base);
                usart->isPowerOn = false;
                usart->isConfigured = false;
            }
            break;
        case ARM_POWER_LOW:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        case ARM_POWER_FULL:
            /* Must be initialized first. */
            if (!usart->isInitialized)
            {
                return ARM_DRIVER_ERROR;
            }

            if (!usart->isPowerOn)
            {
                USART_GetDefaultConfig(&config);
                config.enableTx = true;
                config.enableRx = true;

                USART_Init(usart->resource->base, &config, usart->resource->GetFreq());
                USART_TransferCreateHandle(usart->resource->base, usart->handle, KSDK_USART_NonBlockingCallback,
                                           (void *)usart->cb_event);
                usart->isPowerOn = true;
                usart->isConfigured = true;
            }
            break;
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

static int32_t USART_NonBlockingSend(const void *data, uint32_t num, cmsis_usart_non_blocking_driver_state_t *usart)
{
    int32_t ret;
    status_t status;
    usart_transfer_t xfer;

    xfer.data = (uint8_t *)data;
    xfer.dataSize = num;

    status = USART_TransferSendNonBlocking(usart->resource->base, usart->handle, &xfer);

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_USART_TxBusy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t USART_NonBlockingReceive(void *data, uint32_t num, cmsis_usart_non_blocking_driver_state_t *usart)
{
    int32_t ret;
    status_t status;
    usart_transfer_t xfer;

    xfer.data = data;
    xfer.dataSize = num;

    status = USART_TransferReceiveNonBlocking(usart->resource->base, usart->handle, &xfer, NULL);

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_USART_RxBusy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t USART_NonBlockingTransfer(const void *data_out,
                                         void *data_in,
                                         uint32_t num,
                                         cmsis_usart_non_blocking_driver_state_t *usart)
{
    /* Only in synchronous mode */
    return ARM_DRIVER_ERROR;
}

static uint32_t USART_NonBlockingGetTxCount(cmsis_usart_non_blocking_driver_state_t *usart)
{
    uint32_t cnt;

    /* If TX not in progress, then the TX count is txDataSizeAll saved in handle. */
    if (kUSART_TxIdle == usart->handle->txState)
    {
        cnt = usart->handle->txDataSizeAll;
    }
    else
    {
        cnt = usart->handle->txDataSizeAll - usart->handle->txDataSize;
    }

    return cnt;
}

static uint32_t USART_NonBlockingGetRxCount(cmsis_usart_non_blocking_driver_state_t *usart)
{
    uint32_t cnt;

    if (kUSART_RxIdle == usart->handle->rxState)
    {
        cnt = usart->handle->rxDataSizeAll;
    }
    else
    {
        cnt = usart->handle->rxDataSizeAll - usart->handle->rxDataSize;
    }

    return cnt;
}

static int32_t USART_NonBlockingControl(uint32_t control, uint32_t arg, cmsis_usart_non_blocking_driver_state_t *usart)
{
    /* Must be power on. */
    if (!usart->isPowerOn)
    {
        return ARM_DRIVER_ERROR;
    }

    /* Does not support these features. */
    if (control & (ARM_USART_FLOW_CONTROL_Msk | ARM_USART_CPOL_Msk | ARM_USART_CPHA_Msk))
    {
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    switch (control & ARM_USART_CONTROL_Msk)
    {
        /* Abort Send */
        case ARM_USART_ABORT_SEND:
            usart->resource->base->FIFOINTENSET &= ~USART_FIFOINTENSET_TXLVL_MASK;
            usart->handle->txDataSize = 0;
            usart->handle->txState = kUSART_TxIdle;
            return ARM_DRIVER_OK;

        /* Abort receive */
        case ARM_USART_ABORT_RECEIVE:
            usart->resource->base->FIFOINTENSET &= ~USART_FIFOINTENSET_RXLVL_MASK;
            usart->handle->rxDataSize = 0U;
            usart->handle->rxState = kUSART_RxIdle;
            return ARM_DRIVER_OK;

        default:
            break;
    }

    return USART_CommonControl(control, arg, usart->resource, &usart->isConfigured);
}

static ARM_USART_STATUS USART_NonBlockingGetStatus(cmsis_usart_non_blocking_driver_state_t *usart)
{
    ARM_USART_STATUS stat;
    uint32_t ksdk_usart_status = usart->resource->base->STAT;

    stat.tx_busy = ((kUSART_TxBusy == usart->handle->txState) ? (1U) : (0U));
    stat.rx_busy = ((kUSART_RxBusy == usart->handle->rxState) ? (1U) : (0U));

    stat.tx_underflow = 0U;
    stat.rx_overflow = 0U;

    stat.rx_break = (!(!(ksdk_usart_status & USART_STAT_RXBRK_MASK)));

    stat.rx_framing_error = (!(!(ksdk_usart_status & USART_STAT_FRAMERRINT_MASK)));
    stat.rx_parity_error = (!(!(ksdk_usart_status & USART_STAT_PARITYERRINT_MASK)));

    return stat;
}

static int32_t USARTx_SetModemControl(ARM_USART_MODEM_CONTROL control)
{
    return ARM_DRIVER_ERROR_UNSUPPORTED;
}

static ARM_USART_MODEM_STATUS USARTx_GetModemStatus(void)
{
    ARM_USART_MODEM_STATUS modem_status;

    modem_status.cts = 0U;
    modem_status.dsr = 0U;
    modem_status.ri = 0U;
    modem_status.dcd = 0U;

    return modem_status;
}

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)
void KSDK_USART_DmaCallback(USART_Type *base, usart_dma_handle_t *handle, status_t status, void *userData)
{
    uint32_t event;

    if (kStatus_USART_TxIdle == status)
    {
        event = ARM_USART_EVENT_SEND_COMPLETE;
    }

    if (kStatus_USART_RxIdle == status)
    {
        event = ARM_USART_EVENT_RECEIVE_COMPLETE;
    }

    /* User data is actually CMSIS driver callback. */
    if (userData)
    {
        ((ARM_USART_SignalEvent_t)userData)(event);
    }
}

static int32_t USART_DmaInitialize(ARM_USART_SignalEvent_t cb_event, cmsis_usart_dma_driver_state_t *usart)
{
    if (usart->isInitialized)
    {
        /* Driver is already initialized */
        return ARM_DRIVER_OK;
    }

    usart->cb_event = cb_event;
    usart->isInitialized = true;

    return ARM_DRIVER_OK;
}

static int32_t USART_DmaUninitialize(cmsis_usart_dma_driver_state_t *usart)
{
    usart->isInitialized = false;
    return ARM_DRIVER_OK;
}

static int32_t USART_DmaPowerControl(ARM_POWER_STATE state, cmsis_usart_dma_driver_state_t *usart)
{
    usart_config_t config;

    switch (state)
    {
        case ARM_POWER_OFF:
            if (usart->isPowerOn)
            {
                USART_Deinit(usart->resource->base);
                DMA_DisableChannel(usart->dmaResource->rxDmaBase, usart->dmaResource->rxDmaChannel);
                DMA_DisableChannel(usart->dmaResource->txDmaBase, usart->dmaResource->txDmaChannel);
                usart->isPowerOn = false;
                usart->isConfigured = false;
            }
            break;
        case ARM_POWER_LOW:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
        case ARM_POWER_FULL:
            /* Must be initialized first. */
            if (!usart->isInitialized)
            {
                return ARM_DRIVER_ERROR;
            }

            if (!usart->isPowerOn)
            {
                USART_GetDefaultConfig(&config);
                config.enableTx = true;
                config.enableRx = true;

                /* Set up DMA setting. */
                DMA_EnableChannel(usart->dmaResource->txDmaBase, usart->dmaResource->txDmaChannel);
                DMA_EnableChannel(usart->dmaResource->rxDmaBase, usart->dmaResource->rxDmaChannel);

                DMA_CreateHandle(usart->rxHandle, usart->dmaResource->rxDmaBase, usart->dmaResource->rxDmaChannel);
                DMA_CreateHandle(usart->txHandle, usart->dmaResource->txDmaBase, usart->dmaResource->txDmaChannel);

                /* Setup the usart. */
                USART_Init(usart->resource->base, &config, usart->resource->GetFreq());
                USART_TransferCreateHandleDMA(usart->resource->base, usart->handle, KSDK_USART_DmaCallback,
                                              (void *)usart->cb_event, usart->txHandle, usart->rxHandle);

                usart->isPowerOn = true;
                usart->isConfigured = true;
            }
            break;
        default:
            return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    return ARM_DRIVER_OK;
}

static int32_t USART_DmaSend(const void *data, uint32_t num, cmsis_usart_dma_driver_state_t *usart)
{
    int32_t ret;
    status_t status;
    usart_transfer_t xfer;

    xfer.data = (uint8_t *)data;
    xfer.dataSize = num;

    status = USART_TransferSendDMA(usart->resource->base, usart->handle, &xfer);

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_USART_TxBusy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t USART_DmaReceive(void *data, uint32_t num, cmsis_usart_dma_driver_state_t *usart)
{
    int32_t ret;
    status_t status;
    usart_transfer_t xfer;

    xfer.data = data;
    xfer.dataSize = num;

    status = USART_TransferReceiveDMA(usart->resource->base, usart->handle, &xfer);

    switch (status)
    {
        case kStatus_Success:
            ret = ARM_DRIVER_OK;
            break;
        case kStatus_InvalidArgument:
            ret = ARM_DRIVER_ERROR_PARAMETER;
            break;
        case kStatus_USART_RxBusy:
            ret = ARM_DRIVER_ERROR_BUSY;
            break;
        default:
            ret = ARM_DRIVER_ERROR;
            break;
    }

    return ret;
}

static int32_t USART_DmaTransfer(const void *data_out,
                                 void *data_in,
                                 uint32_t num,
                                 cmsis_usart_dma_driver_state_t *usart)
{
    /* Only in synchronous mode */
    return ARM_DRIVER_ERROR;
}

static int32_t USART_DmaGetTxCount(cmsis_usart_dma_driver_state_t *usart)
{
    /* Does not support */
    return ARM_DRIVER_ERROR;
}

static int32_t USART_DmaGetRxCount(cmsis_usart_dma_driver_state_t *usart)
{
    /* Does not support */
    return ARM_DRIVER_ERROR;
}

static int32_t USART_DmaControl(uint32_t control, uint32_t arg, cmsis_usart_dma_driver_state_t *usart)
{
    /* Must be power on. */
    if (!usart->isPowerOn)
    {
        return ARM_DRIVER_ERROR;
    }

    /* Does not support these features. */
    if (control & (ARM_USART_FLOW_CONTROL_Msk | ARM_USART_CPOL_Msk | ARM_USART_CPHA_Msk))
    {
        return ARM_DRIVER_ERROR_UNSUPPORTED;
    }

    switch (control & ARM_USART_CONTROL_Msk)
    {
        /* Abort Send */
        case ARM_USART_ABORT_SEND:
            USART_EnableTxDMA(usart->resource->base, false);
            DMA_AbortTransfer(usart->handle->txDmaHandle);
            usart->handle->txState = kUSART_TxIdle;
            return ARM_DRIVER_OK;

        /* Abort receive */
        case ARM_USART_ABORT_RECEIVE:
            USART_EnableRxDMA(usart->resource->base, false);
            DMA_AbortTransfer(usart->handle->rxDmaHandle);
            usart->handle->rxState = kUSART_RxIdle;
            return ARM_DRIVER_OK;

        default:
            break;
    }

    return USART_CommonControl(control, arg, usart->resource, &usart->isConfigured);
}

static ARM_USART_STATUS USART_DmaGetStatus(cmsis_usart_dma_driver_state_t *usart)
{
    ARM_USART_STATUS stat;
    uint32_t ksdk_usart_status = usart->resource->base->STAT;

    stat.tx_busy = ((kUSART_TxBusy == usart->handle->txState) ? (1U) : (0U));
    stat.rx_busy = ((kUSART_RxBusy == usart->handle->rxState) ? (1U) : (0U));

    stat.tx_underflow = 0U;
    stat.rx_overflow = 0U;

    stat.rx_break = (!(!(ksdk_usart_status & USART_STAT_RXBRK_MASK)));

    stat.rx_framing_error = (!(!(ksdk_usart_status & USART_STAT_FRAMERRINT_MASK)));
    stat.rx_parity_error = (!(!(ksdk_usart_status & USART_STAT_PARITYERRINT_MASK)));

    return stat;
}
#endif

#if defined(USART0)

extern uint32_t USART0_GetFreq(void);
extern void USART0_InitPins(void);
extern void USART0_DeinitPins(void);

cmsis_usart_resource_t usart0_Resource = {USART0, USART0_GetFreq};

usart_handle_t USART0_Handle;

#if defined(__CC_ARM)
ARMCC_SECTION("usart0_non_blocking_driver_state")
cmsis_usart_non_blocking_driver_state_t usart0_NonBlockingDriverState = {
#else
cmsis_usart_non_blocking_driver_state_t usart0_NonBlockingDriverState = {
#endif
    &usart0_Resource, &USART0_Handle,
};

static int32_t USART0_NonBlockingInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART0_InitPins();
    return USART_NonBlockingInitialize(cb_event, &usart0_NonBlockingDriverState);
}

static int32_t USART0_NonBlockingUninitialize(void)
{
    USART0_DeinitPins();
    return USART_NonBlockingUninitialize(&usart0_NonBlockingDriverState);
}

static int32_t USART0_NonBlockingPowerControl(ARM_POWER_STATE state)
{
    return USART_NonBlockingPowerControl(state, &usart0_NonBlockingDriverState);
}

static int32_t USART0_NonBlockingSend(const void *data, uint32_t num)
{
    return USART_NonBlockingSend(data, num, &usart0_NonBlockingDriverState);
}

static int32_t USART0_NonBlockingReceive(void *data, uint32_t num)
{
    return USART_NonBlockingReceive(data, num, &usart0_NonBlockingDriverState);
}

static int32_t USART0_NonBlockingTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_NonBlockingTransfer(data_out, data_in, num, &usart0_NonBlockingDriverState);
}

static uint32_t USART0_NonBlockingGetTxCount(void)
{
    return USART_NonBlockingGetTxCount(&usart0_NonBlockingDriverState);
}

static uint32_t USART0_NonBlockingGetRxCount(void)
{
    return USART_NonBlockingGetRxCount(&usart0_NonBlockingDriverState);
}

static int32_t USART0_NonBlockingControl(uint32_t control, uint32_t arg)
{
    return USART_NonBlockingControl(control, arg, &usart0_NonBlockingDriverState);
}

static ARM_USART_STATUS USART0_NonBlockingGetStatus(void)
{
    return USART_NonBlockingGetStatus(&usart0_NonBlockingDriverState);
}

/* usart0 Driver Control Block */
ARM_DRIVER_USART USART0_NonBlocking_Driver = {USARTx_GetVersion,
                                              USARTx_GetCapabilities,
                                              USART0_NonBlockingInitialize,
                                              USART0_NonBlockingUninitialize,
                                              USART0_NonBlockingPowerControl,
                                              USART0_NonBlockingSend,
                                              USART0_NonBlockingReceive,
                                              USART0_NonBlockingTransfer,
                                              USART0_NonBlockingGetTxCount,
                                              USART0_NonBlockingGetRxCount,
                                              USART0_NonBlockingControl,
                                              USART0_NonBlockingGetStatus,
                                              USARTx_SetModemControl,
                                              USARTx_GetModemStatus};

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)

cmsis_usart_dma_resource_t usart0_DmaResource = {
    RTE_USART0_DMA_TX_DMA_BASE, RTE_USART0_DMA_TX_CH, RTE_USART0_DMA_RX_DMA_BASE, RTE_USART0_DMA_RX_CH,
};

usart_dma_handle_t USART0_DmaHandle;
dma_handle_t USART0_DmaRxHandle;
dma_handle_t USART0_DmaTxHandle;

#if defined(__CC_ARM)
ARMCC_SECTION("usart0_dma_driver_state")
cmsis_usart_dma_driver_state_t usart0_DmaDriverState = {
#else
cmsis_usart_dma_driver_state_t usart0_DmaDriverState = {
#endif
    &usart0_Resource, &usart0_DmaResource, &USART0_DmaHandle, &USART0_DmaRxHandle, &USART0_DmaTxHandle,
};

static int32_t USART0_DmaInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART0_InitPins();
    return USART_DmaInitialize(cb_event, &usart0_DmaDriverState);
}

static int32_t USART0_DmaUninitialize(void)
{
    USART0_DeinitPins();
    return USART_DmaUninitialize(&usart0_DmaDriverState);
}

static int32_t USART0_DmaPowerControl(ARM_POWER_STATE state)
{
    return USART_DmaPowerControl(state, &usart0_DmaDriverState);
}

static int32_t USART0_DmaSend(const void *data, uint32_t num)
{
    return USART_DmaSend(data, num, &usart0_DmaDriverState);
}

static int32_t USART0_DmaReceive(void *data, uint32_t num)
{
    return USART_DmaReceive(data, num, &usart0_DmaDriverState);
}

static int32_t USART0_DmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_DmaTransfer(data_out, data_in, num, &usart0_DmaDriverState);
}

static uint32_t USART0_DmaGetTxCount(void)
{
    return USART_DmaGetTxCount(&usart0_DmaDriverState);
}

static uint32_t USART0_DmaGetRxCount(void)
{
    return USART_DmaGetRxCount(&usart0_DmaDriverState);
}

static int32_t USART0_DmaControl(uint32_t control, uint32_t arg)
{
    return USART_DmaControl(control, arg, &usart0_DmaDriverState);
}

static ARM_USART_STATUS USART0_DmaGetStatus(void)
{
    return USART_DmaGetStatus(&usart0_DmaDriverState);
}

/* usart0 Driver Control Block */
ARM_DRIVER_USART USART0_Dma_Driver = {
    USARTx_GetVersion, USARTx_GetCapabilities, USART0_DmaInitialize,   USART0_DmaUninitialize, USART0_DmaPowerControl,
    USART0_DmaSend,    USART0_DmaReceive,      USART0_DmaTransfer,     USART0_DmaGetTxCount,   USART0_DmaGetRxCount,
    USART0_DmaControl, USART0_DmaGetStatus,    USARTx_SetModemControl, USARTx_GetModemStatus};
#endif /* FSL_FEATURE_SOC_DMA_COUNT */

#endif /* usart0 */

#if defined(USART1)

extern uint32_t USART1_GetFreq(void);
extern void USART1_InitPins(void);
extern void USART1_DeinitPins(void);

cmsis_usart_resource_t usart1_Resource = {USART1, USART1_GetFreq};

usart_handle_t USART1_Handle;

#if defined(__CC_ARM)
ARMCC_SECTION("usart1_non_blocking_driver_state")
cmsis_usart_non_blocking_driver_state_t usart1_NonBlockingDriverState = {
#else
cmsis_usart_non_blocking_driver_state_t usart1_NonBlockingDriverState = {
#endif
    &usart1_Resource, &USART1_Handle,
};

static int32_t USART1_NonBlockingInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART1_InitPins();
    return USART_NonBlockingInitialize(cb_event, &usart1_NonBlockingDriverState);
}

static int32_t USART1_NonBlockingUninitialize(void)
{
    USART1_DeinitPins();
    return USART_NonBlockingUninitialize(&usart1_NonBlockingDriverState);
}

static int32_t USART1_NonBlockingPowerControl(ARM_POWER_STATE state)
{
    return USART_NonBlockingPowerControl(state, &usart1_NonBlockingDriverState);
}

static int32_t USART1_NonBlockingSend(const void *data, uint32_t num)
{
    return USART_NonBlockingSend(data, num, &usart1_NonBlockingDriverState);
}

static int32_t USART1_NonBlockingReceive(void *data, uint32_t num)
{
    return USART_NonBlockingReceive(data, num, &usart1_NonBlockingDriverState);
}

static int32_t USART1_NonBlockingTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_NonBlockingTransfer(data_out, data_in, num, &usart1_NonBlockingDriverState);
}

static uint32_t USART1_NonBlockingGetTxCount(void)
{
    return USART_NonBlockingGetTxCount(&usart1_NonBlockingDriverState);
}

static uint32_t USART1_NonBlockingGetRxCount(void)
{
    return USART_NonBlockingGetRxCount(&usart1_NonBlockingDriverState);
}

static int32_t USART1_NonBlockingControl(uint32_t control, uint32_t arg)
{
    return USART_NonBlockingControl(control, arg, &usart1_NonBlockingDriverState);
}

static ARM_USART_STATUS USART1_NonBlockingGetStatus(void)
{
    return USART_NonBlockingGetStatus(&usart1_NonBlockingDriverState);
}

/* usart1 Driver Control Block */
ARM_DRIVER_USART USART1_NonBlocking_Driver = {USARTx_GetVersion,
                                              USARTx_GetCapabilities,
                                              USART1_NonBlockingInitialize,
                                              USART1_NonBlockingUninitialize,
                                              USART1_NonBlockingPowerControl,
                                              USART1_NonBlockingSend,
                                              USART1_NonBlockingReceive,
                                              USART1_NonBlockingTransfer,
                                              USART1_NonBlockingGetTxCount,
                                              USART1_NonBlockingGetRxCount,
                                              USART1_NonBlockingControl,
                                              USART1_NonBlockingGetStatus,
                                              USARTx_SetModemControl,
                                              USARTx_GetModemStatus};

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)

cmsis_usart_dma_resource_t usart1_DmaResource = {
    RTE_USART1_DMA_TX_DMA_BASE, RTE_USART1_DMA_TX_CH, RTE_USART1_DMA_RX_DMA_BASE, RTE_USART1_DMA_RX_CH,
};

usart_dma_handle_t USART1_DmaHandle;
dma_handle_t USART1_DmaRxHandle;
dma_handle_t USART1_DmaTxHandle;

#if defined(__CC_ARM)
ARMCC_SECTION("usart1_dma_driver_state")
cmsis_usart_dma_driver_state_t usart1_DmaDriverState = {
#else
cmsis_usart_dma_driver_state_t usart1_DmaDriverState = {
#endif
    &usart1_Resource, &usart1_DmaResource, &USART1_DmaHandle, &USART1_DmaRxHandle, &USART1_DmaTxHandle,
};

static int32_t USART1_DmaInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART1_InitPins();
    return USART_DmaInitialize(cb_event, &usart1_DmaDriverState);
}

static int32_t USART1_DmaUninitialize(void)
{
    USART1_DeinitPins();
    return USART_DmaUninitialize(&usart1_DmaDriverState);
}

static int32_t USART1_DmaPowerControl(ARM_POWER_STATE state)
{
    return USART_DmaPowerControl(state, &usart1_DmaDriverState);
}

static int32_t USART1_DmaSend(const void *data, uint32_t num)
{
    return USART_DmaSend(data, num, &usart1_DmaDriverState);
}

static int32_t USART1_DmaReceive(void *data, uint32_t num)
{
    return USART_DmaReceive(data, num, &usart1_DmaDriverState);
}

static int32_t USART1_DmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_DmaTransfer(data_out, data_in, num, &usart1_DmaDriverState);
}

static uint32_t USART1_DmaGetTxCount(void)
{
    return USART_DmaGetTxCount(&usart1_DmaDriverState);
}

static uint32_t USART1_DmaGetRxCount(void)
{
    return USART_DmaGetRxCount(&usart1_DmaDriverState);
}

static int32_t USART1_DmaControl(uint32_t control, uint32_t arg)
{
    return USART_DmaControl(control, arg, &usart1_DmaDriverState);
}

static ARM_USART_STATUS USART1_DmaGetStatus(void)
{
    return USART_DmaGetStatus(&usart1_DmaDriverState);
}

/* usart1 Driver Control Block */
ARM_DRIVER_USART USART1_Dma_Driver = {
    USARTx_GetVersion, USARTx_GetCapabilities, USART1_DmaInitialize,   USART1_DmaUninitialize, USART1_DmaPowerControl,
    USART1_DmaSend,    USART1_DmaReceive,      USART1_DmaTransfer,     USART1_DmaGetTxCount,   USART1_DmaGetRxCount,
    USART1_DmaControl, USART1_DmaGetStatus,    USARTx_SetModemControl, USARTx_GetModemStatus};
#endif /* FSL_FEATURE_SOC_DMA_COUNT */

#endif /* usart1 */

#if defined(USART2)

extern uint32_t USART2_GetFreq(void);
extern void USART2_InitPins(void);
extern void USART2_DeinitPins(void);

cmsis_usart_resource_t usart2_Resource = {USART2, USART2_GetFreq};

usart_handle_t USART2_Handle;

#if defined(__CC_ARM)
ARMCC_SECTION("usart2_non_blocking_driver_state")
cmsis_usart_non_blocking_driver_state_t usart2_NonBlockingDriverState = {
#else
cmsis_usart_non_blocking_driver_state_t usart2_NonBlockingDriverState = {
#endif
    &usart2_Resource, &USART2_Handle,
};

static int32_t USART2_NonBlockingInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART2_InitPins();
    return USART_NonBlockingInitialize(cb_event, &usart2_NonBlockingDriverState);
}

static int32_t USART2_NonBlockingUninitialize(void)
{
    USART2_DeinitPins();
    return USART_NonBlockingUninitialize(&usart2_NonBlockingDriverState);
}

static int32_t USART2_NonBlockingPowerControl(ARM_POWER_STATE state)
{
    return USART_NonBlockingPowerControl(state, &usart2_NonBlockingDriverState);
}

static int32_t USART2_NonBlockingSend(const void *data, uint32_t num)
{
    return USART_NonBlockingSend(data, num, &usart2_NonBlockingDriverState);
}

static int32_t USART2_NonBlockingReceive(void *data, uint32_t num)
{
    return USART_NonBlockingReceive(data, num, &usart2_NonBlockingDriverState);
}

static int32_t USART2_NonBlockingTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_NonBlockingTransfer(data_out, data_in, num, &usart2_NonBlockingDriverState);
}

static uint32_t USART2_NonBlockingGetTxCount(void)
{
    return USART_NonBlockingGetTxCount(&usart2_NonBlockingDriverState);
}

static uint32_t USART2_NonBlockingGetRxCount(void)
{
    return USART_NonBlockingGetRxCount(&usart2_NonBlockingDriverState);
}

static int32_t USART2_NonBlockingControl(uint32_t control, uint32_t arg)
{
    return USART_NonBlockingControl(control, arg, &usart2_NonBlockingDriverState);
}

static ARM_USART_STATUS USART2_NonBlockingGetStatus(void)
{
    return USART_NonBlockingGetStatus(&usart2_NonBlockingDriverState);
}

/* usart2 Driver Control Block */
ARM_DRIVER_USART USART2_NonBlocking_Driver = {USARTx_GetVersion,
                                              USARTx_GetCapabilities,
                                              USART2_NonBlockingInitialize,
                                              USART2_NonBlockingUninitialize,
                                              USART2_NonBlockingPowerControl,
                                              USART2_NonBlockingSend,
                                              USART2_NonBlockingReceive,
                                              USART2_NonBlockingTransfer,
                                              USART2_NonBlockingGetTxCount,
                                              USART2_NonBlockingGetRxCount,
                                              USART2_NonBlockingControl,
                                              USART2_NonBlockingGetStatus,
                                              USARTx_SetModemControl,
                                              USARTx_GetModemStatus};

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)

cmsis_usart_dma_resource_t usart2_DmaResource = {
    RTE_USART2_DMA_TX_DMA_BASE, RTE_USART2_DMA_TX_CH, RTE_USART2_DMA_RX_DMA_BASE, RTE_USART2_DMA_RX_CH,
};

usart_dma_handle_t USART2_DmaHandle;
dma_handle_t USART2_DmaRxHandle;
dma_handle_t USART2_DmaTxHandle;

#if defined(__CC_ARM)
ARMCC_SECTION("usart2_dma_driver_state")
cmsis_usart_dma_driver_state_t usart2_DmaDriverState = {
#else
cmsis_usart_dma_driver_state_t usart2_DmaDriverState = {
#endif
    &usart2_Resource, &usart2_DmaResource, &USART2_DmaHandle, &USART2_DmaRxHandle, &USART2_DmaTxHandle,
};

static int32_t USART2_DmaInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART2_InitPins();
    return USART_DmaInitialize(cb_event, &usart2_DmaDriverState);
}

static int32_t USART2_DmaUninitialize(void)
{
    USART2_DeinitPins();
    return USART_DmaUninitialize(&usart2_DmaDriverState);
}

static int32_t USART2_DmaPowerControl(ARM_POWER_STATE state)
{
    return USART_DmaPowerControl(state, &usart2_DmaDriverState);
}

static int32_t USART2_DmaSend(const void *data, uint32_t num)
{
    return USART_DmaSend(data, num, &usart2_DmaDriverState);
}

static int32_t USART2_DmaReceive(void *data, uint32_t num)
{
    return USART_DmaReceive(data, num, &usart2_DmaDriverState);
}

static int32_t USART2_DmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_DmaTransfer(data_out, data_in, num, &usart2_DmaDriverState);
}

static uint32_t USART2_DmaGetTxCount(void)
{
    return USART_DmaGetTxCount(&usart2_DmaDriverState);
}

static uint32_t USART2_DmaGetRxCount(void)
{
    return USART_DmaGetRxCount(&usart2_DmaDriverState);
}

static int32_t USART2_DmaControl(uint32_t control, uint32_t arg)
{
    return USART_DmaControl(control, arg, &usart2_DmaDriverState);
}

static ARM_USART_STATUS USART2_DmaGetStatus(void)
{
    return USART_DmaGetStatus(&usart2_DmaDriverState);
}

/* usart2 Driver Control Block */
ARM_DRIVER_USART USART2_Dma_Driver = {
    USARTx_GetVersion, USARTx_GetCapabilities, USART2_DmaInitialize,   USART2_DmaUninitialize, USART2_DmaPowerControl,
    USART2_DmaSend,    USART2_DmaReceive,      USART2_DmaTransfer,     USART2_DmaGetTxCount,   USART2_DmaGetRxCount,
    USART2_DmaControl, USART2_DmaGetStatus,    USARTx_SetModemControl, USARTx_GetModemStatus};
#endif /* FSL_FEATURE_SOC_DMA_COUNT */

#endif /* usart2 */

#if defined(USART3)

extern uint32_t USART3_GetFreq(void);
extern void USART3_InitPins(void);
extern void USART3_DeinitPins(void);

cmsis_usart_resource_t usart3_Resource = {USART3, USART3_GetFreq};

usart_handle_t USART3_Handle;

#if defined(__CC_ARM)
ARMCC_SECTION("usart3_non_blocking_driver_state")
cmsis_usart_non_blocking_driver_state_t usart3_NonBlockingDriverState = {
#else
cmsis_usart_non_blocking_driver_state_t usart3_NonBlockingDriverState = {
#endif
    &usart3_Resource, &USART3_Handle,
};

static int32_t USART3_NonBlockingInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART3_InitPins();
    return USART_NonBlockingInitialize(cb_event, &usart3_NonBlockingDriverState);
}

static int32_t USART3_NonBlockingUninitialize(void)
{
    USART3_DeinitPins();
    return USART_NonBlockingUninitialize(&usart3_NonBlockingDriverState);
}

static int32_t USART3_NonBlockingPowerControl(ARM_POWER_STATE state)
{
    return USART_NonBlockingPowerControl(state, &usart3_NonBlockingDriverState);
}

static int32_t USART3_NonBlockingSend(const void *data, uint32_t num)
{
    return USART_NonBlockingSend(data, num, &usart3_NonBlockingDriverState);
}

static int32_t USART3_NonBlockingReceive(void *data, uint32_t num)
{
    return USART_NonBlockingReceive(data, num, &usart3_NonBlockingDriverState);
}

static int32_t USART3_NonBlockingTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_NonBlockingTransfer(data_out, data_in, num, &usart3_NonBlockingDriverState);
}

static uint32_t USART3_NonBlockingGetTxCount(void)
{
    return USART_NonBlockingGetTxCount(&usart3_NonBlockingDriverState);
}

static uint32_t USART3_NonBlockingGetRxCount(void)
{
    return USART_NonBlockingGetRxCount(&usart3_NonBlockingDriverState);
}

static int32_t USART3_NonBlockingControl(uint32_t control, uint32_t arg)
{
    return USART_NonBlockingControl(control, arg, &usart3_NonBlockingDriverState);
}

static ARM_USART_STATUS USART3_NonBlockingGetStatus(void)
{
    return USART_NonBlockingGetStatus(&usart3_NonBlockingDriverState);
}

/* usart3 Driver Control Block */
ARM_DRIVER_USART USART3_NonBlocking_Driver = {USARTx_GetVersion,
                                              USARTx_GetCapabilities,
                                              USART3_NonBlockingInitialize,
                                              USART3_NonBlockingUninitialize,
                                              USART3_NonBlockingPowerControl,
                                              USART3_NonBlockingSend,
                                              USART3_NonBlockingReceive,
                                              USART3_NonBlockingTransfer,
                                              USART3_NonBlockingGetTxCount,
                                              USART3_NonBlockingGetRxCount,
                                              USART3_NonBlockingControl,
                                              USART3_NonBlockingGetStatus,
                                              USARTx_SetModemControl,
                                              USARTx_GetModemStatus};

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)

cmsis_usart_dma_resource_t usart3_DmaResource = {
    RTE_USART3_DMA_TX_DMA_BASE, RTE_USART3_DMA_TX_CH, RTE_USART3_DMA_RX_DMA_BASE, RTE_USART3_DMA_RX_CH,
};

usart_dma_handle_t USART3_DmaHandle;
dma_handle_t USART3_DmaRxHandle;
dma_handle_t USART3_DmaTxHandle;

#if defined(__CC_ARM)
ARMCC_SECTION("usart3_dma_driver_state")
cmsis_usart_dma_driver_state_t usart3_DmaDriverState = {
#else
cmsis_usart_dma_driver_state_t usart3_DmaDriverState = {
#endif
    &usart3_Resource, &usart3_DmaResource, &USART3_DmaHandle, &USART3_DmaRxHandle, &USART3_DmaTxHandle,
};

static int32_t USART3_DmaInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART3_InitPins();
    return USART_DmaInitialize(cb_event, &usart3_DmaDriverState);
}

static int32_t USART3_DmaUninitialize(void)
{
    USART3_DeinitPins();
    return USART_DmaUninitialize(&usart3_DmaDriverState);
}

static int32_t USART3_DmaPowerControl(ARM_POWER_STATE state)
{
    return USART_DmaPowerControl(state, &usart3_DmaDriverState);
}

static int32_t USART3_DmaSend(const void *data, uint32_t num)
{
    return USART_DmaSend(data, num, &usart3_DmaDriverState);
}

static int32_t USART3_DmaReceive(void *data, uint32_t num)
{
    return USART_DmaReceive(data, num, &usart3_DmaDriverState);
}

static int32_t USART3_DmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_DmaTransfer(data_out, data_in, num, &usart3_DmaDriverState);
}

static uint32_t USART3_DmaGetTxCount(void)
{
    return USART_DmaGetTxCount(&usart3_DmaDriverState);
}

static uint32_t USART3_DmaGetRxCount(void)
{
    return USART_DmaGetRxCount(&usart3_DmaDriverState);
}

static int32_t USART3_DmaControl(uint32_t control, uint32_t arg)
{
    return USART_DmaControl(control, arg, &usart3_DmaDriverState);
}

static ARM_USART_STATUS USART3_DmaGetStatus(void)
{
    return USART_DmaGetStatus(&usart3_DmaDriverState);
}

/* usart3 Driver Control Block */
ARM_DRIVER_USART USART3_Dma_Driver = {
    USARTx_GetVersion, USARTx_GetCapabilities, USART3_DmaInitialize,   USART3_DmaUninitialize, USART3_DmaPowerControl,
    USART3_DmaSend,    USART3_DmaReceive,      USART3_DmaTransfer,     USART3_DmaGetTxCount,   USART3_DmaGetRxCount,
    USART3_DmaControl, USART3_DmaGetStatus,    USARTx_SetModemControl, USARTx_GetModemStatus};
#endif /* FSL_FEATURE_SOC_DMA_COUNT */

#endif /* usart3 */

#if defined(USART4)

extern uint32_t USART4_GetFreq(void);
extern void USART4_InitPins(void);
extern void USART4_DeinitPins(void);

cmsis_usart_resource_t usart4_Resource = {USART4, USART4_GetFreq};

usart_handle_t USART4_Handle;

#if defined(__CC_ARM)
ARMCC_SECTION("usart4_non_blocking_driver_state")
cmsis_usart_non_blocking_driver_state_t usart4_NonBlockingDriverState = {
#else
cmsis_usart_non_blocking_driver_state_t usart4_NonBlockingDriverState = {
#endif
    &usart4_Resource, &USART4_Handle,
};

static int32_t USART4_NonBlockingInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART4_InitPins();
    return USART_NonBlockingInitialize(cb_event, &usart4_NonBlockingDriverState);
}

static int32_t USART4_NonBlockingUninitialize(void)
{
    USART4_DeinitPins();
    return USART_NonBlockingUninitialize(&usart4_NonBlockingDriverState);
}

static int32_t USART4_NonBlockingPowerControl(ARM_POWER_STATE state)
{
    return USART_NonBlockingPowerControl(state, &usart4_NonBlockingDriverState);
}

static int32_t USART4_NonBlockingSend(const void *data, uint32_t num)
{
    return USART_NonBlockingSend(data, num, &usart4_NonBlockingDriverState);
}

static int32_t USART4_NonBlockingReceive(void *data, uint32_t num)
{
    return USART_NonBlockingReceive(data, num, &usart4_NonBlockingDriverState);
}

static int32_t USART4_NonBlockingTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_NonBlockingTransfer(data_out, data_in, num, &usart4_NonBlockingDriverState);
}

static uint32_t USART4_NonBlockingGetTxCount(void)
{
    return USART_NonBlockingGetTxCount(&usart4_NonBlockingDriverState);
}

static uint32_t USART4_NonBlockingGetRxCount(void)
{
    return USART_NonBlockingGetRxCount(&usart4_NonBlockingDriverState);
}

static int32_t USART4_NonBlockingControl(uint32_t control, uint32_t arg)
{
    return USART_NonBlockingControl(control, arg, &usart4_NonBlockingDriverState);
}

static ARM_USART_STATUS USART4_NonBlockingGetStatus(void)
{
    return USART_NonBlockingGetStatus(&usart4_NonBlockingDriverState);
}

/* usart4 Driver Control Block */
ARM_DRIVER_USART USART4_NonBlocking_Driver = {USARTx_GetVersion,
                                              USARTx_GetCapabilities,
                                              USART4_NonBlockingInitialize,
                                              USART4_NonBlockingUninitialize,
                                              USART4_NonBlockingPowerControl,
                                              USART4_NonBlockingSend,
                                              USART4_NonBlockingReceive,
                                              USART4_NonBlockingTransfer,
                                              USART4_NonBlockingGetTxCount,
                                              USART4_NonBlockingGetRxCount,
                                              USART4_NonBlockingControl,
                                              USART4_NonBlockingGetStatus,
                                              USARTx_SetModemControl,
                                              USARTx_GetModemStatus};

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)

cmsis_usart_dma_resource_t usart4_DmaResource = {
    RTE_USART4_DMA_TX_DMA_BASE, RTE_USART4_DMA_TX_CH, RTE_USART4_DMA_RX_DMA_BASE, RTE_USART4_DMA_RX_CH,
};

usart_dma_handle_t USART4_DmaHandle;
dma_handle_t USART4_DmaRxHandle;
dma_handle_t USART4_DmaTxHandle;

#if defined(__CC_ARM)
ARMCC_SECTION("usart4_dma_driver_state")
cmsis_usart_dma_driver_state_t usart4_DmaDriverState = {
#else
cmsis_usart_dma_driver_state_t usart4_DmaDriverState = {
#endif
    &usart4_Resource, &usart4_DmaResource, &USART4_DmaHandle, &USART4_DmaRxHandle, &USART4_DmaTxHandle,
};

static int32_t USART4_DmaInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART4_InitPins();
    return USART_DmaInitialize(cb_event, &usart4_DmaDriverState);
}

static int32_t USART4_DmaUninitialize(void)
{
    USART4_DeinitPins();
    return USART_DmaUninitialize(&usart4_DmaDriverState);
}

static int32_t USART4_DmaPowerControl(ARM_POWER_STATE state)
{
    return USART_DmaPowerControl(state, &usart4_DmaDriverState);
}

static int32_t USART4_DmaSend(const void *data, uint32_t num)
{
    return USART_DmaSend(data, num, &usart4_DmaDriverState);
}

static int32_t USART4_DmaReceive(void *data, uint32_t num)
{
    return USART_DmaReceive(data, num, &usart4_DmaDriverState);
}

static int32_t USART4_DmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_DmaTransfer(data_out, data_in, num, &usart4_DmaDriverState);
}

static uint32_t USART4_DmaGetTxCount(void)
{
    return USART_DmaGetTxCount(&usart4_DmaDriverState);
}

static uint32_t USART4_DmaGetRxCount(void)
{
    return USART_DmaGetRxCount(&usart4_DmaDriverState);
}

static int32_t USART4_DmaControl(uint32_t control, uint32_t arg)
{
    return USART_DmaControl(control, arg, &usart4_DmaDriverState);
}

static ARM_USART_STATUS USART4_DmaGetStatus(void)
{
    return USART_DmaGetStatus(&usart4_DmaDriverState);
}

/* usart4 Driver Control Block */
ARM_DRIVER_USART USART4_Dma_Driver = {
    USARTx_GetVersion, USARTx_GetCapabilities, USART4_DmaInitialize,   USART4_DmaUninitialize, USART4_DmaPowerControl,
    USART4_DmaSend,    USART4_DmaReceive,      USART4_DmaTransfer,     USART4_DmaGetTxCount,   USART4_DmaGetRxCount,
    USART4_DmaControl, USART4_DmaGetStatus,    USARTx_SetModemControl, USARTx_GetModemStatus};
#endif /* FSL_FEATURE_SOC_DMA_COUNT */

#endif /* usart4 */

#if defined(USART5)

extern uint32_t USART5_GetFreq(void);
extern void USART5_InitPins(void);
extern void USART5_DeinitPins(void);

cmsis_usart_resource_t usart5_Resource = {USART5, USART5_GetFreq};

usart_handle_t USART5_Handle;

#if defined(__CC_ARM)
ARMCC_SECTION("usart5_non_blocking_driver_state")
cmsis_usart_non_blocking_driver_state_t usart5_NonBlockingDriverState = {
#else
cmsis_usart_non_blocking_driver_state_t usart5_NonBlockingDriverState = {
#endif
    &usart5_Resource, &USART5_Handle,
};

static int32_t USART5_NonBlockingInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART5_InitPins();
    return USART_NonBlockingInitialize(cb_event, &usart5_NonBlockingDriverState);
}

static int32_t USART5_NonBlockingUninitialize(void)
{
    USART5_DeinitPins();
    return USART_NonBlockingUninitialize(&usart5_NonBlockingDriverState);
}

static int32_t USART5_NonBlockingPowerControl(ARM_POWER_STATE state)
{
    return USART_NonBlockingPowerControl(state, &usart5_NonBlockingDriverState);
}

static int32_t USART5_NonBlockingSend(const void *data, uint32_t num)
{
    return USART_NonBlockingSend(data, num, &usart5_NonBlockingDriverState);
}

static int32_t USART5_NonBlockingReceive(void *data, uint32_t num)
{
    return USART_NonBlockingReceive(data, num, &usart5_NonBlockingDriverState);
}

static int32_t USART5_NonBlockingTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_NonBlockingTransfer(data_out, data_in, num, &usart5_NonBlockingDriverState);
}

static uint32_t USART5_NonBlockingGetTxCount(void)
{
    return USART_NonBlockingGetTxCount(&usart5_NonBlockingDriverState);
}

static uint32_t USART5_NonBlockingGetRxCount(void)
{
    return USART_NonBlockingGetRxCount(&usart5_NonBlockingDriverState);
}

static int32_t USART5_NonBlockingControl(uint32_t control, uint32_t arg)
{
    return USART_NonBlockingControl(control, arg, &usart5_NonBlockingDriverState);
}

static ARM_USART_STATUS USART5_NonBlockingGetStatus(void)
{
    return USART_NonBlockingGetStatus(&usart5_NonBlockingDriverState);
}

/* usart5 Driver Control Block */
ARM_DRIVER_USART USART5_NonBlocking_Driver = {USARTx_GetVersion,
                                              USARTx_GetCapabilities,
                                              USART5_NonBlockingInitialize,
                                              USART5_NonBlockingUninitialize,
                                              USART5_NonBlockingPowerControl,
                                              USART5_NonBlockingSend,
                                              USART5_NonBlockingReceive,
                                              USART5_NonBlockingTransfer,
                                              USART5_NonBlockingGetTxCount,
                                              USART5_NonBlockingGetRxCount,
                                              USART5_NonBlockingControl,
                                              USART5_NonBlockingGetStatus,
                                              USARTx_SetModemControl,
                                              USARTx_GetModemStatus};

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)

cmsis_usart_dma_resource_t usart5_DmaResource = {
    RTE_USART5_DMA_TX_DMA_BASE, RTE_USART5_DMA_TX_CH, RTE_USART5_DMA_RX_DMA_BASE, RTE_USART5_DMA_RX_CH,
};

usart_dma_handle_t USART5_DmaHandle;
dma_handle_t USART5_DmaRxHandle;
dma_handle_t USART5_DmaTxHandle;

#if defined(__CC_ARM)
ARMCC_SECTION("usart5_dma_driver_state")
cmsis_usart_dma_driver_state_t usart5_DmaDriverState = {
#else
cmsis_usart_dma_driver_state_t usart5_DmaDriverState = {
#endif
    &usart5_Resource, &usart5_DmaResource, &USART5_DmaHandle, &USART5_DmaRxHandle, &USART5_DmaTxHandle,
};

static int32_t USART5_DmaInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART5_InitPins();
    return USART_DmaInitialize(cb_event, &usart5_DmaDriverState);
}

static int32_t USART5_DmaUninitialize(void)
{
    USART5_DeinitPins();
    return USART_DmaUninitialize(&usart5_DmaDriverState);
}

static int32_t USART5_DmaPowerControl(ARM_POWER_STATE state)
{
    return USART_DmaPowerControl(state, &usart5_DmaDriverState);
}

static int32_t USART5_DmaSend(const void *data, uint32_t num)
{
    return USART_DmaSend(data, num, &usart5_DmaDriverState);
}

static int32_t USART5_DmaReceive(void *data, uint32_t num)
{
    return USART_DmaReceive(data, num, &usart5_DmaDriverState);
}

static int32_t USART5_DmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_DmaTransfer(data_out, data_in, num, &usart5_DmaDriverState);
}

static uint32_t USART5_DmaGetTxCount(void)
{
    return USART_DmaGetTxCount(&usart5_DmaDriverState);
}

static uint32_t USART5_DmaGetRxCount(void)
{
    return USART_DmaGetRxCount(&usart5_DmaDriverState);
}

static int32_t USART5_DmaControl(uint32_t control, uint32_t arg)
{
    return USART_DmaControl(control, arg, &usart5_DmaDriverState);
}

static ARM_USART_STATUS USART5_DmaGetStatus(void)
{
    return USART_DmaGetStatus(&usart5_DmaDriverState);
}

/* usart5 Driver Control Block */
ARM_DRIVER_USART USART5_Dma_Driver = {
    USARTx_GetVersion, USARTx_GetCapabilities, USART5_DmaInitialize,   USART5_DmaUninitialize, USART5_DmaPowerControl,
    USART5_DmaSend,    USART5_DmaReceive,      USART5_DmaTransfer,     USART5_DmaGetTxCount,   USART5_DmaGetRxCount,
    USART5_DmaControl, USART5_DmaGetStatus,    USARTx_SetModemControl, USARTx_GetModemStatus};
#endif /* FSL_FEATURE_SOC_DMA_COUNT */

#endif /* usart5 */

#if defined(USART6)

extern uint32_t USART6_GetFreq(void);
extern void USART6_InitPins(void);
extern void USART6_DeinitPins(void);

cmsis_usart_resource_t usart6_Resource = {USART6, USART6_GetFreq};

usart_handle_t USART6_Handle;

#if defined(__CC_ARM)
ARMCC_SECTION("usart6_non_blocking_driver_state")
cmsis_usart_non_blocking_driver_state_t usart6_NonBlockingDriverState = {
#else
cmsis_usart_non_blocking_driver_state_t usart6_NonBlockingDriverState = {
#endif
    &usart6_Resource, &USART6_Handle,
};

static int32_t USART6_NonBlockingInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART6_InitPins();
    return USART_NonBlockingInitialize(cb_event, &usart6_NonBlockingDriverState);
}

static int32_t USART6_NonBlockingUninitialize(void)
{
    USART6_DeinitPins();
    return USART_NonBlockingUninitialize(&usart6_NonBlockingDriverState);
}

static int32_t USART6_NonBlockingPowerControl(ARM_POWER_STATE state)
{
    return USART_NonBlockingPowerControl(state, &usart6_NonBlockingDriverState);
}

static int32_t USART6_NonBlockingSend(const void *data, uint32_t num)
{
    return USART_NonBlockingSend(data, num, &usart6_NonBlockingDriverState);
}

static int32_t USART6_NonBlockingReceive(void *data, uint32_t num)
{
    return USART_NonBlockingReceive(data, num, &usart6_NonBlockingDriverState);
}

static int32_t USART6_NonBlockingTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_NonBlockingTransfer(data_out, data_in, num, &usart6_NonBlockingDriverState);
}

static uint32_t USART6_NonBlockingGetTxCount(void)
{
    return USART_NonBlockingGetTxCount(&usart6_NonBlockingDriverState);
}

static uint32_t USART6_NonBlockingGetRxCount(void)
{
    return USART_NonBlockingGetRxCount(&usart6_NonBlockingDriverState);
}

static int32_t USART6_NonBlockingControl(uint32_t control, uint32_t arg)
{
    return USART_NonBlockingControl(control, arg, &usart6_NonBlockingDriverState);
}

static ARM_USART_STATUS USART6_NonBlockingGetStatus(void)
{
    return USART_NonBlockingGetStatus(&usart6_NonBlockingDriverState);
}

/* usart6 Driver Control Block */
ARM_DRIVER_USART USART6_NonBlocking_Driver = {USARTx_GetVersion,
                                              USARTx_GetCapabilities,
                                              USART6_NonBlockingInitialize,
                                              USART6_NonBlockingUninitialize,
                                              USART6_NonBlockingPowerControl,
                                              USART6_NonBlockingSend,
                                              USART6_NonBlockingReceive,
                                              USART6_NonBlockingTransfer,
                                              USART6_NonBlockingGetTxCount,
                                              USART6_NonBlockingGetRxCount,
                                              USART6_NonBlockingControl,
                                              USART6_NonBlockingGetStatus,
                                              USARTx_SetModemControl,
                                              USARTx_GetModemStatus};

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)

cmsis_usart_dma_resource_t usart6_DmaResource = {
    RTE_USART6_DMA_TX_DMA_BASE, RTE_USART6_DMA_TX_CH, RTE_USART6_DMA_RX_DMA_BASE, RTE_USART6_DMA_RX_CH,
};

usart_dma_handle_t USART6_DmaHandle;
dma_handle_t USART6_DmaRxHandle;
dma_handle_t USART6_DmaTxHandle;

#if defined(__CC_ARM)
ARMCC_SECTION("usart6_dma_driver_state")
cmsis_usart_dma_driver_state_t usart6_DmaDriverState = {
#else
cmsis_usart_dma_driver_state_t usart6_DmaDriverState = {
#endif
    &usart6_Resource, &usart6_DmaResource, &USART6_DmaHandle, &USART6_DmaRxHandle, &USART6_DmaTxHandle,
};

static int32_t USART6_DmaInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART6_InitPins();
    return USART_DmaInitialize(cb_event, &usart6_DmaDriverState);
}

static int32_t USART6_DmaUninitialize(void)
{
    USART6_DeinitPins();
    return USART_DmaUninitialize(&usart6_DmaDriverState);
}

static int32_t USART6_DmaPowerControl(ARM_POWER_STATE state)
{
    return USART_DmaPowerControl(state, &usart6_DmaDriverState);
}

static int32_t USART6_DmaSend(const void *data, uint32_t num)
{
    return USART_DmaSend(data, num, &usart6_DmaDriverState);
}

static int32_t USART6_DmaReceive(void *data, uint32_t num)
{
    return USART_DmaReceive(data, num, &usart6_DmaDriverState);
}

static int32_t USART6_DmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_DmaTransfer(data_out, data_in, num, &usart6_DmaDriverState);
}

static uint32_t USART6_DmaGetTxCount(void)
{
    return USART_DmaGetTxCount(&usart6_DmaDriverState);
}

static uint32_t USART6_DmaGetRxCount(void)
{
    return USART_DmaGetRxCount(&usart6_DmaDriverState);
}

static int32_t USART6_DmaControl(uint32_t control, uint32_t arg)
{
    return USART_DmaControl(control, arg, &usart6_DmaDriverState);
}

static ARM_USART_STATUS USART6_DmaGetStatus(void)
{
    return USART_DmaGetStatus(&usart6_DmaDriverState);
}

/* usart6 Driver Control Block */
ARM_DRIVER_USART USART6_Dma_Driver = {
    USARTx_GetVersion, USARTx_GetCapabilities, USART6_DmaInitialize,   USART6_DmaUninitialize, USART6_DmaPowerControl,
    USART6_DmaSend,    USART6_DmaReceive,      USART6_DmaTransfer,     USART6_DmaGetTxCount,   USART6_DmaGetRxCount,
    USART6_DmaControl, USART6_DmaGetStatus,    USARTx_SetModemControl, USARTx_GetModemStatus};
#endif /* FSL_FEATURE_SOC_DMA_COUNT */

#endif /* usart6 */

#if defined(USART7)

extern uint32_t USART7_GetFreq(void);
extern void USART7_InitPins(void);
extern void USART7_DeinitPins(void);

cmsis_usart_resource_t usart7_Resource = {USART7, USART7_GetFreq};

usart_handle_t USART7_Handle;

#if defined(__CC_ARM)
ARMCC_SECTION("usart7_non_blocking_driver_state")
cmsis_usart_non_blocking_driver_state_t usart7_NonBlockingDriverState = {
#else
cmsis_usart_non_blocking_driver_state_t usart7_NonBlockingDriverState = {
#endif
    &usart7_Resource, &USART7_Handle,
};

static int32_t USART7_NonBlockingInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART7_InitPins();
    return USART_NonBlockingInitialize(cb_event, &usart7_NonBlockingDriverState);
}

static int32_t USART7_NonBlockingUninitialize(void)
{
    USART7_DeinitPins();
    return USART_NonBlockingUninitialize(&usart7_NonBlockingDriverState);
}

static int32_t USART7_NonBlockingPowerControl(ARM_POWER_STATE state)
{
    return USART_NonBlockingPowerControl(state, &usart7_NonBlockingDriverState);
}

static int32_t USART7_NonBlockingSend(const void *data, uint32_t num)
{
    return USART_NonBlockingSend(data, num, &usart7_NonBlockingDriverState);
}

static int32_t USART7_NonBlockingReceive(void *data, uint32_t num)
{
    return USART_NonBlockingReceive(data, num, &usart7_NonBlockingDriverState);
}

static int32_t USART7_NonBlockingTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_NonBlockingTransfer(data_out, data_in, num, &usart7_NonBlockingDriverState);
}

static uint32_t USART7_NonBlockingGetTxCount(void)
{
    return USART_NonBlockingGetTxCount(&usart7_NonBlockingDriverState);
}

static uint32_t USART7_NonBlockingGetRxCount(void)
{
    return USART_NonBlockingGetRxCount(&usart7_NonBlockingDriverState);
}

static int32_t USART7_NonBlockingControl(uint32_t control, uint32_t arg)
{
    return USART_NonBlockingControl(control, arg, &usart7_NonBlockingDriverState);
}

static ARM_USART_STATUS USART7_NonBlockingGetStatus(void)
{
    return USART_NonBlockingGetStatus(&usart7_NonBlockingDriverState);
}

/* usart7 Driver Control Block */
ARM_DRIVER_USART USART7_NonBlocking_Driver = {USARTx_GetVersion,
                                              USARTx_GetCapabilities,
                                              USART7_NonBlockingInitialize,
                                              USART7_NonBlockingUninitialize,
                                              USART7_NonBlockingPowerControl,
                                              USART7_NonBlockingSend,
                                              USART7_NonBlockingReceive,
                                              USART7_NonBlockingTransfer,
                                              USART7_NonBlockingGetTxCount,
                                              USART7_NonBlockingGetRxCount,
                                              USART7_NonBlockingControl,
                                              USART7_NonBlockingGetStatus,
                                              USARTx_SetModemControl,
                                              USARTx_GetModemStatus};

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)

cmsis_usart_dma_resource_t usart7_DmaResource = {
    RTE_USART7_DMA_TX_DMA_BASE, RTE_USART7_DMA_TX_CH, RTE_USART7_DMA_RX_DMA_BASE, RTE_USART7_DMA_RX_CH,
};

usart_dma_handle_t USART7_DmaHandle;
dma_handle_t USART7_DmaRxHandle;
dma_handle_t USART7_DmaTxHandle;

#if defined(__CC_ARM)
ARMCC_SECTION("usart7_dma_driver_state")
cmsis_usart_dma_driver_state_t usart7_DmaDriverState = {
#else
cmsis_usart_dma_driver_state_t usart7_DmaDriverState = {
#endif
    &usart7_Resource, &usart7_DmaResource, &USART7_DmaHandle, &USART7_DmaRxHandle, &USART7_DmaTxHandle,
};

static int32_t USART7_DmaInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART7_InitPins();
    return USART_DmaInitialize(cb_event, &usart7_DmaDriverState);
}

static int32_t USART7_DmaUninitialize(void)
{
    USART7_DeinitPins();
    return USART_DmaUninitialize(&usart7_DmaDriverState);
}

static int32_t USART7_DmaPowerControl(ARM_POWER_STATE state)
{
    return USART_DmaPowerControl(state, &usart7_DmaDriverState);
}

static int32_t USART7_DmaSend(const void *data, uint32_t num)
{
    return USART_DmaSend(data, num, &usart7_DmaDriverState);
}

static int32_t USART7_DmaReceive(void *data, uint32_t num)
{
    return USART_DmaReceive(data, num, &usart7_DmaDriverState);
}

static int32_t USART7_DmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_DmaTransfer(data_out, data_in, num, &usart7_DmaDriverState);
}

static uint32_t USART7_DmaGetTxCount(void)
{
    return USART_DmaGetTxCount(&usart7_DmaDriverState);
}

static uint32_t USART7_DmaGetRxCount(void)
{
    return USART_DmaGetRxCount(&usart7_DmaDriverState);
}

static int32_t USART7_DmaControl(uint32_t control, uint32_t arg)
{
    return USART_DmaControl(control, arg, &usart7_DmaDriverState);
}

static ARM_USART_STATUS USART7_DmaGetStatus(void)
{
    return USART_DmaGetStatus(&usart7_DmaDriverState);
}

/* usart7 Driver Control Block */
ARM_DRIVER_USART USART7_Dma_Driver = {
    USARTx_GetVersion, USARTx_GetCapabilities, USART7_DmaInitialize,   USART7_DmaUninitialize, USART7_DmaPowerControl,
    USART7_DmaSend,    USART7_DmaReceive,      USART7_DmaTransfer,     USART7_DmaGetTxCount,   USART7_DmaGetRxCount,
    USART7_DmaControl, USART7_DmaGetStatus,    USARTx_SetModemControl, USARTx_GetModemStatus};
#endif /* FSL_FEATURE_SOC_DMA_COUNT */

#endif /* usart7 */

#if defined(USART8)

extern uint32_t USART8_GetFreq(void);
extern void USART8_InitPins(void);
extern void USART8_DeinitPins(void);

cmsis_usart_resource_t usart8_Resource = {USART8, USART8_GetFreq};

usart_handle_t USART8_Handle;

#if defined(__CC_ARM)
ARMCC_SECTION("usart8_non_blocking_driver_state")
cmsis_usart_non_blocking_driver_state_t usart8_NonBlockingDriverState = {
#else
cmsis_usart_non_blocking_driver_state_t usart8_NonBlockingDriverState = {
#endif
    &usart8_Resource, &USART8_Handle,
};

static int32_t USART8_NonBlockingInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART8_InitPins();
    return USART_NonBlockingInitialize(cb_event, &usart8_NonBlockingDriverState);
}

static int32_t USART8_NonBlockingUninitialize(void)
{
    USART8_DeinitPins();
    return USART_NonBlockingUninitialize(&usart8_NonBlockingDriverState);
}

static int32_t USART8_NonBlockingPowerControl(ARM_POWER_STATE state)
{
    return USART_NonBlockingPowerControl(state, &usart8_NonBlockingDriverState);
}

static int32_t USART8_NonBlockingSend(const void *data, uint32_t num)
{
    return USART_NonBlockingSend(data, num, &usart8_NonBlockingDriverState);
}

static int32_t USART8_NonBlockingReceive(void *data, uint32_t num)
{
    return USART_NonBlockingReceive(data, num, &usart8_NonBlockingDriverState);
}

static int32_t USART8_NonBlockingTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_NonBlockingTransfer(data_out, data_in, num, &usart8_NonBlockingDriverState);
}

static uint32_t USART8_NonBlockingGetTxCount(void)
{
    return USART_NonBlockingGetTxCount(&usart8_NonBlockingDriverState);
}

static uint32_t USART8_NonBlockingGetRxCount(void)
{
    return USART_NonBlockingGetRxCount(&usart8_NonBlockingDriverState);
}

static int32_t USART8_NonBlockingControl(uint32_t control, uint32_t arg)
{
    return USART_NonBlockingControl(control, arg, &usart8_NonBlockingDriverState);
}

static ARM_USART_STATUS USART8_NonBlockingGetStatus(void)
{
    return USART_NonBlockingGetStatus(&usart8_NonBlockingDriverState);
}

/* usart8 Driver Control Block */
ARM_DRIVER_USART USART8_NonBlocking_Driver = {USARTx_GetVersion,
                                              USARTx_GetCapabilities,
                                              USART8_NonBlockingInitialize,
                                              USART8_NonBlockingUninitialize,
                                              USART8_NonBlockingPowerControl,
                                              USART8_NonBlockingSend,
                                              USART8_NonBlockingReceive,
                                              USART8_NonBlockingTransfer,
                                              USART8_NonBlockingGetTxCount,
                                              USART8_NonBlockingGetRxCount,
                                              USART8_NonBlockingControl,
                                              USART8_NonBlockingGetStatus,
                                              USARTx_SetModemControl,
                                              USARTx_GetModemStatus};

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)

cmsis_usart_dma_resource_t usart8_DmaResource = {
    RTE_USART8_DMA_TX_DMA_BASE, RTE_USART8_DMA_TX_CH, RTE_USART8_DMA_RX_DMA_BASE, RTE_USART8_DMA_RX_CH,
};

usart_dma_handle_t USART8_DmaHandle;
dma_handle_t USART8_DmaRxHandle;
dma_handle_t USART8_DmaTxHandle;

#if defined(__CC_ARM)
ARMCC_SECTION("usart8_dma_driver_state")
cmsis_usart_dma_driver_state_t usart8_DmaDriverState = {
#else
cmsis_usart_dma_driver_state_t usart8_DmaDriverState = {
#endif
    &usart8_Resource, &usart8_DmaResource, &USART8_DmaHandle, &USART8_DmaRxHandle, &USART8_DmaTxHandle,
};

static int32_t USART8_DmaInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART8_InitPins();
    return USART_DmaInitialize(cb_event, &usart8_DmaDriverState);
}

static int32_t USART8_DmaUninitialize(void)
{
    USART8_DeinitPins();
    return USART_DmaUninitialize(&usart8_DmaDriverState);
}

static int32_t USART8_DmaPowerControl(ARM_POWER_STATE state)
{
    return USART_DmaPowerControl(state, &usart8_DmaDriverState);
}

static int32_t USART8_DmaSend(const void *data, uint32_t num)
{
    return USART_DmaSend(data, num, &usart8_DmaDriverState);
}

static int32_t USART8_DmaReceive(void *data, uint32_t num)
{
    return USART_DmaReceive(data, num, &usart8_DmaDriverState);
}

static int32_t USART8_DmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_DmaTransfer(data_out, data_in, num, &usart8_DmaDriverState);
}

static uint32_t USART8_DmaGetTxCount(void)
{
    return USART_DmaGetTxCount(&usart8_DmaDriverState);
}

static uint32_t USART8_DmaGetRxCount(void)
{
    return USART_DmaGetRxCount(&usart8_DmaDriverState);
}

static int32_t USART8_DmaControl(uint32_t control, uint32_t arg)
{
    return USART_DmaControl(control, arg, &usart8_DmaDriverState);
}

static ARM_USART_STATUS USART8_DmaGetStatus(void)
{
    return USART_DmaGetStatus(&usart8_DmaDriverState);
}

/* usart8 Driver Control Block */
ARM_DRIVER_USART USART8_Dma_Driver = {
    USARTx_GetVersion, USARTx_GetCapabilities, USART8_DmaInitialize,   USART8_DmaUninitialize, USART8_DmaPowerControl,
    USART8_DmaSend,    USART8_DmaReceive,      USART8_DmaTransfer,     USART8_DmaGetTxCount,   USART8_DmaGetRxCount,
    USART8_DmaControl, USART8_DmaGetStatus,    USARTx_SetModemControl, USARTx_GetModemStatus};
#endif /* FSL_FEATURE_SOC_DMA_COUNT */

#endif /* usart8 */

#if defined(USART9)

extern uint32_t USART9_GetFreq(void);
extern void USART9_InitPins(void);
extern void USART9_DeinitPins(void);

cmsis_usart_resource_t usart9_Resource = {USART9, USART9_GetFreq};

usart_handle_t USART9_Handle;

#if defined(__CC_ARM)
ARMCC_SECTION("usart9_non_blocking_driver_state")
cmsis_usart_non_blocking_driver_state_t usart9_NonBlockingDriverState = {
#else
cmsis_usart_non_blocking_driver_state_t usart9_NonBlockingDriverState = {
#endif
    &usart9_Resource, &USART9_Handle,
};

static int32_t USART9_NonBlockingInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART9_InitPins();
    return USART_NonBlockingInitialize(cb_event, &usart9_NonBlockingDriverState);
}

static int32_t USART9_NonBlockingUninitialize(void)
{
    USART9_DeinitPins();
    return USART_NonBlockingUninitialize(&usart9_NonBlockingDriverState);
}

static int32_t USART9_NonBlockingPowerControl(ARM_POWER_STATE state)
{
    return USART_NonBlockingPowerControl(state, &usart9_NonBlockingDriverState);
}

static int32_t USART9_NonBlockingSend(const void *data, uint32_t num)
{
    return USART_NonBlockingSend(data, num, &usart9_NonBlockingDriverState);
}

static int32_t USART9_NonBlockingReceive(void *data, uint32_t num)
{
    return USART_NonBlockingReceive(data, num, &usart9_NonBlockingDriverState);
}

static int32_t USART9_NonBlockingTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_NonBlockingTransfer(data_out, data_in, num, &usart9_NonBlockingDriverState);
}

static uint32_t USART9_NonBlockingGetTxCount(void)
{
    return USART_NonBlockingGetTxCount(&usart9_NonBlockingDriverState);
}

static uint32_t USART9_NonBlockingGetRxCount(void)
{
    return USART_NonBlockingGetRxCount(&usart9_NonBlockingDriverState);
}

static int32_t USART9_NonBlockingControl(uint32_t control, uint32_t arg)
{
    return USART_NonBlockingControl(control, arg, &usart9_NonBlockingDriverState);
}

static ARM_USART_STATUS USART9_NonBlockingGetStatus(void)
{
    return USART_NonBlockingGetStatus(&usart9_NonBlockingDriverState);
}

/* usart9 Driver Control Block */
ARM_DRIVER_USART USART9_NonBlocking_Driver = {USARTx_GetVersion,
                                              USARTx_GetCapabilities,
                                              USART9_NonBlockingInitialize,
                                              USART9_NonBlockingUninitialize,
                                              USART9_NonBlockingPowerControl,
                                              USART9_NonBlockingSend,
                                              USART9_NonBlockingReceive,
                                              USART9_NonBlockingTransfer,
                                              USART9_NonBlockingGetTxCount,
                                              USART9_NonBlockingGetRxCount,
                                              USART9_NonBlockingControl,
                                              USART9_NonBlockingGetStatus,
                                              USARTx_SetModemControl,
                                              USARTx_GetModemStatus};

#if (defined(FSL_FEATURE_SOC_DMA_COUNT) && FSL_FEATURE_SOC_DMA_COUNT)

cmsis_usart_dma_resource_t usart9_DmaResource = {
    RTE_USART9_DMA_TX_DMA_BASE, RTE_USART9_DMA_TX_CH, RTE_USART9_DMA_RX_DMA_BASE, RTE_USART9_DMA_RX_CH,
};

usart_dma_handle_t USART9_DmaHandle;
dma_handle_t USART9_DmaRxHandle;
dma_handle_t USART9_DmaTxHandle;

#if defined(__CC_ARM)
ARMCC_SECTION("usart9_dma_driver_state")
cmsis_usart_dma_driver_state_t usart9_DmaDriverState = {
#else
cmsis_usart_dma_driver_state_t usart9_DmaDriverState = {
#endif
    &usart9_Resource, &usart9_DmaResource, &USART9_DmaHandle, &USART9_DmaRxHandle, &USART9_DmaTxHandle,
};

static int32_t USART9_DmaInitialize(ARM_USART_SignalEvent_t cb_event)
{
    USART9_InitPins();
    return USART_DmaInitialize(cb_event, &usart9_DmaDriverState);
}

static int32_t USART9_DmaUninitialize(void)
{
    USART9_DeinitPins();
    return USART_DmaUninitialize(&usart9_DmaDriverState);
}

static int32_t USART9_DmaPowerControl(ARM_POWER_STATE state)
{
    return USART_DmaPowerControl(state, &usart9_DmaDriverState);
}

static int32_t USART9_DmaSend(const void *data, uint32_t num)
{
    return USART_DmaSend(data, num, &usart9_DmaDriverState);
}

static int32_t USART9_DmaReceive(void *data, uint32_t num)
{
    return USART_DmaReceive(data, num, &usart9_DmaDriverState);
}

static int32_t USART9_DmaTransfer(const void *data_out, void *data_in, uint32_t num)
{
    return USART_DmaTransfer(data_out, data_in, num, &usart9_DmaDriverState);
}

static uint32_t USART9_DmaGetTxCount(void)
{
    return USART_DmaGetTxCount(&usart9_DmaDriverState);
}

static uint32_t USART9_DmaGetRxCount(void)
{
    return USART_DmaGetRxCount(&usart9_DmaDriverState);
}

static int32_t USART9_DmaControl(uint32_t control, uint32_t arg)
{
    return USART_DmaControl(control, arg, &usart9_DmaDriverState);
}

static ARM_USART_STATUS USART9_DmaGetStatus(void)
{
    return USART_DmaGetStatus(&usart9_DmaDriverState);
}

/* usart9 Driver Control Block */
ARM_DRIVER_USART USART9_Dma_Driver = {
    USARTx_GetVersion, USARTx_GetCapabilities, USART9_DmaInitialize,   USART9_DmaUninitialize, USART9_DmaPowerControl,
    USART9_DmaSend,    USART9_DmaReceive,      USART9_DmaTransfer,     USART9_DmaGetTxCount,   USART9_DmaGetRxCount,
    USART9_DmaControl, USART9_DmaGetStatus,    USARTx_SetModemControl, USARTx_GetModemStatus};
#endif /* FSL_FEATURE_SOC_DMA_COUNT */

#endif /* usart9 */
