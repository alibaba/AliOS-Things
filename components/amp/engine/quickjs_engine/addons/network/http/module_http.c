/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "amp_config.h"
#include "amp_platform.h"
#include "aos_system.h"
#include "aos_network.h"
#include "amp_defines.h"
#include "aos_network.h"
#include "aos_socket.h"
#include "aos_httpc.h"
#include "httpclient.h"
#include "aos_tcp.h"
#include "amp_task.h"
#include "cJSON.h"
#include "aos/list.h"
#include "quickjs.h"
#include "quickjs_addon_common.h"

#define MOD_STR "HTTP"
#define HTTP_BUFF_SIZE 2048
#define HTTP_HEADER_SIZE 1024
#define HTTP_HEADER_COUNT 8
#define HTTP_REQUEST_PARAMS_LEN_MAX 2048
#define HTTP_SEND_RECV_TIMEOUT 10000
#define HTTP_REQUEST_TIMEOUT 30000
#define HTTP_DEFAULT_HEADER_NAME "content-type"
#define HTTP_DEFAULT_HEADER_DATA "application/json"
#define HTTP_CRLF "\r\n"

#ifndef countof
#define countof(x) (sizeof(x) / sizeof((x)[0]))
#endif

static JSClassID js_http_class_id;
static int http_header_index = 0;
typedef struct {
    char *name;
    char *data;
} http_header_t;

typedef struct {
    char *url;
    char *filepath;
    int method;
    int content_size;
    http_header_t http_header[HTTP_HEADER_COUNT];
    uint32_t timeout;
    char *params;
    int params_len;
    char *buffer;
    char *header_buffer;
    int error;
    JSValue js_cb_ref;
} http_param_t;

/**************** http download ******************/
typedef struct {
    http_param_t *http_param;
    int index;
} http_download_param_t;

typedef struct {
    JSValue js_cb_ref;
    int error;
} http_download_resp_t;

typedef struct {
    http_download_param_t *dload_param;
    dlist_t node;
} http_dload_node_t;

#define HTTP_HEADER \
    "GET /%s HTTP/1.1\r\nAccept:*/*\r\n" \
    "User-Agent: Mozilla/5.0\r\n" \
    "Cache-Control: no-cache\r\n" \
    "Connection: close\r\n" \
    "Host:%s:%d\r\n\r\n"

#define TCP_WRITE_TIMEOUT 3000
#define TCP_READ_TIMEOUT 3000

#define HTTP_ERROR_NET 1
#define HTTP_ERROR_FS 2

static dlist_t g_dload_list = AOS_DLIST_HEAD_INIT(g_dload_list);

static int dload_list_count = 0;

int32_t httpc_construct_header(char *buf, uint16_t buf_size, const char *name, const char *data)
{
    uint32_t hdr_len;
    uint32_t hdr_data_len;
    int32_t hdr_length;

    if (buf == NULL || buf_size == 0 || name == NULL || data == NULL) {
        return HTTP_EARG;
    }

    hdr_len = strlen(name);
    hdr_data_len = strlen(data);
    hdr_length = hdr_len + hdr_data_len + 4;

    if (hdr_length < 0 || hdr_length > buf_size) {
        return HTTP_ENOBUFS;
    }

    memcpy(buf, name, hdr_len);
    buf += hdr_len;
    memcpy(buf, ": ", 2);
    buf += 2;
    memcpy(buf, data, hdr_data_len);
    buf += hdr_data_len;
    memcpy(buf, HTTP_CRLF, 2);

    return hdr_length;
}

static void http_request_notify(void *pdata)
{
    http_param_t *msg = (http_param_t *)pdata;
    JSContext *ctx = js_get_context();
    JSValue args[2];
    int num = 1;
    args[0] = JS_NewString(ctx, msg->buffer);
    if (msg->header_buffer) {
        args[1] = JS_NewString(ctx, msg->header_buffer);
        num = 2;
    }
    JSValue val = JS_Call(ctx, msg->js_cb_ref, JS_UNDEFINED, num, args);
    JS_FreeValue(ctx, args[0]);
    if (msg->header_buffer) {
        JS_FreeValue(ctx, args[1]);
    }
    JS_FreeValue(ctx, msg->js_cb_ref);
    if (JS_IsException(val)) {
        amp_info(MOD_STR, "http_request_notify callback error");
    }
    JS_FreeValue(ctx, val);
    amp_free(msg->buffer);
    amp_free(msg);
}

