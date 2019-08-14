/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include "itls/config.h"
#include "itls/net.h"
#include "itls/debug.h"
#include "itls/timing.h"
#include "itls/ssl.h"
#include "itls/platform.h"

#include "ali_crypto.h"

#if defined(CONFIG_SESSION_TICKET_USED)
#include "sst.h"
#endif

#if defined(CONFIG_SSL_DTLS)

#if !defined(PLATFORM_LINUX_LE)

#if defined(ON_DAILY)
#define SERVER_PORT     5684
#define SERVER_NAME     "11.158.132.143"
#elif defined(ON_PRE)
#define SERVER_PORT     5684
#define SERVER_NAME     "pre.itls.cn-shanghai.aliyuncs.com"
#else
#define SERVER_PORT     5684
#define SERVER_NAME     "coap.cn-shanghai.link.aliyuncs.com"
#endif

#define MAX_RETRY        2
#define READ_TIMEOUT_MS  100

#define SESSION_TICKET   "./session_ticket"

unsigned char coap_req_data[] = {
    0x44, 0x02, 0x00, 0x01, 0x04, 0x03, 0x02, 0x01,
    0xb4, 0x61, 0x75, 0x74, 0x68, 0x11, 0x32, 0x51,
    0x32, 0xff, 0x7b, 0x22, 0x70, 0x72, 0x6f, 0x64,
    0x75, 0x63, 0x74, 0x4b, 0x65, 0x79, 0x22, 0x3a,
    0x22, 0x61, 0x31, 0x56, 0x32, 0x57, 0x53, 0x69,
    0x6e, 0x6b, 0x66, 0x63, 0x22, 0x2c, 0x22, 0x64,
    0x65, 0x76, 0x69, 0x63, 0x65, 0x4e, 0x61, 0x6d,
    0x65, 0x22, 0x3a, 0x22, 0x64, 0x65, 0x76, 0x69,
    0x63, 0x65, 0x5f, 0x30, 0x31, 0x22, 0x2c, 0x22,
    0x63, 0x6c, 0x69, 0x65, 0x6e, 0x74, 0x49, 0x64,
    0x22, 0x3a, 0x22, 0x61, 0x31, 0x56, 0x32, 0x57,
    0x53, 0x69, 0x6e, 0x6b, 0x66, 0x63, 0x2e, 0x64,
    0x65, 0x76, 0x69, 0x63, 0x65, 0x5f, 0x30, 0x31,
    0x22, 0x2c, 0x22, 0x73, 0x65, 0x63, 0x75, 0x72,
    0x65, 0x6d, 0x6f, 0x64, 0x65, 0x22, 0x3a, 0x22,
    0x39, 0x22, 0x2c, 0x22, 0x61, 0x75, 0x74, 0x68,
    0x74, 0x79, 0x70, 0x65, 0x22, 0x3a, 0x22, 0x69,
    0x64, 0x32, 0x22, 0x2c, 0x22, 0x73, 0x69, 0x67,
    0x6e, 0x22, 0x3a, 0x22, 0x37, 0x39, 0x66, 0x31,
    0x38, 0x63, 0x39, 0x66, 0x31, 0x37, 0x30, 0x66,
    0x65, 0x63, 0x62, 0x66, 0x36, 0x64, 0x33, 0x37,
    0x61, 0x35, 0x37, 0x33, 0x64, 0x32, 0x33, 0x38,
    0x38, 0x64, 0x66, 0x39, 0x22, 0x7d
};

