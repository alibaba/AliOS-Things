/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "iot_export.h"
#include "utils_httpc.h"
#include "utils_hmac.h"
#include "iotx_utils.h"
#include "fs_upload_debug.h"
#include "iot_export_http2.h"
#include "iot_export_http2_stream.h"
#include "lite-list.h"

#define KEEP_ALIVE_TIMES                     (60) /*seconds*/
#define MAX_HTTP2_INTERVAL_TIMES             (3)  /*seconds*/

#define MAKE_HEADER(NAME, VALUE)                                             \
    {                                                                        \
        (char *) NAME, (char *)VALUE, sizeof(NAME) - 1, sizeof(VALUE) - 1    \
    }

#define MAKE_HEADER_CS(NAME, VALUE)                                          \
    {                                                                        \
        (char *) NAME, (char *)VALUE, strlen(NAME) , strlen(VALUE)           \
    }

typedef enum {
    NUM_STRING_ENUM      = 0,
    PATH_CREATE_STR_ENUM = 1,
    PATH_UPLOAD_STR_ENUM = 2,
    CID_STRING_ENUM      = 3,
    ORI_SIGN_STR_ENUM    = 4,
    FS_STRING_ENUM       = 5,
    REAL_SIGN_STR_ENUM   = 6,
} HEADER_TYPE_ENUM;


typedef struct _device_info_struct_ {
    char        product_key[PRODUCT_KEY_LEN + 1];
    char        device_name[DEVICE_NAME_LEN + 1];
    char        device_secret[DEVICE_SECRET_LEN + 1];
} device_info;


static int stream_id;
static device_info g_device_info;
// static stream_user_cb_t *_user_cb = NULL;
static httpclient_t g_client;

static int _set_device_info(char *pk, char *dn, char *ds)
{
    memcpy(g_device_info.product_key, pk, PRODUCT_KEY_LEN);
    memcpy(g_device_info.device_name, dn, DEVICE_NAME_LEN);
    memcpy(g_device_info.device_secret, ds, DEVICE_SECRET_LEN);

    return 0;
}

static int print_header(http2_header *nva, int num)
{
    int i;
    if (nva == NULL) {
        return -1;
    }
    for (i = 0; i < num; i++) {
         h2stream_info("%s %d:%s %d\n", nva[i].name, nva[i].namelen, nva[i].value,nva[i].valuelen);
    }
    return 0;
}

static int http2_nv_copy(http2_header *nva, int start, http2_header *nva_copy, int num)
{
    int i, j;
    for (i = start, j = 0; j < num; i++, j++) {
        nva[i].name = nva_copy[j].name;
        nva[i].value = nva_copy[j].value;
        nva[i].namelen = nva_copy[j].namelen;
        nva[i].valuelen = nva_copy[j].valuelen;
    }
    return i;
}

static int iotx_http2_get_url(char *buf, char *productkey)
{
#if defined(ON_DAILY)
    sprintf(buf, "%s", "10.101.12.205");
    return 9999;
#elif defined(ON_PRE)
    sprintf(buf, "%s", "100.67.141.158");
    return 8443;
#else
    sprintf(buf, "%s", productkey);
    strcat(buf, ".iot-as-http2.cn-shanghai.aliyuncs.com");
    return 443;
#endif
}

static void file_upload_gen_string(char *str, int type, char *para1, int para2)
{
    switch (type) {
        case NUM_STRING_ENUM: {
            sprintf(str, "%d", para2);
            break;
        }
        case PATH_CREATE_STR_ENUM:
        case PATH_UPLOAD_STR_ENUM:
        case ORI_SIGN_STR_ENUM:
        case CID_STRING_ENUM: {
            if (type == PATH_CREATE_STR_ENUM) {
                sprintf(str, "/message/pub_with_resp/sys/%s/%s/thing/%s/create",
                        g_device_info.product_key,
                        g_device_info.device_name,
                        para1);
            } else if (type == PATH_UPLOAD_STR_ENUM) {
                sprintf(str, "/message/pub_with_resp/sys/%s/%s/thing/%s/upload",
                        g_device_info.product_key,
                        g_device_info.device_name,
                        para1);
            } else if (type == ORI_SIGN_STR_ENUM) {
                sprintf(str, "clientId%sdeviceName%sproductKey%s",
                        para1,
                        g_device_info.device_name,
                        g_device_info.product_key);
            } else {
                sprintf(str, "%s.%s", g_device_info.product_key, g_device_info.device_name);
            }
            break;
        }
        case REAL_SIGN_STR_ENUM: {
            utils_hmac_md5(para1, strlen(para1), str, g_device_info.device_secret, strlen(g_device_info.device_secret));
            break;
        }
        default: {
            h2stream_err("ASSERT\n");
            break;
        }
    }
}

