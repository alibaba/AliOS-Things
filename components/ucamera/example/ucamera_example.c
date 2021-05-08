/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include "wifi_camera.h"
#include "ucamera_service.h"
#include "ulog/ulog.h"

#if AOS_COMP_CLI
#include "aos/cli.h"
#endif
#define TAG "ucamera_example"
#define LOG printf

#define CAPTURED_IMAGE "/data/capture.jpg"

static void ucamera_comp_example(int argc, char **argv)
{
    int ret;
    frame_buffer_t *frame = NULL;

    if (argc < 3)
        LOG("wrong parameter number\n");

    if (!strncmp(argv[1], "init", 4)) {
        /*init network*/
        event_service_init(NULL);
        netmgr_service_init(NULL);
        LOG("ucamera comp init successfully!\n");
    } else if (!strncmp(argv[1], "-t", 2)) {
        if (!strcmp(argv[2], "wifi")) {
            /*init ucamera service*/
            ret = ucamera_service_init("wifi_camera");
            if (ret < 0) {
                LOGE(TAG, "ucamera service init fail\n");
                return;
            }

            /*get one camera frame*/
            frame = ucamera_service_get_frame();
            if (!frame) {
                LOGE(TAG, "ucamera get frame fail\n");
                return;
            } else {
                LOG("ucamera get frame OK!\n");
            }

            /*save frame to jpeg file*/
            ret = ucamera_service_save_frame(frame, CAPTURED_IMAGE);
            if (ret < 0) {
                LOGE(TAG, "save image fail\n");
                return;
            } else {
                LOG("save image to %s successfully!\n", CAPTURED_IMAGE);
            }

            /*uninit ucamera service*/
            ucamera_service_uninit();
        } else {
            LOG("unknown camera device type!\n");
        }
    }
    return;
}

#if AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(ucamera_comp_example, ucamera, ucamera component base example)
#endif
