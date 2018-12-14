/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
 

#include "aos/kernel.h"
#include "aos/hal/i2c.h"

#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CMSP432.h>

#include <ti/drivers/I2CSlave.h>
#include <ti/drivers/i2cslave/I2CSlaveMSP432.h>

#define I2C_MEM_ADD_MSB(__ADDRESS__)              ((uint8_t)((uint16_t)(((uint16_t)((__ADDRESS__) & (uint16_t)(0xFF00U))) >> 8U)))
#define I2C_MEM_ADD_LSB(__ADDRESS__)              ((uint8_t)((uint16_t)((__ADDRESS__) & (uint16_t)(0x00FFU))))

extern const I2C_Config I2C_config[];
extern const uint_least8_t I2C_count;

extern const I2CSlave_Config I2CSlave_config[];
extern const uint_least8_t I2CSlave_count;


static void i2c_param_transfer (i2c_dev_t *i2c, I2C_Params *pstparams)
{
    switch (i2c->config.freq) {
        case I2C_BUS_BIT_RATES_100K:
            pstparams->bitRate = I2C_100kHz;
            break;
        case I2C_BUS_BIT_RATES_400K:
            pstparams->bitRate = I2C_400kHz;
            break;
        case I2C_BUS_BIT_RATES_3400K:
            pstparams->bitRate = I2C_3330kHz;
            break;
        default:
            pstparams->bitRate = I2C_100kHz;
            break;
    }
    return;
}

static int32_t hal_i2c_master_init (i2c_dev_t *i2c)
{
    I2C_Params stparams = {0};
    I2C_Handle handle = NULL;
    I2CMSP432_Object *object = NULL;
    
    if (i2c->port >= I2C_count) {
        printf("i2c init fail for invalid i2c port %d \r\n", i2c->port);
        return -1;
    }
    
    stparams.transferMode = I2C_MODE_BLOCKING;
    i2c_param_transfer(i2c, &stparams);

    /* Get handle for this driver instance */
    handle = (I2C_Handle)&(I2C_config[i2c->port]);
    object = (I2CMSP432_Object *)handle->object;

    if (object->isOpen == true) {
        return 0;
    }
    handle = I2CMSP432_open(handle, &stparams);
    if (NULL == handle) {
        printf("I2CMSP432_open fail \r\n");
        return -1;
    }

    i2c->priv = handle;

    return 0;
}

static int32_t hal_i2c_slave_init (i2c_dev_t *i2c)
{
    I2CSlave_Params stparams = {0};
    I2CSlave_Handle handle = NULL;
    I2CSlaveMSP432_Object *object = NULL;
    
    if (i2c->port >= I2CSlave_count) {
        printf("i2c init fail for invalid i2c port %d \r\n", i2c->port);
        return -1;
    }

    stparams.transferMode = I2CSLAVE_MODE_BLOCKING;
    /* Get handle for this driver instance */
    handle = (I2CSlave_Handle)&(I2CSlave_config[i2c->port]);
    object = (I2CSlaveMSP432_Object *)handle->object;
    if (object->isOpen == true) {
        return 0;
    }

    handle = I2CSlaveMSP432_open(handle, &stparams);
    if (NULL == handle) {
        printf("I2CSlaveMSP432_open fail \r\n");
        return -1;
    }

    i2c->priv = handle;
    return 0;
}

int32_t hal_i2c_init(i2c_dev_t *i2c)
{
    int ret = 0;
    
    if (NULL == i2c) {
        printf("i2c init fail for invalid input param\r\n");
        return -1;
    }

    if (i2c->config.mode == I2C_MODE_MASTER) {
        ret = hal_i2c_master_init(i2c);
    } else if (i2c->config.mode == I2C_MODE_SLAVE) {
        ret = hal_i2c_slave_init(i2c);
    } else {
        printf("i2c init fail for invalid i2c mode %d \r\n", i2c->config.mode);
        return -1;
    }

    if (ret) {
        printf("i2c init fail in %d mode \r\n", i2c->config.mode);
    }
	return ret;
}

int32_t hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    int ret = 0;
    I2C_Transaction i2cTransaction = {0};
    I2C_Handle handle = NULL;
    I2CMSP432_Object *object = NULL;

    if (NULL == i2c || NULL == data) {
        printf("input param invalid in %s \r\n", __func__);
        return -1;
    }

    if (i2c->port >= I2C_count) {
        printf("i2c master send fail for invalid i2c port %d \r\n", i2c->port);
        return -1;
    }

    handle = (I2C_Handle)&(I2C_config[i2c->port]);
    object = handle->object;

    if (object->isOpen != true) {
        printf("i2c master send fail for i2c port %d haven't init yet\r\n", i2c->port);
        return -1;
    }
    
    /*for now timeout is useless*/
    
    i2cTransaction.slaveAddress = ((uint8_t)dev_addr >> 1) & 0xff;
    i2cTransaction.writeBuf = data;
    i2cTransaction.writeCount = size;
    i2cTransaction.readBuf = NULL;
    i2cTransaction.readCount = 0;

    /*in block mode, arg is useless*/
    i2cTransaction.arg = NULL;

    ret = I2CMSP432_transfer(handle, &i2cTransaction);
    if (ret == false) {
        printf("i2c master send fail %d \r\n", ret);
        return -1;
    }

    return 0;
}

