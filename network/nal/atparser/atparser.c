/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "ulog/ulog.h"

#include "atparser_internal.h"

#define TAG            "atparser"

#define atpsr_debug(format, ...)  LOGD(TAG, format, ##__VA_ARGS__)
#define atpsr_err(format, ...)    LOGE(TAG, format, ##__VA_ARGS__)

static uint8_t    inited = 0;
static at_dev_t at_dev[ATPSR_DEV_MAX_NUM] = { 0 };
static at_dev_ops_t *at_dev_ops[AT_DEV_TYPE_MAX] = { 0 };

#define RECV_BUFFER_SIZE 512
static int register_at_dev_ops(void)
{
    extern at_dev_ops_t at_uart_ops;
    at_dev_ops[AT_DEV_UART] = &at_uart_ops;

    /* Register more dev opt here */
    return 0;
}

static void unregister_at_dev_ops(void)
{
    memset(at_dev_ops, 0, AT_DEV_TYPE_MAX * sizeof(at_dev_ops_t *));
    return;
}

static int check_fd_valid(int fd)
{
    return (fd >= 0 && fd < ATPSR_DEV_MAX_NUM);
}

static int check_type_valid(at_dev_type_t type)
{
    return (type >= 0 && type < AT_DEV_TYPE_MAX);
}

static int check_dev_ready(int fd)
{
    return (at_dev[fd]._inited == 1);
}

static at_dev_t *obtain_dev_by_fd(int fd)
{
    return &at_dev[fd];
}

static at_dev_ops_t *obtain_op_by_type(at_dev_type_t type)
{
    return at_dev_ops[type];
}

