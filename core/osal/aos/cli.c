/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "aos/cli.h"
#include "aos/errno.h"

#include "cli/cli_api.h"

static int _cli_to_aos_res(int res)
{
    switch (res) {
        case CLI_OK:
            return 0;
        case CLI_ERR_NOMEM:
            return -ENOMEM;
        case CLI_ERR_DENIED:
            return -EPERM;
        case CLI_ERR_INVALID:
            return -EINVAL;
       default:
            return -EIO;
    }
}

int aos_cli_init(void)
{
    return _cli_to_aos_res(cli_init());
}

int aos_cli_stop(void)
{
    return _cli_to_aos_res(cli_stop());
}

const char *aos_cli_get_tag(void)
{
    return cli_tag_get();
}

int aos_cli_register_command(const struct cli_command *cmd)
{
    return _cli_to_aos_res(cli_register_command((struct cli_command_st *)cmd));
}

int aos_cli_unregister_command(const struct cli_command *cmd)
{
    return _cli_to_aos_res(cli_unregister_command((struct cli_command_st *)cmd));
}

int aos_cli_register_commands(const struct cli_command *cmds, int num)
{
    return _cli_to_aos_res(cli_register_commands((struct cli_command_st *)cmds, num));
}

int aos_cli_unregister_commands(const struct cli_command *cmds, int num)
{
    return _cli_to_aos_res(cli_unregister_commands((struct cli_command_st *)cmds, num));
}

