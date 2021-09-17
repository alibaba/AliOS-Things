/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <drv/spi.h>
#include "objects.h"
#include "PinNames.h"
#include "pinmap.h"

typedef struct csi_spi_pin {
    uint32_t spi_idx;
    PinName spi_mosi;
    PinName spi_miso;
    PinName spi_sclk;
    PinName spi_cs;
} csi_spi_pin_t;

static csi_spi_pin_t CSI_PinMap_SPI[2] = {
    { 0, PA_16, PA_17, PA_18, PA_19 },
    { 1, PB_4, PB_5, PB_6, PB_7 }
};

#define CSI_SPI_STATE_READY 0x00
#define CSI_SPI_STATE_RX_BUSY (1 << 1)
#define CSI_SPI_STATE_TX_BUSY (1 << 2)

static uint32_t ssi_interrupt(void *Adaptor)
{
    csi_spi_t *spi = (csi_spi_t *)Adaptor;
    uint32_t spi_idx = spi->dev.idx;
    SPI_TypeDef *spi_dev = SPI_DEV_TABLE[spi_idx].SPIx;
    SSI_InitTypeDef *SSI_InitStruct = (SSI_InitTypeDef *)spi->priv;

    uint32_t InterruptStatus = SSI_GetIsr(spi_dev);
    uint32_t DataFrameSize = SSI_GetDataFrameSize(spi_dev);

    SSI_SetIsrClean(spi_dev, InterruptStatus);

    if (InterruptStatus & (BIT_ISR_TXOIS | BIT_ISR_RXUIS | BIT_ISR_RXOIS | BIT_ISR_MSTIS)) {
        printf("[INT] Tx/Rx Warning %x\n", InterruptStatus);
    }

    // printf("InterruptStatus:%d\n",InterruptStatus);

    if ((InterruptStatus & BIT_ISR_RXFIS)) {
        uint32_t TransLen = 0;

        TransLen = SSI_ReceiveData(spi_dev, spi->rx_data, spi->rx_size);
        spi->rx_size -= TransLen;
        if (DataFrameSize > 8) {
            // 16~9 bits mode
            spi->rx_data = (uint8_t *)(((uint16_t *)spi->rx_data) + TransLen);
        } else {
            // 8~4 bits mode
            spi->rx_data = (uint8_t *)(((uint8_t *)spi->rx_data) + TransLen);
        }

        if (spi->rx_size == 0) {
            SSI_INTConfig(spi_dev, (BIT_IMR_RXFIM | BIT_IMR_RXOIM | BIT_IMR_RXUIM), DISABLE);
            if (spi->callback != NULL) {
                spi->callback(spi, SPI_EVENT_RECEIVE_COMPLETE, NULL);
            }
        }
    }

    if (InterruptStatus & BIT_ISR_TXEIS) {
        uint32_t TransLen = 0;
        volatile uint32_t bus_busy; /* SPIx is busy or not.*/
        uint32_t i;

        /* all data complete */
        if (spi->tx_size == 0) {
            SSI_INTConfig(spi_dev, (BIT_IMR_TXOIM | BIT_IMR_TXEIM), DISABLE);
            for (i = 0; i < 1000000; i++) {
                bus_busy = SSI_Busy(spi_dev);
                if (!bus_busy) {
                    break; // break the for loop
                }
            }

            // If it's not a dummy TX for master read SPI, then call the TX_done callback
            if (spi->tx_data != NULL) {
                if (spi->callback != NULL) {
                    spi->callback(spi, SPI_EVENT_SEND_COMPLETE, NULL);
                }
            }

            return 0;
        }

        TransLen = SSI_SendData(spi_dev, spi->tx_data, spi->tx_size, SSI_InitStruct->SPI_Role);

        spi->tx_size -= TransLen;
        if (spi->tx_data != NULL) {
            if (DataFrameSize > 8) {
                // 16~9 bits mode
                spi->tx_data = (((uint16_t *)spi->tx_data) + TransLen);
            } else {
                // 8~4 bits mode
                spi->tx_data = (void *)(((uint8_t *)spi->tx_data) + TransLen);
            }
        }
        /* all data write into fifo */
        if (spi->tx_size == 0) {
            SSI_INTConfig(spi_dev, (BIT_IMR_TXOIM), DISABLE);
            // If it's not a dummy TX for master read SPI, then call the TX_done callback
            if (spi->tx_data != NULL) {
                if (spi->callback != NULL) {
                    spi->callback(spi, SPI_EVENT_SEND_COMPLETE, NULL);
                }
            }
        }
    }

    return 0;
}

