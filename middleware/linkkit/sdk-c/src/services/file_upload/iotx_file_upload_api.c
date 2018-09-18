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
#include "iot_export_file_uploader.h"

#define KEEP_ALIVE_TIMES                     (60) /*seconds*/
#define MAX_HTTP2_INTERVAL_TIMES             (3)  /*seconds*/
#define MAX_HTTP2_FILE_NAME_LEN              (128)
#define MAX_HTTP2_SUPPORT_FILE_UPLOAD_NUM    (4)
#define MAX_HTTP2_PER_DATA_LEN               (1024 * 8)
#define MAX_HTTP2_DATA_SEND_TIMES            (64)

#define MAX_HTTP2_PER_SLICE_LEN        (MAX_HTTP2_DATA_SEND_TIMES * MAX_HTTP2_PER_DATA_LEN)

#define MAKE_HEADER(NAME, VALUE)                                             \
{                                                                            \
    (char *) NAME, (char *)VALUE, sizeof(NAME) - 1, sizeof(VALUE) - 1  \
}

#define MAKE_HEADER_CS(NAME, VALUE)                                          \
{                                                                            \
    (char *) NAME, (char *)VALUE, sizeof(NAME) - 1, strlen(VALUE)      \
}

typedef enum {
    NUM_STRING_ENUM      = 0,
    PATH_CREATE_STR_ENUM = 1,
    PATH_UPLOAD_STR_ENUM = 2,
    CID_STRING_ENUM      = 3,
    ORI_SIGN_STR_ENUM    = 4,
    FS_STRING_ENUM       = 5,
    REAL_SIGN_STR_ENUM   = 6,
}HEADER_TYPE_ENUM;

typedef struct _file_info_struct_ {
    char *filename;
    int file_size;
    int slice_num;
    int type;
    int is_recover;
}file_info;

typedef struct _file_data_info_struct_ {
    int curr_slice_num;
    int total_len;
    char *data;
    int data_len;
    int stream_id;
    int is_last_frame;
    int type;
    int need_auth;
}file_data_info;

typedef struct _file_upload_info_struct_ {
    char file_id[MAX_HTTP2_FILE_ID_LEN];
    char store_id[MAX_HTTP2_FILE_STORE_LEN];
    char *filename;
    int file_size;
    int curr_slice_num;
    int slice_num;
    int need_auth;
    int is_used;
    int type;
    int retry_count;
    void *user_data;
    iotx_upload_file_callback callback;
}file_upload_info;

typedef struct _file_upload_list_struct_ {
    struct _file_upload_list_struct_ *next;
    char           *name;
    int            type;
    iotx_upload_file_callback callback;
    void           *user_data;
}file_upload_list;

typedef struct _device_info_struct_ {
    char        product_key[PRODUCT_KEY_LEN + 1];
    char        device_name[DEVICE_NAME_LEN + 1];
    char        device_secret[DEVICE_SECRET_LEN + 1];
}device_info;

typedef struct _file_upload_contex_struct_ {
    file_upload_info  file_upload[MAX_HTTP2_SUPPORT_FILE_UPLOAD_NUM];
    void              *lock_thread;
    int               keep_alive;
    int               delay_time;
    int               num;
    int               max_num;
}file_upload_contex;

static http2_connection_t *iotx_get_http2_conn();
static char *find_file_name(char *name);
static void file_upload_gen_string(char *str, int type, char *para1, int para2);
static int iotx_cloud_file_create(http2_connection_t *conn, file_info *fileinfo, char *file_id,char *store_id);
static int iotx_data_upload_to_cloud(http2_connection_t *conn, char *file_id, char *store_id, file_data_info *file_data);
static void process_upload_info(file_upload_info *upload_info);
static int submit_file_auth_req(http2_connection_t *connection, int is_recover, int type);
static int submit_file_create_request(http2_connection_t *connection, file_info *fileinfo, void *user_data);
static int iotx_add_file_into_list(char *file_name, int type, iotx_upload_file_callback callback, void *user_data);
static void iotx_chagne_file_to_upload();
static HTTP2_UPLOAD_FILE_RET_TYPE iotx_upload_file_check_list(char *file_name, int type);

http2_connection_t *g_http2_conn = NULL;
file_upload_contex g_file_upload;
file_upload_contex *g_file_upload_ptr = &g_file_upload;
device_info g_device_info;
file_upload_list *g_file_list = NULL;

static http2_connection_t *iotx_get_http2_conn()
{
    return g_http2_conn;
}


static char *find_file_name(char *name)
{
    char *name_start = NULL;
    int sep = '/';

    if (NULL == name) {
      return NULL;
    }
    name_start = strrchr(name, sep);
    return (NULL == name_start) ? name : (name_start + 1);
}


static void file_upload_gen_string(char *str, int type, char *para1, int para2)
{
    switch(type) {
        case NUM_STRING_ENUM: {
            sprintf(str, "%d", para2);
            break;
        }
        case PATH_CREATE_STR_ENUM:
        case PATH_UPLOAD_STR_ENUM:
        case ORI_SIGN_STR_ENUM:
        case CID_STRING_ENUM:
        {
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
            } else if (type == ORI_SIGN_STR_ENUM){
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
            fsupload_err("ASSERT\n");
            break;
        }
    }
}


