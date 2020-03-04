/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include <stdio.h>
#include <stdlib.h>

#include <aos/kernel.h>
#include "aos/hal/uart.h"
#include "board.h"

#if (AOS_HAL_UART_ENABLED > 0)

#define TASK_RECVDATA_NAME      "revdata"
#define TASK_RECVDATA_STACKSIZE 512
#define TASK_RECVDATA_PRI       50

static uart_dev_t uart_demo;

/* task handle */
aos_task_t task_recvdata;

/* task entry */
static void task_recvdata_entry(void *arg)
{
    int  i;
    int  ret;
    char rev_buf[1];
    int  rev_length;

    while (1) {
        ret = hal_uart_recv_II(&uart_demo, rev_buf, sizeof(rev_buf), &rev_length, AOS_WAIT_FOREVER);
        if (ret != 0) {
            printf("recv data from uart error\r\n");
            break;
        }

        printf("recv data from uart:\r\n");
        for(i = 0; i < rev_length; i++) {
            printf("%c ", rev_buf[i]);
        }
        printf("\r\n");
    }
}

static int uart_app_init()
{
    int ret;

    if (PORT_UART_DEMO == PORT_UART_INVALID) {
        printf("Since the board has no uart for testing, this demo is not supported\r\n");
        return -1;
    }

    uart_demo.port                = PORT_UART_DEMO;
    uart_demo.config.baud_rate    = 115200;
    uart_demo.config.mode         = MODE_TX_RX;
    uart_demo.config.flow_control = FLOW_CONTROL_DISABLED;
    uart_demo.config.stop_bits    = STOP_BITS_1;
    uart_demo.config.parity       = NO_PARITY;
    uart_demo.config.data_width   = DATA_WIDTH_8BIT;

    ret = hal_uart_init(&uart_demo);
    if(ret != 0) {
        printf("init uart error\r\n");
        return -1;
    }

    return ret;
}

void hal_uart_app_run(void)
{
    int  ret;
    char send_buf[20];
    int  send_length;

    ret = uart_app_init();
    if (ret == -1) {
        return;
    }

    send_length = snprintf(send_buf, sizeof(send_buf), "hello world\r\n");

    printf("Send data from the test uart\r\n");
    ret = hal_uart_send(&uart_demo, send_buf, send_length,  AOS_WAIT_FOREVER);

    /* Create the task to receive data */
    ret = aos_task_new_ext(&task_recvdata, TASK_RECVDATA_NAME, task_recvdata_entry, NULL,
                           TASK_RECVDATA_STACKSIZE, TASK_RECVDATA_PRI);
    if (ret != 0) {
        hal_uart_finalize(&uart_demo);
        printf("create uart data recv task error\r\n");
        return;
    }
}
#endif

