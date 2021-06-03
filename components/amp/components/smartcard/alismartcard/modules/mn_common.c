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
 * mn_common.c: common api impl
 *
 */
#include <errno.h>
#include <stdio.h>

#include "aliyun_iot_smartcard_export.h"
#include "aliyun_iot_smartcard_hal.h"
#include "mn_common.h"
#include "sim.h"
#include "module_common.h"

extern bool is_global_exit;

extern bool sim_checking;
extern char g_mn_ping_web[256];
extern uint16_t g_mn_once_ping_count;

static int _mn_switch_card(aliyun_iot_smartcard_type_t switch_type)
{
    int ret;

    if (switch_type >= SWITCH_MAX || switch_type <  SWITCH_TO_SIM1) {
        module_log_err("switch type %d error!", switch_type);
        return MN_ERROR_COMMON_PARAMETER;
    }

    module_log_info("### start to switch to next sim card......");

    ret = sim_switch_to_next_card(switch_type);

    if (ret != 0) {
        module_log_err("sim_switch_to_next_card fail!");
        return ret;
    }

    return 0;
}

int mn_hal_switch(aliyun_iot_smartcard_type_t switch_type)
{
    int ret;

    aliyun_iot_smartcard_slot_num_t cur_slot = SIM_NULL;
    aliyun_iot_smartcard_slot_num_t target_slot = SIM_NULL;
    aliyun_iot_smartcard_get_card_slot(&cur_slot);

    if (switch_type == SWITCH_TO_NEXT) {
        target_slot = (aliyun_iot_smartcard_slot_num_t)(cur_slot + 1);

        if (target_slot > SIM3) {
            target_slot = SIM1;
        }
    } else {
        target_slot = (aliyun_iot_smartcard_slot_num_t)switch_type;
    }

    ret = _mn_switch_card(switch_type);

    if (ret == MN_ERROR_GENERIC) {
        mn_log_err("switch_card fail!");
        return ret;
    }

    return ret;
}

int aliyun_iot_smartcard_get_card_slot(aliyun_iot_smartcard_slot_num_t *slot_num)
{
    return sim_get_cur_slot(slot_num);
}

typedef struct {
    const char *iccid_prefix;
    const char *carrier_name;
    aliyun_iot_smartcard_carrier_type_t car_type;
} carrier_iccid_t;

static carrier_iccid_t g_carrier_iccid[] = {
    { .iccid_prefix = "898600", .carrier_name = "CMCC", .car_type = CHINA_MOBILE },
    { .iccid_prefix = "898602", .carrier_name = "CMCC", .car_type = CHINA_MOBILE },
    { .iccid_prefix = "898604", .carrier_name = "CMCC", .car_type = CHINA_MOBILE },
    { .iccid_prefix = "898607", .carrier_name = "CMCC", .car_type = CHINA_MOBILE },
    { .iccid_prefix = "898601", .carrier_name = "CUCC", .car_type = CHINA_UNICOM },
    { .iccid_prefix = "898606", .carrier_name = "CUCC", .car_type = CHINA_UNICOM },
    { .iccid_prefix = "898609", .carrier_name = "CUCC", .car_type = CHINA_UNICOM },
    { .iccid_prefix = "898603", .carrier_name = "CTCC", .car_type = CHINA_TELECOM },
    { .iccid_prefix = "898611", .carrier_name = "CTCC", .car_type = CHINA_TELECOM }
};

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

int mn_parse_carrier_from_iccid(const char *iccid, aliyun_iot_smartcard_carrier_type_t *car_type, char *car_name)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(g_carrier_iccid); i++) {

        if (strstr(iccid, g_carrier_iccid[i].iccid_prefix) != NULL) {
            *car_type = g_carrier_iccid[i].car_type;

            if (car_name) {
                strncpy(car_name, g_carrier_iccid[i].carrier_name, 8);
            }

            return 0;
        }

    }

    return -1;
}

