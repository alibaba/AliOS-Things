/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

#include <aos/network.h>
#include <aos/kernel.h>
#include <aos/yloop.h>
#include <netmgr.h>

#include "ali_crypto.h"
#include "itls/config.h"
#include "itls/net_sockets.h"
#include "itls/debug.h"
#include "itls/ssl.h"

#define DEBUG_LEVEL     2

#if defined(ON_DAILY)
#define SERVER_PORT     "1885"
#define SERVER_NAME     "11.160.112.156"
#define PRODUCT_KEY     "a1V2WSinkfc"
#define PRODUCT_SECRET  "i11fSJmDtgUTCRUahfv5D9BC64FA62B2"
#else
#define SERVER_PORT     "1883"
#define SERVER_NAME     "itls.cn-shanghai.aliyuncs.com"
#define PRODUCT_KEY     "a1WO4Z9qHRw"
#define PRODUCT_SECRET  "F113nbRWjxX67YY6L8GF20CD0BDDFFFF"
#endif

struct cookie {
    int flag;
};

unsigned char mqtt_req_data[] = {
    0x10, 0xa7, 0x01, 0x00, 0x04, 0x4d, 0x51, 0x54,
    0x54, 0x04, 0xc2, 0x00, 0x3c, 0x00, 0x59, 0x42,
    0x66, 0x4b, 0x78, 0x42, 0x44, 0x53, 0x6a, 0x57,
    0x43, 0x48, 0x2e, 0x61, 0x6f, 0x73, 0x5f, 0x6d,
    0x71, 0x74, 0x74, 0x5f, 0x74, 0x65, 0x73, 0x74,
    0x7c, 0x73, 0x65, 0x63, 0x75, 0x72, 0x65, 0x6d,
    0x6f, 0x64, 0x65, 0x3d, 0x2d, 0x31, 0x2c, 0x74,
    0x69, 0x6d, 0x65, 0x73, 0x74, 0x61, 0x6d, 0x70,
    0x3d, 0x32, 0x35, 0x32, 0x34, 0x36, 0x30, 0x38,
    0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x2c, 0x73,
    0x69, 0x67, 0x6e, 0x6d, 0x65, 0x74, 0x68, 0x6f,
    0x64, 0x3d, 0x68, 0x6d, 0x61, 0x63, 0x73, 0x68,
    0x61, 0x31, 0x2c, 0x67, 0x77, 0x3d, 0x30, 0x7c,
    0x00, 0x1e, 0x4c, 0x76, 0x55, 0x4d, 0x52, 0x49,
    0x33, 0x57, 0x39, 0x48, 0x31, 0x50, 0x6d, 0x69,
    0x64, 0x74, 0x4d, 0x39, 0x67, 0x42, 0x30, 0x30,
    0x31, 0x30, 0x61, 0x34, 0x66, 0x62, 0x30, 0x31,
    0x00, 0x20, 0x30, 0x34, 0x30, 0x66, 0x36, 0x32,
    0x36, 0x63, 0x34, 0x39, 0x33, 0x38, 0x34, 0x65,
    0x33, 0x30, 0x61, 0x62, 0x35, 0x63, 0x31, 0x66,
    0x61, 0x64, 0x31, 0x32, 0x63, 0x39, 0x61, 0x39,
    0x66, 0x63
};

static int tls_random(void *p_rng, unsigned char *output, size_t output_len)
{
    struct timeval tv;

    (void)p_rng;

    gettimeofday(&tv, NULL);
    ali_seed((uint8_t *)&tv.tv_usec, sizeof(suseconds_t));
    ali_rand_gen(output, output_len);

    return 0;
}

#if defined(MBEDTLS_DEBUG_C)
static void tls_debug( void *ctx, int level,
                       const char *file, int line, const char *str )
{
    ((void) ctx);
    ((void) level);

    printf("%s:%04d: %s", file, line, str);
}
#endif

