/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <aos/log.h>
#include "atparser.h"

#ifdef CENTRALIZE_MAPPING
#include "board.h"
#endif

#ifdef HDLC_UART
#include "hdlc.h"
#endif

#define MODULE_NAME            "atparser"
#define TASK_DEFAULT_WAIT_TIME 5000

static uint8_t    inited = 0;
static uart_dev_t at_uart;
static uint32_t   at_worker_stacksize = 1024;

#ifdef HDLC_UART
static encode_context_t hdlc_encode_ctx;
static decode_context_t hdlc_decode_ctx;
#endif

static void at_worker(void *arg);

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

static void at_set_worker_stack_size(uint16_t size)
{
    at_worker_stacksize = size;
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
    if (0 != aos_mutex_new(&at.task_mutex)) {
        LOGE(MODULE_NAME, "Creating task mutex failed\r\n");
        return -1;
    }

    return 0;
}

static void at_uinit_task_mutex()
{
    if (aos_mutex_is_valid(&at.task_mutex)) {
        aos_mutex_free(&at.task_mutex);
    }
    return;
}

static int at_init_at_mutex()
{
    if (0 != aos_mutex_new(&at.at_mutex)) {
        LOGE(MODULE_NAME, "Creating at mutex failed\r\n");
        return -1;
    }

    return 0;
}

static void at_uinit_at_mutex()
{
    if (aos_mutex_is_valid(&at.at_mutex)) {
        aos_mutex_free(&at.at_mutex);
    }
    return;
}

static int at_worker_uart_send_mutex_init()
{
    if (0 != aos_mutex_new(&at.at_uart_send_mutex)) {
        LOGE(MODULE_NAME, "Creating at worker sem failed\r\n");
        return -1;
    }
    return 0;
}

static void at_worker_uart_send_mutex_uinit()
{
    if (aos_mutex_is_valid(&at.at_uart_send_mutex)) {
        aos_mutex_free(&at.at_uart_send_mutex);
    }
}

static int at_init(const char *recv_prefix, const char *recv_success_postfix,
                   const char *recv_fail_postfix, const char *send_delimiter,
                   int timeout)
{
    if (!recv_prefix || !recv_success_postfix || !recv_fail_postfix ||
        !send_delimiter || (timeout < 0)) {
        LOGE(MODULE_NAME, "%s: invalid argument", __func__);
        return -1;
    }

    if (inited == 1) {
        LOGI(MODULE_NAME, "have already inited ,it will init again\r\n");
        inited = 0;
    }

    if (at_init_uart() != 0) {
        LOGE(MODULE_NAME, "at uart init fail \r\n");
        return -1;
    }

    memset(at._oobs, 0, sizeof(oob_t) * OOB_MAX);

    at_set_timeout(timeout);
    at_set_recv_delimiter(recv_prefix, recv_success_postfix, recv_fail_postfix);
    at_set_send_delimiter(send_delimiter);

    LOGD(MODULE_NAME,
         "at worker rcv prefix is %s success postfix is %s fail postfix is %s "
         "\r\n",
         recv_prefix, recv_success_postfix, recv_fail_postfix);
    if (at_init_at_mutex() != 0) {
        LOGE(MODULE_NAME, "at uart mutex init fail \r\n");
        return -1;
    }

    if (at_init_task_mutex() != 0) {
        at_uinit_at_mutex();
        LOGE(MODULE_NAME, "at mutex init fail \r\n");
        return -1;
    }

    slist_init(&at.task_l);

    if (at_worker_uart_send_mutex_init() != 0) {
        at_uinit_at_mutex();
        at_uinit_task_mutex();
        LOGE(MODULE_NAME, "fail to creat at worker sem\r\n");
    }

    if (aos_task_new("at_worker", at_worker, NULL, at_worker_stacksize)) {
        at_uinit_at_mutex();
        at_uinit_task_mutex();
        at_worker_uart_send_mutex_uinit();
        LOGE(MODULE_NAME, "fail to creat at task\r\n");
        return -1;
    }

    inited = 1;

    return 0;
}

