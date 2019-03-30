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
#include "app_entry.h"

/* sensor sampling period is 1000 ms*/
#define SENSOR_SAMPLE_TIME    (1000)
#define SENSOR_PRINT          printf

#define PROP_POST_FORMAT_TEMP         "{\"CurrentTemperature\":%.1f}"
#define PROP_POST_FORMAT_ACC          "{\"Accelerometer\":{\"x\":%.2f, \"y\":%.2f, \"z\":%.2f}}"

extern int app_cloud_report(void* pdata, uint32_t len);

void sensor_data_cloud_entry(void)
{
    int ret;
    temperature_data_t temp;
    accel_data_t acc;
    char param[128];

    ret = sensor_hal_read(TAG_DEV_ACC, 0, &acc, sizeof(acc));
    if(ret > 0){
        SENSOR_PRINT("\nAcceleration value: x-axis(%.3f g) y-axis(%.3f g) z-axis(%.3f g) \n",((float)acc.data[0])/1000 ,((float)acc.data[1])/1000, ((float)acc.data[2])/1000);

        memset(param,0,128);
        sprintf(param, PROP_POST_FORMAT_ACC, ((float)acc.data[0])/1000 ,((float)acc.data[1])/1000, ((float)acc.data[2])/1000);

        ret = app_cloud_report(param,strlen(param) + 1);
        if(unlikely(ret)){
            SENSOR_PRINT("%s errline %d\n", __func__,__LINE__);
        }
    }

    ret = sensor_hal_read(TAG_DEV_TEMP, 0, &temp, sizeof(temp));
    if(ret > 0){
        SENSOR_PRINT("\nTemperature value : %.1f centidegree\n",((float)temp.t)/10);

        memset(param,0,128);
        sprintf(param, PROP_POST_FORMAT_TEMP, ((float)(temp.t))/10);

        ret = app_cloud_report(param,strlen(param) + 1);
        if(unlikely(ret)){
            SENSOR_PRINT("%s errline %d\n", __func__,__LINE__);
        }
    }
}

void sensor_data_cloud_test(void)
{
    int ret;

    linkkit_sample_start();

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

    aos_loop_run();
}

int application_start(int argc, char **argv)
{
    sensor_data_cloud_test();

    return 0;
}


