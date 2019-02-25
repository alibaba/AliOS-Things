/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 *
 *
 * uData queue
 */



#include <stdio.h>
#include <string.h>
#include "udata_queue.h"
#include "abs_data_model.h"


#define UDATA_QUEUE_MAXSLOTS 8
#ifndef UDATA_TASK_STACK_SIZE
#define UDATA_TASK_STACK_SIZE 4096 /* 4kByte */
#endif
#define UDATA_TASK_PRIO \
    (AOS_DEFAULT_APP_PRI - 2)      /*higher prio than normal app's */
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

servicetask_related_tag_t g_udata_own_task_tag[UDATA_MAX_OWN_TASK_NUM];

aos_queue_t             g_udata_queue;
static udata_queue_cb_t g_udata_queue_cb[UDATA_QUEUE_MAXSLOTS];
char                    g_udata_msg[UDATA_QUEUE_SIZE] = { 0 };
aos_task_t              g_udata_task;
aos_task_t              g_udata_own_task[UDATA_MAX_OWN_TASK_NUM];
int                     g_udata_own_task_cnt =0;
aos_queue_t             g_udata_own_task_queue[UDATA_MAX_OWN_TASK_NUM];
char                    g_udata_own_task_msg[UDATA_MAX_OWN_TASK_NUM][UDATA_QUEUE_SIZE] = { 0 };

static void udata_msg_dispatcher(void *arg)
{

    int  ret = 0;
    char data[256];
    uint32_t          size = 0;
    sensor_msg_pkg_t *msg  = NULL;
    /* all the cmd of sensorhub will be sent to be handled here;
       the dispatcher will asign the new sub task to the fitted model */
    while (DO_FOREVER) {
        ret = aos_queue_recv(&g_udata_queue, AOS_WAIT_FOREVER,
                             (void *)data, (unsigned int *)(&size));

        if (unlikely(ret)) {
            continue;
        }

        if (size == 0) {
            continue;
        }

        msg = (sensor_msg_pkg_t *)data;
        for (int n = 0; n < UDATA_QUEUE_MAXSLOTS; n++) {
            if (g_udata_queue_cb[n].status == UDATA_QUEUE_OPEN) {
                g_udata_queue_cb[n].msg_cb(msg);
            }
        }
    }
}

int udata_msg_dispatcher_task_create(void)
{
    int ret = 0;
    ret = aos_task_new_ext(&g_udata_task, "udata_queue_task", udata_msg_dispatcher,
                       NULL, UDATA_TASK_STACK_SIZE, UDATA_TASK_PRIO);
    if (unlikely(ret)) {
        return -1;
    }

    return 0;
}

int udata_register_msg_handler(void *func)
{
    int index;
    if (func == NULL) {
        return -1;
    }

    for (index = 0; index < UDATA_QUEUE_MAXSLOTS; index++) {
        if (g_udata_queue_cb[index].status == UDATA_QUEUE_CLOSE) {
            g_udata_queue_cb[index].status = UDATA_QUEUE_OPEN;
            g_udata_queue_cb[index].msg_cb = func;
            return index;
        }
    }
    return -1;
}


int udata_unregister_msg_handler(int index)
{
    if (index >= UDATA_QUEUE_MAXSLOTS || index <= 0)
        return -1;
    g_udata_queue_cb[index].msg_cb = NULL;
    g_udata_queue_cb[index].status = UDATA_QUEUE_CLOSE;
    return 0;
}


void udata_own_task_post_msg(sensor_msg_pkg_t msg)
{
    for(int i = 0; i < g_udata_own_task_cnt; i++)
    {
        for(int j =0; j< g_udata_own_task_tag[i].actual_num;j++)
        {
            if(g_udata_own_task_tag[i].index[j] == msg.index)
            {
                aos_queue_send(&g_udata_own_task_queue[i],(void *)&msg, sizeof(msg));
                break;
            }
        }
    }

}


int udata_post_msg(sensor_msg_pkg_t msg)
{
    if(msg.cmd == UDATA_MSG_SERVICE_PROCESS)
    {
        udata_own_task_post_msg(msg);
    }
    aos_queue_send(&g_udata_queue, (void *)&msg, sizeof(msg));
    return 0;
}


int udata_new_servicetask(const char *name, void (*fn)(void *),void *arg,
                            int stack_size, int prio)
{
    int ret;
    int taskid;
    if(g_udata_own_task_cnt >= UDATA_MAX_OWN_TASK_NUM) return -1;
    if(name == NULL || fn ==NULL) return -1;

    ret = aos_queue_new(&g_udata_own_task_queue[g_udata_own_task_cnt], (void *)&g_udata_own_task_msg[g_udata_own_task_cnt],
            UDATA_QUEUE_SIZE,UDATA_QUEUE_MAX_MSG_SIZE);

    if (unlikely(ret)) {
        return -1;
    }
    ret = aos_task_new_ext(&g_udata_own_task[g_udata_own_task_cnt], name, fn,
                       arg, stack_size, prio);

    if (unlikely(ret)) {
        LOG("udata_new_servicetask error\n");
        return -1;
    }

    g_udata_own_task_cnt++;
    taskid = g_udata_own_task_cnt -1;
    return taskid;
}

int udata_observe_servicetask_tag(int taskid,sensor_tag_e tag, uint8_t instance)
{
    int i = 0;
    int ret;
    uint32_t index;

    if(taskid >= g_udata_own_task_cnt || taskid < 0)
    {
        return -1;
    }
    int actual_num = g_udata_own_task_tag[taskid].actual_num;
    if( actual_num >= UDATA_SERVICE_TAG_NUM)
    {
        return -1;
    }
    ret = abs_data_get_abs_index(tag, instance, &index);
    if(unlikely(ret)){
        return -1;
    }

    LOG("udata_set_servicetask_tag taskid=%d,actual_num=%d,tag=%d,g_udata_own_task_cnt=%d\n",taskid,actual_num,tag,g_udata_own_task_cnt);
    for(i=0; i < actual_num; i++)
    {
        if(g_udata_own_task_tag[taskid].index[i] == index)
        {
            return -1;
        }
    }
    g_udata_own_task_tag[taskid].index[actual_num] = index;
    g_udata_own_task_tag[taskid].actual_num++;
    LOG("udata_set_servicetask_tag sucessfull\n");
    return 0;
}

int udata_msg_recv(int task_id, unsigned int ms, void *msg,
                   unsigned int *size)
{
    int ret = 0;
    if(msg == NULL) return -1;
    if (task_id >= g_udata_own_task_cnt) return -1;
    ret = aos_queue_recv(&g_udata_own_task_queue[task_id], ms,(void *)msg, size);
    return ret;
}

int udata_start()
{
    int ret;
    memset(g_udata_queue_cb, 0,UDATA_QUEUE_MAXSLOTS * sizeof(udata_queue_cb_t));
    memset(g_udata_own_task_tag,0,UDATA_MAX_OWN_TASK_NUM * sizeof(servicetask_related_tag_t));

    // create the buf queue for the service dispatcher
    ret = aos_queue_new(&g_udata_queue, (void *)&g_udata_msg, UDATA_QUEUE_SIZE,
                        UDATA_QUEUE_MAX_MSG_SIZE);
    if (unlikely(ret)) {
        return -1;
    }

    // create the service dispatcher task
    ret = udata_msg_dispatcher_task_create();
    if (unlikely(ret)) {
        return -1;
    }
    return 0;
}
