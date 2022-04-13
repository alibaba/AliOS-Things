/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "amp_hal_i2c.h"
#include "drv_i2c.h"
#include "osi_log.h"

#define DRV_NAME_I2C1 OSI_MAKE_TAG('I', '2', 'C', '1')
#define DRV_NAME_I2C2 OSI_MAKE_TAG('I', '2', 'C', '2')
#define DRV_NAME_I2C3 OSI_MAKE_TAG('I', '2', 'C', '3')
static drvI2cMaster_t *i2c_p;


#define HAL_WAIT_FOREVER 0xFFFFFFFFU

#define I2C_MODE_MASTER 1 /* i2c communication is master mode */
#define I2C_MODE_SLAVE  2 /* i2c communication is slave mode */
int32_t amp_hal_i2c_init(i2c_dev_t *i2c)
{
    int ret = -1;
    drvI2cBps_t i2c_bps;

    if(i2c->config.freq == I2C_BUS_BIT_RATES_100K)
    {
        i2c_bps = DRV_I2C_BPS_100K;
    }
    else if(i2c->config.freq == I2C_BUS_BIT_RATES_400K)
    {
        i2c_bps = DRV_I2C_BPS_400K;
    }
    else
    {
        OSI_PRINTFI("i2c open fail,bps error!");
        return ret;
    }
    OSI_PRINTFI("i2c open name:%d", i2c->port);
    if(i2c->port == 1)
    {
        i2c_p = drvI2cMasterAcquire(DRV_NAME_I2C1, i2c_bps);
    }
    else if(i2c->port == 2)
    {
        i2c_p = drvI2cMasterAcquire(DRV_NAME_I2C2, i2c_bps);
    }
    else if(i2c->port == 3)
    {
        i2c_p = drvI2cMasterAcquire(DRV_NAME_I2C3, i2c_bps);
    }
    else
    {
        OSI_PRINTFI("i2c open fail,name error!");
        return ret;
    }
    if (i2c_p == NULL)
    {
        OSI_PRINTFI("i2c open fail");
        return ret;
    }
    OSI_PRINTFI("i2c open success:%p",i2c_p);
    ret = 0;

    return ret;

}

int32_t amp_hal_i2c_master_send(i2c_dev_t *i2c, uint16_t dev_addr, const uint8_t *data,
                            uint16_t size, uint32_t timeout)
{
    int ret = -1;
    drvI2cSlave_t idAddress;
    uint8_t wrtBuff[size-1], i;
    uint8_t datalen=0;
    int addWidth = 0;
	
    if(data == NULL || i2c == NULL)
    {
        OSI_PRINTFI("i2c is NULL or data is NULL!\n");
	 return ret;    	 
    }
    if (i2c_p == NULL)
    {
        OSI_PRINTFI("i2c device not open");
	 return ret;
    }
	
    idAddress.addr_device = dev_addr;	
    if(i2c->config.address_width == I2C_HAL_ADDRESS_WIDTH_7BIT)
    {
        addWidth = I2C_MEM_ADDR_SIZE_8BIT;
    }
    else
    {
    	addWidth = I2C_MEM_ADDR_SIZE_16BIT;
    }
    OSI_PRINTFI("hal_i2c_master_send i2c->port:%d addWidth:%d\n",i2c->port, addWidth);
    if(i2c->config.address_width == I2C_MEM_ADDR_SIZE_8BIT)
    {
    	idAddress.is16_bit	= false;
       idAddress.addr_data = data[0];
	for(i=0; i < size-1; i++)
	{
		wrtBuff[i] = data[i+1];
	}
	datalen = size-1;
    }
    else if(i2c->config.address_width == I2C_MEM_ADDR_SIZE_16BIT)
    {
    	idAddress.is16_bit = true;
	memcpy((char*)(&idAddress.addr_data), data, 2);
	for(i=0; i < size-2; i++)
	{
		wrtBuff[i] = data[i+2];
	}
	datalen = size-2;
    }
		
 //   idAddress = {dev_addr, memAddr,is16bit};

    ret = drvI2cWrite(i2c_p, &idAddress, wrtBuff, datalen);
    OSI_PRINTFI("i2c_send_data:%d,ret:%d", datalen, ret);
    if(!ret)
    {
        ret = -1;
    }
    else
    {
    	ret = 0;
    }
    return ret;
}

int32_t amp_hal_i2c_master_recv(i2c_dev_t *i2c, uint16_t dev_addr, uint8_t *data,
                            uint16_t size, uint32_t timeout)
{

    int ret = -1;
    drvI2cSlave_t idAddress;
    int addWidth = 0;
    uint8_t readBuff[size-1];
    uint8_t datalen=0;
    
    if(data == NULL || i2c == NULL)
    {
        OSI_PRINTFI("i2c is NULL or data is NULL!\n");
	 return ret;    	 
    }
    if (i2c_p == NULL)
    {
        OSI_PRINTFI("i2c device not open");
	 return ret;
    }
	
    idAddress.addr_device = dev_addr;
    if(i2c->config.address_width == I2C_HAL_ADDRESS_WIDTH_7BIT)
    {
        addWidth = I2C_MEM_ADDR_SIZE_8BIT;
    }
    else
    {
    	addWidth = I2C_MEM_ADDR_SIZE_16BIT;
    }
    OSI_PRINTFI("hal_i2c_master_recv i2c->port:%d addWidth:%d\n",i2c->port, addWidth);
	
    if(addWidth == I2C_MEM_ADDR_SIZE_8BIT)
    {
    	idAddress.is16_bit	= false;
       idAddress.addr_data = data[0];
	datalen = size-1;
    }
    else if(addWidth == I2C_MEM_ADDR_SIZE_16BIT)
    {
    	idAddress.is16_bit = true;
	memcpy((char*)(&idAddress.addr_data), data, 2);
	datalen = size-2;
    }
		
    //idAddress = {dev_addr, memAddr,is16bit};
    memset(readBuff, 0x00, sizeof(readBuff));
    ret = drvI2cRead(i2c_p, &idAddress, readBuff, datalen);
    if(!ret)
    {
        ret = -1;
        OSI_PRINTFI("i2c_read_data failed:%d,ret:%d", datalen, ret);
        return ret;
    }
    else
    {
        ret = 0;
    }
	
    if(i2c->config.address_width == I2C_MEM_ADDR_SIZE_8BIT)
    {
    	   memcpy(&(data[1]), readBuff, datalen);
    }
    else if(i2c->config.address_width == I2C_MEM_ADDR_SIZE_16BIT)
    {
    	   memcpy(&(data[2]), readBuff, datalen);
    }	
    return ret;
}

