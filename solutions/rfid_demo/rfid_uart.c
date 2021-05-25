/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <aos/kernel.h>
#include "aos/hal/gpio.h"
#include "ulog/ulog.h"
#include "k_api.h"
#include <vfsdev/uart_dev.h>

#define TASK_RECVDATA_NAME      "revdata"
#define TASK_RECVDATA_STACKSIZE 4096
#define TASK_RECVDATA_PRI       50

static int fd = 0;

/* task entry */
static void task_recvdata_entry(void *arg)
{
    int  i = 0;
    int  ret = 0;
    char rfid_data_buf[50] = {0};
    int  rev_length = 0;
    char params[30];
    char rfid_deviceid[12];

    while (1) {
        ret = read(fd, rfid_data_buf, sizeof(rfid_data_buf));
        if (ret > 0) {
            printf("read length:%d\r\n", ret);
            for (i = 0; i < ret; i++) {
                printf("%02x ", rfid_data_buf[i]);
            }
            printf("\r\n");
            if ((rfid_data_buf[0] == 0x20) && (rfid_data_buf[1] == 0x0) && (rfid_data_buf[2] == 0x0) && (rfid_data_buf[3] == 0x8)) {
                snprintf(rfid_deviceid, sizeof(rfid_deviceid), "%02x%02x%02x%02x", rfid_data_buf[8], rfid_data_buf[9], rfid_data_buf[10], rfid_data_buf[11]);
                memset(rfid_data_buf, 0, sizeof(rfid_data_buf));
                printf("rfid_deviceid:%s\n", rfid_deviceid);
                snprintf(params, sizeof(params), "{\"card_id\": \"%s\"}", rfid_deviceid);
                printf("params:%s\n", params);
                send_property_post(params);
            }
        }
        usleep(1000);
    }
}

int rfid_uart_init(void)
{
    int port_id = 2;
    int ret = 0;
    char dev_name[16] = {0};
    aos_task_t task_recvdata;

    snprintf(dev_name, sizeof(dev_name), "/dev/ttyUART%d", port_id);
    printf("opening device:%s\r\n", dev_name);
    fd = open(dev_name, 0);
    if (fd < 0) {
        printf("open uart error\r\n");
        return fd;
    }
    ret = ioctl(fd, IOC_UART_SET_CFLAG, B9600 | CS8);
    if (ret != 0) {
        close(fd);
        printf("ioctl uart error\r\n");
        return ret;
    }

    /* Create the task to receive data */
    ret = aos_task_new_ext(&task_recvdata, TASK_RECVDATA_NAME, task_recvdata_entry, NULL,
                           TASK_RECVDATA_STACKSIZE, TASK_RECVDATA_PRI);
    if (ret != 0) {
        close(fd);
        printf("create uart data recv task error\r\n");
        return ret;
    }
    return ret;
}
