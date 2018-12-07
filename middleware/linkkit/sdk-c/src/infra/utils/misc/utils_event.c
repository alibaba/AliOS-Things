/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifdef UTILS_EVENT

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static void *g_event_monitor = NULL;

int iotx_event_regist_cb(void (*monitor_cb)(int event))
{
    g_event_monitor = monitor_cb;
    return 0;
}

int iotx_event_post(int event)
{
    if (g_event_monitor == NULL)
        return -1;
    ((void (*)(int))g_event_monitor)(event);
    return 0;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif