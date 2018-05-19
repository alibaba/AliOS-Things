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


#ifndef _IOTX_AUTH_H_
#define _IOTX_AUTH_H_

#if defined(__cplusplus)
extern "C" {
#endif

#include "iot_import.h"
#include "iot_export.h"

#define MQTT_SDK_VERSION    "2.0"

#ifndef MQTT_ID2_AUTH
int iotx_guider_authenticate(void);
#else
/* func name: iotx_guider_id2_authenticate
 * description: used for id2 authentication
 * intput :
 * output :
 * return : int
 *           0   success
 *          -1   error
 */
int iotx_guider_id2_authenticate(void);
#endif

int     iotx_device_info_init(void);

int     iotx_device_info_set(
            const char *product_key,
            const char *device_name,
            const char *device_secret);

iotx_device_info_pt iotx_device_info_get(void);
iotx_conn_info_pt iotx_conn_info_get(void);


#include <stdio.h>


#define MIDREPORT_PAYLOAD_LEN   (62 + PID_STRLEN_MAX + MID_STRLEN_MAX + 32 +1)
#define MIDREPORT_REQID_LEN     (PRODUCT_KEY_LEN + DEVICE_NAME_LEN + 6)

int iotx_midreport_reqid(char *requestId, char *product_key, char *device_name);
int iotx_midreport_payload(char *msg, char *requestId, char *mid, char *pid);
int iotx_midreport_topic(char *topic_name, char *topic_head, char *product_key, char *device_name);


const char *iotx_ca_get(void);

#if defined(__cplusplus)
}
#endif
#endif
