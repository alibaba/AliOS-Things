/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#define CONFIG_LOGMACRO_DETAILS

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "app_mgr.h"
#include "be_common.h"
#include "be_jse_export.h"
#include "be_log.h"

#include "be_port_osal.h"
#include "cli_ext.h"
#include "hal/system.h"

#define TAG ("cli_js")

#ifdef DUMP_SYMBL_USAGE

static void handle_meminfo_cmd(char *pwbuf, int blen, int argc, char **argv) {
    be_debug(TAG, "memory usage= %d max=%d", be_jse_get_memory_usage(),
             be_jse_get_memory_max_usage());
#ifdef ESP_PLATFORM
    extern uint32_t esp_get_free_heap_size(void);
    be_debug(TAG, "free heap size: %d Byte ", esp_get_free_heap_size());
#endif
}

static struct be_be_cli_command meminfo_cmd = {
    .name     = "meminfo",
    .help     = "show BoneEngine meminfo",
    .function = handle_meminfo_cmd};

static void jstrace(void *arg) {
    trace_symbol_info(bone_engine_get_executor()->root, 0);
}

static void handle_jstrace_cmd(char *pwbuf, int blen, int argc, char **argv) {
    be_jse_task_schedule_call(jstrace, NULL);
}

static struct be_be_cli_command jstrace_cmd = {
    .name     = "jstrace",
    .help     = "show BoneEngine symbol info",
    .function = handle_jstrace_cmd};

#endif

/*
    eval  <js>
    eval hex  <js>    js is hex encode
*/
static void eval_js(void *arg) {
    bone_engine_start(arg);
    free(arg);
}

static void handle_eval_cmd(char *pwbuf, int blen, int argc, char **argv) {
    if (argc == 2) {
        char *str = calloc(1, strlen(argv[1]) + 1);
        if (NULL == str) {
            return;
        }
        strcpy(str, argv[1]);
        be_jse_task_schedule_call(eval_js, str);
    } else if (argc == 3) {
        if (strcmp("hex", argv[1]) == 0) {
            int i;
            int size              = strlen(argv[2]);
            unsigned char *outStr = calloc(1, size / 2 + 1);
            if (NULL == outStr) {
                return;
            }
            char *data = argv[2];
            unsigned char num;
            for (i = 0; i < size / 2; i++) {
                num = hex2num(data[0]);
                num = num * 16 + hex2num(data[1]);
                data += 2;
                outStr[i] = num;
            }
            outStr[i] = 0;
            be_jse_task_schedule_call(eval_js, outStr);
        }
    }
}

#ifdef TRACE_JSE_INFO
static struct be_cli_command eval_cmd = {.name = "eval",
                                         .help = "BoneEngine eval javascript",
                                         .function = handle_eval_cmd};

static void handle_jslook_cmd_cmd(char *pwbuf, int blen, int argc,
                                  char **argv) {
    if (argc == 2) {
        int id;
        id = atoi(argv[1]);

        dump_symbol_node_id(id);
    }
}
static struct be_cli_command jslook_cmd = {.name     = "jslook_cmd",
                                           .help     = "BoneEngine look symbol",
                                           .function = handle_jslook_cmd_cmd};
#endif

static void handle_boneflag_cmd(char *pwbuf, int blen, int argc, char **argv) {
    int ret;
    int len;
    if (argc > 1) {
        ret = atoi(argv[1]);
        hal_system_kv_set(BoneFlag, &ret, 4, 1);
    } else {
        len = 4;
        hal_system_kv_get(BoneFlag, &ret, &len);
        be_debug(TAG, "BoneFlag = %d len = %d", ret, len);
    }
}

static struct be_cli_command boneflag_cmd = {
    .name     = "BoneFlag",
    .help     = "get or set BoneEngine flag ",
    .function = handle_boneflag_cmd};

/*
    tree <dir>
    <dir>  optional
    target:
        /spiffs/<dir>
*/

static void tree(char *path) {
    be_dir_t *dir;
    be_dirent_t *ent;
    char childpath[48] = {0};

    dir = be_opendir(path);
    if (dir) {
        while ((ent = be_readdir(dir)) != NULL) {
#ifdef STM32F767xx
            sprintf(childpath, "%s %s\n", BE_CLI_REPLY, ent->fname);
#else
            sprintf(childpath, "%s %s\n", BE_CLI_REPLY, ent->d_name);
#endif
            be_cli_printf(childpath);
        }
        be_closedir(dir);
    }

    sprintf(childpath, "%s success\n", BE_CLI_REPLY);
    be_cli_printf(childpath);
}

static void handle_tree_cmd(char *pwbuf, int blen, int argc, char **argv) {
    char *targetname = NULL;
    char path[48]    = {0};

    strcpy(path, BE_FS_ROOT_DIR);
    if (argc > 1) {
        snprintf(path, sizeof(path), "%s/", BE_FS_ROOT_DIR);
        targetname = argv[1];
        if (targetname[0] == '.') {
            if (targetname[1] == '/') {
                strcat(path, targetname + 2);
            }
        } else {
            strcat(path, targetname);
        }
    }
    tree(path);
}

static struct be_cli_command tree_cmd = {
    .name     = "tree",
    .help     = "list contents of directories in a tree-like format ",
    .function = handle_tree_cmd};

/*
    rm  <target>
    <target> == format
*/

static void handle_rmdir_cmd(char *pwbuf, int blen, int argc, char **argv) {
    be_rmdir(BE_FS_ROOT_DIR);
}

static struct be_cli_command rmdir_cmd = {
    .name = "rmdir", .help = "rmdir js dir", .function = handle_rmdir_cmd};

static void handle_rm_cmd(char *pwbuf, int blen, int argc, char **argv) {
    char *targetname = NULL;
    char path[48]    = {0};
    int fd           = -1;
    int size;
    int len;
    char *ptr;

    if (argc > 1) {
        snprintf(path, sizeof(path), "%s/", BE_FS_ROOT_DIR);
        if (strcmp(argv[1], "format") == 0) {
            sprintf(path, "%s success\n", BE_CLI_REPLY);
            be_cli_printf(path);
            return;
        }

        targetname = argv[1];
        if (targetname[0] == '.') {
            if (targetname[1] == '/') {
                strcat(path, targetname + 2);
            }
        } else {
            strcat(path, targetname);
        }

        if (be_unlink(path) == 0) {
            sprintf(path, "%s success\n", BE_CLI_REPLY);
        } else {
            sprintf(path, "%s fail\n", BE_CLI_REPLY);
        }
        be_cli_printf(path);
    }
}

static struct be_cli_command rm_cmd = {
    .name = "rm", .help = "rm file", .function = handle_rm_cmd};

void cli_cmd_register_js() {
#ifdef DUMP_SYMBL_USAGE
    be_cli_register_command(&meminfo_cmd);
    be_cli_register_command(&jstrace_cmd);
#endif
#ifdef TRACE_JSE_INFO
    be_cli_register_command(&eval_cmd);
#endif
    be_cli_register_command(&boneflag_cmd);
    be_cli_register_command(&tree_cmd);
    be_cli_register_command(&rm_cmd);
    be_cli_register_command(&rmdir_cmd);
#ifdef TRACE_JSE_INFO
    be_cli_register_command(&jslook_cmd);
#endif
}