static void *http2_io(void *user_data)
{
    while (1) {
        HAL_MutexLock(((http2_connection_t*)user_data)->mutex);
        iotx_http2_exec_io((http2_connection_t*)user_data);
        HAL_MutexUnlock(((http2_connection_t*)user_data)->mutex);

        HAL_SleepMs(100);
    }

    return 0;
}

static int http2_stream_node_insert(http2_connection_t *connection, unsigned int id, http2_stream_node_t **p_node)
{
    http2_stream_node_t *node = NULL;
    void *semaphore = NULL;

    if (connection == NULL || id == 0) {
        return FAIL_RETURN;
    }

    node = (http2_stream_node_t *)HAL_Malloc(sizeof(http2_stream_node_t));
    if (node == NULL) {
        return FAIL_RETURN;
    }

    memset(node, 0, sizeof(http2_stream_node_t));
    node->stream_id = id;

    semaphore = HAL_SemaphoreCreate();
    if (semaphore == NULL) {
        HAL_Free(node);
        return FAIL_RETURN;
    }
    node->semaphore = semaphore;

    INIT_LIST_HEAD((list_head_t *)&node->list);
    list_add((list_head_t *)&node->list, (list_head_t *)&connection->stream_list);

    if (p_node != NULL) {
        *p_node = node;
    }

    return SUCCESS_RETURN;
}

static int http2_stream_node_remove(http2_connection_t *connection, unsigned int id)
{
    http2_stream_node_t *search_node;

    if (connection == NULL || id == 0) {
        return FAIL_RETURN;
    }

    list_for_each_entry(search_node, &connection->stream_list, list, http2_stream_node_t) {
        if (id == search_node->stream_id) {
            h2stream_info("stream_node found, delete\n");

            list_del((list_head_t *)&search_node->list);
            HAL_Free(search_node->app_stream_id);
            HAL_SemaphoreDestroy(search_node->semaphore);
            HAL_Free(search_node);
            return SUCCESS_RETURN;
        }
    }

    return FAIL_RETURN;
}

http2_connection_t *IOT_HTTP2_Stream_Connect( device_conn_info_t *conn_info,http2_user_cb_t *user_cb)
{
    http2_connection_t *conn = NULL;
    char buf[100] = {0};
    int port = 0;
    int ret = 0;
    static char status[4] = {0};

    memset(&g_client, 0, sizeof(httpclient_t));

    if (conn_info->product_key == NULL ||
        conn_info->device_name == NULL ||
        conn_info->device_secret == NULL) {
         h2stream_err("device parameter is error.\n");
        return NULL;
    }
    _set_device_info(conn_info->product_key, conn_info->device_name, conn_info->device_secret);

    if (conn_info->url == NULL || conn_info->port == 0) {
        port = iotx_http2_get_url(buf, conn_info->product_key);
        conn = iotx_http2_client_connect_with_cb((void *)&g_client, buf, port,user_cb);
    } else {
        conn = iotx_http2_client_connect_with_cb((void *)&g_client, conn_info->url, conn_info->port,user_cb);
    }
    if (conn == NULL) {
        return NULL;
    }

    conn->statuscode = status;

    conn->mutex = HAL_MutexCreate();
    if (conn->mutex == NULL) {
        h2stream_err("mutex create error\n");
    }

    hal_os_thread_param_t thread_parms = {0};
    thread_parms.stack_size = 6144;
    thread_parms.name = "http2_io";
    ret = HAL_ThreadCreate(&conn->thread_handle, http2_io, (void *)conn, &thread_parms, NULL);
    if (ret != 0) {
        h2stream_err("thread create error\n");
    }

    INIT_LIST_HEAD((list_head_t *)&(conn->stream_list));

    return conn;
}

