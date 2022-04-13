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
#include "udata/udata.h"
#include "ulog/ulog.h"
#include "sensor_hal.h"
#include "sensor_drv_api.h"

void udata_report_demo(sensor_msg_pkg_t *msg)
{
    int         ret;
    udata_pkg_t buf;

    humidity_data_t     *humi;
    temperature_data_t *temp;

    if (msg == NULL) {
        return;
    }

    if (msg->cmd == UDATA_MSG_REPORT_PUBLISH) {
        /* Read the sensor data */
        ret = udata_report_publish(msg->value, &buf);
        if (ret != 0) {
            return;
        }

        /* Process the sensor data */
        switch (buf.type) {
             case UDATA_SERVICE_HUMI: {
                /* Print the acceleration sensor data */
                humi = (humidity_data_t *)(buf.payload);
                printf("Humidity value : %.1f H\n", ((float)humi->h)/10);
                break;
             }
             case UDATA_SERVICE_TEMP: {
                /* Print the temperature sensor data */
                temp = (temperature_data_t *)(buf.payload);
                printf("Temperature value : %.1f centidegree\n", ((float)temp->t)/10.0);
                break;
             }

             default:
                break;
         }

    }
}

int udata_local_test(void)
{
    int ret = 0;

    /* uData start */
    (void)udata_init();

    /* Register msg handler to receive sensor data */
    ret = udata_register_msg_handler(udata_report_demo);
    if (ret < 0) {
        LOG("%s errline %d\n", __func__, __LINE__);
        return ret;
    }

    /* Subscribe Humidity sensor service */
    ret = udata_subscribe(UDATA_SERVICE_HUMI);
    if (ret != 0) {
        LOG("%s errline %d\n", __func__, __LINE__);
    }

    /* Subscribe temperature sensor service */
    ret = udata_subscribe(UDATA_SERVICE_TEMP);
    if (ret != 0) {
        LOG("%s errline %d\n", __func__, __LINE__);
    }

    return 0;
}

static int udata_local_demo(int argc, char **argv)
{
    int ret;
    i2c_dev_t i2c_dev;

    i2c_dev.port                 = 1;
    i2c_dev.config.address_width = I2C_HAL_ADDRESS_WIDTH_7BIT;
    i2c_dev.config.freq          = I2C_BUS_BIT_RATES_100K;
    i2c_dev.config.mode          = I2C_MODE_MASTER;
    sensor_i2c_init(&i2c_dev);


    ret = sensor_init();
    if (ret != 0) {
        printf("sensor_init fail %s %d\n\r", __func__,__LINE__);
        return -1;
    }

    /* uData local test sample */
    ret = udata_local_test();
    if(ret != 0){
        return -1;
    }

    while(1) {
        aos_msleep(1000);
    }
    return 0;
}

#ifdef AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(udata_local_demo, udata_local_test, udata temp base example)
#endif
