#include "soc_types.h"
#include "spimst/drv_spimst.h"
#include <hal/soc/spi.h>
#include "errno.h"
#include "osal.h"

#ifndef DEV_ERROR(no)
#define DEV_ERROR(no) (no==0?0:EIO)
#endif

static int32_t spi_trans_data(const uint8_t *read_data,\
	const uint8_t *write_data,uint16_t size)
{
	uint8_t *duty_data = NULL;
	uint8_t *spi_rx_data = read_data;
	uint8_t *spi_tx_data = write_data;
	int ret;
	uint16_t index = 0;
	uint16_t trans_size;
	
	if(read_data == NULL || write_data == NULL)//alloc duty data
	{
		duty_data = OS_MemAlloc(size);
		if(duty_data == NULL)
			return EIO;
	}

	else if(read_data == NULL && write_data != NULL)//tx only
	{
		spi_rx_data = duty_data;
		spi_tx_data = write_data;
	}
	else if(read_data != NULL && write_data == NULL)//rx ready
	{
		spi_rx_data = read_data;
		spi_tx_data = duty_data;
	}

	do{
		if(size > 1024)
		{
			trans_size = 1024;
			size -= 1024;
		}
		else
		{
			trans_size = size;
			size = 0;
		}
		ret = drv_spi_mst_dma_trx(spi_tx_data+index, spi_rx_data+index, trans_size);
		if(ret != 0)
			break;

	}while(size != 0);

	if(duty_data != NULL)
		OS_MemFree(duty_data);

	return DEV_ERROR(ret);

}
int32_t hal_spi_init(spi_dev_t *spi)
{
	int ret;
	if(spi->config.freq < 1 * 1000 * 1000 || 
		spi->config.freq > 40 * 1000 * 1000)
	{
		return EIO;
	}

	if(spi->config.mode > 3)
	{
		return EIO;
	}

	ret = drv_spi_mst_init(spi->config.freq, 
		(spi->config.mode & 0x2) , 
		(spi->config.mode & 0x1));

	return DEV_ERROR(ret);;
}

int32_t hal_spi_send(spi_dev_t *spi, const uint8_t *data, uint16_t size, uint32_t timeout)
{
	return spi_trans_data(NULL, data, size);
}

int32_t hal_spi_recv(spi_dev_t *spi, uint8_t *data, uint16_t size, uint32_t timeout)
{
	return spi_trans_data(data, NULL, size);
}

int32_t hal_spi_send_recv(spi_dev_t *spi, uint8_t *tx_data, uint8_t *rx_data,
	                      uint16_t size, uint32_t timeout)
{
	return spi_trans_data(rx_data, tx_data, size);
}

int32_t hal_spi_finalize(spi_dev_t *spi)
{
	int ret;
	ret = drv_spi_mst_deinit();
	return DEV_ERROR(ret);
}


