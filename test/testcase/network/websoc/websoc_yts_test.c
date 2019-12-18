/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>

#include "yunit/yunit.h"
#include "yunit/yts.h"

#include <aos/kernel.h>
#include <ulog/ulog.h>
#include <network/websoc/librws.h>

#define TAG "websoc_yts"

#define TEST_SCHEME "ws"
#define TEST_HOST "demos.kaazing.com"
#define TEST_PORT 80
#define TEST_PATH "/echo"

#define TEST_TEXT "Hello world"
#define TEST_BIN1 {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07}
#define TEST_BIN2 {0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17}
#define TEST_BIN3 {0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27}

static uint8_t _cb_flags;
#define CB_FLAG_ON_CONNECTED_BIT        (1 << 0)
#define CB_FLAG_ON_DISCONNECTED_BIT     (1 << 1)
#define CB_FLAG_ON_RECEIVED_TEXT_BIT    (1 << 2)
#define CB_FLAG_ON_RECEIVED_BIN_BIT     (1 << 3)
#define CB_FLAG_ON_ON_RECEIVED_PONG_BIT (1 << 4)

static void on_connected(rws_socket s)
{
    if (!s) return;
    LOGI(TAG, "socket %p connected", s);
    _cb_flags |= CB_FLAG_ON_CONNECTED_BIT;
}

static void on_disconnected(rws_socket s)
{
    if (!s) return;
    LOGI(TAG, "socket %p disconnected", s);
    _cb_flags |= CB_FLAG_ON_DISCONNECTED_BIT;
}

static void on_recv_text(rws_socket s, const char *text, const unsigned int len, bool is_finished)
{
    if (!s || !text) return;
    LOGI(TAG, "socket %p on received text", s);
    _cb_flags |= CB_FLAG_ON_RECEIVED_TEXT_BIT;
}

static void on_recv_bin(rws_socket s, const char *bin, const unsigned int len, bool is_finished)
{
    if (!s || !bin) return;
    LOGI(TAG, "socket %p on received binary", s);
    _cb_flags |= CB_FLAG_ON_RECEIVED_BIN_BIT;
}

static void on_recv_pong(rws_socket s)
{
    if (!s) return;
    LOGI(TAG, "socket %p on received pong", s);
    _cb_flags |= CB_FLAG_ON_ON_RECEIVED_PONG_BIT;
}