static void itls_client_sample(void)
{
    int ret = 0, len;
    unsigned char buf[1024];
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_net_context server_fd;
    char host[256] = {0};
    char *product_key = PRODUCT_KEY;
    char *product_secret = PRODUCT_SECRET;

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold( DEBUG_LEVEL );
#endif

    /*
     * 0. Initialize the RNG and the session data
     */
    mbedtls_net_init(&server_fd);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);

    /*
     * 1. Start the connection
     */
#if defined(ON_DAILY)
    sprintf(host, "%s", SERVER_NAME);
#else
    sprintf(host, "%s.%s", product_key, SERVER_NAME);
#endif

    printf("  . Connecting to tcp/%s/%s...", host, SERVER_PORT);

    if ((ret = mbedtls_net_connect(&server_fd, host,
                                   SERVER_PORT, MBEDTLS_NET_PROTO_TCP)) != 0) {
        printf(" failed\n  ! mbedtls_net_connect returned %d\n\n", ret);
        goto exit;
    }

    printf(" ok\n");

    /*
     * 2. Setup stuff
     */
    printf("  . Setting up the SSL/TLS structure...");

    if ((ret = mbedtls_ssl_config_defaults(&conf,
                                           MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_STREAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        printf(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret);
        goto exit;
    }

    printf(" ok\n");

    mbedtls_ssl_conf_rng(&conf, tls_random, NULL);
#if defined(MBEDTLS_DEBUG_C)
    mbedtls_ssl_conf_dbg(&conf, tls_debug, NULL);
#endif

    /* OPTIONAL extra data for authentication */
    if ((ret = mbedtls_ssl_conf_auth_extra(
                   &conf, product_key, strlen(product_key))) != 0) {
        printf(" failed\n  ! mbedtls_ssl_conf_auth_extra returned %d\n\n", ret);
        goto exit;
    }

    /* OPTIONAL token for one-time provisioning */
    if ((ret = mbedtls_ssl_conf_auth_token(
                   &conf, product_secret, strlen(product_secret))) != 0) {
        printf(" failed\n  ! mbedtls_ssl_conf_auth_token returned %d\n\n", ret);
        goto exit;
    }

#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
    if ((ret = mbedtls_ssl_conf_max_frag_len( &conf, MBEDTLS_SSL_MAX_FRAG_LEN_1024)) != 0) {
        printf(" failed\n  ! mbedtls_ssl_conf_max_frag_len returned %d\n\n", ret);
        goto exit;
    }
#endif

    if ((ret = mbedtls_ssl_setup( &ssl, &conf)) != 0) {
        printf(" failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret);
        goto exit;
    }

    mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    /*
     * 3. Handshake
     */
    printf("  . Performing the SSL/TLS handshake...");

    while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            printf(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret);
            goto exit;
        }
    }

    printf(" ok\n");

    /*
     * 4. Write the GET request
     */
    printf("  > Write to server:");

    len = sizeof(mqtt_req_data);
    memcpy(buf, mqtt_req_data, len);

    while ((ret = mbedtls_ssl_write(&ssl, buf, len)) <= 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            printf(" failed\n  ! mbedtls_ssl_write returned %d\n\n", ret);
            goto exit;
        }
    }

    len = ret;

    printf(" %d bytes written\n\n", len);

    /*
     * 5. Read the HTTP response
     */
    printf("  < Read from server:");

    do {
        len = sizeof(buf) - 1;
        memset(buf, 0, sizeof(buf));
        ret = mbedtls_ssl_read(&ssl, buf, len);

        if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
            continue;
        }

        if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
            break;
        }

        if (ret < 0) {
            printf("failed\n  ! mbedtls_ssl_read returned %d\n\n", ret);
            break;
        }

        if (ret == 0) {
            printf("\n\nEOF\n\n");
            break;
        }

        len = ret;
        printf(" %d bytes read\n\n", len);
    } while (1);

    mbedtls_ssl_close_notify(&ssl);

exit:
    mbedtls_net_free(&server_fd);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);

    return;
}

static void app_delayed_action(void *arg)
{
    printf("===========> iTLS Client Sample start.\n");
    itls_client_sample();
    printf("<=========== iTLS Client Sample End.\n\n");
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

