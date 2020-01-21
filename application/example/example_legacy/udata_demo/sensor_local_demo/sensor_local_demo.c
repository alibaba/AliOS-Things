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

#define SENSOR_SAMPLE_TIME 1000 /* sensor sampling period is 1000 ms */

void sensor_local_test()
{
    int ret;
    temperature_data_t temp;
    accel_data_t       acc;

    /* Sensor Hal start */
    ret = sensor_hal_init();
    if (ret != 0) {
        return;
    }

    /* Open the acceleration sensor device */
    ret = sensor_hal_open(TAG_DEV_ACC, 0);
    if (ret != 0) {
        return;
    }
    /* Open the temperature sensor device */
    ret = sensor_hal_open(TAG_DEV_TEMP, 0);
    if (ret != 0) {
        return;
    }

    /* Set the sampling period for sensors */
    (void)sensor_hal_ioctl(TAG_DEV_ACC, 0, SENSOR_IOCTL_ODR_SET, SENSOR_SAMPLE_TIME);
    (void)sensor_hal_ioctl(TAG_DEV_TEMP, 0, SENSOR_IOCTL_ODR_SET, SENSOR_SAMPLE_TIME);

    while(1) {

        /* Read the acceleration sensor data */
        ret = sensor_hal_read(TAG_DEV_ACC, 0, &acc, sizeof(acc));
        if(ret > 0){
            printf("\nAcceleration value: x-axis(%.3f g) y-axis(%.3f g) z-axis(%.3f g) \n", ((float)acc.data[0])/1000 , ((float)acc.data[1])/1000, ((float)acc.data[2])/1000);
        }

        /* Read the temperature sensor data */
        ret = sensor_hal_read(TAG_DEV_TEMP, 0, &temp, sizeof(temp));
        if(ret > 0){
            printf("\nTemperature value : %.1f centidegree\n", ((float)temp.t)/10);
        }

        /* Task sleep */
        aos_msleep(1000);
    }

}

int application_start(int argc, char **argv)
{
    /* Sensor local test sample */
    sensor_local_test();

    return 0;
}


