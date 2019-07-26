/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */


#ifdef HTTP2_COMM_ENABLED

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "sdk-testsuites_internal.h"
#include "iot_export.h"
#include "iot_export_http2_stream.h"
#include "cut.h"


#define HTTP2_STREAM_URL                "10.101.12.205"
#define HTTP2_STREAM_PORT               9999
#define HTTP2_STREAM_PK                 "a1L5EUOh21s"
#define HTTP2_STREAM_DN                 "yinghao_test01"
#define HTTP2_STREAM_DS                 "XZSVWrdoTCCLyJ8o8IaDN0tQd55Ik42D"

#define HTTP2_STREAM_DN_ERROR           "device_name_error"
#define HTTP2_STREAM_URL_ERROR          "10.101.12.1"

static http2_header header[] = {
    MAKE_HEADER("test_name", "test_http2_header"),
    MAKE_HEADER_CS("x-for-test", " hello world"),
};


static header_ext_info_t my_header_info = {
    header,
    2
};


static void stream_download_recv_cb(uint32_t stream_id, char *channel_id, const uint8_t *data, uint32_t len,
                                    uint8_t flags,void *user_data)
{
    printf("stream download recv %d bytes: %s\n", len, data);
}



DATA(HTTP2_STREAM)
{
    void *connection_ctx;
    device_conn_info_t connection_info;
    stream_data_info_t stream_info;
    http2_stream_cb_t stream_cb;

};

SETUP(HTTP2_STREAM)
{
    memset(&data->connection_info, 0, sizeof(device_conn_info_t));
    memset(&data->stream_info, 0, sizeof(stream_data_info_t));

    data->connection_info.product_key = HTTP2_STREAM_PK;
    data->connection_info.device_name = HTTP2_STREAM_DN;
    data->connection_info.device_secret = HTTP2_STREAM_DS;
    data->connection_info.url = HTTP2_STREAM_URL;
    data->connection_info.port = HTTP2_STREAM_PORT;

    data->connection_ctx = NULL;
    data->stream_info.identify = "iotx/vision/voice/intercom/live";

    data->stream_cb.on_stream_header_cb = NULL;
    data->stream_cb.on_stream_chunk_recv_cb = stream_download_recv_cb;
    data->stream_cb.on_stream_close_cb = NULL;
    data->stream_cb.on_stream_frame_send_cb = NULL;
    data->stream_cb.on_stream_frame_recv_cb = NULL;
}

TEARDOWN(HTTP2_STREAM)
{
    IOT_HTTP2_Disconnect(data->connection_ctx);
}

/*
CASEs(HTTP2_STREAM, case_02_connection_multi_connect)
{
    stream_handle_t *ctx_temp;

    data->connection_ctx = IOT_HTTP2_Connect(&(data->connection_info), &(data->stream_cb));
    ASSERT_NE(data->connection_ctx, NULL);

    ctx_temp = IOT_HTTP2_Connect(&(data->connection_info), &(data->stream_cb));
    ASSERT_EQ(ctx_temp, NULL);
}
*/

