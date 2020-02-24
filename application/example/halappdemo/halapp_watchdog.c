/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>

#include <aos/kernel.h>
#include "aos/hal/wdg.h"
#include "board.h"

#if (AOS_HAL_WDG_ENABLED > 0)

/* task parameters */
#define TASK_FEEDWDG_NAME      "feedwdg"
#define TASK_FEEDWDG_STACKSIZE 512
#define TASK_FEEDWDG_PRI       50

/* task handle */
aos_task_t task_feedwdg;

static wdg_dev_t watchdog;

/* task entry */
static void task_feedwdg_entry(void *arg)
{
    /**
     * The task fed the watchdog every 1000ms. The feeding interval must be less than the
     * watchdog timeout, otherwise it will trigger by mistake.
     */
    while (1) {
        printf("feed the watchdog!\r\n");
        hal_wdg_reload(&watchdog);

        aos_msleep(1000);
    }
}

void hal_watchdog_app_enable(void)
{
    int ret;

    /* wdg port set */
    watchdog.port = PORT_WDG_0;

    /*  set reload time to 1500ms */
    watchdog.config.timeout = 1500;

    /* init watchdog with the given settings */
    ret = hal_wdg_init(&watchdog);
    if (ret != 0) {
        printf("watchdog init error !\n");
        return -1;
    }

    /* Create the task to feed the watchdog */
    ret = aos_task_new_ext(&task_feedwdg, TASK_FEEDWDG_NAME, task_feedwdg_entry, NULL,
                           TASK_FEEDWDG_STACKSIZE, TASK_FEEDWDG_PRI);
    if (ret != 0) {
        printf("create watchdog task error\r\n");
        ret = hal_wdg_finalize(&watchdog);
        if (ret != 0) {
            printf("delete watchdog error!\r\n");
        }
        return;
    }

    aos_msleep(10000);

    /* warning: This is just to show the effect of not feeding the dog.
     * When the watchdog feeding task is deleted, the watchdog timeout will
     * trigger the system reboot. */
    printf("delete the watchdog fedding task\r\n");
    aos_task_delete(TASK_FEEDWDG_NAME);
}
#endif