static void at_set_mode(at_mode_t m)
{
    if (m == at._mode) {
        return;
    }
    /*at operate mode changed, it should wait the uart read operate finished in
     * the origin operate mode*/
    aos_mutex_lock(&at.at_mutex, AOS_WAIT_FOREVER);
    at._mode = m;
    aos_mutex_unlock(&at.at_mutex);
}

static int at_putc(char c)
{
    int ret = 0;
    if (inited == 0) {
        LOGE(MODULE_NAME, "at have not init yet\r\n");
        return -1;
    }

    if (at._mode != ASYN) {
        LOGE(MODULE_NAME, "AT mode is normal, can no use at_putc \r\n");
        return -1;
    }

    LOGD(MODULE_NAME, "uart sending %c(0x%02x)\r\n", c, c);
    aos_mutex_lock(&at.at_uart_send_mutex, AOS_WAIT_FOREVER);
#ifdef HDLC_UART
    ret = hdlc_uart_send(&hdlc_encode_ctx, at._pstuart, (void *)&c, 1,
                         at._timeout, false);
#else
    ret = hal_uart_send(at._pstuart, (void *)&c, 1, at._timeout);
#endif
    aos_mutex_unlock(&at.at_uart_send_mutex);

    return ret;
}

static int at_getc(char *c)
{
    int      ret = 0;
    char     data;
    uint32_t recv_size = 0;

    if (NULL == c) {
        return -1;
    }

    if (inited == 0) {
        LOGE(MODULE_NAME, "at have not init yet\r\n");
        return -1;
    }
#if 1
    if (at._mode != ASYN) {
        return -1;
    }
#endif

    aos_mutex_lock(&at.at_mutex, AOS_WAIT_FOREVER);
#ifdef HDLC_UART
    ret = hdlc_uart_recv(&hdlc_decode_ctx, at._pstuart, (void *)&data, 1,
                         &recv_size, at._timeout);
#else
    ret =
      hal_uart_recv_II(at._pstuart, (void *)&data, 1, &recv_size, at._timeout);
#endif
    aos_mutex_unlock(&at.at_mutex);

    if (ret != 0) {
        return -1;
    }

    if (recv_size == 1) {
        *c = data;
        return 0;
    } else {
        return -1;
    }
}

static int at_write(const char *data, int size)
{
    int ret = 0;

    if (inited == 0) {
        LOGE(MODULE_NAME, "at have not init yet\r\n");
        return -1;
    }

    aos_mutex_lock(&at.at_uart_send_mutex, AOS_WAIT_FOREVER);
#ifdef HDLC_UART
    ret = hdlc_uart_send(&hdlc_encode_ctx, at._pstuart, (void *)data, size,
                         AOS_WAIT_FOREVER, true);
#else
    ret = hal_uart_send(at._pstuart, (void *)data, size, AOS_WAIT_FOREVER);
#endif
    aos_mutex_unlock(&at.at_uart_send_mutex);

    if (ret != 0) {
        return -1;
    }

    return size;
}

static int at_read(char *data, int size)
{
    int      ret = 0;
    uint32_t recv_size, total_read = 0;

    if (inited == 0) {
        LOGE(MODULE_NAME, "at have not init yet\r\n");
        return -1;
    }

    aos_mutex_lock(&at.at_mutex, AOS_WAIT_FOREVER);
    while (total_read < size) {
#ifdef HDLC_UART
        ret = hdlc_uart_recv(&hdlc_decode_ctx, at._pstuart,
                             (void *)(data + total_read), size - total_read,
                             &recv_size, at._timeout);
#else
        ret = hal_uart_recv_II(at._pstuart, (void *)(data + total_read),
                               size - total_read, &recv_size, at._timeout);
#endif
        if (ret != 0) {
            LOGE(MODULE_NAME, "at_read failed on uart_recv.");
            break;
        }

        if (recv_size <= 0) {
            continue;
        }
        total_read += recv_size;
        if (total_read >= size) {
            break;
        }
    }
    aos_mutex_unlock(&at.at_mutex);

    if (ret != 0) {
        return -1;
    }

    return total_read;
}

