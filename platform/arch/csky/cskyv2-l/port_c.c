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

void *cpu_task_stack_init(cpu_stack_t *stack_base, size_t stack_size,
                          void *arg, task_entry_t entry)
{
    cpu_stack_t *stk;
    uint32_t temp = (uint32_t)(stack_base + stack_size);

    temp &= 0xfffffffc;

    stk = (cpu_stack_t *)temp;

    *(--stk) = (uint32_t)entry;               /* entry point   */
#ifndef CONFIG_SYSTEM_SECURE
    *(--stk) = (uint32_t)0x80000140L;         /* PSR           */
#else
    *(--stk) = (uint32_t)0xe0000140L;         /* PSR           */
#endif
    *(--stk) = (uint32_t)krhino_task_deathbed; /* R15 (LR)      */
    *(--stk) = (uint32_t)0x13131313L;         /* R13           */
    *(--stk) = (uint32_t)0x12121212L;         /* R12           */
    *(--stk) = (uint32_t)0x11111111L;         /* R11           */
    *(--stk) = (uint32_t)0x10101010L;         /* R10           */
    *(--stk) = (uint32_t)0x09090909L;         /* R9            */
    *(--stk) = (uint32_t)0x00000000L;         /* R8            */
    *(--stk) = (uint32_t)0x07070707L;         /* R7            */
    *(--stk) = (uint32_t)0x06060606L;         /* R6            */
    *(--stk) = (uint32_t)0x05050505L;         /* R5            */
    *(--stk) = (uint32_t)0x04040404L;         /* R4            */
    *(--stk) = (uint32_t)0x03030303L;         /* R3            */
    *(--stk) = (uint32_t)0x02020202L;         /* R2            */
    *(--stk) = (uint32_t)0x01010101L;         /* R1            */
    *(--stk) = (uint32_t)arg;                 /* R0 : argument */

    return stk;
}

