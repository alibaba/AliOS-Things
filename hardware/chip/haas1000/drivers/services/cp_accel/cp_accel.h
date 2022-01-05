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
#ifndef __CP_ACCEL_H__
#define __CP_ACCEL_H__

#include "plat_types.h"
#include "hal_location.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int (*CP_ACCEL_CP_MAIN)(void);

typedef unsigned int (*CP_ACCEL_EVT_HDLR)(uint32_t event);

int cp_accel_open(CP_ACCEL_CP_MAIN cp_main, CP_ACCEL_EVT_HDLR cp_hdlr, CP_ACCEL_EVT_HDLR mcu_hdlr, CP_ACCEL_EVT_HDLR mcu_sys_hdlr);

int cp_accel_close(void);

int cp_accel_init_done(void);

int cp_accel_send_event_mcu2cp(uint32_t event);

int cp_accel_send_event_cp2mcu(uint32_t event);

#ifdef __cplusplus
}
#endif

#endif

