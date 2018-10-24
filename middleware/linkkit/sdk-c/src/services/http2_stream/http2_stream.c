/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "iot_export.h"
#include "utils_hmac.h"
#include "iotx_utils.h"
#include "iot_export_http2.h"
#include "iot_export_http2_stream.h"
#include "lite-list.h"

#define KEEP_ALIVE_TIMES                     (60) /*seconds*/
#define MAX_HTTP2_INTERVAL_TIMES             (3)  /*seconds*/


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

static device_info g_device_info;
static stream_handle_t *g_stream_handle = NULL;
static httpclient_t g_client;

static int _set_device_info(char *pk, char *dn, char *ds)
{
    memcpy(g_device_info.product_key, pk, PRODUCT_KEY_LEN);
    memcpy(g_device_info.device_name, dn, DEVICE_NAME_LEN);
    memcpy(g_device_info.device_secret, ds, DEVICE_SECRET_LEN);

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
            utils_hmac_sha1(para1, strlen(para1), str, g_device_info.device_secret, strlen(g_device_info.device_secret));
            break;
        }
        default: {
            h2stream_err("ASSERT\n");
            break;
        }
    }
}

static int http2_stream_node_search(stream_handle_t *handle, unsigned int stream_id, http2_stream_node_t **p_node)
{
    *p_node = NULL;

    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(p_node, NULL_VALUE_ERROR);

    http2_stream_node_t *search_node = NULL;
    list_for_each_entry(search_node, &handle->stream_list, list, http2_stream_node_t) {
        if (search_node->stream_id == stream_id) {
            *p_node = search_node;
            return SUCCESS_RETURN;
        }
    }

    h2stream_debug("stream node not exist, stream_id = %d", stream_id);
    return FAIL_RETURN;
}

static void on_stream_header(int32_t stream_id, int cat, const uint8_t *name, size_t namelen,
                             const uint8_t *value, size_t valuelen, uint8_t flags)
{
    http2_stream_node_t *node = NULL;

    if (g_stream_handle == NULL) {
        return;
    }
    http2_stream_node_search(g_stream_handle, stream_id, &node);
    if (node == NULL) {
        return;
    }

    switch (cat) {
        case 0x01:
            if (strncmp((char *)name, "x-data-stream-id", (int)namelen) == 0) {
                node->channel_id = HAL_Malloc(valuelen + 1);
                if (node->channel_id == NULL) {
                    return;
                }
                memset(node->channel_id, 0, (int)valuelen + 1);
                memcpy(node->channel_id, (char *)value, (int)valuelen);
            } else if (strncmp((char *)name, "x-response-status", (int)namelen) == 0) {
                strncpy(node->status_code, (char *)value, (int)valuelen);
                HAL_SemaphorePost(node->semaphore);
            } else if (strncmp((char *)name, ":status", (int)namelen) == 0 && 
                strncmp((char *)value, "200", (int)valuelen)) {
                    strncpy(node->status_code, (char *)value, (int)valuelen);
                    HAL_SemaphorePost(node->semaphore);
            }
    }

    if (g_stream_handle->cbs && g_stream_handle->cbs->on_stream_header_cb) {
        g_stream_handle->cbs->on_stream_header_cb(node->channel_id, cat, name, namelen, value, valuelen, flags);
    }
}

static void on_stream_chunk_recv(int32_t stream_id, const uint8_t *data, size_t len, uint8_t flags)
{
    http2_stream_node_t *node;
    if (g_stream_handle == NULL) {
        return;
    }
    http2_stream_node_search(g_stream_handle, stream_id, &node);
    if (node == NULL) {
        return;
    }
    if (STREAM_TYPE_DOWNLOAD != node->stream_type) {
        return;
    }

    if (g_stream_handle->cbs && g_stream_handle->cbs->on_stream_chunk_recv_cb) {
        g_stream_handle->cbs->on_stream_chunk_recv_cb(node->channel_id, data, len, flags);
    }
}