static int at_worker_task_add(at_task_t *tsk)
{
    if (NULL == tsk) {
        LOGE(MODULE_NAME, "invalid input %s \r\n", __func__);
        return -1;
    }

    aos_mutex_lock(&at.task_mutex, AOS_WAIT_FOREVER);
    slist_add_tail(&tsk->next, &at.task_l);
    aos_mutex_unlock(&at.task_mutex);

    return 0;
}

static int at_worker_task_del(at_task_t *tsk)
{
    if (NULL == tsk) {
        LOGE(MODULE_NAME, "invalid input %s \r\n", __func__);
        return -1;
    }

    aos_mutex_lock(&at.task_mutex, AOS_WAIT_FOREVER);
    slist_del(&tsk->next, &at.task_l);
    aos_mutex_unlock(&at.task_mutex);
    if (aos_sem_is_valid(&tsk->smpr)) {
        aos_sem_free(&tsk->smpr);
    }
    if (tsk) {
        aos_free(tsk);
    }

    return 0;
}

static int at_send_raw_self_define_respone_formate_internal(
  const char *command, uint32_t cmdlen, char *rsp, uint32_t rsplen,
  char *rsp_prefix, char *rsp_success_postfix, char *rsp_fail_postfix)
{
    int ret = 0;

    if (inited == 0) {
        LOGE(MODULE_NAME, "at have not init yet\r\n");
        return -1;
    }

    if (at._mode != ASYN) {
        LOGE(MODULE_NAME, "Operation not supported in non asyn mode \r\n");
        return -1;
    }

    if (NULL == command) {
        LOGE(MODULE_NAME, "%s invalid input \r\n", __FUNCTION__);
        return -1;
    }

    if (NULL == rsp || 0 == rsplen) {
        LOGE(MODULE_NAME, "%s invalid input \r\n", __FUNCTION__);
        return -1;
    }

    at_task_t *tsk = (at_task_t *)aos_malloc(sizeof(at_task_t));
    if (NULL == tsk) {
        LOGE(MODULE_NAME, "tsk buffer allocating failed");
        return -1;
    }
    memset(tsk, 0, sizeof(at_task_t));

    if ((ret = aos_sem_new(&tsk->smpr, 0)) != 0) {
        LOGE(MODULE_NAME, "failed to allocate semaphore");
        goto end;
    }

    LOGD(MODULE_NAME, "at task created: %d, smpr: %d", (uint32_t)tsk,
         (uint32_t)&tsk->smpr);

    if (NULL != rsp_prefix) {
        tsk->rsp_prefix     = rsp_prefix;
        tsk->rsp_prefix_len = strlen(rsp_prefix);
    }

    if (NULL != rsp_success_postfix) {
        tsk->rsp_success_postfix     = rsp_success_postfix;
        tsk->rsp_success_postfix_len = strlen(rsp_success_postfix);
    }

    if (NULL != rsp_fail_postfix) {
        tsk->rsp_fail_postfix     = rsp_fail_postfix;
        tsk->rsp_fail_postfix_len = strlen(rsp_fail_postfix);
    }

    tsk->command = (char *)command;
    tsk->rsp     = rsp;
    tsk->rsp_len = rsplen;

    at_worker_task_add(tsk);

    // uart operation should be inside mutex lock
#ifdef HDLC_UART
    if ((ret = hdlc_uart_send(&hdlc_encode_ctx, at._pstuart, (void *)command,
                              cmdlen, at._timeout, true)) != 0)
#else
    if ((ret = hal_uart_send(at._pstuart, (void *)command, cmdlen,
                             at._timeout)) != 0)
#endif
    {
        LOGE(MODULE_NAME, "uart send command failed");
        goto end;
    }
    LOGD(MODULE_NAME, "Sending command %s", command);

#ifdef HDLC_UART
    if ((ret = hdlc_uart_send(
           &hdlc_encode_ctx, at._pstuart, (void *)at._send_delimiter,
           strlen(at._send_delimiter), at._timeout, false)) != 0)
#else
    if ((ret = hal_uart_send(at._pstuart, (void *)at._send_delimiter,
                             strlen(at._send_delimiter), at._timeout)) != 0)
#endif
    {
        LOGE(MODULE_NAME, "uart send delimiter failed");
        goto end;
    }
    LOGD(MODULE_NAME, "Sending delimiter %s", at._send_delimiter);

    if ((ret = aos_sem_wait(&tsk->smpr, TASK_DEFAULT_WAIT_TIME)) != 0) {
        LOGD(MODULE_NAME, "sem_wait failed");
        goto end;
    }

    LOGD(MODULE_NAME, "sem_wait succeed.");

end:
    at_worker_task_del(tsk);
    return ret;
}

