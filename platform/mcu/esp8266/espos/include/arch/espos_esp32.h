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

#ifndef _ESPOS_ESP32_H_
#define _ESPOS_ESP32_H_

#include "sdkconfig.h"

#ifdef __cplusplus
    extern "C" {
#endif

#ifndef CONFIG_FREERTOS_UNICORE
#define ESPOS_PROCESSORS_NUM 2u
#define CONFIG_ESPOS_SMP
#else
#define ESPOS_PROCESSORS_NUM 1u
#endif

typedef struct espos_spinlock_arch {
    int lock;
} espos_spinlock_arch_t;

#define ESPOS_SPINLOCK_ARCH_UNLOCK_INITIALIZER \
    {                                          \
        0,                                     \
    }

static inline int espos_get_core_id(void)
{
    int id;

    __asm__ __volatile__ (
            "       rsr.prid %0\n"
            "       extui    %0, %0, 13, 1\n"
            :"=r"(id));

    return id;
}

static inline void espos_spinlock_arch_init(espos_spinlock_arch_t *lock)
{
    lock->lock = 0;
}

static inline void espos_spinlock_arch_deinit(espos_spinlock_arch_t *lock)
{
    lock->lock = 0;
}

static inline void espos_spinlock_arch_lock(espos_spinlock_arch_t *lock)
{
    unsigned long tmp;

    __asm__ __volatile__(
            "       movi    %0, 0\n"
            "       wsr     %0, scompare1\n"
            "1:     movi    %0, 1\n"
            "       s32c1i  %0, %1, 0\n"
            "       bnez    %0, 1b\n"
            : "=&a" (tmp)
            : "a" (&lock->lock)
            : "memory");
}

static inline int espos_spinlock_arch_trylock(espos_spinlock_arch_t *lock)
{
    unsigned long tmp;

    __asm__ __volatile__(
            "       movi    %0, 0\n"
            "       wsr     %0, scompare1\n"
            "       movi    %0, 1\n"
            "       s32c1i  %0, %1, 0\n"
            : "=&a" (tmp)
            : "a" (&lock->lock)
            : "memory");

    return tmp == 0 ? 0 : 1;
}

static inline void espos_spinlock_arch_unlock(espos_spinlock_arch_t *lock)
{
    unsigned long tmp;

    __asm__ __volatile__(
            "       movi    %0, 0\n"
            "       s32ri   %0, %1, 0\n"
            : "=&a" (tmp)
            : "a" (&lock->lock)
            : "memory");
}


#ifdef __cplusplus
    }
#endif

#endif /* _ESPOS_ESP32_H_ */