csi_error_t csi_spi_init(csi_spi_t *spi, uint32_t idx)
{
    if (!spi)
        return CSI_ERROR;

    if ((idx != 0) && (idx != 1)) {
        printf("error: spi idx should be 0 or 1\n");
        return CSI_ERROR;
    }

    spi->priv = (SSI_InitTypeDef *)malloc(sizeof(SSI_InitTypeDef));
    SSI_InitTypeDef *SSI_InitStruct = (SSI_InitTypeDef *)spi->priv;
    SPI_TypeDef *spi_dev;
    IRQn_Type IrqNum;

    spi_dev = SPI_DEV_TABLE[idx].SPIx;
    spi->dev.idx = idx;

    SSI_StructInit(SSI_InitStruct);
    PinName mosi = CSI_PinMap_SPI[idx].spi_mosi;
    PinName miso = CSI_PinMap_SPI[idx].spi_miso;
    PinName sclk = CSI_PinMap_SPI[idx].spi_sclk;
    PinName ssel = CSI_PinMap_SPI[idx].spi_cs;

    if (idx == 1) {
        RCC_PeriphClockCmd(APBPeriph_SPI1, APBPeriph_SPI1_CLOCK, ENABLE);
        Pinmux_Config(mosi, PINMUX_FUNCTION_SPIM);
        Pinmux_Config(miso, PINMUX_FUNCTION_SPIM);
        Pinmux_Config(sclk, PINMUX_FUNCTION_SPIM);
        Pinmux_Config(ssel, PINMUX_FUNCTION_SPIM);
    } else {
        RCC_PeriphClockCmd(APBPeriph_SPI0, APBPeriph_SPI0_CLOCK, ENABLE);
        Pinmux_Config(mosi, PINMUX_FUNCTION_SPIS);
        Pinmux_Config(miso, PINMUX_FUNCTION_SPIS);
        Pinmux_Config(sclk, PINMUX_FUNCTION_SPIS);
        Pinmux_Config(ssel, PINMUX_FUNCTION_SPIS);
    }

    SSI_Init(spi_dev, SSI_InitStruct);

    IrqNum = SPI_DEV_TABLE[idx].IrqNum;
    InterruptRegister((IRQ_FUN)ssi_interrupt, IrqNum, (uint32_t)spi, 5);
    InterruptEn(IrqNum, 5);

    return CSI_OK;
}

void csi_spi_uninit(csi_spi_t *spi)
{
    uint32_t spi_idx = spi->dev.idx;
    SPI_TypeDef *spi_dev = SPI_DEV_TABLE[spi_idx].SPIx;
    IRQn_Type IrqNum = SPI_DEV_TABLE[spi_idx].IrqNum;

    InterruptDis(IrqNum);
    InterruptUnRegister(IrqNum);

    SSI_INTConfig(spi_dev, (BIT_IMR_RXFIM | BIT_IMR_RXOIM | BIT_IMR_RXUIM), DISABLE);
    SSI_Cmd(spi_dev, DISABLE);

    if (spi && spi->priv) {
        free(spi->priv);
        spi->priv = NULL;
    }
}

csi_error_t csi_spi_attach_callback(csi_spi_t *spi, void *callback, void *arg)
{
    spi->callback = callback;
    spi->arg = arg;
    return CSI_OK;
}

void csi_spi_detach_callback(csi_spi_t *spi)
{
    spi->callback = NULL;
}

csi_error_t csi_spi_mode(csi_spi_t *spi, csi_spi_mode_t mode)
{
    uint32_t spi_idx = spi->dev.idx;
    SPI_TypeDef *spi_dev = SPI_DEV_TABLE[spi_idx].SPIx;
    SSI_InitTypeDef *SSI_InitStruct = (SSI_InitTypeDef *)spi->priv;

    switch (mode) {
    case SPI_MASTER:
        SSI_InitStruct->SPI_Role = SSI_MASTER;
        break;
    case SPI_SLAVE:
        SSI_InitStruct->SPI_Role = SSI_SLAVE;
        break;
    }

    if (spi_idx == 0)
        SSI_SetRole(spi_dev, SSI_InitStruct->SPI_Role);

    // Re-init after setting role
    SSI_StructInit(SSI_InitStruct);
    SSI_Init(spi_dev, SSI_InitStruct);
    return CSI_OK;
}