static int at_send_raw_self_define_respone_formate(const char *command,
                                                   char *rsp, uint32_t rsplen,
                                                   char *rsp_prefix,
                                                   char *rsp_success_postfix,
                                                   char *rsp_fail_postfix)
{
    int ret = 0;

    aos_mutex_lock(&at.at_uart_send_mutex, AOS_WAIT_FOREVER);
    ret = at_send_raw_self_define_respone_formate_internal(
      command, strlen(command), rsp, rsplen, rsp_prefix, rsp_success_postfix,
      rsp_fail_postfix);
    aos_mutex_unlock(&at.at_uart_send_mutex);

    return ret;
}

static int at_send_raw(const char *command, char *rsp, uint32_t rsplen)
{
    if (inited == 0) {
        LOGE(MODULE_NAME, "at have not init yet\r\n");
        return -1;
    }

    if (at._mode != ASYN) {
        LOGE(MODULE_NAME, "Operation not supported in non asyn mode");
        return -1;
    }

    return at_send_raw_self_define_respone_formate(command, rsp, rsplen, NULL,
                                                   NULL, NULL);
}

#ifdef DEBUG
static void dump_payload(uint8_t *p, uint32_t len)
{
    if (!p || !len) {
        return;
    }

    printf("\r\n");
    while (len-- > 0) {
        printf(" %02x", *p++);
    }
    printf("\r\n");
}
#endif

/**
 * This API can be used to send packet, without response required.
 *
 * AT stream format as below:
 *    <header>, [data], [tailer]
 *
 * In which, data and tailer is optional.
 */
static int at_send_data_3stage_no_rsp(const char *header, const uint8_t *data,
                                      uint32_t len, const char *tailer)
{
    int ret;

    if (inited == 0) {
        LOGE(MODULE_NAME, "at have not init yet\r\n");
        return -1;
    }

    if (at._mode != ASYN) {
        LOGE(MODULE_NAME, "Operation not supported in non asyn mode");
        return -1;
    }

    if (!header) {
        LOGE(MODULE_NAME, "Invalid null header\n");
        return -1;
    }

    aos_mutex_lock(&at.at_uart_send_mutex, AOS_WAIT_FOREVER);

    if (header) {
#ifdef HDLC_UART
        if ((ret = hdlc_uart_send(&hdlc_encode_ctx, at._pstuart, (void *)header,
                                  strlen(header), at._timeout, true)) != 0)
#else
        if ((ret = hal_uart_send(at._pstuart, (void *)header, strlen(header),
                                 at._timeout)) != 0)
#endif
        {
            LOGE(MODULE_NAME, "uart send packet header failed");
            aos_mutex_unlock(&at.at_uart_send_mutex);
            assert(0);
            return -1;
        }

        LOGD(MODULE_NAME, "Packet header sent: %s", header);
    }

    if (data && len) {
#ifdef HDLC_UART
        if ((ret = hdlc_uart_send(&hdlc_encode_ctx, at._pstuart, (void *)data,
                                  len, at._timeout, true)) != 0)
#else
        if ((ret =
               hal_uart_send(at._pstuart, (void *)data, len, at._timeout)) != 0)
#endif
        {
            LOGE(MODULE_NAME, "uart send packet failed");
            aos_mutex_unlock(&at.at_uart_send_mutex);
            assert(0);
            return -1;
        }
        LOGD(MODULE_NAME, "Packet sent, len: %d", len);
    }

    if (tailer) {
#ifdef HDLC_UART
        if ((ret = hdlc_uart_send(&hdlc_encode_ctx, at._pstuart, (void *)tailer,
                                  strlen(tailer), at._timeout, false)) != 0)
#else
        if ((ret = hal_uart_send(at._pstuart, (void *)tailer, strlen(tailer),
                                 at._timeout)) != 0)
#endif
        {
            LOGE(MODULE_NAME, "uart send packet tailer failed");
            aos_mutex_unlock(&at.at_uart_send_mutex);
            assert(0);
            return -1;
        }

        LOGD(MODULE_NAME, "Packet tailer sent: %s", tailer);
    }

    aos_mutex_unlock(&at.at_uart_send_mutex);

    return 0;
}

