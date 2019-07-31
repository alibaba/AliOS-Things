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
#include "udata/udata.h"
#include "device.h"

#if defined(UDATA_CJSON_SUPPORTED) || defined(DTC_LINKKIT)
#include "cJSON.h"
#endif

#define GATEWAY_PRINT    printf

#define GATEWAY_SHOW_UINT_1(TYPE,TIME,DATA1) \
do{\
    GATEWAY_PRINT("gateway_application::::::::::::::type = (%d)\n", (int)(TYPE)); \
    GATEWAY_PRINT("gateway_application::::::::::::::data = (%d)\n", (int)(DATA1)); \
    GATEWAY_PRINT("gateway_application:::::::::timestamp = (%u)\n", (unsigned int)(TIME)); \
}while(0);

#define GATEWAY_SHOW_UINT_3(TYPE,TIME,DATA1,DATA2,DATA3) \
do{\
    GATEWAY_PRINT("gateway_application::::::::::::::type = (%d)\n", (int)(TYPE)); \
    GATEWAY_PRINT("gateway_application::::::::::::::data = (%d) (%d) (%d)\n", (int)(DATA1),(int)(DATA2),(int)(DATA3)); \
    GATEWAY_PRINT("gateway_application:::::::::timestamp = (%u)\n", (unsigned int)(TIME)); \
}while(0);

#define GATEWAY_SHOW_UINT_7(TYPE,TIME,DATA1,DATA2,DATA3,DATA4,DATA5,DATA6,DATA7) \
do{\
    GATEWAY_PRINT("gateway_application::::::::::::::type = (%d)\n", (int)(TYPE)); \
    GATEWAY_PRINT("gateway_application::::::::::::::data = (%d) (%d) (%d) (%d) (%d) (%d) (%d)\n", (int)(DATA1),(int)(DATA2),(int)(DATA3),(int)(DATA4),(int)(DATA5),(int)(DATA6),(int)(DATA7)); \
    GATEWAY_PRINT("gateway_application:::::::::timestamp = (%u)\n", (unsigned int)(TIME)); \
}while(0);

#define GATEWAY_SHOW_FLOAT_3(TYPE,TIME,DATA1,DATA2,DATA3) \
do{\
    GATEWAY_PRINT("gateway_application::::::::::::::type = (%d)\n", (int)(TYPE)); \
    GATEWAY_PRINT("gateway_application::::::::::::::data = (%f) (%f) (%f)\n", (DATA1),(DATA2),(DATA3)); \
    GATEWAY_PRINT("gateway_application:::::::::timestamp = (%u)\n", (unsigned int)(TIME)); \
}while(0);

#define DEFAULT_INTERVAL  (3000)

#if defined DTC_MQTT
extern int mqtt_sample_start(void);

extern int udata_mqtt_publish(udata_type_e type, void* pdata, uint32_t len);

#elif defined DTC_LINKKIT
extern int linkkit_sample_start(void);
extern int udata_linkkit_publish(udata_type_e type, void* pdata, uint32_t len);
#endif


int gateway_local_publish(udata_type_e type, void* pdata, uint32_t len)
{
    int     ret = 0;
    char    param[128] = { 0 };
    if(pdata == NULL){
        return -1;
    }

    switch (type) {

         case UDATA_SERVICE_ACC: {
             accel_data_t *acc = (accel_data_t *)pdata;
             GATEWAY_SHOW_UINT_3(type, (uint32_t)acc->timestamp, acc->data[0], acc->data[1], acc->data[2]);
             break;
         }


         case UDATA_SERVICE_MAG: {
             mag_data_t *mag = (mag_data_t *)pdata;
             GATEWAY_SHOW_UINT_3(type, (uint32_t)mag->timestamp, mag->data[0], mag->data[1], mag->data[2]);
             break;
         }

         case UDATA_SERVICE_GYRO: {
             gyro_data_t *gyro = (gyro_data_t *)pdata;
             GATEWAY_SHOW_UINT_3(type, (uint32_t)gyro->timestamp, gyro->data[0], gyro->data[1], gyro->data[2]);
             break;
         }

         case UDATA_SERVICE_ALS: {
             als_data_t *als = (als_data_t *)pdata;
             GATEWAY_SHOW_UINT_1(type, als->timestamp, als->lux);
             break;
         }

         case UDATA_SERVICE_PS: {
             proximity_data_t *ps = (proximity_data_t *)pdata;
             GATEWAY_SHOW_UINT_1(type, ps->timestamp, ps->present);
             break;
         }

         case UDATA_SERVICE_BARO: {
             barometer_data_t *baro = (barometer_data_t *)pdata;
             GATEWAY_SHOW_UINT_1(type, baro->timestamp, baro->p);
             break;
         }

         case UDATA_SERVICE_TEMP: {
             temperature_data_t *temp = (temperature_data_t *)pdata;
             GATEWAY_SHOW_UINT_1(type, temp->timestamp, temp->t);
             break;
         }

         case UDATA_SERVICE_UV: {
             uv_data_t *uv = (uv_data_t *)pdata;
             GATEWAY_SHOW_UINT_1(type, uv->timestamp, uv->uvi);
             break;
         }

         case UDATA_SERVICE_HUMI: {
             humidity_data_t *humi = (humidity_data_t *)pdata;
             GATEWAY_SHOW_UINT_1(type, humi->timestamp, humi->h);
             break;
         }

         case UDATA_SERVICE_HALL: {
             hall_data_t *hall = (hall_data_t *)pdata;
             GATEWAY_SHOW_UINT_1(type, hall->timestamp, hall->hall_level);
             break;
         }
         case UDATA_SERVICE_HR: {
             heart_rate_data_t *heart = (heart_rate_data_t *)pdata;
             GATEWAY_SHOW_UINT_1(type, heart->timestamp, heart->hear_rate);
             break;
         }

         case UDATA_SERVICE_GPS: {
             gps_data_t *gps = (gps_data_t *)pdata;
             GATEWAY_SHOW_FLOAT_3(type, gps->timestamp, gps->lat, gps->lon, gps->elv);
             break;
         }

         case UDATA_SERVICE_RTC: {
             rtc_data_t *rtc = (rtc_data_t *)pdata;
             GATEWAY_SHOW_UINT_7(type, rtc->timestamp, rtc->year, rtc->month, rtc->date, rtc->day, rtc->hours, rtc->minutes, rtc->seconds);
             break;
         }

         default:
             return -1;
     }

    return 0;

}

