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



#ifndef _IOTX_COMMON_HMAC_H_
#define _IOTX_COMMON_HMAC_H_

#include <string.h>

#define SHA1_DIGEST_SIZE 20

void utils_hmac_md5(const char *msg, int msg_len, char *digest, const char *key, int key_len);

void utils_hmac_sha1(const char *msg, int msg_len, char *digest, const char *key, int key_len);

void utils_hmac_sha1_hex(const char *msg, int msg_len, char *digest, const char *key, int key_len);

void utils_hmac_sha1_raw(const char *msg, int msg_len, char *digest, const char *key, int key_len);

void utils_hmac_sha1_base64(const char *msg, int msg_len, const char *key, int key_len, char *digest, int* digest_len);


#endif

