/**
 * Copyright (2017) Baidu Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
// Author: Chang Li (changli@baidu.com)
//
// Description: NTP client API decleration.

#include <stdint.h>

#ifndef BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_IOT_NTP_H
#define BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_IOT_NTP_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _DuerTime {
    uint32_t sec;
    uint32_t usec;
} DuerTime;

#define NTP_PKG_LEN 48
#define NTP_MAC_LEN 20  // contains 4 Bytes:Key ID and 16 Bytes:Message Authentication Code

#define MD5_KEYID_LEN  4
#define MD5_DIGEST_LEN 16
#define MD5_KEY_LEN    16

typedef struct _md5_key {
    uint32_t key_id;
    char key_str[MD5_KEY_LEN];
} Md5Key;

/**
 * @brief get NTP time
 * 
 * @param host: NTP server name. If host is NULL, use "s2c.time.edu.cn" by
 * default.
 *
 * @param timeout: timeout to acquire the NTP time, unit is micro seconds. If
 * it's 0, use 2000 ms by default. 
 *
 * @param result_time: the acquired NTP time.
 *
 * @param key: Using keyed MD5 for Authentication for some specific NTP servers.
 * If unnecessary, specify it NULL.
 *
 * @return 0 is okay, netgative value is failed. 
 */
int duer_ntp_client(char* host, int timeout, DuerTime* result_time, Md5Key* key);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_IOT_NTP_H

