/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#if (CONFIG_CLI_SYSCALL > 0)

#include <cli_api.h>
#include <cli_syscall_arg.h>

int32_t sys_cli_register_command_stub(void *arg)
{
    cli_register_command_syscall_arg_t *_arg = arg;

    return cli_register_command(_arg->cmd);
}

#endif /* CONFIG_CLI_SYSCALL */