int IOT_HTTP2_Stream_Open(http2_connection_t *connection, stream_data_info_t *info, header_ext_info_t *header)
{
    char client_id[64 + 1] = {0};
    char sign_str[256 + 1] = {0};
    char sign[41 + 1] = {0};
    char path[128] = {0};
    int header_count = 0;
    int rv = 0;
    http2_data h2_data;
    http2_stream_node_t *node = NULL;
    http2_header nva[MAX_HTTP2_HEADER_NUM] = {{0}};

    if (info == NULL || connection == NULL) {
        h2stream_err("parameter is error.\n");
        return -1;
    }

    memset(&h2_data, 0, sizeof(http2_data));

    HAL_Snprintf(path, sizeof(path), "/stream/open/%s", info->identify);

    file_upload_gen_string(client_id, CID_STRING_ENUM, NULL, 0);
    file_upload_gen_string(sign_str, ORI_SIGN_STR_ENUM, client_id, 0);
    file_upload_gen_string(sign, REAL_SIGN_STR_ENUM, sign_str, 0);

    const http2_header static_header[] = { MAKE_HEADER(":method", "POST"),
                                           MAKE_HEADER_CS(":path", path),
                                           MAKE_HEADER(":scheme", "https"),
                                           MAKE_HEADER("x-auth-name", "devicename"),
                                           MAKE_HEADER_CS("x-auth-param-client-id",client_id),
                                           MAKE_HEADER("x-auth-param-signmethod", "hmacmd5"),
                                           MAKE_HEADER_CS("x-auth-param-product-key", g_device_info.product_key),
                                           MAKE_HEADER_CS("x-auth-param-device-name", g_device_info.device_name),
                                           MAKE_HEADER_CS("x-auth-param-sign", sign),
                                           MAKE_HEADER("content-length", "0"),
                                         };
    header_count = sizeof(static_header) / sizeof(static_header[0]);

    /* add external header if it's not NULL */
    header_count = http2_nv_copy(nva, 0, (http2_header *)static_header, sizeof(static_header) / sizeof(static_header[0]));
    if (header != NULL) {
        header_count = http2_nv_copy(nva, header_count, (http2_header *)header->nva, header->num);
    }

    h2_data.header = (http2_header *)nva;
    h2_data.header_count = header_count;
    h2_data.data = NULL;
    h2_data.len = 0;
    h2_data.flag = 1 ;
    h2_data.stream_id = 0;

    HAL_MutexLock(connection->mutex);
    rv = iotx_http2_client_send((void *)connection, &h2_data);
    HAL_MutexUnlock(connection->mutex);
    
    if (rv < 0) {
        h2stream_err("client send error\n");
        return rv;
    }

    http2_stream_node_insert(connection, h2_data.stream_id, &node);

    rv = HAL_SemaphoreWait(node->semaphore, IOT_HTTP2_RES_OVERTIME_MS);
    if (rv < 0) {
        h2stream_err("semaphore wait overtime\n");
        http2_stream_node_remove(connection, node->stream_id);
        return FAIL_RETURN;
    }

    if (memcmp(node->status_code, "200", 3)) {
        http2_stream_node_remove(connection, node->stream_id);
        return FAIL_RETURN;
    }

    info->stream_id = node->app_stream_id;

    return SUCCESS_RETURN;
}

int IOT_HTTP2_Stream_Send(http2_connection_t *connection, stream_data_info_t *info)
{
    int rv = 0;
    http2_data h2_data;
    char path[128] = {0};
    char data_len_str[33] = {0};
    int windows_size;
    int count = 0;
    if (connection == NULL || info == NULL) {
        h2stream_err("parameter is error.\n");
        return -1;
    }

    windows_size = iotx_http2_get_available_window_size(connection);
    while (windows_size < info->packet_len) {
        h2stream_warning("windows_size < info->packet_len ,wait ...\n");
        HAL_SleepMs(50);
        rv = iotx_http2_update_window_size(connection);
        if (rv < 0) {
            h2stream_err("update window size err\n");
            return -1;
        }

        if(++count > 100) {
            return -1;
        }
        windows_size = iotx_http2_get_available_window_size(connection);
    }

    snprintf(data_len_str, sizeof(data_len_str), "%d", info->stream_len);
    //snprintf(stream_id_str, sizeof(stream_id_str), "%d", info->stream_id);
    snprintf(path, sizeof(path), "/stream/send/%s", info->identify);
    if (info->send_len == 0) { //first send,need header
        const http2_header static_header[] = { MAKE_HEADER(":method", "POST"),
                                               MAKE_HEADER_CS(":path", path),
                                               MAKE_HEADER(":scheme", "https"),
                                               MAKE_HEADER_CS("content-length", data_len_str),
                                               MAKE_HEADER_CS("x-data-stream-id", info->stream_id),
                                               MAKE_HEADER_CS("x-auth-param-product-key", g_device_info.product_key),
                                               MAKE_HEADER_CS("x-auth-param-device-name", g_device_info.device_name),
                                             };

        int header_count = sizeof(static_header) / sizeof(static_header[0]);
        print_header((http2_header *)static_header, header_count);
        h2_data.header = (http2_header *)static_header;
        h2_data.header_count = header_count;
        h2_data.data = info->stream;
        h2_data.len = info->packet_len;
        
        if(info->packet_len + info->send_len == info->stream_len ) { //last frame
            h2_data.flag = 1;
        } else {
            h2_data.flag = 0;
        }

        HAL_MutexLock(connection->mutex);
        rv = iotx_http2_client_send((void *)connection, &h2_data);
        HAL_MutexUnlock(connection->mutex);

        if (rv < 0) {
            return -1;
        }

        stream_id = h2_data.stream_id;
        info->send_len += info->packet_len;
    } else {
        h2_data.header = NULL;
        h2_data.header_count = 0;
        h2_data.data = info->stream;
        h2_data.len = info->packet_len;
        
        h2_data.stream_id = stream_id;
        if(info->packet_len + info->send_len == info->stream_len ) { //last frame
            h2_data.flag = 1;
        } else {
           h2_data.flag = 0; 
        }
        rv = iotx_http2_client_send((void *)connection, &h2_data);

        if (rv >= 0) {
            info->send_len += info->packet_len;
        }
    }

    if(h2_data.flag ==1) {
        http2_stream_node_t *node = NULL;
        http2_stream_node_insert(connection, h2_data.stream_id, &node);

        rv = HAL_SemaphoreWait(node->semaphore, IOT_HTTP2_RES_OVERTIME_MS);
        if (rv < 0) {
            h2stream_err("semaphore wait overtime\n");
            http2_stream_node_remove(connection, node->stream_id);
            return FAIL_RETURN;
        }

        if (memcmp(node->status_code, "200", 3)) {
            http2_stream_node_remove(connection, node->stream_id);
            return FAIL_RETURN;
        }
    }
    return rv;
}