/**
 * This API is used, usually by athost, to send stream content without response
 * required. The content is usually status event, such as
 * YEVENT:MONITOR_UP/MONITOR_DOWN, etc.
 */
static int at_send_raw_no_rsp(const char *content)
{
    int ret;

    aos_mutex_lock(&at.at_uart_send_mutex, AOS_WAIT_FOREVER);

    if (content) {
#ifdef HDLC_UART
        if ((ret =
               hdlc_uart_send(&hdlc_encode_ctx, at._pstuart, (void *)content,
                              strlen(content), at._timeout, true)) != 0)
#else
        if ((ret = hal_uart_send(at._pstuart, (void *)content, strlen(content),
                                 at._timeout)) != 0)
#endif
        {
            LOGE(MODULE_NAME, "uart send raw content (%s) failed", content);
            aos_mutex_unlock(&at.at_uart_send_mutex);
            assert(0);
            return -1;
        }

        LOGD(MODULE_NAME, "Raw content (%s) with no response required sent.",
             content);
    }

    aos_mutex_unlock(&at.at_uart_send_mutex);

    return 0;
}

/**
 * Example:
 *          AT+ENETRAWSEND=<len>
 *          ><data>
 *          OK
 *
 * Send data in 2 stages. These 2 stages must be finished inside
 * one mutex lock.
 *   1. Send 'fst' string (first stage);
 *   2. Receving prompt, usually ">" character;
 *   3. Send data (second stage) in 'len' length.
 */
