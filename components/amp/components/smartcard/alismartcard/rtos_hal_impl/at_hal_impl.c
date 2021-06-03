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
 * module_host_impl.c: module hal impl by AT cmd
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "aliyun_iot_smartcard_hal.h"
#include "aliyun_iot_smartcard_os_hal.h"

#include "sim.h"
#include "module_at.h"
#include "module_common.h"
#include "mn_common.h"

static int g_argc = 0;
static char *g_argv[ARGC_LIMIT];

extern uint16_t g_mn_module_startup_time;
extern char g_reset_extra_cmd[256];
extern uint16_t g_mn_card_switch_time;
extern bool g_module_reset_while_switch;
extern char g_module_type[32];
static char g_str_resp[512];

#define MN_EXTRA_MMD_MAX (3)
typedef enum {
    MN_EXTRA_CMD_NULL = 0,
    MN_EXTRA_CMD_SHELL,
    MN_EXTRA_CMD_AT
} _mn_extra_cmd_type_et;

typedef struct {
    _mn_extra_cmd_type_et type;
    char cmd[64];
} _mn_extra_cmd_type_st;

static bool _mn_extra_cmd_reset_inited = false;
static _mn_extra_cmd_type_st g_mn_extra_reset_cmd[MN_EXTRA_MMD_MAX];

static bool _module_is_asr(void);
static bool _module_is_asr1802(void);

static bool _module_is_ec_cat1(void)
{
    if (strlen(g_module_type) != 0) {
        if (strncasecmp(g_module_type, "ec200s", strlen("ec200s")) == 0 || strncasecmp(g_module_type, "ec100y", strlen("ec100y")) == 0) {
            return true;
        }
    }

    return false;
}

static bool _module_is_nbiot(void)
{
    if (strlen(g_module_type) != 0) {
        if (strncasecmp(g_module_type, "m5311", strlen("m5311")) == 0 ||
            strncasecmp(g_module_type, "nbiot", strlen("nbiot")) == 0 ||
            strncasecmp(g_module_type, "bc20", strlen("bc20")) == 0   ||
            strncasecmp(g_module_type, "bc26", strlen("bc26")) == 0) {
            return true;
        }
    }

    return false;
}

static bool _module_is_bc(void)
{
    if (strlen(g_module_type) != 0) {
        if (strncasecmp(g_module_type, "bc20", strlen("bc20")) == 0   ||
            strncasecmp(g_module_type, "bc26", strlen("bc26")) == 0) {
            return true;
        }
    }

    return false;
}

static bool _module_is_m5311(void)
{
    if (strlen(g_module_type) != 0) {
        if (strncasecmp(g_module_type, "m5311", strlen("m5311")) == 0) {
            return true;
        }
    }

    return false;
}

/*!
 * \brief   check sim stat if it's inserted or not
 */
static int _module_check_simstat(void)
{
    int ret;

    ret = aliyun_iot_smartcard_hal_send_at("AT+CIMI", "OK", g_str_resp, sizeof(g_str_resp), 3000);

    if (ret != 0) {
        module_log_err("AT+CIMI fail, ret = %d!", ret);
        return MN_ERROR_NO_SIM;
    }

    return 0;
}

