/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <aos/log.h>
#include "atparser.h"

#define MODULE_NAME "atparser"

#define MODULE_NAME "atparser"

static void at_worker(void *arg);

static void at_init_uart(uart_dev_t *u)
{
    at._uart = *u;
    hal_uart_init(u);
}

static void at_set_timeout(int timeout)
{
    at._timeout = timeout;
}

static void at_set_delimiter(const char *delimiter)
{
    at._recv_delimiter = (char *)delimiter;
    at._send_delimiter = (char *)delimiter;
    at._recv_delim_size = strlen(delimiter);
    at._send_delim_size = strlen(delimiter);
}

static void at_set_recv_delimiter(const char *delimiter)
{
    at._recv_delimiter = (char *)delimiter;
    at._recv_delim_size = strlen(delimiter);
}

static void at_set_send_delimiter(const char *delimiter)
{
    at._send_delimiter = (char *)delimiter;
    at._send_delim_size = strlen(delimiter);
}

static int at_init_mutex()
{
    if (0 != aos_mutex_new(&at._mutex)) {
        LOGE(MODULE_NAME, "Creating mutex failed");
        return -1;
    }

    return 0;
}

static void at_init(uart_dev_t *u, const char *recv_delimiter,
                    const char *send_delimiter, int timeout)
{
    if (!u || !recv_delimiter || !send_delimiter || (timeout < 0)) {
        LOGE(MODULE_NAME, "%s: invalid argument", __func__);
        return;
    }

    at_init_uart(u);
    at_set_timeout(timeout);
    at_set_recv_delimiter(recv_delimiter);
    at_set_send_delimiter(send_delimiter);
    if (at._mode == ASYN) {
        at_init_mutex();
        slist_init(&at.task_l);
        aos_task_new("at_worker", at_worker, NULL, 4096);
    }
}

static void at_set_mode(at_mode_t m)
{
    at._mode = m;
}

static int at_putc(char c)
{
    LOGD(MODULE_NAME, "uart sending %c(0x%02x)", c, c);
    return hal_uart_send(&at._uart, (void *)&c, 1, at._timeout);
}

static int at_getc()
{
    char data;
    uint32_t recv_size;

    if (hal_uart_recv(&at._uart, (void *)&data, 1,
                      &recv_size, at._timeout) != 0) {
        //LOGD(MODULE_NAME, "uart get failed or timeout.");
        return -1;
    }
    LOGD(MODULE_NAME, "uart get returned (size: %d, data: %c(0x%02x))",
         recv_size, data, data);
    return data;
}

static int at_write(const char *data, int size)
{
    int i = 0;
    for ( ; i < size; i++) {
        if (at_putc(data[i]) < 0) {
            return -1;
        }
    }
    return i;
}

static int at_read(char *data, int size)
{
    int i = 0;
    for ( ; i < size; i++) {
        int c = at_getc();
        if (c < 0) {
            return -1;
        }
        data[i] = c;
    }
    return i;
}

static bool at_vsend(const char *command, va_list args)
{
    char *_buffer = at._buffer;
    char *_send_delimiter = at._send_delimiter;

    if (vsprintf(_buffer, command, args) < 0) {
        return false;
    }
    for (int i = 0; _buffer[i]; i++) {
        if (at_putc(_buffer[i]) < 0) {
            return false;
        }
    }

    // Finish with newline
    for (int i = 0; _send_delimiter[i]; i++) {
        if (at_putc(_send_delimiter[i]) < 0) {
            return false;
        }
    }

    LOGD(MODULE_NAME, "AT> %s\r\n", _buffer);
    return true;
}