static int at_send_data_2stage(const char *fst, const char *data, uint32_t len,
                               char *rsp, uint32_t rsplen)
{
    int ret = 0;

    if (inited == 0) {
        LOGE(MODULE_NAME, "at have not init yet\r\n");
        return -1;
    }

    if (at._mode != ASYN) {
        LOGE(MODULE_NAME, "Operation not supported in non asyn mode \r\n");
        return -1;
    }

    if (NULL == fst) {
        LOGE(MODULE_NAME, "%s invalid input \r\n", __FUNCTION__);
        return -1;
    }

    if (NULL == rsp || 0 == rsplen) {
        LOGE(MODULE_NAME, "%s invalid input \r\n", __FUNCTION__);
        return -1;
    }

    at_task_t *tsk = (at_task_t *)aos_malloc(sizeof(at_task_t));
    if (NULL == tsk) {
        LOGE(MODULE_NAME, "tsk buffer allocating failed");
        return -1;
    }
    memset(tsk, 0, sizeof(at_task_t));

    if ((ret = aos_sem_new(&tsk->smpr, 0)) != 0) {
        LOGE(MODULE_NAME, "failed to allocate semaphore");
        goto end;
    }

    LOGD(MODULE_NAME, "at task created: %d, smpr: %d", (uint32_t)tsk,
         (uint32_t)&tsk->smpr);

    tsk->command = (char *)fst;
    tsk->rsp     = rsp;
    tsk->rsp_len = rsplen;

    aos_mutex_lock(&at.at_uart_send_mutex, AOS_WAIT_FOREVER);

    at_worker_task_add(tsk);

    // uart operation should be inside mutex lock
#ifdef HDLC_UART
    if ((ret = hdlc_uart_send(&hdlc_encode_ctx, at._pstuart, (void *)fst,
                              strlen(fst), at._timeout, true)) != 0)
#else
    if ((ret = hal_uart_send(at._pstuart, (void *)fst, strlen(fst),
                             at._timeout)) != 0)
#endif
    {
        LOGE(MODULE_NAME, "uart send command failed");
        goto end;
    }
    LOGD(MODULE_NAME, "Sending command %s", fst);

#ifdef HDLC_UART
    if ((ret = hdlc_uart_send(
           &hdlc_encode_ctx, at._pstuart, (void *)at._send_delimiter,
           strlen(at._send_delimiter), at._timeout, false)) != 0)
#else
    if ((ret = hal_uart_send(at._pstuart, (void *)at._send_delimiter,
                             strlen(at._send_delimiter), at._timeout)) != 0)
#endif
    {
        LOGE(MODULE_NAME, "uart send delimiter failed");
        goto end;
    }
    LOGD(MODULE_NAME, "Sending delimiter %s", at._send_delimiter);

#ifdef AT_PARSER_DELAY_FLAG
    aos_msleep(20);
#endif

#ifdef HDLC_UART
    if ((ret = hdlc_uart_send(&hdlc_encode_ctx, at._pstuart, (void *)data, len,
                              at._timeout, true)) != 0)
#else
    if ((ret = hal_uart_send(at._pstuart, (void *)data, len, at._timeout)) != 0)
#endif
    {
        LOGE(MODULE_NAME, "uart send 2stage data failed");
        goto end;
    }

    LOGD(MODULE_NAME, "Sending 2stage data %s", data);

    if ((ret = aos_sem_wait(&tsk->smpr, TASK_DEFAULT_WAIT_TIME)) != 0) {
        LOGE(MODULE_NAME, "sem_wait failed");
        goto end;
    }

    LOGD(MODULE_NAME, "sem_wait succeed.");
end:
    at_worker_task_del(tsk);
    aos_mutex_unlock(&at.at_uart_send_mutex);
    return ret;
}


