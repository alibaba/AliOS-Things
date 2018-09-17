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

#include <assert.h>

#include "k_api.h"
#include "espos_err.h"
#include "espos_task.h"
#include "espos_scheduler.h"

/**
 * @brief create a task
 */
esp_err_t espos_task_create_on_cpu (
    espos_task_t *task,
    const char *name,
    void *arg,
    espos_prio_t prio,
    espos_tick_t ticks,
    espos_size_t stack_size,
    espos_task_entry_t entry,
    espos_opt_t opt,
    espos_cpu_t cpu_id
)
{
    kstat_t ret;
    ktask_t **ptask = (ktask_t **)task;
    uint8_t auto_run;
    extern void _cleanup_r(struct _reent* r);
    cpu_stack_t os_task_size;

    if (name == NULL) {
    	name = "default_task";
    }

    if (prio > ESPOS_TASK_PRIO_NUM)
        return espos_err_map(RHINO_BEYOND_MAX_PRI);

    if (ESPOS_TASK_CREATE_NORMAL == opt) {
        auto_run = 1;
    }
    else {
        auto_run = 0;
    }

#if defined(ESPOS_FOR_ESP32)
    os_task_size = stack_size / sizeof(cpu_stack_t);
#elif defined(ESPOS_FOR_ESP8266)
    os_task_size = stack_size;
#endif

    prio = RHINO_CONFIG_USER_PRI_MAX - prio;
    assert(prio > 10);
    
    if ( strcmp(name,"event_task") == 0 )
    {
        /* default event_task size is too small */
        os_task_size += 64;
    }
    
    ret = krhino_task_dyn_create(ptask, name, arg, prio, ticks,
                        os_task_size, entry, auto_run);

    /* The following code may open it later */
    #if 0
    if (ret == 0) {
        struct _reent *r;
        r = krhino_mm_alloc(sizeof(*r));
        esp_reent_init(r);
        ret = krhino_task_info_set(*ptask, 2, r);
    }
    #endif

    return espos_err_map(ret);
}

/**
 * @brief delete a task
 */
esp_err_t espos_task_del (
    espos_task_t task
)
{
    kstat_t ret;
    ktask_t *ptask = (ktask_t *)task;

    /* Be very careful here user_info[1] is used here */
    #if 0
    espos_sem_t *sem = ptask->user_info[1];
    if (sem && *sem) {
    	espos_sem_del(*sem);
    }
    if (sem){
        krhino_mm_free(sem);
    }
    #endif

    ret = krhino_task_dyn_del(ptask);

    return espos_err_map(ret);
}

/**
 * @brief let current task sleep for some ticks
 */
esp_err_t espos_task_delay (
    const espos_tick_t delay_ticks
)
{
    kstat_t ret;

    ret = krhino_task_sleep(delay_ticks);

    return espos_err_map(ret);
}

/**
 * @brief suspend target task
 */
esp_err_t espos_task_suspend (
    espos_task_t task
)
{
    kstat_t ret;
    ktask_t *ptask = (ktask_t *)task;

    ret = krhino_task_suspend(ptask);

    return espos_err_map(ret);
}

/**
 * @brief resume target task
 */
esp_err_t espos_task_resume (
    espos_task_t task
)
{
    kstat_t ret;
    ktask_t *ptask = (ktask_t *)task;

    ret = krhino_task_resume(ptask);

    return espos_err_map(ret);
}

/**
 * @brief yield the cpu once
 */
esp_err_t espos_task_yield(void)
{
    kstat_t ret;

    ret = krhino_task_yield();

    return espos_err_map(ret);
}

/**
 * @brief get current task handle
 */
espos_task_t espos_task_get_current(void)
{
    return (espos_task_t)krhino_cur_task_get();
}

/**
 * @brief get current task name point
 */
esp_err_t espos_task_get_name (
    espos_task_t task,
    char **pname
)
{
    ktask_t *ptask = (ktask_t *)task;

    *pname = (char *)ptask->task_name;

    return 0;
}

/**
 * @brief set task private data
 */
esp_err_t espos_task_set_private_data (
    espos_task_t task,
    int idx,
    void *info
)
{
    kstat_t ret;

    ktask_t *ptask = (ktask_t *)task;;
    ret = krhino_task_info_set(ptask, (size_t)idx, info);

    return espos_err_map(ret);
}

/**
 * @brief get task private data
 */
esp_err_t espos_task_get_private_data (
    espos_task_t task,
    int idx,
    void **info
)
{
    kstat_t ret;

    ktask_t *ptask = (ktask_t *)task;;
    ret = krhino_task_info_get(ptask, idx, info);

    return espos_err_map(ret);
}

/**
 * @brief get cpu affinity of task
 */
espos_cpu_t espos_task_get_affinity(espos_task_t task)
{
    return 0;
}

size_t espos_task_prio_num(void)
{
    return RHINO_CONFIG_USER_PRI_MAX + 1 - 10;
}

#if 0
struct _reent* __getreent()
{
    void *info;

    ktask_t *task = krhino_cur_task_get();
    if (task == NULL) {
        return _GLOBAL_REENT;
    }
    else {
        krhino_task_info_get(task, 2, &info);
        return info;
    }
}
#endif

#if defined(ESPOS_FOR_ESP32)
struct _reent* __getreent()
{
    ktask_t *task = krhino_cur_task_get();
    if (task == NULL) {
        return _GLOBAL_REENT;
    }
    else {
        return _GLOBAL_REENT;;
    }
}
#endif
