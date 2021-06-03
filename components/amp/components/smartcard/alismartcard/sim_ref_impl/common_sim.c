/*
 * Copyright (c) 2019 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description:
 * common_sim.c: common sim (not multi-number sim card) impl
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aliyun_iot_smartcard_export.h"
#include "aliyun_iot_smartcard_hal.h"
#include "mn_common.h"
#include "sim.h"
#include "sim_internal.h"

static int common_sim_switch_to_next_card(aliyun_iot_smartcard_type_t switch_type)
{
    return 0;
}

static int common_sim_get_cur_slot(aliyun_iot_smartcard_slot_num_t *slot_num)
{
    int ret;
    aliyun_iot_smartcard_carrier_type_t car_type;

    ret = aliyun_iot_smartcard_hal_get_carrier(&car_type);

    if (ret != 0) {
        mn_log_err("aliyun_iot_smartcard_hal_get_carrier fail!");
        return -1;
    }

    if (car_type == CHINA_MOBILE) {
        *slot_num = SIM1;
        return 0;
    } else if (car_type == CHINA_UNICOM) {
        *slot_num = SIM2;
        return 0;
    } else if (car_type == CHINA_TELECOM) {
        *slot_num = SIM3;
        return 0;
    }

    return -1;
}

static int common_sim_is_switch_support(bool *support)
{
    *support = false;
    return 0;
}

static int common_sim_get_slot_cnt(uint8_t *cnt)
{
    *cnt = 1;
    return 0;
}

static int common_sim_init()
{
    return 0;
}

static sim_callbacks_st common_sim_callback = {
    common_sim_init,
    common_sim_switch_to_next_card,
    common_sim_get_cur_slot,
    common_sim_is_switch_support,
    common_sim_get_slot_cnt
};

int sim_get_common_sim_cb(sim_callbacks_st **cb)
{
    *cb = &common_sim_callback;
    return 0;
}
