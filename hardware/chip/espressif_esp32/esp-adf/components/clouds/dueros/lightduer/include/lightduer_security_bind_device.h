/**
 * Copyright (2018) Baidu Inc. All rights reserved.
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
 * File: lightduer_security_bind_device.h
 * Auth: Chen Xihao (chenxihao@baidu.com)
 * Desc: Support App to bind device securely
 */

#ifndef BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_SECURITY_BIND_DEVICE_H
#define BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_SECURITY_BIND_DEVICE_H

#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * DESC:
 * Start the task to bind device securely.
 *
 * PARAM:
 * @param[in] uuid: the uuid of device
 * @param[in] bind_token: the bind token of device
 * @param[in] lifecycle: the lifecycle of task in seconds, at least 60s
 *
 * @RETURN: success return DUER_OK, failed return DUER_ERR_FAILED.
 */
duer_status_t duer_start_security_bind_device_task(const char *uuid,
        const char *bind_token, size_t lifecycle);

/**
 * DESC:
 * Stop the task that bind device.
 *
 * PARAM: none
 *
 * @RETURN: success return DUER_OK, failed return DUER_ERR_FAILED.
 */
duer_status_t duer_stop_security_bind_device_task(void);

#ifdef __cplusplus
}
#endif

#endif /* BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_SECURITY_BIND_DEVICE_H */

