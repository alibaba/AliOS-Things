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


#ifndef __OTA_MQTT_C_H__
#define __OTA_MQTT_C_H__

#include "iot_export_ota.h"
#include "ota_internal.h"
#include "ota_hal_os.h"
#include "ota_log.h"

/* OSC, OTA signal channel */

/* Specify the maximum characters of version */

typedef struct  {
    void *mqtt;
    const char *product_key;
    const char *device_name;
    char topic_upgrade[OTA_MQTT_TOPIC_LEN];
    char topic_request[OTA_MQTT_TOPIC_LEN];
    char topic_config_get[OTA_MQTT_TOPIC_LEN];
    char topic_config_push[OTA_MQTT_TOPIC_LEN];
    ota_cb_fpt cb;
    void *context;
}otamqtt_Struct_t, *otamqtt_Struct_pt;


/* Generate topic name according to @ota_topic_type, @product_key, @device_name */
/* and then copy to @buf. */
/* 0, successful; -1, failed */
static int otamqtt_GenTopicName(char *buf, size_t buf_len, const char *ota_topic_type, const char *product_key, const char *device_name)
{
    int ret = 0;
    ret = ota_snprintf(buf,
                       buf_len,
                       "/ota/device/%s/%s/%s",
                       ota_topic_type,
                       product_key,
                       device_name);
    if ((ret > buf_len)||(ret < 0)) {
        OTA_LOG_E("HAL_Snprintf failed");
        return -1;
    }

    return 0;
}

/* report progress of OTA */
static int otamqtt_Publish(otamqtt_Struct_pt handle, const char *topic_type, int qos, const char *msg)
{
    int ret;
    char topic_name[OTA_MQTT_TOPIC_LEN];
    iotx_mqtt_topic_info_t topic_info;

    memset(&topic_info, 0, sizeof(iotx_mqtt_topic_info_t));

    if (0 == qos) {
        topic_info.qos = IOTX_MQTT_QOS0;
    } else {
        topic_info.qos = IOTX_MQTT_QOS1;
    }
    topic_info.payload = (void *)msg;
    topic_info.payload_len = strlen(msg);

    /* inform OTA to topic: "/ota/device/progress/$(product_key)/$(device_name)" */
    ret = otamqtt_GenTopicName(topic_name, OTA_MQTT_TOPIC_LEN, topic_type, handle->product_key, handle->device_name);
    if (ret < 0) {
        OTA_LOG_E("generate topic name of info failed");
        return -1;
    }

    ret = IOT_MQTT_Publish(handle->mqtt, topic_name, &topic_info);
    if (ret < 0) {
        OTA_LOG_E("publish failed");
        return IOT_OTAE_OSC_FAILED;
    }

    return 0;
}

static int otamqtt_publish_full_topic(otamqtt_Struct_pt handle, const char *topic_name, iotx_mqtt_topic_info_pt topic_msg)
{
    if (IOT_MQTT_Publish(handle->mqtt, topic_name, topic_msg) < 0) {
        OTA_LOG_E("publish failed");
        return IOT_OTAE_OSC_FAILED;
    }

    return 0;
}


/* decode JSON string to get firmware information, like firmware version, URL, file size, MD5. */
/* return NONE */
static void otamqtt_UpgrageCb(void *pcontext, void *pclient, iotx_mqtt_event_msg_pt msg)
{
    otamqtt_Struct_pt handle = (otamqtt_Struct_pt) pcontext;
    iotx_mqtt_topic_info_pt topic_info = (iotx_mqtt_topic_info_pt)msg->msg;

    OTA_LOG_D("topic=%.*s", topic_info->topic_len, topic_info->ptopic);
    OTA_LOG_D("len=%u, topic_msg=%.*s", topic_info->payload_len, topic_info->payload_len, (char *)topic_info->payload);

    if(NULL != strstr(topic_info->ptopic, "/ota/device/request"))
    {
        OTA_LOG_D("receive device request");
        /*if(NULL != HAL_strnstr(topic_info->payload, topic_info->payload_len,
            "url", strlen("url")))*/
        if (NULL != strstr(topic_info->payload, "url"))
        {
            OTA_LOG_I("get request reply for new version image");
            if (NULL != handle->cb)
            {
                handle->cb(handle->context, topic_info->payload, topic_info->payload_len, IOTX_OTA_TOPIC_TYPE_DEVICE_REQUEST);
            }
        }
    } else if (NULL != strstr(topic_info->ptopic, "/ota/device/upgrade")) {
        OTA_LOG_D("receive device upgrade");
        if (NULL != handle->cb)
        {
            handle->cb(handle->context, topic_info->payload, topic_info->payload_len, IOTX_OTA_TOPIC_TYPE_DEVICE_UPGRATE);
        }
    } else if (NULL != strstr(topic_info->ptopic, "/thing/config/get_reply")){
        OTA_LOG_D("receive config get_reply");
        if (NULL != handle->cb)
        {
            handle->cb(handle->context, topic_info->payload, topic_info->payload_len, IOTX_OTA_TOPIC_TYPE_CONFIG_GET);
        }
    } else if (NULL != strstr(topic_info->ptopic, "/thing/config/push")){
        OTA_LOG_D("receive config push");
        if (NULL != handle->cb)
        {
            if (0 != handle->cb(handle->context, topic_info->payload, topic_info->payload_len, IOTX_OTA_TOPIC_TYPE_CONFIG_PUSH)){
                /* fail, send fail response code:400 */
                const char *pvalue;
                uint32_t val_len;
                char topic[OTA_MQTT_TOPIC_LEN] = {0};
                char message[OTA_MQTT_TOPIC_LEN] = {0};
                iotx_mqtt_topic_info_t message_info;

                memset(&message_info, 0, sizeof(iotx_mqtt_topic_info_t));

                pvalue = otalib_JsonValueOf(topic_info->payload, topic_info->payload_len, "id", &val_len);

                HAL_Snprintf(topic,
                             OTA_MQTT_TOPIC_LEN,
                             "/sys/%s/%s/thing/config/push_reply",
                             handle->product_key,
                             handle->device_name);
                HAL_Snprintf(message,
                             OTA_MQTT_TOPIC_LEN,
                             "\"id\":%d,\"code\":\"%d\",\"data\":{}",
                             pvalue,
                             400);
                message_info.qos = IOTX_MQTT_QOS0;
                message_info.payload = (void *)message;
                message_info.payload_len = strlen(message);

                if (IOT_MQTT_Publish(handle->mqtt, topic, &message_info) < 0) {
                    OTA_LOG_E("publish failed");
                }
            }
        }
    }
}


