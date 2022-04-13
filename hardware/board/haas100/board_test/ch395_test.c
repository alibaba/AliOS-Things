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
#include "netmgr.h"

static int ch395_spi_test_internal()
{
    static int32_t init_flag = 0;
    int32_t ret = 0;
    uint32_t retry = 0;
    spi_dev_t spi;
    uint8_t linkstatus = 0;
    uint8_t dhcp_res = 0;
    uint8_t ip_info[20] = {0};
    uint8_t mac[6] = {0};


#define MAX_RETRY_CNT    20

    spi.port = 0;
    spi.config.data_size = SPI_DATA_SIZE_8BIT;
    spi.config.mode = SPI_WORK_MODE_3;
    spi.config.cs = 0;
    spi.config.freq = 26000000;
    spi.config.role = SPI_ROLE_MASTER;
    spi.config.firstbit = SPI_FIRSTBIT_MSB;
    spi.config.t_mode = SPI_TRANSFER_NORMAL;

    printf("\r\n\r\n");
    printf("***************************************************************\r\n");
    printf("*********************** spi1 eth Test *************************\r\n");
    printf("** How to test: pls connect the network cable to the router  **\r\n");
    printf("***************************************************************\r\n");
    printf("\r\n ===spi1 eth test start====\r\n");

    /*dereset the ch395 chip*/
    hal_gpio_pin_set_dir(HAL_IOMUX_PIN_P3_3, HAL_GPIO_DIR_OUT, 0);

    aos_msleep(1000);

    hal_gpio_pin_set_dir(HAL_IOMUX_PIN_P3_3, HAL_GPIO_DIR_OUT, 1);

    aos_msleep(50);


    ret = ch395_sal_add_dev(&spi);
    if (ret) {
        printf("===spi eth test : ch395 spi init fail \r\n");
        goto fail;
    }

    ret = ch395_dev_init();
    if (ret) {
        printf("===spi eth test : ch395 dev init fail \r\n");
        goto fail;
    }

    ret = ch395_get_mac_addr(mac);
    if (ret) {
        printf("===spi eth test : ch395 get mac fail \r\n");
        goto fail;
    }

    printf("===spi eth test : get mac 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x====\r\n",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    aos_msleep(2000);

    retry = 0;

    do {
        aos_msleep(1000);
        ret = ch395_get_phy_status(&linkstatus);
        if (ret) {
            printf("Fail to get link status 0x%x, nothing to do \r\n", ret);
            continue;
        }

        printf("===spi eth test : get linkstatus 0x%x====\r\n", linkstatus);

        if (linkstatus == 0x08) {
            printf("===spi eht test : Link is up, 100M full duplex \r\n");
            break;
        }
        retry++;
    } while(ret == 0 && retry < MAX_RETRY_CNT);

    if (retry >= MAX_RETRY_CNT) {
        printf("===spi eht test : Link is still down , please check the eth line \r\n");
        goto fail;
    }

    printf("===spi eht test : Link is up start to enable dhcp \r\n");

    ret = ch395_dhcp_enable(1);
    if (ret) {
        printf("===spi eht test : dhcp enbale fail ===\r\n");
        goto fail;
    }

    retry = 0;
    do {
        aos_msleep(1000);
        ret = ch395_dhcp_get_status(&dhcp_res);
        if (ret) {
            printf("===spi eht test : Fail to get dhcp status 0x%x===\r\n", ret);
            continue;
        }
        printf("===spi eht test : Get dhcp result 0x%x====\r\n", dhcp_res);
        if (dhcp_res == 0) {
            printf("===spi eht test : dpch result is ok===\r\n");
            break;
        }
        retry++;
    } while(ret == 0 && retry < MAX_RETRY_CNT);

    if (retry >= MAX_RETRY_CNT) {
        printf("===spi eht test : dhcp result is fail ===\r\n");
        goto fail;
    }

    retry = 0;

    do {
        aos_msleep(2000);
        memset(ip_info, 0, sizeof(ip_info));
        ret = ch395_get_ip_interface(ip_info);
        if (ret){
            printf("===spi eht test : Failed to get ip address info===");
            goto fail;
        }

        printf("===spi eht test : get ip info %d.%d.%d.%d gateway %d.%d.%d.%d mask %d.%d.%d.%d dns1 %d.%d.%d.%d \
        dns2 %d.%d.%d.%d\r\n",
                ip_info[0], ip_info[1], ip_info[2], ip_info[3], ip_info[4], ip_info[5], ip_info[6], ip_info[7],
                ip_info[8], ip_info[9], ip_info[10], ip_info[11], ip_info[12], ip_info[13], ip_info[14], ip_info[15],
                ip_info[16], ip_info[17], ip_info[18], ip_info[19]);
        if (ip_info[0] != 0) {
            break;
        }
        retry++;
    } while(ret == 0 && retry < MAX_RETRY_CNT);

    if (retry >= MAX_RETRY_CNT) {
        printf("===spi eht test : Invalid ip addr got ===\r\n");
        goto fail;
    }


    ret = ch395_ping_enable(1);
    if (ret) {
        printf("===spi eht test : Failed to enable ping===");
        goto fail;
    }

    printf("===Result : spi1 eth test PASS !!! ===\r\n");

    return 0;
fail:
    printf("===Warning: spi1 eth test FAIL !!! ===\r\n");
    return -1;
}

static void handle_ch395test_cmd(char *pwbuf, int blen, int argc, char **argv)
{
    ch395_spi_test_internal();
}

static struct cli_command ch395test_cmd = {
    .name     = "ch395test",
    .help     = "ch395test",
    .function = handle_ch395test_cmd
};

int ch395_spi_test()
{
    aos_cli_register_command(&ch395test_cmd);
    return ch395_spi_test_internal();
}
