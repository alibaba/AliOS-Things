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

#ifndef __SIM_H__
#define __SIM_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_SIM_SLOT_CNT (3)

#include "aliyun_iot_smartcard_internal.h"

/**
 * @brief switch to next card
 *
 * @param   [IN] type: switch type
 *
 * @return  Possible returns are:
 *          MN_ERROR_SAME_CARD: card is same after switch.
 *          -1: other fail.
 *           0: set succesful.
 * @see None.
 * @note None.
 */
int sim_switch_to_next_card(aliyun_iot_smartcard_type_t switch_type);

/*!
 * \brief   get current selected card slot number
 *
 * \param   [OUT] slot_num: current card slot number
 *
 * \retval  Possible returns are:
 *          -1: get failed.
 *           0: set succesful.
 * @see None.
 */
int sim_get_cur_slot(aliyun_iot_smartcard_slot_num_t *slot_num);

/*!
 * \brief   check if sim card support card switch or not
 *
 * \param   [OUT] support: switch support or not
 *
 * \retval  Possible returns are:
 *          -1: failed.
 *           0: succesful.
 * @see None.
 */
int sim_is_switch_support(bool *support);

/*!
 * \brief   get sim card total slot cnt
 *
 * \param   [OUT] cnt: current sim card support max cnt
 *
 * \retval  Possible returns are:
 *          -1: failed.
 *           0: succesful.
 * @see None.
 */
int sim_get_slot_cnt(uint8_t *cnt);

/*!
 * \brief   sim initialization
 *
 * \retval  Possible returns are:
 *          -1: failed.
 *           0: succesful.
 * @see None.
 */
int sim_init(void);
#ifdef __cplusplus
}
#endif

#endif
