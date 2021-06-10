/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "amp_platform.h"
#include "amp_config.h"
#include "aos_system.h"
#include "amp_task.h"
#include "aos_fs.h"
#include "amp_defines.h"
#include "board_mgr.h"
#include "cJSON.h"

#define MOD_STR "AMP_ENGINE"
#define APP_PACKAGE_FILE_NAME JSE_FS_ROOT_DIR "/package.json"
#define MAX_FILE_NAME_LEN 127
static char js_app_file_name[128];

static int32_t g_console_log_enable = 1;

aos_sem_t jse_task_exit_sem;

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

    int js_app_fd = -1;
    int file_len  = 0;
    int json_fd   = -1;

    snprintf(js_app_file_name, 128, AMP_APP_MAIN_JS);

    /* use the currrent dir default index.js file to the main js entry programe
     */
    if ((js_app_fd = aos_open(js_app_file_name, O_RDONLY)) >= 0) {
        aos_close(js_app_fd);
        amp_debug(MOD_STR, "find the default file :%s", js_app_file_name);
        return js_app_file_name;
    }

    snprintf(js_app_file_name, 128, AMP_APP_MAIN_JSB);

    /* use the currrent dir default index.js file to the main js entry programe
     */
    if ((js_app_fd = aos_open(js_app_file_name, O_RDONLY)) >= 0) {
        aos_close(js_app_fd);
        amp_debug(MOD_STR, "find the default file :%s", js_app_file_name);
        return js_app_file_name;
    }

    snprintf(js_app_file_name, 128, AMP_APP_PACKAGE_JSON);
    /* cannot find the index.js int current dir */
    if ((json_fd = aos_open(js_app_file_name, O_RDONLY)) < 0) {
        amp_error(MOD_STR, "cannot find the file :%s", js_app_file_name);
        return NULL;
    }

    /* read package config file to json_data buffer */
    file_len = aos_lseek(json_fd, 0, SEEK_END);
    json_data = aos_calloc(1, sizeof(char) * (file_len + 1));
    if (NULL == json_data) {
        aos_close(json_fd);
        json_fd = -1;
        return NULL;
    }
    aos_lseek(json_fd, 0, SEEK_SET);
    aos_read(json_fd, json_data, file_len);
    aos_close(json_fd);

    /* parser the package json data */
    root = cJSON_Parse(json_data);
    if (NULL == root) {
        aos_free(json_data);
        amp_error(MOD_STR, "cJSON_Parse failed");
        return NULL;
    }

    /* find the test/xxx.js */
    item = cJSON_GetObjectItem(root, "test");
    if (NULL != item && cJSON_String == item->type &&
        strstr(item->valuestring, ".js")) {
        snprintf(js_app_file_name, sizeof(js_app_file_name), "/%s", item->valuestring);
        if ((js_app_fd = aos_open(js_app_file_name, O_RDONLY)) < 0) {
            aos_close(js_app_fd);
            aos_free(json_data);
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
        aos_free(json_data);
        cJSON_Delete(root);
        amp_debug(MOD_STR, "find main index %s", js_app_file_name);
        return js_app_file_name;
    }

    aos_free(json_data);
    cJSON_Delete(root);

    return NULL;
}

void be_jse_task_main_entrance(void *arg)
{
    amp_debug(MOD_STR, "jse main task start...");
    amp_task_main();

    aos_task_exit(0);

    return;
}

void amp_task_main()
{
    uint8_t ssdp_started = 0;
    int app_running = 1;
    char user_dir[32] = {0};

    snprintf(user_dir, sizeof(user_dir), AMP_APP_MAIN_JSON);
    if (0 != board_mgr_init(user_dir)) {
        amp_error(MOD_STR, "read %s error", user_dir);
        app_running = 0;
    }

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

    amp_task_deinit();
    aos_sem_signal(&jse_task_exit_sem);
    amp_memmgt_mem_show_rec();
}

void jsengine_main(void)
{
    int ret = 0;
    aos_task_t jsengine_task;

    if (aos_sem_new(&jse_task_exit_sem, 0) != 0) {
        amp_error(MOD_STR, "create jse exit sem failed");
        return;
    }

    amp_debug(MOD_STR, "jse_task created");
    aos_task_new_ext(&jsengine_task, "amp_jsengine_task", be_jse_task_main_entrance, NULL, JSENGINE_TASK_STACK_SIZE, AOS_DEFAULT_APP_PRI);
}
