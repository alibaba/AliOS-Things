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
 * mn_export.c: export api impl
 *
 */
#include "aliyun_iot_smartcard_internal.h"
#include "aliyun_iot_smartcard_hal.h"
#include "aliyun_iot_smartcard_os_hal.h"
#include "mn_common.h"
#include "sim.h"
#include "module_common.h"
#include "version.h"

static int g_mn_init = 0;
static char g_conf_file_path[256] = {'\0'};

int aliyun_iot_smartcard_set_config_file(const char *file_path)
{
    if (g_mn_init) {
        HAL_Printf("already init!\n");
        return MN_ERROR_INIT;
    }

    strncpy(g_conf_file_path, file_path, sizeof(g_conf_file_path) - 1);
    return 0;

}

int aliyun_iot_smartcard_init(void)
{
    int ret = -1;
    const char *file_path;

    if (g_mn_init) {
        HAL_Printf("already init!\n");
        return MN_ERROR_INIT;
    }

    if (strlen(g_conf_file_path) != 0) {
        file_path = g_conf_file_path;
    } else {
        file_path = SMARTCARD_CONF_FILE;
    }

    ret = mn_parse_configs(file_path);

    if (ret < 0) {
        HAL_Printf("parse configs failed, use default configs!\n");
        return -1;
    }

    /* init log after config parse since log paramter may be set in
     * config file */
    ret = log_init(mn_get_log_dir(), LOG_MODULE_NAME, LOG_FILE, LOG_MOD_VERBOSE);

    if (ret != 0) {
        HAL_Printf("log_init fail!\n");
        return -1;
    }

    ret = aliyun_iot_smartcard_hal_init();

    if (ret < 0) {
        mn_log_err("aliyun_iot_smartcard_hal_init fail!");
        return ret;
    }

    ret = sim_init();

    if (ret != 0) {
        mn_log_err("sim_init fail!");
        aliyun_iot_smartcard_hal_deinit();
        return ret;
    }

    g_mn_init = 1;
    mn_log_info("init done");

    return MN_SUCCESS;
}

int aliyun_iot_smartcard_deinit(void)
{
    if (!g_mn_init) {
        mn_log_err("not init!");
        return MN_ERROR_INIT;
    }

    if (aliyun_iot_smartcard_hal_deinit() != 0) {
        mn_log_err("aliyun_iot_smartcard_hal_deinit fail!");
        return MN_ERROR_GENERIC;
    }

    log_destroy();

    g_mn_init = 0;
    return MN_SUCCESS;
}


int aliyun_iot_is_smartcard(void)
{
    bool support;

    if (!g_mn_init) {
        mn_log_err("not init!");
        return MN_ERROR_INIT;
    }

    sim_is_switch_support(&support);

    if (!support) {
        mn_log_warn("not aliiot smart card!");
        return MN_ERROR_NOT_SMARTCARD;
    }

    return MN_SUCCESS;
}

int aliyun_iot_smartcard_change_operator(aliyun_iot_smartcard_type_t type)
{
    int ret;

    if (!g_mn_init) {
        mn_log_err("not init!");
        return MN_ERROR_INIT;
    }

    if (type < SWITCH_TO_SIM1 || type >= SWITCH_MAX) {
        mn_log_err("parameter abnormal!");
        return MN_ERROR_COMMON_PARAMETER;
    }

    ret = aliyun_iot_is_smartcard();

    if (ret != 0) {
        mn_log_err("not aliiot smart card!");
        return ret;
    }

    mn_log_info("switch to %d", type);

    ret = mn_hal_switch(type);

    return ret;
}

int aliyun_iot_smartcard_get_version(int *major, int *minor)
{
    *major = SMARTCARD_SDK_MAJOR_VER;
    *minor = SMARTCARD_SDK_MINOR_VER;

    return 0;
}

extern bool g_module_reset_while_switch;

int aliyun_iot_smartcard_set_reset_flag(bool reset)
{
    g_module_reset_while_switch = reset;
    return 0;
}

