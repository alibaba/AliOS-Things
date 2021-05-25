/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include "aos/init.h"
#include "board.h"
#include <aos/errno.h>
#include <aos/kernel.h>
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <mbmaster.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <vfsdev/gpio_dev.h>

#define GPIO_PIN                (4 * 8 + 6)

static void test_bsm0404rb(mb_handler_t *mb, uint8_t addr, bool out)
{
    uint16_t coil_addr;

    if (!mb)
        return;

    for (coil_addr = 0x0000; coil_addr <= 0x0003; coil_addr++) {
        mb_status_t status;
        uint16_t r_addr;
        uint16_t r_val;
        uint8_t ec;

        status = mbmaster_write_single_coil(mb, addr, coil_addr,
                                            out ? 0xFF00 : 0x0000,
                                            &r_addr, &r_val, &ec, 250);
        if (status != MB_SUCCESS)
            printf("Failed to write coil.\r\n");
    }
}

static void test_raw_relay(int fd, bool out)
{
    struct gpio_io_config config;

    if (fd < 0)
        return;

    config.id = GPIO_PIN;
    config.config = GPIO_IO_OUTPUT | GPIO_IO_OUTPUT_PP;
    config.data = !!out;
    (void)ioctl(fd, IOC_GPIO_SET, (unsigned long)&config);
}

int application_start(int argc, char *argv[])
{
    mb_handler_t *mb;
    mb_status_t status;
    bool out = false;
    int fd;

    status = mbmaster_rtu_init(&mb, 1, 9600, MB_PAR_NONE);
    if (status != MB_SUCCESS) {
        printf("Failed to init mbmaster.\r\n");
        mb = NULL;
    }

    fd = open("/dev/gpio", 0);
    if (fd < 0)
        printf("Failed to init GPIO.\r\n");

    while (1) {
        test_bsm0404rb(mb, 1, out);
        test_raw_relay(fd, out);
        out = !out;
        aos_msleep(1000);
    };

    close(fd);
    fd = -1;
}
