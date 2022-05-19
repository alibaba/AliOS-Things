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
/**
 * File: lightduer_http_client_ops.h
 * Author: Zhong Shuai(zhongshuai@baidu.com)
 * Desc: HTTP Client OPS Head File
 */

#ifndef BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_HTTP_CLIENT_OPS_H
#define BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_HTTP_CLIENT_OPS_H
#ifdef __cplusplus
extern "C" {
#endif
#include "lightduer_http_client.h"

extern duer_http_client_t *duer_create_http_client(void);

extern void duer_destory_http_client(duer_http_client_t *client);

#ifdef __cplusplus
}
#endif
#endif // BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_HTTP_CLIENT_OPS_H