static int submit_file_create_request(http2_connection_t *connection, file_info *fileinfo, void *user_data)
{
    char num_str[5+1] = {0};
    char path_str[100] = {0};
    char client_id[64+1] = {0};
    char sign_str[256+1] = {0};
    char sign[41 + 1] = {0};
    char is_cover_str[2] = {0};
    char filesize_str[10 + 1] = {0};
    char *logstr = "log";
    char *filestr = "file";
    char type_str[10 + 1] = {0};
    char *filename = NULL;
    int filesize = fileinfo->file_size;
    int slice_num = fileinfo->slice_num;
    int is_recover = fileinfo->is_recover;
    int type = fileinfo->type;
    int rv = 0;
    int header_count = 0;
    http2_data h2_data;

    memset(&h2_data, 0, sizeof(http2_data));
    if(type == 1) { /*log*/
        strncpy(type_str, logstr, strlen(logstr));
    } else {
        strncpy(type_str, filestr, strlen(filestr));
    }
    file_upload_gen_string(num_str, NUM_STRING_ENUM, NULL, slice_num);
    file_upload_gen_string(path_str, PATH_CREATE_STR_ENUM, type_str, 0);
    file_upload_gen_string(client_id, CID_STRING_ENUM, NULL, 0);
    file_upload_gen_string(filesize_str, NUM_STRING_ENUM, sign_str, filesize);
    file_upload_gen_string(sign_str, ORI_SIGN_STR_ENUM, client_id, 0);
    file_upload_gen_string(sign, REAL_SIGN_STR_ENUM, sign_str, 0);
    if(is_recover == 0) {
      sprintf(is_cover_str, "%d", 0);
    } else {
      sprintf(is_cover_str, "%d", 1);
    }

    filename = find_file_name(fileinfo->filename);
    fsupload_info("filename is %s\n", filename);
#ifdef IOTX_H2_SUPPORT
    const http2_header header[] = {MAKE_HEADER(":method", "GET"),
                                   MAKE_HEADER_CS(":path", path_str),
                                   MAKE_HEADER(":scheme", "https"),
                                   MAKE_HEADER("x-auth-name", "devicename"),
                                   MAKE_HEADER_CS("x-auth-param-product-key", g_device_info.product_key),
                                   MAKE_HEADER_CS("x-auth-param-device-name", g_device_info.device_name),
                                   MAKE_HEADER_CS("x-auth-param-client-id", client_id),
                                   MAKE_HEADER_CS("x-auth-param-sign", sign),
                                   MAKE_HEADER_CS("x-file-name", filename),
                                   MAKE_HEADER_CS("x-file-size", filesize_str),
                                   MAKE_HEADER_CS("x-file-slice-number", num_str),
                                   MAKE_HEADER_CS("x-file-recover", is_cover_str)
                                  };
    header_count = sizeof(header) / sizeof(header[0]);
#else
    const http2_header header[] = {MAKE_HEADER(":method", "GET"),
                                   MAKE_HEADER_CS(":path", path_str),
                                   MAKE_HEADER("x-auth-name", "devicename"),
                                   MAKE_HEADER_CS("x-auth-param-product-key", g_device_info.product_key),
                                   MAKE_HEADER_CS("x-auth-param-device-name", g_device_info.device_name),
                                   MAKE_HEADER_CS("x-auth-param-client-id", client_id),
                                   MAKE_HEADER_CS("x-auth-param-sign", sign),
                                   MAKE_HEADER_CS("x-file-name", filename),
                                   MAKE_HEADER_CS("x-file-size", filesize_str),
                                   MAKE_HEADER_CS("x-file-slice-number", num_str),
                                   MAKE_HEADER_CS("x-file-recover", is_cover_str)
                                  };
    header_count = sizeof(header) / sizeof(header[0]);
#endif
    h2_data.header = (http2_header *)header;
    h2_data.header_count = header_count;
    h2_data.data = NULL;
    h2_data.len = 0;
    h2_data.flag = 0;
    h2_data.stream_id = 0;
    rv = iotx_http2_client_send((void *)connection, &h2_data);
    return rv;
}

