/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include "aos/hal/spi.h"
#include "hal_iomux.h"
#include "hal_spi.h"
#include "hal_trace.h"
#include "hal_gpio.h"
#include "hal_iomux_best2001.h"
#include "hal_cache.h"
#include "cmsis_os.h"
#include "cmsis.h"

osSemaphoreDef(spi_dma_semaphore);
static osSemaphoreId g_spi_dma_semaphore;

osMutexDef(spi_mutex);
static osMutexId g_spi_mutex_id;

/**
 * Initialises the SPI interface for a given SPI device
 *
 * @param[in]  spi  the spi device
 *
 * @return  0 : on success, EIO : if the SPI device could not be initialised
 */
int32_t hal_spi_init(spi_dev_t *spi)
{
	int ret;

	static struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spilcd[] = {
		{HAL_IOMUX_PIN_P0_0, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
		{HAL_IOMUX_PIN_P0_1, HAL_IOMUX_FUNC_SPILCD_DIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
		{HAL_IOMUX_PIN_P1_0, HAL_IOMUX_FUNC_SPILCD_CLK, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
		{HAL_IOMUX_PIN_P1_1, HAL_IOMUX_FUNC_SPILCD_CS0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
		{HAL_IOMUX_PIN_P2_2, HAL_IOMUX_FUNC_GPIO, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
	};
	hal_iomux_init(pinmux_spilcd, ARRAY_SIZE(pinmux_spilcd));

	struct HAL_SPI_CFG_T spi_cfg;
	switch(spi->config.mode) {
	case 0:
		spi_cfg.clk_delay_half = false;
		spi_cfg.clk_polarity = false;
		break;
	case 1:
		spi_cfg.clk_delay_half = true;
		spi_cfg.clk_polarity = false;
		break;
	case 2:
		spi_cfg.clk_delay_half = false;
		spi_cfg.clk_polarity = true;
		break;
	case 3:
		spi_cfg.clk_delay_half = true;
		spi_cfg.clk_polarity = true;
		break;
	default:
		spi_cfg.clk_delay_half = true;
		spi_cfg.clk_polarity = true;
	}

	spi_cfg.slave = false;
	spi_cfg.dma_rx = true,
	spi_cfg.dma_tx = true,
	spi_cfg.rate = spi->config.freq;
	if (spi_cfg.rate > 26000000)
		spi_cfg.rate = 26000000;
	spi_cfg.cs = 0;
	spi_cfg.rx_bits = 8;
	spi_cfg.tx_bits = 8;
	spi_cfg.rx_frame_bits = 0;
	ret = hal_spilcd_open(&spi_cfg);
	if (ret != 0) {
		TRACE("spi lcd open error");
		return ret;
	} else {
		TRACE("spi lcd open succ");
	}
	if (!g_spi_dma_semaphore) {
		g_spi_dma_semaphore = osSemaphoreCreate(osSemaphore(spi_dma_semaphore), 0);
	}
	if (!g_spi_dma_semaphore) {
		TRACE("spi_dma_semaphore create failed!");
		return -1;
	}
	if (!g_spi_mutex_id) {
		g_spi_mutex_id = osMutexCreate((osMutex(spi_mutex)));
	}
	if (!g_spi_mutex_id) {
		TRACE("spi_mutex create failed!");
		return -1;
	}

	return ret;
}

static void spi1_dma_irq(uint32_t error)
{
	if (osOK != osSemaphoreRelease(g_spi_dma_semaphore)) {
		TRACE("spi1dmairq osSemaphoreRelease failed!");
	}
}

/**
 * Spi send
 *
 * @param[in]  spi      the spi device
 * @param[in]  data     spi send data
 * @param[in]  size     spi send data size
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0 : on success, EIO : if the SPI device could not be initialised
 */

#ifdef hal_spi_send
#undef hal_spi_send
#endif

#define LIMIT_LEN 2048
#define TIME_OUT 1000
int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
	int ret = 0;
	uint32_t len = size;
	uint32_t i = 0;
	uint8_t *buf = data;

	osStatus status = osMutexWait(g_spi_mutex_id, osWaitForever);
	if (osOK != status) {
		TRACE("%s spi_mutex wait error = 0x%X!", __func__, status);
		return -2;
	}
	if (NULL == data || 0 == size) {
		TRACE("spi input para err");
		return -3;
	}

	while (len > LIMIT_LEN) {
		hal_cache_sync(HAL_CACHE_ID_D_CACHE);
		ret = hal_spilcd_dma_send(buf, LIMIT_LEN, spi1_dma_irq);
		if (ret) {
			TRACE("spi dma send fail %d, size %d", ret, LIMIT_LEN);
			goto OUT;
		}
		len -= LIMIT_LEN;
		buf += LIMIT_LEN;

		if (osSemaphoreWait(g_spi_dma_semaphore, TIME_OUT) <= 0 ) {
			TRACE("spi dma send timeout");
			ret = -1;
			goto OUT;
		}
	}
	hal_cache_sync(HAL_CACHE_ID_D_CACHE);
	ret = hal_spilcd_dma_send(buf, len, spi1_dma_irq);
	if (ret) {
		TRACE("spi dma tail send fail %d, size %d", ret, len);
		goto OUT;
	}
	if (osSemaphoreWait(g_spi_dma_semaphore, TIME_OUT) <= 0) {
		TRACE("spi dma tail send timeout");
		ret = -1;
	}
OUT:
	osMutexRelease(g_spi_mutex_id);
	return ret;
}

#ifdef hal_spi_recv
#undef hal_spi_recv
#endif

/**
 * spi_recv
 *
 * @param[in]   spi      the spi device
 * @param[out]  data     spi recv data
 * @param[in]   size     spi recv data size
 * @param[in]   timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0 : on success, EIO : if the SPI device could not be initialised
 */
int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
{
	//TBD
	//int hal_spi_dma_recv(const void *cmd, void *data, uint32_t len, HAL_SPI_DMA_HANDLER_T handler);
	return 0;
}

/**
 * spi send data and recv
 *
 * @param[in]  spi      the spi device
 * @param[in]  tx_data  spi send data
 * @param[in]  rx_data  spi recv data
 * @param[in]  size     spi data to be sent and recived
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0, on success;  EIO : if the SPI device could not be initialised
 */
int32_t hal_spi_send_recv(spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data,
                          uint16_t size, uint32_t timeout)
{
	return 0;
}

/**
 * De-initialises a SPI interface
 *
 *
 * @param[in]  spi  the SPI device to be de-initialised
 *
 * @return  0 : on success, EIO : if an error occurred
 */
int32_t hal_spi_finalize(spi_dev_t *spi)
{
	return 0;
}