#if defined(CONFIG_SESSION_TICKET_USED)
static int tls_get_session_ticket(mbedtls_ssl_session *session)
{
    uint32_t stat;
    uint32_t type;
    uint32_t item_len = 0;
    uint8_t *item_data = NULL;

    if (session == NULL) {
        return -1;
    }

    memset(session, 0, sizeof(mbedtls_ssl_session));

    sst_init();

    stat = sst_get_item(SESSION_TICKET, NULL, &item_len, &type);
    if (stat == SST_ERROR_ITEM_NOT_FOUND) {
        return 0;
    } else if (stat != SST_SUCCESS &&
               stat != SST_ERROR_SHORT_BUFFER) {
        SSL_DBG_LOG("sst get item fail, %d\n", stat);
        return -1;
    }

    if (item_len <= sizeof(mbedtls_ssl_session)) {
        SSL_DBG_LOG("invalid item length, %d\n", item_len);
        return 0;
    }

    item_data = mbedtls_calloc(1, item_len);
    if (item_data == NULL) {
        SSL_DBG_LOG("out of mem, %d\n", item_len);
        return -1;
    }

    stat = sst_get_item(SESSION_TICKET, item_data, &item_len, &type);
    if (stat != SST_SUCCESS) {
        SSL_DBG_LOG("sst get item fail, %d\n", stat);
        goto _out;
    }

    memcpy(session, item_data, sizeof(mbedtls_ssl_session));

    /* should use "mbedtls_calloc" for session ticket */
    session->ticket = mbedtls_calloc(1, session->ticket_len);
    if (session->ticket == NULL) {
        SSL_DBG_LOG("out of mem, %d\n", session->ticket_len);
        goto _out;
    }

    memcpy(session->ticket, item_data + sizeof(mbedtls_ssl_session), session->ticket_len);

    SSL_DBG_LOG("get a session ticket from secure storage!\n");

_out:
    if (item_data != NULL) {
        mbedtls_free(item_data);
    }

    if (stat != SST_SUCCESS) {
        return -1;
    }

    return 0;
}

static int tls_set_session_ticket(mbedtls_ssl_session *session)
{
    uint32_t stat;
    uint32_t item_len;
    uint8_t *item_data = NULL;

    if (session == NULL) {
        return -1;
    }

    item_len = sizeof(mbedtls_ssl_session) + session->ticket_len;
    item_data = mbedtls_calloc(1, item_len);
    if (item_data == NULL) {
        SSL_DBG_LOG("out of mem, %d\n", item_len);
        return -1;
    }

    memcpy(item_data, session, sizeof(mbedtls_ssl_session));
    memcpy(item_data + sizeof(mbedtls_ssl_session), session->ticket, session->ticket_len);

    sst_delete_item(SESSION_TICKET);

    stat = sst_add_item(SESSION_TICKET, item_data, item_len, SST_TYPE_USERDATA, 1);
    if (stat != SST_SUCCESS) {
        SSL_DBG_LOG("sst add item fail, 0x%x\n", stat);
        goto _out;
    }

    SSL_DBG_LOG("new session ticket has been saved to secure storage!\n");

_out:
    if (item_data != NULL) {
        mbedtls_free(item_data);
    }

    if (stat != SST_SUCCESS) {
        return -1;
    }

    return 0;
}
#endif

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

static int idtls_ssl_sample(char *host, int port,
         char *product_key, char *product_secret, int debug_level)
{
    int ret = 0, len;
    char port_str[16];
    unsigned char buf[1024];
    int retry_left = MAX_RETRY;
#if defined(CONFIG_SESSION_TICKET_USED)
    mbedtls_ssl_session saved_session;
#endif

    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_net_context server_fd;
    mbedtls_timing_delay_context timer;

    mbedtls_debug_set_threshold(debug_level);

    /*
     * 0. Initialize the session data
     */
    mbedtls_net_init(&server_fd);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);

#if defined(CONFIG_SESSION_TICKET_USED)
    memset(&saved_session, 0, sizeof(mbedtls_ssl_session));
#endif

    /*
     * 1. Start the connection
     */
    memset(port_str, 0, 16);
    ls_osa_snprintf(port_str, 16, "%u", SERVER_PORT);

    SSL_DBG_LOG("  . Connecting to udp/%s/%s...\n", host, port_str);

    if ((ret = mbedtls_net_connect(&server_fd,
                       host, port_str, MBEDTLS_NET_PROTO_UDP)) != 0) {
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
                   MBEDTLS_SSL_TRANSPORT_DATAGRAM,
                   MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        SSL_DBG_LOG(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret);
        goto exit;
    }

    mbedtls_ssl_conf_handshake_timeout(&conf, 2000, 8000);

    mbedtls_ssl_conf_rng(&conf, tls_random, NULL);
    mbedtls_ssl_conf_dbg(&conf, tls_debug, NULL);

#if defined(CONFIG_SESSION_TICKET_USED)
    mbedtls_ssl_conf_session_tickets(&conf, MBEDTLS_SSL_SESSION_TICKETS_ENABLED);
