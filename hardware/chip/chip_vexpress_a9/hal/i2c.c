/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdlib.h>
#include "aos/hal/i2c.h"
#include "mt_i2c.h"


/**
 * Initialises an I2C interface
 * Prepares an I2C hardware interface for communication as a master or slave
 *
 * @param[in]  i2c  the device for which the i2c port should be initialised
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t hal_i2c_init(i2c_dev_t *i2c)
{
	return 0;
}

/**
 * I2c master send
 *
 * @param[in]  i2c       the i2c device
 * @param[in]  dev_addr  device address
 * @param[in]  data      i2c send data
 * @param[in]  size      i2c send data size
 * @param[in]  timeout   timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
	int ret = I2C_OK;
	struct i2c_msg msgs;
	struct mtk_i2c i2c_mtk;
	struct mtk_i2c *i2c_t = &i2c_mtk;
	uint8_t *temp;

	memset(i2c_t, 0, sizeof(struct mtk_i2c));
	i2c_t->poll_en = true;
	i2c_t->dma_en = false;
	i2c_t->auto_restart = false;
	i2c_t->pushpull = false;
	i2c_t->filter_msg = false;
	i2c_t->id = i2c->port;
	i2c_t->speed = i2c->config.freq;
	i2c_t->timeout = timeout;
	i2c_t->mode = 0;

	msgs.addr = dev_addr;
	msgs.flags = 0;
	msgs.len = size;
	msgs.buf = data;
	ret = mtk_i2c_transfer(i2c_t, &msgs, 1);

	return ret;
}

/**
 * I2c master recv
 *
 * @param[in]   i2c       the i2c device
 * @param[in]   dev_addr  device address``
 * @param[out]  data      i2c receive data
 * @param[in]   size      i2c receive data size
 * @param[in]   timeout   timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                        if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
	int ret = I2C_OK;
	struct i2c_msg msgs;
	struct mtk_i2c i2c_mtk;
	struct mtk_i2c *i2c_t = &i2c_mtk;

	i2c_t->poll_en = true;
	i2c_t->dma_en = false;
	i2c_t->auto_restart = false;
	i2c_t->pushpull = false;
	i2c_t->filter_msg = false;
	i2c_t->id = i2c->port;
	i2c_t->speed = i2c->config.freq;
	i2c_t->timeout = timeout;
	msgs.addr = dev_addr;
	msgs.flags = 1;
	msgs.buf = data;
	msgs.len = size;
	ret = mtk_i2c_transfer(i2c_t, &msgs, 1);
	return ret;
}

/**
 * I2c slave send
 *
 * @param[in]  i2c      the i2c device
 * @param[in]  data     i2c slave send data
 * @param[in]  size     i2c slave send data size
 * @param[in]  timeout  timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                      if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t hal_i2c_slave_send(i2c_dev_t *i2c, const uint8_t *data, uint16_t size, uint32_t timeout)
{
	return 0;
}

/**
 * I2c slave receive
 *
 * @param[in]   i2c      tthe i2c device
 * @param[out]  data     i2c slave receive data
 * @param[in]   size     i2c slave receive data size
 * @param[in]  timeout   timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                       if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t hal_i2c_slave_recv(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
{
	return 0;
}

/**
 * I2c mem write
 *
 * @param[in]  i2c            the i2c device
 * @param[in]  dev_addr       device address
 * @param[in]  mem_addr       mem address
 * @param[in]  mem_addr_size  mem address
 * @param[in]  data           i2c master send data
 * @param[in]  size           i2c master send data size
 * @param[in]  timeout        timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                            if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
	int ret = I2C_OK;
	struct i2c_msg msgs;
	struct mtk_i2c i2c_mtk;
	struct mtk_i2c *i2c_t = &i2c_mtk;
	uint8_t *temp;
	uint8_t alen = (uint8_t)mem_addr_size;
	uint8_t i;

	memset(i2c_t, 0, sizeof(struct mtk_i2c));
	i2c_t->poll_en = true;
	i2c_t->dma_en = false;
	i2c_t->auto_restart = false;
	i2c_t->pushpull = false;
	i2c_t->filter_msg = false;
	i2c_t->id = i2c->port;
	i2c_t->speed = i2c->config.freq;
	i2c_t->timeout = timeout;
	i2c_t->mode = 0;

	msgs.addr = dev_addr;
	msgs.flags = 0;
	msgs.len = size + alen;

	if (alen > 0)
	{
		uint16_t i;

		temp =(uint8_t *)malloc((alen+size) *  sizeof(uint8_t));
		for (i = 0; i < size; i++)
			temp[i + alen] = data[i];

		for (i=0; alen-- >0; i++)
			temp[alen] = (mem_addr >> (i * 8)) & 0xFF;

			msgs.buf = temp;
	} else {
		msgs.buf = NULL;
	}
	ret = mtk_i2c_transfer(i2c_t, &msgs, 1);
	if (alen > 0)
		free(temp);

	return ret;
}

/**
 * I2c master mem read
 *
 * @param[in]   i2c            the i2c device
 * @param[in]   dev_addr       device address
 * @param[in]   mem_addr       mem address
 * @param[in]   mem_addr_size  mem address
 * @param[out]  data           i2c master send data
 * @param[in]   size           i2c master send data size
 * @param[in]  timeout         timeout in milisecond, set this value to HAL_WAIT_FOREVER
 *                             if you want to wait forever
 *
 * @return  0 : on success, EIO : if an error occurred during initialisation
 */
int32_t hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
	int ret = I2C_OK;
	struct i2c_msg msgs[2];
	struct mtk_i2c i2c_mtk;
	struct mtk_i2c *i2c_t = &i2c_mtk;
	uint8_t *temp;

	i2c_t->poll_en = true;
	i2c_t->dma_en = false;
	i2c_t->auto_restart = false;
	i2c_t->pushpull = false;
	i2c_t->filter_msg = false;
	i2c_t->id = i2c->port;
	i2c_t->speed = i2c->config.freq;
	i2c_t->timeout = timeout;
	msgs[0].addr = dev_addr;
	msgs[0].flags = 0;
	msgs[0].len = mem_addr_size;

	if (mem_addr_size == 1)
	{
		msgs[0].buf[0] = (uint8_t)mem_addr;
	} else if (mem_addr_size == 2) {
		msgs[0].buf[1] = (uint8_t)mem_addr;
		msgs[0].buf[0] = (uint8_t)(mem_addr >> 8);
	} else {
		printf("mem addr size could not more than 2\n");
		return -1;
	}
	msgs[1].addr = dev_addr;
	msgs[1].flags = 1;
	msgs[1].buf = data;
	msgs[1].len = size;
	ret = mtk_i2c_transfer(i2c_t, msgs, 2);
	return ret;
}

/**
 * Deinitialises an I2C device
 *
 * @param[in]  i2c  the i2c device
 *
 * @return  0 : on success, EIO : if an error occurred during deinitialisation
 */
int32_t hal_i2c_finalize(i2c_dev_t *i2c)
{
	return 0;
}