static int _mn_exec_extra_reset_cmd()
{
    int i = 0;
    static int extra_reset_cmd_cnt = 0;
    int ret = 0;

    if (!_mn_extra_cmd_reset_inited) {
        memset(g_mn_extra_reset_cmd, 0, sizeof(_mn_extra_cmd_type_st) * MN_EXTRA_MMD_MAX);

        if (strlen(g_reset_extra_cmd) == 0) {
            _mn_extra_cmd_reset_inited = true;
            return 0;
        }

        char *param = strtok(g_reset_extra_cmd, ";");
        char *param_cmd = NULL;

        while (param) {
            if (strncmp(param, "AT:", strlen("AT:")) == 0) {
                param_cmd = param + strlen("AT:");

                if (strlen(param_cmd)) {
                    g_mn_extra_reset_cmd[extra_reset_cmd_cnt].type = MN_EXTRA_CMD_AT;
                    strncpy(g_mn_extra_reset_cmd[extra_reset_cmd_cnt].cmd, param_cmd, sizeof(g_mn_extra_reset_cmd[extra_reset_cmd_cnt].cmd) - 1);
                    module_log_info("get AT cmd : %s", g_mn_extra_reset_cmd[extra_reset_cmd_cnt].cmd);
                    extra_reset_cmd_cnt ++;

                    if (extra_reset_cmd_cnt == MN_EXTRA_MMD_MAX) {
                        break;
                    }
                }
            }

            param = strtok(NULL, ";");
        }

        _mn_extra_cmd_reset_inited = true;
    }

    for (i = 0; i < extra_reset_cmd_cnt; i++) {
        if (g_mn_extra_reset_cmd[i].type == MN_EXTRA_CMD_AT) {
            module_log_info("call extra reset AT : %s", g_mn_extra_reset_cmd[i].cmd);
            ret = aliyun_iot_smartcard_hal_send_at(g_mn_extra_reset_cmd[i].cmd, "OK", g_str_resp, sizeof(g_str_resp), 5000);

            if (ret < 0) {
                break;
            }
        } else {
            continue;
        }
    }

    return ret;
}

int aliyun_iot_smartcard_hal_init(void)
{
    int ret;

    ret = module_serial_init();

    if (ret != 0) {
        return MN_ERROR_SERIAL;
    }

    ret = _module_check_simstat();

    if (ret != 0) {
        module_serial_deinit();
        return ret;
    }

    return ret;
}

int aliyun_iot_smartcard_hal_deinit(void)
{

    module_serial_deinit();

    module_log_dbg("< hal deinit >");
    return 0;
}

int aliyun_iot_smartcard_hal_get_rssi(int32_t *rssi)
{
    int ret;
    int32_t csq_rssi = 0;

    ret = aliyun_iot_smartcard_hal_send_at("AT+CSQ", "OK", g_str_resp, sizeof(g_str_resp), 3000);

    if (ret != 0) {
        module_log_err("AT+CSQ fail, ret = %d!", ret);
        return -1;
    }

    ret = module_proc_at_resp("+CSQ:", g_str_resp, NULL, &g_argc, g_argv);

    if (ret != 0) {
        module_log_err("module_proc_at_resp +CSQ: fail!");
        return -1;
    }

    csq_rssi = strtol((const char *)g_argv[0], NULL, 0);

    if (csq_rssi >= 0 && csq_rssi <= 32) {
        *rssi = -113 + csq_rssi * 2;
    } else if (csq_rssi >= 100 && csq_rssi <= 191) {
        *rssi = -116 + (csq_rssi - 100);
    } else {
        module_log_err("rssi = %d is abnormal!", csq_rssi);
        return -1;
    }

    return 0;
}

int aliyun_iot_smartcard_hal_get_iccid(char *iccid, int len)
{
    int i;
    int ret;
    char *str;
    char cmd_str[128];

    if (_module_is_bc()) {
        snprintf(cmd_str, sizeof(cmd_str), "AT+QCCID");
    } else if (_module_is_asr()) {
        snprintf(cmd_str, sizeof(cmd_str), "AT*ICCID?");
    } else {
        snprintf(cmd_str, sizeof(cmd_str), "AT+ICCID");
    }

    ret = aliyun_iot_smartcard_hal_send_at(cmd_str, "OK", g_str_resp, sizeof(g_str_resp), 3000);

    if (ret != 0) {
        module_log_err("%s fail, ret = %d!", cmd_str, ret);
        return -1;
    }

    /* longsung return ^SCID: other return +ICCID: */
    ret = module_proc_at_resp("CID:", g_str_resp, NULL, &g_argc, g_argv);

    if (ret != 0) {
        module_log_err("%s return fail!", cmd_str);
        return -1;
    }

    for (i = 0; i < strlen(g_argv[0]); i++) {
        if (g_argv[0][i] != ' ') {
            break;
        }
    }

    if (i == strlen(g_argv[0])) {
        module_log_err("iccid invalid");
        return -1;
    }

    str = g_argv[0] + i;

    strncpy(iccid, str, len);

    return 0;
}

