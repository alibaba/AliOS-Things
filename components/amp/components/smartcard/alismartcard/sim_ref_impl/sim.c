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
 * sim.c: sim card info impl
 *
 */

#include <stddef.h>
#include "mn_common.h"
#include "sim.h"
#include "sim_internal.h"
#include "aliyun_iot_smartcard_hal.h"

static sim_callbacks_st *current_sim = NULL;

/* keep all sim info */
#if 0
static aliyun_iot_smartcard_sim_t g_sim_info[MAX_SIM_SLOT_CNT];
#endif

static int sim_info_init(void)
{
#if 0
    int i;

    for (i = 0; i < MAX_SIM_SLOT_CNT; i++) {
        g_sim_info[i].slot_num = (aliyun_iot_smartcard_slot_num_t)(i + 1);
    }

#endif

    return 0;
}

int sim_init(void)
{
    sim_info_init();
    sim_get_alicard_cb(&current_sim);

    return current_sim->sim_card_init();

}

int sim_switch_to_next_card(aliyun_iot_smartcard_type_t switch_type)
{
    int ret;

    if (current_sim == NULL) {
        return -1;
    }

    ret = current_sim->sim_switch_to_next_card(switch_type);

    return ret;
}

int sim_get_cur_slot(aliyun_iot_smartcard_slot_num_t *slot_num)
{
    if (current_sim == NULL) {
        return -1;
    }

    return current_sim->sim_get_cur_slot(slot_num);
}

int sim_is_switch_support(bool *support)
{
    if (current_sim == NULL) {
        return -1;
    }

    return current_sim->sim_is_switch_support(support);
}

int sim_get_slot_cnt(uint8_t *cnt)
{
    if (current_sim == NULL) {
        return -1;
    }

    return current_sim->sim_get_slot_cnt(cnt);
}