extern uint16_t g_mn_module_startup_time;
int aliyun_iot_smartcard_set_startup_time(uint16_t second)
{
    g_mn_module_startup_time = second;
    return 0;
}

extern char g_module_type[32];
int aliyun_iot_smartcard_set_module_type(const char *type)
{
    strncpy(g_module_type, type, 32 - 1);
    return 0;
}

static void remove_quote(char *str, int length)
{
    int i;
    int len;

    len = strlen(str);

    if (len > length || len < 3) {
        return;
    }

    if (str[0] == '"' && str[len - 1] == '"') {

        for (i = 0; i < len - 2; i++) {
            str[i] = str[i + 1];
        }

        str[len - 2] = '\0';
    }

    return;
}

/* if name or number are with quote, remove it */
static void shrink_cpbr_result(char *name, int name_length, char *number, int number_length)
{
    remove_quote(name, name_length);
    remove_quote(number, name_length);

    return;

}

static char g_ali_iccid[21];

static int parse_alibaba_iccid(char *str)
{
    int i = 0;
    int count = 0;
    char *number = str;

    if (str == NULL) {
        return -1;
    }

    if (strncmp(number, "8886", 4) != 0) {
        return -1;
    }

    number += 4;

    for (i = 0 ; i < 3 ; i++) {
        if (strncmp(number, "10", 2) == 0) {
            number += 2;
            count++;
        } else if (strncmp(number, "20", 2) == 0) {
            number += 2;
            count++;
        } else if (strncmp(number, "30", 2) == 0) {
            number += 2;
            count++;
        } else if (strncmp(number, "00", 2) == 0) {
            number += 2;
        } else {
            return -1;
        }
    }

    if (strncmp(number, "1", 1) != 0 &&
        strncmp(number, "2", 1) != 0 &&
        strncmp(number, "3", 1) != 0) {
        return -1;
    }

    strncpy(g_ali_iccid, str, sizeof(g_ali_iccid) - 1);

    return 0;
}

int aliyun_iot_smartcard_check(bool *support)
{
    int ret;
    char name[32];
    char number[32];

    ret = aliyun_iot_smartcard_hal_init();

    if (ret != 0) {
        mn_log_err("aliyun_iot_smartcard_hal_init failed!");
        return -1;
    }

    /* set phonebook storage to sim */
    ret = aliyun_iot_smartcard_hal_cpbs();

    if (ret != 0) {
        mn_log_err("aliyun_iot_smartcard_hal_cpbs failed!");
        return -1;
    }

    /* set TE Character Set to GSM */
    ret = aliyun_iot_smartcard_hal_cscs();

    if (ret != 0) {
        mn_log_err("aliyun_iot_smartcard_hal_cscs failed!");
        return -1;
    }

    /* parse ali iccid */
    ret = aliyun_iot_smartcard_hal_cpbr(1, name, sizeof(name), number, sizeof(number));

    if (ret != 0) {
        /* cpbr not existed, is not smartcard */
        *support = false;
        return 0;
    }

    shrink_cpbr_result(name, sizeof(name), number, sizeof(number));

    mn_log_info("get ali iccid name: %s number: %s", name, number);

    /* name is not aliiccid */
    if (strncmp(name, "ALIICCID", strlen("ALIICCID")) != 0) {
        *support = false;
        return 0;

    }

    if (parse_alibaba_iccid(number) != 0) {
        mn_log_err("parse_alibaba_iccid failed!");
        *support = false;
        return 0;
    }

    *support = true;
    return 0;
}

int aliyun_iot_smartcard_get_aliiccid(char *iccid, int len)
{
    int ret;
    bool support;

    if (strlen(g_ali_iccid) != 0) {
        strncpy(iccid, g_ali_iccid, len);
        return 0;
    }

    ret = aliyun_iot_smartcard_check(&support);

    if (ret != 0 || support == false) {
        return -1;
    }

    strncpy(iccid, g_ali_iccid, len);

    return 0;
}
