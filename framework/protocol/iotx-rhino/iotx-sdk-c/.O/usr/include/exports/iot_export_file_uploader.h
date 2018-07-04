/*
 * nghttp2 - HTTP/2 C Library
 *
 * Copyright (c) 2013, 2014 Tatsuhiro Tsujikawa
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef IOT_EXPORT_FILE_UPLOADER_H
#define IOT_EXPORT_FILE_UPLOADER_H
#include "iot_export_http2.h"

#ifdef __cplusplus
extern "C" {
#endif
#define MAX_HTTP2_FILE_ID_LEN           (40 + 1)
#define MAX_HTTP2_MAX_RETRANS_TIMES     (6)
#define MAX_HTTP2_FILE_STORE_LEN        (20 + 1)


typedef struct _file_sync_info_struct_ {
    char *file_name;   /*file name*/
    char *file_id;     /*file id for retransmit it when fail to upload*/
    char *store_id;    /*store id when file upload success*/
    int *current_pos;  /*retransmit pos*/
    int type;          /*file type 0 : file. 1: log*/
}file_sync_info;

typedef struct _device_conn_info_struct_ {
    char  *product_key;
    char  *device_name;
    char  *device_secret;
    char  *url;
    int   port;
}device_conn_info;

typedef enum {
    HTTP2_UPLOAD_FILE_RET_OK            = 0,
    HTTP2_MEMORY_NOT_ENOUGH             = -1,
    HTTP2_UPLOAD_FILE_IS_UPLOADING      = -2,
    HTTP2_UPLOAD_FILE_NOT_EXIST         = -3,
    HTTP2_UPLOAD_FILE_READ_FAIL         = -4,
    HTTP2_UPLOAD_FILE_CREATE_FAIL       = -5,
    HTTP2_UPLOAD_FILE_UPLOAD_FAIL       = -6,
    HTTP2_HTTP2_SERIVCE_NOT_READY       = -7,
    HTTP2_UPLOAD_FILE_PARA_ERROR        = -8,
} HTTP2_UPLOAD_FILE_RET_TYPE;

typedef int (*iotx_upload_file_callback)(int result, char *message, void *user_data);
/**
* @brief          the http2 client upload file init for dbus service.
* @return         NULL.
*/
extern void iotx_http2_upload_file_init(device_conn_info *conn_info);
/**
* @brief          the http2 client upload file set bitrate.
* @param[in]      bitrate: the file upload with the gived bitrate.
* @return         The result. 0 is ok.
*/
extern int iotx_http2_set_bitrate(int bitrate);
/**
* @brief          the http2 client upload file in sync way.
                  just call this api directly after http2 client connect.
                  don't need to call iotx_http2_upload_file_init.
* @param[in]      conn: http2 client connection handler.
* @param[in]      file_name: fien name.
* @param[out]     file_id: file id return from server.
* @param[out]     current_pos: file block when the file current upload.
* @return         The result. 0 is ok.
*/
extern int iotx_upload_file(http2_connection_t *conn, file_sync_info *file_info);
/**
* @brief          the http2 client upload file through async way.
                  It should call iotx_http2_upload_file_init in one thread firstly.
* @param[in]      file_name: fien name.
* @param[in]      type: file type. log or file
* @param[in]      callback: the callback function will be called after upload done or error.
* @param[in]      user_data: the user data when callback function will be called.
* @return         The result. 0 is ok.
*/
extern HTTP2_UPLOAD_FILE_RET_TYPE iotx_upload_file_async(char *file_name,
                                int type,
                                iotx_upload_file_callback callback,
                                void *user_data);
/**
* @brief          the http2 set device info.
* @pk[in]         pk: product key.
* @dn[in]         dn: device name.
* @ds[in]         ds: device secret.
* @return         The result. 0 is ok.
*/
extern int iotx_http2_set_device_info(char *pk, char *dn, char *ds);

#ifdef __cplusplus
}
#endif

#endif /* IOT_EXPORT_FILE_UPLOADER_H */
