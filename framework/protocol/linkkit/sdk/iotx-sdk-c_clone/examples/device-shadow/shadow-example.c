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
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "iot_import.h"
#include "iot_export.h"

#define PRODUCT_KEY             "W9LchU2zAAK"
#define DEVICE_NAME             "subdevice_2"
#define DEVICE_SECRET           "Y8QN9QFGvbCVpJ23F2ZFuwhR4785NO5C"


#define SHADOW_MQTT_MSGLEN      (1024)

char g_product_key[PRODUCT_KEY_LEN + 1];
char g_product_secret[PRODUCT_SECRET_LEN + 1];
char g_device_name[DEVICE_NAME_LEN + 1];
char g_device_secret[DEVICE_SECRET_LEN + 1];

#define SHADOW_TRACE(fmt, ...)  \
    do { \
        HAL_Printf("%s|%03d :: ", __func__, __LINE__); \
        HAL_Printf(fmt, ##__VA_ARGS__); \
        HAL_Printf("%s", "\r\n"); \
    } while(0)

/**
 * @brief This is a callback function when a control value coming from server.
 *
 * @param [in] pattr: attribute structure pointer
 * @return none
 * @see none.
 * @note none.
 */
static void _device_shadow_cb_light(iotx_shadow_attr_pt pattr)
{

    /*
     * ****** Your Code ******
     */

    SHADOW_TRACE("----");
    SHADOW_TRACE("Attrbute Name: '%s'", pattr->pattr_name);
    SHADOW_TRACE("Attrbute Value: %d", *(int32_t *)pattr->pattr_data);
    SHADOW_TRACE("----");
}


/* Device shadow demo entry */
int demo_device_shadow(char *msg_buf, char *msg_readbuf)
{
    char buf[1024];
    iotx_err_t rc;
    iotx_conn_info_pt puser_info;
    void *h_shadow;
    iotx_shadow_para_t shadow_para;

    /**< get device info*/
    HAL_GetProductKey(g_product_key);
    HAL_GetDeviceName(g_device_name);
    HAL_GetDeviceSecret(g_device_secret);
    /**< end*/
    /* Device AUTH */
//    rc = IOT_SetupConnInfo(PRODUCT_KEY, DEVICE_NAME, DEVICE_SECRET, (void **)&puser_info);
    rc = IOT_SetupConnInfo(g_product_key, g_device_name, g_device_secret, (void **)&puser_info);
    if (SUCCESS_RETURN != rc) {
        SHADOW_TRACE("rc = IOT_SetupConnInfo() = %d", rc);
        return rc;
    }

    /* Construct a device shadow */
    memset(&shadow_para, 0, sizeof(iotx_shadow_para_t));

    shadow_para.mqtt.port = puser_info->port;
    shadow_para.mqtt.host = puser_info->host_name;
    shadow_para.mqtt.client_id = puser_info->client_id;
    shadow_para.mqtt.username = puser_info->username;
    shadow_para.mqtt.password = puser_info->password;
    shadow_para.mqtt.pub_key = puser_info->pub_key;

    shadow_para.mqtt.request_timeout_ms = 2000;
    shadow_para.mqtt.clean_session = 0;
    shadow_para.mqtt.keepalive_interval_ms = 60000;
    shadow_para.mqtt.pread_buf = msg_readbuf;
    shadow_para.mqtt.read_buf_size = SHADOW_MQTT_MSGLEN;
    shadow_para.mqtt.pwrite_buf = msg_buf;
    shadow_para.mqtt.write_buf_size = SHADOW_MQTT_MSGLEN;

    shadow_para.mqtt.handle_event.h_fp = NULL;
    shadow_para.mqtt.handle_event.pcontext = NULL;

    h_shadow = IOT_Shadow_Construct(&shadow_para);
    if (NULL == h_shadow) {
        SHADOW_TRACE("construct device shadow failed!");
        return rc;
    }


    /* Define and add two attribute */

    int32_t light = 1000, temperature = 1001;
    iotx_shadow_attr_t attr_light, attr_temperature;

    memset(&attr_light, 0, sizeof(iotx_shadow_attr_t));
    memset(&attr_temperature, 0, sizeof(iotx_shadow_attr_t));

    /* Initialize the @light attribute */
    attr_light.attr_type = IOTX_SHADOW_INT32;
    attr_light.mode = IOTX_SHADOW_RW;
    attr_light.pattr_name = "switch";
    attr_light.pattr_data = &light;
    attr_light.callback = _device_shadow_cb_light;

    /* Initialize the @temperature attribute */
    attr_temperature.attr_type = IOTX_SHADOW_INT32;
    attr_temperature.mode = IOTX_SHADOW_READONLY;
    attr_temperature.pattr_name = "temperature";
    attr_temperature.pattr_data = &temperature;
    attr_temperature.callback = NULL;


    /* Register the attribute */
    /* Note that you must register the attribute you want to synchronize with cloud
     * before calling IOT_Shadow_Pull() */
    IOT_Shadow_RegisterAttribute(h_shadow, &attr_light);
    IOT_Shadow_RegisterAttribute(h_shadow, &attr_temperature);


    /* synchronize the device shadow with device shadow cloud */
    IOT_Shadow_Pull(h_shadow);

    do {
        format_data_t format;

        /* Format the attribute data */
        IOT_Shadow_PushFormat_Init(h_shadow, &format, buf, 1024);
        IOT_Shadow_PushFormat_Add(h_shadow, &format, &attr_temperature);
        IOT_Shadow_PushFormat_Add(h_shadow, &format, &attr_light);
        IOT_Shadow_PushFormat_Finalize(h_shadow, &format);

        /* Update attribute data */
        IOT_Shadow_Push(h_shadow, format.buf, format.offset, 10);

        /* Sleep 1000 ms */
        HAL_SleepMs(1000);
    } while (0);


    /* Delete the two attributes */
    IOT_Shadow_DeleteAttribute(h_shadow, &attr_temperature);
    IOT_Shadow_DeleteAttribute(h_shadow, &attr_light);

    IOT_Shadow_Destroy(h_shadow);

    return 0;
}


int main()
{
    IOT_OpenLog("shadow");
    IOT_SetLogLevel(IOT_LOG_DEBUG);
    /**< set device info*/
    HAL_SetProductKey(PRODUCT_KEY);
    HAL_SetDeviceName(DEVICE_NAME);
    HAL_SetDeviceSecret(DEVICE_SECRET);
    /**< end*/
    char *msg_buf = (char *)HAL_Malloc(SHADOW_MQTT_MSGLEN);
    char *msg_readbuf = (char *)HAL_Malloc(SHADOW_MQTT_MSGLEN);

    demo_device_shadow(msg_buf, msg_readbuf);

    HAL_Free(msg_buf);
    HAL_Free(msg_readbuf);

    SHADOW_TRACE("out of demo!");
    IOT_DumpMemoryStats(IOT_LOG_DEBUG);
    IOT_CloseLog();

    return 0;
}

