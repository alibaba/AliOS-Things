/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_config.h"
#include "syscall_no.h"
#include "aos/cli.h"
#include "cli_syscall_arg.h"
#include "syscall.h"
#include "cli_task.h"
#include "cli_conf.h"
#include <stdarg.h>

int32_t aos_cli_register_command(const struct cli_command *cmd)
{
    int32_t ret;

    cli_register_command_syscall_arg_t _arg;

    _arg.cmd = cmd;

    ret = (int32_t)SYSCALL(SYS_CLI_REGISTER_CMD, (void *)&_arg);
    if (ret == 0) {
        cli_cmd_add(cmd);
    }

    return ret;
}

int32_t aos_cli_unregister_command(const struct cli_command *cmd)
{
    int32_t ret;

    cli_unregister_command_syscall_arg_t _arg;

    _arg.cmd = cmd;

    ret = (int32_t)SYSCALL(SYS_CLI_UNREGISTER_CMD, (void *)&_arg);
    if (ret == 0) {
        cli_cmd_rm(cmd);
    }

    return ret;
}

int32_t aos_cli_register_commands(const struct cli_command *cmd, int32_t num_cmds)
{
    int32_t ret;

    cli_register_commands_syscall_arg_t _arg;

    _arg.cmd = cmd;
    _arg.num_cmds = num_cmds;

    ret = (int32_t)SYSCALL(SYS_CLI_REGISTER_CMDS, (void *)&_arg);
    if (ret == 0) {
        cli_cmds_add(cmd, num_cmds);
    }

    return ret;
}

int32_t aos_cli_unregister_commands(const struct cli_command *cmd, int32_t num_cmds)
{
    int32_t ret;

    cli_unregister_commands_syscall_arg_t _arg;

    _arg.cmd = cmd;
    _arg.num_cmds = num_cmds;

    ret = (int32_t)SYSCALL(SYS_CLI_UNREGISTER_CMDS, (void *)&_arg);
    if (ret == 0) {
        cli_cmds_rm(cmd, num_cmds);
    }

    return ret;
}

int32_t aos_cli_task_set_console(void *task_handle, void *console)
{
    cli_task_set_console_syscall_arg_t arg;

    arg.task_handle = task_handle;
    arg.console     = console;

    return (int32_t)SYSCALL(SYS_CLI_TASK_SET_CONSOLE, &arg);
}


int32_t aos_cli_printf(const char *buffer, ...)
{
    va_list ap;
    int32_t sz, len;
    char print_buff[CLI_OUTBUF_SIZE];
    char *pos     = NULL;
    char *message = NULL;

    message = (char *)print_buff;
    memset(message, 0, CLI_OUTBUF_SIZE);
#if (CLI_SEPRATED_CONSOLE > 0)
    // append unit separator for cli output
    message[0] = 0x1F;
    sz = 1;
#else
    sz = 0;
#endif
    pos = message + sz;

    va_start(ap, buffer);
    len = vsnprintf(pos, CLI_OUTBUF_SIZE - sz, buffer, ap);
    va_end(ap);

    if (len <= 0) {
        return 0;
    }

    printf("%s", message);
    return 0;
}