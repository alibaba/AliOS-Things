/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <errno.h>
#include <aos/cloud.h>
#include <aos/aos.h>

static aos_cloud_cb_t cbs[MAX_EVENT_TYPE];
static int (*report_backend)(const char *method, const char *json_buffer);

int aos_cloud_register_callback(int cb_type, aos_cloud_cb_t cb)
{
    if (cb_type >= MAX_EVENT_TYPE) {
        return -EINVAL;
    }

    cbs[cb_type] = cb;
    return 0;
}

int aos_cloud_report(const char *method,
                     const char *json_buffer,
                     void (*done_cb)(void *),
                     void *arg)
{
    if (report_backend == NULL) {
        return -ENOSYS;
    }

    return report_backend(method, json_buffer);
}

void aos_cloud_register_backend(int (*report)(const char *method, const char *json_buffer))
{
    report_backend = report;
}

void aos_cloud_trigger(int cb_type, const char *json_buffer)
{
    if (cb_type >= MAX_EVENT_TYPE) {
        return;
    }

    cbs[cb_type](cb_type, json_buffer);
}

int aos_cloud_init(void)
{
    return 0;
}

