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

extern void soc_hw_timer_init(void);

#if (YUNOS_CONFIG_USER_HOOK > 0)
void yunos_init_hook(void)
{
#if (YUNOS_CONFIG_HW_COUNT > 0)
    soc_hw_timer_init();
#endif
}

void krhino_idle_pre_hook(void)
{

}

void yunos_start_hook(void)
{

}

void yunos_task_create_hook(ktask_t *task)
{

}

void yunos_task_del_hook(ktask_t *task, res_free_t *arg)
{

}

void yunos_task_abort_hook(ktask_t *task)
{

}

void yunos_task_switch_hook(ktask_t *orgin, ktask_t *dest)
{

}

#if defined(CONFIG_LPM_TICKLESS_CNTTIM) && defined(CONFIG_LPM_TICKLESS_SLEEP)
volatile uint32_t g_prev_cnt = 0xffffffff;
#endif
void yunos_tick_hook(void)
{
#if defined(CONFIG_LPM_TICKLESS_CNTTIM) && defined(CONFIG_LPM_TICKLESS_SLEEP)
    extern timer_handle_t count_timer;
    csi_timer_get_current_value(count_timer, &g_prev_cnt);
#endif
}

extern void yoc_idle_hook(void);
void yunos_idle_hook(void)
{
#if defined(CONFIG_LPM_HALT) || defined(CONFIG_LPM_SLEEP)
    yoc_idle_hook();
#endif
}

void yunos_intrpt_hook(int irq)
{

}
#endif

void yunos_intrpt_enter_hook(int irq)
{

}

void yunos_intrpt_exit_hook(int irq)
{

}


