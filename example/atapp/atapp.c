/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>
#include <atparser.h>
#include <netmgr.h>
#include <hal/atcmd.h>
#ifdef AOS_AT_ADAPTER
#include <aos/network.h>
#include <at_adapter.h>
#endif
#include "atapp.h"

uart_dev_t uart_1;

static void at_task1()
{
    char out[64];
    if (at.send_raw("AT", out, sizeof(out)) == 0)
        printf("AT command succeed, rsp: %s", out);
    else
        printf("AT command failed\r\n");
}

static void at_task2()
{
    char out[64];
    if (at.send_raw("TEST", out, sizeof(out)) == 0)
        printf("AT command succeed, rsp: %s", out);
    else
        printf("AT command failed\r\n");
}

static void handle_at(char *pwbuf, int blen, int argc, char **argv)
{
    char *in, out[256], *rtype = argc > 1 ? argv[1] : "";

    if (strcmp(rtype, "simple") == 0) {
        if (argc != 3) {aos_cli_printf("Incorrect argument\r\n"); return;}
        aos_cli_printf("Simple AT command test\r\n");
        in = argv[2];
        aos_cli_printf("AT> %s\r\n", in);

        if (at.send_raw(in, out, sizeof(out)) == 0)
            aos_cli_printf("AT command succeed, rsp: %s\r\n", out);
        else
            aos_cli_printf("AT command failed\r\n");
    } else if (strcmp(rtype, "quit") == 0) {
        aos_cli_printf("quit uart\r\n");
        hal_uart_finalize(&uart_1);
    } else if (strcmp(rtype, "2stage") == 0) {
        if (argc != 4) {aos_cli_printf("Incorrect argument\r\n"); return;}
        aos_cli_printf("2stage AT command test\r\n");
        char *fst = argv[2];
        char *data = argv[3];
        uint32_t len = strlen(data);
        aos_cli_printf("AT> %s %d %s\r\n", fst, len, data);

        if (at.send_data_2stage(fst, data, len, out, sizeof(out)) == 0)
            aos_cli_printf("AT command succeed, rsp: %s\r\n", out);
        else
            aos_cli_printf("AT command failed\r\n");
    } else {
        aos_cli_printf("Full AT command test\r\n");
        aos_task_new("at_task1", at_task1, NULL, 1024);
        aos_cli_printf("New task1 created.\r\n");
        aos_task_new("at_task2", at_task2, NULL, 1024);
        aos_cli_printf("New task2 created.\r\n");
    }
}

#ifdef AOS_AT_ADAPTER
#define MAXDATASIZE 100
#define NET_SEND_DATA_SIZE 1000
static char net_test_data[NET_SEND_DATA_SIZE] = {0};

struct enet_info_s {
    char ip[16];
    char *data;
    int len;
};

static void at_enet_helper(void *arg)
{
    struct enet_info_s *info = (struct enet_info_s *)arg;
    int ret, num;
    int fd = -1;
    struct sockaddr_in saddr, recvaddr;
    char buf[MAXDATASIZE];
    in_port_t port = 12345;
    socklen_t addrlen = sizeof(saddr);

    if (!info || !info->data) {
        printf("info or info->data is NULL\r\n");
        goto end;
    }

    fd = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(info->ip);
    ret = sendto(fd, info->data, info->len, 0,
      (struct sockaddr *)&saddr, sizeof(saddr));
    if (ret < 0) printf("Error: sendto failed\r\n");

    printf("%s: sendto OK, fd: %d, ip: %08x, port: %d\r\n", __func__,
      fd, saddr.sin_addr.s_addr, saddr.sin_port);

    while (1) {
        if((num = recvfrom(fd, buf, MAXDATASIZE, 0,
          (struct sockaddr *)&recvaddr, &addrlen)) < 0) {
            printf("recvfrom() error\n");
            break;
        }

        if (addrlen != sizeof(saddr) || memcmp((const void *)&saddr.sin_addr,
          (const void *)&recvaddr.sin_addr, sizeof(struct in_addr)) != 0 ||
          saddr.sin_port != recvaddr.sin_port) {
            printf("sizeof(saddr): %d\r\n", sizeof(saddr));
            printf("Message: addrlen: %d, ip - %08x, port - %d\n",
              addrlen, recvaddr.sin_addr.s_addr, recvaddr.sin_port);
            continue;
        }

        buf[num]='\0';
        printf("Server Message:%s\n", buf);

        break;
    }

    close(fd);

end:
    if (info) {
        if (info->data) aos_free(info->data);
        aos_free(info);
    }
}

static void handle_test_at_enet(char *pwbuf, int len, int argc, char **argv)
{
    char *rtype = argc > 1 ? argv[1] : "";
    char *d;
    void *tsk;

    if (strcmp(rtype, "udp") == 0) {
        struct enet_info_s *info;

        aos_cli_printf("udp type test, argc: %d\r\n", argc);

        if (argc != 4 && argc != 3) {aos_cli_printf("Invalid argument\r\n"); return;}

        if ((info = (struct enet_info_s *)aos_malloc(
          sizeof(struct enet_info_s))) == NULL) {
            aos_cli_printf("aos_malloc failed (len: %d)\r\n",
              sizeof(struct enet_info_s));
            return;
        }

        memcpy(info->ip, argv[2], strlen(argv[2]) + 1);
        if (argc == 4) {
            d = argv[3];
            info->len = strlen(d) + 1;
        } else {
            d = net_test_data;
            info->len = sizeof(net_test_data);
        }

        if ((info->data = (char *)aos_malloc(info->len)) == NULL) {
            aos_cli_printf("aos_malloc failed (len: %d)\r\n", info->len);
            aos_free(info);
            return;
        }
        memcpy(info->data, d, info->len);

        tsk = aos_loop_schedule_work(0, at_enet_helper, info, NULL, NULL);
        if (!tsk) {
            aos_cli_printf("Failed to created task in %s\r\n", __func__);
        }
    }

    aos_cli_printf("udp type test finished\r\n");
}
#endif


static struct cli_command atcmds[] = {
    {
        .name = "at",
        .help = "at [simple|full|2stage] [input|fst] [data]",
        .function = handle_at
    },
#ifdef AOS_AT_ADAPTER
    {
        .name = "test_at_enet",
        .help = "test_at_enet [tcp|udp] [ip] [data_string_to_send]",
        .function = handle_test_at_enet
    },
#endif
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

    // AT UART init
    uart_1.port                = AT_UART_PORT;
    uart_1.config.baud_rate    = AT_UART_BAUDRATE;
    uart_1.config.data_width   = AT_UART_DATA_WIDTH;
    uart_1.config.parity       = AT_UART_PARITY;
    uart_1.config.stop_bits    = AT_UART_STOP_BITS;
    uart_1.config.flow_control = AT_UART_FLOW_CONTROL;

    if (at.init(&at_uart, AT_RECV_PREFIX, AT_RECV_SUCCESS_POSTFIX, 
            AT_RECV_FAIL_POSTFIX, AT_SEND_DELIMITER, 1000) != 0)
        return -1;

    at.set_mode(ASYN);

    aos_register_event_filter(EV_WIFI, wifi_event_handler, NULL);

#ifdef AOS_AT_ADAPTER
    at_adapter_init();
#endif

    aos_cli_register_commands((const struct cli_command *)&atcmds[0],
      sizeof(atcmds) / sizeof(atcmds[0]));

    netmgr_init();
    netmgr_start(false);

    aos_loop_run();

    return 0;
}

