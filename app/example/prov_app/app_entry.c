/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>

#include "aos/kernel.h"
#include "ulog/ulog.h"

#include <km.h>
#include <plat_gen.h>
#include <prov_test_id2.h>

/*
 * used to save id2 prov data,  which is got from id2 prov sdk.
 */
uint8_t id2_prov_data[] = {
    0x50, 0x72, 0x4f, 0x76, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x52, 0x65, 0x43, 0x6d,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0x50, 0x0a, 0x06,
    0x18, 0x00, 0x00, 0x00, 0x30, 0x30, 0x61, 0x64, 0x64, 0x64, 0x61, 0x61, 0x30, 0x32, 0x38, 0x35,
    0x43, 0x35, 0x36, 0x30, 0x34, 0x34, 0x34, 0x36, 0x46, 0x37, 0x30, 0x30, 0x52, 0x65, 0x43, 0x6d,
    0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x10, 0x00, 0x00, 0x00, 0x65, 0x9c, 0xe8, 0xa1,
    0x10, 0x00, 0x00, 0x00, 0x20, 0xfb, 0x17, 0x5d, 0x59, 0x54, 0xcf, 0x2c, 0x0d, 0x1a, 0xb4, 0x60,
    0x63, 0x66, 0xa0, 0x33, 0x52, 0x65, 0x43, 0x6d, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x19, 0x15, 0xa8, 0x29, 0x20, 0x00, 0x00, 0x00, 0x27, 0x60, 0x52, 0xc2,
    0xf8, 0x05, 0x7b, 0x01, 0xd3, 0xe1, 0x33, 0x5b, 0x41, 0x0e, 0xb4, 0xe5, 0x03, 0x52, 0x13, 0x56,
    0x81, 0x3c, 0xe0, 0x8d, 0x96, 0xb4, 0x12, 0xa7, 0xb5, 0x58, 0x06, 0x42
};

static void app_delayed_action(void *arg)
{
    int ret = 0;
#if defined(CONFIG_ERASE_KEY)
    int fd = 0;
    uint8_t data[4096] = {0};
#endif

#if defined(CONFIG_ERASE_KEY)
    fd = open_rsvd_part(RO_WRITE);
    if (fd < 0) {
        LOG("open rsvd partition failed\n");
        return;
    }

    ret = write_rsvd_part(fd, 0, data, 4096);
    if (ret < 0) {
        LOG("write rsvd partition failed\n");
        close_rsvd_part(fd);
        return;
    }

    close_rsvd_part(fd);
    LOG("key has been erased\n");
#else
    ret = prov_test_id2(id2_prov_data, sizeof(id2_prov_data));
    if (ret != 0) {
        LOG("id2 prov test failed\n");
        return;
    } else {
        LOG("id2 prov test success\n");
    }
#endif

    return;
}

int application_start(int argc, char *argv[])
{
    LOG("application started.");
    aos_post_delayed_action(1000, app_delayed_action, NULL);
    aos_loop_run();

    return 0;
}

