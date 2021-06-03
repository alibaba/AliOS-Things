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
 * aliyun_iot_smartcard_internal.h: export file
 *
 */
#ifndef __ALIYUN_IOT_SMARTCARD_INTERNAL_H__
#define __ALIYUN_IOT_SMARTCARD_INTERNAL_H__

#include <stdint.h>
#include <stdbool.h>

#include "mn_obfus.h"
#include "aliyun_iot_smartcard_hal.h"

/* value MUST be same as aliyun_iot_smartcard_type_t (logical slot) */
typedef enum {
    SIM_NULL = 0,
    SIM1 = 1, /*SIM1 means CHINA_MOBILE*/
    SIM2, /*SIM2 means CHINA_UNICOM*/
    SIM3 /*SIM3 means CHINA_TELECOM*/
} aliyun_iot_smartcard_slot_num_t;

typedef enum {
    ALI_MN_SIM_SWITCHING = 1, /* switch idle */
    ALI_MN_SIM_SWITCH_DONE ,      /* switching */
    ALI_MN_SIM_SWITCH_FAILED ,    /* switch done */
    ALI_MN_SIM_SWITCH_SLIENT    /* no sim available(slient state) */
} ali_mn_sim_state_et;


typedef struct {
    aliyun_iot_smartcard_slot_num_t slot_num;       /**< slot type: SIM1 / SIM2 / SIM3 */
    char iccid[32 + 1];                           /**< iccid of selected slot */
    char car_name[8 + 1];                         /**< carrier name */
    aliyun_iot_smartcard_carrier_type_t car_type;   /**< carrier type derived from iccid */
    bool active;                                  /**< current slot is active or not */
    aliyun_iot_smartcard_act_t act;                 /**< access technology selected */
    int32_t rssi;                                 /**< rssi val */
    uint16_t delay;                               /**< delay in ms */
    uint8_t loss_rate;                            /**< ping loss rate in percent */
    uint8_t score;                                /**< score */
} aliyun_iot_smartcard_sim_t;

/*!
 * \brief   get current selected card slot number
 *
 * \param   [OUT] slot_num: current card slot number, 1 is SIM1, 2 is SIM2, 3 is SIM3
 *
 * \retval  Possible returns are:
 *          -1: get failed.
 *           0: set succesful.
 */
extern int aliyun_iot_smartcard_get_card_slot(aliyun_iot_smartcard_slot_num_t *slot_num);
#endif /* __ALIYUN_IOT_SMARTCARD_INTRENAL_H__ */
