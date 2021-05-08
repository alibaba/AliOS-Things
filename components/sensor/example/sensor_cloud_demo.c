/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "aos/kernel.h"
#include "sensor/sensor.h"
#include "ulog/ulog.h"
#include "netmgr.h"
#include <uservice/uservice.h>
#include <uservice/eventid.h>

#define SENSOR_SAMPLE_TIME 1000 /* sensor sampling period is 1000 ms*/
#define PROP_POST_FORMAT_TEMP "{\"CurrentTemperature\":%.1f}"
#define PROP_POST_FORMAT_HUMI  "{\"Humidity\":%.1f}"

void sensor_cloud_test_init(void)
{
    int  ret = 0;

    i2c_dev_t i2c_dev;
    i2c_dev.port                 = 1;
    i2c_dev.config.address_width = I2C_HAL_ADDRESS_WIDTH_7BIT;
    i2c_dev.config.freq          = I2C_BUS_BIT_RATES_100K;
    i2c_dev.config.mode          = I2C_MODE_MASTER;
    sensor_i2c_init(&i2c_dev);

    /* Sensor Hal start */
    ret = sensor_hal_init();
    if (ret != 0) {
        return;
    }

    /* Open the Humidity sensor device */
    ret = sensor_hal_open(TAG_DEV_HUMI, 0);
    if (ret != 0) {
        //return;
    }

    /* Open the temperature sensor device */
    ret = sensor_hal_open(TAG_DEV_TEMP, 0);
    if (ret != 0) {
        //return;
    }

    /* Set the sampling period for sensors */
    sensor_hal_ioctl(TAG_DEV_HUMI, 0, SENSOR_IOCTL_ODR_SET, SENSOR_SAMPLE_TIME);
    sensor_hal_ioctl(TAG_DEV_TEMP, 0, SENSOR_IOCTL_ODR_SET, SENSOR_SAMPLE_TIME);
}

void sensor_cloud_test_start(void *dm_handle)
{
    int  ret = 0;
    char param[128];
    temperature_data_t  temp;
    humidity_data_t     humi;

    /* Read the Humidity sensor data */
    ret = sensor_hal_read(TAG_DEV_HUMI, 0, &humi, sizeof(humi));
    if (ret > 0) {
        /* Print the Humidity sensor data */
        printf("Humidity value : %.1f H\n", ((float)humi.h)/10);

        memset(param, 0, 128);

        /* build the report payload */
        sprintf(param, PROP_POST_FORMAT_HUMI, ((float)humi.h)/10);

        /* Report the Humidity data to cloud */
        ret = demo_send_property_post(dm_handle, (char *)param);
        if (ret == -1) {
            LOG("%s %d fail\n", __func__,__LINE__);
        }
    }

    /* Read the temperature sensor data */
    ret = sensor_hal_read(TAG_DEV_TEMP, 0, &temp, sizeof(temp));
    if (ret > 0) {
        /* Print the temperature sensor data */
        printf("Temperature value : %.1f centidegree\n", ((float)temp.t)/10);

        memset(param, 0, 128);

        /* build the report payload */
        sprintf(param, PROP_POST_FORMAT_TEMP, ((float)(temp.t))/10);

        /* Report the temperature data to cloud */
        ret = demo_send_property_post(dm_handle, (char *)param);
        if (ret == -1) {
            LOG("%s %d fail\n", __func__,__LINE__);
        }
    }
}
