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
    register int *gp asm("x3");
    uint32_t temp = (uint32_t)(stack_base + stack_size);

    temp &= 0xFFFFFFF8UL;

    stk = (cpu_stack_t *)temp;

    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       
    *(--stk)  = (uint32_t)0x12345678L;          *(--stk)  = (uint32_t)0x87654321L;       

    *(--stk) = (uint32_t)entry;                   /* PC            */
    *(--stk) = (uint32_t)0x31313131L;             /* X31           */
    *(--stk) = (uint32_t)0x30303030L;             /* X30           */
    *(--stk) = (uint32_t)0x29292929L;             /* X29           */
    *(--stk) = (uint32_t)0x28282828L;             /* X28           */
    *(--stk) = (uint32_t)0x27272727L;             /* X27           */
    *(--stk) = (uint32_t)0x26262626L;             /* X26           */
    *(--stk) = (uint32_t)0x25252525L;             /* X25           */
    *(--stk) = (uint32_t)0x24242424L;             /* X24           */
    *(--stk) = (uint32_t)0x23232323L;             /* X23           */
    *(--stk) = (uint32_t)0x22222222L;             /* X22           */
    *(--stk) = (uint32_t)0x21212121L;             /* X21           */
    *(--stk) = (uint32_t)0x20202020L;             /* X20           */
    *(--stk) = (uint32_t)0x19191919L;             /* X19           */
    *(--stk) = (uint32_t)0x18181818L;             /* X18           */
    *(--stk) = (uint32_t)0x17171717L;             /* X17           */
    *(--stk) = (uint32_t)0x16161616L;             /* X16           */
    *(--stk) = (uint32_t)0x15151515L;             /* X15           */
    *(--stk) = (uint32_t)0x14141414L;             /* X14           */
    *(--stk) = (uint32_t)0x13131313L;             /* X13           */
    *(--stk) = (uint32_t)0x12121212L;             /* X12           */
    *(--stk) = (uint32_t)0x11111111L;             /* X11           */
    *(--stk) = (uint32_t)arg;                     /* X10           */
    *(--stk) = (uint32_t)0x09090909L;             /* X9            */
    *(--stk) = (uint32_t)0x08080808L;             /* X8            */
    *(--stk) = (uint32_t)0x07070707L;             /* X7            */
    *(--stk) = (uint32_t)0x06060606L;             /* X6            */
    *(--stk) = (uint32_t)0x05050505L;             /* X5            */
    *(--stk) = (uint32_t)0x04040404L;             /* X4            */
    *(--stk) = (uint32_t)gp;                      /* X3            */
    *(--stk) = (uint32_t)krhino_task_deathbed;    /* X1            */

    return stk;
}

