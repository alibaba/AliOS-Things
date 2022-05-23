/**
 * Copyright (2017) Baidu Inc. All rights reserved.
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
/*
 * File: lightduer_pm_manager.c
 * Auth: Chang Li (changli@baidu.com)
 * Desc: PM manager implementation.
 */

#ifndef BAIDU_DUER_LIGHTDUER_PM_MANAGER_H
#define BAIDU_DUER_LIGHTDUER_PM_MANAGER_H

#include "lightduer_notifier.h"
#include "lightduer_pm_policy.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PM_SYSTEM_ENTER,
    PM_SYSTEM_EXIT
} pm_system_handle_action_t;

typedef int (*pm_system_handler_func)(pm_system_handle_action_t action, pm_policy_t policy,
        uint32_t timer_sec, int *data);

/**
 * Register notifier for observer use
 *
 * @param notifier: duer_notifier_block_t object
 *
 * @return int : 0 for Success, others for failure
 *
 */
int duer_pm_register_notifier(duer_notifier_block_t *notifier);

/**
 * Power Manager initialization
 *
 * @return int : 0 for Success, others for failure
 *
 */
int duer_pm_init(pm_system_handler_func hdlr);

/**
 * Power Manager main logic
 *
 * @return int : 0 for Success, others for failure
 *
 */
int duer_pm_manager();

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIGHTDUER_PM_MANAGER_H



