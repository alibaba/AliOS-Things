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
#include "be_inl.h"
// #include "http.h"
#include "cJSON.h"
#include "amp_list.h"

#define MOD_STR                     "HTTP"
#define HTTP_BUFF_SIZE              2048
#define HTTP_HEADER_SIZE            1024
#define HTTP_HEADER_COUNT           8
#define HTTP_REQUEST_PARAMS_LEN_MAX 2048
#define HTTP_SEND_RECV_TIMEOUT      10000
#define HTTP_REQUEST_TIMEOUT        30000
#define HTTP_DEFAULT_HEADER_NAME    "content-type"
#define HTTP_DEFAULT_HEADER_DATA    "application/json"
#define HTTP_CRLF                   "\r\n"

static int http_header_index = 0;
typedef struct
{
    char *name;
    char *data;
} http_header_t;

typedef struct
{
    char *url;
    char *filepath;
    int method;
    http_header_t http_header[HTTP_HEADER_COUNT];
    uint32_t timeout;
    char *params;
    char *buffer;
    int params_len;
    int js_cb_ref;
} http_param_t;

static char *strncasestr(const char *str, const char *key)
{
    int len;

    if (!str || !key)
        return NULL;

    len = strlen(key);
    if (len == 0)
        return NULL;

    while (*str) {
        if (!strncasecmp(str, key, len))
            return str;
        ++str;
    }
    return NULL;
}

static void parse_url(const char *url, char *uri)
{
    char url_dup[1024] = {0};

    if (url == NULL) {
        amp_warn(MOD_STR, "url is null");
        return;
    }
    memcpy(url_dup, url, strlen(url));
    char *start = NULL;
    char *p_slash = NULL;

    #if 1
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
    duk_context *ctx = be_get_context();
    be_push_ref(ctx, msg->js_cb_ref);
    duk_push_string(ctx, msg->buffer);
    if (duk_pcall(ctx, 1) != DUK_EXEC_SUCCESS) {
        amp_console("%s", duk_safe_to_stacktrace(ctx, -1));
    }
    duk_pop(ctx);
    be_unref(ctx, msg->js_cb_ref);
    aos_free(msg->buffer);
    if (msg->params)
        aos_free(msg->params);
    aos_free(msg);

    duk_gc(ctx, 0);
}

char customer_header[HTTP_HEADER_SIZE] = {0};
char rsp_buf[HTTP_BUFF_SIZE];
char req_buf[HTTP_BUFF_SIZE];

/* create task for http download */
static void task_http_download_func(void *arg)
{
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    http_param_t *param = (http_param_t *)arg;
    int num=0;
    int ret;

    // 字符串偶现丢失，不知原因，暂时写死，by梓同
    // char * req_url = param->url;
    // int fd = aos_open(param->filepath,  O_CREAT | O_RDWR | O_APPEND);
    char * req_url = "http://wangguan-498.oss-cn-beijing.aliyuncs.com/SHOPAD/public/mould5.png";
    int fd = aos_open("/data/http_text.png",  O_CREAT | O_RDWR | O_APPEND);

    memset(rsp_buf, 0, sizeof(rsp_buf));
    client_data.response_buf = rsp_buf;
    client_data.response_buf_len = sizeof(rsp_buf);

    ret = httpclient_conn(&client, req_url);

    if (!ret) {
        ret = httpclient_send(&client, req_url, HTTP_GET, &client_data);

        do{
            ret = httpclient_recv(&client, &client_data);
            printf("response_content_len=%d, retrieve_len=%d,content_block_len=%d\n", client_data.response_content_len,client_data.retrieve_len,client_data.content_block_len);
            printf("ismore=%d \n", client_data.is_more);

            num = aos_write(fd, client_data.response_buf, client_data.content_block_len);
            if(num > 0){
                printf("aos_write num=%d\n", num);
            }
        }while(client_data.is_more);
    }
    ret = aos_sync(fd);
    param->buffer = "http download success";
    httpclient_clse(&client);

    amp_task_schedule_call(http_request_notify, param);
    aos_task_exit(0);
}

