/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <aos/aos.h>
#include <aos/yloop.h>
#include "netmgr.h"
#include "aos/udata.h"
#include "udata_queue.h"
#define AOS_SENSOR_HUMI_SENSIRION_HTS221

#define UDATA_PRINT    printf

#define UDATA_SHOW_UINT_1(TYPE,TIME,DATA1) \
do{\
    UDATA_PRINT("uData_application::::::::::::::type = (%d)\n", (TYPE)); \
    UDATA_PRINT("uData_application::::::::::::::data = (%d)\n", (DATA1)); \
    UDATA_PRINT("uData_application:::::::::timestamp = (%d)\n", (uint32_t)(TIME)); \
}while(0);

#define UDATA_SHOW_UINT_3(TYPE,TIME,DATA1,DATA2,DATA3) \
do{\
    UDATA_PRINT("uData_application::::::::::::::type = (%d)\n", (TYPE)); \
    UDATA_PRINT("uData_application::::::::::::::data = (%d) (%d) (%d)\n", (DATA1),(DATA2),(DATA3)); \
    UDATA_PRINT("uData_application:::::::::timestamp = (%d)\n", (uint32_t)(TIME)); \
}while(0);

#define UDATA_SHOW_FLOAT_3(TYPE,TIME,DATA1,DATA2,DATA3) \
do{\
    UDATA_PRINT("uData_application::::::::::::::type = (%d)\n", (TYPE)); \
    UDATA_PRINT("uData_application::::::::::::::data = (%f) (%f) (%f)\n", (DATA1),(DATA2),(DATA3)); \
    UDATA_PRINT("uData_application:::::::::timestamp = (%d)\n", (uint32_t)(TIME)); \
}while(0);


int uData_local_publish(udata_type_e type, void* pdata, uint32_t len)
{

    if(pdata == NULL){
        return -1;
    }

    switch (type) {
    
         case UDATA_SERVICE_PEDOMETER: {
             accel_data_t *acc = (accel_data_t *)pdata;
             UDATA_SHOW_UINT_3(type, (uint32_t)acc->timestamp, acc->data[0], acc->data[1], acc->data[2]);
             break;
         }
    
    
         case UDATA_SERVICE_MAG: {
             mag_data_t *mag = (mag_data_t *)pdata;
             UDATA_SHOW_UINT_3(type, (uint32_t)mag->timestamp, mag->data[0], mag->data[1], mag->data[2]);
             break;
         }
    
         case UDATA_SERVICE_GYRO: {
             gyro_data_t *gyro = (gyro_data_t *)pdata;
             UDATA_SHOW_UINT_3(type, (uint32_t)gyro->timestamp, gyro->data[0], gyro->data[1], gyro->data[2]);
             break;
         }
    
         case UDATA_SERVICE_ALS: {
             als_data_t *als = (als_data_t *)pdata;
             UDATA_SHOW_UINT_1(type, als->timestamp, als->lux);
             break;
         }
    
         case UDATA_SERVICE_PS: {
             proximity_data_t *ps = (proximity_data_t *)pdata;
             UDATA_SHOW_UINT_1(type, ps->timestamp, ps->present);
             break;
         }
    
         case UDATA_SERVICE_BARO: {
             barometer_data_t *baro = (barometer_data_t *)pdata;
             UDATA_SHOW_UINT_1(type, baro->timestamp, baro->p);
             break;
         }
    
         case UDATA_SERVICE_TEMP: {
             temperature_data_t *temp = (temperature_data_t *)pdata;
             UDATA_SHOW_UINT_1(type, temp->timestamp, temp->t);
             break;
         }
    
         case UDATA_SERVICE_UV: {
             uv_data_t *uv = (uv_data_t *)pdata;
             UDATA_SHOW_UINT_1(type, uv->timestamp, uv->uvi);
             break;
         }
    
         case UDATA_SERVICE_HUMI: {
             humidity_data_t *humi = (humidity_data_t *)pdata;
             UDATA_SHOW_UINT_1(type, humi->timestamp, humi->h);
             break;
         }
    
         case UDATA_SERVICE_HALL: {
             hall_data_t *hall = (hall_data_t *)pdata;
             UDATA_SHOW_UINT_1(type, hall->timestamp, hall->hall_level);
             break;
         }
         case UDATA_SERVICE_HR: {
             heart_rate_data_t *heart = (heart_rate_data_t *)pdata;
             UDATA_SHOW_UINT_1(type, heart->timestamp, heart->hear_rate);
             break;
         }
    
         case UDATA_SERVICE_GPS: {
             gps_data_t *gps = (gps_data_t *)pdata;
             UDATA_SHOW_FLOAT_3(type, gps->timestamp, gps->lat, gps->lon, gps->elv);
             break;
         }
    
         default:
             return -1;
     }

    return 0;

}

void uData_report_demo(sensor_msg_pkg_t *msg)
{
    int ret = 0;
    udata_pkg_t buf;
    if ((msg == NULL)) {
        return;
    }

    if (msg->cmd == UDATA_MSG_REPORT_PUBLISH) {
        ret = uData_report_publish(msg->value, &buf);
        if (ret != 0) {
            return;
        }
        
        ret = uData_local_publish(buf.type,buf.payload,0);
        if (ret != 0) {
            return;
        }

    }
}

int udata_sample(void)
{
    int ret = 0;
    ret = uData_register_msg_handler(uData_report_demo);
    if (ret == -1) {
        LOG("%s %s %s %d\n", uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return ret;
    }

    ret = uData_subscribe(UDATA_SERVICE_HUMI);
    if (ret != 0) {
        LOG("%s %s %s %d\n", uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }

    return 0;
}

int application_start(int argc, char **argv)
{
    int ret;
    ret = uData_main();
    if (unlikely(ret)) {
        return -1;
    }

    LOG("application hts221_i2c test started.");

    udata_sample();

    aos_loop_run();

    return 0;
}
