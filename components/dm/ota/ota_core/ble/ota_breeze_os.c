/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ota_breeze_plat.h"
#include "aos/kernel.h"

#define OTA_BREEZE_THREAD_NAME "OTA_Thread"
#define OTA_BREEZE_THREAD_SIZE 4096
#define OTA_BREEZE_THREAD_PRI  30

void ota_breeze_reboot(void)
{
    aos_reboot();
}

/*Memory malloc*/
void *ota_breeze_malloc(int size)
{
    return aos_malloc(size);
}

/*Memory free*/
void ota_breeze_free(void *ptr)
{
    aos_free(ptr);
}

/*Sleep ms*/
void ota_breeze_msleep(int ms)
{
    aos_msleep(ms);
}

typedef struct
{
    aos_task_t task;
    int        detached;
    void       *arg;
    void       *(*routine)(void *arg);
} task_context_t;

static void task_wrapper(void *arg)
{
    task_context_t *task = arg;
    if(task != NULL) {
        task->routine(task->arg);
        if (task != NULL) {
            aos_free(task);
            task = NULL;
        }
    }
}

/*Thread create*/
int ota_breeze_thread_create(void **thread_handle, void *(*work_routine)(void *), void *arg, void *pm, int stack_size)
{
    int ret     = -1;
    char *tname = OTA_BREEZE_THREAD_NAME;
    if(stack_size <= 0) {
        stack_size = OTA_BREEZE_THREAD_SIZE;
    }
    task_context_t *task = aos_malloc(sizeof(task_context_t));
    if (task == NULL) {
        return -1;
    }
    memset(task, 0, sizeof(task_context_t));
    task->arg      = arg;
    task->routine  = work_routine;

    ret = aos_task_new_ext(&task->task, tname, task_wrapper, task, stack_size, OTA_BREEZE_THREAD_PRI);
    *thread_handle = (void *)task;
    return ret;
}