int32_t hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    int ret = 0;
    I2C_Transaction i2cTransaction = {0};
    I2C_Handle handle = NULL;
    I2CMSP432_Object *object = NULL;

    if (NULL == i2c || NULL == data) {
        printf("input param invalid in %s \r\n", __func__);
        return -1;
    }

    if (i2c->port >= I2C_count) {
        printf("i2c master send fail for invalid i2c port %d \r\n", i2c->port);
        return -1;
    }

    handle = (I2C_Handle)&(I2C_config[i2c->port]);
    object = (I2CMSP432_Object *)handle->object;

    if (object->isOpen != true) {
        printf("i2c master send fail for i2c port %d haven't init yet\r\n", i2c->port);
        return -1;
    }
    
    /*for now timeout is useless*/
    
    i2cTransaction.slaveAddress = ((uint8_t)dev_addr >> 1) & 0xff;
    i2cTransaction.writeBuf = NULL;
    i2cTransaction.writeCount = 0;
    i2cTransaction.readBuf = data;
    i2cTransaction.readCount = size;

    /*in block mode, arg is useless*/
    i2cTransaction.arg = NULL;

    ret = I2CMSP432_transfer(handle, &i2cTransaction);
    if (ret == false) {
        printf("i2c master recv fail %d \r\n", ret);
        return -1;
    }

    return 0;
}

int32_t hal_i2c_slave_send(i2c_dev_t *i2c, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    bool ret = false;
    I2CSlave_Handle handle = NULL;
    I2CSlaveMSP432_Object *object = NULL;
    
    if (NULL == i2c || NULL == data || 0 == size) {
        printf("input param invalid in %s \r\n", __func__);
        return -1;
    }

    if (i2c->port >= I2CSlave_count) {
        printf("i2c slave send fail for invalid i2c port %d \r\n", i2c->port);
        return -1;
    }

    handle = (I2CSlave_Handle)&(I2CSlave_config[i2c->port]);
    object = (I2CSlaveMSP432_Object *)handle->object;
    if (object->isOpen != true) {
        printf("i2c slave send fail for i2c port %d haven't init yet\r\n", i2c->port);
        return -1;
    }
    
    ret = I2CSlaveMSP432_write(handle, data, size);
    if (ret == false) {
        printf("I2CSlaveMSP432_write return false\r\n");
        return -1;
    }
    
    return 0;
}

