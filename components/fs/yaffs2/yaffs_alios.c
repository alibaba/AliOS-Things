/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "yaffs_osglue.h"
#include "k_api.h"

#define YAFFS_BG_TASK_STACKSIZE    1024

static  kmutex_t yaffs_mutex;
static ktask_t yaffs_bg_task_obj;
cpu_stack_t yaffs_bg_task_buf[YAFFS_BG_TASK_STACKSIZE];

void yaffsfs_Lock(void)
{
    krhino_mutex_lock(&yaffs_mutex, RHINO_WAIT_FOREVER);
}

void yaffsfs_Unlock(void)
{
    krhino_mutex_unlock(&yaffs_mutex);
}

u32 yaffsfs_CurrentTime(void)
{
    return krhino_sys_time_get();
}

static int yaffsfs_lastError;

int yaffsfs_GetLastError(void)
{
    return yaffsfs_lastError;
}

void yaffsfs_SetError(int err)
{
    yaffsfs_lastError = err;
}

void *yaffsfs_malloc(size_t size)
{
    return krhino_mm_alloc(size);
}

void yaffsfs_free(void *ptr)
{
    krhino_mm_free(ptr);
}

int yaffsfs_CheckMemRegion(const void *addr, size_t size, int write_request)
{
    /* add code according to the hardware configuration */
    return 0;
}

static void bg_gc_func(void *dummy)
{
    struct yaffs_dev *dev;
    int urgent = 0;
    int result = 0;
    int next_urgent = 0;

    (void)dummy;

    /* Sleep for a bit to allow start up */
    krhino_task_sleep(2);

    while (1) {
        /* Iterate through devices, do bg gc updating ungency */
        yaffs_dev_rewind();
        next_urgent = 0;

        while ((dev = yaffs_next_dev()) != NULL) {
            result = yaffs_do_background_gc_reldev(dev, urgent);
            if (result > 0)
            next_urgent = 1;
        }

        urgent = next_urgent;

        if (next_urgent) {
            krhino_task_sleep(1);
        } else {
            krhino_task_sleep(5);
        }
    }
}

void yaffsfs_OSInitialisation(void)
{
    krhino_mutex_create(&yaffs_mutex, "mutex");

    krhino_task_create(&yaffs_bg_task_obj, "yaffs_bg_task", 0, 20, 
        50, yaffs_bg_task_buf, YAFFS_BG_TASK_STACKSIZE, bg_gc_func, 1);
}

void yaffs_bug_fn(const char *file_name, int line_no)
{
    printf("yaffs bug detected %s:%d\n", file_name, line_no);
}
