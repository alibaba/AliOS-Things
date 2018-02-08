/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>
#include "aos/aos.h"
#include "aos/network.h"
#include "kvmgr.h"
#include <netmgr.h>
#include "aos/uData.h"

void uData_report_demo(input_event_t *event, void *priv_data)
{
    udata_pkg_t buf;
    if ((event == NULL)||(event->type != EV_UDATA)) {
        return;
    }
    
    if(event->code == CODE_UDATA_REPORT_PUBLISH){
        int ret = 0;
        ret = uData_report_publish(event,&buf);
        if(ret == 0){
            barometer_data_t* data = buf.payload;
            printf("uData_application::::::::::::::type = (%d)\n", buf.type);
            printf("uData_application::::::::::::::data = (%d)\n", data->p);
            printf("uData_application:::::::::timestamp = (%d)\n", data->timestamp);
        }
    }
}

int application_start(int argc, char *argv[])
{
    int ret = 0;
    
    aos_register_event_filter(EV_UDATA, uData_report_demo, NULL);

    ret = uData_subscribe(UDATA_SERVICE_BARO);
    if(ret != 0){
        printf("%s %s %s %d\n", uDATA_STR, __func__, ERROR_LINE, __LINE__);
        return -1;
    }
    aos_loop_run();

    return 0;
}