int aliyun_iot_smartcard_hal_get_imei(char *imei, int len)
{
    int i;
    int ret;
    char *str;

    ret = aliyun_iot_smartcard_hal_send_at("AT+GSN", "OK", g_str_resp, sizeof(g_str_resp), 3000);

    if (ret != 0) {
        module_log_err("AT+GSN fail, ret = %d!", ret);
        return -1;
    }

    /* imei is in number format */
    for (i = 0; i < strlen(g_str_resp); i++) {
        if (g_str_resp[i] >= '0' && g_str_resp[i] <= '9') {
            break;
        }
    }

    if (i == strlen(g_str_resp)) {
        module_log_err("imei invalid");
        return -1;
    }

    ret = module_proc_at_resp("", g_str_resp + i, NULL, &g_argc, g_argv);

    if (ret != 0) {
        module_log_err("module_proc_at_resp AT+GSN fail!");
        return -1;
    }

    str = g_argv[0];
    strncpy(imei, str, len);

    return 0;
}

int aliyun_iot_smartcard_hal_get_act(aliyun_iot_smartcard_act_t *act)
{
    int ret;
    int cops_act;

    ret = aliyun_iot_smartcard_hal_send_at("AT+COPS?", "OK", g_str_resp, sizeof(g_str_resp), 3000);

    if (ret != 0) {
        module_log_err("AT+COPS fail, ret = %d!", ret);
        return -1;
    }

    ret = module_proc_at_resp("+COPS:", g_str_resp, NULL, &g_argc, g_argv);

    if (ret != 0) {
        module_log_err("module_proc_at_resp +COPS: fail!");
        return -1;
    }


    if (g_argc != 4) {
        module_log_err("module_proc_at_resp +COPS: fail!");
        return -1;
    }

    cops_act = strtol((const char *)g_argv[3], NULL, 0);

    switch (cops_act) {
        case 0:
        case 3:
        case 100:
            *act = ACT_2G;
            break;

        case 2:
        case 4:
        case 5:
        case 6:
            *act = ACT_3G;
            break;

        case 7:
            *act = ACT_4G;
            break;

        default:
            *act = ACT_NULL;
            module_log_err("act not found");
            return -1;
    }

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

    ret = mn_parse_carrier_from_iccid(iccid, car_type, NULL);

    if (ret != 0) {
        module_log_err("mn_parse_carrier_from_iccid fail!");
        return -1;
    }

    return ret;
}

