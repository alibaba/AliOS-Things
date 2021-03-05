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
#ifndef __HTTP_DEBUG_H__
#define __HTTP_DEBUG_H__

#include "iotx_log.h"

#define h2_emerg(...)    log_emerg("h2", __VA_ARGS__)
#define h2_crit(...)     log_crit("h2", __VA_ARGS__)
#define h2_err(...)      log_err("h2", __VA_ARGS__)
#define h2_warning(...)  log_warning("h2", __VA_ARGS__)
#define h2_info(...)     log_info("h2", __VA_ARGS__)
#define h2_debug(...)    log_debug("h2", __VA_ARGS__)

#endif  /* __HTTP_DEBUG_H__ */
