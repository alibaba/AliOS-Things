/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <aos/aos.h>
#include <netmgr.h>
#include <atparser.h>

#define TAG "hdlcclient"
#define HDLC_ECHO_PRFIX "HDLC_ECHO:"
#define AT_PREFIX "AT+CIPSEND:"
#define HDLC_BUF_MAX_SIZE 1024

char buf[HDLC_BUF_MAX_SIZE];
char out[HDLC_BUF_MAX_SIZE];
char rcv[HDLC_BUF_MAX_SIZE];
char prefix[]   = HDLC_ECHO_PRFIX;
char atprefix[] = AT_PREFIX;

#define MAX_TEST_CASE 320
#define MAX_TEST_STR_LEN 1024
int       test_lens[] = { 1, 2, 4, 8, 16, 32, 64, 128, 240, 243, 1000 };
char      test_str[MAX_TEST_STR_LEN] = { 0 };
long long init_start_time;
long long test_send_time;
int       test_index = 0;
double    throughput = 0;
typedef void (*test_op_t)();

test_op_t test_fun;

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
    for (i = strlen(prefix); i < test_lens[test_str_index] + strlen(prefix);
         i++) {
        test_str[i] =
          'a' + (i + test_send_time + test_lens[test_str_index] + i) % 26;
    }
    throughput += i;

    at.send_raw(test_str, out, sizeof(out));
    // LOGD(TAG, "Case %d Send -->%s<--\n", test_index + 1, test_str);

    aos_task_exit(0);
}

