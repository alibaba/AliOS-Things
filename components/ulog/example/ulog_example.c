/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aos/kernel.h"
#include "ulog/ulog.h"
#if AOS_COMP_CLI
#include "aos/cli.h"
#endif

#ifdef ULOG_MOD
#undef ULOG_MOD
#endif /* ULOG_MOD */

#define ULOG_MOD "ULOG_DEMO"
#define TEST_STR  "this is ulog tester"

static char show_log_list_buffer[512];

void ulog_example(int argc, char **argv)
{
    int i = 0;
    aos_set_log_level(AOS_LL_DEBUG);
    while (1) {
        LOGI(ULOG_MOD, "%d-%s", i, TEST_STR);
        i++;
        if (10 == i) {
#if ULOG_CONFIG_POP_FS
            if (0 == aos_get_ulog_list(show_log_list_buffer, sizeof(show_log_list_buffer))) {
                LOG("Log List %s", show_log_list_buffer);
            }
#endif
        return;
        }
        aos_msleep(500);
    }
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(ulog_example, ulog_example, ulog component base example)
#endif
