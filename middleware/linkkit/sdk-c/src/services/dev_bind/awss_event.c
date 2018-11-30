/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "iot_import.h"
#include "iot_export.h"
#include "sdk-impl_internal.h"

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
extern "C"
{
#endif

int awss_event_post(int event)
{
    int ret = 0;
    void *cb = NULL;

    ret = iotx_event_post(event);

    cb = (void *)iotx_event_callback(ITE_AWSS_STATUS);
    if (cb)
        ret = ((int (*)(int))cb)(event);

    return ret;
}

#if defined(__cplusplus)  /* If this is a C++ compiler, use C linkage */
}
#endif
