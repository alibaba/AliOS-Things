/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "ulog_ring_fifo.h"
#include <string.h>
#include <stdio.h>
#include "aos/kernel.h"
#include "ulog_config.h"
#include "k_api.h"

static aos_queue_t ulog_queue;
//static uint8_t*    ulog_buffer;

extern int uart_puts(const char *string);

/**
* This function will create a ring fifo for ulog.
*
*
* @return  0: success.
*/
int uring_fifo_init()
{
    int rc = -1;

    rc = aos_queue_new(&ulog_queue, DEFAULT_ASYNC_BUF_SIZE, ULOG_SIZE);

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
    char tmp_buf[ULOG_SIZE];
    unsigned int rcv_size = 0;
    int rc = aos_queue_recv(&ulog_queue, AOS_WAIT_FOREVER, tmp_buf, &rcv_size);
    if ((0 == rc) && (cb != NULL)) {
        cb(cb_arg, tmp_buf, (uint16_t)rcv_size);
    }
    return rc;
}

/* flush ulog fifo when panic*/
void uring_fifo_flush(void)
{
    int  rc;
    char log_text[ULOG_SIZE];

    unsigned int i, msg_num_in_fifo, rcv_size;

    msg_num_in_fifo = ((kbuf_queue_t *)(ulog_queue.hdl))->cur_num;

    for(i = 0; i < msg_num_in_fifo; i++) {
        memset(log_text, 0, ULOG_SIZE);
        rc = aos_queue_recv(&ulog_queue, RHINO_NO_WAIT, (void *)log_text, &rcv_size);
        if (rc == 0) {
            uart_puts(&((char*)log_text)[LOG_PREFIX_LEN]);
        }
    }
}