#endif

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

    mbedtls_ssl_set_bio(&ssl, &server_fd,
                        mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);

    mbedtls_ssl_set_timer_cb(&ssl, &timer, mbedtls_timing_set_delay,
                                           mbedtls_timing_get_delay);

#if defined(CONFIG_SESSION_TICKET_USED)
    if (tls_get_session_ticket(&saved_session) == 0) {
        if ((ret = mbedtls_ssl_set_session(&ssl, &saved_session)) != 0) {
            SSL_DBG_LOG(" failed\n  ! mbedtls_ssl_conf_session returned %d\n\n", ret);
            goto exit;
        }
    }
#endif

    SSL_DBG_LOG(" ok\n");

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

#if defined(CONFIG_SESSION_TICKET_USED)
    if (saved_session.ticket == NULL ||
        memcmp(ssl.session->ticket, saved_session.ticket, saved_session.ticket_len)) {
        if ((ret = mbedtls_ssl_get_session(&ssl, &saved_session)) != 0) {
            SSL_DBG_LOG(" failed\n  ! mbedtls_ssl_get_session returned -0x%x\n\n", -ret);
            goto exit;
        } else {
            if (tls_set_session_ticket(&saved_session) != 0) {
                 SSL_DBG_LOG(" failed\n  ! HAL_SetSessionTicket\n\n");
                 goto exit;
            }
        }
    } else {
        SSL_DBG_LOG("a session has been resumed based on session ticket!\n");
    }
#endif

    SSL_DBG_LOG(" ok\n");

    /*
     * 4. Write the GET request
     */
send_request:
    SSL_DBG_LOG("  > Write to server:\n");

    len = sizeof(coap_req_data);
    memcpy(buf, coap_req_data, len);

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

    mbedtls_ssl_conf_read_timeout( &conf, READ_TIMEOUT_MS );

    do {
        len = sizeof(buf) - 1;
        memset(buf, 0, sizeof(buf));
        ret = mbedtls_ssl_read(&ssl, buf, len);

        if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
            continue;
        }

        if (ret <= 0) {
            switch(ret) {
                case MBEDTLS_ERR_SSL_TIMEOUT:
                    SSL_DBG_LOG( " timeout\n\n" );
                    if( retry_left-- > 0 )
                        goto send_request;
                    ret = 0;
                    goto exit;

                case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
                    SSL_DBG_LOG( " connection was closed gracefully\n" );
                    ret = 0;
                    goto close_notify;

                default:
                    SSL_DBG_LOG( " mbedtls_ssl_read returned -0x%x\n\n", -ret );
                    goto exit;
            }
        }

        len = ret;
        SSL_DBG_LOG(" %d bytes read\n\n", len);
    } while (1);

    /*
     * 6. Done, cleanly close the connection
     */
close_notify:
    SSL_DBG_LOG( "  . Closing the connection..." );

    /* No error checking, the connection might be closed already */
    do ret = mbedtls_ssl_close_notify( &ssl );
    while( ret == MBEDTLS_ERR_SSL_WANT_WRITE );
    ret = 0;

    SSL_DBG_LOG( " done\n" );

exit:
    mbedtls_net_free(&server_fd);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);

#if defined(CONFIG_SESSION_TICKET_USED)
    mbedtls_ssl_session_free(&saved_session);
#endif

    if (ret != 0) {
        ret = -1;
    }

    return ret;
}

int idtls_client_sample(
         char *product_key, char *product_secret, int debug_level)
{
    int ret;
    int port = SERVER_PORT;
    char host[128] = {0};

#if defined(ON_DAILY) || defined(ON_PRE)
    ls_osa_snprintf(host, 128, "%s",SERVER_NAME);
#else
    ls_osa_snprintf(host, 128, "%s.%s", product_key, SERVER_NAME);
#endif

    ret = idtls_ssl_sample(host, port,
               product_key, product_secret, debug_level);
    if (ret < 0) {
        return -1;
    }

    return 0;
}

#else

int idtls_client_sample(
         char *product_key, char *product_secret, int debug_level)
{

    SSL_DBG_LOG("iDTLS is not supported for Linux_LE Product!\n");

    return 0;
}

#endif

#endif
