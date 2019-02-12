/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "aos/kernel.h"
#include "ulog/ulog.h"

#include "aos/hal/uart.h"

#include "atparser.h"
#include "atparser_internal.h"

#ifdef CENTRALIZE_MAPPING
#include "board.h"
#endif

#ifdef HDLC_UART
#include "hdlc.h"
#endif

#define TAG            "atparser"

#define atpsr_debug(format, ...)  LOGD(TAG, format, ##__VA_ARGS__)
#define atpsr_err(format, ...)    LOGE(TAG, format, ##__VA_ARGS__)

static uint8_t    inited = 0;
static uart_dev_t at_uart;

#ifdef HDLC_UART
static encode_context_t hdlc_encode_ctx;
static decode_context_t hdlc_decode_ctx;
#endif

static at_parser_t at;

#if !ATPSR_SINGLE_TASK
static void at_worker(void *arg);
#endif

static void at_uart_configure(uart_dev_t *u)
{
    u->port                = AT_UART_PORT;
    u->config.baud_rate    = AT_UART_BAUDRATE;
    u->config.data_width   = AT_UART_DATA_WIDTH;
    u->config.parity       = AT_UART_PARITY;
    u->config.stop_bits    = AT_UART_STOP_BITS;
    u->config.flow_control = AT_UART_FLOW_CONTROL;
    u->config.mode         = AT_UART_MODE;
}

static int at_init_uart()
{
    at_uart_configure(&at_uart);

    if (hal_uart_init(&at_uart) != 0) {
        return -1;
    }

#ifdef HDLC_UART
    if (hdlc_encode_context_init(&hdlc_encode_ctx) != 0 ||
        hdlc_decode_context_init(&hdlc_decode_ctx, &at_uart) != 0) {
        return -1;
    }
#endif

    at._pstuart = &at_uart;

    return 0;
}

static void at_set_timeout(int timeout)
{
    at._timeout = timeout;
}

static void at_set_recv_delimiter(const char *recv_prefix,
                                  const char *recv_success_postfix,
                                  const char *recv_fail_postfix)
{
    at._default_recv_prefix          = (char *)recv_prefix;
    at._default_recv_success_postfix = (char *)recv_success_postfix;
    at._default_recv_fail_postfix    = (char *)recv_fail_postfix;
    at._recv_prefix_len              = strlen(recv_prefix);
    at._recv_success_postfix_len     = strlen(recv_success_postfix);
    at._recv_fail_postfix_len        = strlen(recv_fail_postfix);
}

static void at_set_send_delimiter(const char *delimiter)
{
    at._send_delimiter  = (char *)delimiter;
    at._send_delim_size = strlen(delimiter);
}

static int at_init_task_mutex()
{
    at.task_mutex = atpsr_mutex_new();
    if (NULL == at.task_mutex) {
        atpsr_err("Creating task mutex failed\r\n");
        return -1;
    }

    return 0;
}

static void at_deinit_task_mutex()
{
    atpsr_mutex_free(at.task_mutex);
}

static int at_init_uart_recv_mutex()
{
    at.at_uart_recv_mutex = atpsr_mutex_new();
    if (NULL == at.at_uart_recv_mutex) {
        atpsr_err("Creating at_uart_recv_mutex failed\r\n");
        return -1;
    }

    return 0;
}

static void at_deinit_uart_recv_mutex()
{
    atpsr_mutex_free(at.at_uart_recv_mutex);
}

static int at_worker_uart_send_mutex_init()
{
    at.at_uart_send_mutex = atpsr_mutex_new();
    if (NULL == at.at_uart_send_mutex) {
        atpsr_err("Creating uart send mutex failed\r\n");
        return -1;
    }
    return 0;
}

#if !ATPSR_SINGLE_TASK
static void at_worker_uart_send_mutex_deinit()
{
    atpsr_mutex_free(at.at_uart_send_mutex);
}
#endif

