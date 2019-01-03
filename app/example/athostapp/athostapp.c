/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <string.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"

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
    ret = at_read(outbuf, len);
#endif
    return ret;
}

int HAL_Athost_Write(const char *header, const uint8_t *data, uint32_t len,
                     const char *tailer)
{
    int ret = 0;

#ifdef AOS_ATCMD
    if (!header) {
        LOGE(TAG, "Invalid null header\n");
        return -1;
    }

    if ((ret = at_send_no_reply(header, strlen(header), false)) != 0) {
        LOGE(TAG, "uart send packet header failed");
        return -1;
    }

    if (data && len) {
        if ((ret = at_send_no_reply((char *)data, len, false)) != 0) {
            LOGE(TAG, "uart send packet failed");
            return -1;
        }
    }

    if (tailer) {
        if ((ret = at_send_no_reply(tailer, strlen(tailer), false)) != 0) {
            LOGE(TAG, "uart send packet tailer failed");
            return -1;
        }
    }
#endif

    return ret;
}

int HAL_Athost_HandleRegisterCb(const char              *prefix,
                                athost_atcmd_handle_cb_t fn)
{
    int ret = 0;

#ifdef AOS_ATCMD
    at_register_callback(prefix, NULL, 0, fn, NULL);
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
