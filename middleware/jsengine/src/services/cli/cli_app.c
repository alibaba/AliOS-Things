/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jse_common.h"
#include "app_mgr.h"
#include "cli_ext.h"

/* serial port must write \n to indicates send finished */
#define BE_DEBUGER_START_CMD_REPLY "#be-debuger-start-reply# \n"
#define BE_DEBUGER_STOP_CMD_REPLY "#be-debuger-stop-reply# \n"
#define BE_DEBUGER_RESTART_CMD_REPLY "#be-debuger-restart-reply# \n"

extern void sub_call_start(void *arg);
extern void sub_call_stop(void *arg);
extern void sub_call_restart(void *arg);

#ifdef JSE_IDE_DEBUG
static void handle_appupdate_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    jse_debug("task name=%s \n", jse_osal_get_taskname());
    /* jse_debug("pwbuf=%s  blen=%d", pwbuf, blen); */
    jse_debug("argc=%d \n", argc);

    int ret = 1;

    if (argc > 1) {
        jse_debug("url=%s", argv[1]);
        /* ret = apppack_upgrade(strdup(argv[1])); */
        be_jse_task_schedule_call((bone_engine_call_t)apppack_upgrade, strdup(argv[1]));
        jse_debug("apppack_update=%d", ret);
    }
}
#endif

static void handle_start_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    jse_cli_printf("%s", BE_DEBUGER_START_CMD_REPLY);
    be_jse_task_schedule_call(sub_call_start, argv[1]);
}

static void handle_stop_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    jse_cli_printf("%s", BE_DEBUGER_STOP_CMD_REPLY);
    be_jse_task_schedule_call(sub_call_stop, argv[1]);
}

static void handle_restart_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    jse_cli_printf("%s", BE_DEBUGER_RESTART_CMD_REPLY);
    be_jse_task_schedule_call(sub_call_restart, argv[1]);
}
#ifdef JSE_IDE_DEBUG
/* upgrade app package */
static struct jse_cli_command appupdatecmd = {.name     = "appupdate",
                                             .help     = "update app pack ",
                                             .function = handle_appupdate_cmd};
#endif

static struct cli_command appstartcmd = {
    .name = "start", .help = "start index.js", .function = handle_start_cmd};

static struct jse_cli_command appstopcmd = {
    .name = "stop", .help = "stop index.js", .function = handle_stop_cmd};

static struct jse_cli_command apprestartcmd = {.name     = "restart",
                                              .help     = "restart index.js",
                                              .function = handle_restart_cmd};

void cli_cmd_register_app()
{
#ifdef BE_OS_AOS
    jse_cli_register_command(&appstartcmd);
    jse_cli_register_command(&appstopcmd);
    jse_cli_register_command(&apprestartcmd);
#endif

#ifdef JSE_IDE_DEBUG
    jse_cli_register_command(&appupdatecmd);
#endif
}