/* create task for http download */
static int http_download_func(http_param_t *param)
{
    httpclient_t client = { 0 };
    httpclient_data_t client_data = { 0 };
    int num = 0;
    int wirte_file_len = 0;
    int ret = 0;
    int rtn = 0;
    char *customer_header = NULL;

    if (param == NULL) {
        return HTTP_ERROR_FS;
    }

    if (param->filepath == NULL) {
        ret = HTTP_ERROR_FS;
        amp_warn(MOD_STR, "filepath empty\n");
        goto exit;
    }

    char *req_url = param->url;
    int fd = aos_open(param->filepath, O_CREAT | O_RDWR);

    client_data.header_buf = amp_calloc(HTTP_BUFF_SIZE, 1);
    client_data.header_buf_len = HTTP_BUFF_SIZE;

    client_data.response_buf = amp_calloc(HTTP_BUFF_SIZE, 1);
    client_data.response_buf_len = HTTP_BUFF_SIZE;

    aos_msleep(50); /* need do things after state changed in main task */

    customer_header = amp_calloc(HTTP_HEADER_SIZE, 1);
    for (int i = 0; i < 8; i++) {
        if ((param->http_header[i].name != NULL) && (param->http_header[i].data != NULL)) {
            httpc_construct_header(customer_header, HTTP_HEADER_SIZE, param->http_header[i].name,
                                   param->http_header[i].data);
            amp_free(param->http_header[i].name);
            amp_free(param->http_header[i].data);
        }
    }
    httpclient_set_custom_header(&client, customer_header);

    ret = httpclient_conn(&client, req_url);
    if (ret == HTTP_SUCCESS) {
        ret = httpclient_send(&client, req_url, HTTP_GET, &client_data);
        if (ret == HTTP_SUCCESS) {
            do {
                rtn = 0;
                memset(client_data.response_buf, 0, HTTP_BUFF_SIZE);
                ret = httpclient_recv(&client, &client_data);
                if (ret < HTTP_SUCCESS) {
                    amp_warn(MOD_STR, "http recv file %s error %d\n", param->filepath, ret);
                    rtn = HTTP_ERROR_NET;
                    break;
                }

                num = aos_write(fd, client_data.response_buf, client_data.content_block_len);
                if (num < 0) {
                    amp_warn(MOD_STR, "write file %s error\n", param->filepath);
                    rtn = HTTP_ERROR_FS;
                    break;
                }
                wirte_file_len += num;
            } while (client_data.is_more);
        } else {
            amp_warn(MOD_STR, "http send error %d\n", ret);
        }
    } else {
        amp_warn(MOD_STR, "http conn error %d\n", ret);
    }
    amp_debug(MOD_STR, "write file %s size: %d\n", param->filepath, wirte_file_len);
    ret = aos_sync(fd);
    param->buffer = amp_malloc(32);
    strcpy(param->buffer, "http download success");
    httpclient_clse(&client);

exit:
    if (customer_header != NULL) {
        amp_free(customer_header);
    }
    if (client_data.header_buf) {
        amp_free(client_data.header_buf);
    }
    if (client_data.response_buf) {
        amp_free(client_data.response_buf);
    }

    if (param->url != NULL) {
        amp_free(param->url);
    }

    if (param->filepath != NULL) {
        amp_free(param->filepath);
    }

    if (param->params != NULL) {
        amp_free(param->params);
    }

    param->error = rtn;

    return rtn;
}

static void task_http_download(void *arg)
{
    http_download_func(arg);
    amp_task_schedule_call(http_request_notify, arg);
    aos_task_exit(0);
}

