/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/hal/spi.h"
#include "hal_iomux.h"
#include "hal_spi.h"
#include "hal_trace.h"
#include "hal_gpio.h"
#include "hal_iomux_haas1000.h"
#include "hal_cache.h"
#include "cmsis_os.h"
#include "cmsis.h"

#define SPI_DMA_MAX 4095

osSemaphoreDef(spi0_dma_semaphore);
osSemaphoreDef(spi1_dma_semaphore);

osMutexDef(spi0_mutex);
osMutexDef(spi1_mutex);

static void spi0_dma_irq(uint32_t error);
static void spi1_dma_irq(uint32_t error);

typedef struct
{
	enum HAL_IOMUX_PIN_T spi_pin_DI0;
	enum HAL_IOMUX_PIN_T spi_pin_CLK;
	enum HAL_IOMUX_PIN_T spi_pin_CS0;
	enum HAL_IOMUX_PIN_T spi_pin_DIO;
	enum HAL_IOMUX_FUNCTION_T spi_fun_DI0;
	enum HAL_IOMUX_FUNCTION_T spi_fun_CLK;
	enum HAL_IOMUX_FUNCTION_T spi_fun_CS0;
	enum HAL_IOMUX_FUNCTION_T spi_fun_DIO;
	osSemaphoreId spi_dma_semaphore;
	osMutexId spi_mutex_id;
	int (*spi_open)(const struct HAL_SPI_CFG_T *cfg);
	int (*spi_dma_send)(const void *data, uint32_t len, HAL_SPI_DMA_HANDLER_T handler);
	int (*spi_dma_recv)(const void *cmd, void *data, uint32_t len, HAL_SPI_DMA_HANDLER_T handler);
	int (*spi_send)(const void *data, uint32_t len);
	int (*spi_recv)(const void *cmd, void *data, uint32_t len);
	void (*spi_dma_irq)(uint32_t error);
	int (*spi_close)(uint32_t cs);
} spi_ctx_obj_t;

static spi_ctx_obj_t spi_ctx[2] =
{
	{
		.spi_pin_DI0 = HAL_IOMUX_PIN_P0_4,
		.spi_pin_CLK = HAL_IOMUX_PIN_P0_5,
		.spi_pin_CS0 = HAL_IOMUX_PIN_P0_6,
		.spi_pin_DIO = HAL_IOMUX_PIN_P0_7,
		.spi_fun_DI0 = HAL_IOMUX_FUNC_SPI_DI0,
		.spi_fun_CLK = HAL_IOMUX_FUNC_SPI_CLK,
		.spi_fun_CS0 = HAL_IOMUX_FUNC_SPI_CS0,
		.spi_fun_DIO = HAL_IOMUX_FUNC_SPI_DIO,
		.spi_dma_semaphore = NULL,
		.spi_mutex_id = 0,
		.spi_open = hal_spi_open,
		.spi_dma_send = hal_spi_dma_send,
		.spi_dma_recv = hal_spi_dma_recv,
		.spi_send = hal_spi_no_dma_send,
		.spi_recv = hal_spi_no_dma_recv,
		.spi_dma_irq = spi0_dma_irq,
		.spi_close = hal_spi_close
	},
	{
		.spi_pin_DI0 = HAL_IOMUX_PIN_P3_4,
		.spi_pin_CLK = HAL_IOMUX_PIN_P3_7,
		.spi_pin_CS0 = HAL_IOMUX_PIN_P3_6,
		.spi_pin_DIO = HAL_IOMUX_PIN_P3_5,
		.spi_fun_DI0 = HAL_IOMUX_FUNC_SPILCD_DI0,
		.spi_fun_CLK = HAL_IOMUX_FUNC_SPILCD_CLK,
		.spi_fun_CS0 = HAL_IOMUX_FUNC_SPILCD_CS0,
		.spi_fun_DIO = HAL_IOMUX_FUNC_SPILCD_DIO,
		.spi_dma_semaphore = NULL,
		.spi_mutex_id = 0,
		.spi_open = hal_spilcd_open,
		.spi_dma_send = hal_spilcd_dma_send,
		.spi_dma_recv = hal_spilcd_dma_recv,
		.spi_send = hal_spilcd_send,
		.spi_recv = hal_spilcd_recv,
		.spi_dma_irq = spi1_dma_irq,
		.spi_close = hal_spilcd_close
	}
};