static int submit_file_auth_req(http2_connection_t *connection, int is_recover, int type)
{
    char client_id[64+1] = {0};
    char sign_str[256+1] = {0};
    char sign[41 + 1] = {0};
    char is_cover_str[2] = {0};
    char path_str[100] = {0};
    char type_str[10 + 1] = {0};
    char *logstr = "log";
    char *filestr = "file";
    int header_count = 0;
    int rv = 0;
    http2_data h2_data;

    memset(&h2_data, 0, sizeof(http2_data));
    if(type == 1) { /* log */
        strncpy(type_str, logstr, strlen(logstr));
    } else {
        strncpy(type_str, filestr, strlen(filestr));
    }
    file_upload_gen_string(client_id, CID_STRING_ENUM, NULL, 0);
    file_upload_gen_string(sign_str, ORI_SIGN_STR_ENUM, client_id, 0);
    file_upload_gen_string(sign, REAL_SIGN_STR_ENUM, sign_str, 0);
    file_upload_gen_string(path_str, PATH_CREATE_STR_ENUM, type_str, 0);

    if(is_recover == 0) {
      sprintf(is_cover_str, "%d", 0);
    } else {
      sprintf(is_cover_str, "%d", 1);
    }
#ifdef IOTX_H2_SUPPORT
    const http2_header header[] = { MAKE_HEADER(":method", "GET"),
                                    MAKE_HEADER_CS(":path", path_str),
                                    MAKE_HEADER(":scheme", "https"),
                                    MAKE_HEADER("x-auth-name", "devicename"),
                                    MAKE_HEADER_CS("x-auth-param-product-key", g_device_info.product_key),
                                    MAKE_HEADER_CS("x-auth-param-device-name", g_device_info.device_name),
                                    MAKE_HEADER_CS("x-auth-param-client-id", client_id),
                                    MAKE_HEADER_CS("x-auth-param-sign", sign),
                                    MAKE_HEADER_CS("x-file-recover", is_cover_str)
                                  };
    header_count = sizeof(header) / sizeof(header[0]);
#else
    const http2_header header[] = { MAKE_HEADER(":method", "GET"),
                                    MAKE_HEADER_CS(":path", path_str),
                                    MAKE_HEADER("x-auth-name", "devicename"),
                                    MAKE_HEADER_CS("x-auth-param-product-key", g_device_info.product_key),
                                    MAKE_HEADER_CS("x-auth-param-device-name", g_device_info.device_name),
                                    MAKE_HEADER_CS("x-auth-param-client-id", client_id),
                                    MAKE_HEADER_CS("x-auth-param-sign", sign),
                                    MAKE_HEADER_CS("x-file-recover", is_cover_str)
                                  };
    header_count = sizeof(header) / sizeof(header[0]);
#endif
    h2_data.header = (http2_header *)header;
    h2_data.header_count = header_count;
    h2_data.data = NULL;
    h2_data.len = 0;
    h2_data.flag = 0;
    h2_data.stream_id = 0;
    rv = iotx_http2_client_send((void *)connection, &h2_data);
    return rv;
}


static int http2_nv_copy(http2_header *nva, int start, http2_header *nva_copy, int end)
{
    int i, j;
    for(i = start, j = 0; j < end; i++, j++) {
        nva[i].name = nva_copy[j].name;
        nva[i].value = nva_copy[j].value;
        nva[i].namelen = nva_copy[j].namelen;
        nva[i].valuelen = nva_copy[j].valuelen;
    }
    return i;
}


static int submit_file_data_req(void *conn, char *file_id, char *store_id, file_data_info *file_data)
{
    char path_str[100] = {0};
    http2_connection_t *connection = (http2_connection_t *)conn;
    char num_str[5+1] = {0};
    char len_str[10+1] = {0};
    int windows_size = 0;
    char client_id[64+1] = {0};
    char sign_str[256+1] = {0};
    char sign[41 + 1] = {0};
    http2_header nva[15] = {{0}};
    int nv_len = 0;
    int curr_slice_num = file_data->curr_slice_num;
    char *data = file_data->data;
    int data_len = file_data->data_len;
    int len = file_data->total_len;
    int stream_id = file_data->stream_id;
    int last_frame = file_data->is_last_frame;
    int need_auth = file_data->need_auth;
    int type = file_data->type;
    char *logstr = "log";
    char *filestr = "file";
    char type_str[10 + 1] = {0};
    http2_data h2_data;
    int rv = 0;
    int count = 0;

    connection->file_id = file_id;
    connection->store_id = store_id;
    memset(&h2_data, 0, sizeof(http2_data));
    if(type == 1) { /* log */
        strncpy(type_str, logstr, strlen(logstr));
    } else {
        strncpy(type_str, filestr, strlen(filestr));
    }
    if(need_auth == 1) {
        file_upload_gen_string(client_id, CID_STRING_ENUM, NULL, 0);
        file_upload_gen_string(sign_str, ORI_SIGN_STR_ENUM, client_id, 0);
        file_upload_gen_string(sign, REAL_SIGN_STR_ENUM, sign_str, 0);
    }
    file_upload_gen_string(num_str, NUM_STRING_ENUM, NULL, curr_slice_num);
    file_upload_gen_string(len_str, NUM_STRING_ENUM, NULL, len);
    file_upload_gen_string(path_str, PATH_UPLOAD_STR_ENUM, type_str, 0);
#ifdef IOTX_H2_SUPPORT
    const http2_header nva1[] = {MAKE_HEADER(":method", "POST"),
                                 MAKE_HEADER_CS(":path", path_str),
                                 MAKE_HEADER(":scheme", "https"),
                                 MAKE_HEADER_CS("x-file-upload-id", file_id),
                                 MAKE_HEADER_CS("x-file-current-slice-number", num_str),
                                 MAKE_HEADER_CS("content-length", len_str)
                              };
#else
    const http2_header nva1[] = {MAKE_HEADER(":method", "POST"),
                                 MAKE_HEADER_CS(":path", path_str),
                                 MAKE_HEADER_CS("x-file-upload-id", file_id),
                                 MAKE_HEADER_CS("x-file-current-slice-number", num_str),
                                 MAKE_HEADER_CS("content-length", len_str)
                                };
#endif
    const http2_header nva2[] = {MAKE_HEADER("x-auth-name", "devicename"),
                                 MAKE_HEADER_CS("x-auth-param-product-key", g_device_info.product_key),
                                 MAKE_HEADER_CS("x-auth-param-device-name", g_device_info.device_name),
                                 MAKE_HEADER_CS("x-auth-param-client-id", client_id),
                                 MAKE_HEADER_CS("x-auth-param-sign", sign),
                                };
    if(need_auth == 1) {
        nv_len = http2_nv_copy(nva, 0, (http2_header *)nva1, sizeof(nva1) / sizeof(nva1[0]));
        nv_len = http2_nv_copy(nva, nv_len, (http2_header *)nva2, sizeof(nva2) / sizeof(nva2[0]));
    } else {
        nv_len = http2_nv_copy(nva, 0, (http2_header *)nva1, sizeof(nva1) / sizeof(nva1[0]));
    }

    windows_size = iotx_http2_get_available_window_size(conn);
    while(windows_size < data_len && count < 100) { /* wait 10s */
        rv = iotx_http2_update_window_size(conn);
        if(rv < 0) {
            return -1;
        }
        windows_size = iotx_http2_get_available_window_size(conn);
        count++;
    }
    /*upload to server*/
    h2_data.data = data;
    h2_data.len = data_len;
    connection->flag = HTTP2_FLAG_NONE;
    if (stream_id == 0) {
        if (last_frame == 1) {
            connection->flag = HTTP2_FLAG_END_STREAM;
        }
        h2_data.header = nva;
        h2_data.header_count = nv_len;
        rv = iotx_http2_client_send((void *)connection, &h2_data);
        if(rv < 0) {
            return -1;
        }
        /* stream_id = nghttp2_submit_request(connection->session, NULL, nva, nv_len, &data_prd, NULL); */
    } else {
        uint8_t flags = HTTP2_FLAG_NONE;
        if (last_frame == 1) {
            connection->flag = HTTP2_FLAG_END_STREAM;
            flags = HTTP2_FLAG_END_STREAM;
        }
        h2_data.stream_id = stream_id;
        h2_data.flag = flags;
        rv = iotx_http2_client_send((void *)connection, &h2_data);
        if(rv < 0) {
            return -1;
        }
    }
    stream_id = h2_data.stream_id;
    return stream_id;
}


