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
#include "parson.h"
#include "log.h"
#include "module_common.h"

#define CNPK_MODULE_CONFIG_NAME "module_config"
#define CNPK_SERVER_CONFIG_NAME "server_config"
#define CONFIG_FILE_MAX_SIZE    (1024)

uint16_t g_mn_module_startup_time = 30;
uint16_t g_mn_card_switch_time = 0;
char g_module_at[32] = {0};
uint32_t g_module_at_baudrate = DEFAULT_MODULE_AT_BAUDRATE;
char g_module_type[32] = {0};
bool g_module_reset_while_switch = false;
char g_reset_extra_cmd[256] = {0};

static char g_mn_log_dir[256] = {0};
static uint16_t g_mn_log_level = 0;
static bool g_mn_log_storage = false;

static int mn_gen_default_configs(const char *file_path)
{
    int buff_index = 0;
    char buffer[CONFIG_FILE_MAX_SIZE] = {0};
    int ret = -1;

    ret = snprintf(buffer + buff_index, CONFIG_FILE_MAX_SIZE - strlen(buffer), "{\"%s\":{\"module_startup_time\":%d,", CNPK_MODULE_CONFIG_NAME, g_mn_module_startup_time);

    if (ret <= 0) {
        return -1;
    }

    buff_index += ret;

    ret = snprintf(buffer + buff_index, CONFIG_FILE_MAX_SIZE - strlen(buffer), "\"module_at_port\":\"%s\",\"module_at_baudrate\":%d,", g_module_at, g_module_at_baudrate);

    if (ret <= 0) {
        return -1;
    }

    buff_index += ret;

    ret = snprintf(buffer + buff_index, CONFIG_FILE_MAX_SIZE - strlen(buffer), "\"reset_while_switch\":%d", g_module_reset_while_switch);

    if (ret <= 0) {
        return -1;
    }

    buff_index += ret;

    ret = snprintf(buffer + buff_index, CONFIG_FILE_MAX_SIZE - strlen(buffer), "},");

    if (ret <= 0) {
        return -1;
    }

    buff_index += ret;

    ret = snprintf(buffer + buff_index, CONFIG_FILE_MAX_SIZE - strlen(buffer), "\"%s\":{", CNPK_SERVER_CONFIG_NAME);

    if (ret <= 0) {
        return -1;
    }

    buff_index += ret;

    LOG_LEVEL level;
    log_get_level(&level);

    ret = snprintf(buffer + buff_index, CONFIG_FILE_MAX_SIZE - strlen(buffer), "\"log_level\":%d,", level);

    if (ret <= 0) {
        return -1;
    }

    buff_index += ret;


    bool storage;
    log_get_storage(&storage);

    ret = snprintf(buffer + buff_index, CONFIG_FILE_MAX_SIZE - strlen(buffer), "\"log_storage\":%d", storage);

    if (ret <= 0) {
        return -1;
    }

    buff_index += ret;

    ret = snprintf(buffer + buff_index, CONFIG_FILE_MAX_SIZE - strlen(buffer), "}}");

    if (ret <= 0) {
        return -1;
    }

    buff_index += ret;

    JSON_Value *root_val = json_parse_string_with_comments(buffer);

    if (root_val == NULL) {
        mn_log_err("parse string to json failed!");
        return -1;
    }

    ret = json_serialize_to_file_pretty(root_val, file_path);

    if (ret < 0) {
        mn_log_err("serialize to json failed!");
        return -1;
    }

    json_value_free(root_val);

    return 0;
}

int mn_parse_configs(const char *file_path)
{
    JSON_Value *root_val = NULL;
    JSON_Object *module_conf_obj = NULL;
    JSON_Object *server_conf_obj = NULL;
    JSON_Value *val = NULL;

    /* init global var with default val */
    strncpy(g_module_at, DEFAULT_MODULE_AT_PORT, sizeof(g_module_at) - 1);

    root_val = json_parse_file_with_comments(file_path);

    if (root_val == NULL) {
        mn_log_warn("config file errors! config file will be overwrited with default configs!");
        goto error;
    }

    /*get module configs */
    module_conf_obj = json_object_get_object(json_value_get_object(root_val), CNPK_MODULE_CONFIG_NAME);

    if (module_conf_obj == NULL) {
        mn_log_warn("config file errors! will overwirte the configs will default configs!");
        goto error;
    }

    const char *str;

    str = json_object_get_string(module_conf_obj, "module_type");

    if (str != NULL) {
        strncpy(g_module_type, str, sizeof(g_module_type) - 1);
    }

    val = json_object_get_value(module_conf_obj, "module_startup_time");

    if (val != NULL) {
        g_mn_module_startup_time = (uint16_t)json_value_get_number(val);
    }

    val = json_object_get_value(module_conf_obj, "sim_switch_time");

    if (val != NULL) {
        g_mn_card_switch_time = (uint16_t)json_value_get_number(val);
    }

    str = json_object_get_string(module_conf_obj, "module_at_port");

    if (str != NULL) {
        strncpy(g_module_at, str, sizeof(g_module_at) - 1);
    }

    val = json_object_get_value(module_conf_obj, "module_at_baudrate");

    if (val != NULL) {
        g_module_at_baudrate = (uint32_t)json_value_get_number(val);
    }

    str = json_object_get_string(module_conf_obj, "reset_extra_cmd");

    if (str != NULL) {
        strncpy(g_reset_extra_cmd, str, sizeof(g_reset_extra_cmd) - 1);
    }

    val = json_object_get_value(module_conf_obj, "reset_while_switch");

    if (val != NULL) {
        g_module_reset_while_switch = (bool)(uint16_t)json_value_get_number(val);
    }

    server_conf_obj = json_object_get_object(json_value_get_object(root_val), CNPK_SERVER_CONFIG_NAME);

    if (server_conf_obj == NULL) {
        mn_log_err("config file errors! will overwirte the configs will default configs!");
        goto error;
    }

    str = json_object_get_string(server_conf_obj, "log_dir");

    if (str != NULL) {
        strncpy(g_mn_log_dir, str, sizeof(g_mn_log_dir) - 1);
    }

    val = json_object_get_value(server_conf_obj, "log_level");

    if (val != NULL) {
        g_mn_log_level = (uint16_t)json_value_get_number(val);

        if (g_mn_log_level > LOG_LEVEL_FATAL) {
            g_mn_log_level = LOG_LEVEL_FATAL;
        }

        log_set_level(g_mn_log_level);
    }

    val = json_object_get_value(server_conf_obj, "log_storage");

    if (val != NULL) {
        g_mn_log_storage = (bool)(uint16_t)json_value_get_number(val);
        log_set_storage(g_mn_log_storage);
    }

    mn_log_dbg("module_configs :: module_startup_time %d, reset_while_switch %d, "
               "at_port %s, at_baudrate %d",
               g_mn_module_startup_time, g_module_reset_while_switch,
               g_module_at, g_module_at_baudrate);

    mn_log_dbg("server_configs :: log_dir %s log_level %d, log storage %d",
               strlen(g_mn_log_dir) == 0 ? "NULL" : g_mn_log_dir, g_mn_log_level, g_mn_log_storage);

    json_value_free(root_val);
    return 0;

error:

    if (root_val) {
        json_value_free(root_val);
    }

    return mn_gen_default_configs(file_path);
}

char *mn_get_log_dir(void)
{
    if (strlen(g_mn_log_dir) == 0) {
        return NULL;
    } else {
        return g_mn_log_dir;
    }
}