/**
 * Initialises the SPI interface for a given SPI device
 *
 * @param[in]  spi  the spi device
 *
 * @return  0 : on success, EIO : if the SPI device could not be initialised
 */
int32_t hal_spi_init(spi_dev_t *spi)
{
	int ret = -1;
	spi_config_t cfg_spi = spi->config;
	static struct HAL_IOMUX_PIN_FUNCTION_MAP pinmux_spi[] = {
		{0, 0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
		{0, 0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
		{0, 0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
		{0, 0, HAL_IOMUX_PIN_VOLTAGE_VIO, HAL_IOMUX_PIN_NOPULL},
	};
	pinmux_spi[0].pin = spi_ctx[spi->port].spi_pin_DI0;
	pinmux_spi[1].pin = spi_ctx[spi->port].spi_pin_CLK;
	pinmux_spi[2].pin = spi_ctx[spi->port].spi_pin_CS0;
	pinmux_spi[3].pin = spi_ctx[spi->port].spi_pin_DIO;

	pinmux_spi[0].function = spi_ctx[spi->port].spi_fun_DI0;
	pinmux_spi[1].function = spi_ctx[spi->port].spi_fun_CLK;
	pinmux_spi[2].function = spi_ctx[spi->port].spi_fun_CS0;
	pinmux_spi[3].function = spi_ctx[spi->port].spi_fun_DIO;

	if (cfg_spi.freq > 26000000){
		hal_iomux_set_io_driver(pinmux_spi[1].pin,3);
		hal_iomux_set_io_driver(pinmux_spi[3].pin,3);
	}
	hal_iomux_init(pinmux_spi, ARRAY_SIZE(pinmux_spi));
	struct HAL_SPI_CFG_T spi_cfg;
	switch (cfg_spi.mode)
	{
	case SPI_WORK_MODE_0:
		spi_cfg.clk_delay_half = false;
		spi_cfg.clk_polarity = false;
		break;
	case SPI_WORK_MODE_1:
		spi_cfg.clk_delay_half = true;
		spi_cfg.clk_polarity = false;
		break;
	case SPI_WORK_MODE_2:
		spi_cfg.clk_delay_half = false;
		spi_cfg.clk_polarity = true;
		break;
	case SPI_WORK_MODE_3:
		spi_cfg.clk_delay_half = true;
		spi_cfg.clk_polarity = true;
		break;
	default:
		spi_cfg.clk_delay_half = true;
		spi_cfg.clk_polarity = true;
	}
	spi_cfg.slave = 0;
	if (cfg_spi.t_mode == SPI_TRANSFER_DMA)
	{
		spi_cfg.dma_rx = true;
		spi_cfg.dma_tx = true;
	}
	else
	{
		spi_cfg.dma_rx = false;
		spi_cfg.dma_tx = false;
	}
	spi_cfg.rate = cfg_spi.freq;
	//if (spi_cfg.rate > 26000000)
		//spi_cfg.rate = 26000000;
	spi_cfg.cs = 0;
	spi_cfg.rx_bits = cfg_spi.data_size;
	spi_cfg.tx_bits = cfg_spi.data_size;
	spi_cfg.rx_frame_bits = 0;
	ret = spi_ctx[spi->port].spi_open(&spi_cfg);
	if (ret != 0)
	{
		TRACE("spi %d open error", spi->port);
		return ret;
	}
	else
	{
		/*if cs use as gpio ,pull up at first*/
		if (spi_ctx[spi->port].spi_fun_CS0 == HAL_IOMUX_FUNC_AS_GPIO) {
			hal_gpio_pin_set_dir(spi_ctx[spi->port].spi_pin_CS0, HAL_GPIO_DIR_OUT, 1);
		}
		TRACE("spi %d open succ", spi->port);
	}
	if (!spi_ctx[spi->port].spi_dma_semaphore)
	{
		if (spi->port == 0)
		{
			spi_ctx[spi->port].spi_dma_semaphore = osSemaphoreCreate(osSemaphore(spi0_dma_semaphore), 0);
		}
		else
		{
			spi_ctx[spi->port].spi_dma_semaphore = osSemaphoreCreate(osSemaphore(spi1_dma_semaphore), 0);
		}
	}
	if (!spi_ctx[spi->port].spi_dma_semaphore)
	{
		TRACE("spi0_dma_semaphore create failed!");
		return -1;
	}
	if (!spi_ctx[spi->port].spi_mutex_id)
	{
		if (spi->port == 0)
		{
			spi_ctx[spi->port].spi_mutex_id = osMutexCreate((osMutex(spi0_mutex)));
		}
		else
		{
			spi_ctx[spi->port].spi_mutex_id = osMutexCreate((osMutex(spi1_mutex)));
		}
	}
	if (!spi_ctx[spi->port].spi_mutex_id)
	{
		TRACE("spi0_mutex create failed!");
		return -1;
	}
	return ret;
}

void spi0_dma_irq(uint32_t error)
{
	if (osOK != osSemaphoreRelease(spi_ctx[0].spi_dma_semaphore))
	{
		TRACE("spi1dmairq osSemaphoreRelease failed!");
	}
}

void spi1_dma_irq(uint32_t error)
{
	if (osOK != osSemaphoreRelease(spi_ctx[1].spi_dma_semaphore))
	{
		TRACE("spi0dmairq osSemaphoreRelease failed!");
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
int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
	int32_t ret = 0;
	uint32_t len = size;
	uint32_t i = 0;
	uint8_t *buf = data;
	osStatus status = osErrorOS;

	if (NULL == spi || NULL == data || 0 == size)
	{
		TRACE("spi input para err");
		return -3;
	}

	status = osMutexWait(spi_ctx[spi->port].spi_mutex_id, osWaitForever);
	if (osOK != status)
	{
		TRACE("%s spi_mutex wait error = 0x%X!", __func__, status);
		return -2;
	}

	hal_cache_sync(HAL_CACHE_ID_D_CACHE);
	/*if cs use as gpio, pull down cs at first*/
	if (spi_ctx[spi->port].spi_fun_CS0 == HAL_IOMUX_FUNC_AS_GPIO)
	{
		hal_gpio_pin_set_dir(spi_ctx[spi->port].spi_pin_CS0, HAL_GPIO_DIR_OUT, 0);
	}

	if (spi->config.t_mode == SPI_TRANSFER_DMA)
	{
		ret = spi_ctx[spi->port].spi_dma_send(buf, len, spi_ctx[spi->port].spi_dma_irq);
		if (osSemaphoreWait(spi_ctx[spi->port].spi_dma_semaphore, timeout) <= 0)
		{
			TRACE("spi dma tail send timeout");
			goto OUT;
		}
	}
	else
	{
		ret = spi_ctx[spi->port].spi_send(buf, len);
	}

	if (ret)
	{
		TRACE("spi dma tail send fail %d, size %d", ret, len);
		goto OUT;
	}
OUT:
	/*if cs use as gpio, pull pull up cs at the end*/
	if (spi_ctx[spi->port].spi_fun_CS0 == HAL_IOMUX_FUNC_AS_GPIO)
	{
		hal_gpio_pin_set_dir(spi_ctx[spi->port].spi_pin_CS0, HAL_GPIO_DIR_OUT, 1);
	}
	osMutexRelease(spi_ctx[spi->port].spi_mutex_id);
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
	int32_t ret = 0;
	uint32_t len = size;
	uint32_t remainder = 0;
	osStatus status = osOK;
	uint8_t *cmd;

	if (NULL == spi || NULL == data || 0 == size)
	{
		TRACE("spi input para err");
		return -3;
	}

	cmd = (uint8_t *)malloc(len);
	if (cmd == NULL)
	{
		TRACE("%s malloc size %d error\r", __FUNCTION__, len);
		return -1;
	}

	memset(cmd, 0, len);
	status = osMutexWait(spi_ctx[spi->port].spi_mutex_id, osWaitForever);
	if (osOK != status)
	{
		TRACE("%s spi_mutex wait error = 0x%X!", __func__, status);
		free(cmd);
		return -2;
	}

	hal_cache_sync(HAL_CACHE_ID_D_CACHE); //PSRAM must sync cache to memory when used dma
	/*if cs use as gpio, pull down cs at first*/
	if (spi_ctx[spi->port].spi_fun_CS0 == HAL_IOMUX_FUNC_AS_GPIO)
	{
		hal_gpio_pin_set_dir(spi_ctx[spi->port].spi_pin_CS0, HAL_GPIO_DIR_OUT, 0);
	}
	do
	{
		remainder = len <= SPI_DMA_MAX ? len  : SPI_DMA_MAX;
		//hal_cache_sync(HAL_CACHE_ID_I_CACHE);//PSRAM must sync cache to memory when used dma
		if (spi->config.t_mode == SPI_TRANSFER_DMA)
		{
			ret = spi_ctx[spi->port].spi_dma_recv(cmd, data, remainder, spi_ctx[spi->port].spi_dma_irq);
			if (osSemaphoreWait(spi_ctx[spi->port].spi_dma_semaphore, timeout) <= 0)
			{
				TRACE("SPI Read timeout!");
				goto OUT;
			}
		}
		else
		{
			ret = spi_ctx[spi->port].spi_recv(cmd, data, remainder);
		}
		len -= remainder;
		data += remainder;
		if (ret)
		{
			TRACE("spi tail fail %d, size %d", ret, len);
			goto OUT;
		}
	} while (len);
OUT:
	/*if cs use as gpio, pull pull up cs at the end*/
	if (spi_ctx[spi->port].spi_fun_CS0 == HAL_IOMUX_FUNC_AS_GPIO)
	{
		hal_gpio_pin_set_dir(spi_ctx[spi->port].spi_pin_CS0, HAL_GPIO_DIR_OUT, 1);
	}
	osMutexRelease(spi_ctx[spi->port].spi_mutex_id);
	free(cmd);
	return ret;
}

//full duplex recev
int32_t hal_spi_send_and_recv(spi_dev_t *spi, uint8_t *tx_data, uint16_t tx_size, uint8_t *rx_data,
							  uint16_t rx_size, uint32_t timeout)
{
	int32_t ret = 0;
	uint32_t len = rx_size;
	uint32_t offset = tx_size;
	uint32_t max_len = (len + offset) <= SPI_DMA_MAX ? (len + offset) : SPI_DMA_MAX;
	uint32_t remainder = 0;
	osStatus status = 0;
	uint8_t *cmd;

	if (NULL == spi || NULL == tx_data || 0 == tx_size || NULL == rx_data || 0 == rx_size)
	{
		TRACE("spi input para err");
		return -3;
	}

	cmd = (uint8_t *)malloc(max_len);
	if (cmd == NULL)
	{
		TRACE("%s malloc size %d error\r", __FUNCTION__, max_len);
		return -1;
	}

	memset(cmd, 0, max_len);
	memcpy(cmd, tx_data, tx_size);
	status = osMutexWait(spi_ctx[spi->port].spi_mutex_id, osWaitForever);
	if (osOK != status)
	{
		TRACE("%s spi_mutex wait error = 0x%X!", __func__, status);
		free(cmd);
		return -2;
	}

	/*if cs use as gpio, pull down cs at first*/
	if (spi_ctx[spi->port].spi_fun_CS0 == HAL_IOMUX_FUNC_AS_GPIO) {
		hal_gpio_pin_set_dir(spi_ctx[spi->port].spi_pin_CS0, HAL_GPIO_DIR_OUT, 0);
	}

	do
	{
		remainder = max_len;
		hal_cache_sync(HAL_CACHE_ID_D_CACHE); //PSRAM must sync cache to memory when used dma
		if (spi->config.t_mode == SPI_TRANSFER_DMA)
		{
			ret = spi_ctx[spi->port].spi_dma_recv(cmd, rx_data, remainder, spi_ctx[spi->port].spi_dma_irq);
			if (osSemaphoreWait(spi_ctx[spi->port].spi_dma_semaphore, 1000) <= 0)
			{
				TRACE("SPI Read timeout!");
				goto OUT;
			}
		}
		else
		{
			ret = spi_ctx[spi->port].spi_recv(cmd, rx_data, remainder);
		}
		len -= (remainder - offset);
		rx_data += (remainder - offset);
		if (ret)
		{
			TRACE("spi dma tail fail %d", ret);
			goto OUT;
		}
	} while (len);
OUT:
	/*if cs use as gpio, pull pull up cs at the end*/
	if (spi_ctx[spi->port].spi_fun_CS0 == HAL_IOMUX_FUNC_AS_GPIO) {
		hal_gpio_pin_set_dir(spi_ctx[spi->port].spi_pin_CS0, HAL_GPIO_DIR_OUT, 1);
	}

	osMutexRelease(spi_ctx[spi->port].spi_mutex_id);
	free(cmd);
	return ret;
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
//Half duplex send+recev
int32_t hal_spi_send_recv(spi_dev_t *spi, uint8_t *tx_data,
						  uint8_t *rx_data, uint16_t rx_size, uint32_t timeout)
{
	int ret = 0;
	uint32_t len = rx_size;
	uint32_t offset = 1;
	uint32_t i = 0;
	uint8_t *cmd;

	cmd = (uint8_t *)malloc(rx_size);
	if (cmd == NULL)
	{
		TRACE("%s malloc %d error", __FUNCTION__, rx_size);
		return -1;
	}
	for (size_t i = 0; i < len; i++)
	{
		cmd[i] = 0x00;
	}
	osStatus status = osMutexWait(spi_ctx[spi->port].spi_mutex_id, osWaitForever);
	if (osOK != status)
	{
		TRACE("%s spi_mutex wait error = 0x%X!", __func__, status);
		free(cmd);
		return -2;
	}
	hal_cache_sync(HAL_CACHE_ID_D_CACHE); //PSRAM must sync cache to memory when used dma
	if (spi->config.t_mode == SPI_TRANSFER_DMA)
	{
		ret = spi_ctx[spi->port].spi_dma_send(tx_data, 1, spi_ctx[spi->port].spi_dma_irq);
		if (osSemaphoreWait(spi_ctx[spi->port].spi_dma_semaphore, timeout) <= 0)
		{
			TRACE("spi dma tail send timeout");
			goto OUT;
		}
	}
	else
	{
		spi_ctx[spi->port].spi_send(tx_data, 1);
	}
	do
	{
		uint32_t remainder = len <= SPI_DMA_MAX ? len  : SPI_DMA_MAX;
		hal_cache_sync(HAL_CACHE_ID_I_CACHE);//PSRAM must sync cache to memory when used dma
		if (spi->config.t_mode == SPI_TRANSFER_DMA)
		{
			ret = spi_ctx[spi->port].spi_dma_recv(cmd, rx_data, remainder, spi_ctx[spi->port].spi_dma_irq);
			if (osSemaphoreWait(spi_ctx[spi->port].spi_dma_semaphore, timeout) <= 0)
			{
				TRACE("SPI Read timeout!");
				goto OUT;
			}
		}
		else
		{
			ret = spi_ctx[spi->port].spi_recv(cmd, rx_data, remainder);
		}
		len -= remainder;
		rx_data += remainder;
		if (ret)
		{
			TRACE("spi tail fail %d, size %d", ret, len);
			goto OUT;
		}
	} while (len);
OUT:
	osMutexRelease(spi_ctx[spi->port].spi_mutex_id);
	free(cmd);
	return ret;
}

/**
 * spi send data and then send data
 * @param[in]  spi       the spi device
 * @param[in]  tx1_data  the first data to be sent
 * @param[in]  tx1_size  the first data size to be sent
 * @param[out] tx2_data  the second data to be sent
 * @param[in]  tx2_size  the second data size to be sent
 * @param[in]  timeout   timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0, on success,  otherwise is error
 */
int32_t hal_spi_send_and_send(spi_dev_t *spi, uint8_t *tx1_data, uint16_t tx1_size, uint8_t *tx2_data,
							  uint16_t tx2_size, uint32_t timeout)
{
	//int spi_id = spi->port + 1;
	void *cmd = spi->priv;
	int ret = 0;
	ret = hal_spi_send(spi, tx1_data, tx1_size, timeout);
	if (!ret)
	{
		ret = hal_spi_send(spi, tx2_data, tx2_size, timeout);
		if (ret)
		{
			TRACE("spi send tx2_data fail %d, tx2_size %d", ret, tx2_size);
		}
	}
	else
	{
		TRACE("spi send tx1_data fail %d, tx1_size %d", ret, tx1_size);
	}

	return ret;
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
	int ret = 0;
	ret = spi_ctx[spi->port].spi_close(spi->port + 1);
	if (ret)
	{
		TRACE("hal_spi_finalize fail %d", ret);
	}
	return 0;
}