csi_error_t csi_spi_cp_format(csi_spi_t *spi, csi_spi_cp_format_t format)
{
    uint32_t spi_idx = spi->dev.idx;
    SPI_TypeDef *spi_dev = SPI_DEV_TABLE[spi_idx].SPIx;
    SSI_InitTypeDef *SSI_InitStruct = (SSI_InitTypeDef *)spi->priv;

    uint32_t SclkPhase;
    uint32_t SclkPolarity;

    switch (format) {
    case SPI_FORMAT_CPOL0_CPHA0:
        SclkPolarity = SCPOL_INACTIVE_IS_LOW;
        SclkPhase = SCPH_TOGGLES_IN_MIDDLE;
        break;
    case SPI_FORMAT_CPOL0_CPHA1:
        SclkPolarity = SCPOL_INACTIVE_IS_LOW;
        SclkPhase = SCPH_TOGGLES_AT_START;
        break;
    case SPI_FORMAT_CPOL1_CPHA0:
        SclkPolarity = SCPOL_INACTIVE_IS_HIGH;
        SclkPhase = SCPH_TOGGLES_IN_MIDDLE;
        break;
    case SPI_FORMAT_CPOL1_CPHA1:
        SclkPolarity = SCPOL_INACTIVE_IS_HIGH;
        SclkPhase = SCPH_TOGGLES_AT_START;
        break;
    default:
        // same as 3
        SclkPolarity = SCPOL_INACTIVE_IS_HIGH;
        SclkPhase = SCPH_TOGGLES_AT_START;
        break;
    }

    SSI_SetSclkPhase(spi_dev, SclkPhase);
    SSI_SetSclkPolarity(spi_dev, SclkPolarity);

    if (SSI_InitStruct->SPI_Role == SSI_SLAVE) {
        PinName sclk = CSI_PinMap_SPI[spi_idx].spi_sclk;
        if (SclkPolarity == SCPOL_INACTIVE_IS_LOW) {
            PAD_PullCtrl((uint32_t)sclk, GPIO_PuPd_DOWN);
        } else {
            PAD_PullCtrl((uint32_t)sclk, GPIO_PuPd_UP);
        }
    }
    return CSI_OK;
}

csi_error_t csi_spi_frame_len(csi_spi_t *spi, csi_spi_frame_len_t length)
{
    uint32_t spi_idx = spi->dev.idx;
    SPI_TypeDef *spi_dev = SPI_DEV_TABLE[spi_idx].SPIx;

    uint32_t DataFrameSize = (length - 1);
    SSI_SetDataFrameSize(spi_dev, DataFrameSize);
    return CSI_OK;
}

uint32_t csi_spi_baud(csi_spi_t *spi, uint32_t baud)
{
    uint32_t spi_idx = spi->dev.idx;
    SPI_TypeDef *spi_dev = SPI_DEV_TABLE[spi_idx].SPIx;
    uint32_t IpClk;
    uint32_t ClockDivider;

    if (spi_idx == 0)
        IpClk = 100000000;
    else
        IpClk = 50000000;

    /*Adjust SCKDV-Parameter to an even number */
    ClockDivider = IpClk / baud + 1;
    if ((IpClk % baud) > (uint32_t)(baud / 2)) {
        ClockDivider++;
    }
    if (ClockDivider >= 0xFFFF) {
        /*  devider is 16 bits */
        ClockDivider = 0xFFFE;
    }
    ClockDivider &= 0xFFFE; // bit 0 always is 0

    SSI_SetBaudDiv(spi_dev, ClockDivider);
    return ClockDivider;
}

