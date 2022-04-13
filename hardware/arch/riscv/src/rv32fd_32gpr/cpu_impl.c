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

#if (RHINO_CONFIG_STACK_OVF_CHECK_HW != 0)
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
