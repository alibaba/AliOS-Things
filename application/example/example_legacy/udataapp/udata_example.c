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
#include "ulog/ulog.h"

#if UDATA_CONFIG_CJSON_FORMAT || UDATA_CONFIG_DTC_ENABLE
#include "cJSON.h"
#endif

#define UDATA_PRINT      printf

#define DEFAULT_INTERVAL (3000)

#define UDATA_SHOW_UINT_1(TYPE,TIME,DATA1) \
do{\
    UDATA_PRINT("udata_application::::::::::::::type = (%d)\n", (int)(TYPE)); \
    UDATA_PRINT("udata_application::::::::::::::data = (%d)\n", (int)(DATA1)); \
    UDATA_PRINT("udata_application:::::::::timestamp = (%u)\n", (unsigned int)(TIME)); \
}while(0);

#define UDATA_SHOW_UINT_3(TYPE,TIME,DATA1,DATA2,DATA3) \
do{\
    UDATA_PRINT("udata_application::::::::::::::type = (%d)\n", (int)(TYPE)); \
    UDATA_PRINT("udata_application::::::::::::::data = (%d) (%d) (%d)\n", (int)(DATA1),(int)(DATA2),(int)(DATA3)); \
    UDATA_PRINT("udata_application:::::::::timestamp = (%u)\n", (unsigned int)(TIME)); \
}while(0);

#define UDATA_SHOW_FLOAT_3(TYPE,TIME,DATA1,DATA2,DATA3) \
do{\
    UDATA_PRINT("udata_application::::::::::::::type = (%d)\n", (int)(TYPE)); \
    UDATA_PRINT("udata_application::::::::::::::data = (%f) (%f) (%f)\n", (DATA1),(DATA2),(DATA3)); \
    UDATA_PRINT("udata_application:::::::::timestamp = (%u)\n", (unsigned int)(TIME)); \
}while(0);


#if defined DTC_MQTT
extern int mqtt_sample_start(void);
extern int udata_mqtt_publish(udata_type_e type, void *pdata, uint32_t len);

#elif defined DTC_LINKKIT
extern int linkkit_sample_start(void);
extern int udata_linkkit_publish(udata_type_e type, void *pdata, uint32_t len);
#endif

#if defined( DTC_MQTT ) || defined( DTC_LINKKIT )
#define DATA_ASSEMBLE(a) {a,#a}

typedef struct {
    udata_type_e type;
    char        *type_name;
}udata_sample_service_name_st;

static udata_sample_service_name_st g_sample_service_name[UDATA_MAX_CNT] = {
    DATA_ASSEMBLE(UDATA_SERVICE_ACC),
    DATA_ASSEMBLE(UDATA_SERVICE_MAG),
    DATA_ASSEMBLE(UDATA_SERVICE_GYRO),
    DATA_ASSEMBLE(UDATA_SERVICE_ALS),
    DATA_ASSEMBLE(UDATA_SERVICE_PS),
    DATA_ASSEMBLE(UDATA_SERVICE_BARO),
    DATA_ASSEMBLE(UDATA_SERVICE_TEMP),
    DATA_ASSEMBLE(UDATA_SERVICE_UV),
    DATA_ASSEMBLE(UDATA_SERVICE_HUMI),
    DATA_ASSEMBLE(UDATA_SERVICE_NOISE),
    DATA_ASSEMBLE(UDATA_SERVICE_PM25),
    DATA_ASSEMBLE(UDATA_SERVICE_PM1P0),
    DATA_ASSEMBLE(UDATA_SERVICE_PM10),
    DATA_ASSEMBLE(UDATA_SERVICE_CO2),
    DATA_ASSEMBLE(UDATA_SERVICE_HCHO),
    DATA_ASSEMBLE(UDATA_SERVICE_TVOC),
    DATA_ASSEMBLE(UDATA_SERVICE_PH),
    DATA_ASSEMBLE(UDATA_SERVICE_VWC),
    DATA_ASSEMBLE(UDATA_SERVICE_EC),
    DATA_ASSEMBLE(UDATA_SERVICE_SALINITY),
    DATA_ASSEMBLE(UDATA_SERVICE_TDS),
    DATA_ASSEMBLE(UDATA_SERVICE_WINDSPD),
    DATA_ASSEMBLE(UDATA_SERVICE_WINDDIR),
    DATA_ASSEMBLE(UDATA_SERVICE_RAIN),
    DATA_ASSEMBLE(UDATA_SERVICE_HALL),
    DATA_ASSEMBLE(UDATA_SERVICE_HR),
    DATA_ASSEMBLE(UDATA_SERVICE_RGB),
    DATA_ASSEMBLE(UDATA_SERVICE_GS),
    DATA_ASSEMBLE(UDATA_SERVICE_IR),
    DATA_ASSEMBLE(UDATA_SERVICE_PEDOMETER),
    DATA_ASSEMBLE(UDATA_SERVICE_PDR),
    DATA_ASSEMBLE(UDATA_SERVICE_VDR),
    DATA_ASSEMBLE(UDATA_SERVICE_GPS),
    DATA_ASSEMBLE(UDATA_SERVICE_RTC)
};

