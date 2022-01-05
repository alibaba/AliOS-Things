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
#ifdef __INTERCONNECTION__
#ifndef __APP_INTERCONNECTION_CCMP_DEFINE_H__
#define __APP_INTERCONNECTION_CCMP_DEFINE_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "bluetooth.h"

#define CCMP_RECV_BUFFER_SIZE                        (4096+100)

struct spp_device *app_interconnection_get_ccmp_dev();

void app_interconnection_send_data_via_ccmp(uint8_t *ptrData, uint32_t length);

void app_ccmp_client_open(uint8_t *pServiceSearchRequest, uint8_t serviceSearchRequestLen, uint8_t port);

void app_interconnection_ccmp_disconnected_callback();

#ifdef __cplusplus
}
#endif

#endif // #ifndef __APP_INTERCONNECTION_CCMP_DEFINE_H__
#endif // #ifdef __INTERCONNECTION__