/* create task for http request */
static void task_http_request_func(void *arg)
{
    char *url = NULL;
    uint32_t timeout = 0;
    int http_method = 0;
    int i = 0;
    int ret = 0;
    httpclient_t client = {0};
    httpclient_data_t client_data = {0};
    http_param_t *param = (http_param_t *)arg;

    url = param->url;
    timeout = param->timeout;
    http_method = param->method;

    amp_debug(MOD_STR, "task_http_request_func url: %s", url);
    amp_debug(MOD_STR, "task_http_request_func method: %d", http_method);
    amp_debug(MOD_STR, "task_http_request_func timeout: %d", timeout);

    memset(rsp_buf, 0, HTTP_BUFF_SIZE);
    client_data.response_buf = rsp_buf;
    client_data.response_buf_len = sizeof(rsp_buf);
    aos_msleep(50); /* need do things after state changed in main task */

    for (i = 0; i < http_header_index; i++) {
        httpc_construct_header(customer_header, HTTP_HEADER_SIZE, param->http_header[i].name, param->http_header[i].data);
    }
    http_header_index = 0;
    httpclient_set_custom_header(&client, customer_header);


    if(http_method == HTTP_GET){
        amp_info(MOD_STR,"http GET request=%s,timeout=%d\r\n", url, timeout);
        ret = httpclient_get(&client, url, &client_data);
        if( ret >= 0 ) {
            amp_info(MOD_STR,"GET Data received: %s, len=%d \r\n", client_data.response_buf, client_data.response_buf_len);
            strcpy(param->buffer,client_data.response_buf);
        }
    }else if(http_method == HTTP_POST){
        amp_info(MOD_STR,"http POST request=%s,post_buf=%s,timeout=%d\r\n", url,param->params,timeout);
        memset(req_buf, 0, sizeof(req_buf));
        strcpy(req_buf, "tab_index=0&count=3&group_id=6914830518563373582&item_id=6914830518563373581&aid=1768");
        client_data.post_buf = req_buf;
        client_data.post_buf_len = sizeof(req_buf);
        client_data.post_content_type="application/x-www-form-urlencoded";
        ret = httpclient_post(&client, "https://www.ixigua.com/tlb/comment/article/v5/tab_comments/", &client_data);
        if( ret >= 0 ) {
            amp_info(MOD_STR,"POST Data received: %s, len=%d \r\n", client_data.response_buf, client_data.response_buf_len);
            strcpy(param->buffer,client_data.response_buf);
        }
    }else if(http_method == HTTP_PUT){
        amp_info(MOD_STR,"http PUT request=%s,data=%s,timeout=%d\r\n", url,param->params,timeout);
        client_data.post_buf = param->params;
        client_data.post_buf_len = param->params_len;
        ret = httpclient_put(&client, url, &client_data);
        if( ret >= 0 ) {
            amp_info(MOD_STR,"Data received: %s, len=%d \r\n", client_data.response_buf, client_data.response_buf_len);
            strcpy(param->buffer,client_data.response_buf);
        }
    }else{
        ret = httpclient_get(&client, url, &client_data);
        if( ret >= 0 ) {
            amp_info(MOD_STR,"Data received: %s, len=%d \r\n", client_data.response_buf, client_data.response_buf_len);
            strcpy(param->buffer,client_data.response_buf);
        }
    }

    amp_task_schedule_call(http_request_notify, param);
    aos_task_exit(0);

    return;
}


static duk_ret_t native_http_download(duk_context *ctx)
{
    http_param_t *http_param = NULL;
    char *http_buffer = NULL;
    aos_task_t http_task;

    if (!duk_is_object(ctx, 0))
    {
        amp_warn(MOD_STR, "invalid parameter\n");
        goto done;
    }

    http_param = (http_param_t *)aos_malloc(sizeof(http_param_t));
    if (!http_param)
    {
        amp_warn(MOD_STR, "allocate memory failed\n");
        goto done;
    }
    memset(http_param, 0, sizeof(http_param_t));
    http_param->buffer = http_buffer;

    /* get http request url */
    duk_get_prop_string(ctx, 0, "url");
    if (!duk_is_string(ctx, -1)) {
        amp_debug(MOD_STR, "request url is invalid");
        goto done;
    }
    http_param->url = duk_get_string(ctx, 1);
    duk_pop(ctx);
    amp_debug(MOD_STR, "url: %s", http_param->url);


    /* get http download filepath */
    duk_get_prop_string(ctx, 0, "filepath");
    if (!duk_is_string(ctx, -1)) {
        amp_debug(MOD_STR, "filepath is invalid");
        goto done;
    }
    http_param->filepath = duk_get_string(ctx, 1);
    duk_pop(ctx);
    amp_debug(MOD_STR, "filepath: %s", http_param->filepath);

    /* callback */
    if (duk_get_prop_string(ctx, 0, "success")) {
        duk_dup(ctx, -1);
        http_param->js_cb_ref = be_ref(ctx);
    } else {
        http_param->js_cb_ref = -1;
    }

    aos_task_new_ext(&http_task, "amp http download task", task_http_download_func, http_param, 1024 * 8, ADDON_TSK_PRIORRITY);

    duk_push_int(ctx, 0);
    return 1;

done:
    if (http_buffer)
        aos_free(http_buffer);
    if (http_param && http_param->params)
        cJSON_free(http_param->params);
    if (http_param)
        aos_free(http_param);

    duk_push_int(ctx, -1);
    return 1;

}


