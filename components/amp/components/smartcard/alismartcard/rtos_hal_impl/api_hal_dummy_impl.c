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

static char g_local_iccid[32];

#define CMCC_ICCID "89860478102020004381"
#define CUCC_ICCID "89860620030000838475"
#define CTCC_ICCID "89860319239842422733"

int aliyun_iot_smartcard_hal_init(void)
{
    strncpy(g_local_iccid, CUCC_ICCID, 31);

    return 0;
}

int aliyun_iot_smartcard_hal_deinit(void)
{
    return 0;
}

int aliyun_iot_smartcard_hal_get_rssi(int32_t *rssi)
{
    *rssi = -50;
    return 0;
}

int aliyun_iot_smartcard_hal_get_iccid(char *iccid, int len)
{
    strncpy(iccid, g_local_iccid, len);

    return 0;
}

int aliyun_iot_smartcard_hal_get_imei(char *imei, int len)
{
    strncpy(imei, "86341204730076", len);

    return 0;
}

int aliyun_iot_smartcard_hal_get_act(aliyun_iot_smartcard_act_t *act)
{
    *act = ACT_4G;
    return 0;
}

/* carrier is got by parsing iccid in case of network registration fail and carrier can't be got by other way */
int aliyun_iot_smartcard_hal_get_carrier(aliyun_iot_smartcard_carrier_type_t *car_type)
{
    int ret;
    char iccid[32];

    /* display current ccid */
    ret = aliyun_iot_smartcard_hal_get_iccid(iccid, 32);

    if (ret != 0) {
        return -1;
    }

    if (strncmp(iccid, CMCC_ICCID, strlen(CMCC_ICCID)) == 0) {
        *car_type = CHINA_MOBILE;
    } else if (strncmp(iccid, CUCC_ICCID, strlen(CUCC_ICCID)) == 0) {
        *car_type = CHINA_UNICOM;
    } else if (strncmp(iccid, CTCC_ICCID, strlen(CTCC_ICCID)) == 0) {
        *car_type = CHINA_TELECOM;
    } else {
        return -1;
    }

    return 0;
}

int aliyun_iot_smartcard_hal_reset_module(void)
{
    return 0;
}

int aliyun_iot_smartcard_hal_cpbw(int slot, char *number)
{

    slot = slot - 1;

    if (slot ==  1) {
        strncpy(g_local_iccid, CUCC_ICCID, 31);
    } else if (slot == 2) {
        strncpy(g_local_iccid, CMCC_ICCID, 31);
    } else if (slot == 3) {
        strncpy(g_local_iccid, CTCC_ICCID, 31);
    }

    return 0;
}

int aliyun_iot_smartcard_hal_cpbs(void)
{
    return 0;
}

/* standard CPBR return value */
static int parse_cpbr_result(int slot, char *name, int name_length, char *number, int number_length)
{
    switch (slot) {
        case 1:
            strncpy(name, "ALIICCID", name_length - 1);
            strncpy(number, "88861020302000001234", number_length - 1);
            break;

        case 2:
            strncpy(name, "SIM1", name_length - 1);
            strncpy(number, CUCC_ICCID, number_length - 1);
            break;

        case 3:
            strncpy(name, "SIM2", name_length - 1);
            strncpy(number, CMCC_ICCID, number_length - 1);
            break;

        case 4:
            strncpy(name, "SIM3", name_length - 1);
            strncpy(number, CTCC_ICCID, number_length - 1);
            break;

        default:
            return -1;
    }

    return 0;

}

int aliyun_iot_smartcard_hal_cpbr(int slot, char *name, int name_length, char *number, int number_length)
{
    int ret = -1;

    if (slot > 4 || slot < 0 || !number || !name) {
        return ret;
    }

    memset(name, '\0', name_length);
    memset(number, '\0', number_length);

    ret = parse_cpbr_result(slot, name, name_length, number, number_length);

    return ret;
}