static void on_stream_close(int32_t stream_id, uint32_t error_code)
{
    http2_stream_node_t *node;
    if (g_stream_handle == NULL) {
        return;
    }
    http2_stream_node_search(g_stream_handle, stream_id, &node);
    if (node == NULL) {
        return;
    }    
    if (g_stream_handle->cbs && g_stream_handle->cbs->on_stream_close_cb) {
        g_stream_handle->cbs->on_stream_close_cb(node->channel_id, error_code);
    }
}

static  void on_stream_frame_send(int32_t stream_id, int type, uint8_t flags)
{
    http2_stream_node_t *node;
    if (g_stream_handle == NULL) {
        return;
    }
    http2_stream_node_search(g_stream_handle, stream_id, &node);
    if (node == NULL) {
        return;
    }
    if (g_stream_handle->cbs && g_stream_handle->cbs->on_stream_frame_send_cb) {
        g_stream_handle->cbs->on_stream_frame_send_cb(node->channel_id, type, flags);
    }
}

static void on_stream_frame_recv(int32_t stream_id, int type, uint8_t flags)
{
    http2_stream_node_t *node;
    if (g_stream_handle == NULL) {
        return;
    }
    http2_stream_node_search(g_stream_handle, stream_id, &node);
    if (node == NULL) {
        return;
    }

    if (g_stream_handle->cbs && g_stream_handle->cbs->on_stream_frame_recv_cb) {
        g_stream_handle->cbs->on_stream_frame_recv_cb(node->channel_id, type, flags);
    }
}

static http2_user_cb_t my_cb = {
    .on_user_header_cb = on_stream_header,
    .on_user_chunk_recv_cb = on_stream_chunk_recv,
    .on_user_stream_close_cb = on_stream_close,
    .on_user_frame_send_cb = on_stream_frame_send,
    .on_user_frame_recv_cb = on_stream_frame_recv,
};

static void *http2_io(void *user_data)
{
    stream_handle_t *handle = (stream_handle_t *)user_data;
    POINTER_SANITY_CHECK(handle, NULL);

    while (handle->init_state) {
        HAL_MutexLock(handle->mutex);
        iotx_http2_exec_io(handle->http2_connect);
        HAL_MutexUnlock(handle->mutex);
        HAL_SleepMs(100);
    }
    HAL_SemaphorePost(handle->semaphore);
    return NULL;
}

static int http2_stream_node_insert(stream_handle_t *handle, unsigned int id, http2_stream_node_t **p_node)
{
    http2_stream_node_t *node = NULL;
    void *semaphore = NULL;

    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);

    ARGUMENT_SANITY_CHECK(id != 0, FAIL_RETURN);

    if (p_node != NULL) {
        *p_node = NULL;
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
    list_add((list_head_t *)&node->list, (list_head_t *)&handle->stream_list);

    if (p_node != NULL) {
        *p_node = node;
    }

    return SUCCESS_RETURN;
}

static int http2_stream_node_remove(stream_handle_t *handle, unsigned int id)
{
    http2_stream_node_t *search_node;

    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);
    ARGUMENT_SANITY_CHECK(id != 0, FAIL_RETURN);

    list_for_each_entry(search_node, &handle->stream_list, list, http2_stream_node_t) {
        if (id == search_node->stream_id) {
            h2stream_info("stream_node found, delete\n");

            list_del((list_head_t *)&search_node->list);
            HAL_Free(search_node->channel_id);
            HAL_SemaphoreDestroy(search_node->semaphore);
            HAL_Free(search_node);
            return SUCCESS_RETURN;
        }
    }
    return FAIL_RETURN;
}

