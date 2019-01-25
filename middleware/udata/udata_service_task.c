/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 *
 *
 * uData service task
 */

#include "udata_queue.h"
#include "abs_data_model.h"
#include "service_mgr.h"

int g_taskid;
#define UDATA_TASK_STACK_SIZE 2048 /* 2kByte */
#define UDATA_TASK_PRIO \
    (AOS_DEFAULT_APP_PRI - 2)      /* higher prio than normal app's */

void process_example(void *arg)
{
    
    char buffer[256];
    sensor_msg_pkg_t* msg = NULL;
    unsigned int  size = 0;
    int ret =0;
    sensor_tag_e tag;
    uint8_t index;
    void* data = NULL;
    uint32_t len;
    
    while (DO_FOREVER) {
        ret = udata_msg_recv(g_taskid,AOS_WAIT_FOREVER,(void*)buffer,&size);
        if (size == 0||unlikely(ret)) {
            aos_msleep(100);
            continue;
        }
        
        msg = (sensor_msg_pkg_t *)buffer;
        if (msg->cmd != UDATA_MSG_SERVICE_PROCESS) {
            continue;
        }
        
        ret = abs_data_get_sensor_info(msg->index,&tag, &index);
        if (unlikely(ret)) {
            return;
        }

        if ((tag == TAG_DEV_HUMI) || (tag == TAG_DEV_TEMP)){
            ret = udata_service_get_payload(msg->index, &data, &len);
        
            ret = udata_install_report_pkg(UDATA_SERVICE_HUMI, data, len);
            if (unlikely(ret)) {
                return;
            }
        }
                
    }
    
}

int udata_service_own_task_start(void)
{
    g_taskid = udata_new_servicetask("algo_process_task",process_example,NULL,
                UDATA_TASK_STACK_SIZE,UDATA_TASK_PRIO);
    if(g_taskid < 0)
    {
        return -1;
    }
    udata_observe_servicetask_tag(g_taskid,TAG_DEV_HUMI, 0);
    udata_observe_servicetask_tag(g_taskid,TAG_DEV_TEMP, 0);
    
    return 0;
}