#define MAX_RESET_RETRY_CNT (3)
int aliyun_iot_smartcard_hal_reset_module(void)
{
    int ret;
    int retry_cnt = 0;
    char reset_str[128];

    if (_module_is_m5311()) {
        snprintf(reset_str, sizeof(reset_str), "AT+COLDRB");
    } else if (_module_is_asr1802()) {
        snprintf(reset_str, sizeof(reset_str), "at+reset");
    } else if (_module_is_bc()) {
        snprintf(reset_str, sizeof(reset_str), "AT+QRST=1");
    } else {
        snprintf(reset_str, sizeof(reset_str), "AT+CFUN=1,1");
    }

    if (g_module_reset_while_switch) {
        ret = aliyun_iot_smartcard_hal_send_at(reset_str, "OK", g_str_resp, sizeof(g_str_resp), 5000);

        if (_module_is_asr() && ret == MODULE_AT_TIMEOUT) {
            module_log_dbg("asr module: skip resp check");
        } else if (_module_is_bc() && strlen(g_str_resp) == 0) {
            module_log_dbg("bc module: skip resp check");
        } else if (ret != 0) {
            module_log_err("%s fail!", reset_str);
            return -1;
        }
    }

    module_serial_deinit();

    HAL_SleepMs(g_mn_module_startup_time * 1000);

    while (1) {
        ret = module_serial_init();

        if (ret == 0 || retry_cnt++ >= MAX_RESET_RETRY_CNT) {
            break;
        }

        HAL_SleepMs(5000);
    }

    if (retry_cnt >= MAX_RESET_RETRY_CNT) {
        module_log_err("reset module failed! serial init failed!");
        return -1;
    }

    ret = _mn_exec_extra_reset_cmd();

    if (ret != 0) {
        module_log_err("fail to exec extra reset cmd!");
        return -1;
    }

    /* check sim is ready or not */
    retry_cnt = 0;

    while (1) {
        ret = _module_check_simstat();

        if (ret == 0 || retry_cnt++ >= MAX_RESET_RETRY_CNT) {
            break;
        }

        HAL_SleepMs(5000);
    }

    if (retry_cnt >= MAX_RESET_RETRY_CNT) {
        module_log_err("reset module failed! sim is unconnected!");
        return -1;
    }

    /* if reset, some paramter needed to be set again */
    if (g_module_reset_while_switch) {
        /* set phonebook storage to sim */
        ret = aliyun_iot_smartcard_hal_cpbs();

        if (ret != 0) {
            mn_log_err("aliyun_iot_smartcard_hal_cpbs failed!");
            return ret;
        }

        /* set TE Character Set to GSM */
        ret = aliyun_iot_smartcard_hal_cscs();

        if (ret != 0) {
            mn_log_err("aliyun_iot_smartcard_hal_cscs failed!");
            return ret;
        }
    }

    return 0;
}

#define MAX_RETRY_CNT (2)
#define MAX_ICCID_RETRY_CNT (30)

int aliyun_iot_smartcard_hal_cpbw(int slot, char *number)
{
    int ret;
    int cpbw_retry_cnt = 0;
    char pb_str[128];
    bool nbiot_flag;

    slot = slot - 1;

    if (slot < 1 || slot > 3) {
        return -1;
    }

    if (_module_is_asr()) {
        /* try to set cpbs = sm before cpbw */
        ret = aliyun_iot_smartcard_hal_cpbs();

        if (ret != 0) {
            mn_log_err("aliyun_iot_smartcard_hal_cpbs failed!");
            return ret;
        }
    }

    nbiot_flag = _module_is_nbiot();

    if (_module_is_bc()) {
        snprintf(pb_str, sizeof(pb_str), "AT+CRSM=220,28474,%d,4,28,\"FFFFFFFFFFFFFFFFFFFFFFFFFFFF0681101091100%dFFFFFFFFFFFFFF\",\"7f10\"", slot + 1, slot);
    } else if (nbiot_flag) {
        snprintf(pb_str, sizeof(pb_str), "AT+CRSM=220,28474,%d,4,28,\"FFFFFFFFFFFFFFFFFFFFFFFFFFFF0681101091100%dFFFFFFFFFFFFFF\",\"3f007f10\"", slot + 1, slot);
    } else {
        snprintf(pb_str, sizeof(pb_str), "AT+CPBW=%d,\"01011901%d0\"", slot + 1, slot);
    }

    while (1) {
        ret = aliyun_iot_smartcard_hal_send_at(pb_str, "OK", g_str_resp, sizeof(g_str_resp), 5000);

        if (ret == 0) {
            break;
        }

        if ((_module_is_ec_cat1() || _module_is_asr()) && strstr(g_str_resp, "+CME ERROR: 100") != NULL) {
            break;
        }

        if (ret != 0 && cpbw_retry_cnt >= MAX_RETRY_CNT) {
            module_log_err("%s fail, ret = %d!", pb_str, ret);
            return MN_ERROR_TIMEOUT;
        }

        cpbw_retry_cnt++;
        HAL_SleepMs(10 * 1000);
    }

    /* check nbiot return result */
    if (nbiot_flag) {
        ret = module_proc_at_resp("+CRSM:", g_str_resp, NULL, &g_argc, g_argv);

        if (ret != 0) {
            module_log_err("module_proc_at_resp +CRSM: fail!");
            return -1;
        }

        uint8_t sw1;
        uint8_t sw2;

        sw1 = strtol((const char *)g_argv[0], NULL, 0);
        sw2 = strtol((const char *)g_argv[1], NULL, 0);

        if (sw1 != 145 && (sw1 != 144 || sw2 != 0)) {
            module_log_err("CRSM return 0x%02X%02X failed!", sw1, sw2);
            return -1;
        }
    }

    /* wait for CPBW ready */
    HAL_SleepMs(g_mn_card_switch_time * 1000);

    return ret;
}

