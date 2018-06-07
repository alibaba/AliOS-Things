/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <aos/aos.h>
#include <atparser.h>

#define TAG "hdlcserver"
#define HDLC_BUF_MAX_SIZE  256
#define HDLC_ECHO_PRFIX "HDLC_ECHO:"
#define HDLC_SERVER_ECHO_INFO "MSG_TOO_LONG."

static char buf[HDLC_BUF_MAX_SIZE];
static char out[HDLC_BUF_MAX_SIZE];
static char prefix[] = HDLC_ECHO_PRFIX;
static char info[] = HDLC_SERVER_ECHO_INFO;

static void net_event_handler()
{
    int i = 0;

    do {
        if (i >= sizeof(buf)) {
            LOGE(TAG, "Too long length\r\n");
            break;
        }

        if (at.read(&buf[i], 1) <= 0) {
            LOGE(TAG, "read fail\r\n");
            break;
        }

        // end of message then echo
        if (buf[i] == '.') {
            buf[i] = '\0';
            LOG("Echo server recv msg len %d -->%s<--\n", i, buf);

            memcpy(out, buf, i);
            memcpy(buf, prefix, strlen(prefix));
            if (i + strlen(prefix) + 1 < sizeof(buf)) {
                memcpy(buf + strlen(prefix), out, i);
                buf[strlen(prefix) + i] = '.';
                buf[strlen(prefix) + i + 1] = '\0';
            } else {
                memcpy(buf + strlen(prefix), info, strlen(info));
                buf[strlen(prefix) + strlen(info)] = '\0';
            }

            at.send_raw(buf, out, sizeof(out));
            break;
        }
        i++;
    } while (1);
}

static void app_delayed_action(void *arg)
{
    LOG("hdlc_server: alive %s:%d %s\r\n", __func__, __LINE__, aos_task_name());
    aos_post_delayed_action(5000, app_delayed_action, NULL);
}

int application_start(int argc, char *argv[])
{
    at.set_mode(ASYN);
    at.init(AT_RECV_PREFIX, AT_RECV_SUCCESS_POSTFIX,
            AT_RECV_FAIL_POSTFIX, AT_SEND_DELIMITER, 1000);

    at.oob(HDLC_ECHO_PRFIX,  NULL, 0, net_event_handler, NULL);

    LOG("hdlc echo server start!\n");
    aos_post_delayed_action(1000, app_delayed_action, NULL);

    aos_loop_run();

    return 0;
}