static int iotx_cloud_file_create(http2_connection_t *connection, file_info *fileinfo, char *file_id, char *store_id)
{
    char status[4] = {0};
    char *succ_num = "200";
    int times = 0;

    connection->statuscode = status;
    connection->file_id = file_id;
    connection->store_id = store_id;
    if(fileinfo->is_recover == 1) {
        submit_file_create_request(connection, fileinfo, NULL);
        memset(connection->file_id, 0, MAX_HTTP2_FILE_ID_LEN);
    } else {
        submit_file_auth_req(connection, fileinfo->is_recover, fileinfo->type);
    }
    while(times < 5000) {
        int rv  = 0;
        char data[100];
        int len = 0;
        rv = iotx_http2_client_recv(connection, data, 100, &len, 100);
        if (rv >= 0) {
            if((strncmp((char *)connection->statuscode, succ_num, strlen(succ_num)) == 0) &&
                (connection->file_id[0] != '\0')) {
                break;
            }
        }
        times = times + 100;
    }
    if(times >= 5000) {
        return -3;
    }
    return 0;
}

static int iotx_data_upload_to_cloud(http2_connection_t *connection, char *file_id, char *store_id, file_data_info *file_data)
{
    int new_stream_id = 0;
    char status[4] = {0};
    char *succ_num = "200";
    int times = 0;

    connection->statuscode = status;
    new_stream_id = submit_file_data_req(connection, file_id, store_id, file_data);
    if(new_stream_id < 0) {
        return -3;
    }
    while ((file_data->is_last_frame == 1) && (times < 8000)) {
        int rv  = 0;
        char data[100];
        int len = 0;
        rv = iotx_http2_client_recv(connection, data, 100, &len, 100);
        if (rv >= 0) {
            if(strncmp((char *)connection->statuscode, succ_num, strlen(succ_num)) == 0) {
                break;
            }
        }
        times = times + 100;
    }

    if((file_data->is_last_frame == 1) && times >= 8000) {
        return -2;
    }
    return new_stream_id;
}

/**
* @brief             Http2 client entry function.
* @return            None.
*/
int iotx_get_file_size(char *file_name)
{
    FILE *fp = NULL;
    int size = 0;
    if((fp = fopen(file_name, "r")) == NULL)
    {
        fsupload_err("The file %s can not be opened.\n", file_name);
        return -1;
    }
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fclose(fp);
    return size;
}

int iotx_get_file_data(char *file_name, char *data, int len, int offset)
{
    FILE *fp = NULL;
    int ret = 0;
    if((fp = fopen(file_name, "r")) == NULL)
    {
        fsupload_err("The file %s can not be opened.\n", file_name);
        return -1;
    }
    ret = fseek(fp, offset, SEEK_SET);
    if(ret != 0) {
        fsupload_err("The file %s can not move offset.\n", file_name);
        return -1;
    }
    ret = fread(data, len, 1, fp);
    fclose(fp);
    return len;
}


int iotx_check_file_exist(char *file_name)
{
    FILE *fp = NULL;
    if((fp = fopen(file_name, "r")) == NULL)
    {
        fsupload_err("The %s doesn't exist.\n", file_name);
        return 0;
    }
    fclose(fp);
    return 1;
}