/* create task for http request */
static void task_http_request_func(void *arg)
{
    int ret = 0;
    char *customer_header = NULL;
    httpclient_t client = { 0 };
    httpclient_data_t client_data = { 0 };
    http_param_t *param = (http_param_t *)arg;

    if (param == NULL) {
        amp_error(MOD_STR, "param is NULL");
        return;
    }

    amp_info(MOD_STR, "task_http_request_func url: %s\n", param->url);
    amp_info(MOD_STR, "task_http_request_func method: %d", param->method);
    amp_info(MOD_STR, "task_http_request_func timeout: %d", param->timeout);

    client_data.header_buf = amp_calloc(HTTP_BUFF_SIZE, 1);
    client_data.header_buf_len = HTTP_BUFF_SIZE;

    client_data.response_buf = amp_calloc(HTTP_BUFF_SIZE, 1);
    client_data.response_buf_len = HTTP_BUFF_SIZE;
    aos_msleep(50); /* need do things after state changed in main task */

    customer_header = amp_calloc(HTTP_HEADER_SIZE, 1);
    for (int i = 0; i < 8; i++) {
        if ((param->http_header[i].name != NULL) && (param->http_header[i].data != NULL)) {
            httpc_construct_header(customer_header, HTTP_HEADER_SIZE, param->http_header[i].name,
                                   param->http_header[i].data);
            amp_free(param->http_header[i].name);
            amp_free(param->http_header[i].data);
        }
    }
    http_header_index = 0;
    httpclient_set_custom_header(&client, customer_header);

    param->buffer = amp_malloc(2048);
    memset(param->buffer, 0, 2048);
    param->header_buffer = amp_malloc(2048);
    memset(param->header_buffer, 0, 2048);

    if (param->method == HTTP_GET) {
        amp_info(MOD_STR, "http GET request=%s,timeout=%d\r\n", param->url, param->timeout);
        ret = httpclient_get(&client, param->url, &client_data);
        if (ret >= 0) {
            amp_info(MOD_STR, "GET Data received: %s, len=%d \r\n", client_data.response_buf,
                     client_data.response_buf_len);
            strcpy(param->buffer, client_data.response_buf);
            strcpy(param->header_buffer, client_data.header_buf);
        } else {
            amp_error(MOD_STR, "http get error %d\r\n", ret);
        }
    } else if (param->method == HTTP_POST) {
        if (param->params != NULL) {
            amp_info(MOD_STR, "http POST request=%s, post_buf=%s, timeout=%d\r\n", param->url, param->params,
                     param->timeout);
            memset(client_data.header_buf, 0, sizeof(client_data.header_buf));
            strcpy(client_data.header_buf, param->params);

            client_data.post_buf = client_data.header_buf;
            client_data.post_buf_len = sizeof(client_data.header_buf);
            client_data.post_content_type = "application/x-www-form-urlencoded";
            ret = httpclient_post(&client, param->url, &client_data);
            if (ret >= 0) {
                amp_info(MOD_STR, "POST Data received: %s, len=%d \r\n", client_data.response_buf,
                         client_data.response_buf_len);
                strcpy(param->buffer, client_data.response_buf);
            } else {
                amp_error(MOD_STR, "http post error %d\r\n", ret);
            }
        } else {
            amp_error(MOD_STR, "method POST need params\n");
        }
    } else if (param->method == HTTP_PUT) {
        if (param->params != NULL) {
            amp_info(MOD_STR, "http PUT request=%s, data=%s, timeout=%d\r\n", param->url, param->params,
                     param->timeout);
            client_data.post_buf = param->params;
            client_data.post_buf_len = param->params_len;
            ret = httpclient_put(&client, param->url, &client_data);
            if (ret >= 0) {
                amp_info(MOD_STR, "Data received: %s, len=%d \r\n", client_data.response_buf,
                         client_data.response_buf_len);
                strcpy(param->buffer, client_data.response_buf);
            }
        } else {
            amp_error(MOD_STR, "method PUT need params\n");
        }
    } else {
        amp_error(MOD_STR, "method %d not supported\n", param->method);
    }

    if (param->url != NULL) {
        amp_free(param->url);
    }

    if (param->filepath != NULL) {
        amp_free(param->filepath);
    }

    if (param->params != NULL) {
        amp_free(param->params);
    }

    if (customer_header != NULL) {
        amp_free(customer_header);
    }

    if (client_data.header_buf) {
        amp_free(client_data.header_buf);
    }
    if (client_data.response_buf) {
        amp_free(client_data.response_buf);
    }

    amp_task_schedule_call(http_request_notify, param);
    aos_task_exit(0);

    return;
}

