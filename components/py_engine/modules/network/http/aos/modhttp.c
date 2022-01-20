/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if MICROPY_PY_HTTP

#include "aos/kernel.h"
#include "cJSON.h"
#include "httpclient.h"
#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "ulog/ulog.h"

#define LOG_TAG                     "MOD-HTTP"

#define MOD_STR                     "HTTP"
#define HTTP_BUFF_SIZE              (12 * 1024)
#define HTTP_HEADER_SIZE            2048
#define HTTP_HEADER_COUNT           8
#define HTTP_REQUEST_PARAMS_LEN_MAX 2048
#define HTTP_SEND_RECV_TIMEOUT      10000
#define HTTP_REQUEST_TIMEOUT        30000
#define HTTP_DEFAULT_HEADER_NAME    "content-type"
#define HTTP_DEFAULT_HEADER_DATA    "application/json"
#define HTTP_CRLF                   "\r\n"

#define ADDON_TSK_PRIORRITY         (AOS_DEFAULT_APP_PRI + 3)
static int http_header_index = 0;
typedef struct {
    char *name;
    char *data;
} http_header_t;

typedef struct {
    char *url;
    char *filepath;
    int method;
    http_header_t http_header[HTTP_HEADER_COUNT];
    uint32_t timeout;
    char *params;
    char *rec_data_buffer;
    char *rec_header_buffer;
    int params_len;
    mp_obj_t cb;
} http_param_t;

extern const mp_obj_type_t http_client_type;

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
    char url_dup[1024] = { 0 };

    if (url == NULL) {
        LOGD(LOG_TAG, "url is null");
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

    if (strncmp(url_dup, protocol, strlen(protocol)) == 0) {
        start = url_dup + strlen(protocol) + 3;
        p_slash = strchr(start, '/');
        if (p_slash != NULL) {
            memcpy(uri, p_slash, strlen(p_slash));
            *p_slash = '\0';
        } else {
            memcpy(uri, '/', 1);
        }
    }
}

int32_t pyamp_httpc_construct_header(char *buf, uint16_t buf_size, const char *name, const char *data)
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
    mp_obj_t dict = MP_OBJ_NULL;

    http_param_t *msg = (http_param_t *)pdata;
    LOGD(LOG_TAG, "buf is %s \r\n", msg->rec_data_buffer);
    LOGD(LOG_TAG, "buf len is %d \r\n", strlen(msg->rec_data_buffer));
    LOGD(LOG_TAG, "header is %s \r\n", msg->rec_header_buffer);
    LOGD(LOG_TAG, "buf len is %d \r\n", strlen(msg->rec_header_buffer));

    dict = mp_obj_new_dict(2);
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("header", 6),
                      mp_obj_new_str(msg->rec_header_buffer, strlen(msg->rec_header_buffer)));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("body", 4),
                      mp_obj_new_str(msg->rec_data_buffer, strlen(msg->rec_data_buffer)));

    if (msg->cb != MP_OBJ_NULL && mp_obj_is_callable(msg->cb)) {
        callback_to_python(msg->cb, dict);
    }
}

static void http_download_notify(void *pdata)
{
    http_param_t *msg = (http_param_t *)pdata;
    LOGD(LOG_TAG, "buf is %s \r\n", msg->rec_data_buffer);
    LOGD(LOG_TAG, "buf len is %d \r\n", strlen(msg->rec_data_buffer));

    if (msg->cb != MP_OBJ_NULL && mp_obj_is_callable(msg->cb)) {
        callback_to_python(msg->cb, mp_obj_new_str(msg->rec_data_buffer, strlen(msg->rec_data_buffer)));
    }
}

static char customer_header[HTTP_HEADER_SIZE] = { 0 };
static char rsp_header[HTTP_HEADER_SIZE] = { 0 };
static char rsp_buf[HTTP_BUFF_SIZE];
static char req_buf[HTTP_BUFF_SIZE];