static void at_scan_for_callback(int fd, char c, char  *buf, int buf_size, int *index)
{
    int     k;
    oob_t  *oob = NULL;
    int offset = *index;
    at_dev_t  *dev = NULL;

    if (!buf || buf_size <= 0 || offset < 0) {
        return;
    }

    if (!check_fd_valid(fd) || !check_dev_ready(fd))
        return -1;

    dev = obtain_dev_by_fd(fd);
    for (k = 0; k < dev->_oobs_num; k++) {
        oob = &(dev->_oobs[k]);
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
                if (offset > (buf_size - 2)) {
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

static int at_getc(int fd, char *c, int timeout_ms)
{
    int      ret = 0;
    char     data;
    uint32_t recv_size = 0;
    at_dev_t  *dev = NULL;
    at_dev_ops_t *op = NULL;

    if (inited == 0) {
        atpsr_err("at module have not init yet\r\n");
        return -1;
    }

    if (NULL == c) {
        return -1;
    }

    if (!check_fd_valid(fd) || !check_dev_ready(fd))
        return -1;

    dev = obtain_dev_by_fd(fd);
    op = obtain_op_by_type(dev->_type);

    atpsr_mutex_lock(dev->_recv_mutex);
    ret = op->recv(dev->_dev, (void *)&data, 1, &recv_size, timeout_ms);
    atpsr_mutex_unlock(dev->_recv_mutex);

    if (ret != 0) {
#ifdef WORKAROUND_DEVELOPERBOARD_DMA_UART
        if (ret == 1) {
            atpsr_debug("--->AT dma fail, restart!\n");
            op->deinit(dev->_dev);
            op->init(dev->_dev);
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

#if ATPSR_SINGLE_TASK
static char at_rx_buf[RECV_BUFFER_SIZE];
static int at_yield_impl(int fd, char *replybuf, int bufsize, const at_reply_config_t *atcmdconfig,
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
    char      *buf                     = NULL;
    char      *rsp_prefix              = NULL;
    char      *rsp_success_postfix     = NULL;
    char      *rsp_fail_postfix        = NULL;
    at_dev_t  *dev                     = NULL;

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
        atpsr_err("AT worker fail to malloc, task exist\r\n");
        return -1;
    }

    if (!check_fd_valid(fd) || !check_dev_ready(fd))
        return -1;

    dev = obtain_dev_by_fd(fd);

    memset(buf, 0, RECV_BUFFER_SIZE);

    while (true) {
        /* read from uart and store buf */
        ret = at_getc(fd, &c, timeout_ms);
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

        at_scan_for_callback(fd, c, buf, RECV_BUFFER_SIZE, &offset);

        if (dev->_wait_prompt && atcmdconfig &&
            atcmdconfig->reply_success_postfix &&
            strncmp(atcmdconfig->reply_success_postfix,
                    AT_SEND_DEFAULT_DATA_PROMPT, strlen(AT_SEND_DEFAULT_DATA_PROMPT)) == 0) {
            if (offset >= strlen(AT_SEND_DEFAULT_DATA_PROMPT) &&
                 strncmp(buf + offset - strlen(AT_SEND_DEFAULT_DATA_PROMPT),
                         AT_SEND_DEFAULT_DATA_PROMPT, strlen(AT_SEND_DEFAULT_DATA_PROMPT)) == 0) {
                return 0;
            }
        }

        if (replybuf == NULL || bufsize <= 0) {
            // if no task, continue recv
            continue;
        }

        if (NULL != atcmdconfig && NULL != atcmdconfig->reply_prefix) {
            rsp_prefix     = atcmdconfig->reply_prefix;
            rsp_prefix_len = strlen(rsp_prefix);
        } else {
            rsp_prefix     = dev->_default_recv_prefix;
            rsp_prefix_len = dev->_recv_prefix_len;
        }

        if (NULL != atcmdconfig && NULL != atcmdconfig->reply_success_postfix) {
            rsp_success_postfix     = atcmdconfig->reply_success_postfix;
            rsp_success_postfix_len = strlen(rsp_success_postfix);
        } else {
            rsp_success_postfix     = dev->_default_recv_success_postfix;
            rsp_success_postfix_len = dev->_recv_success_postfix_len;
        }

        if (NULL != atcmdconfig && NULL != atcmdconfig->reply_fail_postfix) {
            rsp_fail_postfix     = atcmdconfig->reply_fail_postfix;
            rsp_fail_postfix_len = strlen(rsp_fail_postfix);
        } else {
            rsp_fail_postfix     = dev->_default_recv_fail_postfix;
            rsp_fail_postfix_len = dev->_recv_fail_postfix_len;
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
static int at_scan_for_response(int fd, char c, char *buf, int *index)
{
    int        rsp_prefix_len          = 0;
    int        rsp_success_postfix_len = 0;
    int        rsp_fail_postfix_len    = 0;
    at_task_t *tsk                 = NULL;
    char      *rsp_prefix          = NULL;
    char      *rsp_success_postfix = NULL;
    char      *rsp_fail_postfix    = NULL;
    slist_t   *cur                 = NULL;
    int       offset               = *index;
    int       memcpy_size          = 0;
    at_dev_t  *dev                 = NULL;

    if (!buf || offset < 0) {
        return 0;
    }

    if (!check_fd_valid(fd) || !check_dev_ready(fd))
        return -1;

    dev = obtain_dev_by_fd(fd);

    atpsr_mutex_lock(dev->_task_mutex);
    slist_for_each_entry_safe(&dev->_task_l, cur, tsk, at_task_t, next) {
    if (tsk->rsp_state == AT_RSP_WAITPROMPT ||
        tsk->rsp_state == AT_RSP_PENDING ||
        tsk->rsp_state == AT_RSP_PROCESSING)
        break;
    }

    /* if no task, continue recv */
    if (NULL == tsk) {
        atpsr_debug("No task in queue");
        atpsr_mutex_unlock(dev->_task_mutex);
        return 0;
    }

    if (dev->_wait_prompt &&
        tsk->rsp_state == AT_RSP_WAITPROMPT &&
        offset >= strlen(AT_SEND_DEFAULT_DATA_PROMPT)) {
        if (strncmp(buf + offset - strlen(AT_SEND_DEFAULT_DATA_PROMPT),
                    AT_SEND_DEFAULT_DATA_PROMPT,
                    strlen(AT_SEND_DEFAULT_DATA_PROMPT)) == 0) {
            tsk->rsp_state = AT_RSP_PENDING;
            atpsr_sem_signal(tsk->smpr);
            memset(buf, 0, offset);
            offset = 0;
        }

        *index = offset;
        atpsr_mutex_unlock(dev->_task_mutex);
        return 0;
    }

    if (NULL != tsk->rsp_prefix && 0 != tsk->rsp_prefix_len) {
        rsp_prefix     = tsk->rsp_prefix;
        rsp_prefix_len = tsk->rsp_prefix_len;
    } else {
        rsp_prefix     = dev->_default_recv_prefix;
        rsp_prefix_len = dev->_recv_prefix_len;
    }

    if (NULL != tsk->rsp_success_postfix &&
        0 != tsk->rsp_success_postfix_len) {
        rsp_success_postfix     = tsk->rsp_success_postfix;
        rsp_success_postfix_len = tsk->rsp_success_postfix_len;
    } else {
        rsp_success_postfix     = dev->_default_recv_success_postfix;
        rsp_success_postfix_len = dev->_recv_success_postfix_len;
    }

    if (NULL != tsk->rsp_fail_postfix && 0 != tsk->rsp_fail_postfix_len) {
        rsp_fail_postfix     = tsk->rsp_fail_postfix;
        rsp_fail_postfix_len = tsk->rsp_fail_postfix_len;
    } else {
        rsp_fail_postfix     = dev->_default_recv_fail_postfix;
        rsp_fail_postfix_len = dev->_recv_fail_postfix_len;
    }

    if (offset < rsp_prefix_len && tsk->rsp_state == AT_RSP_PROCESSING) {
        tsk->rsp_state = AT_RSP_PENDING;
        memset(tsk->rsp, 0, tsk->rsp_len);
        tsk->rsp_offset = 0;
    }

    if (offset >= rsp_prefix_len && tsk->rsp_state == AT_RSP_PENDING &&
        (strncmp(buf + offset - rsp_prefix_len, rsp_prefix,
                 rsp_prefix_len) == 0)) {
        tsk->rsp_state = AT_RSP_PROCESSING;
    }

    if (tsk->rsp_state == AT_RSP_PROCESSING) {
        if (tsk->rsp_offset < tsk->rsp_len) {
            tsk->rsp[tsk->rsp_offset] = c;
            tsk->rsp_offset++;

            if ((tsk->rsp_offset >= rsp_success_postfix_len &&
                strncmp(tsk->rsp + tsk->rsp_offset - rsp_success_postfix_len,
                        rsp_success_postfix, rsp_success_postfix_len) == 0) ||
                (tsk->rsp_offset >= rsp_fail_postfix_len &&
                strncmp(tsk->rsp + tsk->rsp_offset - rsp_fail_postfix_len,
                        rsp_fail_postfix, rsp_fail_postfix_len) == 0)) {
                atpsr_sem_signal(tsk->smpr);
                tsk->rsp_state = AT_RSP_PROCESSED;
                tsk = NULL;
                memset(buf, 0, offset);
                offset = 0;
            }
        } else {
            memset(tsk->rsp, 0, tsk->rsp_len);
            tsk->rsp_state = AT_RSP_PENDING;
        }
    }
    *index = offset;
    atpsr_mutex_unlock(dev->_task_mutex);

    /* for buffer check */
    memcpy_size = rsp_prefix_len > rsp_success_postfix_len
                            ? rsp_prefix_len
                            : rsp_success_postfix_len;

    memcpy_size = memcpy_size > rsp_fail_postfix_len
                            ? memcpy_size
                            : rsp_fail_postfix_len;

    return memcpy_size;
}

static void at_scan_check_buffer(char *buf, int buf_size, int *index, int copy_size)
{
    int offset = *index;

    if (!buf || offset < 0 || copy_size < 0 ||
        copy_size >= buf_size) {
        return;
    }

    if (offset > (buf_size - 2)) {
        atpsr_err("buffer full\r\n");
        memcpy(buf, buf + offset - copy_size, copy_size);
        memset(buf + copy_size, 0, offset - copy_size);
        offset = copy_size;
    }

    *index = offset;
}

static void at_worker(void *arg)
{
    int              offset                  = 0;
    int              ret                     = 0;
    int              memcpy_size             = 0;
    char             c                       = 0;
    char            *buf                     = NULL;
    at_task_para_t  *para                    = NULL;
    at_dev_t        *dev                     = NULL;

    para = (at_task_para_t *)arg;
    if (NULL == para || !check_fd_valid(para->fd)) {
        atpsr_err("Invalid at worker task para, task exit\n");
        return;
    }

    buf = para->buf;
    if (NULL == buf) {
        atpsr_err("AT worker fail to allocate buffer, task exit\r\n");
        return;
    }
    memset(buf, 0, para->buf_size);

    dev = obtain_dev_by_fd(para->fd);
    dev->_inited = 1;
    atpsr_sem_signal(dev->_task_start_sem);

    atpsr_debug("at_work_%d started", para->fd);
    while (dev->_inited) {
        // read from uart and store buf
        ret = at_getc(para->fd, &c, dev->_timeout_ms);
        if (ret != 0) {
            continue;
        }

        if (offset + 1 >= para->buf_size) {
            atpsr_err("Fatal error, no one is handling AT uart");
            goto check_buffer;
        }
        buf[offset++] = c;
        buf[offset]   = 0;

        /* STEP 1: check whether there is a prefix of resgistered callback */
        at_scan_for_callback(para->fd, c, buf, para->buf_size,  &offset);

        /* STEP 2: check whether there is a response for AT cmd sent */
        memcpy_size = at_scan_for_response(para->fd, c, buf, &offset);

        /* STEP 3: check and clear buffer when it is full */
check_buffer:
        at_scan_check_buffer(buf, para->buf_size, &offset, memcpy_size);
    }

    atpsr_sem_signal(dev->_task_exit_sem);
    return;
}

static int stop_at_worker(int fd)
{
    at_dev_t *dev = NULL;

    if (!check_fd_valid(fd) || !check_dev_ready(fd))
        return -1;

    dev = obtain_dev_by_fd(fd);

    dev->_inited = 0;

    if (atpsr_sem_wait(dev->_task_exit_sem, AT_TASK_DEFAULT_WAIT_TIME_MS) != 0) {
        return -1;
    }

    return 0;
}

static int at_worker_task_add(int fd, at_task_t *tsk)
{
    at_dev_t *dev = NULL;

    if (NULL == tsk) {
        atpsr_err("invalid input %s \r\n", __func__);
        return -1;
    }

    if (!check_fd_valid(fd) || !check_dev_ready(fd))
        return -1;

    dev = obtain_dev_by_fd(fd);

    atpsr_mutex_lock(dev->_task_mutex);
    slist_add_tail(&tsk->next, &dev->_task_l);
    atpsr_mutex_unlock(dev->_task_mutex);

    return 0;
}

static int at_worker_task_del(int fd, at_task_t *tsk)
{
    at_dev_t *dev = NULL;

    if (NULL == tsk) {
        atpsr_err("invalid input %s \r\n", __func__);
        return -1;
    }

    if (!check_fd_valid(fd) || !check_dev_ready(fd))
        return -1;

    dev = obtain_dev_by_fd(fd);

    atpsr_mutex_lock(dev->_task_mutex);
    slist_del(&tsk->next, &dev->_task_l);
    atpsr_mutex_unlock(dev->_task_mutex);

    if (tsk->smpr != NULL) {
        atpsr_sem_free(tsk->smpr);
    }
    if (tsk) {
        atpsr_free(tsk);
    }

    return 0;
}
#endif

static int check_config_valid(at_config_t *config)
{
    int i;

    if (NULL == config || NULL == config->dev_cfg) {
        atpsr_err("Invalid AT device config\n");
        return -1;
    }

    /* check existence of port */
    for (i = 0; i < ATPSR_DEV_MAX_NUM; i++) {
        if (at_dev[i]._inited) {
            if (at_dev[i]._port == config->port) {
                atpsr_err("Port exist\n");
                return -1;
            }
        }
    }

    return 0;
}

static int assign_dev_fd(at_dev_type_t type)
{
    int i;
    (void)type;

    for (i = 0; i < ATPSR_DEV_MAX_NUM; i++) {
        if (!at_dev[i]._inited) {
            return i;
        }
    }

    return -1;
}

static void deinit_dev(int fd)
{
    at_dev_t *dev;

    if (!check_fd_valid(fd))
        return;

    dev = obtain_dev_by_fd(fd);

    if (dev->_recv_mutex != NULL) {
        atpsr_mutex_free(dev->_recv_mutex);
    }

    if (dev->_send_mutex != NULL) {
        atpsr_mutex_free(dev->_send_mutex);
    }

#if !ATPSR_SINGLE_TASK
    if (dev->_task_mutex != NULL) {
        atpsr_mutex_free(dev->_task_mutex);
    }


    if (dev->_task_start_sem != NULL) {
        atpsr_sem_free(dev->_task_start_sem);
    }

    if (dev->_task_exit_sem != NULL) {
        atpsr_sem_free(dev->_task_exit_sem);
    }

    if (dev->_task_para != NULL) {
        if (dev->_task_para->buf != NULL) {
            atpsr_free(dev->_task_para->buf);
            dev->_task_para->buf = NULL;
        }

        atpsr_free(dev->_task_para);
    }
#endif

    memset(dev, 0, sizeof(at_dev_t));
}

static int init_dev(int fd, at_config_t *config)
{
    at_dev_t *dev = NULL;
    at_dev_ops_t *op = NULL;

#if !ATPSR_SINGLE_TASK
    void  *task;
    at_task_para_t *para = NULL;
#endif

    if (inited == 0) {
        return -1;
    }

    if (!check_fd_valid(fd) || !config || !config->dev_cfg) {
        atpsr_err("init_dev invalid input\n");
        return -1;
    }

    if (!check_type_valid(config->type) || !at_dev_ops[config->type]) {
        atpsr_err("init_dev invalid type or at_dev_ops\n");
        return -1;
    }

    dev = obtain_dev_by_fd(fd);
    op = obtain_op_by_type(dev->_type);
    memset(dev, 0, sizeof(at_dev_t));

    if (op->init(config->dev_cfg) != 0) {
        atpsr_err("at dev init failed\n");
        return -1;
    }

    dev->_port = config->port;
    dev->_type = config->type;
    dev->_dev = config->dev_cfg;
    if (config->timeout_ms > 0) {
        dev->_timeout_ms = config->timeout_ms;
    } else {
        dev->_timeout_ms = AT_SEND_RECV_DEFAULT_TIMEOUT_MS;
    }

    if (config->reply_cfg.reply_prefix) {
        dev->_default_recv_prefix = config->reply_cfg.reply_prefix;
    } else {
        dev->_default_recv_prefix = AT_RECV_DEFAULT_PREFIX;
    }

    if (config->reply_cfg.reply_success_postfix) {
        dev->_default_recv_success_postfix = config->reply_cfg.reply_success_postfix;
    } else {
        dev->_default_recv_success_postfix = AT_RECV_DEFAULT_SUCCESS_POSTFIX;
    }

    if (config->reply_cfg.reply_fail_postfix) {
        dev->_default_recv_fail_postfix = config->reply_cfg.reply_fail_postfix;
    } else {
        dev->_default_recv_fail_postfix = AT_RECV_DEFAULT_FAIL_POSTFIX;
    }

    dev->_recv_prefix_len = strlen(dev->_default_recv_prefix);
    dev->_recv_success_postfix_len = strlen(dev->_default_recv_success_postfix);
    dev->_recv_fail_postfix_len = strlen(dev->_default_recv_fail_postfix);

    if (config->send_delimiter) {
        dev->_send_delimiter = config->send_delimiter;
    } else {
        dev->_send_delimiter = AT_SEND_DEFAULT_DELIMITER;
    }

    dev->_wait_prompt = config->send_wait_prompt;

    dev->_recv_mutex = atpsr_mutex_new();
    if (NULL == dev->_recv_mutex) {
        atpsr_err("create recv mutex failed\r\n");
        goto err;
    }

    dev->_send_mutex = atpsr_mutex_new();
    if (NULL == dev->_send_mutex) {
        atpsr_err("create send mutex failed\r\n");
        goto err;
    }

#if ATPSR_SINGLE_TASK
    dev->_inited = 1;
#else
    dev->_task_mutex = atpsr_mutex_new();
    if (NULL == dev->_task_mutex) {
        atpsr_err("create task mutex failed\r\n");
        goto err;
    }

    dev->_task_start_sem = atpsr_sem_new();
    if (NULL == dev->_task_start_sem) {
        atpsr_err("create task start sem failed\r\n");
        goto err;
    }

    dev->_task_exit_sem = atpsr_sem_new();
    if (NULL == dev->_task_exit_sem) {
        atpsr_err("create task exit sem failed\r\n");
        goto err;
    }

    dev->_task_para = (at_task_para_t *)atpsr_malloc(sizeof(at_task_para_t));
    if (NULL == dev->_task_para) {
        atpsr_err("create task para failed\r\n");
        goto err;
    }

    dev->_task_para->fd = fd;
    dev->_task_para->buf_size = RECV_BUFFER_SIZE;
    dev->_task_para->buf = (char *)atpsr_malloc(RECV_BUFFER_SIZE);
    if (NULL == dev->_task_para->buf) {
        atpsr_err("create task para buffer failed\r\n");
        goto err;
    }

    if (config->recv_task_priority > 0) {
        dev->_recv_task_prio = config->recv_task_priority;
    } else {
        dev->_recv_task_prio = AT_WORKER_DEFAULT_PRIORITY;
    }

    if (config->recv_task_stacksize > 0) {
        dev->_recv_task_stacksize = config->recv_task_stacksize;
    } else {
        dev->_recv_task_stacksize = AT_WORKER_DEFAULT_STACK_SIZE;
    }

    snprintf(dev->_task_name, AT_TASK_NAME_MAX_LEN, "at_worker_%d", fd);
    if (atpsr_task_new_ext(&task,
                           dev->_task_name, at_worker,
                           dev->_task_para,
                           dev->_recv_task_stacksize,
                           dev->_recv_task_prio) != 0) {
        atpsr_err("fail to create at task\r\n");
        goto err;
    }

    if (atpsr_sem_wait(dev->_task_start_sem, 1000) != 0) {
        atpsr_err("fail to wait task start sem\r\n");
        goto err;
    }

    slist_init(&dev->_task_l);
#endif

    return 0;
err:
    deinit_dev(fd);

    return -1;
}

/*------------------------------public interface----------------------------------*/
int at_init(void)
{
    if (inited == 1) {
        atpsr_err("AT module Already inited\n");
        return -1;
    }

    memset(at_dev, 0, ATPSR_DEV_MAX_NUM * sizeof(at_dev_t));
    memset(at_dev_ops, 0, AT_DEV_TYPE_MAX * sizeof(at_dev_ops_t *));

    if (register_at_dev_ops() != 0) {
        atpsr_err("AT device register failed\n");
        return -1;
    }

    inited = 1;
}

int at_deinit(void)
{
    int i;

    /* delete all at devices */
    for (i = 0; i < ATPSR_DEV_MAX_NUM; i++) {
        at_delete_dev(i);
    }

    unregister_at_dev_ops();

    inited = 0;
}

int at_add_dev(at_config_t *config)
{
    int fd;

    if (inited == 0) {
        atpsr_err("AT module not inited\n");
        return -1;
    }

    if (check_config_valid(config) != 0)
        return -1;

    if ((fd = assign_dev_fd(config->type)) < 0) {
        atpsr_err("assign dev fd fail\n");
        return -1;
    }

    if (init_dev(fd, config) != 0) {
        atpsr_err("assign dev fd fail\n");
        return -1;
    }

    return fd;
}

int at_delete_dev(int fd)
{
    if (!check_fd_valid(fd))
        return -1;

#if !ATPSR_SINGLE_TASK
    stop_at_worker(fd);
#endif

    deinit_dev(fd);

    return 0;
}

int at_send_no_reply(int fd, const char *data, int datalen, bool delimiter)
{
    int ret = 0;
    at_dev_t *dev = NULL;
    at_dev_ops_t *op = NULL;

    if (inited == 0) {
        atpsr_err("at module have not init yet\r\n");
        return -1;
    }

    if (NULL == data || datalen <= 0) {
        atpsr_err("invalid input \r\n");
        return -1;
    }

    if (!check_fd_valid(fd) || !check_dev_ready(fd))
        return -1;

    dev = obtain_dev_by_fd(fd);
    op = obtain_op_by_type(dev->_type);

    atpsr_mutex_lock(dev->_send_mutex);
    if ((ret = op->send(dev->_dev, (void *)data,
                        datalen, dev->_timeout_ms)) != 0) {
        atpsr_err("uart send raw content (%s) failed", data);
        ret = -1;
        goto done;
    }

    if (delimiter) {
        if ((ret = op->send(dev->_dev, (void *)dev->_send_delimiter,
                            strlen(dev->_send_delimiter), dev->_timeout_ms)) != 0) {
            atpsr_err("uart send delimiter failed");
            ret = -1;
            goto done;
        }
    }

done:
    atpsr_mutex_unlock(dev->_send_mutex);
    return ret;
}

int at_read(int fd, char *outbuf, int readsize)
{
    int      ret = 0;
    uint32_t recv_size, total_read = 0;
    at_dev_t *dev = NULL;
    at_dev_ops_t *op = NULL;

    if (inited == 0) {
        atpsr_err("at module have not init yet\r\n");
        return -1;
    }

    if (!check_fd_valid(fd) || !check_dev_ready(fd))
        return -1;

    dev = obtain_dev_by_fd(fd);
    op = obtain_op_by_type(dev->_type);

    atpsr_mutex_lock(dev->_recv_mutex);
    while (total_read < readsize) {
        ret = op->recv(dev->_dev, (void *)(outbuf + total_read),
                       readsize - total_read, &recv_size, dev->_timeout_ms);
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
    atpsr_mutex_unlock(dev->_recv_mutex);

    if (ret != 0) {
        return -1;
    }

    return total_read;
}

#if ATPSR_SINGLE_TASK
int at_send_wait_reply(int fd ,const char *cmd, int cmdlen, bool delimiter,
                       const char *data, int datalen,
                       char *replybuf, int bufsize,
                       const at_reply_config_t *atcmdconfig)
{
    at_dev_t *dev = NULL;

    if (inited == 0) {
        atpsr_err("at module have not init yet\r\n");
        return -1;
    }

    if (!check_fd_valid(fd) || !check_dev_ready(fd))
        return -1;

    dev = obtain_dev_by_fd(fd);

    if (at_send_no_reply(fd, cmd, cmdlen, delimiter) < 0) {
        return -1;
    }

    if (data && datalen) {
        if (dev->_wait_prompt) {
            at_reply_config_t prompt = {NULL, AT_SEND_DEFAULT_DATA_PROMPT, NULL};
            if (at_yield(fd, NULL, 0, &prompt, dev->_timeout_ms) <  0) {
                return -1;
            }
        }

        if (at_send_no_reply(fd, data, datalen, false) < 0) {
            return -1;
        }
    }

    if (at_yield(fd, replybuf, bufsize, atcmdconfig, dev->_timeout_ms) <  0) {
        return -1;
    }

    return 0;
}
#else
int at_send_wait_reply(int fd, const char *cmd, int cmdlen, bool delimiter,
                       const char *data, int datalen,
                       char *replybuf, int bufsize,
                       const at_reply_config_t *atcmdconfig)
{
    int ret = 0;
    at_dev_t *dev = NULL;
    at_dev_ops_t *op = NULL;

    if (inited == 0) {
        atpsr_err("at module have not init yet\r\n");
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

    if (!check_fd_valid(fd) || !check_dev_ready(fd))
        return -1;

    dev = obtain_dev_by_fd(fd);
    op = obtain_op_by_type(dev->_type);

    atpsr_mutex_lock(dev->_send_mutex);
    at_task_t *tsk = (at_task_t *)atpsr_malloc(sizeof(at_task_t));
    if (NULL == tsk) {
        atpsr_err("tsk buffer allocating failed");
        atpsr_mutex_unlock(dev->_send_mutex);
        return -1;
    }
    memset(tsk, 0, sizeof(at_task_t));

    if (NULL == (tsk->smpr = atpsr_sem_new())) {
        atpsr_err("failed to allocate semaphore");
        goto done;
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
    if (dev->_wait_prompt && data != NULL && datalen > 0)
       tsk->rsp_state = AT_RSP_WAITPROMPT;
    else
       tsk->rsp_state = AT_RSP_PENDING;

    at_worker_task_add(fd, tsk);

    if ((ret = op->send(dev->_dev, (void *)cmd, cmdlen,
                        dev->_timeout_ms)) != 0) {
        atpsr_err("uart send command failed");
        goto done;
    }

    if (delimiter) {
        if ((ret = op->send(dev->_dev, (void *)dev->_send_delimiter,
                            strlen(dev->_send_delimiter), dev->_timeout_ms)) != 0) {
            atpsr_err("uart send delimiter failed");
            goto done;
        }
    }

    if (data != NULL && datalen > 0) {
        if (dev->_wait_prompt) {
            if ((ret = atpsr_sem_wait(tsk->smpr, AT_TASK_DEFAULT_WAIT_TIME_MS)) != 0) {
                atpsr_err("sem_wait failed");
                goto done;
            }
        }

        if ((ret = op->send(dev->_dev, (void *)data, datalen, dev->_timeout_ms)) != 0) {
            atpsr_err("uart send delimiter failed");
            goto done;
        }
    }

    if ((ret = atpsr_sem_wait(tsk->smpr, AT_TASK_DEFAULT_WAIT_TIME_MS)) != 0) {
        atpsr_err("sem_wait failed");
        goto done;
    }

done:
    at_worker_task_del(fd, tsk);
    atpsr_mutex_unlock(dev->_send_mutex);
    return ret;
}
#endif

int at_register_callback(int fd, const char *prefix, const char *postfix,
                         char *recvbuf, int bufsize, at_recv_cb cb, void *arg)
{
    oob_t *oob = NULL;
    int    i   = 0;
    at_dev_t *dev = NULL;

    if (inited == 0) {
        atpsr_err("at module have not init yet\r\n");
        return -1;
    }

    if (bufsize < 0 || bufsize >= RECV_BUFFER_SIZE || NULL == prefix) {
        atpsr_err("%s invalid input \r\n", __func__);
        return -1;
    }

    if (NULL != postfix && (NULL == recvbuf || 0 == bufsize)) {
        atpsr_err("%s invalid postfix input \r\n", __func__);
        return -1;
    }

    if (!check_fd_valid(fd) || !check_dev_ready(fd))
        return -1;

    dev = obtain_dev_by_fd(fd);

    if (dev->_oobs_num >= OOB_MAX) {
        atpsr_err("No place left in OOB.\r\n");
        return -1;
    }

    /* check oob is exit */
    for (i = 0; i < dev->_oobs_num; i++) {
        if (strcmp(prefix, dev->_oobs[i].prefix) == 0) {
            atpsr_debug("oob prefix %s is already exist.\r\n", prefix);
            return -1;
        }
    }

    oob = &(dev->_oobs[dev->_oobs_num++]);

    oob->oobinputdata = recvbuf;
    if (oob->oobinputdata != NULL) {
        memset(oob->oobinputdata, 0, bufsize);
    }
    oob->maxlen  = bufsize;
    oob->prefix  = (char *)prefix;
    oob->postfix = (char *)postfix;
    oob->cb      = cb;
    oob->arg     = arg;
    oob->reallen = 0;

    atpsr_debug("New oob registered (%s)", oob->prefix);
    return 0;
}

int at_yield(int fd, char *replybuf, int bufsize, const at_reply_config_t *atcmdconfig,
             int timeout_ms)
{
#if ATPSR_SINGLE_TASK
    return at_yield_impl(fd, replybuf, bufsize, atcmdconfig, timeout_ms);
#else
    atpsr_err("at_yield should not be called in multiple task mode");
    return -1;
#endif
}