int IOT_HTTP2_Stream_Query(http2_connection_t *connection, stream_data_info_t *info)
{
    int rv = 0;
    http2_data h2_data;
    char path[128] = {0};

    if (connection == NULL || info == NULL) {
        h2stream_err("parameter is error.\n");
        return -1;
    }

    HAL_Snprintf(path, sizeof(path), "/stream/send/%s", info->identify);
    const http2_header static_header[] = { MAKE_HEADER(":method", "GET"),
                                           MAKE_HEADER_CS(":path", path),
                                           MAKE_HEADER(":scheme", "https"),
                                           MAKE_HEADER_CS("x-data-stream-id", info->stream_id),
                                           MAKE_HEADER("content-length", "0"),
                                         };

    int header_count = sizeof(static_header) / sizeof(static_header[0]);

    h2_data.header = (http2_header *)static_header;
    h2_data.header_count = header_count;
    h2_data.data = NULL;
    h2_data.len = 0;
    h2_data.flag = 1;
    h2_data.stream_id = 0;
    
    HAL_MutexLock(connection->mutex);
    rv = iotx_http2_client_send((void *)connection, &h2_data);
    HAL_MutexUnlock(connection->mutex);

    if (rv < 0) {
        h2stream_err("client send error\n");
        return rv;
    }

    http2_stream_node_insert(connection, h2_data.stream_id, NULL);

    return rv;
}

int IOT_HTTP2_Stream_Close(http2_connection_t *connection, stream_data_info_t *info)
{
    int rv = 0;
    http2_data h2_data;
    char path[128] = {0};
    if (connection == NULL || info == NULL) {
         h2stream_err("parameter is error.\n");
        return -1;
    }

    snprintf(path, sizeof(path), "/stream/close/%s", info->identify);
    const http2_header static_header[] = { MAKE_HEADER(":method", "POST"),
                                           MAKE_HEADER_CS(":path", path),
                                           MAKE_HEADER(":scheme", "https"),
                                           MAKE_HEADER_CS("x-data-stream-id", info->stream_id),
                                         };

    int header_count = sizeof(static_header) / sizeof(static_header[0]);
    h2_data.header = (http2_header *)static_header;
    h2_data.header_count = header_count;
    h2_data.data = NULL;
    h2_data.len = 0;
    h2_data.flag = 1;
    h2_data.stream_id = stream_id;

    HAL_MutexLock(connection->mutex);
    rv = iotx_http2_client_send((void *)connection, &h2_data);
    HAL_MutexUnlock(connection->mutex);

    if (rv < 0) {
        h2stream_err("client send error\n");
        return rv;
    }

    /* just delete stream node */
    char *stream_id = info->stream_id;
    int len = strlen(stream_id);
    http2_stream_node_t *node;
    list_for_each_entry(node, &connection->stream_list, list, http2_stream_node_t) {
        if ((len == strlen(node->app_stream_id) && !memcmp(node->app_stream_id, stream_id, len))) {
            h2stream_info("stream_node found: %s, Delete It", node->app_stream_id);
            list_del((list_head_t *)&node->list);
            HAL_Free(node->app_stream_id);
            HAL_SemaphoreDestroy(node->semaphore);
            HAL_Free(node);
        }
    }

    return rv;
}

int IOT_HTTP2_Stream_Ping(http2_connection_t *connection)
{
    return iotx_http2_client_send_ping(connection);
}