int32_t csi_spi_send(csi_spi_t *spi, const void *data, uint32_t size, uint32_t timeout)
{
    uint8_t spi_idx = spi->dev.idx;
    SPI_TypeDef *spi_dev = SPI_DEV_TABLE[spi_idx].SPIx;
    SSI_InitTypeDef *SSI_InitStruct = (SSI_InitTypeDef *)spi->priv;
    uint32_t DataFrameSize = SSI_GetDataFrameSize(spi_dev);
    int32_t cnt = 0;
    uint32_t StartCount = 0;

    StartCount = SYSTIMER_TickGet();
    while (1) {
        if (SSI_Writeable(spi_dev)) {
            if (DataFrameSize > 8) {
                // 16~9 bits mode
                if (data != NULL) {
                    SSI_WriteData(spi_dev, *((uint16_t *)(data + cnt)));
                    cnt += 2;

                    // For master mode: Push a dummy to TX FIFO for Read
                    if (SSI_InitStruct->SPI_Role == SSI_MASTER) {
                        if (SSI_Readable(spi_dev))
                            SSI_ReadData(spi_dev);
                    }
                }

            } else {
                if (data != NULL) {
                    SSI_WriteData(spi_dev, *((uint8_t *)(data + cnt)));
                    printf("data:%c\n", *((uint8_t *)(data + cnt)));
                    cnt++;

                    // For master mode: Push a dummy to TX FIFO for Read
                    if (SSI_InitStruct->SPI_Role == SSI_MASTER) {
                        if (SSI_Readable(spi_dev))
                            SSI_ReadData(spi_dev);
                    }
                }
            }
        } else {
            aos_msleep(1);
        }

        if (cnt == size)
            break;

        if (SYSTIMER_GetPassTime(StartCount) > timeout) {
            break;
        }
    }
    return cnt;
}

csi_error_t csi_spi_send_async(csi_spi_t *spi, const void *data, uint32_t size)
{
    uint8_t spi_idx = spi->dev.idx;
    SPI_TypeDef *spi_dev = SPI_DEV_TABLE[spi_idx].SPIx;
    uint32_t DataFrameSize = SSI_GetDataFrameSize(spi_dev);

    if (size == 0)
        return CSI_ERROR;

    if (DataFrameSize > 8) {
        /*  16~9 bits mode */
        spi->tx_size = size >> 1; // 2 bytes(16 bit) every transfer
    } else {
        /*  8~4 bits mode */
        spi->tx_size = size; // 1 byte(8 bit) every transfer
    }

    spi->tx_data = (uint8_t *)data;

    SSI_INTConfig(spi_dev, (BIT_IMR_TXOIM | BIT_IMR_TXEIM), ENABLE);
    return CSI_OK;
}

int32_t csi_spi_receive(csi_spi_t *spi, void *data, uint32_t size, uint32_t timeout)
{
    uint8_t spi_idx = spi->dev.idx;
    SPI_TypeDef *spi_dev = SPI_DEV_TABLE[spi_idx].SPIx;
    uint32_t DataFrameSize = SSI_GetDataFrameSize(spi_dev);
    int32_t cnt = 0;
    uint32_t StartCount = 0;

    StartCount = SYSTIMER_TickGet();
    while (1) {
        if (SSI_Readable(spi_dev)) {
            if (data != NULL) {
                if (DataFrameSize > 8) {
                    /*  16~9 bits mode */
                    *((uint16_t *)(data)) = (uint16_t)SSI_ReadData(spi_dev);
                    data = (void *)(((uint16_t *)data) + 1);
                    cnt += 2;
                } else {
                    /*  8~4 bits mode */
                    *((uint8_t *)(data)) = (uint8_t)SSI_ReadData(spi_dev);
                    data = (void *)(((uint8_t *)data) + 1);
                    cnt++;
                }
            } else {
                /*  for Master mode, doing TX also will got RX data, so drop the dummy data */
                SSI_ReadData(spi_dev);
            }
        } else {
            aos_msleep(1);
        }

        if (cnt == size)
            break;

        if (SYSTIMER_GetPassTime(StartCount) > timeout) {
            break;
        }
    }
    return cnt;
}

