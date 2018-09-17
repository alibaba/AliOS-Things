/*
 * Copyright © 2018 alibaba. All rights reserved.
 */

#define CONFIG_LOGMACRO_DETAILS

#include <be_osal.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <be_osal.h>
#include "app-mgr/app_mgr.h"
#include "cli_ext.h"
#include "utils/be_common.h"
#ifdef BE_OS_AOS
#include <aos/aos.h>
#endif

static int cli_file_fd = -1;
static int cli_file_offset = 0;
#define MODULE_TAG ("cli_file")
typedef struct {
    int type;
    int index;
    int size;
    char *data;
    char *targetname;
    unsigned char *outStr;
} PUSH_s;
static PUSH_s gPushParam;

#ifdef BE_OS_AOS
extern int cli_putstr(char *msg);
extern int cli_getchar(char *inbuf);
#endif

/*
push  <type>  <index>  <size>  <data>  <targetname>
type = 0  normal file
type = 1  app pack file
*/

static int cli_file_open(char *targetname)
{
    int fd;
    char path[48] = {0};

    if (targetname == NULL) {
        return -1;
    }
    snprintf(path, sizeof(path), "%s/", BE_FS_ROOT_DIR);
    if (targetname[0] == '.') {
        if (targetname[1] == '/') {
            strcat(path, targetname + 2);
        } else {
            // .aaa  not support hide file
            return -1;
        }
    } else {
        strcat(path, targetname);
    }

    int i = strlen(BE_FS_ROOT_DIR);  // 8
    int len = strlen(path);
    for (; i < len; i++) {
        if (path[i] == '/') {
            path[i] = 0;
            be_osal_mkdir(path);
            path[i] = '/';
        }
    }

    fd = be_osal_open(path, O_RDWR | O_CREAT | O_TRUNC);

    return fd;
}

static void push(void *arg)
{
    int type;
    int index;
    int size;
    char *data;
    char *targetname = NULL;

    int ret;
    int i;
    unsigned char num;
    unsigned char *outStr;

    type = gPushParam.type;
    index = gPushParam.index;
    size = gPushParam.size;
    data = gPushParam.data;
    targetname = gPushParam.targetname;
    outStr = gPushParam.outStr;

    if (size == 0) {
        // send finished
        // close file
        if (cli_file_fd != -1) {
            be_osal_sync(cli_file_fd);
            be_osal_close(cli_file_fd);
            cli_file_fd = -1;
        }

        if (type == 1) {
            apppack_final();
            app_mgr_set_boneflag(1);
        }

        sprintf(outStr, "%s %d\n", BE_CLI_REPLY, cli_file_offset);
        cli_putstr(outStr);
        free(outStr);
        return;
    }

    if (index == 0) {
        // open targetname
        cli_file_offset = 0;
        if (type == 0) {
            // normal file
            cli_file_fd = cli_file_open(targetname);
        } else {
            // app pack
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
        be_osal_write(cli_file_fd, outStr, size);
    }

    if (type == 1) {
        apppack_update(outStr, size);
    }

    // serialport reply
    sprintf(outStr, "%s %d\n", BE_CLI_REPLY, cli_file_offset);
    cli_putstr(outStr);
    free(outStr);
}

static void handle_push_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    int type;
    int index;
    int size;
    char *data;
    char *targetname = NULL;

    // target file to store such as: drivers/led.js
    if (argc >= 6) {
        targetname = argv[5];
    }

    if (argc >= 5) {
        // push  <type>  <index>  <size>  <data>  <targetname>
        // avoid stack overflow uncomment the following debug
        type = atoi(argv[1]);
        index = atoi(argv[2]);
        size = atoi(argv[3]);
        data = argv[4];

        gPushParam.type = type;
        gPushParam.index = index;
        gPushParam.size = size;
        gPushParam.data = data;
        gPushParam.targetname = targetname;
        gPushParam.outStr = malloc(256);  //(unsigned char*) pwbuf;

        // avoid stack overflow !!!
        be_osal_schedule_call(push, NULL);

        // DO NOT print any character until REPLY send
    }
}

static struct cli_command push_cmd = {
    .name = "push",
    .help = "push file ",
    .function = handle_push_cmd
};

/*
pull   <targetname>
reply:    BE_CLI_REPLY  <size>  <data>
*/

static void handle_pull_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    char *targetname = NULL;
    char path[48] = {0};
    int fd = -1;
    int size;
    int len;
    char *ptr;

    if (argc < 2) {
        sprintf(pwbuf, "%s fail\n", BE_CLI_REPLY);
        return;
    }

    targetname = argv[1];

    //strcpy(path, BE_FS_ROOT_DIR);
    snprintf(path, sizeof(path), "%s/", BE_FS_ROOT_DIR);

    if (targetname[0] == '.') {
        if (targetname[1] == '/') {
            strcat(path, targetname + 2);
        } else {
            // .aaa  not support hide file
        }
    } else if (targetname[0] == '/') {
        strcat(path, targetname + 1);
    } else {
        strcat(path, targetname);
    }

    fd = be_osal_open(path, O_RDONLY);
    if (fd < 0) {
        sprintf(pwbuf, "%s fail\n", BE_CLI_REPLY);
        return;
    }

    // current aos pwbuf ,   blen == 2048
    // start send

    if (blen == 2048) {
        size = 256;
    } else {
        size = 128;  // bad TBD
    }

    ptr = pwbuf + size * 2 * 32;

    int i;
    int j;
    unsigned char *hex;
    len = be_osal_read(fd, ptr, size);
    while (len > 0) {
        j = sprintf(pwbuf, "%s %d ", BE_CLI_REPLY, len);
        hex = pwbuf + j;
        for (i = 0; i < len; i++) {
            num2hex(ptr[i], hex);
            hex += 2;
        }
        hex[0] = '\n';
        hex[1] = 0;

        cli_putstr(pwbuf);
        // continue read
        len = be_osal_read(fd, ptr, size);
    }

    sprintf(pwbuf, "%s 0 00\n", BE_CLI_REPLY);
    be_osal_close(fd);
}

static struct cli_command pull_cmd = {
    .name = "pull",
    .help = "pull file ",
    .function = handle_pull_cmd
};

void cli_cmd_register_file()
{
#ifdef BE_OS_AOS
    aos_cli_register_command(&push_cmd);
    aos_cli_register_command(&pull_cmd);
#endif
}
