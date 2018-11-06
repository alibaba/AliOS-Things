

#include <stdio.h>
#include <string.h>
#include "uData_queue.h"
#include "abs_data_model.h"


#define UDATA_QUEUE_MAXSLOTS 8
#ifndef UDATA_TASK_STACK_SIZE
#define UDATA_TASK_STACK_SIZE 4096 // 4kByte
#endif
#define UDATA_TASK_PRIO \
    (AOS_DEFAULT_APP_PRI - 2) // higher prio than normal app's
#define UDATA_QUEUE_MAX_MSG_SIZE (sizeof(sensor_msg_pkg_t))
#define UDATA_QUEUE_MAX_MSG_COUNT (32)
#define UDATA_QUEUE_SIZE (UDATA_QUEUE_MAX_MSG_SIZE * UDATA_QUEUE_MAX_MSG_COUNT)
#define UDATA_MAX_OWN_TASK_NUM 4
#define UDATA_SERVICE_TAG_NUM 4

struct _servicetask_related_tag_t
{
    uint32_t index[SENSOR_MAX_NUM];
    int actual_num;
};
typedef struct _servicetask_related_tag_t servicetask_related_tag_t;

servicetask_related_tag_t g_uData_own_task_tag[UDATA_MAX_OWN_TASK_NUM];

aos_queue_t             g_uData_queue;
static uData_queue_cb_t g_uData_queue_cb[UDATA_QUEUE_MAXSLOTS];
char                    g_uData_msg[UDATA_QUEUE_SIZE] = { 0 };
aos_task_t              g_uData_task;
aos_task_t              g_uData_own_task[UDATA_MAX_OWN_TASK_NUM];
int                     g_uData_own_task_cnt =0;
aos_queue_t             g_uData_own_task_queue[UDATA_MAX_OWN_TASK_NUM];
char                    g_uData_own_task_msg[UDATA_MAX_OWN_TASK_NUM][UDATA_QUEUE_SIZE] = { 0 };

static void uData_msg_dispatcher(void *arg)
{

    int  ret = 0;
    char rec_cached[256];
    // all the cmd of sensorhub will be sent to be handled here;
    // the dispatcher will asign the new sub task to the fitted model
    while (DO_FOREVER) {
        uint32_t          size = 0;
        sensor_msg_pkg_t *msg  = NULL;
        ret = aos_queue_recv(&g_uData_queue, AOS_WAIT_FOREVER,
                             (void *)rec_cached, (unsigned int *)(&size));
        
        if (unlikely(ret)) {
            continue;
        }
                             
        if (size == 0) {
            continue;
        }

        msg = (sensor_msg_pkg_t *)rec_cached;
        for (int n = 0; n < UDATA_QUEUE_MAXSLOTS; n++) {
            if (g_uData_queue_cb[n].status == UDATA_QUEUE_OPEN) {
                g_uData_queue_cb[n].msg_cb(msg);
            }
        }
    }
}

int uData_msg_dispatcher_task_create(void)
{
    int ret = 0;
    ret = aos_task_new_ext(&g_uData_task, "uData_queue_task", uData_msg_dispatcher,
                       NULL, UDATA_TASK_STACK_SIZE, UDATA_TASK_PRIO);
    if (unlikely(ret)) {
        return -1;
    }

    return 0;
}

int uData_register_msg_handler(void *func)
{
    int index;
    if (func == NULL) {
        return -1;
    }

    for (index = 0; index < UDATA_QUEUE_MAXSLOTS; index++) {
        if (g_uData_queue_cb[index].status == UDATA_QUEUE_CLOSE) {
            g_uData_queue_cb[index].status = UDATA_QUEUE_OPEN;
            g_uData_queue_cb[index].msg_cb = func;
            return index;
        }
    }
    return -1;
}


int uData_unregister_msg_handler(int index)
{
    if (index >= UDATA_QUEUE_MAXSLOTS || index <= 0)
        return -1;
    g_uData_queue_cb[index].msg_cb = NULL;
    g_uData_queue_cb[index].status = UDATA_QUEUE_CLOSE;
    return 0;
}