void send_2stage_test()
{
    int  i, test_str_index;
    char cmd[30] = { 0 };

    test_str_index = 10;

    if (test_index >= MAX_TEST_CASE) {
        return;
    }

    if (test_lens[test_str_index] + 1 > MAX_TEST_STR_LEN) {
        return;
    }

    if (strlen(prefix) + strlen(atprefix) + 5 + 1 > sizeof(cmd)) {
        return;
    }

    snprintf(cmd, sizeof(cmd), "%s%s%d", prefix, atprefix,
             test_lens[test_str_index]);

    test_send_time = aos_now_ms();
    memset(test_str, 0, sizeof(test_str));
    for (i = 0; i < test_lens[test_str_index]; i++) {
        test_str[i] =
          'a' + (i + test_send_time + test_lens[test_str_index] + i) % 26;
    }
    throughput += i + strlen(cmd);

    at.send_data_2stage(cmd, test_str, strlen(test_str), out, sizeof(out));
    // LOGD(TAG, "Case %d Send -->%s  %s<--\n", test_index + 1, cmd, test_str);

    aos_task_exit(0);
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
    if (strlen(atprefix) + strlen(input) >= sizeof(buf) - 1) {
        LOGE(TAG, "client input too long\r\n");
    } else {
        memcpy(buf, atprefix, strlen(atprefix));
        memcpy(buf + strlen(atprefix), input, strlen(input));
        // buf[strlen(input) + strlen(prefix)] = ';';
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

    cmd  = argv[2];
    data = argv[3];

    memset(buf, 0, sizeof(buf));
    if (strlen(prefix) + strlen(cmd) >= sizeof(buf) - 1) {
        LOGE(TAG, "client input too long\r\n");
    } else {
        memcpy(buf, prefix, strlen(prefix));
        memcpy(buf + strlen(prefix), cmd, strlen(cmd));
        buf[strlen(cmd) + strlen(prefix)] = '\0';
        // at.send_raw(buf, out, sizeof(out));
        LOG("send cmd %s data %s\n", buf, data);
        at.send_data_2stage(buf, data, strlen(data), out, sizeof(out));
    }

    return 0;
}

static void net_event_handler()
{
    int i = 0, offset = 0;

    do {
        if (i >= sizeof(rcv)) {
            LOGE(TAG, "Too long length\r\n");
            break;
        }

        if (at.read(&rcv[i], 1) <= 0) {
            LOGE(TAG, "read fail\r\n");
            break;
        }

        if (memcmp(&rcv[i], AT_SEND_DELIMITER, strlen(AT_SEND_DELIMITER)) ==
            0) {
            rcv[i] = '\0';

            if (test_fun == send_raw_test) {
                offset = strlen(prefix);
            }

            if (memcmp(test_str + offset, rcv, strlen(test_str) - offset) ==
                0) {
                LOGD(TAG, "Case %d Pass! len %d time %u\n", test_index + 1, i,
                     aos_now_ms() - test_send_time);
                test_index++;
                // test_fun();
                if (test_index < MAX_TEST_CASE) {
                    aos_task_new("hdlc_client", test_fun, NULL, 1024);
                }
            } else {
                LOGE(TAG, "Case %d Fail! len %d\n", test_index + 1, i);
            }

            break;
        }
        i++;
    } while (1);


    if (test_index >= MAX_TEST_CASE) {
        double duration = aos_now_ms() - init_start_time;
        LOG("Case test done: %.2fms %.2fB/s\n", duration,
            throughput * 2000 / duration);
        test_index = 0;
    }
}

static void dump_hex(uint8_t *data, uint32_t len)
{
    printf("\r\n");
    while (len--) {
        printf("%02x ", *data++);
    }
    printf("\r\n");
}

#define ATYWSSMNTRSTART "AT+YWSSSTARTMONITOR"
#define ATYWSSMNTRSTOP "AT+YWSSSTOPMONITOR"
#define ATYWSSSETCH "AT+YWSSSETCHANNEL"
#define ATYWSSSUSPSTA "AT+YWSSSUSPENDSTATION"

#define MAX_RSSI 256
#define MONITOR_PKT_MAX_LEN 2000

/**
 * ywss events:
 *     +YEVENT:MONITOR_UP
 *     +YEVENT:MONITOR_DOWN
 *     +YEVENT:rssi,len,data
 *
 *     Note: rssi and len value should be in dec format.
 */
static void ywss_cb(void *arg, char *buff, int bufflen)
{
    char     c, buf[32] = { 0 };
    uint8_t *payload = NULL;
    int      rssi = 0, nflag = 1;
    uint32_t len = 0;

    at.read(&c, 1);
    switch (c) {
        case 'M':
            memset(buf, 0, 32);
            at.read(buf, 9);
            if (strcmp(buf, "ONITOR_UP") == 0) {
                LOGD(TAG, "ywss monitor UP event received.");
                // do sth. here, e.g. to signal a sem of another task
            } else if (strcmp(buf, "ONITOR_DO") == 0) {
                memset(buf, 0, 32);
                at.read(buf, 2);
                if (strcmp(buf, "WN") == 0) {
                    LOGD(TAG, "ywss monitor DOWN event received.");
                    // do sth. here, e.g. to signal a sem of another task
                } else {
                    LOGE(TAG, "invalid ywss event (expected: WN, but: %s)",
                         buf);
                    assert(0);
                }
            } else {
                LOGE(TAG, "invalid ywss event (expected: MONITOR_xx, but: %s)",
                     buf);
                assert(0);
            }
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            if (c == '-') {
                nflag = -1;
                at.read(&c, 1);
            }

            /* rssi */
            while (c != ',' && rssi < MAX_RSSI) {
                if (c > '9' || c < '0') {
                    LOGE(TAG,
                         "ywss packet event reading rssi value failed (%c is "
                         "not number char).",
                         c);
                    assert(0);
                }
                rssi = (rssi << 3) + (rssi << 1) + (c - '0');
                at.read(&c, 1);
            }

            if (c != ',') {
                LOGE(TAG, "Invalid rssi info received.");
                assert(0);
            }

            /* len */
            at.read(&c, 1);
            while (c != ',' && len < MONITOR_PKT_MAX_LEN) {
                if (c > '9' || c < '0') {
                    LOGE(TAG,
                         "ywss packet event reading len value failed (%c is "
                         "not number char).",
                         c);
                    assert(0);
                }
                len = (len << 3) + (len << 1) + (c - '0');
                at.read(&c, 1);
            }

            if (c != ',') {
                LOGE(TAG, "Invalid packet len info received.");
                assert(0);
            }

            rssi *= nflag;
            LOG("The rssi value is %d", rssi);

            payload = (uint8_t *)aos_malloc(len);
            if (!payload) {
                LOGE(TAG, "Failed to allocate memory for ywss packet.");
                assert(0);
            }

            LOG("The packet len value is %d", len);

            at.read(payload, len);
            dump_hex(payload, len);
            aos_free(payload);

            break;
        default:
            LOGE(TAG, "Invalid ywss event found (first char: %c)", c);
            assert(0);
            break;
    }
}

static void handle_hdlc(char *pwbuf, int blen, int argc, char **argv)
{
    int   ret   = 0;
    char *ptype = argc > 1 ? argv[1] : "default";

    if (strcmp(ptype, "cmd") == 0) {
        ret = hdlc_client_send(argc, argv);
        if (ret) {
            LOG("fail to execute hdlc client test command \r\n");
            return;
        }
    } else if (strcmp(ptype, "ywss") == 0) {
        char *        ycmd = NULL;
        static oob_cb cb   = NULL;

        if (argc != 3 && argc != 4) {
            LOGE(TAG, "Invalid argument for ywss.");
            return;
        }

        if (strcmp(argv[2], "start") == 0) {
            LOGD(TAG, "Will start ywss");
            if (!cb) {
                cb = ywss_cb;
                at.oob("+YEVENT:", NULL, MONITOR_PKT_MAX_LEN, cb, NULL);
            }
            ycmd = ATYWSSMNTRSTART;
        } else if (strcmp(argv[2], "stop") == 0) {
            LOGD(TAG, "Will stop ywss");
            ycmd = ATYWSSMNTRSTOP;
        } else if (strcmp(argv[2], "suspend_sta") == 0) {
            LOGD(TAG, "Will suspend station");
            ycmd = ATYWSSSUSPSTA;
        } else if (strcmp(argv[2], "setch") == 0) {
            char chcmd[sizeof(ATYWSSSETCH) + 3] = { 0 };
            if (argc != 4) {
                LOGE(TAG, "Invalid argument for ywss.");
                return;
            }
            LOGD(TAG, "Will set channel %s", argv[3]);
            snprintf(chcmd, sizeof(chcmd), "%s,%s", ATYWSSSETCH, argv[3]);
            ycmd = chcmd;
        } else {
            LOGE(TAG, "Invalid ywss cmd: %s", argv[2]);
        }

        if (ycmd) {
            at.send_raw(ycmd, out, sizeof(out));
        }
    } else if (strcmp(ptype, "data") == 0) {
        ret = hdlc_client_send_2stage(argc, argv);
        if (ret) {
            LOG("fail to execute hdlc client test command \r\n");
            return;
        }
    } else if (strcmp(ptype, "test1") == 0) {
        init_start_time = aos_now_ms();
        test_index      = 0;
        throughput      = 0;
        test_fun        = send_raw_test;
        aos_task_new("hdlc_client", test_fun, NULL, 1024);
        LOG("Test1 starts! wait...\n");
        return;
    } else if (strcmp(ptype, "test2") == 0) {
        init_start_time = aos_now_ms();
        test_index      = 0;
        throughput      = 0;
        test_fun        = send_2stage_test;
        aos_task_new("hdlc_client", test_fun, NULL, 1024);
        LOG("Test2 starts! wait...\n");
        return;
    } else {
        LOGE(TAG, "Usage: hdlc cmd [cmd string]\r\n");
        return;
    }
}

static struct cli_command hdlccmds[] = {
    { .name     = "hdlc",
      .help     = "hdlc cmd [cmd] | data [cmd data] | test1 | test2",
      .function = handle_hdlc }
};

int application_start(int argc, char *argv[])
{
    at.set_mode(ASYN);
    at.init(AT_RECV_PREFIX, AT_RECV_SUCCESS_POSTFIX, AT_RECV_FAIL_POSTFIX,
            AT_SEND_DELIMITER, 1000);

    aos_cli_register_commands((const struct cli_command *)&hdlccmds[0],
                              sizeof(hdlccmds) / sizeof(hdlccmds[0]));

    at.oob(HDLC_ECHO_PRFIX, NULL, 0, net_event_handler, NULL);

    netmgr_init();
    netmgr_start(false);

    aos_loop_run();

    return 0;
}
