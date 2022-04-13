/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "amp_system.h"
#include "amp_network.h"
#include "amp_defines.h"
#include "amp_network.h"
#include "amp_socket.h"
#include "amp_task.h"
#include "be_inl.h"
#include "http.h"

#define MOD_STR                     "HTTP"
#define HTTP_BUFF_SIZE              2048
#define HTTP_HEADER_SIZE            64
#define HTTP_HEADER_COUNT           8
#define HTTP_SEND_RECV_TIMEOUT      10000
#define HTTP_REQUEST_TIMEOUT        30000
#define HTTP_DEFAULT_HEADER_NAME    "content-type"
#define HTTP_DEFAULT_HEADER_DATA    "application/json"

typedef struct
{
    char *name;
    char *data;
} http_header_t;

typedef struct
{
    char *url;
    int method;
    http_header_t http_header[HTTP_HEADER_COUNT];
    uint32_t timeout;
    char *buffer;
    int js_cb_ref;
} http_param_t;

typedef struct
{
    int port;
    char hostname[128];
    char filepath[128];
} http_uri_t;

uint8_t req_buf[HTTP_BUFF_SIZE];
uint8_t rsp_buf[HTTP_BUFF_SIZE];
httpc_handle_t httpc_handle = 0;
static int http_header_index = 0;

static void parse_url(const char *url, char *uri)
{
    char url_dup[128] = {0};

    if (url == NULL) {
        amp_warn(MOD_STR, "url is null");
        return;
    }
    memcpy(url_dup, url, strlen(url));
    char *start = NULL;
    char *p_slash = NULL;

    #if CONFIG_HTTP_SECURE
    const char *protocol = "https";
    #else
    const char *protocol = "http";
    #endif

    if (strncmp(url_dup, protocol, strlen(protocol)) == 0)
    {
        start = url_dup + strlen(protocol) + 3;
        p_slash = strchr(start, '/');
        if (p_slash != NULL)
        {
            memcpy(uri, p_slash, strlen(p_slash));
            *p_slash = '\0';
        }
        else
        {
            memcpy(uri, '/', 1);
        }
    }
}

static int http_request_and_recv(char *url, int method, char *hdr, uint32_t timeout, char *http_buffer)
{
    int ret = -1;
    char uri[HTTP_HEADER_SIZE] = {0};
    http_rsp_info_t rsp_info;
    
    parse_url(url, uri);
    amp_debug(MOD_STR, "%d uri is: %s", __LINE__, uri);
    ret = httpc_send_request(httpc_handle, method, uri, hdr, NULL, NULL, 0);
    if (ret != HTTP_SUCCESS)
    {
        amp_warn(MOD_STR, "http request fail");
        return ret;
    }

    ret = httpc_recv_response(httpc_handle, rsp_buf, HTTP_BUFF_SIZE, &rsp_info, timeout);
    if (ret < 0)
    {
        amp_warn(MOD_STR, "http response fail");
        return ret;
    }

    amp_debug(MOD_STR, "rsp length is: %d, content is: %s", rsp_info.rsp_len, rsp_info.body_start);
    memcpy(http_buffer, rsp_info.body_start, rsp_info.rsp_len);
    ret = 0;

    return ret;
}

static void http_recv_notify(void *pdata)
{
    int i = 0;
    http_param_t *msg = (http_param_t *)pdata;
    duk_context *ctx = be_get_context();
    be_push_ref(ctx, msg->js_cb_ref);
    duk_push_string(ctx, msg->buffer);
    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);
    be_unref(ctx, msg->js_cb_ref);
    amp_free(msg->buffer);
    amp_free(msg);

    duk_gc(ctx, 0);
}

/* create task for http request */
static void *task_http_request_fun(void *arg)
{
    int ret = -1;
    int socketid = -1;
    int i;
    char *url = NULL;
    uint32_t timeout = 0;
    int http_method = 0;
    char *http_buf = NULL;
    char hdr[HTTP_HEADER_SIZE] = {0};
    httpc_connection_t settings;
    http_param_t *msg = (http_param_t *)arg;

    url = msg->url;
    timeout = msg->timeout;
    http_buf = msg->buffer;
    
    HAL_SleepMs(50); /* need do things after state changed in main task */

    http_method = msg->method;

    /* inti http client */
    http_client_initialize();
    /* create socket */
    socketid = httpc_wrapper_socket_create();

    settings.socket = socketid;
    settings.server_name = url;
    settings.req_buf = req_buf;
    settings.req_buf_size = HTTP_BUFF_SIZE;

    httpc_handle = httpc_init(&settings);

    // if (httpc_handle == 0)
    // {
    //     amp_warn(MOD_STR, "http session init fail");
    //     return ret;
    // }

    for (i = 0; i < http_header_index; i++) {
        ret = httpc_construct_header(hdr, HTTP_HEADER_SIZE, msg->http_header[i].name, msg->http_header[i].data);
        if (ret < 0) {
            amp_warn(MOD_STR, "http construct header fail");
            goto out;
        }
    }
    http_header_index = 0;
    /* Blocking send & recv */
    ret = http_request_and_recv(url, http_method, hdr, timeout, http_buf);
    if (ret != 0)
    {
        memset(http_buf, 0, HTTP_BUFF_SIZE);
        sprintf(http_buf, "%s", "ERROR");
    }

    httpc_wrapper_socket_close(socketid);

    httpc_deinit(httpc_handle);
    
    httpc_handle = 0;

    amp_task_schedule_call(http_recv_notify, msg);
    return NULL;

out:
    httpc_wrapper_socket_close(socketid);
    httpc_deinit(httpc_handle);
    httpc_handle = 0;
    if (msg) {
        amp_free(msg->buffer);
        amp_free(msg);
    }

    HAL_ThreadDelete(NULL);

    return NULL;
}

