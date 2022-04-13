/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 *
 *
 * udata config para
 *
 *
 */

#include "udata_parse.h"

#define UDATA_ARRAY_NUM_CALC(A)         (sizeof(A)/sizeof(A[0]))

typedef struct _udata_tag_para_st{
    sensor_tag_e tag;
    int          instance;
    int          interval;
}udata_tag_para_st;

typedef struct udata_service_para_t{
    udata_type_e        type;
    bool                task_flag;
    uint8_t             dev_num;
    udata_tag_para_st*  p_tag_para;
}udata_service_para_t;

typedef struct udata_dtc_para_t{
    udata_type_e        type;
    bool                dtc_flag;
    int                 dtc_cycle;
    udata_data_type     data_type;
    int                 coeff;
    char*               name;
    uint8_t             name_num;
    char**              para_name;
}udata_dtc_para_t;

udata_tag_para_st g_dev_acc[] = {
{TAG_DEV_ACC, 0, 1000},
};

udata_tag_para_st g_dev_mag[] = {
{TAG_DEV_MAG, 0, 1000},
};

udata_tag_para_st g_dev_gyro[] = {
{TAG_DEV_GYRO, 0, 1000},
};

udata_tag_para_st g_dev_als[] = {
{TAG_DEV_ALS, 0, 2000},
};

udata_tag_para_st g_dev_ps[] = {
{TAG_DEV_PS, 0, 2000},
};

udata_tag_para_st g_dev_baro[] = {
{TAG_DEV_BARO, 0, 2000},
};

udata_tag_para_st g_dev_temp[] = {
{TAG_DEV_TEMP, 0, 2000},
};

udata_tag_para_st g_dev_noise[] = {
{TAG_DEV_NOISE, 0, 2000},
};

udata_tag_para_st g_dev_pm25[] = {
{TAG_DEV_PM25, 0, 2000},
};

udata_tag_para_st g_dev_humi[] = {
{TAG_DEV_HUMI, 0, 2000},
};
udata_tag_para_st g_dev_gps[] = {
{TAG_DEV_GPS, 0, 2000},
};

udata_tag_para_st g_dev_rtctime[] = {
{TAG_DEV_RTC, 0, 2000},
};

char* g_para_acc[] = {
"x","y","z"
};
char* g_para_mag[] = {
"x_gs","y_gs","z_gs"
};
char* g_para_gyro[] = {
"x_dps","y_dps","z_dps"
};

char* g_para_gps[] = {
"Latitude","Longitude","Elevation"
};
char* g_para_rtctime[] = {
"year","month","date","day","hours","minutes","seconds"
};

udata_service_para_t g_service_para[] = {
{UDATA_SERVICE_ACC, false, UDATA_ARRAY_NUM_CALC(g_dev_acc),&g_dev_acc[0]},
{UDATA_SERVICE_MAG, false, UDATA_ARRAY_NUM_CALC(g_dev_mag),&(g_dev_mag)[0]},
{UDATA_SERVICE_GYRO, false, UDATA_ARRAY_NUM_CALC(g_dev_gyro),&g_dev_gyro[0]},
{UDATA_SERVICE_ALS, false, UDATA_ARRAY_NUM_CALC(g_dev_als),&g_dev_als[0]},
{UDATA_SERVICE_PS, false, UDATA_ARRAY_NUM_CALC(g_dev_ps),&g_dev_ps[0]},
{UDATA_SERVICE_BARO, false,  UDATA_ARRAY_NUM_CALC(g_dev_baro), &g_dev_baro[0]},
{UDATA_SERVICE_TEMP, false,  UDATA_ARRAY_NUM_CALC(g_dev_temp), &g_dev_temp[0]},
{UDATA_SERVICE_NOISE, false,  UDATA_ARRAY_NUM_CALC(g_dev_noise), &g_dev_noise[0]},
{UDATA_SERVICE_PM25, false,  UDATA_ARRAY_NUM_CALC(g_dev_pm25), &g_dev_pm25[0]},
{UDATA_SERVICE_HUMI, false,  UDATA_ARRAY_NUM_CALC(g_dev_humi), &g_dev_humi[0]},
{UDATA_SERVICE_GPS, false,  UDATA_ARRAY_NUM_CALC(g_dev_gps), &g_dev_gps[0]},
{UDATA_SERVICE_RTC, false,  UDATA_ARRAY_NUM_CALC(g_dev_rtctime), &g_dev_rtctime[0]}
};

udata_dtc_para_t g_dtc_para[] = {
{UDATA_SERVICE_ACC, false, 4000,UDATA_FLOAT,1000,"Accelerometer",UDATA_ARRAY_NUM_CALC(g_para_acc),&g_para_acc[0]},
{UDATA_SERVICE_MAG, false, 4000,UDATA_FLOAT,1000,"Magnetometer",UDATA_ARRAY_NUM_CALC(g_para_mag),&g_para_mag[0]},
{UDATA_SERVICE_GYRO, false, 4000,UDATA_FLOAT,1000000,"Gyroscope",UDATA_ARRAY_NUM_CALC(g_para_gyro),&g_para_gyro[0]},

{UDATA_SERVICE_ALS, false, 4000,UDATA_UINT32,1,"LightLux",1, NULL},
{UDATA_SERVICE_PS, false, 4000,UDATA_UINT32,1,"Proximity",1, NULL},
{UDATA_SERVICE_BARO, false, 4000,UDATA_UINT32,1,"Barometer",1, NULL},
{UDATA_SERVICE_TEMP, false, 4000,UDATA_FLOAT,10,"CurrentTemperature",1, NULL},
{UDATA_SERVICE_NOISE, false, 4000,UDATA_FLOAT,10,"NoiseLoudness",1, NULL},
{UDATA_SERVICE_PM25, false, 4000,UDATA_FLOAT,10,"PM25Value",1, NULL},
{UDATA_SERVICE_HUMI, false, 4000,UDATA_FLOAT,10,"CurrentHumidity",1, NULL},

{UDATA_SERVICE_GPS, false, 4000,UDATA_FLOAT,1,"Gps",UDATA_ARRAY_NUM_CALC(g_para_gps),&g_para_gps[0]},
{UDATA_SERVICE_RTC, false, 4000,UDATA_INT32,1,"RTCTime",UDATA_ARRAY_NUM_CALC(g_para_rtctime),&g_para_rtctime[0]}

};
