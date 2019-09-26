/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#include "k_config.h"

#if (RHINO_CONFIG_UCLI_SYSCALL > 0)

#include <stdint.h>
#include "cli/cli_api.h"
#include "cli_syscall_arg.h"

int32_t sys_cli_register_command_stub(void *arg)
{
    cli_register_command_syscall_arg_t *_arg = arg;

    return cli_register_command(_arg->cmd);
}

int32_t sys_cli_unregister_command_stub(void *arg)
{
    cli_unregister_command_syscall_arg_t *_arg = arg;

    return cli_unregister_command(_arg->cmd);
}

int32_t sys_cli_register_commands_stub(void *arg)
{
    cli_register_commands_syscall_arg_t *_arg = arg;
    const struct cli_command_st         *cmd;

    int i;
    int ret;

    cmd = _arg->cmd;
    for (i = 0; i < _arg->num_cmds; i++) {
        ret = cli_register_command(cmd);
        if (0 != ret) {
            return ret;
        }
        cmd++;
    }

    return 0;
}

int32_t sys_cli_unregister_commands_stub(void *arg)
{
    cli_unregister_commands_syscall_arg_t *_arg = arg;
    const struct cli_command_st           *cmd;

    int i;
    int ret;

    cmd = _arg->cmd;
    for (i = 0; i < _arg->num_cmds; i++) {
        ret = cli_unregister_command(cmd);
        if (0 != ret) {
            return ret;
        }
        cmd++;
    }

    return 0;
}

#endif /* RHINO_CONFIG_UCLI_SYSCALL */

