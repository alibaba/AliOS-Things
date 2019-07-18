/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include <be_jse_module.h>
#include "be_jse_api.h"
#include "be_jse_task.h"
#include "be_port_osal.h"
#include "hal/system.h"
#include "module_http.h"

#define CONFIG_LOGMACRO_DETAILS 1
#define JS_HTTP_TAG "HTTP"

typedef struct {
    int port;
    char hostname[128];
    char filepath[128];
} http_uri_t;

typedef enum {
    HTTP_REQUEST = 1,
    ERROR,
} schedule_msg_id_t;

typedef struct {
    void *arg1;
    void *arg2;
    int id;
    be_jse_symbol_t *func;
} schedule_msg_t;

void http_parse_url(char *buf, char *host, int *port, char *file_name)
{
    int length = 0;
    char port_buf[8];
    char *buf_end = (char *)(buf + strlen(buf));
    char *begin, *host_end, *colon, *file;

    /* 查找主机的开始位置 */
    begin = strstr(buf, "//");
    begin = begin ? begin + 2 : buf;

    colon    = strchr(begin, ':');
    host_end = strchr(begin, '/');

    if (host_end == NULL) {
        host_end = buf_end;
    } else { /* 得到文件名 */
        file = strrchr(host_end, '/');
        if (file && (file + 1) != buf_end) strcpy(file_name, file + 1);
    }
    if (colon) /* 得到端口号 */
    {
        colon++;

        length = host_end - colon;
        memcpy(port_buf, colon, length);
        port_buf[length] = 0;
        *port            = atoi(port_buf);

        host_end = colon - 1;
    }

    /* 得到主机信息 */
    length = host_end - begin;
    memcpy(host, begin, length);
    host[length] = 0;
}

static int http_init_connnection(char *hostname, int port)
{
    struct addrinfo hints;
    int ret              = -1;
    int sockfd           = 0;
    struct addrinfo *res = NULL;
    char s_port[10]      = {0};

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    itoa(port, s_port, 10);
    ret = getaddrinfo(hostname, s_port, &hints, &res);

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd < 0) return -1;

    ret = connect(sockfd, res->ai_addr, res->ai_addrlen);
    if (ret != 0) return -1;

    return sockfd;
}

static int http_build_header(char *hostname, char *file_path, char *http_buffer)
{
    int len = 0;

    if (strlen(file_path) == 0) {
        len = sprintf(http_buffer, "GET %s HTTP/1.0\r\n",
                      "/"); /* default get root dirctory */
    } else {
        len = sprintf(http_buffer, "GET %s HTTP/1.0\r\n", file_path);
    }

    len += sprintf(http_buffer + len, "Host: %s\r\n", hostname);
    len += sprintf(http_buffer + len, "Connection: close\r\n\r\n");

    return 0;
}

static int http_send(int sockfd, char *buffer)
{
    int bytes_sent = 0;

    int size = strlen(buffer);

    while (size > 0) {
        bytes_sent = send(sockfd, buffer, size, 0);
        if (bytes_sent <= 0) break;

        size   = size - bytes_sent;
        buffer = buffer + bytes_sent;
    }

    return size;
}

static int http_recv(int sockfd, char *buffer)
{
    size_t bytes_received = 0;
    size_t total_received = 0;
    int status            = 0;

    be_debug(JS_HTTP_TAG, "Receiving data ...");

    while (total_received < HTTP_BUFF_SIZE) {
        bytes_received =
            recv(sockfd, buffer, (HTTP_BUFF_SIZE - total_received), 0);

        if (bytes_received == -1) {
            return -1;
        } else if (bytes_received == 0) {
            be_debug(JS_HTTP_TAG, "http_recv finish size=%d\n\r",
                     total_received);
            return 0;
        }

        if (bytes_received > 0) {
            buffer = buffer + bytes_received;
        }
        total_received += bytes_received;
    }

    be_debug(JS_HTTP_TAG, "Finished receiving data =%d.", total_received);

    if (total_received <= 0) {
        return -1; /* http received fail */
    } else {
        return 0; /* http request and received ok */
    }
}

static int http_request_and_recv(char *url, char *http_buffer)
{
    int socketid      = -1;
    int ret           = -1;
    http_uri_t *p_uri = NULL;

    p_uri = calloc(1, sizeof(http_uri_t));
    if (!p_uri) goto done;

    p_uri->port = 80; /* default http port if no specify */

    http_parse_url(url, p_uri->hostname, &(p_uri->port), p_uri->filepath);
    socketid = http_init_connnection(p_uri->hostname, p_uri->port);
    if (socketid < 0) goto done;

    ret = http_build_header(p_uri->hostname, p_uri->filepath, http_buffer);
    ret = http_send(socketid, http_buffer);
    if (ret != 0) goto done;

    ret = http_recv(socketid, http_buffer);

done:
    if (socketid > 0) close(socketid);
    if (p_uri) free(p_uri);
    return ret;
}

