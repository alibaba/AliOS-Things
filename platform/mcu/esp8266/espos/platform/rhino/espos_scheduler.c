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

#include <stddef.h>
#include <stdint.h>

#include "k_api.h"
#include "espos_err.h"
#include "espos_scheduler.h"

static espos_size_t s_isr_nested_count[ESPOS_PROCESSORS_NUM];
static espos_size_t s_os_nested_count[ESPOS_PROCESSORS_NUM];
#if 1
static espos_size_t s_critial_count[ESPOS_PROCESSORS_NUM];
static espos_critical_t s_critial_status[ESPOS_PROCESSORS_NUM];
#endif

/**
 * @brief initialize ESPOS system
 */
esp_err_t espos_init(void)
{
    kstat_t ret;

    ret = krhino_init();

    return espos_err_map(ret);
}

/**
 * @brief start ESPOS system
 */
esp_err_t espos_start(void)
{
    kstat_t ret;

    ret = krhino_start();

    return espos_err_map(ret);
}

/**
 * @brief start ESPOS system CPU port
 */
esp_err_t espos_start_port(int port)
{
    return 0;
}

/**
 * @brief get ESPOS system state
 */
espos_stat_t espos_sched_state_get(void)
{
    if (g_sys_stat == RHINO_RUNNING) {
        return ESPOS_IS_RUNNING;
    }

    return ESPOS_IS_NOT_STARTED;
}

/**
 * @brief check if the CPU is at ISR state
 */
bool espos_in_isr(void)
{
    return (s_isr_nested_count[0] > 0) ? true : false;
}

/**
 * @brief check if the function is at real OS internal fucntion
 */
bool espos_os_isr(void)
{
    return (s_os_nested_count[0] > 0) ? true : false;
}

static inline uint32_t __set_int(void)
{
    uint32_t flags;

    __asm__ __volatile__(
            "rsil %0, 2\n"
            : "=a"(flags)
            :
            : "memory"
    );

    return flags;
}

static inline void __set_ps(uint32_t flags)
{
     __asm__ __volatile__(
            "wsr %0, ps\n"
            "esync"
            :
            : "a"(flags)
            : "memory"
    );
}

/**
 * @brief enter ESPOS system critical state
 */
espos_critical_t IRAM_ATTR _espos_enter_critical(espos_spinlock_t *spinlock)
{
    espos_critical_t tmp;

    tmp = (espos_critical_t)__set_int();
    if (!s_critial_count[espos_get_core_id()]) {
        s_critial_status[espos_get_core_id()] = tmp;
    }
    s_critial_count[espos_get_core_id()]++;

    return tmp;
}

/**
 * @brief exit ESPOS system critical state
 */
void IRAM_ATTR _espos_exit_critical(espos_spinlock_t *spinlock, espos_critical_t tmp)
{
    s_critial_count[espos_get_core_id()]--;
    if (!s_critial_count[espos_get_core_id()]) {
        tmp = s_critial_status[espos_get_core_id()];
        __set_ps(tmp);
    }
}


/**
* @brief suspend the preempt and the current task will not be preempted
*/
esp_err_t espos_preempt_suspend_local(void)
{
    kstat_t ret;

    if (g_sys_stat == RHINO_STOPPED) {
        return 0;
    }

    ret = krhino_sched_disable();

    return espos_err_map(ret);
}

/**
 * @brief resume the preempt
 */
esp_err_t espos_preempt_resume_local(void)
{
    kstat_t ret;

    if (g_sys_stat == RHINO_STOPPED) {
        return 0;
    }

    ret = krhino_sched_enable();

    return espos_err_map(ret);
}

/**
 * @brief enter system interrupt server, the function must be used after entering hardware interrupt
 */
esp_err_t espos_isr_enter (void)
{
    kstat_t ret;

    ret =  krhino_intrpt_enter();

    return espos_err_map(ret);
}

/**
 * @brief exit system interrupt server, the function must be used before exiting hardware interrupt
 */
void espos_isr_exit(void)
{
    krhino_intrpt_exit();
}

/**
 * @brief mark it enters real OS interal function
 */
esp_err_t espos_os_enter (void)
{
    s_os_nested_count[espos_get_core_id()]++;

    return 0;
}

/**
 * @brief remove mark it enters real OS interal function
 */
void espos_os_exit(void)
{
    if (!s_os_nested_count[espos_get_core_id()]) {
        return ;
    }

    s_os_nested_count[espos_get_core_id()]--;

    return ;
}
