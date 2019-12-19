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

#include <csi_config.h>
#include <stdlib.h>
#include <stdio.h>
#include <k_api.h>
#include <drv_timer.h>
#include "soc.h"
#include <csi_core.h>

#define THIS_MODULE MODULE_NONE

/* auto define heap size */
extern size_t __heap_start;
extern size_t __heap_end;

#if (RHINO_CONFIG_HW_COUNT > 0)
void soc_hw_timer_init(void)
{
}

hr_timer_t soc_hr_hw_cnt_get(void)
{
    return 0;
}

lr_timer_t soc_lr_hw_cnt_get(void)
{
    return 0;
}
#endif

/* auto define heap size */
k_mm_region_t g_mm_region[] = {
    {(uint8_t *)&__heap_start, (size_t)0},
};
int g_region_num  = sizeof(g_mm_region)/sizeof(k_mm_region_t);

uint32_t soc_get_cur_sp(void)
{
    uint32_t result;

    asm volatile("mov %0, sp" : "=r"(result));
    return (result);
}

void soc_err_proc(kstat_t err)
{
    printf("soc_err_proc: %s!\n", g_active_task[0]->task_name);
    while(1);
}

krhino_err_proc_t g_err_proc = soc_err_proc;