CASEs(HTTP2_STREAM, case_01_open_error_device_info)
{
    int ret = 0;

    data->connection_info.device_name = HTTP2_STREAM_DN_ERROR;

    data->connection_ctx = IOT_HTTP2_Connect(&(data->connection_info), &(data->stream_cb));
    ASSERT_NE(data->connection_ctx, NULL);

    ret = IOT_HTTP2_Stream_Open(data->connection_ctx, &(data->stream_info), &my_header_info);
    if (ret == 0) {
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(ret, FAIL_RETURN);
}


CASEs(HTTP2_STREAM, case_02_open_error_request_path)
{
    int ret = 0;

    data->connection_ctx = IOT_HTTP2_Connect(&(data->connection_info), &(data->stream_cb));
    ASSERT_NE(data->connection_ctx, NULL);

    data->stream_info.identify = "iotx/error/path/test";

    ret = IOT_HTTP2_Stream_Open(data->connection_ctx, &(data->stream_info), &my_header_info);
    if (ret == 0) {
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(ret, FAIL_RETURN);
}


CASEs(HTTP2_STREAM, case_03_multi_request)
{
    int ret = 0;
    stream_data_info_t stream_data_info_send;
    memset(&stream_data_info_send, 0, sizeof(stream_data_info_t));
    stream_data_info_send.stream = "ABCDEFG";
    stream_data_info_send.stream_len = 8;
    stream_data_info_send.packet_len = 8;
    stream_data_info_send.identify = "iotx/vision/voice/intercom/live";

    data->connection_ctx = IOT_HTTP2_Connect(&(data->connection_info), &(data->stream_cb));
    ASSERT_NE(data->connection_ctx, NULL);

    ret = IOT_HTTP2_Stream_Open(data->connection_ctx, &(data->stream_info), &my_header_info);
    if (ret < 0) {
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(ret, SUCCESS_RETURN);

    ret = IOT_HTTP2_Stream_Query(data->connection_ctx, &(data->stream_info), &my_header_info);
    if (ret < 0) {
        IOT_HTTP2_Stream_Close(data->connection_ctx, &(data->stream_info));
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(ret, SUCCESS_RETURN);

    ret = IOT_HTTP2_Stream_Open(data->connection_ctx, &stream_data_info_send, &my_header_info);
    if (ret < 0) {
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(ret, SUCCESS_RETURN);

    ret = IOT_HTTP2_Stream_Send(data->connection_ctx, &stream_data_info_send, &my_header_info);
    if (ret < 0) {
        IOT_HTTP2_Stream_Close(data->connection_ctx, &stream_data_info_send);
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(ret, SUCCESS_RETURN);

    HAL_SleepMs(5000);

    ret = IOT_HTTP2_Stream_Close(data->connection_ctx, &stream_data_info_send);
    if (ret < 0) {
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(ret, SUCCESS_RETURN);

    ret = IOT_HTTP2_Stream_Close(data->connection_ctx, &(data->stream_info));
    if (ret < 0) {
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(ret, SUCCESS_RETURN);
}


CASEs(HTTP2_STREAM, case_04_whole_flow)
{
    int ret = 0;

    data->connection_ctx = IOT_HTTP2_Connect(&(data->connection_info), &(data->stream_cb));
    ASSERT_NE(data->connection_ctx, NULL);

    ret = IOT_HTTP2_Stream_Open(data->connection_ctx, &(data->stream_info), &my_header_info);
    if (ret < 0) {
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(ret, SUCCESS_RETURN);

    data->stream_info.stream = "ABCDEFG";
    data->stream_info.stream_len = 8;
    data->stream_info.packet_len = 8;
    ret = IOT_HTTP2_Stream_Send(data->connection_ctx, &(data->stream_info), &my_header_info);
    if (ret < 0) {
        IOT_HTTP2_Stream_Close(data->connection_ctx, &(data->stream_info));
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(ret, SUCCESS_RETURN);

    ret = IOT_HTTP2_Stream_Close(data->connection_ctx, &(data->stream_info));
    if (ret < 0) {
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(ret, SUCCESS_RETURN);

    ret = IOT_HTTP2_Stream_Open(data->connection_ctx, &(data->stream_info), &my_header_info);
    if (ret < 0) {
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(ret, SUCCESS_RETURN);

    ret = IOT_HTTP2_Stream_Query(data->connection_ctx, &(data->stream_info), &my_header_info);
    if (ret < 0) {
        IOT_HTTP2_Stream_Close(data->connection_ctx, &(data->stream_info));
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(ret, SUCCESS_RETURN);

    HAL_SleepMs(5000);

    ret = IOT_HTTP2_Stream_Close(data->connection_ctx, &(data->stream_info));
    if (ret < 0) {
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(ret, SUCCESS_RETURN);
}

CASEs(HTTP2_STREAM, case_05_multi_packet_send)
{
    int ret = 0;
    data->connection_ctx = IOT_HTTP2_Connect(&(data->connection_info), NULL);
    ASSERT_NE(data->connection_ctx, NULL);

    ret = IOT_HTTP2_Stream_Open(data->connection_ctx, &(data->stream_info), &my_header_info);
    if (ret < 0) {
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(ret, SUCCESS_RETURN);

    data->stream_info.stream = "12345678";
    data->stream_info.stream_len = 8;
    data->stream_info.packet_len = 4;
    while (data->stream_info.send_len < data->stream_info.stream_len) {
        ret = IOT_HTTP2_Stream_Send(data->connection_ctx, &(data->stream_info), &my_header_info);
        if (ret < 0) {
            IOT_HTTP2_Stream_Close(data->connection_ctx, &(data->stream_info));
            IOT_HTTP2_Disconnect(data->connection_ctx);
            data->connection_ctx = NULL;
            ret = -1;
        }
        ASSERT_NE(ret, FAIL_RETURN);
        data->stream_info.stream += data->stream_info.packet_len;
    }

    ret = IOT_HTTP2_Stream_Close(data->connection_ctx, &(data->stream_info));
    if (ret < 0) {
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(ret, SUCCESS_RETURN);

}

CASEs(HTTP2_STREAM, case_06_null_paras)
{
    int ret = 0;
    data->connection_ctx = IOT_HTTP2_Connect(NULL, NULL);
    ASSERT_EQ(data->connection_ctx, NULL);

    data->connection_ctx = IOT_HTTP2_Connect(&(data->connection_info), NULL);
    ASSERT_NE(data->connection_ctx, NULL);

    ret = IOT_HTTP2_Stream_Open(data->connection_ctx, NULL, NULL);
    if (ret == 0) {
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_NE(ret, SUCCESS_RETURN);
}


CASEs(HTTP2_STREAM, case_07_null_data_send)
{
    int ret = 0;

    data->connection_ctx = IOT_HTTP2_Connect(&(data->connection_info), NULL);
    ASSERT_NE(data->connection_ctx, NULL);

    ret = IOT_HTTP2_Stream_Open(data->connection_ctx, &(data->stream_info), &my_header_info);
    if (ret < 0) {
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(ret, SUCCESS_RETURN);

    data->stream_info.stream = NULL;
    data->stream_info.stream_len = 8;
    data->stream_info.packet_len = 8;
    ret = IOT_HTTP2_Stream_Send(data->connection_ctx, &(data->stream_info), &my_header_info);
    if (ret == 0) {
        IOT_HTTP2_Stream_Close(data->connection_ctx, &(data->stream_info));
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_NE(ret, SUCCESS_RETURN);

    ret = IOT_HTTP2_Stream_Close(data->connection_ctx, &(data->stream_info));
    if (ret < 0) {
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(ret, SUCCESS_RETURN);
}
#ifdef FS_ENABLED
static int upload_finish;
static int up_result;
static void upload_file_result(const char *path, int result, void *user_data)
{
    log_info("===========file path = %s,result =%d=========", path, result);
    up_result = result;
}

CASEs(HTTP2_STREAM, case_08_null_file_path)
{
    int ret = 0;

    data->connection_ctx = IOT_HTTP2_Connect(&(data->connection_info), NULL);
    ASSERT_NE(data->connection_ctx, NULL);

    ret = IOT_HTTP2_Stream_Open(data->connection_ctx, &(data->stream_info), &my_header_info);
    if (ret < 0) {
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(ret, SUCCESS_RETURN);

    ret = IOT_HTTP2_Stream_UploadFile(data->connection_ctx, NULL, "iotx/vision/voice/intercom/live", &my_header_info, NULL,
                                      NULL);
    if (ret == 0) {
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_NE(ret, SUCCESS_RETURN);
}

CASEs(HTTP2_STREAM, case_09_file_not_found)
{
    int ret = 0;
    int cnt = 10;

    data->connection_ctx = IOT_HTTP2_Connect(&(data->connection_info), NULL);
    ASSERT_NE(data->connection_ctx, NULL);

    ret = IOT_HTTP2_Stream_Open(data->connection_ctx, &(data->stream_info), &my_header_info);
    if (ret < 0) {
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(ret, SUCCESS_RETURN);
    upload_finish = 0;
    ret = IOT_HTTP2_Stream_UploadFile(data->connection_ctx, "not_exist_file.zip", "iotx/vision/voice/intercom/live",
                                      &my_header_info,
                                      upload_file_result, NULL);
    if (ret < 0) {
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(ret, SUCCESS_RETURN);
    while (!upload_finish && --cnt) {
        HAL_SleepMs(1000);
    }
    if (up_result != UPLOAD_FILE_NOT_EXIST) {
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(up_result, UPLOAD_FILE_NOT_EXIST);
}

CASEs(HTTP2_STREAM, case_10_file_upload)
{
    int ret = 0;
    int cnt = 10;
    void *file;
    const char filename[] = "http2_upload.txt";
    file = HAL_Fopen(filename, "w+");
    ASSERT_NE(file, NULL);

    ret = HAL_Fwrite("test data", 1, sizeof("test data"), file);
    if (ret <= 0) {
        HAL_Fclose(file);
    }
    ASSERT_NE(ret, 0);


    ret = HAL_Fclose(file);

    data->connection_ctx = IOT_HTTP2_Connect(&(data->connection_info), NULL);
    ASSERT_NE(data->connection_ctx, NULL);

    ret = IOT_HTTP2_Stream_Open(data->connection_ctx, &(data->stream_info), &my_header_info);
    if (ret < 0) {
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(ret, SUCCESS_RETURN);
    upload_finish = 0;
    ret = IOT_HTTP2_Stream_UploadFile(data->connection_ctx, filename, "iotx/vision/voice/intercom/live", &my_header_info,
                                      upload_file_result, NULL);
    if (ret < 0) {
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(ret, SUCCESS_RETURN);
    while (!upload_finish && --cnt) {
        HAL_SleepMs(1000);
    }
    if (up_result != SUCCESS_RETURN) {
        IOT_HTTP2_Disconnect(data->connection_ctx);
        data->connection_ctx = NULL;
    }
    ASSERT_EQ(up_result, SUCCESS_RETURN);
}

#endif
SUITE(HTTP2_STREAM) = {
    ADD_CASE(HTTP2_STREAM, case_01_open_error_device_info),
    ADD_CASE(HTTP2_STREAM, case_02_open_error_request_path),
    // ADD_CASE(HTTP2_STREAM, case_03_multi_request),
    ADD_CASE(HTTP2_STREAM, case_04_whole_flow),
    ADD_CASE(HTTP2_STREAM, case_05_multi_packet_send),
    ADD_CASE(HTTP2_STREAM, case_06_null_paras),
    ADD_CASE(HTTP2_STREAM, case_07_null_data_send),
#ifdef FS_ENABLED
    ADD_CASE(HTTP2_STREAM, case_08_null_file_path),
    ADD_CASE(HTTP2_STREAM, case_09_file_not_found),
    ADD_CASE(HTTP2_STREAM, case_10_file_upload),
#endif
    ADD_CASE_NULL
};
#endif
