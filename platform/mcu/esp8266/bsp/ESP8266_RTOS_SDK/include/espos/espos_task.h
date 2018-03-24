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

#ifndef _ESPOS_TASK_H_
#define _ESPOS_TASK_H_

#include "espos_types.h"
#include "espos_errno.h"
#include "espos_time.h"

#ifdef __cplusplus
extern "C" {
#endif

/* task will run at any CPU */
#define ESPOS_TASK_NO_AFFINITY  -1

/* create task and start it automatically */
#define ESPOS_TASK_CREATE_NORMAL 0
/* just create task and don't start it */
#define ESPOS_TASK_CREATE_SILENCE 1

/* task delete itself */
#define ESPOS_TASK_DELETE_SELF 0

/*
 * we should use a platform API instead of 'marco'
 */
#define ESPOS_TASK_PRIO_NUM espos_task_prio_num()

/* task maximum priority number */
#define ESPOS_TASK_PRIO_MAX (ESPOS_TASK_PRIO_NUM - 1)

/**
 * @brief create a task
 *
 * @param task  task handle point
 * @param name  task name, if name is NULL, we will use "default_task" default
 * @param arg   task entry function inputting parameter
 * @param prio  task priority
 * @param ticks task time slice
 * @param stack_size task stack size, its unit is "byte"
 * @param entry task entry function
 * @param opt   task option
 *               ESPOS_TASK_CREATE_NORMAL : the created task will be started automatically
 *               ESPOS_TASK_CREATE_SILENCE : the created task will not be started automatically
 * @param cpu_id task CPU id
 *               natural number : the task only runs at the CPU of the number
 *               ESPOS_TASK_NO_AFFINITY : the task runs at any CPU
 *
 * @return the result
 *             0 : successful
 *       -ENOMEM : no enough memory
 *       -EINVAL : input parameter error
 *       -EINTR  : you do this at interrupt state, and it is not supported
 */
esp_err_t espos_task_create_on_cpu(espos_task_t *task, const char *name, void *arg, espos_prio_t prio,
                                     espos_tick_t ticks, espos_size_t stack_size, espos_task_entry_t entry, espos_opt_t opt, espos_cpu_t cpu_id);


/**
 * @brief create a task and set its CPU id to be "ESPOS_TASK_NO_AFFINITY"
 *
 * @param task  task handle point
 * @param name  task name, if name is NULL, we will use "default_task" default
 * @param arg   task entry function inputting parameter
 * @param prio  task priority
 * @param ticks task time slice
 * @param stack_size task stack size, its unit is "byte"
 * @param entry task entry function
 * @param opt   task option
 *               ESPOS_TASK_CREATE_NORMAL : the created task will be started automatically
 *               ESPOS_TASK_CREATE_SILENCE : the created task will not be started automatically
 *
 * @return the result
 *             0 : successful
 *       -ENOMEM : no enough memory
 *       -EINVAL : input parameter error
 *       -EINTR  : you do this at interrupt state, and it is not supported
 */
#define espos_task_create(task, name, arg, prio, ticks, stack_size, entry, opt)   \
            espos_task_create_on_cpu(task, name, arg, prio, ticks, stack_size, entry, opt, ESPOS_TASK_NO_AFFINITY)

/**
 * @brief delete a task
 *
 * @param task task handle
 *               ESPOS_TASK_DELETE_SELF : task will delete itself
 *
 * @return the result
 *             0 : successful
 *       -EACCES : failed to do it with some special reason
 */
esp_err_t espos_task_del(espos_task_t task);

/**
 * @brief let current task sleep for some ticks
 *
 * @param delay_ticks system ticks
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 */
esp_err_t espos_task_delay(const espos_tick_t delay_ticks);

/**
 * @brief suspend target task
 *
 * @param task task handle
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 */
esp_err_t espos_task_suspend(espos_task_t task);

/**
 * @brief resume target task
 *
 * @param task task handle
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 */
esp_err_t espos_task_resume(espos_task_t task);

/**
 * @brief yield the cpu once
 *
 * @return the result
 *             0 : successful
 *       -EACCES : failed to do it with some special reason
 */
esp_err_t espos_task_yield(void);

/**
 * @brief get current task handle
 *
 * @return current task handle
 */
espos_task_t espos_task_get_current(void);

/**
 * @brief get current task name point
 *
 * @param task task handle
 * @param pname pointing at name's point
 *
 * @return current task handle
 */
esp_err_t espos_task_get_name (espos_task_t task, char **pname);

/**
 * @brief set task private data
 *
 * @param task task handle
 * @param idx  task data index
 * @param info task private data
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 */
esp_err_t espos_task_set_private_data(espos_task_t task, int idx, void *info);

/**
 * @brief get task private data
 *
 * @param task task handle
 * @param idx  task data index
 * @param info task private data point
 *
 * @return the result
 *             0 : successful
 *       -EINVAL : input parameter error
 */
esp_err_t espos_task_get_private_data(espos_task_t task, int idx, void **info);

/**
 * @brief get CPU affinity of task
 *
 * @return CPU affinity of task
 */
espos_cpu_t espos_task_get_affinity(espos_task_t task);

/**
 * @brief get ESPOS task priority number
 *
 * @return ESPOS task priority number
 */
espos_size_t espos_task_prio_num(void);

#ifdef __cplusplus
}
#endif

#endif /* _ESPOS_TASK_H_ */
