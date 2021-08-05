/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>
#include <aos/errno.h>
#include <aos/kernel.h>
#include "aos/init.h"
#include "board.h"
#include <k_api.h>
#include "aos/cli.h"

extern int usb_class_detect;

static int usb_test_internal()
{
    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("*********************** USB Test ******************************\r\n");
    printf("** How to test: ***********************************************\r\n");
    printf("***************************************************************\r\n");
    printf("***************************************************************\r\n");
    printf("\r\n ===usb test start====\r\n");
/*
    usb_class_detect = 1;

    if (usb_class_detect > 0) {
        printf("===Result : usb test PASS !!! ===\r\n");
        return 0;
    } else {
        printf("===Warning: usb test FAIL !!! ===\r\n");
        return -1;
    }
*/
    return 0;
}

static void handle_usbtest_cmd(char *pwbuf, int blen, int argc, char **argv)
{
     usb_test_internal();
}
static struct cli_command usbtest_cmd = {
    .name     = "usbtest",
    .help     = "usbtest",
    .function = handle_usbtest_cmd
};

int usb_test(void)
{
    aos_cli_register_command(&usbtest_cmd);
    return usb_test_internal();
}
