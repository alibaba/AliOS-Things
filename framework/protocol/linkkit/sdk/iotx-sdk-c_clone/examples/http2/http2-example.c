/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "iot_export.h"
#include "iot_export_file_uploader.h"

#if defined(ON_DAILY)
	#define HTTP2_ONLINE_SERVER_URL       "10.101.12.205/"
	#define HTTP2_ONLINE_SERVER_PORT      9999
	#define HTTP2_PRODUCT_KEY             "a1QWlwJO4Z2"
	#define HTTP2_DEVICE_NAME             "cHzzrWfxLiRLYHik3sYJ"
	#define HTTP2_DEVICE_SECRET           "vZxZiUMf3vwTViC3XIxJ2ZHt0zVoZlRa"
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

#define HTTP2_SYNC_API 0

#if HTTP2_SYNC_API
void* upload_thread(void *user_data)
{
    httpclient_t client = {0};
    file_sync_info file_info;
    char file_id[MAX_HTTP2_FILE_ID_LEN] = {0};     /*file id*/
    char store_id[MAX_HTTP2_FILE_STORE_LEN] = {0};     /*store id*/
    int pos = 1;
    void *handler = NULL;
    int ret = -1;
    int retry = 0;

    file_info.file_name = (char *)user_data;
    file_info.file_id = file_id;
    file_info.store_id = store_id;
    file_info.current_pos = &pos;
    file_info.type = 0;  /*0->file, 1-> log*/

    printf("name:%s\n", file_info.file_name);
    iotx_http2_set_device_info(HTTP2_PRODUCT_KEY, HTTP2_DEVICE_NAME, HTTP2_DEVICE_SECRET);

    while(retry < MAX_HTTP2_MAX_RETRANS_TIMES) {
        memset(&client, 0, sizeof(httpclient_t));
        handler = iotx_http2_client_connect((void *)&client, HTTP2_ONLINE_SERVER_URL, HTTP2_ONLINE_SERVER_PORT);
        if(handler == NULL) {
            printf("can't connect\n");
            return 0;
        }

        ret = iotx_upload_file(handler, &file_info);

        if(ret == HTTP2_UPLOAD_FILE_RET_OK) {
            printf("upload file success，exit!\n");
            break;
        } else {
            printf("upload file fail，ret=%d!\n", ret);
            retry++;
        }
    }
    iotx_http2_client_disconnect(handler);
    return NULL;
}

int main(int argc, char **argv)
{
    int ret = -1;
    pthread_t  pid;
    char *file_name = NULL;

    if(argc > 1) {
        file_name = argv[1];
    } else {
        printf("no file name input!\n");
        return 0;
    }
    ret = pthread_create(&pid, NULL, upload_thread, file_name);
    if (ret != 0) {
        printf("upload failed!\n");
        return 0;
    }
    while(1);
    return 0;
}
#else
int file_upload_callback1(int result ,char *store_id, void *user_data)
{
    if (result == 0)
        printf("file callback1 name:%s upload success: %s\n", (char *)user_data, store_id);
    else
        printf("file callback1 name:%s upload fail: %d\n", (char *)user_data, result);
    return 1;
}

int file_upload_callback2(int result ,char *store_id, void *user_data)
{
    if (result == 0)
        printf("file callback2 name:%s upload success: %s\n", (char *)user_data, store_id);
    else
        printf("file callback2 name:%s upload fail: %d\n", (char *)user_data, result);
    return 1;
}


void* thread1(void *user_data)
{
    char *file_name = (char *)user_data;
    int ret = -1;
    int type = 0; /*0->file, 1-> log*/
    if (file_name != NULL) {
        printf("name:%s\n", (char *)file_name);
        ret = iotx_upload_file_async(file_name, type, file_upload_callback1, user_data);
        printf("the result is %d\n", ret);
    }
    return NULL;
}


void* thread2(void *user_data)
{
    char *file_name = (char *)user_data;
    int ret = -1;
    int type = 1; /*0->file, 1-> log*/

    if (file_name != NULL) {
        printf("name:%s\n", (char *)file_name);
        ret = iotx_upload_file_async(file_name, type, file_upload_callback2, user_data);
        printf("the result is %d\n", ret);
    }
    return NULL;
}


void* thread_http2(void *user_data)
{
    device_conn_info conn_info;
    memset(&conn_info, 0, sizeof(device_conn_info));
    conn_info.product_key = HTTP2_PRODUCT_KEY;
    conn_info.device_name = HTTP2_DEVICE_NAME;
    conn_info.device_secret = HTTP2_DEVICE_SECRET;
    conn_info.url = HTTP2_ONLINE_SERVER_URL;
    conn_info.port = HTTP2_ONLINE_SERVER_PORT;
    iotx_http2_upload_file_init(&conn_info);
    return NULL;
}

int main(int argc, char **argv)
{
    int ret = -1;
    pthread_t  pid;
    pthread_t  pid1;
    pthread_t  pid2;
    char *file_name1 = NULL;
    char *file_name2 = NULL;
    IOT_OpenLog("http2");
    IOT_SetLogLevel(IOT_LOG_DEBUG);

    ret = pthread_create(&pid, NULL, thread_http2, NULL);
    if (ret != 0) {
        printf("pthread_create1 failed!\n");
        return 0;
    }
    /*wait http2 thread ready.*/
    HAL_SleepMs(1000);
    if(argc > 1) {
        file_name1 = argv[1];
        file_name2 = argv[2];
    } else {
        printf("no file name input!\n");
        return 0;
    }
    ret = pthread_create(&pid1, NULL, thread1, file_name1);
    if (ret != 0) {
        printf("pthread_create1 failed!\n");
        return 0;
    }

    ret = pthread_create(&pid2, NULL, thread2, file_name2);
    if (ret != 0) {
        printf("pthread_create2 failed!\n");
        return 0;
    }

    while(1);
    IOT_CloseLog();
    return 0;
}
#endif
