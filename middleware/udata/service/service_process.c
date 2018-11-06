/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 *
 *
 * uData service process example
 */

#include <stdlib.h>
#include <string.h>
#include "service_mgr.h"
#include "abs_data_model.h"
#include "uData_parse.h"


typedef struct{
    udata_type_e type;
    size_t (*service_process_cb)(uint32_t abs_index, void *pdata,uint32_t len); 
    int (*service_ioctl_cb)(udata_type_e type,uint32_t abs_index);
}service_cb_func_t;
static int udata_std_service_ioctl_cb(udata_type_e type, uint32_t abs_index);
static size_t udata_std_service_process_callback(uint32_t abs_index, void *arg, uint32_t len);


static service_cb_func_t  g_service_cb[UDATA_MAX_CNT] = {
{UDATA_SERVICE_ACC, udata_std_service_process_callback, udata_std_service_ioctl_cb}, 
{UDATA_SERVICE_MAG, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_GYRO, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_ALS, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_PS, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_BARO, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_TEMP, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_UV, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_HUMI, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_NOISE, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_PM25, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_PM1P0, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_PM10, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_CO2, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_HCHO, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_TVOC, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_PH, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_VWC, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_EC, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_SALINITY, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_TDS, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_WINDSPD, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_WINDDIR, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_RAIN, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_HALL, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_HR, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_RGB, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_GS, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_IR, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_PEDOMETER, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_PDR, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_VDR, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_GPS, udata_std_service_process_callback, udata_std_service_ioctl_cb},
{UDATA_SERVICE_RTC, udata_std_service_process_callback, udata_std_service_ioctl_cb}
};


static int udata_std_service_ioctl_cb(udata_type_e type, uint32_t abs_index)
{
    (void)type;
    (void)abs_index;
    return 0;
}

static size_t udata_std_service_process_callback(uint32_t abs_index, void *arg,
                                                 uint32_t len)
{
    return DATA_SIZE;
}

int service_process_init(void)
{
    int i,j;
    int ret;
    uint32_t num = 0;
    uData_service_t svc;

    ret = uData_config_num_get(&num);
    if (unlikely(ret)) {
        return -1;
    }

    if(num == 0){
        return -1;
    }
    for (i = 0; i < num; i++) {
        memset(&svc, 0, sizeof(uData_service_t));
        ret = uData_service_config_parse(i, &svc);
        if (unlikely(ret)) {
            //LOG("%s parse service index %d  %d skip\n", uDATA_STR, i,__LINE__);
            continue;
        }
        svc.config.id          = SENSOR_IOCTL_ODR_SET;
        svc.config.range       = 0;/* no need here, set by the default value in the driver layer */
        svc.service_process_cb = NULL;
        svc.service_ioctl_cb   = NULL;

        for(j = 0; j < UDATA_MAX_CNT; j++){
            if(g_service_cb[j].type == svc.type){
                svc.service_process_cb = g_service_cb[j].service_process_cb;
                svc.service_ioctl_cb   =  g_service_cb[j].service_ioctl_cb;
                break;
            }
        }

        if (uData_service_register(&svc)) {
            //LOG("%s register service index %d type %d skip\n", uDATA_STR, i, svc.type);
            continue;
        }

        LOG("%s register service type %d  success\n", uDATA_STR, svc.type);
    }
    return 0;
}