// register oob
static int at_oob(const char *prefix, const char *postfix, int maxlen,
                  oob_cb cb, void *arg)
{
    oob_t *oob = NULL;
    int    i   = 0;

    if (maxlen < 0 || NULL == prefix) {
        LOGE(MODULE_NAME, "%s invalid input \r\n", __func__);
        return -1;
    }

    LOGD(MODULE_NAME, "New oob to register pre (%s) post %s \r\n", prefix,
         ((postfix == NULL) ? "NULL" : postfix));

    if (at._oobs_num >= OOB_MAX) {
        LOGW(MODULE_NAME, "No place left in OOB.\r\n");
        return -1;
    }

    /*check oob is exit*/
    for (i = 0; i < at._oobs_num; i++) {
        if (strcmp(prefix, at._oobs[i].prefix) == 0) {
            LOGW(MODULE_NAME, "oob prefix %s is already exist.\r\n", prefix);
            return -1;
        }
    }

    oob = &(at._oobs[at._oobs_num++]);

    oob->oobinputdata = NULL;
    if (postfix != NULL) {
        oob->oobinputdata = aos_malloc(maxlen);
        if (NULL == oob->oobinputdata) {
            LOGE(MODULE_NAME, "fail to malloc len %d at %s for prefix %s \r\n",
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

    LOGD(MODULE_NAME, "New oob registered (%s)", oob->prefix);

    return 0;
}

#define RECV_BUFFER_SIZE 512
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
    char       c;
    at_task_t *tsk;
    char      *buf                 = NULL;
    char      *rsp_prefix          = NULL;
    char      *rsp_success_postfix = NULL;
    char      *rsp_fail_postfix    = NULL;
    oob_t     *oob                 = NULL;

    LOGD(MODULE_NAME, "at_work started.");

    buf = aos_malloc(RECV_BUFFER_SIZE);
    if (NULL == buf) {
        LOGE(MODULE_NAME, "AT worker fail to malloc ,task exist \r\n");
        aos_task_exit(0);
        return;
    }

    memset(buf, 0, RECV_BUFFER_SIZE);

    while (true) {
        // read from uart and store buf
        if (at._mode != ASYN) {
            aos_msleep(1);
        }

        ret = at_getc(&c);
        if (ret != 0) {
            continue;
        }

        if (offset + 1 >= RECV_BUFFER_SIZE) {
            LOGE(MODULE_NAME, "Fatal error, no one is handling AT uart");
            goto check_buffer;
        }
        buf[offset++] = c;
        buf[offset]   = 0;

        for (int k = 0; k < at._oobs_num; k++) {
            oob = &(at._oobs[k]);
            if (oob->reallen > 0 ||
                (offset >= strlen(oob->prefix) &&
                 memcmp(oob->prefix, buf + offset - strlen(oob->prefix),
                        strlen(oob->prefix)) == 0)) {
                LOGD(MODULE_NAME, "AT! %s\r\n", oob->prefix);
                if (oob->postfix == NULL) {
                    oob->cb(oob->arg, NULL, 0);
                    memset(buf + offset - strlen(oob->prefix), 0,
                           strlen(oob->prefix));
                    offset -= strlen(oob->prefix);
                } else {
                    if (oob->reallen == 0) {
                        memset(oob->oobinputdata, 0, oob->maxlen);
                        memcpy(oob->oobinputdata, oob->prefix,
                               strlen(oob->prefix) - 1);
                        oob->reallen += strlen(oob->prefix) - 1;
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
                        LOGE(MODULE_NAME,
                             "invalid oob %s input , for oversize %s \r\n",
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

        aos_mutex_lock(&at.task_mutex, AOS_WAIT_FOREVER);
        at_task_empty = slist_empty(&at.task_l);

        if (!at_task_empty) {
            tsk = slist_first_entry(&at.task_l, at_task_t, next);
        }
        aos_mutex_unlock(&at.task_mutex);

        // if no task, continue recv
        if (at_task_empty) {
            LOGD(MODULE_NAME, "No task in queue");
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
                    aos_sem_signal(&tsk->smpr);
                    at_task_reponse_begin = 0;
                    memset(buf, 0, offset);
                    offset = 0;
                }
            } else {
                memset(tsk->rsp, 0, tsk->rsp_len);
                strcpy(tsk->rsp, rsp_fail_postfix);
                aos_sem_signal(&tsk->smpr);
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

at_parser_t at = { ._oobs                 = { { 0 } },
                   ._oobs_num             = 0,
                   ._mode                 = ASYN, // default mode - atworker
                   .init                  = at_init,
                   .set_mode              = at_set_mode,
                   .set_timeout           = at_set_timeout,
                   .set_recv_delimiter    = at_set_recv_delimiter,
                   .set_worker_stack_size = at_set_worker_stack_size,
                   .set_send_delimiter    = at_set_send_delimiter,
                   .send_raw_self_define_respone_formate =
                     at_send_raw_self_define_respone_formate,
                   .send_raw                = at_send_raw,
                   .send_data_2stage        = at_send_data_2stage,
                   .send_data_3stage_no_rsp = at_send_data_3stage_no_rsp,
                   .send_raw_no_rsp         = at_send_raw_no_rsp,
                   .putch                   = at_putc,
                   .getch                   = at_getc,
                   .write                   = at_write,
                   .read                    = at_read,
                   .parse                   = at_read,
                   .oob                     = at_oob };
