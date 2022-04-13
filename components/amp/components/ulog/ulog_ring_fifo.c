/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "ulog_ring_fifo.h"
#include <string.h>
#include <stdio.h>
#include "ulog_config.h"

static void *ulog_queue;
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
        ulog_buffer = HAL_Malloc(DEFAULT_ASYNC_BUF_SIZE);
        if (ulog_buffer != NULL) {
            ulog_queue = HAL_MessageQCreate(ulog_buffer, ULOG_SIZE);
            if (NULL == ulog_queue) {
                HAL_Free(ulog_buffer);
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
    return HAL_MessageQPut(ulog_queue, (void*)buf, len, 0);
}

int uring_fifo_pop_cb(pop_callback cb, void* cb_arg)
{
    char tmp_buf[ULOG_SIZE];
    unsigned int rcv_size = 0;
    int rc = HAL_MessageQGet(ulog_queue, tmp_buf, &rcv_size, 1000);
    if ((0 == rc) && (cb != NULL)) {
        cb(cb_arg, tmp_buf, (uint16_t)rcv_size);
    }
    return rc;
}