static bool at_vrecv(const char *response, va_list args)
{
    char *_buffer = at._buffer;
    char *_recv_delimiter = at._recv_delimiter;

vrecv_start:
    while (response[0]) {
        int i = 0;
        int offset = 0;

        while (response[i]) {
            if (memcmp(&response[i + 1 - at._recv_delim_size], _recv_delimiter,
                       at._recv_delim_size) == 0) {
                i++;
                break;
            } else if (response[i] == '%' && response[i + 1] != '%' &&
                       response[i + 1] != '*') {
                _buffer[offset++] = '%';
                _buffer[offset++] = '*';
                i++;
            } else {
                _buffer[offset++] = response[i++];
            }
        }

        _buffer[offset++] = '%';
        _buffer[offset++] = 'n';
        _buffer[offset++] = 0;

        int j = 0;

        while (true) {
            int c = at_getc();
            if (c < 0) {
                return false;
            }
            _buffer[offset + j++] = c;
            _buffer[offset + j] = 0;

            for (int k = 0; k < at._oobs_num; k++) {
                oob_t *oob = &(at._oobs[k]);
                if (j == oob->len &&
                    memcmp(oob->prefix, at._buffer + offset, oob->len) == 0) {
                    LOGD(MODULE_NAME, "AT! %s\r\n", oob->prefix);
                    oob->cb(oob->arg);

                    goto vrecv_start;
                }
            }

            int count = -1;
            sscanf(_buffer + offset, _buffer, &count);

            if (count == j) {
                LOGD(MODULE_NAME, "AT= %s\r\n", _buffer + offset);
                memcpy(_buffer, response, i);
                _buffer[i] = 0;
                vsscanf(_buffer + offset, _buffer, args);
                response += i;
                break;
            }

            if (j + 1 >= BUFFER_SIZE - offset ||
                strcmp(&_buffer[offset + j - at._recv_delim_size],
                       _recv_delimiter) == 0) {
                LOGD(MODULE_NAME, "AT< %s", _buffer + offset);
                j = 0;
            }
        }
    }

    return true;
}

static bool at_send(const char *command, ...)
{
    if (at._mode != NORMAL) {
        LOGE(MODULE_NAME, "Operation not supported in non normal mode.");
        return false;
    }

    va_list args;
    va_start(args, command);
    bool res = at_vsend(command, args);
    va_end(args);
    return res;
}

static int at_send_raw(const char *command, char *rsp)
{
    int ret = 0;

    if (at._mode != ASYN) {
        LOGE(MODULE_NAME, "Operation not supported in non asyn mode");
        return -1;
    }

    at_task_t *tsk = (at_task_t *)aos_malloc(sizeof(at_task_t));
    if (NULL == tsk) {
        LOGE(MODULE_NAME, "tsk buffer allocating failed");
        return -1;
    }

    if ((ret = aos_sem_new(&tsk->smpr, 0)) != 0) {
        LOGE(MODULE_NAME, "failed to allocate semaphore");
        goto end;
    }

    LOGD(MODULE_NAME, "at task created: %d, smpr: %d",
         (uint32_t)tsk, (uint32_t)&tsk->smpr);

    tsk->rsp = rsp;
    tsk->rsp_offset = 0;

    aos_mutex_lock(&at._mutex, AOS_WAIT_FOREVER);
    slist_add_tail(&tsk->next, &at.task_l);

    // uart operation should be inside mutex lock
    if ((ret = hal_uart_send(&at._uart, (void *)command,
                             strlen(command), at._timeout)) != 0) {
        aos_mutex_unlock(&at._mutex);
        LOGE(MODULE_NAME, "uart send command failed");
        goto end;
    }
    LOGD(MODULE_NAME, "Sending command %s", command);

    if ((ret = hal_uart_send(&at._uart, (void *)at._send_delimiter,
                             strlen(at._send_delimiter), at._timeout)) != 0) {
        aos_mutex_unlock(&at._mutex);
        LOGE(MODULE_NAME, "uart send delimiter failed");
        goto end;
    }
    LOGD(MODULE_NAME, "Sending delimiter %s", at._send_delimiter);

    aos_mutex_unlock(&at._mutex);

    if ((ret = aos_sem_wait(&tsk->smpr, AOS_WAIT_FOREVER)) != 0) {
        LOGE(MODULE_NAME, "sem_wait failed");
        goto end;
    }

    LOGD(MODULE_NAME, "sem_wait succeed.");

end:
    aos_mutex_lock(&at._mutex, AOS_WAIT_FOREVER);
    slist_del(&tsk->next, &at.task_l);
    aos_mutex_unlock(&at._mutex);
    if (aos_sem_is_valid(&tsk->smpr)) {
        aos_sem_free(&tsk->smpr);
    }
    if (tsk) {
        aos_free(tsk);
    }
    return  ret;
}

static bool at_recv(const char *response, ...)
{
    if (at._mode != NORMAL) {
        LOGE(MODULE_NAME, "Operation not supported in non normal mode.");
        return false;
    }

    va_list args;
    va_start(args, response);
    bool res = at_vrecv(response, args);
    va_end(args);
    return res;
}

