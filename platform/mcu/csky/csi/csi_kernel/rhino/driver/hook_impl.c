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
#include <drv_timer.h>
#include <csi_config.h>

extern void soc_hw_timer_init(void);

#if (RHINO_CONFIG_USER_HOOK > 0)
void krhino_init_hook(void)
{
#if (RHINO_CONFIG_HW_COUNT > 0)
    soc_hw_timer_init();
#endif
}

void krhino_start_hook(void)
{

}

void krhino_task_create_hook(ktask_t *task)
{

}

void krhino_task_del_hook(ktask_t *task, res_free_t *arg)
{

}

void krhino_task_abort_hook(ktask_t *task)
{

}

void krhino_task_switch_hook(ktask_t *orgin, ktask_t *dest)
{

}

#if defined(CONFIG_LPM_TICKLESS_SLEEP)
volatile uint32_t g_prev_cnt = 0xffffffff;
#endif
void krhino_tick_hook(void)
{
#if defined(CONFIG_LPM_TICKLESS_SLEEP)
    extern timer_handle_t count_timer;
    csi_timer_get_current_value(count_timer, (uint32_t *)&g_prev_cnt);
#endif
}

void krhino_idle_hook(void)
{
#if defined(CONFIG_LPM_TICKLESS_SLEEP) || defined(CONFIG_LPM_HALT)
    extern void lpm_idle_hook(void);
    lpm_idle_hook();
#endif
}

void krhino_intrpt_hook(int irq)
{

}

void krhino_mm_alloc_hook(void *mem, size_t size)
{

}

#endif

void krhino_intrpt_enter_hook(int irq)
{

}

void krhino_intrpt_exit_hook(int irq)
{

}