int at_init(void)
{
#if !ATPSR_SINGLE_TASK
    void  *task;
    slist_init(&at.task_l);
#endif

    char *recv_prefix = AT_RECV_PREFIX;
    char *recv_success_postfix = AT_RECV_SUCCESS_POSTFIX;
    char *recv_fail_postfix = AT_RECV_FAIL_POSTFIX;
    char *send_delimiter = AT_SEND_DELIMITER;

    int  timeout = AT_UART_TIMEOUT_MS;

    if (inited == 1) {
        atpsr_debug("have already inited ,it will init again\r\n");
        return -1;
    }

    memset(&at, 0, sizeof(at_parser_t));

    if (at_init_uart() != 0) {
        atpsr_err("at uart init fail \r\n");
        return -1;
    }

    memset(at._oobs, 0, sizeof(oob_t) * OOB_MAX);

    at_set_timeout(timeout);
    at_set_recv_delimiter(recv_prefix, recv_success_postfix, recv_fail_postfix);
    at_set_send_delimiter(send_delimiter);

    if (at_init_uart_recv_mutex() != 0) {
        atpsr_err("at_uart_recv_mutex init fail \r\n");
        return -1;
    }

    if (at_init_task_mutex() != 0) {
        at_deinit_uart_recv_mutex();
        atpsr_err("at mutex init fail \r\n");
        return -1;
    }

    if (at_worker_uart_send_mutex_init() != 0) {
        at_deinit_uart_recv_mutex();
        at_deinit_task_mutex();
        atpsr_err("fail to creat at worker sem\r\n");
    }

#if ATPSR_SINGLE_TASK
    inited = true;
#else
    if (atpsr_task_new_ext(&task, "at_worker", at_worker,
                           NULL, AT_WORKER_STACK_SIZE,
                           AT_WORKER_PRIORITY) != 0) {
        at_deinit_uart_recv_mutex();
        at_deinit_task_mutex();
        at_worker_uart_send_mutex_deinit();
        atpsr_err("fail to creat at task\r\n");
        return -1;
    }
#endif

    return 0;
}

static int at_sendto_lower(uart_dev_t *uart, void *data, uint32_t size,
                           uint32_t timeout, bool ackreq)
{
    int ret = -1;

#ifdef HDLC_UART
    ret = hdlc_uart_send(&hdlc_encode_ctx, uart, data,
                         size, timeout, ackreq);
#else
    (void) ackreq;
    ret = hal_uart_send(uart, data, size, timeout);
#endif

    return ret;
}

static int at_recvfrom_lower(uart_dev_t *uart, void *data, uint32_t expect_size,
                             uint32_t *recv_size, uint32_t timeout)
{
    int ret = -1;

#ifdef HDLC_UART
    ret = hdlc_uart_recv(&hdlc_decode_ctx, uart, data, expect_size,
                         recv_size, timeout);
#else
    ret = hal_uart_recv_II(uart, data, expect_size, recv_size, timeout);
#endif

    return ret;
}

int at_send_no_reply(const char *data, int datalen, bool delimiter)
{
    int ret;

    if (inited == 0) {
        atpsr_err("at have not init yet\r\n");
        return -1;
    }

    if (NULL == data || datalen <= 0) {
        atpsr_err("invalid input \r\n");
        return -1;
    }

    atpsr_mutex_lock(at.at_uart_send_mutex);
    if ((ret = at_sendto_lower(at._pstuart, (void *)data,
                               datalen, at._timeout, true)) != 0) {
        atpsr_err("uart send raw content (%s) failed", data);
        atpsr_mutex_unlock(at.at_uart_send_mutex);
        return -1;
    }

    if (delimiter) {
        if ((ret = at_sendto_lower(at._pstuart, (void *)at._send_delimiter,
                    strlen(at._send_delimiter), at._timeout, false)) != 0) {
            atpsr_err("uart send delimiter failed");
            atpsr_mutex_unlock(at.at_uart_send_mutex);
            return -1;
        }
    }
    atpsr_mutex_unlock(at.at_uart_send_mutex);

    return 0;
}

