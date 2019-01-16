/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include <aos/kernel.h>
#include <aos/log.h>
#ifdef CONFIG_MBEDTLS_TEST_TLS
#include <aos/yloop.h>
#include <aos/network.h>
#include <netmgr.h>
#endif

extern int mbedtls_selftest_main(int argc, char *argv[]);
#ifdef CONFIG_MBEDTLS_TEST_TLS
extern int tls_client_test(void);

static void app_delayed_action(void *arg)
{
    LOG("TLS client test start.");
    tls_client_test();
    LOG("TLS client test end.\n");

#if defined(DTLS_ENABLED)
    LOG("DTLS client test start.");
    dtls_client_test();
    LOG("DTLS client test end.\n");
#endif
}

static void handle_event(input_event_t *event, void *arg)
{
    if ((event->type != EV_WIFI) || (event->code != CODE_WIFI_ON_GOT_IP)) {
        return;
    }

    aos_post_delayed_action(1000, app_delayed_action, arg);
}

static int tls_client_test_start(void)
{
#ifdef WITH_SAL
    sal_init();
#endif

    aos_register_event_filter(EV_WIFI, handle_event, NULL);

    netmgr_init();
    netmgr_start(true);

    aos_loop_run();
    /* never return */

    return 0;
}

#endif /*CONFIG_MBEDTLS_TEST_TLS*/

static void mbedtls_test(void *arg)
{
    char *argv[] = {"mbedtls_selftest", NULL};

    LOG("mbedtls selftest started.");
    mbedtls_selftest_main(1, argv);
    LOG("mbedtls selftest end.");
#ifdef CONFIG_MBEDTLS_TEST_TLS
    LOG("mbedtls TLS client test started.");
    tls_client_test_start();
#endif
}

int application_start(int argc, char *argv[])
{
    LOG("application started.");
    LOG("mbedtls test started.");
    aos_task_new("mbedtls_test", mbedtls_test, NULL, 4096);
    return 0;
}

