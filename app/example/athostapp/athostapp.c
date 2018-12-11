/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <aos/aos.h>

#ifdef AOS_ATCMD
#include <atparser.h>
#endif

#include "athost_export.h"
#include "athost_import.h"

#define TAG "athostapp"

int HAL_Athost_Read(char *outbuf, uint32_t len)
{
    int ret = 0;

#ifdef AOS_ATCMD
    ret = at.parse(outbuf, len);
#endif
    return ret;
}

int HAL_Athost_Write(const char *header, const uint8_t *data, uint32_t len,
                     const char *tailer)
{
    int ret = 0;

#ifdef AOS_ATCMD
    ret = at.send_data_3stage_no_rsp(header, data, len, tailer);
#endif

    return ret;
}

int HAL_Athost_HandleRegisterCb(const char              *prefix,
                                athost_atcmd_handle_cb_t fn)
{
    int ret = 0;

#ifdef AOS_ATCMD
    at.oob(prefix, NULL, 0, fn, NULL);
#endif

    return ret;
}

static void app_delayed_action(void *arg)
{
    LOG("AT host server: alive %s:%d %s\r\n", __func__, __LINE__,
        aos_task_name());
    aos_post_delayed_action(50000, app_delayed_action, NULL);
}

int application_start(int argc, char *argv[])
{
#ifdef AOS_ATCMD
    at_init();
#endif

    athost_instance_init();

    LOG("NEW AT host server start!\n");
    aos_post_delayed_action(1000, app_delayed_action, NULL);
    aos_loop_run();

    return 0;
}
