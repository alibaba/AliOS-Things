/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <aos/aos.h>
#include <netmgr.h>
#include <atparser.h>

#define TAG "hdlcclient"
#define HDLC_ECHO_PRFIX "HDLC_ECHO:"
#define HDLC_BUF_MAX_SIZE  1024

char buf[HDLC_BUF_MAX_SIZE];
char out[HDLC_BUF_MAX_SIZE];
char rcv[HDLC_BUF_MAX_SIZE];
char prefix[] = HDLC_ECHO_PRFIX;

#define MAX_TEST_CASE 320
#define MAX_TEST_STR_LEN 1024
int test_lens[] = {1, 2, 4, 8, 16, 32, 64, 128, 240, 243, 1000};
char test_str[1024] = {0};
long long init_start_time;
long long test_send_time;
int test_index = 0;
double throughput = 0;

void send_raw_test()
{
    int i, test_str_index;
    if (test_index >= MAX_TEST_CASE) {
        return;
    }

    test_str_index = 10;

    if (test_lens[test_str_index] + strlen(prefix) + 1 > MAX_TEST_STR_LEN) {
        return;
    }

    memset(test_str, 0, sizeof(test_str));
    memcpy(test_str, prefix, strlen(prefix));
    test_send_time = aos_now_ms();
    for (i = strlen(prefix); i < test_lens[test_str_index] + strlen(prefix); i++) {
        test_str[i] = 'a' + (i + test_send_time + test_lens[test_str_index] + i) % 26;
    }
    test_str[i] = ';';
    throughput += i;

    at.send_raw(test_str, out, sizeof(out));
    LOG("Case %d Send -->%s<--\n", test_index + 1, test_str);
}

static int hdlc_client_send(int argc, char **argv)
{
    char *input;

    if (argc < 3) {
        LOGE(TAG, "client input error\r\n");
        return -1;
    }

    input = argv[2];

    memset(buf, 0, sizeof(buf));
    if (strlen(prefix) + strlen(input) >= sizeof(buf) - 1) {
        LOGE(TAG, "client input too long\r\n");
    } else {
        memcpy(buf, prefix, strlen(prefix));
        memcpy(buf + strlen(prefix), input, strlen(input));
        buf[strlen(input) + strlen(prefix)] = '\0';
        at.send_raw(buf, out, sizeof(out));
    }

    return 0;
}

static int hdlc_client_send_2stage(int argc, char **argv)
{
    char *cmd;
    char *data;

    if (argc < 4) {
        LOGE(TAG, "client input error\r\n");
        return -1;
    }

    cmd = argv[2];
    data = argv[3];

    memset(buf, 0, sizeof(buf));
    if (strlen(prefix) + strlen(cmd) >= sizeof(buf) - 1) {
        LOGE(TAG, "client input too long\r\n");
    } else {
        memcpy(buf, prefix, strlen(prefix));
        memcpy(buf + strlen(prefix), cmd, strlen(cmd));
        buf[strlen(cmd) + strlen(prefix)] = '\0';
        //at.send_raw(buf, out, sizeof(out));
        LOG("send cmd %s data %s\n", buf, data);
        at.send_data_2stage(buf, data, strlen(data), out, sizeof(out));
    }

    return 0;
}

static void net_event_handler()
{
    int i = 0;

    do {
        if (i >= sizeof(rcv)) {
            LOGE(TAG, "Too long length\r\n");
            break;
        }

        if (at.read(&rcv[i], 1) <= 0) {
            LOGE(TAG, "read fail\r\n");
            break;
        }

        if (rcv[i] == ';') {
            rcv[i] = '\0';
            if (memcmp(test_str + strlen(prefix), rcv, strlen(rcv)) == 0) {
                LOG("Case %d Pass! len %d -->%s<--- time %u\n",
                    test_index + 1, i, rcv, aos_now_ms() - test_send_time);
                test_index++;
                send_raw_test();
                if (test_index >= MAX_TEST_CASE) {
                    double duration = aos_now_ms() - init_start_time;
                    LOG("Case test done: %.2fms %.2fB/s\n", duration, throughput * 2000 / duration );
                    test_index = 0;
                }

            } else {
                LOG("Case %d Fail! len %d -->%s<--- Fail!\n", test_index, i, rcv);
            }
            break;
        }
        i++;
    } while (1);

}

static void handle_hdlc(char *pwbuf, int blen, int argc, char **argv)
{
    int  ret = 0;
    char *ptype = argc > 1 ? argv[1] : "default";

    if (strcmp(ptype, "cmd") == 0) {
        ret = hdlc_client_send(argc, argv);
        if (ret) {
            LOG("fail to execute hdlc client test command \r\n");
            return;
        }
    } else if (strcmp(ptype, "data") == 0) {
        ret = hdlc_client_send_2stage(argc, argv);
        if (ret) {
            LOG("fail to execute hdlc client test command \r\n");
            return;
        }
    } else if (strcmp(ptype, "test") == 0) {
        init_start_time = aos_now_ms();
        test_index = 0;
        throughput = 0;
        send_raw_test();
        return;
    } else {
        LOGE(TAG, "Usage: hdlc cmd [cmd string]\r\n");
        return;
    }
}

static struct cli_command hdlccmds[] = {
    {
        .name = "hdlc",
        .help = "hdlc cmd [cmd] | data [cmd data]",
        .function = handle_hdlc
    }
};

int application_start(int argc, char *argv[])
{
    at.set_mode(ASYN);
    at.init(AT_RECV_PREFIX, AT_RECV_SUCCESS_POSTFIX,
            AT_RECV_FAIL_POSTFIX, AT_SEND_DELIMITER, 1000);

    aos_cli_register_commands((const struct cli_command *)&hdlccmds[0],
                              sizeof(hdlccmds) / sizeof(hdlccmds[0]));

    at.oob(HDLC_ECHO_PRFIX,  NULL, 0, net_event_handler, NULL);
    //at.oob("AT+",  NULL, 0, net_event_handler, NULL);

    netmgr_init();
    netmgr_start(false);

    aos_loop_run();

    return 0;
}
