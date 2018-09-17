/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "hal/hal.h"
#include "k_types.h"
#include "errno.h"

#include <fsl_i2c.h>
#include <LPC54102_cm4.h>


#define LOG_TAG		"i2c"

static int i2c_transfer(I2C_Type *base, uint8_t slave, uint16_t reg_addr, uint16_t addr_len,
		uint8_t *data, uint16_t len, uint32_t timeout, int dir)
{
	i2c_master_transfer_t xfer;
	int rem_len = len;
	uint8_t *pdata = data;

	memset(&xfer, 0, sizeof(i2c_master_transfer_t));
	xfer.slaveAddress = slave >> 1;
	xfer.direction = dir;
	xfer.dataSize = 1;
	xfer.subaddressSize = addr_len;

	while (rem_len-- > 0) {
		xfer.data = pdata++;
		xfer.subaddress = reg_addr++;
		I2C_MasterTransferBlocking(base, &xfer);
	}

	return 0;
}

static I2C_Type *hal_get_i2c_base(uint8_t port)
{
	switch (port) {
	case 0:
		return I2C0;
	case 1:
		return I2C1;
	case 2:
		return I2C2;
	default:
		return NULL;
	}
	return NULL;
}


int32_t hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
	uint16_t mem_addr_size, const uint8_t *data, uint16_t size, uint32_t timeout)
{
	uint8_t send_buff[size];

	if (!i2c || !data) {
		LOGE(LOG_TAG, "%s: invalid arguments !", __func__);
		return -1;
	}

	memcpy(send_buff, data, size);
	I2C_Type *base = hal_get_i2c_base(i2c->port);
	if (!base) {
		LOGE(LOG_TAG, "%s: I2C%u base not found !", __func__, i2c->port);
		return -2;
	}

	return i2c_transfer(base, dev_addr, mem_addr, mem_addr_size, send_buff, size, timeout, kI2C_Write);
};

int32_t hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
	uint16_t mem_addr_size, uint8_t *data, uint16_t size, uint32_t timeout)
{
	if (!i2c || !data || size <= 0) {
		LOGE(LOG_TAG, "%s: invalid arguments !", __func__);
		return -1;
	}

	I2C_Type *base = hal_get_i2c_base(i2c->port);
	if (!base) {
		LOGE(LOG_TAG, "%s: I2C%u base not found !", __func__, i2c->port);
		return -2;
	}

	return i2c_transfer(base, dev_addr, mem_addr, mem_addr_size, data, size, timeout, kI2C_Read);
}


int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
	uint16_t size, uint32_t timeout)
{
	uint8_t send_buff[size];
	if (!i2c || !data) {
		LOGE(LOG_TAG, "%s: invalid arguments !", __func__);
		return -1;
	}

	I2C_Type *base = hal_get_i2c_base(i2c->port);
	if (!base) {
		LOGE(LOG_TAG, "%s: I2C%u base not found !", __func__, i2c->port);
		return -2;
	}

	i2c_master_transfer_t xfer;
	uint32_t subaddress = 0;

	memcpy(send_buff, data, size);
	memset(&xfer, 0, sizeof(i2c_master_transfer_t));
	xfer.slaveAddress = dev_addr;
	xfer.direction = kI2C_Write;
	xfer.data = send_buff;
	xfer.dataSize = size;
	xfer.subaddress = 0;
	xfer.subaddressSize = 0;
	I2C_MasterTransferBlocking(base, &xfer);

	return 0;
}

int32_t hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
	uint16_t size, uint32_t timeout)
{
	if (!i2c || !data) {
		LOGE(TAG, "%s: invalid arguments !", __func__);
		return -1;
	}

	I2C_Type *base = hal_get_i2c_base(i2c->port);
	if (!base) {
		LOGE(TAG, "%s: I2C%u base not found !",
			__func__, i2c->port);
		return -2;
	}

	i2c_master_transfer_t xfer;
	uint32_t subaddress = 0;

	memset(&xfer, 0, sizeof(i2c_master_transfer_t));
	xfer.slaveAddress = dev_addr;
	xfer.direction = kI2C_Read;
	xfer.data = data;
	xfer.dataSize = size;
	xfer.subaddress = 0;
	xfer.subaddressSize = 0;
	I2C_MasterTransferBlocking(base, &xfer);

    return 0;
}


int32_t hal_i2c_init(i2c_dev_t *i2c)
{
	i2c_master_config_t conf;

    I2C_Type *base = hal_get_i2c_base(i2c->port);
	if (!base) {
		LOGE(TAG, "%s: I2C%u base not found !", __func__, i2c->port);
		return -1;
	}

	memset(&conf, 0, sizeof(i2c_master_config_t));
	conf.enableMaster = true;
	conf.enableTimeout = 0;
	conf.baudRate_Bps = 400000;
	I2C_MasterInit(base, &conf, CLOCK_GetFreq(kCLOCK_I2c));
	/* I2C clock refer to LPC5410x User manual Rev2.5 Page332 */

	LOG("%s: init i2c%u success", __func__, i2c->port);
	return 0;
}