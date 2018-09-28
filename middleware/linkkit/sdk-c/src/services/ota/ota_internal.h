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



#ifndef _OTA_INTERNAL_H_
#define _OTA_INTERNAL_H_

#define OSC_COAP_URI_MAX_LEN (135)
#define OTA_MQTT_TOPIC_LEN   (128)
typedef enum {
    IOTX_OTA_TOPIC_TYPE_DEVICE_REQUEST  = 1,
    IOTX_OTA_TOPIC_TYPE_DEVICE_UPGRATE = 2,
    IOTX_OTA_TOPIC_TYPE_CONFIG_GET     = 3,
    IOTX_OTA_TOPIC_TYPE_CONFIG_PUSH  = 4,
    IOTX_OTA_TOPIC_TYPE_MAX
}iotx_ota_topic_types_t;

typedef int (*ota_cb_fpt)(void *pcontext, const char *msg, uint32_t msg_len, iotx_ota_topic_types_t type);
/* is_fetch = 0; start fetch */
/* is_fetch = 1; stop fetch */
typedef void(*ota_fetch_cb_fpt)(void* user_data, int is_fetch, uint32_t size_file, char *purl, char *version);
/* is_fetch = 0; start fetch */
/* is_fetch = 1; stop fetch */
typedef void(*cota_fetch_cb_fpt)(void* user_data, int is_fetch, char* configId, uint32_t configSize, char *sign, \
                                 char *signMethod, char* url, char* getType);

int iotx_ota_set_fetch_callback(void* pt, ota_fetch_cb_fpt fetch_cb, void* user_data);
int iotx_ota_set_cota_fetch_callback(void* pt, cota_fetch_cb_fpt fetch_cb, void* user_data);
#endif /* _OTA_INTERNAL_H_ */