int32_t hal_i2c_slave_recv(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
{
    bool ret = false;
    I2CSlave_Handle handle = NULL;
    I2CSlaveMSP432_Object *object = NULL;
    
    if (NULL == i2c || NULL == data || 0 == size) {
        printf("input param invalid in %s \r\n", __func__);
        return -1;
    }

    if (i2c->port >= I2CSlave_count) {
        printf("i2c slave recv fail for invalid i2c port %d \r\n", i2c->port);
        return -1;
    }

    handle = (I2CSlave_Handle)&(I2CSlave_config[i2c->port]);
    object = (I2CSlaveMSP432_Object *)handle->object;
    if (object->isOpen != true) {
        printf("i2c slave recv fail for i2c port %d haven't init yet\r\n", i2c->port);
        return -1;
    }
    
    ret = I2CSlaveMSP432_read(handle, data, size);
    if (ret == false) {
        printf("I2CSlaveMSP432_read return false\r\n");
        return -1;
    }
	return 0;
}

int32_t hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{
    int ret = 0;
    size_t writecount = 0;
    uint8_t *pwritebuf = NULL;
    I2C_Transaction i2cTransaction = {0};
    I2C_Handle handle = NULL;
    I2CMSP432_Object *object = NULL;

    if (NULL == i2c || NULL == data || size == 0 
        || ((mem_addr_size != I2C_MEM_ADDR_SIZE_8BIT) && (mem_addr_size != I2C_MEM_ADDR_SIZE_16BIT))) {
        printf("input param invalid in %s \r\n", __func__);
        return -1;
    }


    if (i2c->port >= I2C_count) {
        printf("i2c mem write fail for invalid i2c port %d \r\n", i2c->port);
        return -1;
    }

    handle = (I2C_Handle)&(I2C_config[i2c->port]);
    object = handle->object;

    if (object->isOpen != true) {
        printf("i2c mem write fail for i2c port %d haven't init yet\r\n", i2c->port);
        return -1;
    }

    writecount = size + mem_addr_size;
    pwritebuf = aos_malloc(writecount);
    if (NULL == pwritebuf) {
        printf("i2c master mem write fail for i2c memory %d malloc fail\r\n", writecount);
        return -1;
    }
    
    if (mem_addr_size == I2C_MEM_ADDR_SIZE_8BIT) {
        pwritebuf[0] = I2C_MEM_ADD_LSB(mem_addr);
    } else if (mem_addr_size == I2C_MEM_ADDR_SIZE_16BIT) {
        pwritebuf[0] = I2C_MEM_ADD_MSB(mem_addr);
        pwritebuf[1] = I2C_MEM_ADD_LSB(mem_addr);
    } 
    memcpy(pwritebuf + mem_addr_size, data, size);
    
    /*for now timeout is useless*/
    
    i2cTransaction.slaveAddress = ((uint8_t)dev_addr >> 1) & 0xff;
    i2cTransaction.writeBuf = pwritebuf;
    i2cTransaction.writeCount = writecount;
    i2cTransaction.readBuf = NULL;
    i2cTransaction.readCount = 0;

    /*in block mode, arg is useless*/
    i2cTransaction.arg = NULL;

    ret = I2CMSP432_transfer(handle, &i2cTransaction);

    aos_free(pwritebuf);
    if (!ret) {
        printf("i2c transfer fail %d \r\n", ret);
        return -1;
    }

    return 0;
}

int32_t hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                       uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                       uint32_t timeout)
{
    int ret = 0;
    uint8_t mem_address[2] = {0};
    I2C_Transaction i2cTransaction = {0};
    I2C_Handle handle = NULL;
    I2CMSP432_Object *object = NULL;

    if (NULL == i2c || NULL == data || size == 0 
        || ((mem_addr_size != I2C_MEM_ADDR_SIZE_8BIT) && (mem_addr_size != I2C_MEM_ADDR_SIZE_16BIT))) {
        printf("input param invalid in %s \r\n", __func__);
        return -1;
    }

    if (i2c->port >= I2C_count) {
        printf("i2c master mem read for invalid i2c port %d \r\n", i2c->port);
        return -1;
    }

    handle = (I2C_Handle)&(I2C_config[i2c->port]);
    object = handle->object;

    if (object->isOpen != true) {
        printf("i2c master mem read for i2c port %d haven't init yet\r\n", i2c->port);
        return -1;
    }

    if (mem_addr_size == I2C_MEM_ADDR_SIZE_8BIT) {
        mem_address[0] = I2C_MEM_ADD_LSB(mem_addr);
    } else if (mem_addr_size == I2C_MEM_ADDR_SIZE_16BIT) {
        mem_address[0] = I2C_MEM_ADD_MSB(mem_addr);
        mem_address[1] = I2C_MEM_ADD_LSB(mem_addr);
    }    
    /*for now timeout is useless*/

    i2cTransaction.slaveAddress = ((uint8_t)dev_addr >> 1) & 0xff;
    i2cTransaction.writeBuf = mem_address;
    i2cTransaction.writeCount = mem_addr_size;
    i2cTransaction.readBuf = data;
    i2cTransaction.readCount = size;

    /*in block mode, arg is useless*/
    i2cTransaction.arg = NULL;

    ret = I2CMSP432_transfer(handle, &i2cTransaction);
    if (!ret) {
        printf("i2c transfer fail %d \r\n", ret);
        return -1;
    }
    
    return 0;
}



static int32_t hal_i2c_master_close(i2c_dev_t *i2c)
{
    I2C_Handle handle = NULL;
    
    if (i2c->port >= I2C_count) {
        printf("i2c slave recv fail for invalid i2c port %d \r\n", i2c->port);
        return -1;
    }
    handle = (I2C_Handle)&(I2C_config[i2c->port]);
    
    I2CMSP432_close(handle);
    i2c->priv = NULL;
    return 0;
}

static int32_t hal_i2c_slave_close(i2c_dev_t *i2c)
{
    I2CSlave_Handle handle = NULL;
    
    if (i2c->port >= I2CSlave_count) {
        printf("i2c slave recv fail for invalid i2c port %d \r\n", i2c->port);
        return -1;
    }
    handle = (I2CSlave_Handle)&(I2CSlave_config[i2c->port]);
    
    I2CSlaveMSP432_close(handle);
    i2c->priv = NULL;
    return 0;

}


int32_t hal_i2c_finalize(i2c_dev_t *i2c)
{
    int32_t ret = -1;

    if (i2c == NULL) {
        return -1;
    }

    if (i2c->config.mode == I2C_MODE_MASTER) {
        ret = hal_i2c_master_close(i2c);
    } else if (i2c->config.mode == I2C_MODE_SLAVE) {
        ret = hal_i2c_slave_close(i2c);
    } else {
        printf("i2c init fail for invalid i2c mode %d \r\n", i2c->config.mode);
        return -1;
    }

    if (ret) {
        printf("i2c init fail in %d mode \r\n", i2c->config.mode);
    }
	return ret;
}


