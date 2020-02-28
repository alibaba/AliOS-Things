/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "aos/hal/gpio.h"
#include "aos/hal/i2c.h"
#include "board.h"
#include <k_api.h>

#if (AOS_HAL_I2C_ENABLED > 0)
#ifndef HALAPP_I2C_TEST
#define HALAPP_I2C_TEST             PORT_I2C_1
#endif

#define HAL_AT24CXX_DEV_WRITE_ADDR  0XA0
#define HAL_AT24CXX_DEV_READ_ADDR   0XA1
#define HAL_AT24CXX_TEST_MEM_ADDR   0

static char *halapp_i2c_test_data = "halapp_i2c test OK!";
static i2c_dev_t i2c_dev = {0};

/* toggle gpio_out periodically in timer handler */
void hal_i2c_app_run(void)
{
    int32_t ret;
	int i;
    uint16_t size = strlen(halapp_i2c_test_data) + 1;
    char data_in[40] = {0};

    printf("hal_i2c_app_run in\r\n");
    i2c_dev.port = HALAPP_I2C_TEST;
    i2c_dev.config.address_width = I2C_HAL_ADDRESS_WIDTH_7BIT;
    i2c_dev.config.freq = I2C_BUS_BIT_RATES_400K;
    i2c_dev.config.mode = I2C_MODE_MASTER;
    ret = hal_i2c_init(&i2c_dev);

    if(ret){
        printf("hal_i2c_app_run: hal_i2c_init error!\r\n");
        return;
    }
	for(i = 0; i < size; i++){
		ret = hal_i2c_mem_write(&i2c_dev, HAL_AT24CXX_DEV_WRITE_ADDR, HAL_AT24CXX_TEST_MEM_ADDR + i,
                              I2C_MEMADD_SIZE_8BIT, halapp_i2c_test_data + i, 1,50);
		if(ret){
			printf("hal_i2c_app_run: hal_i2c_mem_write error! ret:%d\r\n",ret);
			return;
		}
		aos_msleep(50);	
	}

	aos_msleep(50);

    ret = hal_i2c_mem_read(&i2c_dev, HAL_AT24CXX_DEV_READ_ADDR, HAL_AT24CXX_TEST_MEM_ADDR,
                             I2C_MEMADD_SIZE_8BIT, data_in, size, 50);
    if(ret){
        printf("hal_i2c_app_run: hal_i2c_mem_read error! ret:%d\r\n",ret);
        return;
    }
	for(i = 0; i < size; i++){
        printf("%c",data_in[i]);	
	}
	
    ret = memcmp(data_in,halapp_i2c_test_data,size);
    if(ret){
        printf("hal_i2c_app_run: read data not equal to write data!\r\n");
        return;
    }

    printf("hal_i2c_app_run end\r\n");
}

#endif
