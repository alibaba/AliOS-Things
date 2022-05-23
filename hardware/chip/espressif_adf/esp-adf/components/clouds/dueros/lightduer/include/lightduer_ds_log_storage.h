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
/*
 * Auth: Gang Chen(chengang12@baidu.com)
 * Desc: Save the ds log to ROM.
 */

#ifndef BAIDU_DUER_LIGHTDUER_DS_LOG_STORAGE_H
#define BAIDU_DUER_LIGHTDUER_DS_LOG_STORAGE_H

#ifdef DUER_DS_LOG_STORAGE_ENABLED

#ifdef __cplusplus
extern "C" {
#endif

#include "lightduer_types.h"
#include "lightduer_flash_ring_buf.h"

duer_status_t duer_ds_log_storage_push(const char *log);

duer_status_t duer_ds_log_storage_send(void);

duer_status_t duer_ds_log_storage_init(duer_flash_context_t *ctx,
                                       duer_flash_config_t *config);

#ifdef __cplusplus
}
#endif

#endif // DUER_DS_LOG_STORAGE_ENABLED

#endif // BAIDU_DUER_LIGHTDUER_DS_LOG_STORAGE_H