/* create task for http download */
static void task_http_download_func(void *arg)
{
    httpclient_t client = { 0 };
    httpclient_data_t client_data = { 0 };
    http_param_t *http_param = (http_param_t *)arg;
    int num = 0;
    int ret, i = 0;
    bool result = false;
    char *header = "Accept: */*\r\n";

    char *req_url = http_param->url;

    if (!aos_access(http_param->filepath, 0)) {
        aos_remove(http_param->filepath);
    }

    int fd = aos_open(http_param->filepath, O_CREAT | O_RDWR | O_TRUNC);
    if (fd < 0) {
        printf("aos open fd %d fail\n", fd);
        result = false;
        goto end;
    }
    memset(rsp_buf, 0, sizeof(rsp_buf));
    client_data.response_buf = rsp_buf;
    client_data.response_buf_len = sizeof(rsp_buf);
    client_data.header_buf = rsp_header;
    client_data.header_buf_len = HTTP_HEADER_SIZE;

    httpclient_set_custom_header(&client, header);

    ret = httpclient_conn(&client, req_url);

    if (!ret) {
        ret = httpclient_send(&client, req_url, HTTP_GET, &client_data);

        do {
            ret = httpclient_recv(&client, &client_data);
            // printf("response_content_len=%d,
            // retrieve_len=%d,content_block_len=%d\n",
            // client_data.response_content_len,client_data.retrieve_len,client_data.content_block_len);
            // printf("ismore=%d\n", client_data.is_more);

            num = aos_write(fd, client_data.response_buf, client_data.content_block_len);
            if (num > 0) {
                // printf("aos_write num=%d\n", num);
            } else {
                result = false;
                break;
            }
        } while (client_data.is_more);
        if (client_data.response_content_len == 0)
            result = false;
        else
            result = true;
    } else {
        result = false;
        printf("[%s]httpclient_conn fail, ret : %d\n", __func__, ret);
    }

    aos_close(fd);
    httpclient_clse(&client);

end:
    if (result) {
        if (http_param->rec_data_buffer)
            strcpy(http_param->rec_data_buffer, "success");
    } else {
        if (http_param->rec_data_buffer)
            strcpy(http_param->rec_data_buffer, "fail");
    }

    http_download_notify(http_param);

    if (http_param && http_param->rec_data_buffer) {
        aos_free(http_param->rec_data_buffer);
    }

    if (http_param && http_param->rec_header_buffer) {
        aos_free(http_param->rec_header_buffer);
    }

    if (http_param) {
        aos_free(http_param);
    }

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
    httpclient_t client = { 0 };
    httpclient_data_t client_data = { 0 };
    http_param_t *http_param = (http_param_t *)arg;

    url = http_param->url;
    timeout = http_param->timeout;
    http_method = http_param->method;

    LOGD(LOG_TAG, "task_http_request_func url: %s", url);
    LOGD(LOG_TAG, "task_http_request_func method: %d", http_method);
    LOGD(LOG_TAG, "task_http_request_func timeout: %d", timeout);

    memset(rsp_buf, 0, HTTP_BUFF_SIZE);
    client_data.response_buf = rsp_buf;
    client_data.response_buf_len = sizeof(rsp_buf);
    client_data.header_buf = rsp_header;
    client_data.header_buf_len = HTTP_HEADER_SIZE;
    aos_msleep(50); /* need do things after state changed in main task */

    uint32_t len = 0;
    for (i = 0; i < http_header_index; i++) {
        len = pyamp_httpc_construct_header(customer_header + len, HTTP_HEADER_SIZE - len,
                                           http_param->http_header[i].name, http_param->http_header[i].data);
    }
    http_header_index = 0;
    httpclient_set_custom_header(&client, customer_header);

    if (http_method == HTTP_GET) {
        LOGD(LOG_TAG, "http GET request=%s,timeout=%d\r\n", url, timeout);
        ret = httpclient_get(&client, url, &client_data);
        if (ret >= 0) {
            LOGD(LOG_TAG, "GET Data received: %s, len=%d \r\n", client_data.response_buf, client_data.response_buf_len);
            strcpy(http_param->rec_data_buffer, client_data.response_buf);
            strcpy(http_param->rec_header_buffer, client_data.header_buf);
        }
    } else if (http_method == HTTP_HEAD) {
        LOGD(LOG_TAG, "http HEAD request=%s,timeout=%d\r\n", url, timeout);
        ret = httpclient_head(&client, url, &client_data);
        if (ret >= 0) {
            LOGD(LOG_TAG, "HEAD Data received: %s, len=%d \r\n", client_data.response_buf,
                 client_data.response_buf_len);
            strcpy(http_param->rec_data_buffer, client_data.response_buf);
            strcpy(http_param->rec_header_buffer, client_data.header_buf);
        }
    } else if (http_method == HTTP_POST) {
        LOGD(LOG_TAG, "http POST request=%s,post_buf=%s,timeout=%d\r\n", url, http_param->params, timeout);
        memset(req_buf, 0, sizeof(req_buf));
        strcpy(req_buf, http_param->params);
        client_data.post_buf = req_buf;
        client_data.post_buf_len = sizeof(req_buf);
        client_data.post_content_type = "application/x-www-form-urlencoded";
        ret = httpclient_post(&client, url, &client_data);
        if (ret >= 0) {
            LOGD(LOG_TAG, "POST Data received: %s, len=%d \r\n", client_data.response_buf,
                 client_data.response_buf_len);
            strcpy(http_param->rec_data_buffer, client_data.response_buf);
            strcpy(http_param->rec_header_buffer, client_data.header_buf);
        }
    } else if (http_method == HTTP_PUT) {
        LOGD(LOG_TAG, "http PUT request=%s,data=%s,timeout=%d\r\n", url, http_param->params, timeout);
        memset(req_buf, 0, sizeof(req_buf));
        strcpy(req_buf, http_param->params);
        client_data.post_buf = req_buf;
        client_data.post_buf_len = sizeof(req_buf);
        client_data.post_content_type = "application/x-www-form-urlencoded";
        ret = httpclient_put(&client, url, &client_data);
        if (ret >= 0) {
            LOGD(LOG_TAG, "Data received: %s, len=%d \r\n", client_data.response_buf, client_data.response_buf_len);
            strcpy(http_param->rec_data_buffer, client_data.response_buf);
            strcpy(http_param->rec_header_buffer, client_data.header_buf);
        }
    } else if (http_method == HTTP_DELETE) {
        LOGD(LOG_TAG, "http DELETE request=%s,data=%s,timeout=%d\r\n", url, http_param->params, timeout);
        memset(req_buf, 0, sizeof(req_buf));
        strcpy(req_buf, http_param->params);
        client_data.post_buf = req_buf;
        client_data.post_buf_len = sizeof(req_buf);
        client_data.post_content_type = "application/x-www-form-urlencoded";
        ret = httpclient_put(&client, url, &client_data);
        if (ret >= 0) {
            LOGD(LOG_TAG, "Data received: %s, len=%d \r\n", client_data.response_buf, client_data.response_buf_len);
            strcpy(http_param->rec_data_buffer, client_data.response_buf);
            strcpy(http_param->rec_header_buffer, client_data.header_buf);
        }
    } else {
        ret = httpclient_get(&client, url, &client_data);
        if (ret >= 0) {
            LOGD(LOG_TAG, "Data received: %s, len=%d \r\n", client_data.response_buf, client_data.response_buf_len);
            strcpy(http_param->rec_data_buffer, client_data.response_buf);
            strcpy(http_param->rec_header_buffer, client_data.header_buf);
        }
    }

    httpclient_clse(&client);
    http_request_notify(http_param);

    if (http_param && http_param->rec_data_buffer) {
        aos_free(http_param->rec_data_buffer);
    }

    if (http_param && http_param->rec_header_buffer) {
        aos_free(http_param->rec_header_buffer);
    }

    if (http_param) {
        aos_free(http_param);
    }

    LOGD(LOG_TAG, " task_http_request_func end \r\n ");
    aos_task_exit(0);

    return;
}