static http_param_t *native_http_get_param(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    http_param_t *http_param = NULL;
    char *http_buffer = NULL;
    cJSON *param_root;
    char localip[32];

    http_param = (http_param_t *)amp_malloc(sizeof(http_param_t));
    if (!http_param) {
        amp_warn(MOD_STR, "allocate memory failed\n");
        return NULL;
    }
    memset(http_param, 0, sizeof(http_param_t));

    /* get http request url */
    JSValue url = JS_GetPropertyStr(ctx, argv[0], "url");
    if (!JS_IsString(url)) {
        amp_debug(MOD_STR, "param no request url");
        amp_free(http_param);
        return NULL;
    }
    char *url_c = JS_ToCString(ctx, url);
    if (url_c != NULL) {
        http_param->url = amp_malloc(strlen(url_c) + 1);
        strcpy(http_param->url, url_c);
        JS_FreeCString(ctx, url_c);
    }
    JS_FreeValue(ctx, url);

    /* get http request method */
    JSValue method = JS_GetPropertyStr(ctx, argv[0], "method");
    char *m = NULL;
    if (JS_IsString(method)) {
        m = JS_ToCString(ctx, method);
        if (strcmp(m, "GET") == 0) {
            http_param->method = HTTP_GET; /* GET */
        } else if (strcmp(m, "POST") == 0) {
            http_param->method = HTTP_POST; /* POST */
        } else if (strcmp(m, "PUT") == 0) {
            http_param->method = HTTP_PUT; /* PUT */
        } else {
            http_param->method = HTTP_GET;
        }
    } else {
        http_param->method = HTTP_GET;
    }
    JS_FreeCString(ctx, m);
    JS_FreeValue(ctx, method);

    /* get http request timeout */
    JSValue timeout = JS_GetPropertyStr(ctx, argv[0], "timeout");
    int32_t time = 0;
    if (JS_IsNumber(timeout)) {
        JS_ToInt32(ctx, &time, timeout);
        http_param->timeout = time;
    } else {
        http_param->timeout = HTTP_REQUEST_TIMEOUT;
    }
    JS_FreeValue(ctx, timeout);

    // size
    JSValue jsize = JS_GetPropertyStr(ctx, argv[0], "size");
    if ((!JS_IsException(jsize)) && (!JS_IsUndefined(jsize))) {
        if (JS_IsNumber(jsize)) {
            JS_ToInt32(ctx, &http_param->timeout, jsize);
        }
    }
    JS_FreeValue(ctx, jsize);

    /* get http request headers */
    JSValue headers = JS_GetPropertyStr(ctx, argv[0], "headers");
    JSPropertyEnum *ptab = NULL;
    int plen = 0;
    char *name = NULL;
    int ret = JS_GetOwnPropertyNames(ctx, &ptab, &plen, headers, JS_GPN_STRING_MASK);
    if (ret == 0) {
        for (int i = 0; i < plen; i++) {
            name = JS_AtomToCString(ctx, ptab[i].atom);
            if (name != NULL) {
                http_param->http_header[i].name = amp_malloc(strlen(name) + 1);
                strcpy(http_param->http_header[i].name, name);
                JS_FreeCString(ctx, name);
                JSValue value = JS_GetPropertyStr(ctx, headers, name);
                char *data = JS_ToCString(ctx, value);
                http_param->http_header[i].data = amp_malloc(strlen(data) + 1);
                strcpy(http_param->http_header[i].data, data);
                JS_FreeCString(ctx, data);
                JS_FreeValue(ctx, value);
                amp_debug(MOD_STR, "headers: %s:%s\n", http_param->http_header[i].name,
                          http_param->http_header[i].data);
            }
        }
    }
    js_free(ctx, ptab);
    JS_FreeValue(ctx, headers);

    amp_debug(MOD_STR, "url: %s\n", http_param->url);
    amp_debug(MOD_STR, "method: %d\n", http_param->method);
    amp_debug(MOD_STR, "timeout: %d\n", http_param->timeout);

    /* get http request params */
    JSValue params = JS_GetPropertyStr(ctx, argv[0], "params");
    if (JS_IsString(params)) {
        char *params_str = JS_ToCString(ctx, params);
        if (params_str != NULL) {
            http_param->params = amp_malloc(strlen(params_str) + 1);
            strcpy(http_param->params, params_str);
            JS_FreeCString(ctx, params_str);
            http_param->params_len = strlen(http_param->params);
            amp_debug(MOD_STR, "params: %s, len %d\n", http_param->params, http_param->params_len);
        }
    } else {
        amp_debug(MOD_STR, "params not contained");
    }
    JS_FreeValue(ctx, params);

    /* callback */
    JSValue cb = JS_GetPropertyStr(ctx, argv[0], "success");
    http_param->js_cb_ref = JS_DupValue(ctx, cb);
    JS_FreeValue(ctx, cb);

    JSValue filepath = JS_GetPropertyStr(ctx, argv[0], "filepath");
    if (JS_IsString(filepath)) {
        char *filepath_c = JS_ToCString(ctx, filepath);
        if (filepath_c != NULL) {
            http_param->filepath = amp_malloc(strlen(filepath_c) + 1);
            strcpy(http_param->filepath, filepath_c);
            JS_FreeCString(ctx, filepath_c);
        }
    }
    JS_FreeValue(ctx, filepath);
    amp_debug(MOD_STR, "filepath: %s\n", http_param->filepath);

    return http_param;
}

