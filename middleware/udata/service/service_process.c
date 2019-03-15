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
#include "udata_parse.h"

static int udata_std_service_ioctl_cb(udata_type_e type, uint32_t abs_index)
{
    (void)type;
    (void)abs_index;
    return 0;
}

static size_t udata_std_service_process_callback(udata_type_e type, uint32_t abs_index, void *arg,
                                                 uint32_t len)
{
    int ret;
    ret = udata_install_report_pkg(type, arg, len);
    if(unlikely(ret)){
        return 0;
    }
    
    return DATA_SIZE;
}

int service_process_init(void)
{
    int i;
    int ret;
    uint32_t num = 0;
    udata_service_t svc;

    ret = udata_service_num_get(&num);
    if (unlikely(ret)) {
        return -1;
    }

    if(num == 0){
        return -1;
    }
    for (i = 0; i < num; i++) {
        memset(&svc, 0, sizeof(udata_service_t));
        ret = udata_service_config_parse(i, &svc);
        if (unlikely(ret)) {
            //LOG("%s parse service index %d  %d skip\n", uDATA_STR, i,__LINE__);
            continue;
        }
        svc.config.id          = SENSOR_IOCTL_ODR_SET;
        svc.config.range       = 0;/* no need here, set by the default value in the driver layer */
        svc.service_process_cb = NULL;
        svc.service_ioctl_cb   = NULL;

        svc.service_process_cb = udata_std_service_process_callback;
        svc.service_ioctl_cb   = udata_std_service_ioctl_cb;


        if (udata_service_register(&svc)) {
            //LOG("%s register service index %d type %d skip\n", uDATA_STR, i, svc.type);
            continue;
        }

        LOG("%s register service type %d  success\n", uDATA_STR, svc.type);
    }
    return 0;
}


