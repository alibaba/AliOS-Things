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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if !defined(_WIN32)
#include <unistd.h>
#endif

#include "iot_import.h"
#include "iot_export.h"

#define IOTX_PRODUCT_KEY             "W9LchU2zAAK"
#define IOTX_DEVICE_NAME             "subdevice_2"
#define IOTX_DEVICE_SECRET           "Y8QN9QFGvbCVpJ23F2ZFuwhR4785NO5C"
#define IOTX_DEVICE_ID               "subdevice_2"


#define DEFAULT_TIMEOUT_MS 5000

static int iotx_post_data_to_server(void *handle)
{
    int          ret = -1;
    char         path[IOTX_URI_MAX_LEN + 1] = {0};
    char         rsp_buf[1024];
    iotx_http_t *iotx_http_context = (iotx_http_t *)handle;
    iotx_device_info_t *p_devinfo = iotx_http_context->p_devinfo;

    iotx_http_message_param_t msg_param;
    msg_param.request_payload = (char *)"{\"name\":\"hello world\"}";
    msg_param.response_payload = rsp_buf;
    msg_param.timeout_ms = iotx_http_context->timeout_ms;
    msg_param.request_payload_len = strlen(msg_param.request_payload) + 1;
    msg_param.response_payload_len = 1024;
    msg_param.topic_path = path;

    HAL_Snprintf(msg_param.topic_path, IOTX_URI_MAX_LEN, "/topic/%s/%s/update",
                 p_devinfo->product_key, p_devinfo->device_name);

    if (0 == (ret = IOT_HTTP_SendMessage(iotx_http_context, &msg_param))) {
        HAL_Printf("message response is %s\r\n", msg_param.response_payload);
    } else {
        HAL_Printf("error\r\n");
    }

    return ret;
}

int main(int argc, char **argv)
{
    iotx_device_info_t      device_info;
    iotx_http_param_t       http_param;
    int                     opt;
    void                   *handle = NULL;

    IOT_OpenLog("http");
    IOT_SetLogLevel(IOT_LOG_DEBUG);
    /**< set device info*/
    HAL_SetProductKey(IOTX_PRODUCT_KEY);
    HAL_SetDeviceName(IOTX_DEVICE_NAME);
    HAL_SetDeviceSecret(IOTX_DEVICE_SECRET);
    /**< end*/
    memset(&device_info, 0, sizeof(device_info));
    memset(&http_param, 0, sizeof(http_param));

    /**< get device info*/
    HAL_GetProductKey(device_info.product_key);
    HAL_GetDeviceName(device_info.device_name);
    HAL_GetDeviceSecret(device_info.device_secret);
    HAL_GetDeviceID(device_info.device_id);
    /**< end*/

#if !defined(_WIN32)
    HAL_Printf("[HTTP-Client]: Enter HTTP Client\r\n");
    while ((opt = getopt(argc, argv, "lh")) != -1) {
        switch (opt) {
            case 'l':
                http_param.keep_alive = 1;
                break;
            case 'h':
                /* TODO: */
                break;
            default:
                break;
        }
    }
#endif
    HAL_Printf("[HTTP-Client]: keep_alive=%d\r\n", http_param.keep_alive);
    http_param.device_info = &device_info;
    http_param.timeout_ms = DEFAULT_TIMEOUT_MS;

    handle = IOT_HTTP_Init(&http_param);
    if (NULL != handle) {
        IOT_HTTP_DeviceNameAuth(handle);
        iotx_post_data_to_server(handle);
        HAL_Printf("IoTx HTTP Message Sent\r\n");
    } else {
        HAL_Printf("IoTx HTTP init failed\r\n");

        return 0;
    }

    IOT_HTTP_Disconnect(handle);

    IOT_HTTP_DeInit(&handle);

    IOT_DumpMemoryStats(IOT_LOG_DEBUG);
    IOT_CloseLog();

    return 0;
}