int iotx_check_file_same(char *file_name, char *file_name_upload)
{
    if(strncmp(file_name, file_name_upload, strlen(file_name_upload)) == 0)
    {
        fsupload_err("The %s file is uploading.\n", file_name);
        return 1;
    }
    return 0;
}

/*#define TEST*/
HTTP2_UPLOAD_FILE_RET_TYPE iotx_upload_file(http2_connection_t *conn, file_sync_info *info)
{
    char *data = NULL;
    int file_size = 0;               /* file data size*/
    int slice_num = 0;               /* slice number*/
    int curr_slice = *(info->current_pos);   /* current slice nunber*/
    int real_length = 0;
    file_info fileinfo = {0};
    int need_auth = 0;
    int ret;

    ret = iotx_upload_file_check_list(info->file_name, info->type);
    if (ret != HTTP2_UPLOAD_FILE_RET_OK) {
        return ret;
    }
    if(curr_slice == 0) {
        curr_slice =  1;
        *(info->current_pos) =  1;
    }
    file_size = iotx_get_file_size(info->file_name);
    if(file_size < 0) {
        return HTTP2_UPLOAD_FILE_NOT_EXIST;
    }
    if(file_size > MAX_HTTP2_PER_DATA_LEN) {
        real_length = MAX_HTTP2_PER_DATA_LEN;
    } else {
        real_length  = file_size;
    }

    data  = LITE_malloc(real_length + 1);
    if(data == NULL) {
        return HTTP2_MEMORY_NOT_ENOUGH;
    }
    slice_num = file_size / MAX_HTTP2_PER_SLICE_LEN;
    if(file_size % MAX_HTTP2_PER_SLICE_LEN > 0)
    {
        slice_num = slice_num + 1;
    }
    fileinfo.filename = info->file_name;
    fileinfo.file_size = file_size;
    fileinfo.slice_num = slice_num;
    fileinfo.is_recover = 0;
    fileinfo.type = info->type;
    #ifndef TEST
    if(info->file_id[0] == '\0') { /*file id is null, will recover,app need to save file id.*/
        int result = 0;
        fileinfo.is_recover = 1;
        info->store_id[0] = '\0';
        /*file_id need to save.*/
        result = iotx_cloud_file_create(conn, &fileinfo, info->file_id, info->store_id);
        if(result < 0)
        {
            *(info->current_pos) = 1;
             fsupload_err("Creat cloud file fail,result = %d\n", result);
             LITE_free(data);
             return HTTP2_UPLOAD_FILE_CREATE_FAIL;
        }
    } else {
        need_auth = 1;
    }
    #endif
    /*curr_slice need to save.*/
    while(curr_slice <= slice_num) {
        int stream_id = 0;
        int send_times = 0;
        int send_size = 0;
        int i = 0;

        if (curr_slice < slice_num) {
            send_times = MAX_HTTP2_DATA_SEND_TIMES;
            send_size = MAX_HTTP2_PER_SLICE_LEN;
        } else { /*curr_slice == slice_num :last slice*/
            send_size = file_size - (curr_slice - 1) * MAX_HTTP2_PER_SLICE_LEN;
            send_times = send_size / MAX_HTTP2_PER_DATA_LEN;
            if (send_size % MAX_HTTP2_PER_DATA_LEN > 0) {
                send_times = send_times + 1;
            }
        }
        for(i = 1; i <= send_times; i++) {
            file_data_info file_data;
            int last_frame = 0;
            int offset = (curr_slice - 1)  * MAX_HTTP2_PER_SLICE_LEN + (i - 1) * MAX_HTTP2_PER_DATA_LEN;
            if (curr_slice == slice_num && send_times == i) { /*last slice & last packet*/
                real_length = file_size - offset;
            }
            iotx_get_file_data(info->file_name, data, real_length, offset);
            data[real_length] = '\0';
            if (send_times == i) { /*last packet in every stream*/
                last_frame = 1;
            }
            file_data.curr_slice_num = curr_slice;
            file_data.data = data;
            file_data.data_len = real_length;
            file_data.total_len = send_size;
            file_data.type = info->type;
            file_data.stream_id = stream_id;
            file_data.is_last_frame = last_frame;
            file_data.need_auth = need_auth;
        #ifndef TEST
            stream_id = iotx_data_upload_to_cloud(conn, info->file_id, info->store_id, &file_data);
            need_auth = 0;
            if(stream_id <= 0)
            {
                *(info->current_pos) = curr_slice;
                 fsupload_err("upload data to cloud fail,stream_id = %d\n", stream_id);
                 LITE_free(data);
                 return HTTP2_UPLOAD_FILE_UPLOAD_FAIL;
            }
        #endif
        }
        curr_slice++;
    }
    LITE_free(data);
    return HTTP2_UPLOAD_FILE_RET_OK;
}

static file_upload_list *iotx_get_file_info(char *file_name,
                                int type,
                                iotx_upload_file_callback callback,
                                void *user_data)
{
    file_upload_list *temp = LITE_calloc(1, sizeof(file_upload_list));
    if(temp == NULL) {
        return NULL;
    }
    temp->next = NULL;
    temp->name = file_name;
    temp->type = type;
    temp->callback = callback;
    temp->user_data = user_data;
    return temp;
}

