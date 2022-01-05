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
#ifndef __APP_FACTORY_BT_H__
#define __APP_FACTORY__BTH__

#include "app_key.h"


void app_factorymode_bt_create_connect(void);

void app_factorymode_bt_init_connect(void);

int app_factorymode_bt_xtalcalib_proc(void);

void app_factorymode_bt_xtalrangetest(APP_KEY_STATUS *status, void *param);

void app_factorymode_bt_signalingtest(APP_KEY_STATUS *status, void *param);

void app_factorymode_bt_nosignalingtest(APP_KEY_STATUS *status, void *param);

void app_factorymode_bt_xtalcalib(APP_KEY_STATUS *status, void *param);

#endif
