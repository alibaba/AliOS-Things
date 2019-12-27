/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "cli.h"
#include "aos/kernel.h"
#include "sensor/sensor.h"
#include "udata/udata.h"
#include "ulog/ulog.h"

void udata_report_demo(sensor_msg_pkg_t *msg)
{
    int         ret;
    udata_pkg_t buf;

    accel_data_t       *acc;
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
             case UDATA_SERVICE_ACC: {
                /* Print the acceleration sensor data */
                acc = (accel_data_t *)(buf.payload);
                printf("\nAcceleration value: x-axis(%.3f g) y-axis(%.3f g) z-axis(%.3f g) \n", ((float)acc->data[0])/1000, ((float)acc->data[1])/1000, ((float)acc->data[2])/1000);
                break;
             }
             case UDATA_SERVICE_TEMP: {
                /* Print the temperature sensor data */
                temp = (temperature_data_t *)(buf.payload);
                printf("\nTemperature value : %.1f centidegree\n", ((float)temp->t)/10.0);
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

    /* Subscribe acceleration sensor service */
    ret = udata_subscribe(UDATA_SERVICE_ACC);
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

int application_start(int argc, char **argv)
{
    int ret;

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


