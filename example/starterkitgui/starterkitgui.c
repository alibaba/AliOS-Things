/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>

extern void GUIDEMO_Main(void);

static void app_delayed_action(void *arg)
{
    LOG("helloworld %s:%d %s\r\n", __func__, __LINE__, aos_task_name());
    aos_post_delayed_action(5000, app_delayed_action, NULL);
}

static void gui_loop(void *arg)
{
    GUIDEMO_Main();
}

int application_start(int argc, char *argv[])
{
    aos_post_delayed_action(1000, app_delayed_action, NULL);
    BSP_GUI_init();

    aos_task_new("gui", gui_loop, 0, 4096);
    aos_loop_run();

    return 0;
}
