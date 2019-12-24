// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "k_api.h"
#include "espos_err.h"
#include "espos_time.h"

/**
 * @brief get current system ticks
 */
espos_tick_t espos_get_tick_count(void)
{
    return (espos_tick_t)krhino_sys_tick_get();
}

/**
 * @brief transform milliseconds to system ticks
 */
espos_tick_t espos_ms_to_ticks(espos_time_t ms)
{
     return (ms / (1000 / RHINO_CONFIG_TICKS_PER_SECOND));
}

/**
 * @brief transform system ticks to milliseconds
 */
espos_time_t espos_ticks_to_ms(espos_tick_t ticks)
{
    return (ticks * (1000 / RHINO_CONFIG_TICKS_PER_SECOND));
}

void espos_add_tick_count(size_t t)
{
    g_tick_count += (tick_t)t;
}

size_t espos_get_expected_idle_time(void)
{
    ktask_t *task = krhino_cur_task_get();

    if (!strcmp(task->task_name, "idle_task")) {
        return task->time_slice;
    }

    return 0;
}