static int at_getc(char *c, int timeout_ms)
{
    int      ret = 0;
    char     data;
    uint32_t recv_size = 0;

    if (NULL == c) {
        return -1;
    }

    if (inited == 0) {
        atpsr_err("at have not init yet\r\n");
        return -1;
    }

    atpsr_mutex_lock(at.at_uart_recv_mutex);
    ret = at_recvfrom_lower(at._pstuart, (void *)&data, 1, &recv_size, timeout_ms);
    atpsr_mutex_unlock(at.at_uart_recv_mutex);

    if (ret != 0) {
#ifdef WORKAROUND_DEVELOPERBOARD_DMA_UART
        if (ret == 1) {
            atpsr_debug("--->AT dma fail, restart!\n");
            hal_uart_finalize(at._pstuart);
            at_init_uart();
            atpsr_debug("<----AT dma fail, restart!\n");
        }
#endif
        return -1;
    }

    if (recv_size == 1) {
        *c = data;
        return 0;
    } else {
        return -1;
    }
}

int at_read(char *outbuf, int readsize)
{
    int      ret = 0;
    uint32_t recv_size, total_read = 0;

    if (inited == 0) {
        atpsr_err("at have not init yet\r\n");
        return -1;
    }

    atpsr_mutex_lock(at.at_uart_recv_mutex);
    while (total_read < readsize) {
        ret = at_recvfrom_lower(at._pstuart, (void *)(outbuf + total_read),
                                readsize - total_read, &recv_size, at._timeout);
        if (ret != 0) {
            atpsr_err("at_read failed on uart_recv.");
            break;
        }

        if (recv_size <= 0) {
            continue;
        }
        total_read += recv_size;
        if (total_read >= readsize) {
            break;
        }
    }
    atpsr_mutex_unlock(at.at_uart_recv_mutex);

    if (ret != 0) {
        return -1;
    }

    return total_read;
}

#if ATPSR_SINGLE_TASK
int at_send_wait_reply(const char *cmd, int cmdlen, bool delimiter,
                       const char *data, int datalen,
                       char *replybuf, int bufsize,
                       const atcmd_config_t *atcmdconfig)
{
    if (at_send_no_reply(cmd, cmdlen, delimiter) < 0) {
        return -1;
    }

    if (data && datalen) {
        if (at_send_no_reply(data, datalen, false) < 0) {
            return -1;
        }
    }

    if (at_yield(replybuf, bufsize, atcmdconfig, at._timeout) <  0) {
        return -1;
    }

    return 0;
}
#else
static int at_worker_task_add(at_task_t *tsk)
{
    if (NULL == tsk) {
        atpsr_err("invalid input %s \r\n", __func__);
        return -1;
    }

    atpsr_mutex_lock(at.task_mutex);
    slist_add_tail(&tsk->next, &at.task_l);
    atpsr_mutex_unlock(at.task_mutex);

    return 0;
}

static int at_worker_task_del(at_task_t *tsk)
{
    if (NULL == tsk) {
        atpsr_err("invalid input %s \r\n", __func__);
        return -1;
    }

    atpsr_mutex_lock(at.task_mutex);
    slist_del(&tsk->next, &at.task_l);
    atpsr_mutex_unlock(at.task_mutex);

    if (tsk->smpr != NULL) {
        atpsr_sem_free(tsk->smpr);
    }
    if (tsk) {
        atpsr_free(tsk);
    }

    return 0;
}