// register oob
static void at_oob(const char *prefix, oob_cb cb, void *arg)
{
    oob_t *oob;

    LOGD(MODULE_NAME, "New oob to register (%s)", prefix);

    if (at._oobs_num >= OOB_MAX) {
        LOGW(MODULE_NAME, "No place left in OOB.");
        return;
    }

    oob = &(at._oobs[at._oobs_num++]);
    oob->len = strlen(prefix);
    if (oob->len > PREFIX_MAX - 1) {
        LOGW(MODULE_NAME, "Prefix too long.");
        at._oobs_num--;
        return;
    }
    memcpy(oob->prefix, prefix, oob->len + 1);
    oob->cb = cb;
    oob->arg = arg;

    LOGD(MODULE_NAME, "New oob registered (%s)", oob->prefix);
}

#define RECV_BUFFER_SIZE 256
static void at_worker(void *arg)
{
    char buf[RECV_BUFFER_SIZE] = {0};
    int offset = 0, c;
    int at_task_empty = 0;

    LOGD(MODULE_NAME, "at_work started.");
    at_set_timeout(500);

    while (true) {
recv_start:
        // read from uart and store buf
        c = at_getc();
        if (c < 0) {
            continue;
        }

        if (offset + 1 >= RECV_BUFFER_SIZE) {
            LOGE(MODULE_NAME, "Fatal error, no one is handling AT uart");
            assert(0);
        }
        buf[offset++] = c;
        buf[offset] = 0;

        // check oob first
        for (int k = 0; k < at._oobs_num; k++) {
            oob_t *oob = &(at._oobs[k]);
            if (offset == oob->len &&
                memcmp(oob->prefix, buf, oob->len) == 0) {
                LOGD(MODULE_NAME, "AT! %s\r\n", oob->prefix);
                // oob.cb is to consume uart data if necessary
                oob->cb(oob->arg);
                // start a new round after oob cb
                offset = 0;
                goto recv_start;
            }
        }

        aos_mutex_lock(&at._mutex, AOS_WAIT_FOREVER);
        at_task_empty = slist_empty(&at.task_l);
        aos_mutex_unlock(&at._mutex);
        // if no task, continue recv
        if (at_task_empty) {
            goto check_buffer;
        }

        // otherwise, get the first task in list
        at_task_t *tsk = slist_first_entry(&at.task_l, at_task_t, next);
        aos_mutex_unlock(&at._mutex);

        // check if a rsp end matched
        if (strcmp(buf + offset - strlen(RECV_STATUS_OK), RECV_STATUS_OK) == 0 ||
            strcmp(buf + offset - strlen(RECV_STATUS_ERROR), RECV_STATUS_ERROR) == 0) {
            LOGD(MODULE_NAME, "AT cammand rsp matched");
            LOGD(MODULE_NAME, "at task is going to be waked up: %d, smpr: %d",
                 (uint32_t)tsk, (uint32_t)&tsk->smpr);
            memcpy(tsk->rsp + tsk->rsp_offset, buf, offset);
            tsk->rsp_offset += offset;
            aos_sem_signal(&tsk->smpr); // wakeup send task
            // start a new round after a match hit
            offset = 0; // clear buffer
            goto recv_start;
        }

check_buffer:
        // in case buffer is full
        if ((offset >= (RECV_BUFFER_SIZE - 1)) ||
            (strcmp(&buf[offset - at._recv_delim_size], at._recv_delimiter) == 0)) {
            LOGD(MODULE_NAME, "buffer full or new line hit, offset: %d", offset);
            if (!at_task_empty) {
                memcpy(tsk->rsp + tsk->rsp_offset, buf, offset);
                tsk->rsp_offset += offset;
            }
            offset = 0;
            goto recv_start;
        }
    }

    // never return
    return;
}

at_parser_t at = {
    ._oobs = {{0}},
    ._oobs_num = 0,
    ._mode = ASYN, // default mode - atworker
    .init = at_init,
    .set_mode = at_set_mode,
    .set_timeout = at_set_timeout,
    .set_delimiter = at_set_delimiter,
    .set_recv_delimiter = at_set_recv_delimiter,
    .set_send_delimiter = at_set_send_delimiter,
    .send = at_send,
    .vsend = at_vsend,
    .send_raw = at_send_raw,
    .recv = at_recv,
    .vrecv = at_vrecv,
    .putc = at_putc,
    .getc = at_getc,
    .write = at_write,
    .read = at_read,
    .oob = at_oob
};
