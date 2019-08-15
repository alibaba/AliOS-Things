/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#define CONFIG_LOGMACRO_DETAILS

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jse_common.h"
#include "app_mgr.h"
#include "cli_ext.h"

static int cli_file_fd     = -1;
static int cli_file_offset = 0;
#define MODULE_TAG ("cli_file")
typedef struct {
    int type;
    int index;
    int size;
    char* data;
    char* targetname;
    unsigned char* outStr;
} PUSH_s;
static PUSH_s gPushParam;

#ifdef BE_OS_AOS
extern void sub_call_start(void* arg);
#endif

static void push(void* arg)
{
    int type;
    int index;
    int size;
    char* data;
    char* targetname = NULL;

    int ret;
    int i;
    unsigned char num;
    unsigned char* outStr;

    type       = gPushParam.type;
    index      = gPushParam.index;
    size       = gPushParam.size;
    data       = gPushParam.data;
    targetname = gPushParam.targetname;
    outStr     = gPushParam.outStr;

    if (size == 0) {
        /* send finished */
        /* close file */
        if (cli_file_fd != -1) {
            jse_sync(cli_file_fd);
            jse_close(cli_file_fd);
            cli_file_fd = -1;
        }

        if (type == 1) {
            apppack_final();
        }
        app_mgr_set_boneflag(1);

        sprintf((char*)outStr, "%s %d\n", BE_CLI_REPLY, cli_file_offset);
        jse_cli_printf("%s", outStr);
        jse_free(outStr);
        /* push finished restart app or reboot device */
        if (type == 0) {
            /* be_jse_task_schedule_call(sub_call_start, targetname); */
        } else {
            jse_system_reboot();
        }
        return;
    }

    if (index == 0) {
        /* open targetname */
        cli_file_offset = 0;
        if (type == 0) {
            /* normal file */
            cli_file_fd = app_mgr_open_file(targetname);
        } else {
            /* app pack */
            cli_file_fd = -1;
            app_mgr_set_boneflag(0);
            apppack_init(write_app_pack);
        }
    }

    cli_file_offset += size;

    for (i = 0; i < size; i++) {
        num = hex2num(data[0]);
        num = num * 16 + hex2num(data[1]);
        data += 2;
        outStr[i] = num;
    }
    outStr[i] = 0;

    if ((size > 0) && (cli_file_fd != -1)) {
        jse_write(cli_file_fd, outStr, size);
    }

    if (type == 1) {
        ret = apppack_update(outStr, size);
        if (ret != 0) {
            jse_cli_printf("%s", BE_PUSH_ERROR_REPLY);
        }
    }

    /* serialport reply */
    sprintf((char*)outStr, "%s %d\n", BE_CLI_REPLY, cli_file_offset);
    jse_cli_printf("%s", outStr);
    jse_free(outStr);
    jse_osal_delete_task(NULL);
}

static void handle_push_cmd(char* pwbuf, int blen, int argc, char** argv)
{
    int type;
    int index;
    int size;
    char* data;
    char* targetname = NULL;

    /* target file to store such as: drivers/led.js */
    if (argc >= 6) targetname = argv[5];

    if (argc >= 5) {
        /* push  <type>  <index>  <size>  <data>  <targetname> */
        /* avoid stack overflow uncomment the following debug */
        type  = atoi(argv[1]);
        index = atoi(argv[2]);
        size  = atoi(argv[3]);
        data  = argv[4];

        gPushParam.type       = type;
        gPushParam.index      = index;
        gPushParam.size       = size;
        gPushParam.data       = data;
        gPushParam.targetname = targetname;
        gPushParam.outStr     = jse_malloc(size + 1);

        /* disable JSEngine logout */
        bone_console_log_disable();
        /* create task to parser push data, may cause stack overflow in cli task!!! */
        int ret = jse_osal_create_task("be_push", push, NULL, 4096,
                                      CLI_TSK_PRIORITY, NULL);
    }
}

static struct jse_cli_command push_cmd = {
    .name = "push", .help = "push file ", .function = handle_push_cmd};

/*
pull   <targetname>
reply:    BE_CLI_REPLY  <size>  <data>
*/

static void handle_pull_cmd(char* pwbuf, int blen, int argc, char** argv)
{
    char* targetname = NULL;
    char path[48]    = {0};
    int fd           = -1;
    int size;
    int len;
    char* ptr;

    if (argc < 2) {
        sprintf(pwbuf, "%s fail\n", BE_CLI_REPLY);
        return;
    }

    targetname = argv[1];

    /* strcpy(path, JSE_FS_ROOT_DIR); */
    snprintf(path, sizeof(path), "%s/", JSE_FS_ROOT_DIR);

    if (targetname[0] == '.') {
        if (targetname[1] == '/') {
            strcat(path, targetname + 2);
        } else {
            /* .aaa  not support hide file */
        }
    } else if (targetname[0] == '/') {
        strcat(path, targetname + 1);
    } else {
        strcat(path, targetname);
    }

    fd = jse_open(path, O_RDONLY);
    if (fd < 0) {
        sprintf(pwbuf, "%s fail\n", BE_CLI_REPLY);
        return;
    }

    /* current aos pwbuf ,   blen == 2048 */
    /* start send */

    if (blen == 2048) {
        size = 256;
    } else {
        size = 128; /* bad TBD */
    }

    ptr = pwbuf + size * 2 * 32;

    int i;
    int j;
    unsigned char* hex;
    len = jse_read(fd, ptr, size);
    while (len > 0) {
        j   = sprintf(pwbuf, "%s %d ", BE_CLI_REPLY, len);
        hex = (uint8_t*)pwbuf + j;
        for (i = 0; i < len; i++) {
            num2hex(ptr[i], hex);
            hex += 2;
        }
        hex[0] = '\n';
        hex[1] = 0;

        jse_debug(pwbuf);
        fflush(stdout);

        /* continue read */
        len = jse_read(fd, ptr, size);
    }

    sprintf(pwbuf, "%s 0 00\n", BE_CLI_REPLY);
    jse_close(fd);
}

static struct jse_cli_command pull_cmd = {
    .name = "pull", .help = "pull file ", .function = handle_pull_cmd};

void cli_cmd_register_file()
{
    jse_cli_register_command(&push_cmd);
    jse_cli_register_command(&pull_cmd);
}
