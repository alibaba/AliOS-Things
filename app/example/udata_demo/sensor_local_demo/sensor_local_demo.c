/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "aos/cli.h"
#include "aos/kernel.h"
#include "sensor/sensor.h"

/* sensor sampling period is 1000 ms*/
#define SENSOR_SAMPLE_TIME      (1000)
#define SENSOR_PRINT            printf

void sensor_local_test()
{
    int ret;
    temperature_data_t temp;
    accel_data_t acc;

    ret = sensor_hal_init();
    if (unlikely(ret)){
        return;
    }

    ret = sensor_hal_open(TAG_DEV_ACC, 0);
    if (unlikely(ret)){
        return;
    }
    ret = sensor_hal_open(TAG_DEV_TEMP, 0);
    if (unlikely(ret)){
        return;
    }

    (void)sensor_hal_ioctl(TAG_DEV_ACC, 0, SENSOR_IOCTL_ODR_SET, SENSOR_SAMPLE_TIME);
    (void)sensor_hal_ioctl(TAG_DEV_TEMP, 0, SENSOR_IOCTL_ODR_SET, SENSOR_SAMPLE_TIME);

    while(1){
        ret = sensor_hal_read(TAG_DEV_ACC, 0, &acc, sizeof(acc));
        if(ret > 0){
             SENSOR_PRINT("\nAcceleration value: x-axis(%.3f g) y-axis(%.3f g) z-axis(%.3f g) \n",((float)acc.data[0])/1000 ,((float)acc.data[1])/1000, ((float)acc.data[2])/1000);
        }

        ret = sensor_hal_read(TAG_DEV_TEMP, 0, &temp, sizeof(temp));
        if(ret > 0){
            SENSOR_PRINT("\nTemperature value : %.1f centidegree\n",((float)temp.t)/10);
        }

        aos_msleep(1000);
    }

}

int application_start(int argc, char **argv)
{

    sensor_local_test();

    return 0;
}


