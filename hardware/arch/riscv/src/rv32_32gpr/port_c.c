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
#include <k_arch.h>

void *cpu_task_stack_init(cpu_stack_t *stack_base, size_t stack_size,
                          void *arg, task_entry_t entry)
{
    context_t *ctx, *ctx_prev;
    register int *gp asm("x3");
    ctx_prev = (context_t *)((unsigned long)(stack_base + stack_size) & ~0x0fUL);

    ctx = ctx_prev - 1;

    ctx->MSTATUS = (cpu_stack_t)SR_FS_INITIAL | SR_MPP_M | SR_MPIE; /* mstatus */
    ctx->PC = (cpu_stack_t)entry;
    ctx->X31 = (cpu_stack_t)0x31313131L;             /* X31           */
    ctx->X30 = (cpu_stack_t)0x30303030L;             /* X30           */
    ctx->X29 = (cpu_stack_t)0x29292929L;             /* X29           */
    ctx->X28 = (cpu_stack_t)0x28282828L;             /* X28           */
    ctx->X27 = (cpu_stack_t)0x27272727L;             /* X27           */
    ctx->X26 = (cpu_stack_t)0x26262626L;             /* X26           */
    ctx->X25 = (cpu_stack_t)0x25252525L;             /* X25           */
    ctx->X24 = (cpu_stack_t)0x24242424L;             /* X24           */
    ctx->X23 = (cpu_stack_t)0x23232323L;             /* X23           */
    ctx->X22 = (cpu_stack_t)0x22222222L;             /* X22           */
    ctx->X21 = (cpu_stack_t)0x21212121L;             /* X21           */
    ctx->X20 = (cpu_stack_t)0x20202020L;             /* X20           */
    ctx->X19 = (cpu_stack_t)0x19191919L;             /* X19           */
    ctx->X18 = (cpu_stack_t)0x18181818L;             /* X18           */
    ctx->X17 = (cpu_stack_t)0x17171717L;             /* X17           */
    ctx->X16 = (cpu_stack_t)0x16161616L;             /* X16           */
    ctx->X15 = (uint32_t)0x15151515L;             /* X15           */
    ctx->X14 = (uint32_t)0x14141414L;             /* X14           */
    ctx->X13 = (uint32_t)0x13131313L;             /* X13           */
    ctx->X12 = (uint32_t)0x12121212L;             /* X12           */
    ctx->X11 = (uint32_t)0x11111111L;             /* X11           */
    ctx->X10 = (uint32_t)arg;                     /* X10           */
    ctx->X9 = (uint32_t)0x09090909L;             /* X9            */
    ctx->X8 = (uint32_t)0x08080808L;             /* X8            */
    ctx->X7 = (uint32_t)0x07070707L;             /* X7            */
    ctx->X6 = (uint32_t)0x06060606L;             /* X6            */
    ctx->X5 = (uint32_t)0x05050505L;             /* X5            */
    ctx->X4 = (uint32_t)0x04040404L;             /* X4            */
    ctx->X3 = (uint32_t)gp;                      /* X3            */
    ctx->X2 = (uint32_t)ctx_prev;                      /* X3            */
    ctx->X1 = (uint32_t)krhino_task_deathbed;    /* X1            */

    return (void *)ctx;
}

