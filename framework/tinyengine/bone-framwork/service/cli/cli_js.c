/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#define CONFIG_LOGMACRO_DETAILS

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <be_osal.h>
#include "app-mgr/app_mgr.h"
#include "cli_ext.h"
#include "be_jse_api.h"
#include "utils/be_common.h"
#ifdef BE_OS_AOS
#include <aos/aos.h>
#endif
#include "be_osal_fs.h"

#define MODULE_TAG ("cli_js")

#ifdef BE_OS_AOS
extern int cli_putstr(char *msg);
extern int cli_getchar(char *inbuf);
#endif

#ifdef DUMP_SYMBL_USAGE

static void handle_meminfo_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    be_debug(MODULE_TAG, "memory usage= %d max=%d", be_jse_get_memory_usage(),
             be_jse_get_memory_max_usage());
}

static struct cli_command meminfo_cmd = {
    .name = "meminfo",
    .help = "show BoneEngine meminfo",
    .function = handle_meminfo_cmd
};

static void jstrace(void *arg)
{
    trace_symbol_info(bone_engine_get_executor()->root, 0);
}

static void handle_jstrace_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    be_osal_schedule_call(jstrace, NULL);
}

static struct cli_command jstrace_cmd = {
    .name = "jstrace",
    .help = "show BoneEngine symbol info",
    .function = handle_jstrace_cmd
};

#endif

/*
    eval  <js>
    eval hex  <js>    js is hex encode
*/
static void eval_js(void *arg)
{
    bone_engine_start(arg);
    free(arg);
}

static void handle_eval_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    if (argc == 2) {
        char *str = calloc(1, strlen(argv[1]) + 1);
        if (NULL == str) {
            return;
        }
        strcpy(str, argv[1]);
        be_osal_schedule_call(eval_js, str);
    } else if (argc == 3) {
        if (strcmp("hex", argv[1]) == 0) {
            int i;
            int size = strlen(argv[2]);
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
            be_osal_schedule_call(eval_js, outStr);
        }
    }
}

static struct cli_command eval_cmd = {
    .name = "eval",
    .help = "BoneEngine eval javascript",
    .function = handle_eval_cmd
};

static void handle_boneflag_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    int ret;

    if (argc > 1) {
        ret = atoi(argv[1]);
        be_osal_kv_set(BoneFlag, &ret, 4, 1);
    }
}

static struct cli_command boneflag_cmd = {
    .name = "BoneFlag",
    .help = "set BoneEngine flag ",
    .function = handle_boneflag_cmd
};

/*
    tree <dir>
    <dir>  optional
    target:
        /spiffs/<dir>
*/

static void tree(char *path)
{
    be_osal_dir_t *dir;
    be_osal_dirent_t *ent;
    char childpath[48] = {0};

    dir = be_osal_opendir(path);
    if (dir) {
        while ((ent = be_osal_readdir(dir)) != NULL) {
            sprintf(childpath, "%s %s\n", BE_CLI_REPLY, ent->d_name);
            cli_putstr(childpath);
        }
        be_osal_closedir(dir);
    }

    sprintf(childpath, "%s success\n", BE_CLI_REPLY);
    cli_putstr(childpath);
}

static void handle_tree_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    char *targetname = NULL;
    char path[48] = {0};
    int fd = -1;
    int size;
    int len;
    char *ptr;

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

static struct cli_command tree_cmd = {
    .name = "tree",
    .help = "list contents of directories in a tree-like format ",
    .function = handle_tree_cmd
};

/*
    rm  <target>
    <target> == format
*/
static void handle_rm_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    char *targetname = NULL;
    char path[48] = {0};
    int fd = -1;
    int size;
    int len;
    char *ptr;

    if (argc > 1) {
        snprintf(path, sizeof(path), "%s/", BE_FS_ROOT_DIR);
        if (strcmp(argv[1], "format") == 0) {
            sprintf(path, "%s success\n", BE_CLI_REPLY);
            cli_putstr(path);
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

        if (be_osal_unlink(path) == 0) {
            sprintf(path, "%s success\n", BE_CLI_REPLY);
        } else {
            sprintf(path, "%s fail\n", BE_CLI_REPLY);
        }
        cli_putstr(path);
    }
}

static struct cli_command rm_cmd = {
    .name = "rm", .help = "rm file", .function = handle_rm_cmd
};

void cli_cmd_register_js()
{
#ifdef BE_OS_AOS
#ifdef DUMP_SYMBL_USAGE
    aos_cli_register_command(&meminfo_cmd);
    aos_cli_register_command(&jstrace_cmd);
#endif
    aos_cli_register_command(&eval_cmd);
    aos_cli_register_command(&boneflag_cmd);
    aos_cli_register_command(&tree_cmd);
    aos_cli_register_command(&rm_cmd);
#endif
}
