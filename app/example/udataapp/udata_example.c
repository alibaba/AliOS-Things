/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <aos/aos.h>
#include <aos/yloop.h>
#include "aos/uData.h"
#include "uData_queue.h"
#ifdef UDATA_CJSON_SUPPORTED
#include "cJSON.h"
#endif

#define UDATA_PRINT    printf

#define UDATA_SHOW_UINT_1(TYPE,TIME,DATA1) \
do{\
    UDATA_PRINT("uData_application::::::::::::::type = (%d)\n", (int)(TYPE)); \
    UDATA_PRINT("uData_application::::::::::::::data = (%d)\n", (int)(DATA1)); \
    UDATA_PRINT("uData_application:::::::::timestamp = (%u)\n", (unsigned int)(TIME)); \
}while(0);

#define UDATA_SHOW_UINT_3(TYPE,TIME,DATA1,DATA2,DATA3) \
do{\
    UDATA_PRINT("uData_application::::::::::::::type = (%d)\n", (int)(TYPE)); \
    UDATA_PRINT("uData_application::::::::::::::data = (%d) (%d) (%d)\n", (int)(DATA1),(int)(DATA2),(int)(DATA3)); \
    UDATA_PRINT("uData_application:::::::::timestamp = (%u)\n", (unsigned int)(TIME)); \
}while(0);

#define UDATA_SHOW_FLOAT_3(TYPE,TIME,DATA1,DATA2,DATA3) \
do{\
    UDATA_PRINT("uData_application::::::::::::::type = (%d)\n", (int)(TYPE)); \
    UDATA_PRINT("uData_application::::::::::::::data = (%f) (%f) (%f)\n", (DATA1),(DATA2),(DATA3)); \
    UDATA_PRINT("uData_application:::::::::timestamp = (%u)\n", (unsigned int)(TIME)); \
}while(0);

#define DEFAULT_INTERVAL  (3000)

#if defined DTC_MQTT
extern int mqtt_sample_start(void);

extern int udata_mqtt_publish(udata_type_e type, void* pdata, uint32_t len);

#elif defined DTC_LINKKIT
extern int linkkit_sample_start(void);
extern int udata_linkkit_publish(udata_type_e type, void* pdata, uint32_t len);
#endif



#if defined( DTC_MQTT ) || defined( DTC_LINKKIT )
static int64_t g_time[UDATA_MAX_CNT] = {0};
static uint32_t g_interval[UDATA_MAX_CNT] = {0};
int uData_interval_init(udata_type_e type, uint32_t interval)
{
    if(type >= UDATA_MAX_CNT){
        return -1;
    }
    
    g_interval[type] = interval;

    return 0;
}


bool uData_dtc_timeout(udata_type_e type)
{
    int64_t time;
    if(type >= UDATA_MAX_CNT){
        return false;
    }

    if(0 == g_interval[type]){
        return false;
    }
    time = aos_now_ms();

    if((time - g_time[type]) >= g_interval[type]){
        g_time[type] = time;
        return true;
    }

    return false;

}

#endif
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
        ret = uData_report_publish(msg, &buf);
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
    LOG("uData_queue_registerslot service_dtc_handle ret=%d\n", ret);
    if (ret == -1) {
        LOG("error occur reg uData_report_demo \n");
        return ret;
    }

    ret = uData_subscribe(UDATA_SERVICE_HUMI);
    if (ret != 0) {
        LOG("%s %s %s %d\n", uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }

    ret = uData_subscribe(UDATA_SERVICE_TEMP);
    if (ret != 0) {
        LOG("%s %s %s %d\n", uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }

    return 0;
}



int application_start(int argc, char **argv)
{

#ifdef UDATA_CJSON_SUPPORTED
    cJSON_Hooks cjson_hooks;
    cjson_hooks.malloc_fn = aos_malloc;
    cjson_hooks.free_fn = aos_free;
    cJSON_InitHooks(&cjson_hooks);
#endif

#if defined DTC_MQTT
    mqtt_sample_start();
#elif defined DTC_LINKKIT
    linkkit_sample_start();
#endif

    (void)udata_sample();

    aos_loop_run();
    return 0;
}


