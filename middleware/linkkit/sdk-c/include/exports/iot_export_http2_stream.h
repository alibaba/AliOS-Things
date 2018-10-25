/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef IOT_EXPORT_HTTP2_STREAM_H
#define IOT_EXPORT_HTTP2_STREAM_H
#include "iot_export_http2.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_HTTP2_HEADER_NUM                    (16)
#define EXT_HTTP2_HEADER_NUM                    (5)
#define IOT_HTTP2_RES_OVERTIME_MS               (10000)


#include "iotx_log.h"

#define h2stream_err(...)      log_err("h2stream", __VA_ARGS__)
#define h2stream_warning(...)  log_warning("h2stream", __VA_ARGS__)
#define h2stream_info(...)     log_info("h2stream", __VA_ARGS__)
#define h2stream_debug(...)    log_debug("h2stream", __VA_ARGS__)

#define MAKE_HEADER(NAME, VALUE)                                             \
    {                                                                        \
        (char *) NAME, (char *)VALUE, sizeof(NAME) - 1, sizeof(VALUE) - 1    \
    }

#define MAKE_HEADER_CS(NAME, VALUE)                                          \
    {                                                                        \
        (char *) NAME, (char *)VALUE, strlen(NAME) , strlen(VALUE)           \
    }

typedef struct {
    char  *product_key;
    char  *device_name;
    char  *device_secret;
    char  *url;
    int   port;
} device_conn_info_t;

typedef struct {
    http2_header      nva[EXT_HTTP2_HEADER_NUM];
    int               num;
} header_ext_info_t;

typedef enum {
    STREAM_TYPE_DOWNLOAD,
    STREAM_TYPE_UPLOAD,
    STREAM_TYPE_AUXILIARY,
    STREAM_TYPE_NUM
} stream_type_t;

typedef struct {
    char               *stream;             /* point to stream data buffer */
    uint32_t            stream_len;         /* file content length */
    uint32_t            send_len;           /* data had sent length */
    uint32_t            packet_len;         /* one packet length */
    const char          *identify;          /* path string to identify a stream service */
    int                 h2_stream_id;       /* stream identifier which is a field in HTTP2 frame */
    char                *channel_id;        /* string return by server to identify a specific stream channel, different from stream identifier which is a field in HTTP2 frame */
} stream_data_info_t;

typedef void (*on_stream_header_callback)(uint32_t stream_id, char *channel_id, int cat, const uint8_t *name, size_t namelen,
        const uint8_t *value, size_t valuelen, uint8_t flags);

typedef void (*on_stream_chunk_recv_callback)(uint32_t stream_id, char *channel_id,
        const uint8_t *data, size_t len, uint8_t flags);

typedef void (*on_stream_close_callback)(uint32_t stream_id, char *channel_id, uint32_t error_code);

typedef void (*on_stream_frame_send_callback)(uint32_t stream_id, char *channel_id, int type, uint8_t flags);

typedef void (*on_stream_frame_recv_callback)(uint32_t stream_id, char *channel_id, int type, uint8_t flags);

typedef struct {
    on_stream_header_callback       on_stream_header_cb;
    on_stream_chunk_recv_callback   on_stream_chunk_recv_cb;
    on_stream_close_callback        on_stream_close_cb;
    on_stream_frame_send_callback   on_stream_frame_send_cb;
    on_stream_frame_recv_callback   on_stream_frame_recv_cb;
} http2_stream_cb_t;

typedef struct {
    http2_connection_t   *http2_connect;
    void                 *mutex;
    void                 *semaphore;
    void                 *thread_handle;
    http2_list_t         stream_list;
    int                  init_state;
    http2_stream_cb_t    *cbs;
} stream_handle_t;

typedef struct {
    unsigned int stream_id;         /* http2 protocol stream id */
    char *channel_id;               /* string return by server to identify a specific stream channel, different from stream identifier which is a field in http2 frame */
    stream_type_t stream_type;      /* check @stream_type_t */
    void *semaphore;                /* semaphore for http2 response sync */
    char status_code[4];            /* http2 response status code */
    http2_list_t list;              /* list_head */
} http2_stream_node_t;

#ifdef FS_ENABLED
typedef enum {
    UPLOAD_FILE_NOT_EXIST     = -9,
    UPLOAD_FILE_READ_FAILED   = -8,
    UPLOAD_STREAM_OPEN_FAILED = -7,
    UPLOAD_STREAM_SEND_FAILED = -6,
    UPLOAD_MALLOC_FAILED      = -5,
    UPLOAD_NULL_POINT         = -2,
    UPLOAD_ERROR_COMMON       = -1,
    UPLOAD_SUCCESS            = 0,
} http2_file_upload_result_t;

typedef void (* upload_file_result_cb)(const char *path, int result, void *user_data);
DLL_IOT_API int IOT_HTTP2_Stream_UploadFile(stream_handle_t *handle, const char *file_path, const char *identify,
        header_ext_info_t *header,
        upload_file_result_cb cb, void *user_data);
#endif
stream_handle_t *IOT_HTTP2_Stream_Connect(device_conn_info_t *conn_info, http2_stream_cb_t *user_cb);
DLL_IOT_API int IOT_HTTP2_Stream_Open(stream_handle_t *handle, stream_data_info_t *info, header_ext_info_t *header);
DLL_IOT_API int IOT_HTTP2_Stream_Send(stream_handle_t *handle, stream_data_info_t *info, header_ext_info_t *header);
DLL_IOT_API int IOT_HTTP2_Stream_Query(stream_handle_t *handle, stream_data_info_t *info, header_ext_info_t *header);
DLL_IOT_API int IOT_HTTP2_Stream_Close(stream_handle_t *handle, stream_data_info_t *info);
DLL_IOT_API int IOT_HTTP2_Stream_Disconnect(stream_handle_t *handle);

#ifdef __cplusplus
}
#endif

#endif /* IOT_EXPORT_FILE_UPLOADER_H */