/*C call JS 回调*/
static void js_cb_http_recv(void *pdata)
{
    int ret             = 0;
    schedule_msg_t *msg = (schedule_msg_t *)pdata;

    BE_ASYNC_S *async  = (BE_ASYNC_S *)calloc(1, sizeof(BE_ASYNC_S));
    async->func        = msg->func;
    async->param_count = 1;
    async->params      = (be_jse_symbol_t **)calloc(
        1, sizeof(be_jse_symbol_t *) * async->param_count);
    async->params[0] = new_str_symbol(msg->arg2);

    /*已经是BoneEngine线程，无须再使用be_osal_schedule_call*/
    be_jse_async_event_cb(async);

    free(msg->arg1);
    free(msg->arg2);
    free(msg);
}

/* 说明: 创建一个task任务，用于http request请求 */
static void task_http_request_fun(void *arg)
{
    int ret                         = -1;
    char *url                       = NULL;
    char *http_buf                  = NULL;
    be_jse_symbol_t *js_http_req_cb = NULL;
    schedule_msg_t *msg             = (schedule_msg_t *)arg;

    if (msg == NULL || msg->id != HTTP_REQUEST) {
        be_osal_delete_task(NULL);
        return;
    }

    url            = msg->arg1;
    http_buf       = msg->arg2;
    js_http_req_cb = msg->func;
    be_osal_delay(50); /* need do things after state changed in main task */

    /* 阻塞式的send和recv */
    ret = http_request_and_recv(url, http_buf);
    if (ret != 0) {
        memset(http_buf, 0, HTTP_BUFF_SIZE);
        sprintf(http_buf, "%s", "ERROR");
    }

    be_jse_task_schedule_call(js_cb_http_recv, msg);
    be_osal_delete_task(NULL);
}

/*****************************************************************************
*Function:    module_http_request
*Description: js native addon for HTTP.request(url,function(data){});
*Called by:   js api
*Input:       url: STRING object like http://www.baidu.com
                          functon(data) is a function object which is the
callback data is String Object which is the result http request, when data is
"ERROR" means http request error *Output:      None
*****************************************************************************/

static be_jse_symbol_t *module_http_request()
{
    int ret               = -1;
    be_jse_symbol_t *arg0 = NULL;
    be_jse_symbol_t *arg1 = NULL;

    char *url                    = NULL;
    char *http_buffer            = NULL;
    schedule_msg_t *schedule_msg = NULL;

    be_jse_handle_function(0, &arg0, &arg1, NULL, NULL);
    if (arg0 == NULL || !symbol_is_string(arg0)) {
        be_error(JS_HTTP_TAG, "%s arg invalid0\n\r", __FUNCTION__);
        goto done;
    }

    url = calloc(1, symbol_str_len(arg0) + 1);
    if (!url) goto done;

    symbol_to_str(arg0, url, symbol_str_len(arg0));

    http_buffer = calloc(1, (HTTP_BUFF_SIZE + 1));
    if (!http_buffer) {
        free(url);
        goto done;
    }

    schedule_msg = (schedule_msg_t *)calloc(1, sizeof(schedule_msg_t));
    if (!schedule_msg) {
        free(http_buffer);
        free(url);
        goto done;
    }

    schedule_msg->id   = HTTP_REQUEST;
    schedule_msg->arg1 = url;
    schedule_msg->arg2 = http_buffer;
    schedule_msg->func = arg1;

    INC_SYMBL_REF(arg1); /* 必须增加引用，否则在函数退出时符号表会被释放，在be_jse_execute_func中DEC_SYMBL_REF
                          */

    be_osal_create_task("http_request", task_http_request_fun, schedule_msg,
                        4096, ADDON_TSK_PRIORRITY, NULL);

done:
    symbol_unlock(arg0);
    symbol_unlock(arg1);
    return new_int_symbol(ret);
}

static be_jse_symbol_t *module_handle_cb(be_jse_vm_ctx_t *execInfo,
                                         be_jse_symbol_t *var, const char *name)
{
    be_debug(JS_HTTP_TAG, "%s Enter: \n\r", __FUNCTION__);
    if (strcmp(name, "request") == 0) return module_http_request();

    return BE_JSE_FUNC_UNHANDLED;
}

void module_http_register(void)
{
    int ret = -1;

    be_jse_module_load(JS_HTTP_TAG, module_handle_cb);
}
