/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "amp_config.h"
#include "amp_system.h"
#include "amp_task.h"
#include "amp_fs.h"
#include "amp_defines.h"
#include "board_mgr.h"
#include "cJSON.h"

#define MOD_STR "AMP_ENGINE"
#define APP_PACKAGE_FILE_NAME JSE_FS_ROOT_DIR "/package.json"
#define MAX_FILE_NAME_LEN 127
static char js_app_file_name[128];

static int32_t g_console_log_enable = 1;

void *jse_task_exit_sem;

extern void jsengine_init(void);
extern void jsengine_eval_file(const char *filename);

int32_t bone_console_get_log_flag(void)
{
    return g_console_log_enable;
}
void bone_console_log_enable(void)
{
    g_console_log_enable = 1;
}

void bone_console_log_disable(void)
{
    g_console_log_enable = 0;
}

/**
 * 1. search js entry: /spiffs/index.js in spiffs
 * 2. get [test] or [main] in /spiffs/package.json
 */
char *search_js_app_main_entry(void)
{
    cJSON *root     = NULL;
    cJSON *item     = NULL;
    void *json_data = NULL;

    void * js_app_fd = NULL;
    int file_len  = 0;
    void *json_fd   = NULL;
    int32_t curpos = -1;

    char root_dir[64] = {0};
    HAL_Get_User_Dir(root_dir);

    snprintf(js_app_file_name, 128, "%s/app.js", root_dir);

    /* use the currrent dir default index.js file to the main js entry programe
     */
    if ((js_app_fd = HAL_Fopen(js_app_file_name, "r")) != NULL) {
        HAL_Fclose(js_app_fd);
        amp_debug(MOD_STR, "find the default file :%s", js_app_file_name);
        return js_app_file_name;
    }

    snprintf(js_app_file_name, 128, "%s/package.json", root_dir);
    /* cannot find the index.js int current dir */
    if ((json_fd = HAL_Fopen(js_app_file_name, "r")) == NULL) {
        amp_error(MOD_STR, "cannot find the file :%s", js_app_file_name);
        return NULL;
    }

    /* read package config file to json_data buffer */
    HAL_Fseek(json_fd, 0, HAL_SEEK_END, &curpos);
    if (curpos < 0) {
        file_len = HAL_Ftell(json_fd);
    } else {
        file_len = curpos;
    }
    json_data = amp_calloc(1, sizeof(char) * (file_len + 1));
    if (NULL == json_data) {
        HAL_Fclose(json_fd);
        json_fd = NULL;
        return NULL;
    }
    HAL_Fseek(json_fd, 0, SEEK_SET, &curpos);
    HAL_Fread(json_data, 1, file_len, json_fd);
    HAL_Fclose(json_fd);

    /* parser the package json data */
    root = cJSON_Parse(json_data);
    if (NULL == root) {
        amp_free(json_data);
        amp_error(MOD_STR, "cJSON_Parse failed");
        return NULL;
    }

    /* find the test/xxx.js */
    item = cJSON_GetObjectItem(root, "test");
    if (NULL != item && cJSON_String == item->type &&
        strstr(item->valuestring, ".js")) {
        snprintf(js_app_file_name, sizeof(js_app_file_name), "%s/%s",
                 root_dir, item->valuestring);
        if ((js_app_fd = HAL_Fopen(js_app_file_name, "r")) != NULL) {
            HAL_Fclose(js_app_fd);
            amp_free(json_data);
            cJSON_Delete(root);
            amp_debug(MOD_STR, "find test index %s", js_app_file_name);
            return js_app_file_name;
        }
    }

    /* find the main/xxx.js */
    item = cJSON_GetObjectItem(root, "main");
    if (NULL != item && cJSON_String == item->type &&
        strstr(item->valuestring, ".js")) {
        strncpy(js_app_file_name, item->valuestring, MAX_FILE_NAME_LEN);
        amp_free(json_data);
        cJSON_Delete(root);
        amp_debug(MOD_STR, "find main index %s", js_app_file_name);
        return js_app_file_name;
    }

    amp_free(json_data);
    cJSON_Delete(root);

    return NULL;
}

void *be_jse_task_main_entrance(void *arg)
{
    amp_debug(MOD_STR, "jse main task start...");
    jse_task_main();

    HAL_ThreadDelete(NULL);

    return NULL;
}

int update_flag = 0;
void jse_task_main()
{
    uint8_t ssdp_started = 0;
    int app_running = 1;
    char localip[32];
    char user_dir[128] = {0};

    HAL_Get_User_Dir(user_dir);
    snprintf(user_dir + strlen(user_dir), 32 - strlen(user_dir), "/app.json");
    if (0 != board_mgr_init(user_dir)) {
        amp_error(MOD_STR, "read %s error", user_dir);
        app_running = 0;
    }

    update_flag = 0;

    /* JSE task init */
    amp_task_init();

    /* JSE init */
    jsengine_init();
    amp_debug(MOD_STR, "jsengine_init ok");

    /* run the js application */
    char *filename = search_js_app_main_entry();
    amp_debug(MOD_STR, "search_js_app_main_entry: %s", filename ? filename : "null");
    if (filename && app_running) {
        jsengine_eval_file(filename);
    } else {
        amp_error(MOD_STR, "Won't run app.js");
    }

    while (1) {
        /* loop for asynchronous operation */
        if(amp_task_yield(200) == 1) {
            amp_debug(MOD_STR, "jsengine task yield exit!");
            break;
        }   
    }

    update_flag = 1;
	HAL_SleepMs(300);
    jsengine_task_deinit();
    HAL_SemaphorePost(jse_task_exit_sem);
}

void jsengine_main(void)
{
    int ret = 0;
    void *jsengine_task;
    int jsengine_task_stack_used;
    amp_os_thread_param_t task_params = {0};

#ifdef AMP_RECOVERY_ENABLE
    if (amp_recovery_init() != 0)
    {
        // amp_debug(MOD_STR, "amp_recovery_entry");
        /* recovery mode entry */
        amp_recovery_entry();
    }
#endif

    amp_debug(MOD_STR, "jsengine start...");
    // jse_system_kv_init();

#ifdef JSE_HW_ADDON_WIFI
    HAL_WiFi_Init();
#endif

    if ((ret = HAL_Finit()) != 0) {
        amp_error(MOD_STR, "fs init failed %d", ret);
        return;
    }

#ifdef JSE_CLI_DEBUG
    /* jse_cli_init(); */
    cli_cmd_register_js();
    cli_cmd_register_dev();
    cli_cmd_register_file();
    cli_cmd_register_app();
#endif

    jse_task_exit_sem = HAL_SemaphoreCreate();
    if (!jse_task_exit_sem) {
        amp_error(MOD_STR, "create jse exit sem failed");
        return;
    }

    amp_debug(MOD_STR, "jse_task created");
    task_params.name = "amp_jsengine_task";
    task_params.priority = HAL_GetDefaultTaskPriority();
    task_params.stack_size = JSENGINE_TASK_STACK_SIZE;
    HAL_ThreadCreate(&jsengine_task, be_jse_task_main_entrance, NULL, &task_params, &jsengine_task_stack_used);
}
