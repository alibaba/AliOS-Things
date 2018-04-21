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


#ifndef __COAP_PLATFORM_OS_H__
#define __COAP_PLATFORM_OS_H__
#include <stdio.h>
#ifdef COAP_USE_PLATFORM_MEMORY
#include "lite-utils.h"
#endif
#ifdef COAP_USE_PLATFORM_LOG
#include "lite-log.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#ifdef COAP_USE_PLATFORM_MEMORY
#define coap_malloc LITE_malloc
#define coap_free   LITE_free
#else
#include <aos/kernel.h>
#define coap_malloc aos_malloc
#define coap_free   aos_free
#endif

#ifdef COAP_USE_PLATFORM_LOG
#define COAP_TRC   log_debug
#define COAP_DUMP  log_debug
#define COAP_DEBUG log_debug
#define COAP_INFO  log_info
#define COAP_WARN  log_warning
#define COAP_ERR   log_err
#else
#include <aos/log.h>
#define LOG_TAG "CoAP"
#define COAP_DUMP(fmt, args...)  LOGD(LOG_TAG,fmt, ##args)
#define COAP_TRC(fmt,  args...)  LOGD(LOG_TAG,fmt, ##args)
#define COAP_DEBUG(fmt, args...) LOGD(LOG_TAG,fmt, ##args)
#define COAP_INFO(fmt, args...)  LOGI(LOG_TAG,fmt, ##args) 
#define COAP_ERR(fmt,  args...)  LOGE(LOG_TAG,fmt, ##args)
#endif

int platform_is_multicast(const char *ip_str);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
