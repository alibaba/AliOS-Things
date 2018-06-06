/*
 * Copyright © 2018 alibaba. All rights reserved.
 */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils/be_common.h"
#include <be_osal.h>
#ifdef BE_OS_AOS
#include <aos/aos.h>
#endif
#include "app-mgr/app_mgr.h"


// serial port must write \n to indicates send finished
#define BE_DEBUGER_START_CMD_REPLY "#be-debuger-start-reply# \n"
#define BE_DEBUGER_STOP_CMD_REPLY "#be-debuger-stop-reply# \n"
#define BE_DEBUGER_RESTART_CMD_REPLY "#be-debuger-restart-reply# \n"
#define CLI_REPLY printf
#define MODULE_TAG ("cli_app")

extern void sub_call_start(void *arg);
extern void sub_call_stop(void *arg);
extern void sub_call_restart(void *arg);
extern void sub_call_rename(void *arg);
extern void sub_call_wifi(void *arg);

static void handle_appupdate_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    be_debug(MODULE_TAG, "aos_task_name=%s \r\n", be_osal_task_name());
    be_debug(MODULE_TAG, "pwbuf=%s  blen=%d", pwbuf, blen);
    be_debug(MODULE_TAG, "argc=%d", argc);

    int ret;

    if (argc > 1) {
        ret = apppack_upgrade(argv[1]);
        be_debug(MODULE_TAG, "apppack_update=%d", ret);
    }
}

static void handle_start_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    CLI_REPLY(BE_DEBUGER_START_CMD_REPLY);
    be_osal_schedule_call(sub_call_start, argv[1]);
}

static void handle_stop_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    CLI_REPLY(BE_DEBUGER_STOP_CMD_REPLY);
    be_osal_schedule_call(sub_call_stop, argv[1]);
}

static void handle_restart_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    CLI_REPLY(BE_DEBUGER_RESTART_CMD_REPLY);
    be_osal_schedule_call(sub_call_restart, argv[1]);
}

// upgrade app package
static struct cli_command appupdatecmd = {
    .name = "appupdate",
    .help = "update app pack ",
    .function = handle_appupdate_cmd
};

static struct cli_command appstartcmd = {
    .name = "start",
    .help = "start index.js",
    .function = handle_start_cmd
};

static struct cli_command appstopcmd = {
    .name = "stop",
    .help = "stop index.js",
    .function = handle_stop_cmd
};

static struct cli_command apprestartcmd = {
    .name = "restart",
    .help = "restart index.js",
    .function = handle_restart_cmd
};

void cli_cmd_register_app()
{
#ifdef BE_OS_AOS
    aos_cli_register_command(&appupdatecmd);
    aos_cli_register_command(&appstartcmd);
    aos_cli_register_command(&appstopcmd);
    aos_cli_register_command(&apprestartcmd);
#endif
}