int aliyun_iot_smartcard_hal_cscs(void)
{
    int ret;
    int cscs_retry_cnt = 0;

    if (_module_is_bc()) {
        return 0;
    }

    while (1) {
        ret = aliyun_iot_smartcard_hal_send_at("AT+CSCS=\"GSM\"", "OK", g_str_resp, sizeof(g_str_resp), 3000);

        if (ret == 0) {
            break;
        }

        if (ret != 0 && cscs_retry_cnt >= MAX_RETRY_CNT) {
            module_log_err("AT+CSCS fail, ret = %d!", ret);
            return MN_ERROR_TIMEOUT;
        }

        cscs_retry_cnt++;
        HAL_SleepMs(5000);
    }

    return 0;
}

int aliyun_iot_smartcard_hal_cpbs(void)
{
    int ret;
    int cpbs_retry_cnt = 0;
    int wait_timeout = 3000;

    if (_module_is_asr()) {
        wait_timeout = 10000;
    }


    /* NB-IoT module skip cpbs */
    if (_module_is_nbiot()) {
        return 0;
    }


    while (1) {
        ret = aliyun_iot_smartcard_hal_send_at("AT+CPBS=\"SM\"", "OK", g_str_resp, sizeof(g_str_resp), wait_timeout);

        if (ret == 0) {
            break;
        }

        if (ret != 0 && cpbs_retry_cnt >= MAX_RETRY_CNT) {
            module_log_err("AT+CPBS fail, ret = %d!", ret);
            return MN_ERROR_TIMEOUT;
        }

        cpbs_retry_cnt++;
        HAL_SleepMs(5000);
    }

    return 0;
}

/* standard CPBR return value */
static int _module_parse_cpbr_result(int slot, char *name, int name_length, char *number, int number_length)
{
    /*
    [2020-05-02_13:45:37]AT+CPBR=1,4
    [2020-05-02_13:45:37]+CPBR: 1,"88861020302000000290",129,"ALIICCID"
    [2020-05-02_13:45:37]+CPBR: 2,"89860478102020000189",129,"SIM1"
    [2020-05-02_13:45:37]+CPBR: 3,"89860620030007561895",129,"SIM2"
    [2020-05-02_13:45:37]+CPBR: 4,"89860319452071982938",129,"SIM3"
    [2020-05-02_13:45:37]
    [2020-05-02_13:45:37]OK
    */
    int return_slot;
    return_slot = strtol((const char *)g_argv[0], NULL, 0);

    if (slot !=  return_slot) {
        module_log_err("get CPBR index failed!");
        return -1;
    }

    if (number_length < strlen(g_argv[1]) - 1) {
        module_log_err("get CPBR number failed!");
        return -1;
    }

    strncpy(number, g_argv[1] + 1, strlen(g_argv[1]) - 2);

    if (strncmp(g_argv[2], "129", 3) != 0) {
        module_log_err("get CPBR type failed!");
        return -1;
    }


    if (name_length < strlen(g_argv[3]) - 1) {
        module_log_err("get CPBR text failed!");
        return -1;
    }

    strncpy(name, g_argv[3] + 1, strlen(g_argv[3]) - 2);

    return 0;
}


