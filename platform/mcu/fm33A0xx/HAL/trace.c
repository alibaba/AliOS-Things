/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef HAL_TRACE_H
#define HAL_TRACE_H

/**
 * trace data transfer init.
 */
void *trace_hal_init(void);

/**
 * trace data transfer send.
 *
 * @param[in]  handle  data transfer channel object
 * @param[in]  buf     the buffer store data
 * @param[in]  len     the len of data
 *
 * @return  the size send success.
 */
ssize_t trace_hal_send(void *handle, void *buf, size_t len);

/**
 * trace data transfer receive.
 *
 * @param[in]  handle  data transfer channel object
 * @param[in]  buf     the buffer to store data
 *
 * @return  the size receive success.
 */
ssize_t trace_hal_recv(void *handle, void *buf);

/**
 * trace data transfer init.
 *
 * @param[in]  handle  data transfer channel object
 */
void trace_hal_deinit(void *handle);

#endif /* HAL_TRACE_H */

