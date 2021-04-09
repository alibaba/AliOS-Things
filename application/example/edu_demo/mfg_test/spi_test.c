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
#define MNF_ID 0xEF

static int spi_test_internal()
{
#if 1
    static int32_t init_flag = 0;
    int32_t ret = 0;
    uint32_t retry = 0;
    spi_dev_t spi0, spi1;
    uint8_t pattion[4] = {0xaa, 0x55, 0xFF, 0x00};
    uint8_t data_back[4];

#define MAX_RETRY_CNT 3

    spi0.port = 0;
    spi0.config.data_size = SPI_DATA_SIZE_8BIT;
    spi0.config.mode = SPI_WORK_MODE_3;
    spi0.config.cs = 1;
    spi0.config.freq = 2000000;
    spi0.config.role = SPI_ROLE_MASTER;
    spi0.config.firstbit = SPI_FIRSTBIT_MSB;
    spi0.config.t_mode = SPI_TRANSFER_NORMAL;

	ret = hal_spi_init(&spi0);
    if (ret) {
        printf("===spi test : spi init fail \r\n");
        goto fail;
    }
    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("*********************** spi Test *****************************\r\n");
    printf("** How to test: pls connect Pin17 MOSI, Pin18 MISO, Pin21 CS***\r\n");
    printf("** Pin22 CLK **************************************************\r\n");
    printf("***************************************************************\r\n");
    printf("\r\n ===spi test start====\r\n");

    retry = 0;

    do {
        int i = 0;
        aos_msleep(1000);
        ret = hal_spi_send(&spi0, pattion, sizeof(pattion), 10);
        if (ret) {
            printf("spi send and recv fail\r\n");
            continue;
        }
        for(i=0; i<sizeof(pattion); i++)
        {
            if(pattion[i] != data_back[i])
            {
                goto fail;
            }
        }
        retry++;

    } while(ret == 0 && retry < MAX_RETRY_CNT);

    if (retry >= MAX_RETRY_CNT) {
        goto fail;
    }
    printf("===Result : SPI test PASS !!! ===\r\n");
    return 0;

fail:
    printf("===Warning: SPI test FAIL !!! ===\r\n");
    return -1;
}

static void handle_spitest_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    spi_test_internal();
}

static struct cli_command spitest_cmd = {
    .name = "spitest",
    .help = "spitest",
    .function = handle_spitest_cmd};

int spi_test()
{
    aos_cli_register_command(&spitest_cmd);
    return spi_test_internal();
}