int at_send_wait_reply(const char *cmd, int cmdlen, bool delimiter,
                       const char *data, int datalen,
                       char *replybuf, int bufsize,
                       const atcmd_config_t *atcmdconfig)
{
    int ret = 0;

    if (inited == 0) {
        atpsr_err("at have not init yet\r\n");
        return -1;
    }

    if (NULL == cmd || cmdlen <= 0) {
        atpsr_err("%s invalid input \r\n", __FUNCTION__);
        return -1;
    }

    if (NULL == replybuf || 0 == bufsize) {
        atpsr_err("%s invalid input \r\n", __FUNCTION__);
        return -1;
    }

    atpsr_mutex_lock(at.at_uart_send_mutex);
    at_task_t *tsk = (at_task_t *)atpsr_malloc(sizeof(at_task_t));
    if (NULL == tsk) {
        atpsr_err("tsk buffer allocating failed");
        atpsr_mutex_unlock(at.at_uart_send_mutex);
        return -1;
    }
    memset(tsk, 0, sizeof(at_task_t));

    if (NULL == (tsk->smpr = atpsr_sem_new())) {
        atpsr_err("failed to allocate semaphore");
        goto end;
    }

    if (atcmdconfig) {
        if (NULL != atcmdconfig->reply_prefix) {
            tsk->rsp_prefix     = atcmdconfig->reply_prefix;
            tsk->rsp_prefix_len = strlen(atcmdconfig->reply_prefix);
        }

        if (NULL != atcmdconfig->reply_success_postfix) {
            tsk->rsp_success_postfix     = atcmdconfig->reply_success_postfix;
            tsk->rsp_success_postfix_len = strlen(atcmdconfig->reply_success_postfix);
        }

        if (NULL != atcmdconfig->reply_fail_postfix) {
            tsk->rsp_fail_postfix     = atcmdconfig->reply_fail_postfix;
            tsk->rsp_fail_postfix_len = strlen(atcmdconfig->reply_fail_postfix);
        }
    }

    tsk->command = (char *)cmd;
    tsk->rsp     = replybuf;
    tsk->rsp_len = bufsize;

    at_worker_task_add(tsk);

    if ((ret = at_sendto_lower(at._pstuart, (void *)cmd, cmdlen,
                               at._timeout, true)) != 0) {
        atpsr_err("uart send command failed");
        goto end;
    }

    if (delimiter) {
        if ((ret = at_sendto_lower(at._pstuart, (void *)at._send_delimiter,
                    strlen(at._send_delimiter), at._timeout, false)) != 0) {
            atpsr_err("uart send delimiter failed");
            goto end;
        }
    }

    if (data && datalen > 0) {
        if ((ret = at_sendto_lower(at._pstuart, (void *)data, datalen, at._timeout, true)) != 0) {
            atpsr_err("uart send delimiter failed");
            goto end;
        }
    }

    if ((ret = atpsr_sem_wait(tsk->smpr, TASK_DEFAULT_WAIT_TIME)) != 0) {
        atpsr_err("sem_wait failed");
        goto end;
    }

end:
    at_worker_task_del(tsk);
    atpsr_mutex_unlock(at.at_uart_send_mutex);
    return ret;
}
#endif

// register oob
int at_register_callback(const char *prefix, const char *postfix,
                         int maxlen, at_recv_cb cb, void *arg)
{
    oob_t *oob = NULL;
    int    i   = 0;

    if (maxlen < 0 || NULL == prefix) {
        atpsr_err("%s invalid input \r\n", __func__);
        return -1;
    }

    if (at._oobs_num >= OOB_MAX) {
        atpsr_debug("No place left in OOB.\r\n");
        return -1;
    }

    /*check oob is exit*/
    for (i = 0; i < at._oobs_num; i++) {
        if (strcmp(prefix, at._oobs[i].prefix) == 0) {
            atpsr_debug("oob prefix %s is already exist.\r\n", prefix);
            return -1;
        }
    }

    oob = &(at._oobs[at._oobs_num++]);

    oob->oobinputdata = NULL;
    if (postfix != NULL) {
        oob->oobinputdata = atpsr_malloc(maxlen);
        if (NULL == oob->oobinputdata) {
            atpsr_err("fail to malloc len %d at %s for prefix %s \r\n",
                 maxlen, __func__, prefix);
            return -1;
        }
        memset(oob->oobinputdata, 0, maxlen);
    }


    oob->maxlen  = maxlen;
    oob->prefix  = (char *)prefix;
    oob->postfix = (char *)postfix;
    oob->cb      = cb;
    oob->arg     = arg;
    oob->reallen = 0;

    atpsr_debug("New oob registered (%s)", oob->prefix);

    return 0;
}

