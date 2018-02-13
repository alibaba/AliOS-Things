/*
 * Copyright (C) 2016 YunOS Project. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <k_api.h>

#undef  PSR_SP
#define PSR_SP  (1UL << 29)
static ktask_t *tee_caller_task = NULL;

static inline uint32_t getcurrentpsr(void)
{
    uint32_t flags;

     __asm__ __volatile__(
        "mfcr   %0, psr \n"
        :"=r"(flags)
        :
        :
        );

    return flags;
}

static inline void clear_psr_sp(void)
{
    __asm__ __volatile__ (
        "mfcr  r0, psr \n"
        "bclri r0, 29  \n"
        "mtcr  r0, psr \n"
        :
        :
        :"r0"
     );
}

static inline void set_psr_sp(void)
{
    __asm__ __volatile__ (
        "mfcr  r0, psr \n"
        "bseti r0, 29  \n"
        "mtcr  r0, psr \n"
        :
        :
        :"r0"
     );
}

void csky_get_tee_caller_task(void)
{
    uint32_t temp_psr;

    temp_psr = getcurrentpsr();
    if (temp_psr & PSR_SP) {
       tee_caller_task = (tee_caller_task == NULL) ? g_active_task[cpu_cur_get()] : tee_caller_task;
    }
}

void csky_deal_tee_caller_task(void)
{
    uint32_t temp_psr;

    temp_psr = getcurrentpsr();
    if (temp_psr & PSR_SP) {
        if (tee_caller_task != NULL) {
            if (tee_caller_task == g_active_task[cpu_cur_get()]) {
                tee_caller_task = NULL;
            } else {
                clear_psr_sp();
            }
        }
    } else {
        if (tee_caller_task != NULL) {
            if (tee_caller_task == g_active_task[cpu_cur_get()]) {
                tee_caller_task = NULL;
                set_psr_sp();
            }
        }
    }
}