static int _module_parse_crsm_result(int slot, char *name, int name_length, char *number, int number_length)
{
    /*
     * +CRSM: 144,0,"53494D32FFFFFFFFFFFFFFFFFFFF0B8198684087010202003003FFFF"
     */
    int i;
    int j;
    int idx = 0;
    uint8_t sw1;
    uint8_t sw2;
    uint8_t val;
    char *str;

    sw1 = strtol((const char *)g_argv[0], NULL, 0);
    sw2 = strtol((const char *)g_argv[1], NULL, 0);

    if (sw1 != 145 && (sw1 != 144 || sw2 != 0)) {
        module_log_err("CRSM return 0x%02x%02x failed!", sw1, sw2);
        return -1;
    }

    /* skip " */
    str = g_argv[2] + 1;

    char tmp[3] = {'\0'};

    /* 1. get name */
    for (i = 0 ; i <= strlen(str) - 2; i += 2) {
        tmp[0] = str[i];
        tmp[1] = str[i + 1];

        val = strtol(tmp, NULL, 16);

        if (val == 0xff) {
            break;
        }

        if (idx > name_length - 1) {
            module_log_err("name length %d too small!", name_length);
            return -1;
        }

        name[idx++] = val;
    }

    if (idx == 0) {
        module_log_err("name is null!");
        return -1;
    }

    /* 2. skip FF */
    idx = 0;

    for (; i <= strlen(str) - 2; i += 2) {
        tmp[0] = str[i];
        tmp[1] = str[i + 1];

        val = strtol(tmp, NULL, 16);

        if (val != 0xff) {
            break;
        }
    }

    if (val == 0) {
        module_log_err("number len is zero");
        return -1;
    }

    /* skip length and TON/NPI byte */
    i += 4;
    idx = 0;

    /* 3. get number */
    for (; i <= strlen(str) - 2; i += 2) {

        /* revert number */
        tmp[0] = str[i + 1];
        tmp[1] = str[i];

        for (j = 0; j <= 1; j++) {
            if (tmp[j] != 'F' && tmp[j] != 'f') {

                if (idx > number_length - 1) {
                    module_log_err("number length %d too small!", number_length);
                    return -1;
                }

                number[idx++] = tmp[j];
            } else {
                goto end;
            }
        }
    }

end:

    if (idx == 0) {
        module_log_err("number is null!");
        return -1;
    }

    return 0;
}

static int _module_parse_ec_cat1_cpbr_result(int slot, char *name, int name_length, char *number, int number_length)
{
    /*
    AT+CPBR=1,4
    +CPBR:1,2,"88861020302000000432",129,"",,,,,,,,,,,,
    +CPBR:2,3,"89860620030007563313",129,"",,,,,,,,,,,,
    +CPBR:3,4,"89860478102020000331",129,"",,,,,,,,,,,,
    +CPBR:4,5,"89860319452071984355",129,"",,,,,,,,,,,,
    */
    int return_slot;
    return_slot = strtol((const char *)g_argv[0], NULL, 0);

    if (slot !=  return_slot) {
        module_log_err("get CPBR index failed!");
        return -1;
    }

    if (number_length < strlen(g_argv[2]) - 1) {
        module_log_err("get CPBR number failed!");
        return -1;
    }

    strncpy(number, g_argv[2] + 1, strlen(g_argv[2]) - 2);

    if (strncmp(g_argv[3], "129", 3) != 0) {
        module_log_err("get CPBR type failed!");
        return -1;
    }

    switch (return_slot) {
        case 1:
            strncpy(name, "ALIICCID", name_length - 1);
            break;

        case 2:
            strncpy(name, "SIM1", name_length - 1);
            break;

        case 3:
            strncpy(name, "SIM2", name_length - 1);
            break;

        case 4:
            strncpy(name, "SIM3", name_length - 1);
            break;

        default:
            module_log_err("get CPBR index failed!");
            return -1;
    }

    return 0;

}

