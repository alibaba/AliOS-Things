/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include "hal/soc/soc.h"
#include "driver/chip/hal_i2c.h"

/**@brief Initialises an I2C interface
 * @note Prepares an I2C hardware interface for communication as a master or slave
 *
 * @param     device : the device for which the i2c port should be initialised
 * @return    0      : on success.
 * @return    EIO    : if an error occurred during initialisation
 */
int32_t hal_i2c_init(i2c_dev_t *i2c)
{
	I2C_InitParam initParam = {0};
	HAL_Status status = HAL_ERROR;
	I2C_ID id;

	if (i2c == NULL) {
		return -1;
	}
	if (i2c->config.address_width == 7) {
		initParam.addrMode = I2C_ADDR_MODE_7BIT;
	} else if (i2c->config.address_width == 10) {
		initParam.addrMode = I2C_ADDR_MODE_10BIT;
	} else {
		return -1;
	}

	initParam.clockFreq = i2c->config.freq;
	if (i2c->port == 0) {
		id = I2C0_ID;
	} else if (i2c->port == 1) {
		id = I2C1_ID;
	} else {
		return -1;
	}

	status = HAL_I2C_Init(id, &initParam);
	if (status != HAL_OK) {
		printf("IIC init error %d\n", status);
		return -1;
	}
	return 0;
}


/**@brief     i2c master send
 * @param     i2c           : the i2c device
 * @param     dev_addr      : device address
 * @param     data          : i2c send data
 * @param     size          : i2c send data size
 * @param     timeout       : timeout in ms
 * @return    0             : on success.
 * @return    EIO           : if an error occurred during initialisation
 */
int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data, uint16_t size, uint32_t timeout)
{
	HAL_Status status = HAL_ERROR;
	I2C_ID id;

	if (i2c == NULL) {
		return -1;
	}
	if (i2c->port == 0) {
		id = I2C0_ID;
	} else if (i2c->port == 1) {
		id = I2C1_ID;
	} else {
		return -1;
	}
	status = HAL_I2C_Master_Transmit_IT(id, (uint16_t)dev_addr, data, (uint32_t)size);
	if (status != HAL_OK) {
		printf("hal_i2c_master_send error %d\n", status);
		return -1;
	}
	return 0;
}


/**@brief     i2c master recv
 *
 * @param     i2c         : the i2c device
 * @param     dev_addr    : device address
 * @param     data        : i2c receive data
 * @param     size        : i2c receive data size
 * @param     timeout     : timeout in ms
 * @return    0           : on success.
 * @return    EIO         : if an error occurred during initialisation
 */
int32_t hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data, uint16_t size, uint32_t timeout)
{
	HAL_Status status = HAL_ERROR;
	I2C_ID id;

	if (i2c == NULL) {
		return -1;
	}
	if (i2c->port == 0) {
		id = I2C0_ID;
	} else if (i2c->port == 1) {
		id = I2C1_ID;
	} else {
		return -1;
	}
	status = HAL_I2C_Master_Receive_IT(id, (uint16_t)dev_addr, data, (uint32_t)size);
	if (status != HAL_OK) {
		printf("hal_i2c_master_recv error %d\n", status);
		return -1;
	}
	return 0;
}

/**@brief hal_i2C_slave_send
 *
 * @param     i2c         : the i2c device
 * @param     data        : i2c slave send data
 * @param     size        : i2c slave send data size
 * @param     timeout     : timeout in ms
 * @return    0           : on success.
 * @return    EIO         : if an error occurred during initialisation
 */
int32_t hal_i2C_slave_send(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
{
	//TODO:
	return -1;
}


/**@brief Initialises an I2C interface
 *
 * @param     i2c         : tthe i2c device
 * @param     data        : i2c slave receive data
 * @param     size        : i2c slave receive data size
 * @param     timeout     : timeout in ms
 * @return    0           : on success.
 * @return    EIO         : if an error occurred during initialisation
 */
int32_t hal_i2c_slave_recv(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
{
	//TODO:
	return -1;
}


/**@brief i2c mem write
 *
 * @param     i2c           : the i2c device
 * @param     dev_addr      : device address
 * @param     mem_addr      : mem address
 * @param     mem_addr_size : mem address
 * @param     data          : i2c master send data
 * @param     size          : i2c master send data size
 * @param     timeout       : timeout in ms
 * @return    0             : on success.
 * @return    EIO           : if an error occurred during initialisation
 */
int32_t hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size, const uint8_t *data,
						  uint16_t size, uint32_t timeout)
{
	HAL_Status status = HAL_ERROR;
	I2C_ID id;

	if (i2c == NULL) {
		return -1;
	}
	if (i2c->port == 0) {
		id = I2C0_ID;
	} else if (i2c->port == 1) {
		id = I2C1_ID;
	} else {
		return -1;
	}
	//FIXUP: mem addr from uint16_t to uint8_t
	status = HAL_I2C_Master_Transmit_Mem_IT(id, dev_addr, (uint8_t)mem_addr & 0xff, data, (uint32_t)size);
	if (status != HAL_OK) {
		printf("hal_i2c_mem_write error %d\n", status);
		return -1;
	}
	return 0;
}

/**@brief i2c master mem read
 * @param     i2c           : the i2c device
 * @param     dev_addr      : device address
 * @param     mem_addr      : mem address
 * @param     mem_addr_size : mem address
 * @param     data          : i2c master send data
 * @param     size          : i2c master send data size
 * @param     timeout       : timeout in ms
 * @return    0             : on success.
 * @return    EIO           : if an error occurred during initialisation
 */
int32_t hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr, uint16_t mem_addr_size, uint8_t *data,
                         uint16_t size, uint32_t timeout)
{
	HAL_Status status = HAL_ERROR;
	I2C_ID id;

	if (i2c == NULL) {
		return -1;
	}
	if (i2c->port == 0) {
		id = I2C0_ID;
	} else if (i2c->port == 1) {
		id = I2C1_ID;
	} else {
		return -1;
	}
	//FIXUP: mem addr from uint16_t to uint8_t
	status = HAL_I2C_Master_Receive_Mem_IT(id, dev_addr, (uint8_t)mem_addr & 0xff, data, (uint32_t)size);
	if (status != HAL_OK) {
		printf("hal_i2c_mem_read error %d\n", status);
		return -1;
	}
	return 0;
}

/**@brief Deinitialises an I2C device
 *
 * @param     device : the i2c device
 * @return    0      : on success.
 * @return    EIO    : if an error occurred during deinitialisation
 */
int32_t hal_i2c_finalize(i2c_dev_t *i2c)
{
	HAL_Status status = HAL_ERROR;
	I2C_ID id;

	if (i2c == NULL) {
		return -1;
	}
	if (i2c->port == 0) {
		id = I2C0_ID;
	} else if (i2c->port == 1) {
		id = I2C1_ID;
	} else {
		return -1;
	}
	status = HAL_I2C_DeInit(id);
	if (status != HAL_OK) {
		printf("IIC deinit error %d\n", status);
		return -1;
	}
	return 0;
}
