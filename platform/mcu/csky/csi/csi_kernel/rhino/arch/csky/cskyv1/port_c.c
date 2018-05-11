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

    temp &= 0xfffffff8;

    stk = (cpu_stack_t *)temp;

    *(--stk) = (uint32_t)entry;                 /* entry point                          */
    *(--stk) = (uint32_t)0x80000140L;           /* PSR                                  */
    *(--stk) = (uint32_t)krhino_task_deathbed;  /* R15 (LR)                             */
    *(--stk) = (uint32_t)0x14141414L;           /* R14                                  */
    *(--stk) = (uint32_t)0x13131313L;           /* R13                                  */
    *(--stk) = (uint32_t)0x12121212L;           /* R12                                  */
    *(--stk) = (uint32_t)0x11111111L;           /* R11                                  */
    *(--stk) = (uint32_t)0x10101010L;           /* R10                                  */
    *(--stk) = (uint32_t)0x09090909L;           /* R9                                   */
    *(--stk) = (uint32_t)0x00000000L;           /* R8                                   */
    *(--stk) = (uint32_t)0x07070707L;           /* R7                                   */
    *(--stk) = (uint32_t)0x06060606L;           /* R6                                   */
    *(--stk) = (uint32_t)0x05050505L;           /* R5                                   */
    *(--stk) = (uint32_t)0x04040404L;           /* R4                                   */
    *(--stk) = (uint32_t)0x03030303L;           /* R3                                   */
    *(--stk) = (uint32_t)arg;                   /* R2 : argument                        */
    *(--stk) = (uint32_t)0x01010101L;           /* R1                                   */
    *(--stk) = (uint32_t)0x88888888L;           /* REG_HI                               */
    *(--stk) = (uint32_t)0x99999999L;           /* REG_LO                               */
#ifdef __CSKY_HARD_FLOAT__
    *(--stk) = (uint32_t)0x1234568fL;           /* FR31                                 */
    *(--stk) = (uint32_t)0x12345685L;           /* FR30                                 */
    *(--stk) = (uint32_t)0x1234568eL;           /* FR29                                 */
    *(--stk) = (uint32_t)0x1234568dL;           /* FR28                                 */
    *(--stk) = (uint32_t)0x1234568cL;           /* FR27                                 */
    *(--stk) = (uint32_t)0x1234568bL;           /* FR26                                 */
    *(--stk) = (uint32_t)0x1234568aL;           /* FR25                                 */
    *(--stk) = (uint32_t)0x12345689L;           /* FR24                                 */
    *(--stk) = (uint32_t)0x12345688L;           /* FR23                                 */
    *(--stk) = (uint32_t)0x12345687L;           /* FR22                                 */
    *(--stk) = (uint32_t)0x12345686L;           /* FR21                                 */
    *(--stk) = (uint32_t)0x12345684L;           /* FR20                                 */
    *(--stk) = (uint32_t)0x12345683L;           /* FR19                                 */
    *(--stk) = (uint32_t)0x12345682L;           /* FR18                                 */
    *(--stk) = (uint32_t)0x12345681L;           /* FR17                                 */
    *(--stk) = (uint32_t)0x12345680L;           /* FR16                                 */
    *(--stk) = (uint32_t)0x1234567fL;           /* FR15                                 */
    *(--stk) = (uint32_t)0x1234567eL;           /* FR14                                 */
    *(--stk) = (uint32_t)0x1234567dL;           /* FR13                                 */
    *(--stk) = (uint32_t)0x1234567cL;           /* FR12                                 */
    *(--stk) = (uint32_t)0x1234567bL;           /* FR11                                 */
    *(--stk) = (uint32_t)0x1234567aL;           /* FR10                                 */
    *(--stk) = (uint32_t)0x12345679L;           /* FR9                                  */
    *(--stk) = (uint32_t)0x12345678L;           /* FR8                                  */
    *(--stk) = (uint32_t)0x12345677L;           /* FR7                                  */
    *(--stk) = (uint32_t)0x12345676L;           /* FR6                                  */
    *(--stk) = (uint32_t)0x12345675L;           /* FR5                                  */
    *(--stk) = (uint32_t)0x12345674L;           /* FR4                                  */
    *(--stk) = (uint32_t)0x12345673L;           /* FR3                                  */
    *(--stk) = (uint32_t)0x12345672L;           /* FR2                                  */
    *(--stk) = (uint32_t)0x12345671L;           /* FR1                                  */
    *(--stk) = (uint32_t)0x12345670L;           /* FR0                                  */
#endif

    return stk;
}