bool         g_dtc_flag = false;
udata_type_e g_dtc_service_type = UDATA_MAX_CNT;

void handle_dtc_test_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    int          i;
    bool         flag;
    int          ret;
    udata_type_e type;

    char *para1 = argc > 1 ? argv[1] : "";
    char *para2 = argc > 2 ? argv[2] : "";

    for(i = 0; i < UDATA_MAX_CNT; i++){
        if ((strlen(para1) == strlen(g_sample_service_name[i].type_name))&&(strcmp(para1, g_sample_service_name[i].type_name) == 0)) {
            break;
        }
    }

    if(i == UDATA_MAX_CNT){
        return;
    }

    type = g_sample_service_name[i].type;
    if(type >= UDATA_MAX_CNT){
        return;
    }

    if (strcmp(para2, "on") == 0) {
        flag = true;
    }
    else if (strcmp(para2, "off") == 0) {
        flag = false;
    }
    else {
        printf("bad para2 %s \n",para2);
        return;
    }

    ret = service_dtc_publish_set(type, flag);
    if(unlikely(ret)){
        return;
    }

    if ((g_dtc_service_type != type) && (flag == true)){

        if (g_dtc_flag == true){
            ret = service_dtc_publish_set(g_dtc_service_type, false);
            if(unlikely(ret)){
                return;
            }
        }

        g_dtc_service_type = type;
        g_dtc_flag = flag;
    }

    return;
}

static struct cli_command dtccmd = {
    .name     = "dtc",
    .help     = "dtc status set",
    .function = handle_dtc_test_cmd
};

#endif
int udata_local_publish(udata_type_e type, void* pdata, uint32_t len)
{

    if(pdata == NULL){
        return -1;
    }

    switch (type) {

         case UDATA_SERVICE_ACC: {
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

void udata_report_demo(sensor_msg_pkg_t *msg)
{
    int         ret = 0;
    udata_pkg_t buf;

    if ((msg == NULL)) {
        return;
    }

    if (msg->cmd == UDATA_MSG_REPORT_PUBLISH) {
        ret = udata_report_publish(msg->value, &buf);
        if (ret != 0) {
            return;
        }

        ret = udata_local_publish(buf.type,buf.payload,0);
        if (ret != 0) {
            return;
        }

    }
}

int udata_sample(void)
{
    int ret = 0;
    ret = udata_register_msg_handler(udata_report_demo);
    if (ret < 0) {
        LOG("%s %s %d\n", __func__, ERROR_LINE, __LINE__);
        return ret;
    }

    ret = udata_subscribe(UDATA_SERVICE_HUMI);
    if (unlikely(ret)) {
        LOG("%s %s %d\n", __func__, ERROR_LINE, __LINE__);
    }

    ret = udata_subscribe(UDATA_SERVICE_TEMP);
    if (unlikely(ret)) {
        LOG("%s %s %d\n", __func__, ERROR_LINE, __LINE__);
    }


    return 0;
}

int application_start(int argc, char **argv)
{
    int ret;
#if UDATA_CONFIG_CJSON_FORMAT || UDATA_CONFIG_DTC_ENABLE
    cJSON_Hooks cjson_hooks;
    cjson_hooks.malloc_fn = aos_malloc;
    cjson_hooks.free_fn = aos_free;
    cJSON_InitHooks(&cjson_hooks);
#endif

    (void)udata_init();

#if defined DTC_LINKKIT
    linkkit_sample_start();
    aos_cli_register_command(&dtccmd);
#elif defined DTC_MQTT
    mqtt_sample_start();
    aos_cli_register_command(&dtccmd);
#endif

    (void)udata_sample();

#if defined(DTC_MQTT) || defined(DTC_LINKKIT)
    aos_loop_run();
#else
    while(1) {
        aos_msleep(1000);
    };
#endif

    return 0;
}