#define RECV_BUFFER_SIZE 512
static void at_scan_for_callback(char c, char *buf, int *index)
{
    int     k;
    oob_t  *oob = NULL;
    int offset = *index;

    if (!buf || offset < 0) {
        return;
    }

    for (k = 0; k < at._oobs_num; k++) {
        oob = &(at._oobs[k]);
        if (oob->reallen > 0 ||
            (offset >= strlen(oob->prefix) &&
             memcmp(oob->prefix, buf + offset - strlen(oob->prefix),
                    strlen(oob->prefix)) == 0)) {
            atpsr_debug("AT! %s\r\n", oob->prefix);
            if (oob->postfix == NULL) {
                oob->cb(oob->arg, NULL, 0);
                memset(buf, 0, offset);
                offset = 0;
            } else {
                if (oob->reallen == 0) {
                    int len = strlen(oob->prefix) - 1;
                    len = len > 0 ? len : 0;
                    memset(oob->oobinputdata, 0, oob->maxlen);
                    memcpy(oob->oobinputdata, oob->prefix, len);
                    oob->reallen += len;
                }

                if (oob->reallen < oob->maxlen) {
                        oob->oobinputdata[oob->reallen] = c;
                        oob->reallen++;
                        if ((oob->reallen >=
                             strlen(oob->prefix) + strlen(oob->postfix)) &&
                            (strncmp(oob->oobinputdata + oob->reallen -
                                       strlen(oob->postfix),
                                     oob->postfix,
                                     strlen(oob->postfix)) == 0)) {
                            /*recv postfix*/
                            oob->cb(oob->arg, oob->oobinputdata, oob->reallen);
                            memset(oob->oobinputdata, 0, oob->reallen);
                            oob->reallen = 0;
                            memset(buf, 0, offset);
                            offset = 0;
                        }
                    } else {
                        atpsr_err("invalid oob %s input , for oversize %s \r\n",
                                   oob->prefix, oob->oobinputdata);
                        memset(oob->oobinputdata, 0, oob->reallen);
                        oob->reallen = 0;
                        memset(buf, 0, offset);
                        offset = 0;
                    }

                /*oob data maybe more than buf size */
                if (offset > (RECV_BUFFER_SIZE - 2)) {
                    memset(buf, 0, offset);
                    offset = 0;
                }
            }
            continue;
        }
    }

    *index = offset;
    return;
}

