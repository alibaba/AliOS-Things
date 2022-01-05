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
#ifndef __APP_INTERACTION_SPP_H__
#define __APP_INTERACTION_SPP_H__

#include "spp_api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*app_spp_tx_done_t)(void);

//add new function
void green_spp_server_open(void);
void app_interaction_send_cmd_via_spp(uint8_t* ptrData, uint32_t length);

void green_spp_register_tx_done(app_spp_tx_done_t callback);


#ifdef __cplusplus
}
#endif

#endif