static int iotx_add_file_into_list(char *file_name,
                                int type,
                                iotx_upload_file_callback callback,
                                void *user_data)
{
    file_upload_list *p = g_file_list;
    file_upload_list *temp = NULL;
    if(p == NULL) {
        temp = iotx_get_file_info(file_name, type, callback, user_data);
        if(temp == NULL) {
            LITE_free(file_name);
            return HTTP2_MEMORY_NOT_ENOUGH;
        }
        g_file_list = temp;
    } else {
        file_upload_list *pre = p;
        while(p != NULL) {
            if(iotx_check_file_same(file_name, p->name) == 1) {
                LITE_free(file_name);
                return HTTP2_UPLOAD_FILE_IS_UPLOADING;
            }
            pre = p;
            p = p->next;
        }
        temp = iotx_get_file_info(file_name, type, callback, user_data);
        if(temp == NULL) {
            LITE_free(file_name);
            return HTTP2_MEMORY_NOT_ENOUGH;
        }
        pre->next = temp;
    }
    return 0;
}

static void iotx_chagne_file_to_upload()
{
    file_upload_list *p = g_file_list;
    int i;
    file_upload_info *file_upload_info = NULL;

    g_file_list = p->next;
    for(i=0; i < MAX_HTTP2_SUPPORT_FILE_UPLOAD_NUM; i++) {
        file_upload_info = &(g_file_upload_ptr->file_upload[i]);
        if(file_upload_info->is_used == 0) {
            break;
        }
    }
    file_upload_info->filename = p->name;
    file_upload_info->type = p->type;
    file_upload_info->callback = p->callback;
    file_upload_info->user_data = p->user_data;
    file_upload_info->is_used = 1;
    if(p != NULL)
        LITE_free(p);
    g_file_upload_ptr->num = g_file_upload_ptr->num + 1;
}


static void release_upload_file_list()
{
    file_upload_list *p = g_file_list;
    file_upload_list *q = NULL;
    int i;
    file_upload_info *file_upload_info = NULL;

    for(i=0; i < MAX_HTTP2_SUPPORT_FILE_UPLOAD_NUM; i++) {
        file_upload_info = &(g_file_upload_ptr->file_upload[i]);
        if(file_upload_info->is_used == 1) {
            LITE_free(file_upload_info->filename);
        }
    }

    while(p != NULL) {
        q = p->next;
        if(p->name != NULL)
            LITE_free(p->name);
        LITE_free(p);
        p = q;
    }
}


static HTTP2_UPLOAD_FILE_RET_TYPE iotx_upload_file_check_list(char *file_name, int type)
{
    if(iotx_check_file_exist(file_name) == 0) {
        return HTTP2_UPLOAD_FILE_NOT_EXIST;
    }
    if(type < 0 || type > 1) {
        return HTTP2_UPLOAD_FILE_PARA_ERROR;
    }
    return HTTP2_UPLOAD_FILE_RET_OK;
}

HTTP2_UPLOAD_FILE_RET_TYPE iotx_upload_file_async(char *file_name,
                                int type,
                                iotx_upload_file_callback callback,
                                void *user_data)
{
    char *filename = NULL;
    int ret = iotx_upload_file_check_list(file_name, type);
    if(ret != HTTP2_UPLOAD_FILE_RET_OK) {
        return ret;
    }
    if (g_file_upload_ptr->lock_thread  == NULL) {
        return HTTP2_HTTP2_SERIVCE_NOT_READY;
    }
    filename = LITE_calloc(1, (strlen(file_name) + 1));
    if(filename == NULL) {
        return HTTP2_MEMORY_NOT_ENOUGH;
    }
    strcpy(filename, file_name);
    if(g_file_upload_ptr->num == MAX_HTTP2_SUPPORT_FILE_UPLOAD_NUM) {
        int ret = 0;
        HAL_MutexLock(g_file_upload_ptr->lock_thread);
        ret = iotx_add_file_into_list(filename, type, callback, user_data);
        HAL_MutexUnlock(g_file_upload_ptr->lock_thread);
        if(ret < 0) {
            return ret;
        }
    } else {
        int i;
        file_upload_info *file_upload_info = NULL;

        for(i=0; i < MAX_HTTP2_SUPPORT_FILE_UPLOAD_NUM; i++) {
            file_upload_info = &(g_file_upload_ptr->file_upload[i]);
            if(file_upload_info->is_used == 0) {
                break;
            } else {
                if(iotx_check_file_same(filename, file_upload_info->filename) == 1) {
                    return HTTP2_UPLOAD_FILE_IS_UPLOADING;
                }
            }
        }
        file_upload_info->filename = filename;
        file_upload_info->type = type;
        file_upload_info->callback = callback;
        file_upload_info->user_data = user_data;
        file_upload_info->is_used = 1;
        g_file_upload_ptr->num = g_file_upload_ptr->num + 1;
    }
    return HTTP2_UPLOAD_FILE_RET_OK;
}