csi_error_t csi_spi_receive_async(csi_spi_t *spi, void *data, uint32_t size)
{
    uint8_t spi_idx = spi->dev.idx;
    SPI_TypeDef *spi_dev = SPI_DEV_TABLE[spi_idx].SPIx;
    SSI_InitTypeDef *SSI_InitStruct = (SSI_InitTypeDef *)spi->priv;
    uint32_t DataFrameSize = SSI_GetDataFrameSize(spi_dev);

    if (size == 0)
        return CSI_ERROR;

    /*  As a Slave mode, if the peer(Master) side is power off, the BUSY flag is always on */
    if (SSI_Busy(spi_dev)) {
        printf("%s:SSI is busy\n", __func__);
        return CSI_BUSY;
    }

    if (DataFrameSize > 8) {
        /*  16~9 bits mode */
        spi->rx_size = size >> 1; // 2 bytes(16 bit) every transfer
    } else {
        /*  8~4 bits mode */
        spi->rx_size = size; // 1 byte(8 bit) every transfer
    }

    spi->rx_data = (uint8_t *)data;

    SSI_INTConfig(spi_dev, (BIT_IMR_RXFIM | BIT_IMR_RXOIM | BIT_IMR_RXUIM), ENABLE);

    if (SSI_InitStruct->SPI_Role == SSI_MASTER) {
        /* as Master mode, it need to push data to TX FIFO to generate clock out
        then the slave can transmit data out */
        // send some dummy data out
        if (DataFrameSize > 8) {
            /*  16~9 bits mode */
            spi->tx_size = size >> 1; // 2 bytes(16 bit) every transfer
        } else {
            /*  8~4 bits mode */
            spi->tx_size = size; // 1 byte(8 bit) every transfer
        }
        spi->tx_data = (void *)NULL;
        SSI_INTConfig(spi_dev, (BIT_IMR_TXOIM | BIT_IMR_TXEIM), ENABLE);
    }
    return CSI_OK;
}

int32_t csi_spi_send_receive(csi_spi_t *spi, const void *data_out, void *data_in,
                             uint32_t size, uint32_t timeout)
{
    uint8_t spi_idx = spi->dev.idx;
    SPI_TypeDef *spi_dev = SPI_DEV_TABLE[spi_idx].SPIx;
    uint32_t DataFrameSize = SSI_GetDataFrameSize(spi_dev);
    int32_t cnt = 0;
    uint32_t StartCount = 0;

    StartCount = SYSTIMER_TickGet();
    while (1) {
        if (SSI_Writeable(spi_dev)) {
            if (DataFrameSize > 8) {
                /*  16~9 bits mode */
                SSI_WriteData(spi_dev, *((uint16_t *)(data_out)));
                data_out = (void *)(((uint16_t *)data_out) + 1);
                cnt += 2;
                if (SSI_Readable(spi_dev)) {
                    *((uint16_t *)(data_in)) = (uint16_t)SSI_ReadData(spi_dev);
                    data_in = (void *)(((uint16_t *)data_in) + 1);
                }
            } else {
                /*  8~4 bits mode */
                SSI_WriteData(spi_dev, *((uint8_t *)(data_out)));
                data_out = (void *)(((uint8_t *)data_out) + 1);
                cnt++;
                if (SSI_Readable(spi_dev)) {
                    *((uint8_t *)(data_in)) = (uint8_t)SSI_ReadData(spi_dev);
                    data_in = (void *)(((uint8_t *)data_in) + 1);
                }
            }
        } else {
            aos_msleep(1);
        }

        if (cnt == size)
            break;

        if (SYSTIMER_GetPassTime(StartCount) > timeout) {
            break;
        }
    }
    return cnt;
}

void csi_spi_select_slave(csi_spi_t *spi, uint32_t slave_num)
{
    uint8_t spi_idx = spi->dev.idx;
    SPI_TypeDef *spi_dev = SPI_DEV_TABLE[spi_idx].SPIx;
    SSI_InitTypeDef *SSI_InitStruct = (SSI_InitTypeDef *)spi->priv;

    if (SSI_InitStruct->SPI_Role == SSI_MASTER) {
        SSI_SetSlaveEnable(spi_dev, slave_num);
    } else {
        assert_param(0);
    }
}

csi_error_t csi_spi_get_state(csi_spi_t *spi, csi_state_t *state)
{
    uint8_t spi_idx = spi->dev.idx;
    SPI_TypeDef *spi_dev = SPI_DEV_TABLE[spi_idx].SPIx;

    state->readable = SSI_Readable(spi_dev);
    state->writeable = SSI_Writeable(spi_dev);
    state->error = SSI_Busy(spi_dev);

    return CSI_OK;
}

csi_error_t csi_spi_link_dma(csi_spi_t *spi, csi_dma_ch_t *tx_dma, csi_dma_ch_t *rx_dma)
{
    spi->tx_dma = tx_dma;
    spi->rx_dma = rx_dma;
    return CSI_OK;
}
