/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifdef AOS_COMP_CLI
#include "aos/cli.h"
#endif
#include "HaasLog.h"
#include "mpy_main.h"

static void python_entry(int argc, char **argv)
{
    LOG_E("start micropython ...\r\n");
    mpy_init();
    mpy_run(argc, argv);
    LOG_E("end micropython ...\r\n");
    mpy_deinit();
    return;
}

#ifdef AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(python_entry, python, start micropython)
#endif
