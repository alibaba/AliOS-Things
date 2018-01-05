/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*
*
* uData service example
*/

#include "service_mgr.h"

int udata_baro_service_ioctl_cb(udata_type_e type, sensor_tag_e tag)
{
 
    tag = TAG_DEV_BARO;
    return 0;
}

size_t udata_baro_service_process_cb(sensor_tag_e tag, void* pdata)
{
    barometer_data_t * baro =  (barometer_data_t *)pdata;
    size_t len = sizeof(barometer_data_t);
    
    LOG("%s udata_baro_service_cb = (%d), (%d), (%d)\n",uDATA_STR, tag, baro->p, baro->timestamp);
    return len;
}

int udata_baro_service_result(void* pdata){
    return 0;
}

int udata_baro_service_init(void)
{
    int ret = 0;
    uData_service_t * baro;
    baro = (uData_service_t *)malloc(sizeof(uData_service_t));
    if(baro == NULL){
        return -1;
    }
    baro->type = UDATA_SERVICE_BARO;
    baro->tag = TAG_DEV_BARO;
    baro->config.id = SENSOR_IOCTL_ODR_SET;
    baro->config.odr = 2; /* 2Hz */
    baro->config.range = 0; /* no need here, set by the default value in the driver layer */
    baro->service_process_cb = udata_baro_service_process_cb;
    baro->service_ioctl_cb = udata_baro_service_ioctl_cb;
    ret = uData_service_register(baro);
    if(unlikely(ret)){
        free(baro);
        LOG("%s %s %s %d\n",  uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    free(baro);
    LOG("%s %s successfully\n", uDATA_STR, __func__);
    return 0;
}