static int _module_parse_asr_cpbr_result(int slot, char *name, int name_length, char *number, int number_length)
{
    /*
    AT+CPBR=1,4
    +CPBR:1,"88861020302000000432",129,"",,,,,,,,,,,,
    +CPBR:2,"89860620030007563313",129,"",,,,,,,,,,,,
    +CPBR:3,"89860478102020000331",129,"",,,,,,,,,,,,
    +CPBR:4,"89860319452071984355",129,"",,,,,,,,,,,,
    */
    int return_slot;
    return_slot = strtol((const char *)g_argv[0], NULL, 0);

    if (slot !=  return_slot) {
        module_log_err("get CPBR index failed!");
        return MN_ERROR_CHECK;
    }

    if (number_length < strlen(g_argv[1]) - 1) {
        module_log_err("get CPBR number failed!");
        return MN_ERROR_CHECK;
    }

    strncpy(number, g_argv[1] + 1, strlen(g_argv[1]) - 2);

    if (strncmp(g_argv[2], "129", 3) != 0) {
        module_log_err("get CPBR type failed!");
        return MN_ERROR_CHECK;
    }

    switch (return_slot) {
        case 1:
            strncpy(name, "ALIICCID", name_length - 1);
            break;

        case 2:
            strncpy(name, "SIM1", name_length - 1);
            break;

        case 3:
            strncpy(name, "SIM2", name_length - 1);
            break;

        case 4:
            strncpy(name, "SIM3", name_length - 1);
            break;

        default:
            module_log_err("get CPBR index failed!");
            return MN_ERROR_CHECK;
    }

    return 0;

}

static bool _module_is_asr1802(void)
{
    if (strlen(g_module_type) != 0) {
        if (strncasecmp(g_module_type, "asr1802s", strlen("asr1802s")) == 0) {
            return true;
        }
    }

    return false;
}

static bool _module_is_asr(void)
{
    if (strlen(g_module_type) != 0) {
        if (strncasecmp(g_module_type, "asr", strlen("asr")) == 0) {
            return true;
        }
    }

    return false;
}

int aliyun_iot_smartcard_hal_cpbr(int slot, char *name, int name_length, char *number, int number_length)
{
    int ret = -1;
    int cpbr_retry_cnt = 0;
    char pb_str[128];
    bool nbiot_flag;
    char *cmd;
    int argc;

    if (slot > 4 || slot < 0 || !number || !name) {
        module_log_err("params error!");
        return ret;
    }

    memset(name, '\0', name_length);
    memset(number, '\0', number_length);

    nbiot_flag = _module_is_nbiot();

    if (_module_is_bc()) {
        snprintf(pb_str, sizeof(pb_str), "AT+CRSM=178,28474,%d,4,0,,\"7f10\"", slot);
        cmd = "+CRSM:";
        argc = 3;
    } else if (nbiot_flag) {
        snprintf(pb_str, sizeof(pb_str), "AT+CRSM=178,28474,%d,4,0,,\"3f007f10\"", slot);
        cmd = "+CRSM:";
        argc = 3;
    } else {
        snprintf(pb_str, sizeof(pb_str), "AT+CPBR=%d", slot);
        cmd = "+CPBR:";
        argc = 4;
    }

    while (1) {
        ret = aliyun_iot_smartcard_hal_send_at(pb_str, "OK", g_str_resp, sizeof(g_str_resp), 5000);

        if (ret == 0) {
            break;
        }

        if (ret != 0 && cpbr_retry_cnt >= MAX_RETRY_CNT) {
            module_log_err("%s fail!", pb_str);
            return MN_ERROR_TIMEOUT;
        }

        cpbr_retry_cnt++;
        HAL_SleepMs(10 * 1000);
    }

    ret = module_proc_at_resp(cmd, g_str_resp, NULL, &g_argc, g_argv);

    if (ret != 0 || g_argc < argc) {
        module_log_err("module_proc_at_resp %s fail!", cmd);
        return -1;
    }

    if (nbiot_flag) {
        ret = _module_parse_crsm_result(slot, name, name_length, number, number_length);
    } else if (_module_is_ec_cat1()) {
        ret = _module_parse_ec_cat1_cpbr_result(slot, name, name_length, number, number_length);
    } else if (_module_is_asr()) {
        ret = _module_parse_asr_cpbr_result(slot, name, name_length, number, number_length);
    } else {
        ret = _module_parse_cpbr_result(slot, name, name_length, number, number_length);
    }

    return ret;
}