static duk_ret_t native_http_request(duk_context *ctx)
{
    http_param_t *http_param = NULL;
    char *http_buffer = NULL;
    const char *method = NULL;
    cJSON *param_root;
    int http_method = 0;
    int timeout = 0;
    char localip[32];
    int i;
    aos_task_t http_task;

    if (!duk_is_object(ctx, 0))
    {
        amp_warn(MOD_STR, "invalid parameter\n");
        goto done;
    }


    // amp_system.c中netmgr_wifi_get_ip方法没有实现，暂时注释掉这部分  by梓同
    // if (amp_get_ip(localip) != 0)
    // {
    //     amp_warn(MOD_STR, "network not ready\r\n");
    //     goto done;
    // }

    http_param = (http_param_t *)aos_malloc(sizeof(http_param_t));
    if (!http_param)
    {
        amp_warn(MOD_STR, "allocate memory failed\n");
        goto done;
    }
    memset(http_param, 0, sizeof(http_param_t));

    http_buffer = aos_malloc(HTTP_BUFF_SIZE + 1);
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

    /* get http request params */
    if (duk_get_prop_string(ctx, 0, "params")) {
        // duk_enum(ctx, -1, DUK_ENUM_OWN_PROPERTIES_ONLY);
        // param_root = cJSON_CreateObject();
        // if (!param_root) {
        //     amp_error(MOD_STR, "alloc param json object fail");
        //     goto done;
        // }

        // while (duk_next(ctx, -1, 1)) {
        //     if (!duk_is_string(ctx, -2) || !duk_is_string(ctx, -1)) {
        //         amp_error(MOD_STR, "get params fail");
        //         break;
        //     }

        //     cJSON_AddStringToObject(param_root, duk_to_string(ctx, -2), duk_to_string(ctx, -1));
        //     duk_pop_2(ctx);
        // }
        // http_param->params = cJSON_PrintUnformatted(param_root);
        // http_param->params_len = strlen(http_param->params);
        // amp_error(MOD_STR, "params: %s, len %d", http_param->params, http_param->params_len);
        // cJSON_Delete(param_root);
        if (duk_is_string(ctx, -1)) {
            http_param->params = duk_get_string(ctx, -1);
            http_param->params_len = strlen(http_param->params);
            amp_debug(MOD_STR, "params: %s, len %d", http_param->params, http_param->params_len);
        }
    } else {
        amp_debug(MOD_STR, "%s: params not contained", __func__);
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


    aos_task_new_ext(&http_task, "amp http task", task_http_request_func, http_param, 1024 * 8, ADDON_TSK_PRIORRITY);

    duk_push_int(ctx, 0);
    return 1;

done:
    if (http_buffer)
        aos_free(http_buffer);
    if (http_param && http_param->params)
        cJSON_free(http_param->params);
    if (http_param)
        aos_free(http_param);

    duk_push_int(ctx, -1);
    return 1;
}

void module_http_register(void)
{
    duk_context *ctx = be_get_context();
    duk_push_object(ctx);

    /* request */
    AMP_ADD_FUNCTION("request", native_http_request, 1);
    AMP_ADD_FUNCTION("download", native_http_download, 1);
    // AMP_ADD_FUNCTION("upload", native_http_upload, 1);

    duk_put_prop_string(ctx, -2, "HTTP");
}
