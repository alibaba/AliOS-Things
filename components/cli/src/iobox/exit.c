/*
 * Copyright (C) 2022 Alibaba Group Holding Limited
 */

#include <cli_api.h>
#include <cli_console.h>

static int exit_main(int argc, char **argv)
{
    cli_console *cur_console = cli_task_get_console(krhino_cur_task_get());
    cli_console_set_exit_flag(cur_console);
    return 0;
}

ALIOS_CLI_CMD_REGISTER(exit_main, exit, exit the console);
