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
#include "aos/hal/spi.h"
#include "aos/yloop.h"

#include "led.h"
#include "key.h"
#include "hal_gpio.h"
#include "hal_iomux_haas1000.h"

#define READ_JEDEC_ID 0x9F
#define MNF_ID        0xEF

static int spi_test_internal()
{
    static int32_t init_flag = 0;
    int32_t ret = 0;
    uint32_t retry = 0;
    spi_dev_t spi;
    uint8_t command = READ_JEDEC_ID;
    uint8_t data[3] = {};

#define MAX_RETRY_CNT    3

    spi.port = 0;
    spi.config.data_size = SPI_DATA_SIZE_8BIT;
    spi.config.mode = SPI_WORK_MODE_3;
    spi.config.cs = 0;
    spi.config.freq = 2000000;
    spi.config.role = SPI_ROLE_MASTER;
    spi.config.firstbit = SPI_FIRSTBIT_MSB;
    spi.config.t_mode = SPI_TRANSFER_NORMAL;

    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("*********************** spi Test *****************************\r\n");
    printf("** How to test: pls connect Pin17 MOSI, Pin18 MISO, Pin21 CS***\r\n");
    printf("** Pin22 CLK **************************************************\r\n");
    printf("***************************************************************\r\n");
    printf("\r\n ===spi test start====\r\n");

	ret = hal_spi_init(&spi);
    if (ret) {
        printf("===spi test : spi init fail \r\n");
        goto fail;
    }

    retry = 0;

    do {
        aos_msleep(1000);
        ret = hal_spi_send_and_recv(&spi, &command, 1, data, 2, 1000);
        if (ret) {
            printf("spi send and recv fail\r\n");
            continue;
        }

        if (data[1] != MNF_ID) {
            printf("spi recv value is not correct, data: %x, %x\r\n", data[0], data[1]);
            continue;
        }
        else
        {
            break;
        }
        retry++;
    } while(ret == 0 && retry < MAX_RETRY_CNT);

    if (retry >= MAX_RETRY_CNT) {
        goto fail;
    }

    printf("===Result : spi test PASS !!! ===\r\n");

    return 0;
fail:
    printf("===Warning: spi test FAIL !!! ===\r\n");
    return -1;
}

static void handle_spitest_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    spi_test_internal();
}

static struct cli_command spitest_cmd = {
    .name     = "spitest",
    .help     = "spitest",
    .function = handle_spitest_cmd
};

int spi_test()
{
    aos_cli_register_command(&spitest_cmd);
    return spi_test_internal();
}
