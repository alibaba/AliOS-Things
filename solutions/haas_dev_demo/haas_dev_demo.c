/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/init.h"
#include "board.h"
#include <aos/errno.h>
#include <aos/kernel.h>
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <uservice/uservice.h>
#include <uservice/eventid.h>
#include "aos/cli.h"
#include "haas_main.h"

#define AMP_TYPE_MAX_LEN 32
#define AMP_TYPE_JS  "JS"
#define AMP_TYPE_PYTHON "Python"

extern int amp_main(void);

static void amp_set_command(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usge: amp_set [JS/Python]\r\n");
        return;
    }

    if (strcmp(argv[1], AMP_TYPE_JS) == 0) {
        aos_kv_set("amp_type", AMP_TYPE_JS, strlen(AMP_TYPE_JS));
    } else if (strcmp(argv[1], AMP_TYPE_PYTHON) == 0) {
        aos_kv_set("amp_type", AMP_TYPE_PYTHON, strlen(AMP_TYPE_PYTHON));
    }
}

/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(amp_set_command, amp_set, set amp startup type)

int application_start(int argc, char *argv[])
{
    int len = 0;
    int ret = 0;
    aos_task_t amp_task;
    char amp_type[AMP_TYPE_MAX_LEN] = {0};

    printf("haas dev demp entry here!\r\n");

    len = 32;
    ret = aos_kv_get("amp_type", amp_type, &len);
    if ((ret == 0) && (strcmp(amp_type, AMP_TYPE_JS) == 0)) {
        /* Start JS engine. */
        aos_task_new_ext(&amp_task, "amp_task", amp_main, NULL, 8192, AOS_DEFAULT_APP_PRI - 2);
    } else {
        /* Start Python engine. */
        haas_main(argc, argv);
    }

    while (1) {
        aos_msleep(10000);
    };
}
