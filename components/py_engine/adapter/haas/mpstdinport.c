/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>

#include "aos_hal_uart.h"
#include "mpsalport.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "ulog/ulog.h"

#define LOG_TAG "MP_UART_PORT"

STATIC uart_dev_t s_repl_uart = { 0 };
STATIC aos_task_t uart_recv_task = NULL;
STATIC mp_sal_sem_obj_t stdin_sem = NULL;

STATIC mp_int_t py_uart_rx_notify(void *buf, uint16_t size)
{
    uint8_t *data = (uint8_t *)buf;
    for (uint16_t index = 0; index < size; index++) {
        uint8_t c = *(data++);
        if (c == mp_interrupt_char) {
            mp_sched_keyboard_interrupt();
        } else {
            ringbuf_put(&stdin_ringbuf, c);
        }
    }
    mp_sal_sem_give(&stdin_sem);
}

STATIC void uart_recv_mainloop(void *args)
{
    uart_dev_t uart = s_repl_uart;
    uint8_t recv_byte[1] = { 0 };
    mp_uint_t recv_size;
    int ret;

    while (1) {
        ret = aos_hal_uart_recv_II(&uart, recv_byte, sizeof(recv_byte),
                                   &recv_size, HAL_WAIT_FOREVER);
        if (ret != 0 || recv_size == 0) {
            continue;
        }

        py_uart_rx_notify(recv_byte, recv_size);
    }

    aos_task_exit(0);
}

STATIC mp_int_t uart_recv_task_init()
{
    mp_int_t status = -1;

    if (uart_recv_task == NULL) {
        status = aos_task_new_ext(&uart_recv_task, "_uart_recv",
                                  uart_recv_mainloop, NULL, 1024, 32);
        if (status != 0) {
            LOGE(LOG_TAG, "Failed to create task[_uart_recv], status=%d\n",
                 status);
            return status;
        }
    } else {
        status = aos_task_resume(&uart_recv_task);
    }

    return status;
}

STATIC mp_int_t uart_recv_task_deinit()
{
    return aos_task_suspend(&uart_recv_task);
}

uart_dev_t mp_stdin_dev_get()
{
    return s_repl_uart;
}

mp_int_t mp_stdin_init(uint8_t port, mp_uint_t baud_rate)
{
    int ret = uart_recv_task_init();
    if (ret != 0) {
        LOGE(LOG_TAG, "Failed to register cb to uart recv");
        return ret;
    }

    ret = mp_sal_sem_create(&stdin_sem);
    if (ret != 0) {
        LOGE(LOG_TAG, "Failed to create sem for hal stdin");
        return ret;
    }

    return ret;
}

mp_int_t mp_stdin_deinit()
{
    memset(stdin_ringbuf.buf, 0, stdin_ringbuf.size);
    stdin_ringbuf.iget = 0;
    stdin_ringbuf.iput = 0;

    uart_recv_task_deinit();
    mp_sal_sem_delete(&stdin_sem);
}

mp_int_t mp_stdin_sem_give()
{
    return mp_sal_sem_give(&stdin_sem);
}

mp_int_t mp_stdin_sem_take(mp_uint_t timeout)
{
    return mp_sal_sem_take(&stdin_sem, timeout);
}
