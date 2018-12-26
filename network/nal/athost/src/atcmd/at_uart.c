/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aos/kernel.h"

#include "at_util.h"

#define MAX_ATCMD_ECHO_BUF_SIZE 1024
static bool uart_echo_on = false;
static bool inited       = false;

static int uart_echo()
{
    char atcmd_prefix                 = "AT+";
    char buf[MAX_ATCMD_ECHO_BUF_SIZE] = { 0 };
    int  i                            = strlen(atcmd_prefix);

    do {
        if (!uart_echo_on) {
            return 0;
        }

        if (i >= sizeof(buf) - 1) {
            LOGE(TAG, "Echo fail, msg too long\r\n");
            break;
        }

        if (atcmd_read(&buf[i], 1) <= 0) {
            LOGE(TAG, "read fail\r\n");
            break;
        }

        // end of message then echo
        if (buf[i] == '\r') {
            buf[i] = '\0';

            memcpy(buf, atcmd_prefix, strlen(atcmd_prefix));
            // notify_atcmd_recv_status(ATCMD_SUCCESS);
            if (strstr(buf, "UARTE=OFF") != NULL) {
                uart_echo_on = false;
                break;
            }

            LOGD(TAG, "Echo server recv msg len %d\n", i);

            buf[i]     = '\r';
            buf[i + 1] = '\0';

            insert_uart_send_msg(buf, NULL, strlen(buf), 0);
            break;
        }
        i++;
    } while (1);

    return 1;
}


// AT+UARTE=OFF
static int atcmd_uart_echo()
{
    int  ret;
    char single;
    char body[10] = { 0 };
    bool echo;

    if (!inited) {
        LOGE(TAG, "uart not inited yet!");
        goto err;
    }

    // Eat '='
    atcmd_read(&single, 1);
    if ('=' != single) {
        LOGE(TAG, "Invalid cip start prefix %c !", single);
        goto err;
    }

    // echo setting
    ret = atcmd_socket_data_info_get(body, sizeof(body), NULL);
    if (ret < 0) {
        LOGE(TAG, "Invalid command %s !!!\r\n", body);
        goto err;
    }

    if (memcmp(body, "ON", strlen(body)) == 0) {
        uart_echo_on = true;
    } else if (memcmp(body, "OFF", strlen(body)) == 0) {
        uart_echo_on = false;
    } else {
        LOGE(TAG, "Invalid command %s !!!\r\n", body);
        goto err;
    }

    LOGD(TAG, "UART echo done!\n");
    notify_atcmd_recv_status(ATCMD_SUCCESS);

    return 0;

err:
    notify_atcmd_recv_status(ATCMD_FAIL);
    return -1;
}

enum
{
    ATCMD_UART_ECHO = 0,
};

static const atcmd_hdl_t at_uart_cmds_table[] = {
    { .name     = "AT+UARTE",
      .help     = "AT+UARTE=<ON/OFF>",
      .function = atcmd_uart_echo },
};

static atcmd_hdl_ptr_t get_atcmd_uart_handler()
{
    const char *cmd_prefix                   = "ART";
    char        prefix[MAX_ATCMD_PREFIX_LEN] = { 0 };
    char        single;
    int         index = -1;

    // Eat "ART"
    atcmd_read(prefix, strlen(cmd_prefix));
    if (memcmp(prefix, cmd_prefix, strlen(cmd_prefix)) != 0) {
        LOGE(TAG, "invalid uart prefix %s\n", prefix);
        return;
    }

    atcmd_read(&single, 1);

    switch (single) {
        case 'E':
            index = ATCMD_UART_ECHO;
            break;

        default:
            LOGE(TAG, "invalid uart prefix %c\n", single);
            break;
    }

    if (index >= 0) {
        return  (atcmd_hdl_ptr_t) &at_uart_cmds_table[index];
    }

    return NULL;
}

static int at_uart_init()
{
    inited = true;
    return 0;
}

static void at_uart_deinit()
{
    inited = false;
    return;
}

atcmd_op_t uart_op = {
    .name              = "uart",
    .prefix            = "U",
    .init              = at_uart_init,
    .deinit            = at_uart_deinit,
    .get_atcmd_handler = get_atcmd_uart_handler,
};

int register_atcmd_uart_opt()
{
    return atcmd_register_handlers(&uart_op);
}