static void test_websoc_socket(void)
{
    rws_socket skt;
    char *ret_c;
    int ret_i;
    rws_bool ret_b;
    rws_error ret_e;
    void *ret_vp;

    skt = rws_socket_create();
    YUNIT_ASSERT(skt != NULL);
    if (!skt) return;

    rws_socket_set_scheme(skt, TEST_SCHEME);
    ret_c = (const char *)rws_socket_get_scheme(skt);
    YUNIT_ASSERT(ret_c != NULL);
    YUNIT_ASSERT(strcmp(ret_c, TEST_SCHEME) == 0);

    rws_socket_set_host(skt, TEST_HOST);
    ret_c = (const char *)rws_socket_get_host(skt);
    YUNIT_ASSERT(ret_c != NULL);
    YUNIT_ASSERT(strcmp(ret_c, TEST_HOST) == 0);

    rws_socket_set_port(skt, TEST_PORT);
    ret_i = rws_socket_get_port(skt);
    YUNIT_ASSERT(ret_i == TEST_PORT);

    rws_socket_set_path(skt, TEST_PATH);
    ret_c = (const char *)rws_socket_get_path(skt);
    YUNIT_ASSERT(ret_c != NULL);
    YUNIT_ASSERT(strcmp(ret_c, TEST_PATH) == 0);

    rws_socket_set_on_connected(skt, on_connected);
    rws_socket_set_on_disconnected(skt, on_disconnected);
    rws_socket_set_on_received_text(skt, on_recv_text);
    rws_socket_set_on_received_bin(skt, on_recv_bin);
    rws_socket_set_on_received_pong(skt, on_recv_pong);

    /* set an invalid port for error testing */
    rws_socket_set_port(skt, 0);
    ret_b = rws_socket_connect(skt);
    YUNIT_ASSERT(ret_b == rws_false);

    ret_e = rws_socket_get_error(skt);
    YUNIT_ASSERT(ret_e != NULL);

    ret_i = rws_error_get_code(ret_e);
    YUNIT_ASSERT(ret_i != 0);
    YUNIT_ASSERT(ret_i == rws_error_code_missed_parameter);

    ret_i = rws_error_get_http_error(ret_e);
    YUNIT_ASSERT(ret_i == 0);

    ret_c = (const char *)rws_error_get_description(ret_e);
    YUNIT_ASSERT(ret_c != NULL);

    rws_socket_set_url(skt, TEST_SCHEME, TEST_HOST, TEST_PORT, TEST_PATH);
    ret_b = rws_socket_connect(skt);
    YUNIT_ASSERT(ret_b == rws_true);

    rws_thread_sleep(5000);

    ret_b = rws_socket_is_connected(skt);
    YUNIT_ASSERT(ret_b == rws_true);
    if (ret_b == rws_false) {
        LOGE(TAG, "socket is not connected, test terminated here");
        return;
    }

    rws_thread_sleep(5000);
    YUNIT_ASSERT((_cb_flags & CB_FLAG_ON_CONNECTED_BIT) != 0);

    rws_socket_set_user_object(skt, "test");
    ret_vp = rws_socket_get_user_object(skt);
    YUNIT_ASSERT(ret_vp != NULL);
    YUNIT_ASSERT(strcmp(ret_vp, "test") == 0);

    ret_b = rws_socket_send_text(skt, TEST_TEXT);
    YUNIT_ASSERT(ret_b == rws_true);

    rws_thread_sleep(5000);
    YUNIT_ASSERT((_cb_flags & CB_FLAG_ON_RECEIVED_TEXT_BIT) != 0);

    const char bin_start[] = TEST_BIN1;
    ret_b = rws_socket_send_bin_start(skt, bin_start, sizeof(bin_start) / sizeof(bin_start[0]));
    YUNIT_ASSERT(ret_b == rws_true);

    const char bin_cont[] = TEST_BIN2;
    ret_b = rws_socket_send_bin_continue(skt, bin_cont, sizeof(bin_cont) / sizeof(bin_cont[0]));
    YUNIT_ASSERT(ret_b == rws_true);

    const char bin_end[] = TEST_BIN3;
    ret_b = rws_socket_send_bin_finish(skt, bin_end, sizeof(bin_end) / sizeof(bin_end[0]));
    YUNIT_ASSERT(ret_b == rws_true);

    rws_thread_sleep(10000);
    YUNIT_ASSERT((_cb_flags & CB_FLAG_ON_RECEIVED_BIN_BIT) != 0);

    rws_socket_disconnect_and_release(skt);

    rws_thread_sleep(5000);
    ret_b = rws_socket_is_connected(skt);
    YUNIT_ASSERT(ret_b == rws_false);

    rws_thread_sleep(5000);
    //YUNIT_ASSERT((_cb_flags & CB_FLAG_ON_DISCONNECTED_BIT) != 0);
    YUNIT_ASSERT((_cb_flags & CB_FLAG_ON_ON_RECEIVED_PONG_BIT) != 0);
}

static int init(void)
{
    return 0;
}

static int cleanup(void)
{
    return 0;
}

static void setup(void)
{
    memset(_cb_flags, 0, sizeof(_cb_flags));
}

static void teardown(void)
{

}

static yunit_test_case_t aos_websoc_testcases[] = {
    { "basic", test_websoc_socket },
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "http", init, cleanup, setup, teardown, aos_websoc_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_http(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_http);
