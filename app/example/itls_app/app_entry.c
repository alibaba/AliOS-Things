/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include <network/network.h>
#include "aos/kernel.h"
#include "aos/yloop.h"
#include <netmgr.h>

#define DEBUG_LEVEL     1

#if defined(ON_DAILY)
#define PRODUCT_KEY     "a1V2WSinkfc"
#define PRODUCT_SECRET  "i11fSJmDtgUTCRUahfv5D9BC64FA62B2"
#else
#define PRODUCT_KEY     "a1WO4Z9qHRw"
#define PRODUCT_SECRET  "i113nbRWjxX67YFFFFFFFFFFFFFFFFFF"
#endif

struct cookie {
    int flag;
};

extern int itls_client_sample(
           char *product_key, char *product_secret, int debug_level);
extern int idtls_client_sample(
           char *product_key, char *product_secret, int debug_level);

static void app_delayed_action(void *argv)
{
    int ret;

    printf("===========> iTLS Client Sample start.\n");
    ret = itls_client_sample(
               PRODUCT_KEY, PRODUCT_SECRET, DEBUG_LEVEL);
    if (ret < 0) {
        printf("iTLS Client Sample Failed!\n");
        return -1;
    }
    printf("<=========== iTLS Client Sample End.\n\n");

#if defined(CONFIG_SSL_DTLS)
    printf("===========> iDTLS Client Sample start.\n");
    ret = idtls_client_sample(
               PRODUCT_KEY, PRODUCT_SECRET, DEBUG_LEVEL);
    if (ret < 0) {
        printf("iDTLS Client Sample Test Failed!\n");
        return -1;
    }
    printf("<=========== iDTLS Client Sample End.\n\n");
#endif

    return 0;
}

static void handle_event(input_event_t *event, void *arg)
{
    if (event->type != EV_WIFI) {
        return;
    }

    if (event->code != CODE_WIFI_ON_GOT_IP) {
        return;
    }

    aos_post_delayed_action(1000, app_delayed_action, arg);
}

int application_start(int argc, char **argv)
{
    struct cookie *cookie = aos_malloc(sizeof(*cookie));
    memset(cookie, 0, sizeof(*cookie));

#ifdef WITH_SAL
    sal_add_dev(NULL, NULL);
    sal_init();
#endif

    aos_register_event_filter(EV_WIFI, handle_event, cookie);

    netmgr_init();
#if !defined(STM32_USE_SPI_WIFI)
    netmgr_start(true);
#else
    netmgr_start(false);
#endif

    aos_loop_run();
    /* never return */

    return 0;
}


