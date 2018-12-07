// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ESPOS_ESP8266_H_
#define _ESPOS_ESP8266_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ESPOS_PROCESSORS_NUM 1u

#define ESPOS_SPINLOCK_ARCH_UNLOCK_INITIALIZER \
    {                                          \
        0,                                     \
    }

typedef int32_t esp_err_t;

typedef struct espos_spinlock_arch {
    int lock;
} espos_spinlock_arch_t;

static inline uintptr_t espos_suspend_interrupt(void)
{
    uintptr_t state;

    __asm__ volatile ("RSIL %0, 5" : "=a" (state) :: "memory");

    return state;
}

static inline void espos_resume_interrupt(uintptr_t state)
{
    __asm__ volatile ("WSR %0, ps" :: "a" (state) : "memory");
}

#define espos_get_core_id() 0

#ifdef __cplusplus
}
#endif

#endif /* _ESPOS_ESP8266_H_ */
