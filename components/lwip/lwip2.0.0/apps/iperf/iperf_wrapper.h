/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef IPERF_COMMON_H
#define IPERF_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

void iperf_wrapper_free(void * ptr);
void *iperf_wrapper_malloc(unsigned int size);

#ifdef __cplusplus
};  // extern "C"
#endif
#endif
