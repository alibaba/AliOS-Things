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


#ifndef __OTA_DEBUG_H__
#define __OTA_DEBUG_H__

#include "lite-log.h"

#define OTA_LOG_EMERG(...)    log_emerg("ota", __VA_ARGS__)
#define OTA_LOG_CRIT(...)     log_crit("ota", __VA_ARGS__)
#define OTA_LOG_ERROR(...)    log_err("ota", __VA_ARGS__)
#define OTA_LOG_WRN(...)      log_warning("ota", __VA_ARGS__)
#define OTA_LOG_INFO(...)     log_info("ota", __VA_ARGS__)
#define OTA_LOG_DEBUG(...)    log_debug("ota", __VA_ARGS__)

#endif  /* __OTA_DEBUG_H__ */
