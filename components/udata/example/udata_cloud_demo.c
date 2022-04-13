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
#include "sensor_hal.h"
#include "sensor_drv_api.h"
#include "udata/udata.h"
#include "ulog/ulog.h"
#include "netmgr.h"
#include <uservice/uservice.h>
#include <uservice/eventid.h>

#define SENSOR_SAMPLE_TIME 1000 /* sensor sampling period is 1000 ms*/

#define PROP_POST_FORMAT_TEMP "{\"CurrentTemperature\":%.1f}"
#define PROP_POST_FORMAT_HUMI  "{\"Humidity\":%.1f}"

extern void *g_dm_handle;

extern int32_t demo_send_property_post(void *dm_handle, char *params);
/* uData sensor data read and process callback
when sensor data update this function will be called */
void udata_report_demo(sensor_msg_pkg_t *msg)
{
    int     ret = 0;
    char    param[128];
    udata_pkg_t         buf;
    humidity_data_t     *humi;
    temperature_data_t  *temp;

    if ((msg == NULL || g_dm_handle == NULL)) {
        printf("invalid parameter\r\n");
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

                /* Print the humidity sensor data */
                humi = (humidity_data_t *)(buf.payload);
                printf("Humidity value : %.1f H\n", ((float)humi->h) / 10);

                memset(param, 0, 128);
                /* build the report payload */
                sprintf(param, PROP_POST_FORMAT_HUMI, ((float)humi->h) / 10);

                /* Report the Humidity data to cloud */
                ret = demo_send_property_post(g_dm_handle, (char *)param);
                if (ret == -1) {
                    LOG("%s %d fail\n", __func__, __LINE__);
                }

                break;
            }

            case UDATA_SERVICE_TEMP: {
                /* Print the temperature sensor data */
                temp = (temperature_data_t *)(buf.payload);
                printf("\nTemperature value : %.1f centidegree\n", ((float)temp->t) / 10);

                memset(param, 0, 128);

                /* build the report payload */
                sprintf(param, PROP_POST_FORMAT_TEMP, ((float)(temp->t)) / 10);

                /* Report the temperature data to cloud */
                ret = demo_send_property_post(g_dm_handle, (char *)param);
                if (ret == -1) {
                    LOG("%s %d\n", __func__, __LINE__);
                }

                break;
            }

            default:
                break;
        }

    }
}

void udata_cloud_test_init(void)
{
    int  ret = 0;

    i2c_dev_t i2c_dev;
    i2c_dev.port                 = 1;
    i2c_dev.config.address_width = I2C_HAL_ADDRESS_WIDTH_7BIT;
    i2c_dev.config.freq          = I2C_BUS_BIT_RATES_100K;
    i2c_dev.config.mode          = I2C_MODE_MASTER;
    sensor_i2c_init(&i2c_dev);

    ret = sensor_init();
    if (ret != 0) {
        printf("sensor_init fail %s %d\n\r", __func__, __LINE__);
        return;
    }

    /* uData start */
    udata_init();

    /* Register msg handler to receive sensor data */
    ret = udata_register_msg_handler(udata_report_demo);
    if (ret < 0) {
        LOG("%s %d\n", __func__, __LINE__);
        return;
    }

    /* Subscribe humidity sensor service */
    ret = udata_subscribe(UDATA_SERVICE_HUMI);
    if (ret != 0) {
        LOG("%s %d\n", __func__, __LINE__);
    }

    /* Subscribe temperature sensor service */
    ret = udata_subscribe(UDATA_SERVICE_TEMP);
    if (ret != 0) {
        LOG("%s %d\n", __func__, __LINE__);
    }
}
