/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include "itls/config.h"
#include "itls/net.h"
#include "itls/debug.h"
#include "itls/ssl.h"
#include "itls/hal_itls.h"

#include "ali_crypto.h"

#if defined(ON_DAILY)
#define SERVER_PORT     1885
#define SERVER_NAME     "11.158.132.143"
#else
#define SERVER_PORT     1883
#define SERVER_NAME     "itls.cn-shanghai.aliyuncs.com"
#endif

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

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_LINUX_LE)

static int itls_hal_sample(const char *host, int port,
         const char *product_key, const char *product_secret, int debug_level)
{
    int ret = 0, len;
    char version[16] = {0};
    unsigned char buf[1024];
    uintptr_t handle;

    hal_itls_get_version(version);
    SSL_DBG_LOG("itls version: %s\n", version);

    hal_itls_set_debug_level(debug_level);

    handle = hal_itls_establish(host,
                 port, product_key, product_secret);
    if (handle == (uintptr_t)NULL) {
        SSL_DBG_LOG("itls establsih fail\n");
        ret = -1;
        goto _out;
    }

    len = sizeof(mqtt_req_data);
    memcpy(buf, mqtt_req_data, len);
    ret = hal_itls_write(handle, (char *)buf, len, 2000);
    if (ret < 0) {
        SSL_DBG_LOG("itls write fail, %d\n", ret);
        goto _out;
    }

    SSL_DBG_LOG("%d bytes written\n", ret);

    ret = hal_itls_read(handle, (char *)buf, len, 2000);
    if (ret < 0) {
        SSL_DBG_LOG("itls read fail\n");
        goto _out;
    }

    SSL_DBG_LOG("%d bytes read\n", ret);

    ret = 0;

_out:
    SSL_DBG_LOG("hal itls alert type: %d\n", hal_itls_get_alert_type());

    hal_itls_destroy(handle);

    return ret;
}

#else

static int tls_random(void *p_rng, unsigned char *output, size_t output_len)
{
    uint64_t time_ms;

    (void)p_rng;

    time_ms = ls_osa_get_time_ms();

#if defined(MBEDTLS_AES_ALT)
    ali_seed((uint8_t *)&time_ms, sizeof(uint8_t *));
    ali_rand_gen(output, output_len);
#else
    srandom((unsigned int)time_ms);
    while(output_len > 0) {
        output[output_len - 1] = random() & 0xFF;
        output_len--;
    }
#endif

    return 0;
}

static void tls_debug(void *ctx, int level,
                      const char *file, int line, const char *str)
{
    (void)ctx;
    (void)level;

    SSL_DBG_LOG("%s:%04d: %s", file, line, str);
}

static int itls_ssl_sample(char *host, int port,
         char *product_key, char *product_secret, int debug_level)
{
    int ret = 0, len;
    char port_str[16];
    unsigned char buf[1024];
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_net_context server_fd;

    mbedtls_debug_set_threshold(debug_level);

    /*
     * 0. Initialize the session data
     */
    mbedtls_net_init(&server_fd);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);

    /*
     * 1. Start the connection
     */
    memset(port_str, 0, 16);
    ls_osa_snprintf(port_str, 16, "%u", port);

    SSL_DBG_LOG("  . Connecting to tcp/%s/%s...\n", host, port_str);

    if ((ret = mbedtls_net_connect(&server_fd,
                       host, port_str, MBEDTLS_NET_PROTO_TCP)) != 0) {
        SSL_DBG_LOG( " failed\n  ! mbedtls_net_connect returned %d\n\n", ret );
        goto exit;
    }

    SSL_DBG_LOG(" ok\n");

    /*
     * 2. Setup stuff
     */
    SSL_DBG_LOG("  . Setting up the SSL/TLS structure...\n");

    if ((ret = mbedtls_ssl_config_defaults(&conf,
                   MBEDTLS_SSL_IS_CLIENT,
                   MBEDTLS_SSL_TRANSPORT_STREAM,
                   MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        SSL_DBG_LOG(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret);
        goto exit;
    }

    SSL_DBG_LOG(" ok\n");

    mbedtls_ssl_conf_rng(&conf, tls_random, NULL);
    mbedtls_ssl_conf_dbg(&conf, tls_debug, NULL);

    /* OPTIONAL extra data for authentication */
    if ((ret = mbedtls_ssl_conf_auth_extra(
                   &conf, product_key, strlen(product_key))) != 0) {
        SSL_DBG_LOG(" failed\n  ! mbedtls_ssl_conf_auth_extra returned %d\n\n", ret);
        goto exit;
    }

    /* OPTIONAL token for one-time provisioning */
    if ((ret = mbedtls_ssl_conf_auth_token(
                   &conf, product_secret, strlen(product_secret))) != 0) {
        SSL_DBG_LOG(" failed\n  ! mbedtls_ssl_conf_auth_token returned %d\n\n", ret);
        goto exit;
    }

#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
    if ((ret = mbedtls_ssl_conf_max_frag_len( &conf, MBEDTLS_SSL_MAX_FRAG_LEN_1024)) != 0) {
        SSL_DBG_LOG(" failed\n  ! mbedtls_ssl_conf_max_frag_len returned %d\n\n", ret);
        goto exit;
    }
#endif

    if ((ret = mbedtls_ssl_setup( &ssl, &conf)) != 0) {
        SSL_DBG_LOG(" failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret);
        goto exit;
    }

    mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    /*
     * 3. Handshake
     */
    SSL_DBG_LOG("  . Performing the SSL/TLS handshake...\n");

    while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            SSL_DBG_LOG(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret);
            goto exit;
        }
    }

    SSL_DBG_LOG(" ok\n");

    /*
     * 4. Write the GET request
     */
    SSL_DBG_LOG("  > Write to server:\n");

    len = sizeof(mqtt_req_data);
    memcpy(buf, mqtt_req_data, len);

    while ((ret = mbedtls_ssl_write(&ssl, buf, len)) <= 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            SSL_DBG_LOG(" failed\n  ! mbedtls_ssl_write returned %d\n\n", ret);
            goto exit;
        }
    }

    len = ret;

    SSL_DBG_LOG(" %d bytes written\n\n", len);

    /*
     * 5. Read the HTTP response
     */
    SSL_DBG_LOG("  < Read from server:\n");

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
            SSL_DBG_LOG("failed\n  ! mbedtls_ssl_read returned %d\n\n", ret);
            break;
        }

        if (ret == 0) {
            SSL_DBG_LOG("\n\nEOF\n\n");
            break;
        }

        len = ret;
        SSL_DBG_LOG(" %d bytes read\n\n", len);
    } while (1);

    mbedtls_ssl_close_notify(&ssl);

    ret = 0;

exit:
    mbedtls_net_free(&server_fd);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);

    if (ret != 0) {
        ret = -1;
    }

    return ret;
}
#endif

int itls_client_sample(
         char *product_key, char *product_secret, int debug_level)
{
    int ret;
    int port = SERVER_PORT;
    char host[128] = {0};

#if defined(ON_DAILY)
    ls_osa_snprintf(host, 128, "%s",SERVER_NAME);
#else
    ls_osa_snprintf(host, 128, "%s.%s", product_key, SERVER_NAME);
#endif

#if defined(PLATFORM_ANDROID) || defined(PLATFORM_LINUX_LE)
    ret = itls_hal_sample(host, port,
               product_key, product_secret, debug_level);
    if (ret < 0) {
        return -1;
    }
#else
    ret = itls_ssl_sample(host, port,
               product_key, product_secret, debug_level);
    if (ret < 0) {
        return -1;
    }
#endif

    return 0;
}


