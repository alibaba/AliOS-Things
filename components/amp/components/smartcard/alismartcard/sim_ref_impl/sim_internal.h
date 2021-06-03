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
 * sim.h: sim card api
 *
 */

#ifndef __SIM_INTERNAL_H__
#define __SIM_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "aliyun_iot_smartcard_export.h"

typedef struct {
    int (*sim_card_init)();
    int (*sim_switch_to_next_card)(aliyun_iot_smartcard_type_t switch_type);
    int (*sim_get_cur_slot)(aliyun_iot_smartcard_slot_num_t *slot_num);
    int (*sim_is_switch_support)(bool *support);
    int (*sim_get_slot_cnt)(uint8_t *cnt);
} sim_callbacks_st;

/**
 * @brief get simhub func cb
 *
 * @param [out] cb: point to simhub func cb
 * @return  0 on success, negative error on failure.
 * @see None.
 * @note None.
 */
int sim_get_simhub_cb(sim_callbacks_st **cb);

/**
 * @brief get common sim func cb
 *
 * @param [out] cb: point to simhub func cb
 * @return  0 on success, negative error on failure.
 * @see None.
 * @note None.
 */
int sim_get_common_sim_cb(sim_callbacks_st **cb);

/**
 * @brief get alibaba triple sim func cb
 *
 * @param [out] cb: point to simhub func cb
 * @return  0 on success, negative error on failure.
 * @see None.
 * @note None.
 */
int sim_get_alicard_cb(sim_callbacks_st **cb);

#ifdef __cplusplus
}
#endif

#endif
