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
 * api_hal_dummy_impl.c: module hal dummy impl
 * need to be impl by user
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "aliyun_iot_smartcard_hal.h"
#include "aliyun_iot_smartcard_os_hal.h"
#include "mn_common.h"
#include "aos_smartcard.h"

int aliyun_iot_smartcard_hal_init(void)
{
    return 0;
}

int aliyun_iot_smartcard_hal_deinit(void)
{
    return 0;
}

int aliyun_iot_smartcard_hal_get_rssi(int32_t *rssi)
{
    return aos_smartcard_get_rssi(rssi);
}

int aliyun_iot_smartcard_hal_get_iccid(char *iccid, int len)
{
    return aos_smartcard_get_iccid(iccid, len);
}

int aliyun_iot_smartcard_hal_get_imei(char *imei, int len)
{
    return aos_smartcard_get_imei(imei, len);
}

int aliyun_iot_smartcard_hal_get_act(aliyun_iot_smartcard_act_t *act)
{
    aos_smartcard_act_t smartcard_act = AOS_SMARTCARD_ACT_NULL;
    int ret;

    ret = aos_smartcard_get_act(&smartcard_act);
    if (ret) {
        return -1;
    }

    switch (smartcard_act) {
        case AOS_SMARTCARD_ACT_NULL:
            *act = ACT_NULL;
            break;
        case AOS_SMARTCARD_ACT_2G:
            *act = ACT_2G;
            break;
        case AOS_SMARTCARD_ACT_3G:
            *act = ACT_3G;
            break;
        case AOS_SMARTCARD_ACT_4G:
            *act = ACT_4G;
            break;
        default:
            return -1;
    }
    return 0;
}

int aliyun_iot_smartcard_hal_get_carrier(aliyun_iot_smartcard_carrier_type_t *car_type)
{
    int ret;
    char iccid[32] = {0};

    ret = aliyun_iot_smartcard_hal_get_iccid(iccid, 32);
    if (ret != 0) {
        return -1;
    }

    ret = mn_parse_carrier_from_iccid(iccid, car_type, NULL);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int aliyun_iot_smartcard_hal_reset_module(void)
{
    return aos_smartcard_reset_module();
}

int aliyun_iot_smartcard_hal_cpbw(int slot, char *number)
{
    return aos_smartcard_cpbw(slot, number);
}

int aliyun_iot_smartcard_hal_cpbs(void)
{
    return aos_smartcard_cpbs();
}

int aliyun_iot_smartcard_hal_cpbr(int slot, char *name, int name_length, char *number, int number_length)
{
    memset(name, '\0', name_length);
    memset(number, '\0', number_length);

    return aos_smartcard_cpbr(slot, name, name_length, number, number_length);
}
