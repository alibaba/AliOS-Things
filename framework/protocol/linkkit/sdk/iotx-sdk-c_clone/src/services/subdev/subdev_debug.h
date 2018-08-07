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


#ifndef __SUBDEV_DEBUG_H__
#define __SUBDEV_DEBUG_H__

#include "iotx_log.h"

#define subdev_emerg(...)    log_emerg("subdev", __VA_ARGS__)
#define subdev_crit(...)     log_crit("subdev", __VA_ARGS__)
#define subdev_err(...)      log_err("subdev", __VA_ARGS__)
#define subdev_warning(...)  log_warning("subdev", __VA_ARGS__)
#define subdev_info(...)     log_info("subdev", __VA_ARGS__)
#define subdev_debug(...)    log_debug("subdev", __VA_ARGS__)

#endif  /* __SUBDEV_DEBUG_H__ */
