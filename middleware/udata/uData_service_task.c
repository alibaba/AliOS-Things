#include "uData_queue.h"


int g_taskid;
#define UDATA_TASK_STACK_SIZE 4096 // 4kByte
#define UDATA_TASK_PRIO \
    (AOS_DEFAULT_APP_PRI - 2) // higher prio than normal app's

void process_example(void *arg)
{
    
    char rec_cached[256];
    sensor_msg_pkg_t* msg = NULL;
    unsigned int  size = 0;
    int ret =0;
    
    while (DO_FOREVER) {
        
        ret = aos_msg_recv(g_taskid,AOS_WAIT_FOREVER,(void*)rec_cached,&size);
        if (size == 0||unlikely(ret)) {
            krhino_task_sleep(100);
            continue;
        }
        
        msg = (sensor_msg_pkg_t *)rec_cached;
        if (msg->cmd != CODE_UDATA_SERVICE_PROCESS) {
            continue;
        }
        LOG("algo own task tag=%d\n",msg->value);
        
    }
    
    
}

int uData_service_own_task_start(void)
{
    g_taskid = uData_new_servicetask("algo_process_task",process_example,NULL,
                UDATA_TASK_STACK_SIZE,UDATA_TASK_PRIO);
    uData_observe_servicetask_tag(g_taskid,TAG_DEV_HUMI);
    uData_observe_servicetask_tag(g_taskid,TAG_DEV_TEMP);
    
    return 0;
}
