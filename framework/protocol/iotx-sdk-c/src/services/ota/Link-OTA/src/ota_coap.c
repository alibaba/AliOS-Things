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



#ifndef __OTA_COAP_C_H__
#define __OTA_COAP_C_H__

#include "iot_export_ota.h"
#include "iot_import_ota.h"
#include "ota_internal.h"

/* OSC, OTA signal channel */

/* Specify the maximum characters of version */
#define OSC_COAP_URI_MAX_LEN         (135)  /* IoTx CoAP uri maximal length */


typedef struct  {
    void *coap;
    const char *product_key;
    const char *device_name;
    ota_cb_fpt cb;
    void *context;
}otacoap_Struct_t, *otacoap_Struct_pt;


static otacoap_Struct_pt h_osc_coap = NULL;

static void otacoap_response_handler(void *arg, void *p_response)
{
    int len = 0;
    unsigned char *p_payload = NULL;
    iotx_coap_resp_code_t resp_code;
    IOT_CoAP_GetMessageCode(p_response, &resp_code);
    IOT_CoAP_GetMessagePayload(p_response, &p_payload, &len);
    OTA_LOG_DEBUG("CoAP response code = %d", resp_code);
    OTA_LOG_DEBUG("[CoAP msg_len=%d, msg=%s\r\n", len, p_payload);

    if ((NULL != h_osc_coap) && (NULL != p_payload)) {
        h_osc_coap->cb(h_osc_coap->context, (const char *)p_payload, (uint32_t)len);
    }
}


/* Generate topic name according to @ota_topic_type, @product_key, @device_name */
/* and then copy to @buf. */
/* 0, successful; -1, failed */
static int otacoap_GenTopicName(char *buf, size_t buf_len, const char *ota_topic_type, const char *product_key, const char *device_name)
{
    int ret;

    ret = OTA_SNPRINTF(buf,
            buf_len,
            "/topic/ota/device/%s/%s/%s",
            ota_topic_type,
            product_key,
            device_name);

    OTA_ASSERT(ret < buf_len);

    if (ret < 0) {
        OTA_LOG_ERROR("snprintf failed");
        return -1;
    }

    return 0;
}

/* report progress of OTA */
static int otacoap_Publish(otacoap_Struct_pt handle, const char *topic_type, const char *msg)
{
    int ret;
    char uri[IOTX_URI_MAX_LEN+1] = {0};
    iotx_message_t     message;
    message.p_payload = (unsigned char *)msg;
    message.payload_len = (unsigned short)strlen(msg);
    message.resp_callback = otacoap_response_handler;
    message.msg_type = IOTX_MESSAGE_CON;
    message.content_type = IOTX_CONTENT_TYPE_JSON;

    /* topic name: /topic/ota/device/${topic_type}/${productKey}/${deviceName} */
    ret = otacoap_GenTopicName(uri, OSC_COAP_URI_MAX_LEN, topic_type, handle->product_key, handle->device_name);
    if (ret < 0) {
       OTA_LOG_ERROR("generate topic name failed");
       return -1;
    }

    if (IOTX_SUCCESS != (ret = IOT_CoAP_SendMessage(handle->coap, (char *)uri, &message)))
    {
        OTA_LOG_ERROR("send CoAP msg failed%d", ret);
        return -1;
    }

    return 0;
}


void *osc_Init(const char *product_key, const char *device_name, void *ch_signal, ota_cb_fpt cb, void *context)
{
    otacoap_Struct_pt h_osc = NULL;

    if (NULL == (h_osc = OTA_MALLOC(sizeof(otacoap_Struct_t)))) {
        OTA_LOG_ERROR("allocate for h_osc failed");
        return NULL;
    }

    memset(h_osc, 0, sizeof(otacoap_Struct_t));

    h_osc->coap = ch_signal;
    h_osc->product_key = product_key;
    h_osc->device_name = device_name;
    h_osc->cb = cb;
    h_osc->context = context;

    h_osc_coap = h_osc;

    return h_osc;
}


int osc_Deinit(void *handle)
{
    if (NULL != handle) {
        OTA_FREE(handle);
    }

    return 0;
}

/* report progress of OTA */
int osc_ReportProgress(void *handle, const char *msg)
{
    return otacoap_Publish(handle, "progress", msg);
}


/* report version of OTA firmware */
int osc_ReportVersion(void *handle, const char *msg)
{
    static int state = 0;
    int ret;

    if (0 == state) { /* report version in initial state */
        ret = otacoap_Publish(handle, "inform", msg);
        if (0 != ret) {
            return ret;
        }
        state = 1;
    }

    /* request new firmware after initial state */
    return otacoap_Publish(handle, "request", msg);
}

#endif

