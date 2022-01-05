/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#if defined(NEW_NV_RECORD_ENALBED)

#ifndef NVRECORD_ENV_H
#define NVRECORD_ENV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "me_api.h"
#include "nvrecord_extension.h"

#define NVRAM_ENV_MEDIA_LANGUAGE_DEFAULT (0)
#define NVRAM_ENV_TWS_MODE_DEFAULT (0xff)
#define NVRAM_ENV_FACTORY_TESTER_STATUS_DEFAULT (0xaabbccdd)

int nv_record_env_init(void);
int nv_record_env_get(struct nvrecord_env_t **nvrecord_env);
int nv_record_env_set(struct nvrecord_env_t *nvrecord_env);
void nv_record_update_ibrt_info(uint32_t newMode,bt_bdaddr_t *ibrtPeerAddr);
void nvrecord_rebuild_system_env(struct nvrecord_env_t* pSystemEnv);
void nv_record_update_factory_tester_status(uint32_t status);

#ifdef __cplusplus
}
#endif
#endif
#endif // #if defined(NEW_NV_RECORD_ENALBED)
