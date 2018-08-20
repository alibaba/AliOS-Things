/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <k_api.h>
#include <aos/aos.h>
#include "hal/hal.h"
#include "hal/soc/soc.h"

#include "fsl_device_registers.h"
#include "fsl_common.h"
#include "fsl_clock.h"
#include "fsl_spi.h"
#include "fsl_debug_console.h"

#define LOG_TAG		"spi"

static const uint32_t spi_base[FSL_FEATURE_SOC_SPI_COUNT] = SPI_BASE_ADDRS;

int32_t hal_spi_init(spi_dev_t *spi)
{
	spi_master_config_t config;

    if (!spi)
        return -ENODEV;

	memset(&config, 0, sizeof(spi_master_config_t));

	SPI_MasterGetDefaultConfig(&config);
	config.phase = kSPI_ClockPhaseSecondEdge;
	config.polarity = kSPI_ClockPolarityActiveLow;

    if (SPI_MasterInit((SPI_Type *)spi_base[spi->port], &config, CLOCK_GetFreq(kCLOCK_Spi)) != kStatus_Success) {
		LOGE(LOG_TAG, "%s: init spi%u failed !", __func__, spi->port);
		return -EACCES;
    }

	LOG("%s: init spi%u success", __func__, spi->port);
    return 0;
}

int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
{
	return 0;
}

int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
	spi_transfer_t xfer;
	uint8_t send_data[size];

    if (!spi)
		return -EINVAL;

	memset(&xfer, 0, sizeof(spi_transfer_t));
	memcpy(send_data, data, size);

	xfer.txData = send_data;
	xfer.dataSize = size;
	xfer.rxData = NULL;
	xfer.configFlags = kSPI_FrameAssert;

	if (SPI_MasterTransferBlocking((SPI_Type *)spi_base[spi->port], &xfer) != kStatus_Success) {
		LOGE(LOG_TAG, "%s: send %u bytes failed !", __func__, size);
		return -EIO;
	}
    return 0;
}

int32_t hal_spi_send_recv(spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data,
                          uint16_t size, uint32_t timeout)
{
	spi_half_duplex_transfer_t xfer;
	uint16_t rem_size = size;

    if (!spi)
		return -EINVAL;

	memset(&xfer, 0, sizeof(spi_half_duplex_transfer_t));

	xfer.rxDataSize = 1;
	xfer.txDataSize = 1;
	xfer.isTransmitFirst = true;
	xfer.isPcsAssertInTransfer = true;
	xfer.configFlags = kSPI_FrameAssert;

	while (rem_size-- > 0) {
		xfer.txData = tx_data++;
		xfer.rxData = rx_data++;
		if (SPI_MasterHalfDuplexTransferBlocking((SPI_Type *)spi_base[spi->port], &xfer) != kStatus_Success) {
			LOGE(LOG_TAG, "%s: recv %u bytes failed !", __func__, size);
			return -EIO;
		}
	}

	return 0;
}

int32_t hal_spi_finalize(spi_dev_t *spi)
{
	if (!spi)
		return -EINVAL;

    SPI_Deinit((SPI_Type *)spi_base[spi->port]);

    return 0;
}
