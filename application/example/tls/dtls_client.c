/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "ali_crypto.h"
#include "mbedtls/config.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/error.h"
#include "mbedtls/timing.h"

#define DEBUG_LEVEL  4

#define LOCAL_TEST

#if defined(LOCAL_TEST)

#define SERVER_PORT     "4433"
#define SERVER_NAME     "localhost"
#define SERVER_ADDR     "127.0.0.1"
#define MESSAGE         "Echo this"

#define READ_TIMEOUT_MS 1000
#define MAX_RETRY       5

const char *dtls_test_ca_pem = "-----BEGIN CERTIFICATE-----\n"       \
                               "MIIDhzCCAm+gAwIBAgIBADANBgkqhkiG9w0BAQUFADA7MQswCQYDVQQGEwJOTDER\n" \
                               "MA8GA1UEChMIUG9sYXJTU0wxGTAXBgNVBAMTEFBvbGFyU1NMIFRlc3QgQ0EwHhcN\n" \
                               "MTEwMjEyMTQ0NDAwWhcNMjEwMjEyMTQ0NDAwWjA7MQswCQYDVQQGEwJOTDERMA8G\n" \
                               "A1UEChMIUG9sYXJTU0wxGTAXBgNVBAMTEFBvbGFyU1NMIFRlc3QgQ0EwggEiMA0G\n" \
                               "CSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDA3zf8F7vglp0/ht6WMn1EpRagzSHx\n" \
                               "mdTs6st8GFgIlKXsm8WL3xoemTiZhx57wI053zhdcHgH057Zk+i5clHFzqMwUqny\n" \
                               "50BwFMtEonILwuVA+T7lpg6z+exKY8C4KQB0nFc7qKUEkHHxvYPZP9al4jwqj+8n\n" \
                               "YMPGn8u67GB9t+aEMr5P+1gmIgNb1LTV+/Xjli5wwOQuvfwu7uJBVcA0Ln0kcmnL\n" \
                               "R7EUQIN9Z/SG9jGr8XmksrUuEvmEF/Bibyc+E1ixVA0hmnM3oTDPb5Lc9un8rNsu\n" \
                               "KNF+AksjoBXyOGVkCeoMbo4bF6BxyLObyavpw/LPh5aPgAIynplYb6LVAgMBAAGj\n" \
                               "gZUwgZIwDAYDVR0TBAUwAwEB/zAdBgNVHQ4EFgQUtFrkpbPe0lL2udWmlQ/rPrzH\n" \
                               "/f8wYwYDVR0jBFwwWoAUtFrkpbPe0lL2udWmlQ/rPrzH/f+hP6Q9MDsxCzAJBgNV\n" \
                               "BAYTAk5MMREwDwYDVQQKEwhQb2xhclNTTDEZMBcGA1UEAxMQUG9sYXJTU0wgVGVz\n" \
                               "dCBDQYIBADANBgkqhkiG9w0BAQUFAAOCAQEAuP1U2ABUkIslsCfdlc2i94QHHYeJ\n" \
                               "SsR4EdgHtdciUI5I62J6Mom+Y0dT/7a+8S6MVMCZP6C5NyNyXw1GWY/YR82XTJ8H\n" \
                               "DBJiCTok5DbZ6SzaONBzdWHXwWwmi5vg1dxn7YxrM9d0IjxM27WNKs4sDQhZBQkF\n" \
                               "pjmfs2cb4oPl4Y9T9meTx/lvdkRYEug61Jfn6cA+qHpyPYdTH+UshITnmp5/Ztkf\n" \
                               "m/UTSLBNFNHesiTZeH31NcxYGdHSme9Nc/gfidRa0FLOCfWxRlFqAI47zG9jAQCZ\n" \
                               "7Z2mCGDNMhjQc+BYcdnl0lPXjdDK6V0qCg1dVewhUBcW5gZKzV7e9+DpVA==\n"     \
                               "-----END CERTIFICATE-----\n";
#endif /* LOCAL_TEST */

static int ssl_random(void *prng, unsigned char *output, size_t output_len)
{
    struct timeval tv;

    (void)prng;

    gettimeofday(&tv, NULL);
    ali_seed((uint8_t *)&tv.tv_usec, sizeof(suseconds_t));
    ali_rand_gen(output, output_len);

    return 0;
}

#if defined(MBEDTLS_DEBUG_C)
static void ssl_debug(void *ctx, int level,
                      const char *file, int line, const char *str)
{
    (void)ctx;
    (void) level;

    printf("%s, line: %d: %s", file, line, str);

    return;
}
#endif

