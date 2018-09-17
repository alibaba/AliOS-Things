/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <aos/aos.h>

#include "at_util.h"
#include "athost_internal.h"
#include "athost_io.h"

#define TAG "AT_UTIL"

typedef struct
{
    uint8_t *cmdptr;
    uint8_t *dataptr;
    uint16_t cmdlen;
    uint16_t datalen;
} uart_send_info_t;

typedef struct
{
    uint32_t total_byte;
    uint32_t fetch_error;
    uint32_t put_error;
    uint32_t send_error;
} uart_send_stat_t;

#define DEFAULT_UART_SEND_BUF_SIZE 50
static aos_queue_t      uart_send_queue;
static uart_send_stat_t uart_send_statistic;

static bool inited;

#define MAX_ATCMD_HANDLERS 10
static int            atcmd_handlers_count               = 0;
static atcmd_op_ptr_t atcmd_handlers[MAX_ATCMD_HANDLERS] = { 0 };

int atcmd_socket_data_len_check(char data)
{
    if (data > '9' || data < '0') {
        return -1;
    }
    return 0;
}

int atcmd_socket_ip_info_check(char data)
{
    if ((data > '9' || data < '0') && data != '.') {
        return -1;
    }

    return 0;
}

int atcmd_socket_conntype_check(char data)
{
    if ((data > 'z' || data < 'a') && data != '_') {
        return -1;
    }

    return 0;
}

