#include "soc_types.h"
#include "i2cmst/drv_i2cmst.h"
#include <hal/soc/i2c.h>
#include "errno.h"
#include <stdio.h>
#include "osal.h"

#ifndef DEV_ERROR(no)
#define DEV_ERROR(no) (no==0?0:EIO)
#endif

static void set_spi_timeout(uint32_t timeout)
{
	drv_i2c_mst_retry_wait_time_us(timeout * 1000);
	drv_i2c_mst_retry_count(1);
}
int32_t hal_i2c_init(i2c_dev_t *i2c)
{
	int ret;
	if(i2c->config.freq < 100 || i2c->config.freq > 1000 * 1000)
	{
		printf("Error set freq:I2C Clock (100 - 1000 KHz)\n");
		return EIO;
	}

	if(i2c->config.mode == I2C_MODE_SLAVE)
	{
		printf("Error set mode.Unsuport SLAVE mode\n");
		return EIO;
	}

	ret = drv_i2c_mst_init(i2c->config.freq);
	return DEV_ERROR(ret);
}

int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
	int ret;
	
	set_spi_timeout(timeout);
	ret = drv_i2c_mst_write(dev_addr, data, size, 1, 1);

	return DEV_ERROR(ret);

}

int32_t hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
	int ret;

	set_spi_timeout(timeout);
	ret = drv_i2c_mst_read(dev_addr, data, size, 1, 1);

	return DEV_ERROR(ret);
}

int32_t hal_i2c_slave_send(i2c_dev_t *i2c, const uint8_t *data, uint16_t size, uint32_t timeout)
{
	return EIO;
}

int32_t hal_i2c_slave_recv(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
{
	return EIO;
}

int32_t hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
	uint8_t *write_buffer;
	int ret = EIO;

	write_buffer = OS_MemAlloc(mem_addr_size + size);

	if(write_buffer == NULL)
	{
		return EIO;
	}

	memcpy(write_buffer, mem_addr, mem_addr_size);
	memcpy(write_buffer + mem_addr_size, data, size);

	set_spi_timeout(timeout);
	ret = drv_i2c_mst_write(dev_addr, write_buffer, mem_addr_size + size, 1, 1);

	OS_MemFree(write_buffer);

	return DEV_ERROR(ret);	
}

int32_t hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
	int ret;

	set_spi_timeout(timeout);
	if(0 != drv_i2c_mst_write(dev_addr, (uint8_t *)&mem_addr, mem_addr_size, 1, 1)){
		printf("write fail!\n");
		return EIO;
	}

	if(0 != drv_i2c_mst_read(dev_addr, data, size, 1, 1)){
		printf("read fail!\n");
		return EIO;
	}

	return 0;
}

int32_t hal_i2c_finalize(i2c_dev_t *i2c)
{
	int ret;
	ret = drv_i2c_mst_deinit();
	return DEV_ERROR(ret);
}