int dtls_client_sample(void)
{
    int ret, len;
    mbedtls_net_context server_fd;
    uint32_t flags;
    unsigned char buf[1024];
    int retry_left = MAX_RETRY;

    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt cacert;
    mbedtls_timing_delay_context timer;

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif

    /*
     * 0. Initialize the session data
     */
    mbedtls_net_init(&server_fd);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_x509_crt_init(&cacert);

    /*
     * 0. Load certificates
     */
    printf( "  . Loading the CA root certificate ..." );

    ret = mbedtls_x509_crt_parse(&cacert, (const unsigned char *)dtls_test_ca_pem,
                                 strlen(dtls_test_ca_pem) + 1);
    if (ret < 0) {
        printf(" failed\n !  mbedtls_x509_crt_parse returned -0x%x\n\n", -ret);
        goto exit;
    }

    printf( " ok (%d skipped)\n", ret );

    /*
     * 1. Start the connection
     */
    printf("  . Connecting to udp/%s/%s...", SERVER_NAME, SERVER_PORT);

    if ((ret = mbedtls_net_connect(&server_fd, SERVER_ADDR,
                                   SERVER_PORT, MBEDTLS_NET_PROTO_UDP)) != 0) {
        printf( " failed\n  ! mbedtls_net_connect returned %d\n\n", ret );
        goto exit;
    }

    printf(" ok\n");

    /*
     * 2. Setup stuff
     */
    printf("  . Setting up the DTLS structure...");

    if ((ret = mbedtls_ssl_config_defaults(&conf,
                                           MBEDTLS_SSL_IS_CLIENT,
                                           MBEDTLS_SSL_TRANSPORT_DATAGRAM,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        printf(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret);
        goto exit;
    }

    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
    mbedtls_ssl_conf_rng(&conf, ssl_random, NULL);

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_ssl_conf_dbg(&conf, ssl_debug, NULL);
#endif

#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
    ret = mbedtls_ssl_conf_max_frag_len(&conf, MBEDTLS_SSL_MAX_FRAG_LEN_4096);
    if (ret != 0) {
        printf( " failed\n  ! mbedtls_ssl_conf_max_frag_len returned %d\n\n", ret );
        goto exit;
    }
#endif

    if ((ret = mbedtls_ssl_setup( &ssl, &conf)) != 0) {
        printf( " failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret );
        goto exit;
    }

    if ((ret = mbedtls_ssl_set_hostname(&ssl, SERVER_NAME)) != 0) {
        printf( " failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret );
        goto exit;
    }

    mbedtls_ssl_set_bio(&ssl, &server_fd,
                        mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);

    mbedtls_ssl_set_timer_cb(&ssl, &timer, mbedtls_timing_set_delay,
                             mbedtls_timing_get_delay);

    printf( " ok\n" );

    /*
     * 4. Handshake
     */
    printf( "  . Performing the SSL/TLS handshake..." );

    do {
        ret = mbedtls_ssl_handshake( &ssl );
    } while ( ret == MBEDTLS_ERR_SSL_WANT_READ ||
              ret == MBEDTLS_ERR_SSL_WANT_WRITE );

    if (ret != 0) {
        printf( " failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n", -ret );
        goto exit;
    }

    printf(" ok\n");

    /*
     * 5. Verify the server certificate
     */
    printf( "  . Verifying peer X.509 certificate..." );

    if ((flags = mbedtls_ssl_get_verify_result(&ssl)) != 0) {
        printf("verify result not confirmed - %d\n", flags);
        goto exit;
    } else {
        printf( " ok\n" );
    }

    /*
     * 6. Write the echo request
     */
send_request:
    printf( "  > Write to server:" );

    len = sizeof( MESSAGE ) - 1;

    do {
        ret = mbedtls_ssl_write( &ssl, (unsigned char *) MESSAGE, len );
    } while ( ret == MBEDTLS_ERR_SSL_WANT_READ ||
              ret == MBEDTLS_ERR_SSL_WANT_WRITE );

    if ( ret < 0 )  {
        printf( " failed\n  ! mbedtls_ssl_write returned %d\n\n", ret );
        goto exit;
    }

    len = ret;
    printf( " %d bytes written\n%s\n", len, MESSAGE );

    /*
     * 7. Read the echo response
     */
    printf( "  < Read from server:" );

    len = sizeof( buf ) - 1;
    memset( buf, 0, sizeof( buf ) );

    do {
        ret = mbedtls_ssl_read( &ssl, buf, len );
    } while ( ret == MBEDTLS_ERR_SSL_WANT_READ ||
              ret == MBEDTLS_ERR_SSL_WANT_WRITE );

    if (ret <= 0) {
        switch (ret) {
            case MBEDTLS_ERR_SSL_TIMEOUT:
                printf( " timeout\n\n" );
                if ( retry_left-- > 0 ) {
                    goto send_request;
                }
                goto exit;

            case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
                printf( " connection was closed gracefully\n" );
                ret = 0;
                goto close_notify;

            default:
                printf( " mbedtls_ssl_read returned -0x%x\n\n", -ret );
                goto exit;
        }
    }

    len = ret;
    printf( " %d bytes read\n%s\n", len, buf );

    /*
     * 8. Done, cleanly close the connection
     */
close_notify:
    printf( "  . Closing the connection..." );

    /* No error checking, the connection might be closed already */
    do {
        ret = mbedtls_ssl_close_notify( &ssl );
    } while ( ret == MBEDTLS_ERR_SSL_WANT_WRITE );
    ret = 0;

    printf( " done\n" );

    /*
     * 9. Final clean-ups and exit
     */
exit:
    mbedtls_net_free(&server_fd);
    mbedtls_x509_crt_free(&cacert);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);

    return ret;
}
