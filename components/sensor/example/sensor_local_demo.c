/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "aos/cli.h"
#include "aos/kernel.h"
#include "sensor/sensor.h"
#include <vfsdev/i2c_dev.h>

#define SENSOR_SAMPLE_TIME 1000 /* sensor sampling period is 1000 ms */

void sensor_local_test()
{
    int ret;
    temperature_data_t temp;
    humidity_data_t humi;

    /* Sensor Hal start */
    ret = sensor_hal_init();
    if (ret != 0) {
        return;
    }
    // printf("\n enter %s : %d\n", __func__, __LINE__);
    /* Open the acceleration sensor device */
    ret = sensor_hal_open(TAG_DEV_TEMP, 0);
    if (ret != 0) {
        return;
    }

    ret = sensor_hal_open(TAG_DEV_HUMI, 0);
    if (ret != 0) {
        return;
    }

    // printf("\n enter %s : %d\n", __func__, __LINE__);
    /* Set the sampling period for sensors */
    //(void)sensor_hal_ioctl(TAG_DEV_TEMP, 0, SENSOR_IOCTL_ODR_SET, SENSOR_SAMPLE_TIME);

    while(1) {
        // printf("\n v1.4 enter %s : %d\n", __func__, __LINE__);
        /* Read the temperature sensor data */
        ret = sensor_hal_read(TAG_DEV_TEMP, 0, &temp, sizeof(temp));
        if(ret > 0){
            printf("Temperature value : %.1f centidegree\r\n", ((float)temp.t)/10);
        }

        ret = sensor_hal_read(TAG_DEV_HUMI, 0, &humi, sizeof(humi));
        if(ret > 0){
            printf("Humidity value : %.1f H\r\n", ((float)humi.h)/10);
        } else {
            printf("read Humidity fail , ret = %d\r\n", ret);
        }

        // printf("\n enter %s : %d\n", __func__, __LINE__);
        /* Task sleep */
        aos_msleep(1000);
    }

}

extern int32_t sensor_i2c_init(i2c_dev_t *i2c);
void i2c_init(void)
{
    i2c_dev_t i2c_dev;
    i2c_dev.port                 = 1;
    i2c_dev.config.address_width = I2C_HAL_ADDRESS_WIDTH_7BIT;
    i2c_dev.config.freq          = I2C_BUS_BIT_RATES_100K;
    i2c_dev.config.mode          = I2C_MODE_MASTER;
    //i2c_dev.config.dev_addr      = SI7006_I2C_ADDRESS;

    // hal_i2c_init(&i2c_dev);
    sensor_i2c_init(&i2c_dev);
}


static void sensor_local_demo(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    i2c_init();
    /* Sensor local test sample */
    sensor_local_test();

    return 0;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(sensor_local_demo, sensor_local_test, sensor local base example)
#endif