int gateway_cloud_publish(udata_type_e type, void* pdata, uint32_t len)
{
    int     ret = 0;
    char    param[128] = { 0 };
    if(pdata == NULL){
        return -1;
    }

    switch (type) {
         #ifdef PT_SENSOR
         case UDATA_SERVICE_ACC: {
             accel_data_t *acc = (accel_data_t *)pdata;
             ret = sprintf(param, PROP_POST_FORMAT_ACC, ((float)acc->data[0]/1000), ((float)acc->data[1]/1000), ((float)acc->data[2]/1000));
             break;
         }

         case UDATA_SERVICE_GYRO: {
             gyro_data_t *gyro = (gyro_data_t *)pdata;
             ret = sprintf(param, PROP_POST_FORMAT_GYRO, gyro->data[0]/1000, gyro->data[1]/1000, gyro->data[2]/1000);
             break;
         }

         case UDATA_SERVICE_TEMP: {
             temperature_data_t *temp = (temperature_data_t *)pdata;
             ret = sprintf(param, PROP_POST_FORMAT_TEMP, ((float)(temp->t))/10);
             break;
         }

         case UDATA_SERVICE_HUMI: {
             humidity_data_t *humi = (humidity_data_t *)pdata;
             ret = sprintf(param, PROP_POST_FORMAT_HUMI, ((float)(humi->h))/10);
             break;
         }

         case UDATA_SERVICE_RTC: {
             rtc_data_t *rtc = (rtc_data_t *)pdata;
             ret = sprintf(param, PROP_POST_FORMAT_RTC, rtc->year, rtc->month, rtc->date, rtc->day,
                           rtc->hours, rtc->minutes, rtc->seconds);
             break;
         }
         #endif
         default:
             return -1;
     }

    gateway_cloud_report(&param[0], strlen(param));
    return 0;

}

void gateway_report_demo(sensor_msg_pkg_t *msg)
{
    int ret = 0;
    udata_pkg_t buf;
    if ((msg == NULL)) {
        return;
    }

    if (msg->cmd == UDATA_MSG_REPORT_PUBLISH) {
        ret = udata_report_publish(msg->value, &buf);
        if (ret != 0) {
            return;
        }

        ret = gateway_local_publish(buf.type,buf.payload,0);
        if (ret != 0) {
            return;
        }

        ret = gateway_cloud_publish(buf.type,buf.payload,0);
        if (ret != 0) {
            return;
        }
    }
}

int gateway_sample(void)
{
    int ret = 0;
    ret = udata_register_msg_handler(gateway_report_demo);
    if (ret < 0) {
        LOG("%s %s %d\n", __func__, ERROR_LINE, __LINE__);
        return ret;
    }
    #ifdef PT_SENSOR
    ret = udata_subscribe(UDATA_SERVICE_ACC);
    if (unlikely(ret != 0)) {
        LOG("%s %s %d\n", __func__, ERROR_LINE, __LINE__);
    }

    ret = udata_subscribe(UDATA_SERVICE_GYRO);
    if (unlikely(ret != 0)) {
        LOG("%s %s %d\n", __func__, ERROR_LINE, __LINE__);
    }

    ret = udata_subscribe(UDATA_SERVICE_HUMI);
    if (unlikely(ret != 0)) {
        LOG("%s %s %d\n", __func__, ERROR_LINE, __LINE__);
    }

    ret = udata_subscribe(UDATA_SERVICE_TEMP);
    if (unlikely(ret != 0)) {
        LOG("%s %s %d\n", __func__, ERROR_LINE, __LINE__);
    }

    ret = udata_subscribe(UDATA_SERVICE_RTC);
    if (unlikely(ret != 0)) {
        LOG("%s %s %d\n", __func__, ERROR_LINE, __LINE__);
    }
    #endif
    return 0;
}


int application_start(int argc, char **argv)
{
    int ret;
#if defined(UDATA_CJSON_SUPPORTED) || defined(DTC_LINKKIT)
    cJSON_Hooks cjson_hooks;
    cjson_hooks.malloc_fn = aos_malloc;
    cjson_hooks.free_fn = aos_free;
    cJSON_InitHooks(&cjson_hooks);
#endif

    ret = udata_init();
    if (unlikely(ret != 0)) {
        LOG("%s %s %d\n", __func__, ERROR_LINE, __LINE__);
    }

#if defined DTC_LINKKIT
    linkkit_sample_start();
#elif defined DTC_MQTT
    mqtt_sample_start();
#endif

    (void)gateway_sample();

#if defined(DTC_MQTT) || defined(DTC_LINKKIT)
    aos_loop_run();
#else
    while(1) {
        aos_msleep(1000);
    };
#endif

    return 0;
}


