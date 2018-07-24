#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <aos/aos.h>
#include <aos/network.h>
#include <sys/time.h>

#include "ali_crypto.h"
#include "mbedtls/config.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/net_sockets.h"

#if defined(MBEDTLS_THREADING_ALT)
#include "mbedtls/threading.h"
#endif

#define SSL_DEBUG_LEVEL 1

#define SSL_PARAM_MAGIC 0x54321212

typedef struct _ssl_param_t
{
    size_t              magic;
    mbedtls_net_context net;
    mbedtls_x509_crt    ca_cert;
    mbedtls_ssl_config  conf;
    mbedtls_ssl_context ssl;
} ssl_param_t;

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
static void ssl_debug(void *ctx, int level, const char *file, int line,
                      const char *str)
{
    (void)ctx;
    (void)level;

    printf("%s, line: %d: %s", file, line, str);

    return;
}
#endif

void *mbedtls_ssl_connect(void *tcp_fd, const char *ca_cert, int ca_cert_len)
{
    int          ret;
    unsigned int result;
    ssl_param_t *ssl_param = NULL;

    if (ca_cert == NULL || ca_cert_len <= 0) {
        printf("ssl_connect: invalid input args!\n");
        return NULL;
    }

    if ((int)tcp_fd < 0) {
        printf("ssl_connect: invalid tcp fd - %d\n", (int)tcp_fd);
        return NULL;
    }

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(SSL_DEBUG_LEVEL);
#endif

#if defined(MBEDTLS_THREADING_ALT)
    mbedtls_threading_set_alt(threading_mutex_init, threading_mutex_free,
                              threading_mutex_lock, threading_mutex_unlock);
#endif

    ssl_param = malloc(sizeof(ssl_param_t));
    if (NULL == ssl_param) {
        printf("ssl_connect: malloc(%d) fail\n", sizeof(ssl_param_t));
        return NULL;
    } else {
        memset(ssl_param, 0, sizeof(ssl_param_t));

        mbedtls_net_init(&ssl_param->net);
        mbedtls_x509_crt_init(&ssl_param->ca_cert);
        mbedtls_ssl_config_init(&ssl_param->conf);
        mbedtls_ssl_init(&ssl_param->ssl);
        ssl_param->magic = SSL_PARAM_MAGIC;
    }

    /*
     * Initialize the connection
     */
    ssl_param->net.fd = (int)tcp_fd;

#if !defined(STM32_USE_SPI_WIFI)
    ret = mbedtls_net_set_block(&ssl_param->net);
    if (ret != 0) {
        printf("ssl_connect: set block failed- 0x%x\n", -ret);
        goto _err;
    }
#endif

    /*
     * Initialize certificates
     */
#if defined(CONFIG_SSL_DEBUG)
    printf("...... Loading the CA root certificate ... ");
#endif

    ret = mbedtls_x509_crt_parse(&ssl_param->ca_cert, (unsigned char *)ca_cert,
                                 (size_t)ca_cert_len + 1);
    if (ret < 0) {
        printf("ssl_connect: x509 parse failed- 0x%x\n", -ret);
        goto _err;
    }

#if defined(CONFIG_SSL_DEBUG)
    printf("ok (%d skipped)\n", ret);
#endif

    /*
     * setup stuff
     */
#if defined(CONFIG_SSL_DEBUG)
    printf("...... Setting up the SSL/TLS structure ... ");
#endif

    ret = mbedtls_ssl_config_defaults(&ssl_param->conf, MBEDTLS_SSL_IS_CLIENT,
                                      MBEDTLS_SSL_TRANSPORT_STREAM,
                                      MBEDTLS_SSL_PRESET_DEFAULT);
    if (ret != 0) {
        printf("ssl_connect: set ssl config failed - %d\n", ret);
        goto _err;
    }

#if defined(CONFIG_SSL_DEBUG)
    printf("ok\n");
#endif

    mbedtls_ssl_conf_authmode(&ssl_param->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_ca_chain(&ssl_param->conf, &ssl_param->ca_cert, NULL);
    mbedtls_ssl_conf_rng(&ssl_param->conf, ssl_random, NULL);

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_ssl_conf_dbg(&ssl_param->conf, ssl_debug, NULL);
#endif

#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
    ret = mbedtls_ssl_conf_max_frag_len(&ssl_param->conf,
                                        MBEDTLS_SSL_MAX_FRAG_LEN_4096);
    if (ret != 0) {
        printf("ssl_connect: mbedtls_ssl_conf_max_frag_len returned - %d\n",
               ret);
        goto _err;
    }
#endif

    ret = mbedtls_ssl_setup(&ssl_param->ssl, &ssl_param->conf);
    if (ret != 0) {
        printf("ssl_connect: mbedtls_ssl_setup returned - %d\n", ret);
        goto _err;
    }

    mbedtls_ssl_set_bio(&ssl_param->ssl, &ssl_param->net, mbedtls_net_send,
                        mbedtls_net_recv, NULL);

    /*
     * handshake
     */
#if defined(CONFIG_SSL_DEBUG)
    printf("...... Performing the SSL/TLS handshake ... ");
#endif

    while ((ret = mbedtls_ssl_handshake(&ssl_param->ssl)) != 0) {
        if ((ret != MBEDTLS_ERR_SSL_WANT_READ) &&
            (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            printf("ssl_connect: mbedtls_ssl_handshake returned -0x%x\n", -ret);
            goto _err;
        }
    }

#if defined(CONFIG_SSL_DEBUG)
    printf("ok\n");
#endif

    /*
     * verify the server certificate
     */
#if defined(CONFIG_SSL_DEBUG)
    printf("...... Verifying peer X.509 certificate ... ");
#endif

    result = mbedtls_ssl_get_verify_result(&ssl_param->ssl);
    if (result != 0) {
        printf("ssl_connect: verify result not confirmed - %d\n", result);
        goto _err;
    }

#if defined(CONFIG_SSL_DEBUG)
    printf("ok\n");
#endif

    return (void *)ssl_param;

_err:
    if (ssl_param != NULL) {
        mbedtls_net_free(&ssl_param->net);
        mbedtls_x509_crt_free(&ssl_param->ca_cert);
        mbedtls_ssl_free(&ssl_param->ssl);
        mbedtls_ssl_config_free(&ssl_param->conf);

        free(ssl_param);
        ssl_param = NULL;
    }

    return NULL;
}

int mbedtls_ssl_send(void *ssl, const char *buffer, int length)
{
    int          ret;
    int          total_len = 0;
    int          retry     = 0;
    ssl_param_t *ssl_param;

    if (ssl == NULL || buffer == NULL || length <= 0) {
        printf("ssl_send: invalid input args\n");
        return -1;
    }

#if defined(CONFIG_SSL_DEBUG)
    printf("...... > Send to server: ");
#endif

    ssl_param = (ssl_param_t *)ssl;
    if (ssl_param->magic != SSL_PARAM_MAGIC) {
        printf("ssl_send: invalid magic - 0x%x\n", ssl_param->magic);
        return -1;
    }

    do {
        ret = mbedtls_ssl_write(&ssl_param->ssl, (const unsigned char *)buffer,
                                (size_t)(length - total_len));
        if (ret > 0) {
            total_len += ret;
            buffer += ret;
        } else if (ret == 0) {
            /* EOF */
            break;
        } else {
            if (ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
                retry++;
                aos_msleep(10);
                continue;
            }

            if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
                /*
                 * indicate that write already complete,
                 * if call read again, it will return 0(EOF))
                 */
                break;
            }

            printf("ssl_send: mbedtls_ssl_write failed - 0x%x\n", -ret);

            return -1;
        }
    } while (total_len < length && retry < 10);

#if defined(CONFIG_SSL_DEBUG)
    printf("%d bytes sent retry %d\n", ret, retry);
#endif

#if defined(CONFIG_SSL_DEBUG)
    int            i    = 0;
    unsigned char *data = (unsigned char *)buffer;

    printf("send msg(%d %d): \n", length, ret);
    for (i = 0; i < ret; i++) {
        printf(" %02x ", data[i]);
    }
    printf("\n");
#endif

    return ret < 0 ? -1 : total_len;
}

int mbedtls_ssl_recv(void *ssl, char *buffer, int length)
{
    int          ret;
    int          total_len = 0;
    ssl_param_t *ssl_param;

    if (ssl == NULL || buffer == NULL || length <= 0) {
        printf("ssl_recv: invalid input args\n");
        return -1;
    } else {
        ssl_param = (ssl_param_t *)ssl;
        if (ssl_param->magic != SSL_PARAM_MAGIC) {
            printf("ssl_recv: invalid magic - 0x%x\n", ssl_param->magic);
            return -1;
        }
    }

#if defined(CONFIG_SSL_DEBUG)
    printf("...... < Read from server: ");
#endif

    do {
        ret = mbedtls_ssl_read(&ssl_param->ssl, (unsigned char *)buffer,
                               (size_t)length);
        if (ret > 0) {
            total_len = ret;
            break;
        } else if (ret == 0) {
            /* EOF */
            return 0;
        } else {
            if (ret == MBEDTLS_ERR_SSL_WANT_READ) {
                return -EAGAIN;
            }

            if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
                /*
                 * indicate that read already complete,
                 * if call read again, it will return 0(EOF))
                 */
                break;
            }

            printf("ssl_recv: mbedtls_ssl_read failed - 0x%x\n", -ret);

            return -1;
        }
    } while (0);

#if defined(CONFIG_SSL_DEBUG)
    printf("%d bytes read\n", total_len);
#endif

#if defined(CONFIG_SSL_DEBUG)
    int            i    = 0;
    unsigned char *data = (unsigned char *)buffer;

    printf("recv msg(%d %d): \n", length, total_len);
    for (i = 0; i < total_len; i++) {
        printf(" %02x ", data[i]);
    }
    printf("\n");
#endif

    return total_len;
}

int mbedtls_ssl_close(void *ssl)
{
    ssl_param_t *ssl_param;

    if (ssl == NULL) {
        return 0;
    }

    ssl_param = (ssl_param_t *)ssl;
    if (ssl_param->magic != SSL_PARAM_MAGIC) {
        printf("ssl_close: invalid magic - 0x%x\n", ssl_param->magic);
        return -1;
    }

    mbedtls_ssl_close_notify(&ssl_param->ssl);

    mbedtls_net_free(&ssl_param->net);
    mbedtls_x509_crt_free(&ssl_param->ca_cert);
    mbedtls_ssl_free(&ssl_param->ssl);
    mbedtls_ssl_config_free(&ssl_param->conf);

    memset(ssl_param, 0, sizeof(ssl_param_t));
    free(ssl_param);

#if defined(MBEDTLS_THREADING_ALT)
    mbedtls_threading_free_alt();
#endif

    return 0;
}
