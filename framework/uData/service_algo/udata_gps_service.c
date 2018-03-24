/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*
*
* uData service example
*/

#include "service_mgr.h"

/*read 4 times beforce report once */
#define  SERVICE_GPS_REORT_FREQ_REDUCE     4
static  int g_gpsreportfreq = 0;

int udata_gps_service_ioctl_cb(udata_type_e type, sensor_tag_e tag)
{
    return 0;
}

size_t udata_gps_service_process_cb(sensor_tag_e tag, void* pdata)
{
    gps_data_t * gps =  (gps_data_t *)pdata;
    size_t len = sizeof(gps_data_t);

    LOG("%s udata_gps_service_process_cb = (%d), (%f), (%f)\n",uDATA_STR, tag, gps->lon, gps->lat);

    if(0 == ((g_gpsreportfreq++)%SERVICE_GPS_REORT_FREQ_REDUCE))
    {
        return len;
    }
    else
    {
        return 0;
    }
}

int udata_gps_service_result(void* pdata)
{
    return 0;
}

int udata_gps_service_init(void)
{
    int ret = 0;
    uData_service_t * gps;
    gps = (uData_service_t *)malloc(sizeof(uData_service_t));
    if(gps == NULL){
        return -1;
    }
    gps->type = UDATA_SERVICE_GPS;
    gps->tag = TAG_DEV_GPS;
    gps->config.id = SENSOR_IOCTL_ODR_SET;
    gps->config.odr = 1; /* 1Hz */
    gps->config.range = 0; /* no need here, set by the default value in the driver layer */
    gps->service_process_cb = udata_gps_service_process_cb;
    gps->service_ioctl_cb = udata_gps_service_ioctl_cb;
    ret = uData_service_register(gps);
    if(unlikely(ret)){
        free(gps);
        LOG("%s %s %s %d\n",  uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    free(gps);
    LOG("%s %s successfully\n", uDATA_STR, __func__);
    return 0;
}

