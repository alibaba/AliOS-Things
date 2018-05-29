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


#ifndef __GUIDER_INTERNAL_H__
#define __GUIDER_INTERNAL_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

#include "iot_import.h"

#include "lite-log.h"
#include "lite-utils.h"
#include "utils_md5.h"
#include "utils_base64.h"
#include "utils_hmac.h"
#include "utils_httpc.h"
#include "lite-system.h"

#define GUIDER_IOT_ID_LEN           (256)
#define GUIDER_IOT_TOKEN_LEN        (512)
#define GUIDER_DEFAULT_TS_STR       "2524608000000"

#define CONN_SECMODE_LEN            (32)

#define GUIDER_SIGN_LEN             (256)

#define GUIDER_TS_LEN               (16)
#define GUIDER_URL_LEN              (256)

#define GUIDER_DEVCODE_LEN          (256)
#define GUIDER_URLENCODE_LEN        (256)

#define GUIDER_DIRECT_DOMAIN        "iot-as-mqtt.cn-shanghai.aliyuncs.com"

#define SHA_METHOD              "hmacsha1"
#define MD5_METHOD              "hmacmd5"

/* By default we use hmac-sha1 algorithm for hmac in PK/DN/DS case */
#define USING_SHA1_IN_HMAC      (1)

typedef enum _SECURE_MODE {
    MODE_TLS_GUIDER             = -1,
    MODE_TCP_GUIDER_PLAIN       = 0,
    MODE_TCP_GUIDER_ID2_ENCRYPT = 1,
    MODE_TLS_DIRECT             = 2,
    MODE_TCP_DIRECT_PLAIN       = 3,
    MODE_TCP_DIRECT_ID2_ENCRYPT = 4,
    MODE_TLS_GUIDER_ID2_ENCRYPT = 5,
    MODE_TLS_DIRECT_ID2_ENCRYPT = 7,
} SECURE_MODE;

extern const char *secmode_str[];

SECURE_MODE _secure_mode_num(void);
void _ident_partner(char *buf, int len);
void _ident_module(char *buf, int len);
int _fill_conn_string(char *dst, int len, const char *fmt, ...);
void guider_print_dev_guider_info(iotx_device_info_pt dev,
                                  char *partner_id,
                                  char *module_id,
                                  char *guider_url,
                                  int secure_mode,
                                  char *time_stamp,
                                  char *guider_sign,
                                  char *id2,
                                  char *dev_code);
void guider_print_conn_info(iotx_conn_info_pt conn);

#ifndef MQTT_DIRECT
int _http_response(char *payload,
                   const int payload_len,
                   const char *request_string,
                   const char *url,
                   const int port_num,
                   const char *pkey);
#endif

#endif  /* __GUIDER_INTERNAL_H__ */
