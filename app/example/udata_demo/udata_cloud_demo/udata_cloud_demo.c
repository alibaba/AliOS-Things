/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "aos/cli.h"
#include "aos/kernel.h"
#include "udata/udata.h"
#include "udata_queue.h"
#include "service_mgr.h"
#include "app_entry.h"


#define UDATA_PRINT    printf

#define PROP_POST_FORMAT_TEMP         "{\"CurrentTemperature\":%.1f}"
#define PROP_POST_FORMAT_ACC          "{\"Accelerometer\":{\"x\":%.2f, \"y\":%.2f, \"z\":%.2f}}"
extern int app_cloud_report(void* pdata, uint32_t len);

int udata_local_publish(udata_type_e type, void* pdata, uint32_t len)
{
    int ret;
    uint8_t param[128];
    if(pdata == NULL){
        return -1;
    }

    switch (type) {

         case UDATA_SERVICE_ACC: {
            accel_data_t *acc = (accel_data_t *)pdata;
            UDATA_PRINT("\nAcceleration value: x-axis(%.3f g) y-axis(%.3f g) z-axis(%.3f g) \n",((float)acc->data[0])/1000 ,((float)acc->data[1])/1000, ((float)acc->data[2])/1000);

            memset(param,0,128);
            sprintf(param, PROP_POST_FORMAT_ACC, ((float)acc->data[0])/1000 ,((float)acc->data[1])/1000, ((float)acc->data[2])/1000);

            ret = app_cloud_report(param,strlen(param) + 1);
            if(unlikely(ret)){
                LOG("%s %s %d\n", uDATA_STR, __func__,__LINE__);
            }

            break;
         }
         case UDATA_SERVICE_TEMP: {
            temperature_data_t *temp = (temperature_data_t *)pdata;
            UDATA_PRINT("\nTemperature value : %.1f centidegree\n",((float)temp->t)/10);

            memset(param,0,128);
            sprintf(param, PROP_POST_FORMAT_TEMP, ((float)(temp->t))/10);

            ret = app_cloud_report(param,strlen(param) + 1);
            if(unlikely(ret)){
                LOG("%s %s %d\n", uDATA_STR, __func__,__LINE__);
            }

            break;
         }

         default:
             break;
     }

    return 0;

}

void udata_report_demo(sensor_msg_pkg_t *msg)
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

        ret = udata_local_publish(buf.type,buf.payload,0);
        if (ret != 0) {
            return;
        }

    }
}

void udata_cloud_test(void)
{
    int ret = 0;

    linkkit_sample_start();
    (void)udata_init();

    ret = udata_register_msg_handler(udata_report_demo);
    if (ret < 0) {
        LOG("%s %s %d\n", uDATA_STR, __func__,__LINE__);
        return;
    }

    ret = udata_subscribe(UDATA_SERVICE_ACC);
    if (unlikely(ret)) {
        LOG("%s %s %d\n", uDATA_STR, __func__, __LINE__);
    }

    ret = udata_subscribe(UDATA_SERVICE_TEMP);
    if (unlikely(ret)) {
        LOG("%s %s %d\n", uDATA_STR, __func__, __LINE__);
    }

    aos_loop_run();
}

int application_start(int argc, char **argv)
{
    udata_cloud_test();

    return 0;
}


