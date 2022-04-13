/*
 * Copyright (C) 2021-2023 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include "wifi_camera.h"
#include "ucamera_service.h"

#ifdef AOS_COMP_CLI
#include "aos/cli.h"
#endif

#define CAPTURED_IMAGE "/data/capture.jpg"

static void ucamera_comp_example(int argc, char **argv)
{
    int ret;
    frame_buffer_t *frame = NULL;

    if (argc < 3)
        printf("wrong parameter number\n");

    if (!strcmp(argv[1], "-t")) {
        if (!strcmp(argv[2], "wifi")) {
            ret = ucamera_service_init("wifi_camera");
            if (ret < 0) {
                printf("ucamera service init fail\n");
                return;
            } else {
                printf("ucamera service init ok\n");
                frame = ucamera_service_get_frame();
                if (!frame) {
                    printf("ucamera get frame fail\n");
                    return;
                } else {
                    printf("ucamera get frame OK\n");
                }
                ret = ucamera_service_save_frame(frame, CAPTURED_IMAGE);
                if (ret < 0) {
                    printf("save image fail\n");
                    return;
                } else {
                    printf("save image to %s success\n", CAPTURED_IMAGE);
                }
            }
        } else {
            printf("unknown camera device type\n");
        }
    }
    return;
}

#ifdef AOS_COMP_CLI
/* reg args: fun, cmd, description*/
ALIOS_CLI_CMD_REGISTER(ucamera_comp_example, ucamera, ucamera component base example)
#endif