STATIC mp_obj_t http_download(mp_obj_t data, mp_obj_t callback)
{
    http_param_t *http_param = NULL;
    char *http_buffer = NULL;
    char *http_header_buffer = NULL;
    const char *url = NULL;
    const char *filepath = NULL;

    aos_task_t http_task;

    http_param = (http_param_t *)aos_malloc(sizeof(http_param_t));
    if (!http_param) {
        LOGD(LOG_TAG, "allocate memory failed\n");
        goto done;
    }
    memset(http_param, 0, sizeof(http_param_t));

    http_buffer = aos_malloc(32 + 1);
    if (!http_buffer) {
        LOGE(LOG_TAG, "allocate memory failed\n");
        goto done;
    }
    memset(http_buffer, 0, 32 + 1);
    http_param->rec_data_buffer = http_buffer;

    http_header_buffer = aos_malloc(HTTP_HEADER_SIZE + 1);
    if (!http_header_buffer) {
        LOGE(LOG_TAG, "allocate memory failed\n");
        goto done;
    }
    memset(http_header_buffer, 0, HTTP_HEADER_SIZE + 1);
    http_param->rec_header_buffer = http_header_buffer;

    /* get http download url */

    mp_obj_t index = mp_obj_new_str_via_qstr("url", 3);

    url = mp_obj_str_get_str(mp_obj_dict_get(data, index));
    http_param->url = url;

    /* get http download filepath */

    index = mp_obj_new_str_via_qstr("filepath", 8);

    filepath = mp_obj_str_get_str(mp_obj_dict_get(data, index));
    http_param->filepath = filepath;

    /* callback */
    http_param->cb = callback;

    aos_task_new_ext(&http_task, "amp http download task", task_http_download_func, http_param, 1024 * 8,
                     ADDON_TSK_PRIORRITY);

    return mp_obj_new_int(0);

done:
    if (http_buffer) {
        aos_free(http_buffer);
    }

    if (http_header_buffer) {
        aos_free(http_header_buffer);
    }

    if (http_param) {
        aos_free(http_param);
    }

    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_2(mp_obj_http_download, http_download);

static mp_obj_t http_request(mp_obj_t data, mp_obj_t callback)
{
    http_param_t *http_param = NULL;
    char *http_buffer = NULL;
    char *http_header_buffer = NULL;
    const char *method = NULL;
    const char *url = NULL;
    cJSON *param_root;
    int http_method = 0;
    int timeout = 0;
    char localip[32];
    int i;
    aos_task_t http_task;

    if (!mp_obj_is_dict_or_ordereddict(data)) {
        LOGE(LOG_TAG, "%s param type error, param type must be dict\r\n", __func__);
        return mp_const_none;
    }

    http_param = (http_param_t *)aos_malloc(sizeof(http_param_t));
    if (!http_param) {
        LOGE(LOG_TAG, "allocate memory failed\n");
        goto done;
    }
    memset(http_param, 0, sizeof(http_param_t));

    http_buffer = aos_malloc(HTTP_BUFF_SIZE + 1);
    if (!http_buffer) {
        LOGE(LOG_TAG, "allocate memory failed\n");
        goto done;
    }
    memset(http_buffer, 0, HTTP_BUFF_SIZE + 1);
    http_param->rec_data_buffer = http_buffer;

    http_header_buffer = aos_malloc(HTTP_HEADER_SIZE + 1);
    if (!http_header_buffer) {
        LOGE(LOG_TAG, "allocate memory failed\n");
        goto done;
    }
    memset(http_header_buffer, 0, HTTP_HEADER_SIZE + 1);
    http_param->rec_header_buffer = http_header_buffer;

    /* get http request url */
    mp_obj_t index = mp_obj_new_str_via_qstr("url", 3);

    url = mp_obj_str_get_str(mp_obj_dict_get(data, index));
    http_param->url = url;

    /* get http request method */
    index = mp_obj_new_str_via_qstr("method", 6);

    method = mp_obj_str_get_str(mp_obj_dict_get(data, index));
    if (strcmp(method, "GET") == 0) {
        http_method = HTTP_GET; /* GET */
    } else if (strcmp(method, "HEAD") == 0) {
        http_method = HTTP_HEAD; /* HEAD */
    } else if (strcmp(method, "POST") == 0) {
        http_method = HTTP_POST; /* POST */
    } else if (strcmp(method, "PUT") == 0) {
        http_method = HTTP_PUT; /* PUT */
    } else if (strcmp(method, "DELETE") == 0) {
        http_method = HTTP_DELETE; /* DELETE */
    } else {
        http_method = HTTP_GET;
    }
    http_param->method = http_method;

    /* get http request timeout */
    index = mp_obj_new_str_via_qstr("timeout", 7);
    timeout = mp_obj_get_int(mp_obj_dict_get(data, index));
    http_param->timeout = timeout;

    LOGD(LOG_TAG, "timeout is : %d", http_param->timeout);

    /* get http request headers */
    index = mp_obj_new_str_via_qstr("headers", 7);
    mp_obj_t header = mp_obj_dict_get(data, index);
    if (header == MP_OBJ_NULL) {
        // pass
        http_param->http_header[0].name = HTTP_DEFAULT_HEADER_NAME;
        http_param->http_header[0].data = HTTP_DEFAULT_HEADER_DATA;
        http_header_index++;
    } else if (mp_obj_is_type(header, &mp_type_dict)) {
        // dictionary
        mp_map_t *map = mp_obj_dict_get_map(header);
        // assert(args2_len + 2 * map->used <= args2_alloc); // should have
        // enough, since kw_dict_len is in this case hinted correctly above
        for (size_t i = 0; i < map->alloc; i++) {
            if (mp_map_slot_is_filled(map, i)) {
                // the key must be a qstr, so intern it if it's a string
                mp_obj_t key = map->table[i].key;
                if (!mp_obj_is_qstr(key)) {
                    key = mp_obj_str_intern_checked(key);
                }
                http_header_index = i;
                http_param->http_header[http_header_index].name = mp_obj_str_get_str(map->table[i].key);
                http_param->http_header[http_header_index].data = mp_obj_str_get_str(map->table[i].value);
            }
        }
    }

    index = mp_obj_new_str_via_qstr("params", 6);
    mp_obj_t params = mp_obj_dict_get(data, index);

    http_param->params = mp_obj_str_get_str(params);
    http_param->params_len = strlen(http_param->params);

    LOGD(LOG_TAG, "url: %s", http_param->url);
    LOGD(LOG_TAG, "method: %d", http_param->method);
    LOGD(LOG_TAG, "timeout: %d", http_param->timeout);
    for (i = 0; i < http_header_index; i++) {
        LOGD(LOG_TAG, MOD_STR, "headers: %s:%s", http_param->http_header[i].name, http_param->http_header[i].data);
    }

    /* callback */

    http_param->cb = callback;

    aos_task_new_ext(&http_task, "amp http task", task_http_request_func, http_param, 1024 * 8, ADDON_TSK_PRIORRITY);

    return mp_obj_new_int(0);

done:

    if (http_buffer) {
        aos_free(http_buffer);
    }

    if (http_header_buffer) {
        aos_free(http_header_buffer);
    }

    if (http_param) {
        aos_free(http_param);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_obj_http_request, http_request);

STATIC const mp_rom_map_elem_t http_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_http) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_request), MP_ROM_PTR(&mp_obj_http_request) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_download), MP_ROM_PTR(&mp_obj_http_download) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_client), MP_ROM_PTR(&http_client_type) },
};

STATIC MP_DEFINE_CONST_DICT(http_locals_dict, http_locals_dict_table);

const mp_obj_module_t mp_module_http = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&http_locals_dict,
};

MP_REGISTER_MODULE(MP_QSTR_http, mp_module_http, MICROPY_PY_HTTP);
#endif