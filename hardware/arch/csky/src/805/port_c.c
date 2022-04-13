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

    temp &= 0xFFFFFFFCUL;

    stk = (cpu_stack_t *)temp;

    *(--stk) = (uint32_t)entry;                 /* Entry Point       */
    *(--stk) = (uint32_t)0x80000340L;           /* PSR               */
    *(--stk) = (uint32_t)krhino_task_deathbed;  /* R15 (LR)          */
    *(--stk) = (uint32_t)0x13131313L;           /* R13               */
    *(--stk) = (uint32_t)0x12121212L;           /* R12               */
    *(--stk) = (uint32_t)0x11111111L;           /* R11               */
    *(--stk) = (uint32_t)0x10101010L;           /* R10               */
    *(--stk) = (uint32_t)0x09090909L;           /* R9                */
    *(--stk) = (uint32_t)0x00000000L;           /* R8                */
    *(--stk) = (uint32_t)0x07070707L;           /* R7                */
    *(--stk) = (uint32_t)0x06060606L;           /* R6                */
    *(--stk) = (uint32_t)0x05050505L;           /* R5                */
    *(--stk) = (uint32_t)0x04040404L;           /* R4                */
    *(--stk) = (uint32_t)0x03030303L;           /* R3                */
    *(--stk) = (uint32_t)0x02020202L;           /* R2                */
    *(--stk) = (uint32_t)0x01010101L;           /* R1                */
    *(--stk) = (uint32_t)arg;                   /* R0 : argument     */

    *(--stk) = (uint32_t)0x31313131L;           /* R31               */
    *(--stk) = (uint32_t)0x30303030L;           /* R30               */
    *(--stk) = (uint32_t)0x29292929L;           /* R29               */
    *(--stk) = (uint32_t)0x28282828L;           /* R28               */
    *(--stk) = (uint32_t)0x27272727L;           /* R27               */
    *(--stk) = (uint32_t)0x26262626L;           /* R26               */
    *(--stk) = (uint32_t)0x25252525L;           /* R25               */
    *(--stk) = (uint32_t)0x24242424L;           /* R24               */
    *(--stk) = (uint32_t)0x23232323L;           /* R23               */
    *(--stk) = (uint32_t)0x22222222L;           /* R22               */
    *(--stk) = (uint32_t)0x21212121L;           /* R21               */
    *(--stk) = (uint32_t)0x20202020L;           /* R20               */
    *(--stk) = (uint32_t)0x19191919L;           /* R19               */
    *(--stk) = (uint32_t)0x18181818L;           /* R18               */
    *(--stk) = (uint32_t)0x17171717L;           /* R17               */
    *(--stk) = (uint32_t)0x16161616L;           /* R16               */

    *(--stk) = (uint32_t)0x12345678L;           /* VR15 (Bits 96-127) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR15 (Bits 64-95 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR15 (Bits 32-63 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR15 (Bits  0-31 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR14 (Bits 96-127) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR14 (Bits 64-95 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR14 (Bits 32-63 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR14 (Bits  0-31 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR13 (Bits 96-127) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR13 (Bits 64-95 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR13 (Bits 32-63 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR13 (Bits  0-31 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR12 (Bits 96-127) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR12 (Bits 64-95 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR12 (Bits 32-63 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR12 (Bits  0-31 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR11 (Bits 96-127) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR11 (Bits 64-95 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR11 (Bits 32-63 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR11 (Bits  0-31 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR10 (Bits 96-127) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR10 (Bits 64-95 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR10 (Bits 32-63 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR10 (Bits  0-31 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR9 (Bits 96-127) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR9 (Bits 64-95 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR9 (Bits 32-63 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR9 (Bits  0-31 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR8 (Bits 96-127) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR8 (Bits 64-95 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR8 (Bits 32-63 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR8 (Bits  0-31 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR7 (Bits 96-127) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR7 (Bits 64-95 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR7 (Bits 32-63 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR7 (Bits  0-31 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR6 (Bits 96-127) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR6 (Bits 64-95 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR6 (Bits 32-63 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR6 (Bits  0-31 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR5 (Bits 96-127) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR5 (Bits 64-95 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR5 (Bits 32-63 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR5 (Bits  0-31 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR4 (Bits 96-127) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR4 (Bits 64-95 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR4 (Bits 32-63 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR4 (Bits  0-31 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR3 (Bits 96-127) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR3 (Bits 64-95 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR3 (Bits 32-63 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR3 (Bits  0-31 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR2 (Bits 96-127) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR2 (Bits 64-95 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR2 (Bits 32-63 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR2 (Bits  0-31 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR1 (Bits 96-127) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR1 (Bits 64-95 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR1 (Bits 32-63 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR1 (Bits  0-31 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR0 (Bits 96-127) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR0 (Bits 64-95 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR0 (Bits 32-63 ) */
    *(--stk) = (uint32_t)0x12345678L;           /* VR0 (Bits  0-31 ) */

    return stk;
}

