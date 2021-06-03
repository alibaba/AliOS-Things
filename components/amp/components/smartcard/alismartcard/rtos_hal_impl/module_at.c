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
 * module_at.c: module at impl
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "module_common.h"
#include "aliyun_iot_smartcard_hal.h"
#include "aliyun_iot_smartcard_os_hal.h"

typedef struct {
    char *cmd;
    int (*fn)(int argc, char *argv[]);
} at_cmd_t;

//AT functions
static int at_comm_func(int argc, char *argv[]);

#define AT_CSQ "+CSQ:"
#define AT_CREG "+CREG:"
#define AT_ICCID "CID:"
#define AT_QPING "+QPING:"
#define AT_QCOPS "+QCOPS:"
#define AT_COPS "+COPS:"
#define AT_CPBR "+CPBR:"
#define AT_CRSM "+CRSM:"

#ifdef AT_LOG_ENABLED
#define at_log_dbg module_log_dbg
#define at_log_info module_log_info
#else
#define at_log_dbg(fmt, args ...) \
    do { \
    } while(0);
#define at_log_info(fmt, args ...) \
    do { \
    } while(0);
#endif

static at_cmd_t g_at_table[] = {
    {AT_CSQ, at_comm_func},
    {AT_CREG, at_comm_func},
    {AT_ICCID, at_comm_func},
    {AT_QPING, at_comm_func},
    {AT_QCOPS, at_comm_func},
    {AT_COPS, at_comm_func},
    {AT_CPBR, at_comm_func},
    {AT_CRSM, at_comm_func},
    {"", at_comm_func}
};

#define AT_TABLE_SIZE	(sizeof(g_at_table) / sizeof(at_cmd_t))

extern char g_module_at[32];
extern uint32_t g_module_at_baudrate;

int module_serial_init(void)
{
    return 0;
}

int module_serial_deinit(void)
{
    return 0;
}

static int at_comm_func(int argc, char *argv[])
{
    int i;

    for (i = 0; i < argc; i++) {
        at_log_dbg("argv[%d]: %s", i, argv[i]);
    }

    return 0;
}

int module_proc_at_resp(const char *cmd, char *str_resp, char **str_resp_next, int *argc, char *argv[])
{
    int i;
    int index;
    char *rxcmd;
    char *ptr = NULL;
    int len;
    int ret;

    *argc = 0;

    /* strip rxcmd from str_resp */
    rxcmd = strstr(str_resp, cmd);

    if (rxcmd == NULL) {
        module_log_err("cmd %s not found!", cmd);
        return -1;
    }

    len = strlen(rxcmd);

    for (i = 0; i < len; i++) {
        if (rxcmd[i] == '\r' || rxcmd[i] == '\n') {
            rxcmd[i] = '\0';

            if (str_resp_next) {
                *str_resp_next = rxcmd + i + 1;
            }

            break;
        }
    }

    if (i == len) {
        module_log_err("not CR / LF found!");
        return -1;
    }


    for (index = 0; index < AT_TABLE_SIZE; index++) {
        int cmd_len = strlen(g_at_table[index].cmd);

        if (!strncmp((const char *)rxcmd, g_at_table[index].cmd, cmd_len)) {
            ptr = (char *)rxcmd + cmd_len;
            break;
        }
    }

    if (index >= AT_TABLE_SIZE || !g_at_table[index].fn) {
        module_log_err("no func found!");
        return -1;
    }

    char *str = strtok((char *)ptr, ",");

    while (str) {
        argv[*argc] = str;
        *argc += 1;
        str = strtok((char *)NULL, ",");
    }

    ret = g_at_table[index].fn(*argc, argv);

    return ret;
}

int module_serial_get_name(char *name, int len)
{
    if (strlen(g_module_at) == 0) {
        return -1;
    }

    strncpy(name, g_module_at, len - 1);

    return 0;
}