static void http_param_free(JSContext *ctx, http_param_t *http_param)
{
    if (http_param == NULL) {
        return;
    }

    if (http_param->url != NULL) {
        amp_free(http_param->url);
    }

    if (http_param->filepath != NULL) {
        amp_free(http_param->filepath);
    }

    if (http_param->params != NULL) {
        amp_free(http_param->params);
    }

    JS_FreeValue(ctx, http_param->js_cb_ref);
}

static JSValue native_http_download(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    aos_task_t http_task;

    http_param_t *http_param = native_http_get_param(ctx, this_val, argc, argv);
    if (http_param == NULL) {
        return JS_NewInt32(ctx, -1);
    }

    if (http_param->filepath == NULL) {
        http_param_free(ctx, http_param);
        return JS_NewInt32(ctx, -1);
    }

    aos_task_new_ext(&http_task, "amp http task", task_http_download, http_param, 1024 * 8, ADDON_TSK_PRIORRITY);

    return JS_NewInt32(ctx, 1);
}

static JSValue native_http_request(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    aos_task_t http_task;

    http_param_t *http_param = native_http_get_param(ctx, this_val, argc, argv);
    if (http_param == NULL) {
        return JS_NewInt32(ctx, -1);
    }

    aos_task_new_ext(&http_task, "amp http task", task_http_request_func, http_param, 1024 * 8, ADDON_TSK_PRIORRITY);

    return JS_NewInt32(ctx, 1);
}

static void http_recv_and_save_notify(void *pdata)
{
    http_download_resp_t *resp = (http_download_resp_t *)pdata;
    JSContext *ctx = js_get_context();
    JSValue args = JS_NewInt32(ctx, resp->error);
    JSValue val = JS_Call(ctx, resp->js_cb_ref, JS_UNDEFINED, 1, &args);
    JS_FreeValue(ctx, args);
    if (JS_IsException(val)) {
        amp_info(MOD_STR, "http_request_notify callback error");
    }
    JS_FreeValue(ctx, val);
    JS_FreeValue(ctx, resp->js_cb_ref);

    amp_free(resp);
}

static void task_http_download_loop(void *arg)
{
    http_download_resp_t *resp = (http_download_resp_t *)arg;
    http_dload_node_t *dload_node;
    dlist_t *temp;
    int ret;

    resp->error = 0;

    dlist_for_each_entry_safe(&g_dload_list, temp, dload_node, http_dload_node_t, node)
    {
        ret = http_download_func(dload_node->dload_param->http_param);
        if (ret > 0) {
            resp->error = ret;
            break;
        }
        dlist_del(&dload_node->node);
        amp_free(dload_node->dload_param);
        amp_free(dload_node);
    }

    if (ret > 0) {
        dlist_for_each_entry_safe(&g_dload_list, temp, dload_node, http_dload_node_t, node)
        {
            dlist_del(&dload_node->node);
            amp_free(dload_node->dload_param);
            amp_free(dload_node);
        }
    }

    dload_list_count = 0;
    amp_task_schedule_call(http_recv_and_save_notify, resp);
    aos_task_exit(0);
}