HTTP2_UPLOAD_FILE_RET_TYPE iotx_http2_upload_file(file_upload_info *upload_info)
{
    char *data = NULL;
    int curr_slice;   /* current slice nunber*/
    int need_auth = upload_info->need_auth;
    http2_connection_t *conn = NULL;
    int real_length = 0;
    file_info fileinfo = {0};

    if (upload_info->file_size == 0) {
        upload_info->file_size = iotx_get_file_size(upload_info->filename);
        if(upload_info->file_size < 0) {
            return HTTP2_UPLOAD_FILE_NOT_EXIST;
        }
    }

    if (upload_info->curr_slice_num == 0) {
        upload_info->curr_slice_num =  1;
    }

    if (upload_info->file_size > MAX_HTTP2_PER_DATA_LEN) {
        real_length = MAX_HTTP2_PER_DATA_LEN;
    } else {
        real_length  = upload_info->file_size;
    }

    data  = LITE_malloc(real_length + 1);
    if(data == NULL) {
        return HTTP2_MEMORY_NOT_ENOUGH;
    }
    upload_info->slice_num = upload_info->file_size / MAX_HTTP2_PER_SLICE_LEN;
    if (upload_info->file_size % MAX_HTTP2_PER_SLICE_LEN > 0) {
        upload_info->slice_num = upload_info->slice_num + 1;
    }
    fileinfo.filename = upload_info->filename;
    fileinfo.file_size = upload_info->file_size;
    fileinfo.slice_num = upload_info->slice_num;
    fileinfo.type = upload_info->type;
    fileinfo.is_recover = 0;
    conn = iotx_get_http2_conn();

    if(upload_info->file_id[0] == '\0') { /*file id is null, will recover,app need to save file id.*/
        int result = 0;
        fileinfo.is_recover = 1;
        /*file_id need to save.*/
        result = iotx_cloud_file_create(conn, &fileinfo, upload_info->file_id, upload_info->store_id);
        if(result < 0)
        {
             upload_info->curr_slice_num = 1;
             fsupload_err("Creat cloud file fail,result = %d\n", result);
             LITE_free(data);
             return HTTP2_UPLOAD_FILE_CREATE_FAIL;
        }
    } else {
        need_auth = 1;
    }
    curr_slice = upload_info->curr_slice_num;
    /*curr_slice need to save.*/
    if(curr_slice <= upload_info->slice_num) {
        int stream_id = 0;
        int send_times = 0;
        int send_size = 0;
        int i = 0;

        if (curr_slice < upload_info->slice_num) {
            send_times = MAX_HTTP2_DATA_SEND_TIMES;
            send_size = MAX_HTTP2_PER_SLICE_LEN;
        } else { /*curr_slice == slice_num :last slice*/
            send_size = upload_info->file_size - (curr_slice - 1) * MAX_HTTP2_PER_SLICE_LEN;
            send_times = send_size / MAX_HTTP2_PER_DATA_LEN;
            if (send_size % MAX_HTTP2_PER_DATA_LEN > 0) {
                send_times = send_times + 1;
            }
        }
        for(i = 1; i <= send_times; i++) {
            file_data_info file_data;
            int last_frame = 0;
            int offset = (curr_slice - 1)  * MAX_HTTP2_PER_SLICE_LEN + (i - 1) * MAX_HTTP2_PER_DATA_LEN;
            int res = 0;
            if (curr_slice == upload_info->slice_num && send_times == i) { /* last slice & last packet */
                real_length = upload_info->file_size - offset;
            }
            res = iotx_get_file_data(upload_info->filename, data, real_length, offset);
            if(res < 0) {
                return HTTP2_UPLOAD_FILE_READ_FAIL;
            }
            data[real_length] = '\0';
            if (send_times == i) { /*last packet in every stream*/
                last_frame = 1;
            }
            file_data.curr_slice_num = curr_slice;
            file_data.data = data;
            file_data.data_len = real_length;
            file_data.total_len = send_size;
            file_data.stream_id = stream_id;
            file_data.is_last_frame = last_frame;
            file_data.need_auth = need_auth;
            file_data.type = upload_info->type;
            stream_id = iotx_data_upload_to_cloud(conn, upload_info->file_id, upload_info->store_id, &file_data);
            fsupload_info("delay_time: %d\n", g_file_upload_ptr->delay_time);
            if (g_file_upload_ptr->delay_time > 0) {
                HAL_SleepMs((g_file_upload_ptr->delay_time)*1000);
            }
            need_auth = 0;
            if(stream_id <= 0)
            {
                 upload_info->curr_slice_num = curr_slice;
                 fsupload_err("upload data to cloud fail, ret = %d\n", stream_id);
                 LITE_free(data);
                 return HTTP2_UPLOAD_FILE_UPLOAD_FAIL;
            }
        }
        curr_slice++;
        upload_info->curr_slice_num = curr_slice;
    }
    LITE_free(data);
    return HTTP2_UPLOAD_FILE_RET_OK;
}


