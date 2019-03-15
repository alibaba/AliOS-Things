/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "uring_fifo.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "aos/kernel.h"

static aos_queue_t ulog_queue;
static uint8_t*    ulog_buffer;


/**
* This function will create a ring fifo for ulog.
*
*
* @return  0: success.
*/
int uring_fifo_init()
{
    int rc = -1;
    if (ulog_buffer == NULL) {
        ulog_buffer = aos_malloc(DEFAULT_ASYNC_BUF_SIZE);
        if (ulog_buffer != NULL) {
            rc = aos_queue_new(&ulog_queue, ulog_buffer, DEFAULT_ASYNC_BUF_SIZE, SYSLOG_SIZE);
            if (0 != rc) {
                aos_free(ulog_buffer);
                ulog_buffer = NULL;
            }
        }
    }

    return rc;
}

/**
* Thread Safe to put the msg into ring - fifo.
*
* @param[in]  queue  pointer to the queue.
* @param[in]  msg    msg to send.
* @param[in]  size   size of the msg.
*
* @return  0: success.
*/
int uring_fifo_push_s(const void* buf, const uint16_t len)
{
    return aos_queue_send(&ulog_queue, (void*)buf, len);
}

int uring_fifo_pop_cb(pop_callback cb, void* cb_arg)
{
    char tmp_buf[SYSLOG_SIZE];
    unsigned int rcv_size = 0;
    int rc = aos_queue_recv(&ulog_queue, AOS_WAIT_FOREVER, tmp_buf, &rcv_size);
    if ((0 == rc) && (cb != NULL)) {
        cb(cb_arg, tmp_buf, (uint16_t)rcv_size);
    }
    return rc;
}
