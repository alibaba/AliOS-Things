/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "iperf_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void *rt_malloc(int size);
extern void rt_free(void *rmem);

void iperf_wrapper_free(void * ptr)
{
    rt_free(ptr);
}

void *iperf_wrapper_malloc(unsigned int size)
{
    return rt_malloc(size);
}

#ifdef __cplusplus
};  /* extern "C" */
#endif
