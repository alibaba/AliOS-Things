/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "iot_export.h"
#include "iot_export_http2_stream.h"
#include "app_entry.h"
#include "utils_httpc.h"

#define  ON_DAILY
#if defined(ON_DAILY)
	#define HTTP2_ONLINE_SERVER_URL       "10.101.12.205"
	#define HTTP2_ONLINE_SERVER_PORT      9999
	#define HTTP2_PRODUCT_KEY             "a1L5EUOh21s"
	#define HTTP2_DEVICE_NAME             "zhangmei_test01"
	#define HTTP2_DEVICE_SECRET           "KQCftfEDCx35LChyEwZoYY6FCYidTOp0"
#elif defined(ON_PRE)
	#define HTTP2_ONLINE_SERVER_URL       "100.67.141.158/"
	#define HTTP2_ONLINE_SERVER_PORT      8443
	#define HTTP2_PRODUCT_KEY             "b1XVhqfan1X"
	#define HTTP2_DEVICE_NAME             "YvhjziEQmKusCFUgRpeo"
	#define HTTP2_DEVICE_SECRET           "QjkhCrAX0SbNWgKpIamuiDdLkk23Q1r7"
#else
	#define HTTP2_ONLINE_SERVER_URL       NULL
	#define HTTP2_ONLINE_SERVER_PORT      443
	#define HTTP2_PRODUCT_KEY             "DM5b8zbTWJs"
	#define HTTP2_DEVICE_NAME             "mydevice1"
	#define HTTP2_DEVICE_SECRET           "q4tiwQuICYfr6JQ8aUFjWxocuXJ5ruEx"
#endif

//#define FILE_NAME "test.tar.gz2"

#define EXAMPLE_TRACE(fmt, ...)                        \
    do {                                               \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__);                \
        HAL_Printf("%s", "\r\n");                      \
    } while (0)


static int upload_end = 0;
static void on_header(uint32_t stream_id, char *channel_id,int cat,const uint8_t *name,size_t namelen, 
                              const uint8_t *value,size_t valuelen, uint8_t flags)
{
    EXAMPLE_TRACE("~~~~~stream_id = %d, channel_id=%s, name = %s, value = %s, flag = %d\n", stream_id,channel_id,name,value,flags);
}

static void on_chunk_recv(uint32_t stream_id, char *channel_id,const uint8_t *data, size_t len,uint8_t flags)
{
     EXAMPLE_TRACE("~~~~~stream_id = %d, channel_id=%s, data = %s ,len = %d flag = %d\n", stream_id,channel_id,data,len,flags);
}
static void on_stream_close(uint32_t stream_id, char *channel_id,uint32_t error_code)
{
     EXAMPLE_TRACE("~~~~~stream_id = %d channel_id=%s, error_code = %d\n", stream_id,channel_id,error_code);
}

static http2_stream_cb_t my_cb = {
    .on_stream_header_cb = on_header,
    .on_stream_chunk_recv_cb = on_chunk_recv,
    .on_stream_close_cb = on_stream_close,
};

void upload_file_result(const char * path,int result, void * user_data)
{
    EXAMPLE_TRACE("===========path = %s,result =%d=========", path,result);
    upload_end =1;
}

static int http2_stream_test(char * file_path)
{
    int ret;
    device_conn_info_t conn_info;
    void *handle;
    memset(&conn_info, 0, sizeof( device_conn_info_t));
    conn_info.product_key = HTTP2_PRODUCT_KEY;
    conn_info.device_name = HTTP2_DEVICE_NAME;
    conn_info.device_secret = HTTP2_DEVICE_SECRET;
    conn_info.url = HTTP2_ONLINE_SERVER_URL;
    conn_info.port = HTTP2_ONLINE_SERVER_PORT;

    handle = IOT_HTTP2_Stream_Connect(&conn_info,&my_cb);
    if(handle == NULL) {
        return -1;
    }
    
    ret = IOT_HTTP2_Stream_UploadFile(handle,file_path,"iotx/vision/voice/intercom/live",NULL, 
                                upload_file_result, NULL);
    if(ret < 0) {
        return -1;
    }                        
    while(!upload_end) {
        HAL_SleepMs(200);
    }
    ret = IOT_HTTP2_Stream_Disconnect(handle);
    EXAMPLE_TRACE("close connect %d\n",ret);
    return 0;
}

int linkkit_main(void *paras)
{
    int ret;
    int argc = 0;
    char **argv = NULL;
    char * file_name;
    if (paras != NULL) {
        app_main_paras_t *p = (app_main_paras_t *)paras;
        argc = p->argc;
        argv = p->argv;
    }
    if(argc > 1) {
        file_name = argv[1];
    } else {
        printf("no file name input!\n");
        return 0;
    }

    IOT_SetLogLevel(IOT_LOG_DEBUG);

    ret = http2_stream_test(file_name);
    return ret;
}
