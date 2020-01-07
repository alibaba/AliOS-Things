/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <string.h>
#include <aos/kernel.h>
#include <hal/wifi.h>
#include <aos/yloop.h>

typedef void (*umesh_call_t)(void *arg);


int umesh_timer_start(void **timer, int ms,umesh_call_t cb, void *arg)
{
    int ret = aos_post_delayed_action(ms, cb, arg);
    if(timer != NULL && ret == 0) {
        *timer =  cb;
    }
    return ret;
}

int umesh_timer_cancel(void **timer, umesh_call_t cb, void *arg)
{
    aos_cancel_delayed_action(-1, cb, arg);
    if(timer != NULL ) {
        *timer = NULL;
    }
    return 0;
}

