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
#ifndef __NVRECORD_OTA_H__
#define __NVRECORD_OTA_H__

#ifdef __cplusplus
extern "C"{
#endif

/*****************************header include********************************/
#include "nvrecord_extension.h"

/******************************macro defination*****************************/

/******************************type defination******************************/

/****************************function declearation**************************/
#if defined(GSOUND_OTA_ENABLED)&&defined(VOICE_DATAPATH)
void nv_record_otarec_init(void);

void nv_record_otarec_get_ptr(void **ptr);

void nv_record_extension_update_gsound_ota_session(uint8_t gsoundOtaStatus,
                                                   uint32_t totalImageSize,
                                                   const char *session);

void nv_record_extension_update_gsound_ota_progress(uint32_t otaOffset);

void nv_record_extension_update_gsound_info(NV_GSOUND_INFO_T* gsoundInfo);
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifndef __NVRECORD_OTA_H__ */