static void process_upload_info(file_upload_info *upload_info)
{
    int ret = 0;

    if(upload_info->curr_slice_num > upload_info->slice_num) {
        /*upload success*/
        if(upload_info->callback != NULL) {
            upload_info->callback(HTTP2_UPLOAD_FILE_RET_OK,  upload_info->store_id, upload_info->user_data);
        }
        LITE_free(upload_info->filename);
        memset(upload_info, 0, sizeof(file_upload_info));
        g_file_upload_ptr->num = g_file_upload_ptr->num - 1;
        if(g_file_list != NULL) {
            /* add into uploading */
            iotx_chagne_file_to_upload();
        }
    } else {
        ret = iotx_http2_upload_file(upload_info);
        if(ret < 0) {
            if(upload_info->retry_count == MAX_HTTP2_MAX_RETRANS_TIMES ||
                ret <= HTTP2_UPLOAD_FILE_NOT_EXIST) {
                fsupload_err("upload file failed, %d\n", ret);
                if(upload_info->callback != NULL) {
                    upload_info->callback(ret, NULL, upload_info->user_data);
                }
                LITE_free(upload_info->filename);
                memset(upload_info, 0, sizeof(file_upload_info));
                g_file_upload_ptr->num = g_file_upload_ptr->num - 1;
                if(g_file_list != NULL) {
                    /* add into uploading */
                    iotx_chagne_file_to_upload();
                }
            } else {
                upload_info->retry_count = upload_info->retry_count + 1;
            }
        }
    }
}


int iotx_http2_keep_alive()
{
    int time = g_file_upload_ptr->keep_alive;
    http2_connection_t *conn = iotx_get_http2_conn();

    if(time <= 10) {
        int rv = 0;
        rv = iotx_http2_client_send_ping(conn);
        g_file_upload_ptr->keep_alive  = KEEP_ALIVE_TIMES;
        if(rv < 0) {
            return rv;
        }
    } else {
        g_file_upload_ptr->keep_alive = time - MAX_HTTP2_INTERVAL_TIMES;
    }
    return 0;
}


int iotx_http2_set_bitrate(int bitrate)
{
    if(bitrate > 0) {
        /* one block is 4096 byte*/
        g_file_upload_ptr->delay_time = (64 * 8 * 8) / bitrate;
        if (g_file_upload_ptr->delay_time > 50) {
            /*need to keep alive with cloud*/
            g_file_upload_ptr->delay_time = 50;
        }
        return 0;
    }
    return -1;
}

int iotx_http2_set_device_info(char *pk, char *dn, char *ds)
{
    memcpy(g_device_info.product_key, pk, PRODUCT_KEY_LEN);
    memcpy(g_device_info.device_name, dn, DEVICE_NAME_LEN);
    memcpy(g_device_info.device_secret, ds, DEVICE_SECRET_LEN);
    return 0;
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

void iotx_http2_upload_file_init(device_conn_info *conn_info)
{
    httpclient_t client = {0};
    http2_connection_t *conn = NULL;
    int is_http2_connection = 0;
    int count = 0;
    char buf[100] = {0};
    int port = 0;

    memset(&g_file_upload, 0, sizeof(file_upload_contex));
    memset(&client, 0, sizeof(httpclient_t));
    memset(&g_device_info, 0, sizeof(device_info));
    g_file_upload_ptr->lock_thread = HAL_MutexCreate();
    if (g_file_upload_ptr->lock_thread  == NULL) {
        fsupload_err("lock create failed.\n");
        return;
    }
    if(conn_info->product_key == NULL ||
       conn_info->device_name == NULL ||
       conn_info->device_secret == NULL) {
        fsupload_err("device parameter is error.\n");
        return;
    }
    if(conn_info->url == NULL || conn_info->port == 0)
        port = iotx_http2_get_url(buf, conn_info->product_key);
    iotx_http2_set_device_info(conn_info->product_key, conn_info->device_name, conn_info->device_secret);
    while(1) {
        if(g_file_upload_ptr->num > 0) {
            int i;
            file_upload_info *upload_info = NULL;
            if(is_http2_connection == 0) {
                if(conn_info->url == NULL || conn_info->port == 0) {
                    conn = iotx_http2_client_connect((void *)&client, buf, port);
                } else {
                    conn = iotx_http2_client_connect((void *)&client, conn_info->url, conn_info->port);
                }
                if(conn == NULL) {
                    if(count < MAX_HTTP2_MAX_RETRANS_TIMES) {
                        count++;
                        continue;
                    }
                    else{
                        break;
                    }
                }
                is_http2_connection = 1;
                conn->status = 1;
                g_http2_conn = conn;
            }
            HAL_MutexLock(g_file_upload_ptr->lock_thread);
            for(i = 0; i < MAX_HTTP2_SUPPORT_FILE_UPLOAD_NUM; i++) {
                upload_info = &(g_file_upload_ptr->file_upload[i]);
                if(upload_info->is_used == 1 && conn->status != 0) {
                    process_upload_info(upload_info);
                }
            }
            HAL_MutexUnlock(g_file_upload_ptr->lock_thread);
        }else {
            HAL_SleepMs(MAX_HTTP2_INTERVAL_TIMES * 1000);
            if (is_http2_connection == 1) {
                int res;
                res = iotx_http2_keep_alive();
                if (res < 0) {
                    conn->status = 0;
                }
            }
        }
        if(conn != NULL && conn->status == 0) {
            iotx_http2_client_disconnect(conn);
            fsupload_err("network disconnect, try it later\n");
            g_http2_conn = NULL;
            is_http2_connection = 0;
        }
    }
    HAL_MutexLock(g_file_upload_ptr->lock_thread);
    release_upload_file_list();
    HAL_MutexUnlock(g_file_upload_ptr->lock_thread);
    fsupload_err("can't connect cloud server, exit\n");
    HAL_MutexDestroy(g_file_upload_ptr->lock_thread);
}
