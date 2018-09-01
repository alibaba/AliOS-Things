/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <aos/aos.h>
#include <atparser.h>
#include <aos/network.h>

#include "athost_io.h"
#include "athost.h"

#define TAG "athostapp"

static int at_read(char *outbuf, uint32_t len)
{
    return at.parse(outbuf, len);
}

static int at_write(const char *header, const uint8_t *data, uint32_t len,
                    const char *tailer)
{
    return at.send_data_3stage_no_rsp(header, data, len, tailer);
}

static int at_handle_register_cb(const char              *prefix,
                                 athost_atcmd_handle_cb_t fn)
{
    at.oob(prefix, NULL, 0, fn, NULL);
    return 0;
}

athost_io_t athost_io = { .at_read               = at_read,
                          .at_write              = at_write,
                          .at_handle_register_cb = at_handle_register_cb };

static void app_delayed_action(void *arg)
{
    LOG("AT host server: alive %s:%d %s\r\n", __func__, __LINE__,
        aos_task_name());
    aos_post_delayed_action(50000, app_delayed_action, NULL);
}

int application_start(int argc, char *argv[])
{
    at.set_mode(ASYN);
    // mk3060: 4096 mk3165: 1024
    at.set_worker_stack_size(4096);
    at.init(AT_RECV_PREFIX, AT_RECV_SUCCESS_POSTFIX, AT_RECV_FAIL_POSTFIX,
            AT_SEND_DELIMITER, 1000);

    athost_io_register(&athost_io);

    athost_instance_init();

    LOG("NEW AT host server start!\n");
    aos_post_delayed_action(1000, app_delayed_action, NULL);

    aos_loop_run();

    return 0;
}
