/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/hal/i2c.h"
#include "hal_i2c.h"
#include "app_hal.h"
#include "aos/kernel.h"
#include "hal_trace.h"
aos_mutex_t i2c_mutex;
static bool mutex_init = false;

/*#define I2C1_IOMUX_INDEX                    22*/

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
	uint32_t ret = 0;
	int32_t lock_ret = -1;
	uint8_t i2c_port;
	struct HAL_I2C_CONFIG_T i2c_cfg;

    if (mutex_init == false) {
        aos_mutex_new(&i2c_mutex);
        mutex_init = true;
    }

	lock_ret = aos_mutex_lock(&i2c_mutex, AOS_WAIT_FOREVER);
	if (lock_ret != 0) {
		TRACE("get i2c_mutex lock fail");
		return lock_ret;
    }

	i2c_port = i2c->port;

	if (i2c_port == 0) {
		hal_iomux_set_i2c0();
	} else if (i2c_port == 1) {
		hal_iomux_set_i2c1();
	}

	i2c_cfg.mode = HAL_I2C_API_MODE_TASK;
	i2c_cfg.use_dma = 0;
	i2c_cfg.use_sync = 1;
	i2c_cfg.speed = i2c->config.freq;
	i2c_cfg.as_master = 1;

	ret = hal_i2c_open(i2c_port, &i2c_cfg);
	if (ret) {
		TRACEME("open i2c fail\n");
	} else {
		TRACEME("open i2c succ.\n");
	}
	aos_mutex_unlock(&i2c_mutex);

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

	int32_t ret;
	uint8_t i2c_port;
	int32_t lock_ret = -1;

	lock_ret = aos_mutex_lock(&i2c_mutex, timeout);
	if (lock_ret != 0) {
		TRACE("i2c_master_send, get i2c_mutex lock fail");
		return lock_ret;
	}
	i2c_port = i2c->port;
	ret = hal_i2c_task_send(i2c_port, dev_addr, data, size, 0, NULL);
	if(ret) {
		TRACEME("%s:%d,i2c send fail, dev_addr = 0x%x, data[0] = 0x%x, data[1]= 0x%x, ret = %d\n", __func__,__LINE__,dev_addr, data[0], data[1], ret);
	}
	aos_mutex_unlock(&i2c_mutex);
	return ret;
}

/**
 * I2c master recv
 *
 * @param[in]   i2c       the i2c device
 * @param[in]   dev_addr  device address
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
	int32_t ret;
	uint8_t i2c_port;
	int32_t lock_ret = -1;

	lock_ret = aos_mutex_lock(&i2c_mutex, timeout);
	if (lock_ret != 0) {
		TRACE("i2c_master_recv, get i2c_mutex lock fail");
		return lock_ret;
	}
	i2c_port = i2c->port;
	/*ret = hal_i2c_recv(i2c_port, dev_addr, data, 1, size - 1, HAL_I2C_RESTART_AFTER_WRITE, 0, NULL);*/
	ret = hal_i2c_recv(i2c_port, dev_addr, data, 0, size, HAL_I2C_RESTART_AFTER_WRITE, 0, NULL);
	if(ret) {
		TRACEME("%s:%d,i2c read failed, dev_addr = 0x%x, data[0] = 0x%x, data[1]= 0x%x, ret = %d\n", __func__,__LINE__,dev_addr, data[0], data[1], ret);
	}
	aos_mutex_unlock(&i2c_mutex);
	return ret;
}