stream_handle_t *IOT_HTTP2_Stream_Connect(device_conn_info_t *conn_info, http2_stream_cb_t *user_cb)
{
    stream_handle_t *stream_handle = NULL;
    http2_connection_t *conn = NULL;
    char buf[100] = {0};
    int port = 0;
    int ret = 0;
    static char status[4] = {0};
    POINTER_SANITY_CHECK(conn_info, NULL);
    POINTER_SANITY_CHECK(conn_info->product_key, NULL);
    POINTER_SANITY_CHECK(conn_info->device_name, NULL);    
    POINTER_SANITY_CHECK(conn_info->device_secret, NULL);

    memset(&g_client, 0, sizeof(httpclient_t));
    
    stream_handle = malloc(sizeof(stream_handle_t));
    if (stream_handle == NULL) {
        return NULL;
    }
    stream_handle->mutex = HAL_MutexCreate();
    if (stream_handle->mutex == NULL) {
        HAL_Free(stream_handle);
        h2stream_err("mutex create error\n");
        return NULL;
    }
    stream_handle->semaphore = HAL_SemaphoreCreate();
    if (stream_handle->semaphore == NULL) {
        h2stream_err("semaphore create error\n");
        HAL_MutexDestroy(stream_handle->mutex);
        HAL_Free(stream_handle);
        return NULL;
    }

    INIT_LIST_HEAD((list_head_t *) & (stream_handle->stream_list));

    _set_device_info(conn_info->product_key, conn_info->device_name, conn_info->device_secret);
    g_stream_handle = stream_handle;
    g_stream_handle->cbs = user_cb;
    if (conn_info->url == NULL || conn_info->port == 0) {
        port = iotx_http2_get_url(buf, conn_info->product_key);
        conn = iotx_http2_client_connect_with_cb((void *)&g_client, buf, port, &my_cb);
    } else {
        conn = iotx_http2_client_connect_with_cb((void *)&g_client, conn_info->url, conn_info->port, &my_cb);
    }
    if (conn == NULL) {
        HAL_MutexDestroy(stream_handle->mutex);
        HAL_SemaphoreDestroy(stream_handle->semaphore);
        HAL_Free(stream_handle);
        return NULL;
    }
    stream_handle->http2_connect = conn;
    stream_handle->init_state = 1;

    conn->statuscode = status;
    hal_os_thread_param_t thread_parms = {0};
    thread_parms.stack_size = 6144;
    thread_parms.name = "http2_io";
    ret = HAL_ThreadCreate(&stream_handle->thread_handle, http2_io, stream_handle, &thread_parms, NULL);
    if (ret != 0) {
        h2stream_err("thread create error\n");
        IOT_HTTP2_Stream_Disconnect(stream_handle);
        return NULL;
    }


    return stream_handle;
}

