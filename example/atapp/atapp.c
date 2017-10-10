/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <stdio.h>
#include <string.h>
#include "atapp.h"
#include "atparser.h"
#include <hal/soc/uart.h>
#include <aos/aos.h>
#include "netmgr.h"

#define SSID "alibaba-guest"
#define PASSWD "sh@alibaba"

uart_dev_t uart_1;

static void at_task1()
{
    char out[48];
    if (at.send_raw("AT", out) == 0)
        printf("AT command succeed, rsp: %s", out);
    else
        printf("AT command failed\r\n");
}

static void at_task2()
{
    char out[48];
    if (at.send_raw("TEST", out) == 0)
        printf("AT command succeed, rsp: %s", out);
    else
        printf("AT command failed\r\n");
}

static void handle_at(char *pwbuf, int blen, int argc, char **argv)
{
    char *in, out[256], *rtype = argc > 1 ? argv[1] : "";

    if (strcmp(rtype, "simple") == 0) {
        if (argc != 3) {printf("Incorrect argument\r\n"); return;}
        printf("Simple AT command test\r\n");
        in = argv[2];
        printf("AT> %s\r\n", in);

        if (at.send_raw(in, out) == 0)
            printf("AT command succeed, rsp: %s", out);
        else
            printf("AT command failed\r\n");
    } else if (strcmp(rtype, "quit") == 0) {
        printf("quit uart\r\n");
        hal_uart_finalize(&uart_1);
    } else {
        printf("Full AT command test\r\n");
        aos_task_new("at_task1", at_task1, NULL, 1024);
        printf("New task1 created.\r\n");
        aos_task_new("at_task2", at_task2, NULL, 1024);
        printf("New task2 created.\r\n");
    }
}

/**
 * This func is called when recv finds oob data in the buffer.
 * It's expected to either queue the oob data for someone else
 * to consume later, or trigger the consuming immediately.
*/
static void socket_handler(void *arg)
{
    printf("Hello, socket received.\r\n");
    // <TODO>
}

static struct cli_command atcmds[] = {
    {
        .name = "at",
        .help = "at [simple|full] [input] [expected_out]",
        .function = handle_at
    }
};

static void wifi_event_handler(input_event_t *event, void *priv_data)
{
    if (event->type != EV_WIFI) return;
    if (event->code == CODE_WIFI_ON_PRE_GOT_IP)
        LOG("Hello, WiFi PRE_GOT_IP event!");
    if (event->code == CODE_WIFI_ON_GOT_IP)
        LOG("Hello, WiFi GOT_IP event!");
}

int application_start(int argc, char *argv[])
{
    printf("Hello app started\r\n");

    aos_set_log_level(AOS_LL_DEBUG);

    uart_1.port                = 1;
    uart_1.config.baud_rate    = 115200;
    uart_1.config.data_width   = DATA_WIDTH_8BIT;
    uart_1.config.parity       = NO_PARITY;
    uart_1.config.stop_bits    = STOP_BITS_1;
    uart_1.config.flow_control = FLOW_CONTROL_DISABLED;

    at.init(&uart_1, "\r\n", "\r\n", 1000);
    at.set_mode(ASYN);
    at.oob("+CIPEVENT:SOCKET", socket_handler, NULL);

    aos_register_event_filter(EV_WIFI, wifi_event_handler, NULL);

    aos_cli_register_commands((const struct cli_command *)&atcmds[0], 
      sizeof(atcmds) / sizeof(atcmds[0]));

    netmgr_init();
    netmgr_start(false);

    aos_loop_run();

    return 0;
}