uint32_t hal_i2c_master_recv_vendor(i2c_dev_t *i2c, uint16_t device_addr, uint8_t *tx_buf, uint16_t tx_len, uint8_t *rx_buf, uint16_t rx_len)
{
	int32_t ret;
	uint8_t i2c_port;
	int32_t lock_ret = -1;

	lock_ret = aos_mutex_lock(&i2c_mutex, AOS_WAIT_FOREVER);
	if (lock_ret != 0) {
		TRACE("i2c_master_recv, get i2c_mutex lock fail");
		return lock_ret;
	}
	i2c_port = i2c->port;

	ret = hal_i2c_task_recv(i2c_port, device_addr, tx_buf, tx_len, rx_buf, rx_len, 0, NULL);
	if(ret) {
		TRACEME("%s:%d,i2c read failed, dev_addr=0x%x, data[0]=0x%x, ret=%d", __func__,__LINE__,device_addr, ret);
	}
	aos_mutex_unlock(&i2c_mutex);
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
	uint32_t act_write;
	int32_t ret;
	uint8_t i2c_port;
	int32_t lock_ret = -1;

	lock_ret = aos_mutex_lock(&i2c_mutex, timeout);
	if (lock_ret != 0) {
		TRACE("hal_i2c_slave_send, get i2c_mutex lock fail");
		return lock_ret;
	}

	i2c_port = i2c->port;
	ret = hal_i2c_slv_write(i2c_port, data, size, &act_write);
	aos_mutex_unlock(&i2c_mutex);

	return ret;
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
	uint32_t act_read;
	int32_t ret;
	uint8_t i2c_port;
	int32_t lock_ret = -1;

	lock_ret = aos_mutex_lock(&i2c_mutex, timeout);
	if (lock_ret != 0) {
		TRACE("hal_i2c_slave_recv, get i2c_mutex lock fail");
		return lock_ret;
	}

	i2c_port = i2c->port;

	ret = hal_i2c_slv_read(i2c_port, data, size, &act_read);
	aos_mutex_unlock(&i2c_mutex);
	return ret;
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
	int32_t ret;
	uint8_t i2c_port;
	int32_t lock_ret = -1;
	uint8_t *txbuf;
	uint16_t txlen;

	if((NULL == i2c) || (NULL == data) || (0 == mem_addr_size) || (0 == size))
	{
		TRACE("i2c input para err");
		return -1;
	}

	i2c_port = i2c->port;

	txlen = size + mem_addr_size;
	txbuf = (uint8_t *)malloc(txlen);
	if (txbuf  == NULL)
	{
		TRACE("%s malloc size %d error\r", __FUNCTION__, txlen);
		return -1;
	}

	memset(txbuf, 0, txlen);
	txbuf[0] = (uint8_t)mem_addr;
	memcpy(&txbuf[1], data, size);

	lock_ret = aos_mutex_lock(&i2c_mutex, timeout);
	if (lock_ret != 0) {
		TRACE("hal_i2c_mem_write, get i2c_mutex lock fail");
		free(txbuf);
		return lock_ret;
	}
	ret = hal_i2c_task_send(i2c_port, dev_addr, txbuf, txlen, 0, NULL);
	if(ret) {
		TRACEME("%s:%d,i2c send failed,dev_addr = 0x%x,ret = %d\n", __func__,__LINE__,dev_addr,ret);
	}
	aos_mutex_unlock(&i2c_mutex);
	free(txbuf);

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
	int32_t ret;
	uint8_t i2c_port;
	int32_t lock_ret = -1;
	uint8_t *txrxbuf;
	uint16_t txrxlen;

	if((NULL == i2c) || (NULL == data) || (0 == mem_addr_size) || (0 == size))
	{
		TRACE("i2c input para err");
		return -1;
	}

	i2c_port = i2c->port;

	txrxlen = size + mem_addr_size;
	txrxbuf = (uint8_t *)malloc(txrxlen);
	if (txrxbuf  == NULL)
	{
		TRACE("%s malloc size %d error\r", __FUNCTION__, txrxlen);
		return -1;
	}

	memset(txrxbuf, 0, txrxlen);
	txrxbuf[0] = (uint8_t)mem_addr;

	lock_ret = aos_mutex_lock(&i2c_mutex, timeout);
	if (lock_ret != 0) {
		TRACE("hal_i2c_mem_read, get i2c_mutex lock fail");
		free(txrxbuf);
		return lock_ret;
	}

	ret = hal_i2c_recv(i2c_port, dev_addr, txrxbuf, mem_addr_size, size, HAL_I2C_RESTART_AFTER_WRITE, 0, 0);
	if (ret) {
		TRACEME("%s:i2c recv failed,dev_addr = 0x%x,ret = %d\n", __func__, dev_addr, ret);
	} else {
		memcpy(data, &txrxbuf[1], size);
	}

	aos_mutex_unlock(&i2c_mutex);

	free(txrxbuf);

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


