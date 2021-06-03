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
 * at_hal_dummy_impl.c: module hal dummy impl
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

typedef enum {
    TYPE_AT_CSQ = 0,
    TYPE_AT_ICCID,
    TYPE_AT_CIMI,
    TYPE_AT_GSN,
    TYPE_AT_COPS,
    TYPE_AT_CFUN,
    TYPE_AT_CPBS,
    TYPE_AT_CSCS,
    TYPE_AT_CPBR1,
    TYPE_AT_CPBR2,
    TYPE_AT_CPBR3,
    TYPE_AT_CPBR4,
    TYPE_AT_CPBW2,
    TYPE_AT_CPBW3,
    TYPE_AT_CPBW4
} at_type_t;

typedef struct {
    char *cmd;
    at_type_t type;
} at_cmd_t;

static at_cmd_t g_at_table[] = {
    {"AT+CSQ",     TYPE_AT_CSQ},
    {"AT+ICCID",   TYPE_AT_ICCID},
    {"AT+CIMI",    TYPE_AT_CIMI},
    {"AT+GSN",     TYPE_AT_GSN},
    {"AT+COPS",    TYPE_AT_COPS},
    {"AT+CFUN",    TYPE_AT_CFUN},
    {"AT+CPBS",    TYPE_AT_CPBS},
    {"AT+CSCS",    TYPE_AT_CSCS},
    {"AT+CPBR=1",  TYPE_AT_CPBR1},
    {"AT+CPBR=2",  TYPE_AT_CPBR2},
    {"AT+CPBR=3",  TYPE_AT_CPBR3},
    {"AT+CPBR=4",  TYPE_AT_CPBR4},
    {"AT+CPBW=2",  TYPE_AT_CPBW2},
    {"AT+CPBW=3",  TYPE_AT_CPBW3},
    {"AT+CPBW=4",  TYPE_AT_CPBW4},
};

#define AT_TABLE_SIZE	(sizeof(g_at_table) / sizeof(at_cmd_t))

int aliyun_iot_smartcard_hal_send_at(const char *at_cmd, const char *final_rsp, char *str_resp, int resp_len, long timeout_ms)
{
    int i;
    at_type_t type;

    if (strlen(g_local_iccid) == 0) {
        strncpy(g_local_iccid, CUCC_ICCID, 31);
    }

    memset(str_resp, '\0', resp_len);

    for (i = 0; i < AT_TABLE_SIZE; i++) {
        int cmd_len = strlen(g_at_table[i].cmd);

        if (!strncmp((const char *)at_cmd, g_at_table[i].cmd, cmd_len)) {
            type = g_at_table[i].type;
            break;
        }
    }

    if (i >= AT_TABLE_SIZE) {
        return -1;
    }


    switch (type) {
        case TYPE_AT_CSQ:
            snprintf(str_resp, resp_len, "%s", "+CSQ: 13,99\n");
            break;

        case TYPE_AT_ICCID:
            snprintf(str_resp, resp_len, "+ICCID: %s\n", g_local_iccid);
            break;

        case TYPE_AT_CIMI:
            snprintf(str_resp, resp_len, "%s\n", "460049860000866");
            break;

        case TYPE_AT_GSN:
            snprintf(str_resp, resp_len, "%s\n", "862810040555251");
            break;

        case TYPE_AT_COPS:
            snprintf(str_resp, resp_len, "%s\n", "+COPS: 0,0,\"CHINA MOBILE\",7");
            break;

        case TYPE_AT_CFUN:
            break;

        case TYPE_AT_CSCS:
            break;

        case TYPE_AT_CPBS:
            break;

        case TYPE_AT_CPBR1:
            snprintf(str_resp, resp_len, "%s\n", "+CPBR: 1,\"88861020302000001234\",129,\"ALIICCID\"");
            break;

        case TYPE_AT_CPBR2:
            snprintf(str_resp, resp_len, "+CPBR: 2,\"%s\",129,\"SIM1\"\n", CUCC_ICCID);
            break;

        case TYPE_AT_CPBR3:
            snprintf(str_resp, resp_len, "+CPBR: 3,\"%s\",129,\"SIM2\"\n", CMCC_ICCID);
            break;

        case TYPE_AT_CPBR4:
            snprintf(str_resp, resp_len, "+CPBR: 4,\"%s\",129,\"SIM3\"\n", CTCC_ICCID);
            break;

        case TYPE_AT_CPBW2:
            strncpy(g_local_iccid, CUCC_ICCID, 31);
            break;

        case TYPE_AT_CPBW3:
            strncpy(g_local_iccid, CMCC_ICCID, 31);
            break;

        case TYPE_AT_CPBW4:
            strncpy(g_local_iccid, CTCC_ICCID, 31);
            break;

        default:
            return -1;
    }

    return 0;
}