static JSValue native_http_download_start(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    aos_task_t http_task;
    http_download_resp_t *resp;
    int prio = AOS_DEFAULT_APP_PRI + 2;
    int ret;

    resp = amp_malloc(sizeof(http_download_resp_t));
    if (!resp) {
        amp_error(MOD_STR, "alloc download resp fail\n");
        return JS_NewInt32(ctx, -1);
    }

    /* callback */
    JSValue val = JS_GetPropertyStr(ctx, argv[0], "success");
    if ((!JS_IsException(val)) && (!JS_IsUndefined(val))) {
        resp->js_cb_ref = JS_DupValue(ctx, val);
    } else {
        resp->js_cb_ref = JS_UNDEFINED;
    }

    ret = aos_task_new_ext(&http_task, "http-dload", task_http_download_loop, resp, 10240, prio);
    if (ret) {
        amp_error(MOD_STR, "create dload task fail %d", ret);
        return JS_NewInt32(ctx, -1);
    }

    return JS_NewInt32(ctx, 0);
}

static JSValue native_http_download_add(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
    http_download_param_t *http_download_param = NULL;
    http_dload_node_t *dload_node;

    http_param_t *http_param = native_http_get_param(ctx, this_val, argc, argv);
    if (http_param == NULL) {
        return JS_NewInt32(ctx, -1);
    }

    http_download_param = amp_malloc(sizeof(http_download_param_t));
    if (http_download_param == NULL) {
        http_param_free(ctx, http_param);
        return JS_NewInt32(ctx, -1);
    }
    http_download_param->http_param = http_param;

    dload_node = amp_malloc(sizeof(http_dload_node_t));
    if (!dload_node) {
        amp_error(MOD_STR, "alloc http dload node failed");
        http_param_free(ctx, http_param);
        amp_free(http_download_param);
        return JS_NewInt32(ctx, -1);
    }
    memset(dload_node, 0, sizeof(http_dload_node_t));
    dload_node->dload_param = http_download_param;
    dlist_add_tail(&dload_node->node, &g_dload_list);
    http_download_param->index = ++dload_list_count;

    return JS_NewInt32(ctx, 0);
}

static JSClassDef js_http_class = {
    "HTTP",
};

static const JSCFunctionListEntry js_http_funcs[] = { JS_CFUNC_DEF("request", 1, native_http_request),
                                                      JS_CFUNC_DEF("download", 1, native_http_download),
                                                      JS_CFUNC_DEF("startDownload", 1, native_http_download_start),
                                                      JS_CFUNC_DEF("addDownload", 1, native_http_download_add) };

static int js_http_init(JSContext *ctx, JSModuleDef *m)
{
    JSValue proto;

    JS_NewClassID(&js_http_class_id);

    JS_NewClass(JS_GetRuntime(ctx), js_http_class_id, &js_http_class);
    proto = JS_NewObject(ctx);
    JS_SetPropertyFunctionList(ctx, proto, js_http_funcs, countof(js_http_funcs));
    JS_SetClassProto(ctx, js_http_class_id, proto);
    return JS_SetModuleExportList(ctx, m, js_http_funcs, countof(js_http_funcs));
}

JSModuleDef *js_init_module_http(JSContext *ctx, const char *module_name)
{
    JSModuleDef *m;
    m = JS_NewCModule(ctx, module_name, js_http_init);
    if (!m)
        return NULL;
    JS_AddModuleExportList(ctx, m, js_http_funcs, countof(js_http_funcs));
    return m;
}

void module_http_register(void)
{
    amp_debug(MOD_STR, "module_http_register");
    JSContext *ctx = js_get_context();
    js_init_module_http(ctx, "http");
}
