/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <aos/aos.h>
#include <atparser.h>

#define TAG "hdlcserver"
#define HDLC_BUF_MAX_SIZE 1024
#define HDLC_ECHO_PRFIX "HDLC_ECHO:"
#define HDLC_SERVER_ECHO_INFO "MSG_TOO_LONG."
#define AT_STRING "AT+CIPSEND:"

static char buf[HDLC_BUF_MAX_SIZE];
static char out[HDLC_BUF_MAX_SIZE];
static char prefix[]   = HDLC_ECHO_PRFIX;
static char atprefix[] = AT_STRING;
static char info[]     = HDLC_SERVER_ECHO_INFO;

static void net_event_handler()
{
    int  i = 0, offset = 0;
    bool len_exist  = false;
    char len_str[5] = { 0 };
    int  len_start;
    int  len = 0;

    memset(buf, 0, sizeof(buf));
    memset(out, 0, sizeof(out));

    do {
        if (i >= sizeof(buf)) {
            LOGE(TAG, "Too long length\r\n");
            break;
        }

        if (at.read(&buf[i], 1) <= 0) {
            LOGE(TAG, "read fail\r\n");
            break;
        }

        if (!len_exist && memcmp(buf, atprefix, strlen(atprefix)) == 0) {
            len_exist = true;
            len_start = i + 1;
        }

        // end of message then echo
        if (memcmp(&buf[i], AT_SEND_DELIMITER, strlen(AT_SEND_DELIMITER)) ==
            0) {
            buf[i] = '\0';

            LOGD(TAG, "Echo server recv msg len %d\n", strlen(buf));

            if (len_exist && (i - len_start) > 0 &&
                (i - len_start) < sizeof(len_str)) {
                memcpy(len_str, buf + len_start, i - len_start);
                len = atoi(len_str);

                if (len > 0) {
                    memset(buf, 0, sizeof(buf));
                    if (at.read(buf, len) == 0) {
                        LOGD(TAG, "recv data len: %d\n", len);
                    }
                }
            }


            memcpy(out, buf, strlen(buf));
            memcpy(buf, AT_RECV_PREFIX, strlen(AT_RECV_PREFIX));
            offset += strlen(AT_RECV_PREFIX);
            memcpy(buf + offset, AT_RECV_SUCCESS_POSTFIX,
                   strlen(AT_RECV_SUCCESS_POSTFIX));
            offset += strlen(AT_RECV_SUCCESS_POSTFIX);
            buf[offset] = '\0';
            at.send_raw_no_rsp(buf);

            memcpy(buf, prefix, strlen(prefix));
            offset = strlen(prefix);
            if (strlen(out) + strlen(prefix) + 1 + 1 < sizeof(buf)) {
                memcpy(buf + offset, out, strlen(out));
                offset += strlen(out);
                buf[offset++] = '\r';
                buf[offset]   = '\0';
            } else {
                LOGE(TAG, "message too long!\n");
                break;
            }

            at.send_raw_no_rsp(buf);
            break;
        }
        i++;
    } while (1);
}

static void app_delayed_action(void *arg)
{
    LOGD(TAG, "hdlc_server: alive %s:%d %s\r\n", __func__, __LINE__,
         aos_task_name());
    aos_post_delayed_action(5000, app_delayed_action, NULL);
}

int application_start(int argc, char *argv[])
{
    at.set_mode(ASYN);
    at.init(AT_RECV_PREFIX, AT_RECV_SUCCESS_POSTFIX, AT_RECV_FAIL_POSTFIX,
            AT_SEND_DELIMITER, 1000);

    at.oob(HDLC_ECHO_PRFIX, NULL, 0, net_event_handler, NULL);

    LOG(TAG, "hdlc echo server start!\n");
    aos_post_delayed_action(1000, app_delayed_action, NULL);

    aos_loop_run();

    return 0;
}
