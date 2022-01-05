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
#ifndef __SYSTEM_ARMCM_H__
#define __SYSTEM_ARMCM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"

extern uint32_t SystemCoreClock;     /*!< System Clock Frequency (Core Clock)  */

void SystemInit (void);

// #ifdef UNALIGNED_ACCESS

// __STATIC_FORCEINLINE bool get_unaligned_access_status(void) { return true; }

// __STATIC_FORCEINLINE bool config_unaligned_access(bool enable) { return true; }

// #else

bool get_unaligned_access_status(void);

bool config_unaligned_access(bool enable);

// #endif

uint32_t get_cpu_id(void);

#ifdef __cplusplus
}
#endif

#endif

