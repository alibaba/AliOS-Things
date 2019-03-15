/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef U_RING_FIFO_H_
#define U_RING_FIFO_H_

#include "ulog_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*pop_callback)(void*, const void* src, const uint16_t len);

extern int uring_fifo_push_s(const void* buf, const uint16_t len);

extern int uring_fifo_pop_cb(pop_callback cb, void* cb_arg);

extern int uring_fifo_init(void);
#ifdef __cplusplus
}
#endif

#endif /*U_RING_FIFO_H_*/