int own_task_post_msg(sensor_msg_pkg_t msg)
{
    for(int i = 0; i < g_uData_own_task_cnt; i++)
    {
        for(int j =0; j< g_uData_own_task_tag[i].actual_num;j++)
        {
            if(g_uData_own_task_tag[i].index[j] == msg.value)
            {
                aos_queue_send(&g_uData_own_task_queue[i],(void *)&msg, sizeof(msg));
                break;
            }
        }
    }

    return 0;
}


int uData_post_msg(sensor_msg_pkg_t msg)
{
    if(msg.cmd == UDATA_MSG_SERVICE_PROCESS)
    {
        own_task_post_msg(msg);
    }
    aos_queue_send(&g_uData_queue, (void *)&msg, sizeof(msg));
    return 0;
}


int uData_new_servicetask(const char *name, void (*fn)(void *),void *arg,
                            int stack_size, int prio)
{
    int ret;
    int taskid;
    if(g_uData_own_task_cnt >= UDATA_MAX_OWN_TASK_NUM) return -1;
    if(name == NULL || fn ==NULL) return -1;

    ret = aos_queue_new(&g_uData_own_task_queue[g_uData_own_task_cnt], (void *)&g_uData_own_task_msg[g_uData_own_task_cnt], 
            UDATA_QUEUE_SIZE,UDATA_QUEUE_MAX_MSG_SIZE);
    
    if (unlikely(ret)) {
        return -1;
    }
    ret = aos_task_new_ext(&g_uData_own_task[g_uData_own_task_cnt], name, fn,
                       arg, stack_size, prio);
    
    if (unlikely(ret)) {
        LOG("uData_new_servicetask error\n");
        return -1;
    }
    
    g_uData_own_task_cnt++;
    taskid = g_uData_own_task_cnt -1;
    return taskid;
}

int uData_observe_servicetask_tag(int taskid,sensor_tag_e tag, uint8_t instance)
{
    int i = 0;
    int ret;
    uint32_t index;
    
    if(taskid >= g_uData_own_task_cnt || taskid < 0) 
    {
        return -1;
    }
    int actual_num = g_uData_own_task_tag[taskid].actual_num;
    if( actual_num >= UDATA_SERVICE_TAG_NUM) 
    {
        return -1;
    }
    ret = abs_data_get_abs_index(tag, instance, &index);
    if(unlikely(ret)){
        return -1;
    }
    
    LOG("uData_set_servicetask_tag taskid=%d,actual_num=%d,tag=%d,g_uData_own_task_cnt=%d\n",taskid,actual_num,tag,g_uData_own_task_cnt);
    for(i=0; i < actual_num; i++)
    {
        if(g_uData_own_task_tag[taskid].index[i] == index)
        {
            return -1;
        }
    }
    g_uData_own_task_tag[taskid].index[actual_num] = index;
    g_uData_own_task_tag[taskid].actual_num++;
    LOG("uData_set_servicetask_tag sucessfull\n");
    return 0;
}


int aos_msg_recv(int task_id, unsigned int ms, void *msg,
                   unsigned int *size)
{
    int ret = 0;
    if(msg == NULL) return -1;
    if (task_id >= g_uData_own_task_cnt) return -1;
    ret = aos_queue_recv(&g_uData_own_task_queue[task_id], ms,(void *)msg, size);
    return ret;
}

int uData_start()
{
    int ret;
    memset(g_uData_queue_cb, 0,UDATA_QUEUE_MAXSLOTS * sizeof(uData_queue_cb_t));
    memset(g_uData_own_task_tag,0,UDATA_MAX_OWN_TASK_NUM * sizeof(servicetask_related_tag_t));

    // create the buf queue for the service dispatcher
    ret = aos_queue_new(&g_uData_queue, (void *)&g_uData_msg, UDATA_QUEUE_SIZE,
                        UDATA_QUEUE_MAX_MSG_SIZE);
    if (unlikely(ret)) {
        return -1;
    }

    // create the service dispatcher task
    ret = uData_msg_dispatcher_task_create();
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}
