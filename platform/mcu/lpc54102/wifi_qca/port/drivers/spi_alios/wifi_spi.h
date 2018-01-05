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

#ifndef __WIFI_SPI_H__
#define __WIFI_SPI_H__

#include <stdint.h>
#include "fsl_spi.h"
#include "fsl_dma.h"

/* can be extended in future */
typedef struct {
    struct {
        uint32_t enabled;
        DMA_Type *dma_base;
        int32_t dma_rx_chnl;
        dma_priority_t dma_rx_chnl_prio;
        int32_t dma_tx_chnl;
        dma_priority_t dma_tx_chnl_prio;
        uint32_t dma_irq_prio;
    } dma_mode;
    struct {
        uint32_t enabled;
        uint32_t spi_irq_prio;
    } irq_mode;
    struct {
        SPI_Type *base;
        uint32_t clk_hz;
        uint32_t xfer_cs;
        uint32_t baudrate;
        uint32_t irq_threshold;
        spi_master_config_t config;
    } spi;
} WIFIDRVS_SPI_config_t;

/* prototypes */
A_STATUS WIFIDRVS_SPI_Init(WIFIDRVS_SPI_config_t *config);
A_STATUS WIFIDRVS_SPI_Deinit(WIFIDRVS_SPI_config_t *config);
A_STATUS WIFIDRVS_SPI_InOutToken(uint32_t OutToken, uint8_t DataSize, uint32_t *pInToken);
A_STATUS WIFIDRVS_SPI_InOutBuffer(uint8_t *pBuffer, uint16_t length, uint8_t doRead, boolean sync);
A_STATUS WIFIDRVS_SPI_GetDefaultConfig(WIFIDRVS_SPI_config_t *config);
A_STATUS WIFIDRVS_SPI_GetSPIConfig(spi_master_config_t *user_config, uint32_t baudrate, spi_ssel_t cs);

#endif
