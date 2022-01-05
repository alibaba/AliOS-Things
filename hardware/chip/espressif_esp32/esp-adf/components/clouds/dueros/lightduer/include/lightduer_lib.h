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
// Author: Su Hao (suhao@baidu.com)
//
// Description: The internal common header.

#ifndef BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_INTERNAL_H
#define BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_INTERNAL_H

#include <string.h>

#define DUER_MEMCPY(...)         memcpy(__VA_ARGS__)
#define DUER_MEMCMP(...)         memcmp(__VA_ARGS__)
#define DUER_MEMSET(...)         memset(__VA_ARGS__)
#define DUER_MEMMOVE(...)        memmove(__VA_ARGS__)
#define DUER_STRLEN(...)         strlen(__VA_ARGS__)
#define DUER_STRCMP(...)         strcmp(__VA_ARGS__)
#define DUER_STRNCPY(...)        strncpy(__VA_ARGS__)
#define DUER_STRNCMP(...)        strncmp(__VA_ARGS__)
#define DUER_SNPRINTF(...)       snprintf(__VA_ARGS__)
#define DUER_STRNCASECMP(...)    strncasecmp(__VA_ARGS__)
#define DUER_SSCANF(...)         sscanf(__VA_ARGS__)
#define DUER_STRSTR(...)         strstr(__VA_ARGS__)
#define DUER_STRCHR(...)         strchr(__VA_ARGS__)
#define DUER_STRNCAT(...)        strncat(__VA_ARGS__)

// Suppress Compiler warning Function&Variable declared never referenced
#define ALLOW_UNUSED_LOCAL(VAR_FUNC) (void)(VAR_FUNC)

#endif // BAIDU_IOT_TINYDU_IOT_OS_SRC_IOT_BAIDU_CA_SOURCE_BAIDU_CA_INTERNAL_H