int32_t amp_hal_i2c_slave_send(i2c_dev_t *i2c, const uint8_t *data, uint16_t size, uint32_t timeout)
{
    return 0;
}

int32_t amp_hal_i2c_slave_recv(i2c_dev_t *i2c, uint8_t *data, uint16_t size, uint32_t timeout)
{
    return 0;
}

int32_t amp_hal_i2c_mem_write(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                          uint16_t mem_addr_size, const uint8_t *data, uint16_t size,
                          uint32_t timeout)
{

    int ret = -1;
    drvI2cSlave_t idAddress;
    uint8_t wrtBuff[size], i, readBuff[size];
    uint8_t datalen=0;	
    if(data == NULL || i2c == NULL)
    {
        OSI_PRINTFI("i2c is NULL or data is NULL!\n");
	 return ret;    	 
    }
    if (i2c_p == NULL)
    {
        OSI_PRINTFI("i2c device not open");
	 return ret;
    }
	
    idAddress.addr_device = dev_addr;	
    if(i2c->config.address_width == I2C_HAL_ADDRESS_WIDTH_7BIT)
    {
    	idAddress.is16_bit	= false;		
    }
    else
    {
    	idAddress.is16_bit = true;		
    }
    OSI_PRINTFI("hal_i2c_mem_write i2c->port:%d i2c->config.address_width:%d\n",i2c->port, i2c->config.address_width);
    OSI_PRINTFI("dev_addr:%d  mem_addr:%d  size:%d\n ",dev_addr, mem_addr, size);

    idAddress.addr_data = mem_addr;
    for(i=0; i < size; i++)
    {
        wrtBuff[i] = data[i];
        OSI_PRINTFI("data[%d]:%d\n ", i, wrtBuff[i]);
    }
    datalen = size;
		
 //   idAddress = {dev_addr, memAddr,is16bit};

    ret = drvI2cWrite(i2c_p, &idAddress, &wrtBuff[0], datalen);
    OSI_PRINTFI("i2c_send_data:%d,ret:%d", datalen, ret);
    if(!ret)
    {
        ret = -1;
    }
    else
    {
    	ret = 0;
    }
    return ret;
}

int32_t amp_hal_i2c_mem_read(i2c_dev_t *i2c, uint16_t dev_addr, uint16_t mem_addr,
                         uint16_t mem_addr_size, uint8_t *data, uint16_t size,
                         uint32_t timeout)
{
    int ret = -1;
    drvI2cSlave_t idAddress;
    uint8_t readBuff[size];
    uint8_t datalen=0;
    int i =0;
	
    if(data == NULL || i2c == NULL)
    {
        OSI_PRINTFI("i2c is NULL or data is NULL!\n");
	 return ret;    	 
    }
    if (i2c_p == NULL)
    {
        OSI_PRINTFI("i2c device not open");
	 return ret;
    }
	
    idAddress.addr_device = dev_addr;
    if(i2c->config.address_width == I2C_HAL_ADDRESS_WIDTH_7BIT)
    {
    	idAddress.is16_bit	= false;
    }
    else
    {
    	idAddress.is16_bit = true;
    }
    //OSI_PRINTFI("hal_i2c_mem_read i2c->port:%d i2c->config.address_width:%d\n",i2c->port, i2c->config.address_width);
    idAddress.addr_data = mem_addr;
    datalen = size;
    //OSI_PRINTFI("dev_addr:%d  mem_addr:%d  size:%d\n ",dev_addr, mem_addr, size);			
    //idAddress = {dev_addr, memAddr,is16bit};
    memset(readBuff, 0x00, sizeof(readBuff));
    ret = drvI2cRead(i2c_p, &idAddress, &readBuff[0], datalen);
    if(!ret)
    {
        ret = -1;
        OSI_PRINTFI("i2c_read_data failed:%d,ret:%d", datalen, ret);
        return ret;
    }
    else
    {
        ret = 0;
    }
    for(i=0; i < datalen; i++)
    {
        data[i] = readBuff[i] ;
    }
	
    return ret;
}

int32_t amp_hal_i2c_finalize(i2c_dev_t *i2c)
{
    int ret = -1;
    if (i2c_p == NULL)
    {
        OSI_PRINTFI("i2c open fail");
        return ret;
    }
    drvI2cMasterRelease(i2c_p);
    i2c_p = NULL;
    ret = 0;
    return ret;
}