#if ATPSR_SINGLE_TASK
static char at_rx_buf[RECV_BUFFER_SIZE];
int at_yield(char *replybuf, int bufsize, const atcmd_config_t *atcmdconfig,
             int timeout_ms)
{
    int        offset                  = 0;
    int        ret                     = 0;
    int        rsp_prefix_len          = 0;
    int        rsp_success_postfix_len = 0;
    int        rsp_fail_postfix_len    = 0;
    int        at_reply_begin          = 0;
    int        at_reply_offset         = 0;
    char       c                       = 0;
    char      *buf                 = NULL;
    char      *rsp_prefix          = NULL;
    char      *rsp_success_postfix = NULL;
    char      *rsp_fail_postfix    = NULL;

    if (!inited) {
        atpsr_err("AT parser has not inited!\r\n");
        return -1;
    }

    if (replybuf != NULL && bufsize <= 0) {
        atpsr_err("buffer size %d unmatched!\r\n", bufsize);
        return -1;
    }

    buf = at_rx_buf;
    if (NULL == buf) {
        atpsr_err("AT worker fail to malloc ,task exist \r\n");
        return -1;
    }

    memset(buf, 0, RECV_BUFFER_SIZE);

    while (true) {
        // read from uart and store buf
        ret = at_getc(&c, timeout_ms);
        if (ret != 0) {
            atpsr_err("at yield timeout break loop");
            break;
        }

        if (offset + 1 >= RECV_BUFFER_SIZE) {
            atpsr_err("buffer full");
            break;
        }
        buf[offset++] = c;
        buf[offset]   = 0;

        at_scan_for_callback(c, buf, &offset);

        if (replybuf == NULL || bufsize <= 0) {
            // if no task, continue recv
            continue;
        }

        if (NULL != atcmdconfig && NULL != atcmdconfig->reply_prefix) {
            rsp_prefix     = atcmdconfig->reply_prefix;
            rsp_prefix_len = strlen(rsp_prefix);
        } else {
            rsp_prefix     = at._default_recv_prefix;
            rsp_prefix_len = at._recv_prefix_len;
        }

        if (NULL != atcmdconfig && NULL != atcmdconfig->reply_success_postfix) {
            rsp_success_postfix     = atcmdconfig->reply_success_postfix;
            rsp_success_postfix_len = strlen(rsp_success_postfix);
        } else {
            rsp_success_postfix     = at._default_recv_success_postfix;
            rsp_success_postfix_len = at._recv_success_postfix_len;
        }

        if (NULL != atcmdconfig && NULL != atcmdconfig->reply_fail_postfix) {
            rsp_fail_postfix     = atcmdconfig->reply_fail_postfix;
            rsp_fail_postfix_len = strlen(rsp_fail_postfix);
        } else {
            rsp_fail_postfix     = at._default_recv_fail_postfix;
            rsp_fail_postfix_len = at._recv_fail_postfix_len;
        }

        if (offset >= rsp_prefix_len && at_reply_begin == 0 &&
            (strncmp(buf + offset - rsp_prefix_len, rsp_prefix,
                     rsp_prefix_len) == 0)) {
            at_reply_begin = 1;
        }

        if (at_reply_begin == 1) {
            if (at_reply_offset < bufsize) {
                replybuf[at_reply_offset] = c;
                at_reply_offset++;

                if ((at_reply_offset >= rsp_success_postfix_len &&
                     strncmp(
                       replybuf + at_reply_offset - rsp_success_postfix_len,
                       rsp_success_postfix, rsp_success_postfix_len) == 0) ||
                    (at_reply_offset >= rsp_fail_postfix_len &&
                     strncmp(replybuf + at_reply_offset - rsp_fail_postfix_len,
                             rsp_fail_postfix, rsp_fail_postfix_len) == 0)) {
                    return 0;
                }
            } else {
                memset(replybuf, 0, bufsize);
                strcpy(replybuf, rsp_fail_postfix);
                break;
            }
        }

    }

    return -1;
}
#else
static void at_worker(void *arg)
{
    int        offset                  = 0;
    int        ret                     = 0;
    int        at_task_empty           = 0;
    int        at_task_reponse_begin   = 0;
    int        memcpy_size             = 0;
    int        rsp_prefix_len          = 0;
    int        rsp_success_postfix_len = 0;
    int        rsp_fail_postfix_len    = 0;
    char       c                       = 0;
    at_task_t *tsk                 = NULL;
    char      *buf                 = NULL;
    char      *rsp_prefix          = NULL;
    char      *rsp_success_postfix = NULL;
    char      *rsp_fail_postfix    = NULL;

    atpsr_debug("at_work started.");

    buf = atpsr_malloc(RECV_BUFFER_SIZE);
    if (NULL == buf) {
        atpsr_err("AT worker fail to malloc ,task exist \r\n");
        return;
    }

    memset(buf, 0, RECV_BUFFER_SIZE);
    inited = 1;

    while (true) {
        // read from uart and store buf
        ret = at_getc(&c, at._timeout);
        if (ret != 0) {
            continue;
        }

        if (offset + 1 >= RECV_BUFFER_SIZE) {
            atpsr_err("Fatal error, no one is handling AT uart");
            goto check_buffer;
        }
        buf[offset++] = c;
        buf[offset]   = 0;

        at_scan_for_callback(c, buf, &offset);

        atpsr_mutex_lock(at.task_mutex);
        at_task_empty = slist_empty(&at.task_l);
        tsk = NULL;

        if (!at_task_empty) {
            tsk = slist_first_entry(&at.task_l, at_task_t, next);
        }
        atpsr_mutex_unlock(at.task_mutex);

        // if no task, continue recv
        if (at_task_empty || NULL == tsk) {
            atpsr_debug("No task in queue");
            goto check_buffer;
        }

        if (NULL != tsk->rsp_prefix && 0 != tsk->rsp_prefix_len) {
            rsp_prefix     = tsk->rsp_prefix;
            rsp_prefix_len = tsk->rsp_prefix_len;
        } else {
            rsp_prefix     = at._default_recv_prefix;
            rsp_prefix_len = at._recv_prefix_len;
        }

        if (NULL != tsk->rsp_success_postfix &&
            0 != tsk->rsp_success_postfix_len) {
            rsp_success_postfix     = tsk->rsp_success_postfix;
            rsp_success_postfix_len = tsk->rsp_success_postfix_len;
        } else {
            rsp_success_postfix     = at._default_recv_success_postfix;
            rsp_success_postfix_len = at._recv_success_postfix_len;
        }

        if (NULL != tsk->rsp_fail_postfix && 0 != tsk->rsp_fail_postfix_len) {
            rsp_fail_postfix     = tsk->rsp_fail_postfix;
            rsp_fail_postfix_len = tsk->rsp_fail_postfix_len;
        } else {
            rsp_fail_postfix     = at._default_recv_fail_postfix;
            rsp_fail_postfix_len = at._recv_fail_postfix_len;
        }

        if (offset >= rsp_prefix_len && at_task_reponse_begin == 0 &&
            (strncmp(buf + offset - rsp_prefix_len, rsp_prefix,
                     rsp_prefix_len) == 0)) {
            at_task_reponse_begin = 1;
        }

        if (at_task_reponse_begin == 1) {
            if (tsk->rsp_offset < tsk->rsp_len) {
                tsk->rsp[tsk->rsp_offset] = c;
                tsk->rsp_offset++;

                if ((tsk->rsp_offset >= rsp_success_postfix_len &&
                     strncmp(
                       tsk->rsp + tsk->rsp_offset - rsp_success_postfix_len,
                       rsp_success_postfix, rsp_success_postfix_len) == 0) ||
                    (tsk->rsp_offset >= rsp_fail_postfix_len &&
                     strncmp(tsk->rsp + tsk->rsp_offset - rsp_fail_postfix_len,
                             rsp_fail_postfix, rsp_fail_postfix_len) == 0)) {
                    atpsr_sem_signal(tsk->smpr);
                    at_task_reponse_begin = 0;
                    memset(buf, 0, offset);
                    offset = 0;
                }
            } else {
                memset(tsk->rsp, 0, tsk->rsp_len);
                strcpy(tsk->rsp, rsp_fail_postfix);
                atpsr_sem_signal(tsk->smpr);
                at_task_reponse_begin = 0;
                memset(buf, 0, offset);
                offset = 0;
            }
        }
    check_buffer:
        // in case buffer is full
        if (offset > (RECV_BUFFER_SIZE - 2)) {
            printf("buffer full \r\n");
            memcpy_size = rsp_prefix_len > rsp_success_postfix_len
                            ? rsp_prefix_len
                            : rsp_success_postfix_len;
            memcpy_size = memcpy_size > rsp_fail_postfix_len
                            ? memcpy_size
                            : rsp_fail_postfix_len;
            memcpy(buf, buf + offset - memcpy_size, memcpy_size);
            memset(buf + memcpy_size, 0, offset - memcpy_size);
            offset = memcpy_size;
        }
    }

    return;
}
#endif
