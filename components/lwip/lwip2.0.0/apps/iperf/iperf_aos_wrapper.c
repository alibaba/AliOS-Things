/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "iperf_wrapper.h"
#include "aos/kernel.h"

#ifdef __cplusplus
extern "C" {
#endif

void iperf_wrapper_free(void * ptr)
{
    aos_free(ptr);
}

void *iperf_wrapper_malloc(unsigned int size)
{
    return aos_malloc(size);
}

#ifdef __cplusplus
};  /* extern "C" */
#endif
