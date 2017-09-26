/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <aos/aos.h>

#define AOS_START_STACK 2048

ktask_t *g_aos_init;

extern int application_start(int argc, char **argv);

static void aos_init(void)
{
    soc_system_init();

#ifdef BOOTLOADER
    main();
#else
    aos_framework_init();

    application_start(0, NULL);
#endif
}

void aos_start(void)
{
    krhino_init();

    soc_driver_init();

    krhino_task_dyn_create(&g_aos_init, "aos-init", 0, 10, 0, AOS_START_STACK, aos_init, 1);

    krhino_start();
}

