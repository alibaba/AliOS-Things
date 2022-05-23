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
 * File: lightduer_pm_policy.h
 * Auth: Chang Li (changli@baidu.com)
 * Desc: PM Policy Head file
 */

#ifndef BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_PM_POLICY_H
#define BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_PM_POLICY_H

#include "lightduer_pm_dev.h"
#include "lightduer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum pm_policy {
    PM_POLICY_NONE,
    PM_POLICY_CPU_FREQ,
    PM_POLICY_SLEEP,
    PM_POLICY_POWER_OFF,
    PM_POLICY_MAX
} pm_policy_t;

typedef struct user_policy {
    enum _baidu_ca_bool_e  is_valid;
    duer_pm_dev_type_t type;
    duer_pm_dev_state_t state;
    uint32_t duration;
} user_policy_t;

typedef int (*pm_policy_func)(const duer_pm_device_list_t *dev_list, pm_policy_t *out_policy);

/**
 * Make decision according to current situation
 *
 * @param *dev_list : list of registered device
 * @param *out_policy : output Policy
 *
 * @return int : 0 for Success, others for failure.
 *
 */
int duer_pm_policy(const duer_pm_device_list_t *dev_list, pm_policy_t *out_policy);

/**
 * Check pm policy
 *
 * @param *dev_list : list of registered device
 * @param *out_policy : output Policy
 * @param handler: pm policy function
 *
 * @return int : 0 for Success, others for failure.
 *
 */
int duer_pm_check_policy(const duer_pm_device_list_t *dev_list, pm_policy_t *out_policy, pm_policy_func handler);

#ifdef __cplusplus
}
#endif

#endif // BAIDU_DUER_LIGHTDUER_INCLUDE_LIGHTDUER_PM_POLICY_H