static duk_ret_t native_http_request(duk_context *ctx)
{
    http_param_t *http_param = NULL;
    char *http_buffer = NULL;
    const char *method = NULL;
    int http_method = 0;
    int timeout = 0;
    char localip[32];
    int i;
    void *http_task;
    int http_task_stack_used;
    amp_os_thread_param_t http_task_params = {0};

    if (!duk_is_object(ctx, 0))
    {
        amp_warn(MOD_STR, "invalid parameter\n");
        goto done;
    }

    if (HAL_Get_ip(localip) != 0)
    {
        amp_warn(MOD_STR, "network not ready\r\n");
        goto done;
    }

    http_param = (http_param_t *)amp_malloc(sizeof(http_param_t));
    if (!http_param)
    {
        amp_warn(MOD_STR, "allocate memory failed\n");
        goto done;
    }

    http_buffer = amp_malloc(HTTP_BUFF_SIZE + 1);
    if (!http_buffer)
    {
        amp_warn(MOD_STR, "allocate memory failed\n");
        goto done;
    }
    memset(http_buffer, 0, HTTP_BUFF_SIZE + 1);
    http_param->buffer = http_buffer;

    /* get http request url */
    duk_get_prop_string(ctx, 0, "url");
    if (!duk_is_string(ctx, -1)) {
        amp_debug(MOD_STR, "request url is invalid");
        goto done;
    }
    http_param->url = duk_get_string(ctx, 1);
    duk_pop(ctx);

    /* get http request method */
    if (duk_get_prop_string(ctx, 0, "method")) {
        if (duk_is_string(ctx, -1)) {
            method = duk_get_string(ctx, -1);
            if(strcmp(method, "GET") == 0) {
                http_method = HTTP_GET; /* GET */
            }
            else if(strcmp(method, "POST") == 0) {
                http_method = HTTP_POST; /* POST */
            }
            else if(strcmp(method, "PUT") == 0) {
                http_method = HTTP_PUT; /* PUT */
            }
            else {
                http_method = HTTP_GET;
            }

            http_param->method = http_method;
        }
    } else {
        http_param->method = HTTP_GET;
    }
    duk_pop(ctx);

    /* get http request timeout */
    if (duk_get_prop_string(ctx, 0, "timeout")) {
        if (duk_is_number(ctx, -1)) {
            timeout = duk_get_number(ctx, -1);
            http_param->timeout = timeout;
        } else {
            http_param->timeout = HTTP_REQUEST_TIMEOUT;
        }
    } else {
        http_param->timeout = HTTP_REQUEST_TIMEOUT;
    }
    
    if (http_param->timeout <= 0) {
        http_param->timeout = HTTP_REQUEST_TIMEOUT;
    }
    duk_pop(ctx);

    /* get http request headers */
    if (duk_get_prop_string(ctx, 0, "headers")) {
        duk_enum(ctx, -1, DUK_ENUM_OWN_PROPERTIES_ONLY);
        while (duk_next(ctx, -1, 1))
        {
            // amp_debug(MOD_STR, "key=%s, value=%s ", duk_to_string(ctx, -2), duk_to_string(ctx, -1));
            if (!duk_is_string(ctx, -2) || !duk_is_string(ctx, -1)) {
                amp_debug(MOD_STR, "get header failed, index is: %d", http_header_index);
                break;
            }
            http_param->http_header[http_header_index].name = duk_to_string(ctx, -2);
            http_param->http_header[http_header_index].data = duk_to_string(ctx, -1);
            http_header_index++;
            duk_pop_2(ctx);
        }
    } else {
        http_param->http_header[0].name = HTTP_DEFAULT_HEADER_NAME;
        http_param->http_header[0].data = HTTP_DEFAULT_HEADER_DATA;
        http_header_index++;
    }

    amp_debug(MOD_STR, "url: %s", http_param->url);
    amp_debug(MOD_STR, "method: %d", http_param->method);
    amp_debug(MOD_STR, "timeout: %d", http_param->timeout);
    for (i = 0; i < http_header_index; i++) {
        amp_debug(MOD_STR, "headers: %s:%s", http_param->http_header[i].name, http_param->http_header[i].data);
    }

    /* callback */
    if (duk_get_prop_string(ctx, 0, "success")) {
        duk_dup(ctx, -1);
        http_param->js_cb_ref = be_ref(ctx);
    } else {
        http_param->js_cb_ref = -1;
    }


    http_task_params.name = "amp http task";
    http_task_params.priority = ADDON_TSK_PRIORRITY;
    http_task_params.stack_size = 1024 * 4;
    HAL_ThreadCreate(&http_task, task_http_request_fun, http_param, &http_task_params, &http_task_stack_used);

    duk_push_int(ctx, 0);
    return 1;

done:
    if (http_buffer)
        amp_free(http_buffer);
    if (http_param)
        amp_free(http_param);

    duk_push_int(ctx, -1);
    return 1;
}

void module_http_register(void)
{
    duk_context *ctx = be_get_context();
    duk_push_object(ctx);

    /* request */
    duk_push_c_function(ctx, native_http_request, 1);
    duk_put_prop_string(ctx, -2, "request");

    duk_put_prop_string(ctx, -2, "HTTP");
}