int IOT_HTTP2_Stream_Open(stream_handle_t *handle, stream_data_info_t *info, header_ext_info_t *header)
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

    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(info, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(info->identify, NULL_VALUE_ERROR);

    memset(&h2_data, 0, sizeof(http2_data));

    HAL_Snprintf(path, sizeof(path), "/stream/open/%s", info->identify);

    file_upload_gen_string(client_id, CID_STRING_ENUM, NULL, 0);
    file_upload_gen_string(sign_str, ORI_SIGN_STR_ENUM, client_id, 0);
    file_upload_gen_string(sign, REAL_SIGN_STR_ENUM, sign_str, 0);

    const http2_header static_header[] = { MAKE_HEADER(":method", "POST"),
                                           MAKE_HEADER_CS(":path", path),
                                           MAKE_HEADER(":scheme", "https"),
                                           MAKE_HEADER("x-auth-name", "devicename"),
                                           MAKE_HEADER_CS("x-auth-param-client-id", client_id),
                                           MAKE_HEADER("x-auth-param-signmethod", "hmacsha1"),
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

    HAL_MutexLock(handle->mutex);
    rv = iotx_http2_client_send((void *)handle->http2_connect, &h2_data);
    http2_stream_node_insert(handle, h2_data.stream_id, &node);
    HAL_MutexUnlock(handle->mutex);

    if (rv < 0) {
        h2stream_err("client send error\n");
        return rv;
    }

    if (node == NULL) {
        return FAIL_RETURN;
    }

    node->stream_type = STREAM_TYPE_AUXILIARY;
    rv = HAL_SemaphoreWait(node->semaphore, IOT_HTTP2_RES_OVERTIME_MS);
    if (rv < 0 || memcmp(node->status_code, "200", 3)) {
        h2stream_err("semaphore wait overtime or status code error\n");
        HAL_MutexLock(handle->mutex);
        http2_stream_node_remove(handle, node->stream_id);
        HAL_MutexUnlock(handle->mutex);
        return FAIL_RETURN;
    }

    info->channel_id = node->channel_id;

    return SUCCESS_RETURN;
}

int IOT_HTTP2_Stream_Send(stream_handle_t *handle, stream_data_info_t *info)
{
    int rv = 0;
    http2_data h2_data;
    char path[128] = {0};
    char data_len_str[33] = {0};
    int windows_size;
    int count = 0;
    http2_stream_node_t *node = NULL;

    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(info, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(info->stream, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(info->channel_id, NULL_VALUE_ERROR);
    ARGUMENT_SANITY_CHECK(info->stream_len !=0, FAIL_RETURN);
    ARGUMENT_SANITY_CHECK(info->packet_len !=0, FAIL_RETURN);

    windows_size = iotx_http2_get_available_window_size(handle->http2_connect);
    while (windows_size < info->packet_len) {
        h2stream_warning("windows_size < info->packet_len ,wait ...\n");
        HAL_SleepMs(50);
        rv = iotx_http2_update_window_size(handle->http2_connect);
        if (rv < 0) {
            h2stream_err("update window size err\n");
            return -1;
        }

        if (++count > 100) {
            return -1;
        }
        windows_size = iotx_http2_get_available_window_size(handle->http2_connect);
    }

    snprintf(data_len_str, sizeof(data_len_str), "%d", info->stream_len);
    //snprintf(stream_id_str, sizeof(stream_id_str), "%d", info->stream_id);
    snprintf(path, sizeof(path), "/stream/send/%s", info->identify);
    if (info->send_len == 0) { //first send,need header
        const http2_header static_header[] = { MAKE_HEADER(":method", "POST"),
                                               MAKE_HEADER_CS(":path", path),
                                               MAKE_HEADER(":scheme", "https"),
                                               MAKE_HEADER_CS("content-length", data_len_str),
                                               MAKE_HEADER_CS("x-data-stream-id", info->channel_id),
                                             };

        int header_count = sizeof(static_header) / sizeof(static_header[0]);
        
        memset(&h2_data, 0, sizeof(h2_data));
        h2_data.header = (http2_header *)static_header;
        h2_data.header_count = header_count;
        h2_data.data = info->stream;
        h2_data.len = info->packet_len;

        if (info->packet_len + info->send_len == info->stream_len) { //last frame
            h2_data.flag = 1;
        } else {
            h2_data.flag = 0;
        }

        HAL_MutexLock(handle->mutex);
        rv = iotx_http2_client_send((void *)handle->http2_connect, &h2_data);
        http2_stream_node_insert(handle, h2_data.stream_id, &node);
        HAL_MutexUnlock(handle->mutex);

        if (rv < 0) {
            return -1;
        }

        if (node == NULL) {
            return FAIL_RETURN;
        }

        node->stream_type = STREAM_TYPE_UPLOAD;
        info->h2_stream_id = h2_data.stream_id;
        //stream_id = h2_data.stream_id;
        info->send_len += info->packet_len;
    } else {
        h2_data.header = NULL;
        h2_data.header_count = 0;
        h2_data.data = info->stream;
        h2_data.len = info->packet_len;

        h2_data.stream_id = info->h2_stream_id;
        if (info->packet_len + info->send_len == info->stream_len) { //last frame
            h2_data.flag = 1;
        } else {
            h2_data.flag = 0;
        }
        rv = iotx_http2_client_send((void *)handle->http2_connect, &h2_data);

        if (rv >= 0) {
            info->send_len += info->packet_len;
        }
    }

    if (h2_data.flag == 1) {
        http2_stream_node_t *node =NULL;
        HAL_MutexLock(handle->mutex);
        http2_stream_node_search(handle, h2_data.stream_id ,&node);
        HAL_MutexUnlock(handle->mutex);
        if (node == NULL) {
            return FAIL_RETURN;
        }
        rv = HAL_SemaphoreWait(node->semaphore, IOT_HTTP2_RES_OVERTIME_MS);
        if (rv < 0 || memcmp(node->status_code, "200", 3)) {
            h2stream_err("semaphore wait overtime or status code error\n");
            HAL_MutexLock(handle->mutex);
            http2_stream_node_remove(handle, node->stream_id);
            HAL_MutexUnlock(handle->mutex);
            return FAIL_RETURN;
        }
    }
    return rv;
}

int IOT_HTTP2_Stream_Query(stream_handle_t *handle, stream_data_info_t *info)
{
    int rv = 0;
    http2_data h2_data;
    http2_stream_node_t *node = NULL;
    char path[128] = {0};

    POINTER_SANITY_CHECK(info, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(info->channel_id, NULL_VALUE_ERROR);
    
    HAL_Snprintf(path, sizeof(path), "/stream/send/%s", info->identify);
    const http2_header static_header[] = { MAKE_HEADER(":method", "GET"),
                                           MAKE_HEADER_CS(":path", path),
                                           MAKE_HEADER(":scheme", "https"),
                                           MAKE_HEADER_CS("x-data-stream-id", info->channel_id),
                                           MAKE_HEADER("x-test-downstream", "1"),
                                         };

    int header_count = sizeof(static_header) / sizeof(static_header[0]);

    h2_data.header = (http2_header *)static_header;
    h2_data.header_count = header_count;
    h2_data.data = NULL;
    h2_data.len = 0;
    h2_data.flag = 1;
    h2_data.stream_id = 0;

    HAL_MutexLock(handle->mutex);
    rv = iotx_http2_client_send((void *)handle->http2_connect, &h2_data);
    http2_stream_node_insert(handle, h2_data.stream_id, &node);
    HAL_MutexUnlock(handle->mutex);

    if (rv < 0) {
        h2stream_err("client send error\n");
        return rv;
    }

    if (node == NULL) {
        return FAIL_RETURN;
    }    

    node->stream_type = STREAM_TYPE_DOWNLOAD;
    rv = HAL_SemaphoreWait(node->semaphore, IOT_HTTP2_RES_OVERTIME_MS);
    if (rv < 0 || memcmp(node->status_code, "200", 3)) {
        h2stream_err("semaphore wait overtime or status code error\n");
        HAL_MutexLock(handle->mutex);
        http2_stream_node_remove(handle, node->stream_id);
        HAL_MutexUnlock(handle->mutex);
        return FAIL_RETURN;
    }    

    return rv;
}

int IOT_HTTP2_Stream_Close(stream_handle_t *handle, stream_data_info_t *info)
{
    int rv = 0;
    http2_data h2_data;
    char path[128] = {0};

    POINTER_SANITY_CHECK(info, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(info->channel_id, NULL_VALUE_ERROR);

    snprintf(path, sizeof(path), "/stream/close/%s", info->identify);
    const http2_header static_header[] = { MAKE_HEADER(":method", "POST"),
                                           MAKE_HEADER_CS(":path", path),
                                           MAKE_HEADER(":scheme", "https"),
                                           MAKE_HEADER_CS("x-data-stream-id", info->channel_id),
                                         };

    int header_count = sizeof(static_header) / sizeof(static_header[0]);
    h2_data.header = (http2_header *)static_header;
    h2_data.header_count = header_count;
    h2_data.data = NULL;
    h2_data.len = 0;
    h2_data.flag = 1;
    h2_data.stream_id = 0;

    HAL_MutexLock(handle->mutex);
    rv = iotx_http2_client_send((void *)handle->http2_connect, &h2_data);
    HAL_MutexUnlock(handle->mutex);

    if (rv < 0) {
        h2stream_err("client send error\n");
        return rv;
    }

    /* just delete stream node */
    char *stream_id = info->channel_id;
    int len = strlen(stream_id);
    http2_stream_node_t *node;
    HAL_MutexLock(handle->mutex);
    list_for_each_entry(node, &handle->stream_list, list, http2_stream_node_t) {
        if (info->h2_stream_id == node->stream_id) {
            h2stream_info("stream_node found:stream_id= %d, Delete It", node->stream_id);
            list_del((list_head_t *)&node->list);
            HAL_Free(node->channel_id);
            HAL_SemaphoreDestroy(node->semaphore);
            HAL_Free(node);
            continue;
        }       
        if ((node->channel_id != NULL) && (len == strlen(node->channel_id) && !memcmp(node->channel_id, stream_id, len))) {
            h2stream_info("stream_node found: %s, Delete It", node->channel_id);
            list_del((list_head_t *)&node->list);
            HAL_Free(node->channel_id);
            HAL_SemaphoreDestroy(node->semaphore);
            HAL_Free(node);
        }
    }
    HAL_MutexUnlock(handle->mutex);
    return rv;
}

int IOT_HTTP2_Stream_Disconnect(stream_handle_t *handle)
{
    int ret;
    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);
    handle->init_state = 0;

    ret = HAL_SemaphoreWait(handle->semaphore, IOT_HTTP2_RES_OVERTIME_MS);
    if (ret < 0) {
        h2stream_err("semaphore wait overtime\n");
        return FAIL_RETURN;
    }
    http2_stream_node_t *node;
    HAL_MutexLock(handle->mutex);
    list_for_each_entry(node, &handle->stream_list, list, http2_stream_node_t) {
        list_del((list_head_t *)&node->list);
        HAL_Free(node->channel_id);
        HAL_SemaphoreDestroy(node->semaphore);
        HAL_Free(node);
    }
    HAL_MutexUnlock(handle->mutex);
    g_stream_handle = NULL;
    HAL_MutexDestroy(handle->mutex);
    HAL_SemaphoreDestroy(handle->semaphore);

    ret = iotx_http2_client_disconnect(handle->http2_connect);
    HAL_Free(handle);
    return ret;
}

#ifdef FS_ENABLED
#define PACKET_LEN 16384

typedef struct {
    stream_handle_t *handle;
    const char *path;
    const char *identify;
    upload_file_result_cb cb;
    header_ext_info_t *header;
    void *data;
}http2_stream_file_t;

static int http2_stream_get_file_size(const char *file_name)
{
    void *fp = NULL;
    int size = 0;
    if ((fp = HAL_Fopen(file_name, "r")) == NULL) {
        h2stream_err("The file %s can not be opened.\n", file_name);
        return -1;
    }
    HAL_Fseek(fp, 0L, HAL_SEEK_END);
    size = HAL_Ftell(fp);
    HAL_Fclose(fp);
    return size;
}

static int http2_stream_get_file_data(const char *file_name, char *data, int len, int offset)
{
    void *fp = NULL;
    int ret = 0;
    if ((fp = HAL_Fopen(file_name, "r")) == NULL) {
        h2stream_err("The file %s can not be opened.\n", file_name);
        return -1;
    }
    ret = HAL_Fseek(fp, offset, HAL_SEEK_SET);
    if (ret != 0) {
        h2stream_err("The file %s can not move offset.\n", file_name);
        return -1;
    }
    ret = HAL_Fread(data, len, 1, fp);
    HAL_Fclose(fp);
    return len;
}

static void * http_upload_file_func(void *user) {

    stream_data_info_t info;
    int ret;
    if(user == NULL) {
        return NULL;
    }
    
    http2_stream_file_t *user_data = (http2_stream_file_t *)user; 

    int file_size = http2_stream_get_file_size(user_data->path);

    if(file_size<=0) {
        if(user_data->cb) {
            user_data->cb(user_data->path,UPLOAD_FILE_NOT_EXIST,user_data->data);
        }
        HAL_Free(user_data);
        return NULL;
    }

    h2stream_info("file_size=%d",file_size);
    
    char *data = HAL_Malloc(PACKET_LEN);
    if(data == NULL) {
        user_data->cb(user_data->path,UPLOAD_MALLOC_FAILED,user_data->data);
        HAL_Free(user_data);
        return NULL;
    }

    memset(&info,0,sizeof(stream_data_info_t));
    info.stream = data;
    info.stream_len= file_size;
    info.packet_len=PACKET_LEN;
    //info.identify = "com/aliyun/iotx/vision/picture/device/upstream";
    info.identify = user_data->identify;

    ret = IOT_HTTP2_Stream_Open(user_data->handle, &info, user_data->header);
    if(ret < 0) {
        h2stream_err("IOT_HTTP2_Stream_Open failed %d\n", ret);
        if(user_data->cb) {
            user_data->cb(user_data->path,UPLOAD_STREAM_OPEN_FAILED,user_data->data);
        }
        HAL_Free(user_data);
        return NULL;
    }
    
    while(info.send_len<info.stream_len) {

        ret = http2_stream_get_file_data(user_data->path, data, PACKET_LEN, info.send_len);
        if(ret <= 0) {
            ret = -1;
            h2stream_err("read file err %d\n", ret);
            break;
        }
        info.packet_len = ret;
        h2stream_debug("get data len =%d\n", ret);
        if(info.stream_len-info.send_len<info.packet_len) {
            info.packet_len = info.stream_len-info.send_len;
        }

        ret = IOT_HTTP2_Stream_Send(user_data->handle, &info);
        if(ret < 0) {
            h2stream_err("send err %d\n", ret);
            break;
        }
    }
       
    if(user_data->cb) {

        if(ret < 0) {
            ret = UPLOAD_STREAM_SEND_FAILED;
        } else {
            ret = UPLOAD_SUCCESS;
        }
        user_data->cb(user_data->path,ret,user_data->data);
    }
    IOT_HTTP2_Stream_Close(user_data->handle, &info);

    HAL_Free(user_data);
    return NULL;
}

void * file_thread = NULL;
int IOT_HTTP2_Stream_UploadFile(stream_handle_t *handle,const char *file_path,const char *identify,header_ext_info_t *header, 
                                upload_file_result_cb cb, void *user_data)
{

    int ret;
    hal_os_thread_param_t thread_parms = {0};
    POINTER_SANITY_CHECK(handle, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(file_path, NULL_VALUE_ERROR);
    POINTER_SANITY_CHECK(identify, NULL_VALUE_ERROR);
    

    http2_stream_file_t *file_data = (http2_stream_file_t *)HAL_Malloc(sizeof(http2_stream_file_t));
    if(file_data == NULL) {
        return -1;
    }

    file_data->handle = handle;
    file_data->path =  file_path;
    file_data->identify = identify;
    file_data->cb = cb;
    file_data->data = user_data;
    file_data->header = header;

    thread_parms.stack_size = 6144;
    thread_parms.name = "file_upload";
    ret = HAL_ThreadCreate(&file_thread, http_upload_file_func, file_data, &thread_parms, NULL);
    if (ret != 0) {
        h2stream_err("thread create error\n");
        HAL_Free(file_data);
        return -1;
    }
    return 0;
}
#endif