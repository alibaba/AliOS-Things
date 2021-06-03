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
 * mn_config.c: config file api impl
 *
 */

#include "aliyun_iot_smartcard_export.h"

#include "mn_common.h"
#include "log.h"
#include "module_common.h"

#include "amp_smartcard.h"

uint16_t g_mn_module_startup_time = 30;
uint16_t g_mn_card_switch_time = 0;
char g_module_at[32] = {0};
uint32_t g_module_at_baudrate = DEFAULT_MODULE_AT_BAUDRATE;
char g_module_type[32] = {0};
bool g_module_reset_while_switch = false;
char g_reset_extra_cmd[256] = {0};

static char g_mn_log_dir[256] = {0};

int mn_parse_configs(const char *file_path)
{
    int reset_while_switch = 0;
    unsigned short card_switch_time = 0;
    unsigned short module_startup_time = 30;

    aos_smartcard_configure(g_module_type, sizeof(g_module_type),
        &reset_while_switch, &card_switch_time, &module_startup_time);

    g_module_reset_while_switch = !!reset_while_switch;
    g_mn_card_switch_time = card_switch_time;
    g_mn_module_startup_time = module_startup_time;
    return 0;
}

char *mn_get_log_dir(void)
{
    if (strlen(g_mn_log_dir) == 0) {
        return NULL;
    } else {
        return g_mn_log_dir;
    }
}
