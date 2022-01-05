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
#ifndef __APP_FACTORY_H__
#define __APP_FACTORY_H__

#define APP_FACTORY_TRACE(s,...) TRACE(s, ##__VA_ARGS__)

void app_factorymode_result_set(bool result);

void app_factorymode_result_clean(void);

bool app_factorymode_result_wait(void);

void app_factorymode_enter(void);

void app_factorymode_key_init(void);

int app_factorymode_init(uint32_t factorymode);

int app_factorymode_calib_only(void);

int app_factorymode_languageswitch_proc(void);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __USB_COMM__
int app_factorymode_cdc_comm(void);
#endif

bool app_factorymode_get(void);

void app_factorymode_set(bool set);

#ifdef __cplusplus
}
#endif

#endif