void *osc_Init(const char *product_key, const char *device_name, void *ch_signal, ota_cb_fpt cb, void *context)
{
    int ret;
    otamqtt_Struct_pt h_osc = NULL;

    if (NULL == (h_osc = ota_malloc(sizeof(otamqtt_Struct_t)))) {
        OTA_LOG_E("allocate for h_osc failed");
        return NULL;
    }

    memset(h_osc, 0, sizeof(otamqtt_Struct_t));

    /* subscribe the OTA topic: "/ota/device/request/$(product_key)/$(device_name)" */
    ret = otamqtt_GenTopicName(h_osc->topic_request, OTA_MQTT_TOPIC_LEN, "request", product_key, device_name);
    if (ret < 0) {
        OTA_LOG_E("generate topic name of request failed");
        goto do_exit;
    }

    ret = IOT_MQTT_Subscribe(ch_signal, h_osc->topic_request, IOTX_MQTT_QOS1, otamqtt_UpgrageCb, h_osc);
    if (ret < 0) {
        OTA_LOG_E("mqtt subscribe failed");
        goto do_exit;
    }
    
    /* subscribe the OTA topic: "/ota/device/upgrade/$(product_key)/$(device_name)" */
    ret = otamqtt_GenTopicName(h_osc->topic_upgrade, OTA_MQTT_TOPIC_LEN, "upgrade", product_key, device_name);
    if (ret < 0) {
        OTA_LOG_E("generate topic name of upgrade failed");
        goto do_exit;
    }

    ret = IOT_MQTT_Subscribe(ch_signal, h_osc->topic_upgrade, IOTX_MQTT_QOS1, otamqtt_UpgrageCb, h_osc);
    if (ret < 0) {
        OTA_LOG_E("mqtt subscribe failed");
        goto do_exit;
    }

    /* subscribe the OTA topic: "/sys/{productKey}/{deviceName}/thing/config/get_reply" */
    ret = HAL_Snprintf(h_osc->topic_config_get,
                       OTA_MQTT_TOPIC_LEN,
                       "/sys/%s/%s/thing/config/get_reply",
                       product_key,
                       device_name);
    if (ret < 0) {
        OTA_LOG_E("generate topic name of config get failed");
        goto do_exit;
    }

    ret = IOT_MQTT_Subscribe(ch_signal, h_osc->topic_config_get, IOTX_MQTT_QOS0, otamqtt_UpgrageCb, h_osc);
    if (ret < 0) {
        OTA_LOG_E("mqtt subscribe failed");
        goto do_exit;
    }

    /* subscribe the OTA topic: "/sys/{productKey}/{deviceName}/thing/config/push" */
    ret = HAL_Snprintf(h_osc->topic_config_push,
                       OTA_MQTT_TOPIC_LEN,
                       "/sys/%s/%s/thing/config/push",
                       product_key,
                       device_name);
    if (ret < 0) {
        OTA_LOG_E("generate topic name of config get failed");
        goto do_exit;
    }

    ret = IOT_MQTT_Subscribe(ch_signal, h_osc->topic_config_push, IOTX_MQTT_QOS0, otamqtt_UpgrageCb, h_osc);
    if (ret < 0) {
        OTA_LOG_E("mqtt subscribe failed");
        goto do_exit;
    }

    h_osc->mqtt = ch_signal;
    h_osc->product_key = product_key;
    h_osc->device_name = device_name;
    h_osc->cb = cb;
    h_osc->context = context;

    return h_osc;

do_exit:
    if (NULL != h_osc) {
        ota_free(h_osc);
    }

    return NULL;
}


int osc_Deinit(void *handle)
{
    if (NULL != handle) {
        ota_free(handle);
    }

    return 0;
}

/* report progress of OTA */
int osc_ReportProgress(void *handle, const char *msg)
{
    return otamqtt_Publish(handle, "progress", 0, msg);
}


/* report version of OTA firmware */
int osc_ReportVersion(void *handle, const char *msg)
{
    return otamqtt_Publish(handle, "inform", 1, msg);
}

/* request the OTA firmware pushed by user*/
int osc_RequestImage(void *handle, const char *msg)
{
    return otamqtt_Publish(handle, "request", 1, msg);
}

/* request the config */
int osc_RequestConfig(void *handle, const char *topic_name, iotx_mqtt_topic_info_pt topic_msg)
{
    return otamqtt_publish_full_topic(handle, topic_name, topic_msg);
}
#endif
