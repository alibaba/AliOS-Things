/*
* Copyright (C) 2015-2017 Alibaba Group Holding Limited
*
*
* uData api for external part
*/

#include <aos/aos.h>
#include <stdio.h> 
#include <stdbool.h>
#include <fcntl.h>
#include <aos/yloop.h>
#include "uData_com_desc.h"

int uData_report_publish(input_event_t *event,void *pdata)
{
    if(event == NULL){
        return -1;
    }
    if(pdata == NULL){
        return -1;
    }
    if(event->value >= UDATA_MAX_CNT){
        return -1;
    }
    
    return uData_get_report_pkg(pdata,event->value);
}

int uData_dev_ioctl(udata_t* pkg, uint8_t cmd, void* parm)
{
    /* set the udata_type and related info here */
    //will be supported later
}

int uData_subscribe(udata_type_e type)
{
    int ret = 0;
    ret = uData_service_subscribe(type);
    if(unlikely(ret)){
        return -1;
    }
    
    LOG("%s %s successfully\n", uDATA_STR, __func__);
    return 0;
}

int uData_unsubscribe(udata_type_e type)
{
    int ret = 0;
    ret = uData_service_unsubscribe(type);
    if(unlikely(ret)){
        return -1;
    }
    
    LOG("%s %s successfully\n", uDATA_STR, __func__);
    return 0;
}