static void reverse(char s[])
{
    int  i, j;
    char c;

    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c    = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void itoa_decimal(int n, char s[])
{
    int i, sign;

    if ((sign = n) < 0) {
        n = -n; /* make n positive */
    }
    i = 0;
    do {                       /* generate digits in reverse order */
        s[i++] = n % 10 + '0'; /* get next digit */
    } while ((n /= 10) > 0);   /* delete it */
    if (sign < 0) {
        s[i++] = '-';
    }
    s[i] = '\0';
    reverse(s);
}

// ret: -1 error, 0 more field, 1 no more field
int atcmd_socket_data_info_get(char *buf, uint32_t buflen,
                               at_data_check_cb_t valuecheck)
{
    uint32_t i = 0;

    if (NULL == buf || 0 == buflen) {
        return -1;
    }

    do {
        at_read(&buf[i], 1);
        if (buf[i] == ',') {
            buf[i] = 0;
            break;
        } else if (buf[i] == '\r') {
            LOGD(TAG, "delimiter found\n");
            buf[i] = 0;
            return 1;
        }

        if (i >= buflen) {
            LOGE(TAG, "Too long length of data.reader is %s \r\n", buf);
            return -1;
        }
        if (NULL != valuecheck) {
            if (valuecheck(buf[i])) {
                LOGE(TAG, "Invalid string!!!, reader is %s last char %d\r\n",
                     buf, buf[i]);
                return -1;
            }
        }
        i++;
    } while (1);

    return 0;
}


static int uart_send_queue_init()
{
    uint32_t size = sizeof(uart_send_info_t) * DEFAULT_UART_SEND_BUF_SIZE;
    uart_send_info_t *uart_send_buf = NULL;

    memset(&uart_send_statistic, 0, sizeof(uart_send_statistic));

    uart_send_buf = (uart_send_info_t *)aos_malloc(size);
    if (!uart_send_buf) {
        LOGE(TAG, "uart send buf allocate %u fail!\r\n", size);
        goto err;
    }

    if (aos_queue_new(&uart_send_queue, uart_send_buf, size,
                      sizeof(uart_send_info_t)) != 0) {
        LOGE(TAG, "uart send queue create fail!\r\n");
        goto err;
    }

    return 0;
err:
    aos_free(uart_send_buf);

    aos_queue_free(&uart_send_queue);

    return -1;
}

static int uart_send_queue_finalize()
{
    uart_send_info_t *uart_send_buf = NULL;

    if (!aos_queue_is_valid(&uart_send_queue)) {
        return -1;
    }

    uart_send_buf = (uart_send_info_t *)aos_queue_buf_ptr(&uart_send_queue);
    aos_free(uart_send_buf);

    aos_queue_free(&uart_send_queue);

    return 0;
}

void free_uart_send_msg(uart_send_info_t *msgptr)
{
    if (!msgptr) {
        return;
    }

    aos_free(msgptr->cmdptr);
    aos_free(msgptr->dataptr);
}

// at
int at_read(char *outbuf, uint32_t len)
{
    return athost_at_read(outbuf, len);
}

int insert_uart_send_msg(uint8_t *cmdptr, uint8_t *dataptr, uint16_t cmdlen,
                         uint16_t datalen)
{
    uart_send_info_t uart_send_buf;

    if (!cmdptr || !cmdlen) {
        return -1;
    }

    if (strlen((char *)cmdptr) != cmdlen) {
        LOGE(TAG, "Error: cmd len does not match\r\n");
        return -1;
    }

    if (dataptr && !datalen) {
        return -1;
    }

    if (!aos_queue_is_valid(&uart_send_queue)) {
        return -1;
    }

    memset(&uart_send_buf, 0, sizeof(uart_send_info_t));
    uart_send_buf.cmdptr = (uint8_t *)aos_malloc(cmdlen + 1);
    if (!uart_send_buf.cmdptr) {
        LOGE(TAG, "uart send msg allocate fail\n");
        goto err;
    }

    LOGD(TAG, "insert cmd -->%s<-- len %d  addr %x to %x\n", cmdptr, cmdlen,
         uart_send_buf.cmdptr, uart_send_buf.cmdptr + cmdlen);
    memcpy(uart_send_buf.cmdptr, cmdptr, cmdlen);
    uart_send_buf.cmdptr[cmdlen] = 0;
    uart_send_buf.cmdlen         = cmdlen;

    if (dataptr && datalen) {
        uart_send_buf.dataptr = (uint8_t *)aos_malloc(datalen);
        if (!uart_send_buf.dataptr) {
            LOGE(TAG, "Uart send msg allocate fail\n");
            goto err;
        }

        memcpy(uart_send_buf.dataptr, dataptr, datalen);
        uart_send_buf.datalen = datalen;
    }

    if (aos_queue_send(&uart_send_queue, &uart_send_buf,
                       sizeof(uart_send_buf)) != 0) {
        LOGE(TAG, "Error: Uart queue send fail, total fail %d!\r\n",
             ++uart_send_statistic.put_error);
        goto err;
    }

    uart_send_statistic.total_byte += (cmdlen + datalen);
    LOGD(TAG, "uart cmdlen %d datalen %d total %d\n", cmdlen, datalen,
         uart_send_statistic.total_byte);

    return 0;

err:
    free_uart_send_msg(&uart_send_buf);
    return -1;
}

// return total byte sent
int send_over_uart(uart_send_info_t *msgptr)
{
    int ret;
    int size = 0;

    if (!msgptr) {
        return -1;
    }

    if (!msgptr->cmdptr && !msgptr->dataptr) {
        return -1;
    }

    if (strlen((char *)msgptr->cmdptr) != msgptr->cmdlen) {
        LOGE(TAG, "Error: cmd -->%s<-- len %d does not match!\r\n",
             msgptr->cmdptr, msgptr->cmdlen);
        return -1;
    }

    if (!msgptr->dataptr) {
        LOGD(TAG, "at going to send %s!\n", (char *)msgptr->cmdptr);

        ret = athost_at_write((char *)msgptr->cmdptr, NULL, 0, NULL);
        if (ret != 0) {
            LOGE(TAG, "Error: cmd send fail!\r\n");
            return -1;
        }
        size += msgptr->cmdlen;
    } else {
        LOGD(TAG, "at going to send %s! datelen %d\n", (char *)msgptr->cmdptr,
             msgptr->datalen);

        ret = athost_at_write((const char *)msgptr->cmdptr, msgptr->dataptr,
                              msgptr->datalen, NULL);
        if (ret != 0) {
            LOGE(TAG, "Error: cmd and data send fail!\r\n");
            return -1;
        }
        size += (msgptr->cmdlen + msgptr->datalen);
    }

    return size;
}

// AT reponse output
// all uart send should go through this task
void uart_send_task()
{
    int              ret;
    uint32_t         size, sent_size;
    uart_send_info_t msg;

    LOG("uart send task start!\r\n");

    while (true) {
        if (!inited) {
            goto exit;
        }

        if (!aos_queue_is_valid(&uart_send_queue)) {
            LOGE(TAG, "Error uart send queue invalid!");
            goto exit;
        }

        memset(&msg, 0, sizeof(uart_send_info_t));
        ret = aos_queue_recv(&uart_send_queue, AOS_WAIT_FOREVER, &msg, &size);
        if (ret != 0) {
            LOGE(TAG,
                 "Error uart send queue recv, error_no %d, total fetch error "
                 "%d\r\n",
                 ret, ++uart_send_statistic.fetch_error);
            goto done;
        }

        if (size != sizeof(uart_send_info_t)) {
            LOGE(TAG, "Error uart send recv: msg size %d is not valid\r\n",
                 size);
            goto done;
        }

        if ((sent_size = send_over_uart(&msg)) < 0) {
            LOGE(TAG, "Error uart send fail, total send error %d\t\n",
                 ++uart_send_statistic.send_error);
            goto done;
        }

    done:

        if (uart_send_statistic.total_byte >= (msg.datalen + msg.cmdlen)) {
            uart_send_statistic.total_byte -= (msg.datalen + msg.cmdlen);
            LOGD(TAG, "uart send queue remain size %d \r\n",
                 uart_send_statistic.total_byte);
        } else {
            LOGE(TAG, "Error: uart send queue remain %d sent %d \r\n",
                 uart_send_statistic.total_byte, msg.datalen + msg.cmdlen);

            uart_send_statistic.total_byte = 0;
        }

        free_uart_send_msg(&msg);
    }

exit:
    LOG("Uart send task exits!\r\n");
    aos_task_exit(0);
}

void send_at_uart_task(void *arg)
{
    if (!arg) {
        goto exit;
    }

    LOGD(TAG, "at going to send %s!\n", (char *)arg);

    athost_at_write((char *)arg, NULL, 0, NULL);
exit:
    aos_free(arg);
    aos_task_exit(0);
}

static int post_send_at_uart_task(const char *cmd)
{
    int   size   = strlen(cmd) + 1;
    char *tskarg = NULL;

    tskarg = (char *)aos_malloc(size);
    if (tskarg == NULL) {
        LOGE(TAG, "Fail to allcate memory %d byte for uart send task arg\r\n",
             size);
        goto exit;
    }
    memcpy(tskarg, cmd, size);

    if (aos_task_new("uart_send_task", send_at_uart_task, (void *)tskarg,
                     1024) != 0) {
        LOGE(TAG, "Fail to create uart send task\r\n");
        goto exit;
    }

    return 0;

exit:
    aos_free(tskarg);
    return -1;
}

#define MAX_ATCMD_RESPONSE_LEN 20
int notify_atcmd_recv_status(int status)
{
    int   offset = 0;
    char *status_str;
    char  response[MAX_ATCMD_RESPONSE_LEN] = { 0 };

    // prefix
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_RESPONSE_LEN) {
        offset += snprintf(response + offset, MAX_ATCMD_RESPONSE_LEN - offset,
                           "%s", AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    if (status == ATCMD_FAIL) {
        status_str = AT_RECV_FAIL_POSTFIX;
    } else if (status == ATCMD_SUCCESS) {
        status_str = AT_RECV_SUCCESS_POSTFIX;
    } else {
        LOGE(TAG, "unknown status\n", response);
        goto err;
    }

    // status
    if (offset + strlen(status_str) < MAX_ATCMD_RESPONSE_LEN) {
        offset += snprintf(response + offset, MAX_ATCMD_RESPONSE_LEN - offset,
                           "%s", status_str);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "Error insert uart send msg fail\r\n");
        goto err;
    }

    return 0;
err:
    return -1;
}

#define MAX_ATCMD_CME_ERR_RSP_LEN 30
int notify_atcmd_cme_error(int error_no)
{
    const char *cme_error_preifx                    = "+CME ERROR:";
    int         offset                              = 0;
    char        response[MAX_ATCMD_CME_ERR_RSP_LEN] = { 0 };

    if (error_no < 0 || error_no > 65535) {
        LOGE(TAG, "invalid CME error_no %d\n", error_no);
        goto err;
    }

    // prefix
    if (offset + strlen(AT_RECV_PREFIX) < MAX_ATCMD_CME_ERR_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_CME_ERR_RSP_LEN - offset, "%s",
                   AT_RECV_PREFIX);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    // status
    if (offset + strlen(cme_error_preifx) + 7 < MAX_ATCMD_CME_ERR_RSP_LEN) {
        offset +=
          snprintf(response + offset, MAX_ATCMD_CME_ERR_RSP_LEN - offset,
                   "%s%d\r\n", cme_error_preifx, error_no);
    } else {
        LOGE(TAG, "at string too long %s\n", response);
        goto err;
    }

    if (insert_uart_send_msg(response, NULL, strlen(response), 0) != 0) {
        LOGE(TAG, "Error insert uart send msg fail\r\n");
        goto err;
    }

    return 0;
err:
    return -1;
}


int uart_send_task_init()
{
    aos_task_t  task;
    if (uart_send_queue_init() != 0) {
        LOGE(TAG, "Creating uart send que fail (%s %d).", __func__, __LINE__);
        goto err;
    }

    inited = true;

    if (aos_task_new_ext(&task, "athost_uart_send_task", uart_send_task,
                         NULL, 1024, AOS_DEFAULT_APP_PRI - 1) != 0) {
        LOGE(TAG, "Fail to create uart send task\r\n");
        goto err;
    }

    return 0;

err:
    uart_send_queue_finalize();

    inited = false;

    return -1;
}

void uart_send_task_deinit()
{
    uart_send_queue_finalize();

    inited = false;
}

int atcmd_register_handlers(atcmd_op_ptr_t handler)
{
    if (atcmd_handlers_count >= MAX_ATCMD_HANDLERS) {
        LOGE(TAG, "atcmd handlers exceeds limit\n");
        return -1;
    }

    if (atcmd_handlers[atcmd_handlers_count] != NULL) {
        LOGE(TAG, "atcmd handlers overwrite\n");
        return -1;
    }

    atcmd_handlers[atcmd_handlers_count] = handler;
    atcmd_handlers_count++;

    return 0;
}

void atcmd_handle()
{
    char            single;
    char            one_more;
    char            prefix[MAX_ATCMD_PREFIX_LEN] = { 0 };
    atcmd_hdl_ptr_t handler                      = NULL;
    LOGD(TAG, "%s entry.", __func__);

    // uart_echo();
    at_read(&single, 1);
    one_more = 0;

    // search handler
    for (int i = 0; i < atcmd_handlers_count; i++) {
        if (atcmd_handlers[i] != NULL &&
            atcmd_handlers[i]->prefix[0] == single) {
            if (strlen(atcmd_handlers[i]->prefix) == 1) {
                handler = atcmd_handlers[i]->get_atcmd_handler();
            } else if (strlen(atcmd_handlers[i]->prefix) == 2) {
                if (one_more == 0)
                    at_read(&one_more, 1);

                if (atcmd_handlers[i]->prefix[1] == one_more) {
                    handler = atcmd_handlers[i]->get_atcmd_handler();
                }
            } else {
                LOGE(TAG, "Not support long prefix check! %s\n",
                     atcmd_handlers[i]->prefix);
            }
        }
    }

    if (handler != NULL) {
        if (handler->function() < 0) {
            LOGE(TAG, "atcmd %s execution fail!\n", handler->name);
        }
    } else {
        LOGE(TAG, "atcmd handler not found!\n");
    }

    LOGD(TAG, "%s exit.", __func__);
}

int atcmd_handle_init()
{
    if (uart_send_task_init() < 0) {
        LOGE(TAG, "uart send task init!\n");
        goto err;
    }

    for (int i = 0; i < atcmd_handlers_count; i++) {
        if (atcmd_handlers[i]->init() < 0) {
            LOGE(TAG, "atcmd %s init fail!\n", atcmd_handlers[i]->name);
            return -1;
        }
    }

    inited = true;

    return 0;

err:
    uart_send_task_deinit();

    for (int i = 0; i < atcmd_handlers_count; i++) {
        atcmd_handlers[i]->deinit();
    }

    inited = false;

    return -1;
}
