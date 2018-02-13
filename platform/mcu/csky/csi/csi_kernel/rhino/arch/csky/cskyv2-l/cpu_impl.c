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
#include <k_config.h>

#if (YUNOS_CONFIG_STACK_OVF_CHECK_HW != 0)
void cpu_intrpt_stack_protect(void)
{
}

void task_stack_crash_warning(void)
{
    printf("****The task stack base has been broken !!!****\n");
}

void cpu_task_stack_protect(cpu_stack_t *base, size_t size)
{
    uint32_t base_addr = (uint32_t)base;

    int num_return = wp_register(base_addr, AWATCH, task_stack_crash_warning);
    if (num_return == 1) {
        wp_enable(1);
    } else if (num_return == 2 || num_return == -1) {
          wp_unregister(1);
          int number_tmp = wp_register(base_addr, AWATCH, task_stack_crash_warning);
          if (number_tmp == 1){
              wp_enable(1);
          }
      }
}
#endif

#ifdef CONFIG_STACK_GUARD

void csky_set_stackbound(void)
{
    uint32_t     size;
    cpu_stack_t *base;

    ktask_t *get_task = (ktask_t *)g_active_task;

    base = get_task->task_stack_base;
    size = get_task->stack_size;

    asm volatile(
        "mfcr    r9, cr<0, 4>\n\r"
        "bclri   r9, 0\n\r"
        "mtcr    r9, cr<0, 4>\n\r"
        "mtcr    %0, cr<2, 4>\n\r"
        "lsli    %1, %1, 2\n\r"
        "add     %0, %0, %1\n\r"
        "mtcr    %0, cr<1, 4>\n\r"
        ::"r"(base), "r"(size):"r9");
}

int stack_guard_save(void)
{
    int value;

    asm volatile(
        "mfcr    %0, cr<0, 4>\n\t"
        "mov     r0, %0\n\t"
        "bclri   r0, 0\n\t"
        "mtcr    r0, cr<0, 4>\n\t"
        :"=r"(value)
        ::"r0");

    return value;
}

void stack_guard_restore(int value)
{
    asm volatile(
        "mtcr    %0, cr<0, 4>\n\t"
        ::"r"(value):
    );
}

#endif
