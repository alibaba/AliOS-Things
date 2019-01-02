/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <syscall_no.h>

#if (CONFIG_CLI_SYSCALL > 0)

#include <cli_api.h>
#include <cli_syscall_arg.h>

int32_t cli_register_command(const struct cli_command_st *cmd)
{
    cli_register_command_syscall_arg_t _arg;

    _arg.cmd = cmd;

    return (int32_t)SYSCALL(SYS_CLI_REGISTER_CMD, (void*)&_arg);
}

#endif /* CONFIG_CLI_SYSCALL */

