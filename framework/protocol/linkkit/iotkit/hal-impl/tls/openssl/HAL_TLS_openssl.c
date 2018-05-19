
#include "iot_import.h"

#include "openssl/crypto.h"
#include "openssl/x509.h"
#include "openssl/pem.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

static SSL_CTX *ssl_ctx = NULL;
static X509_STORE *ca_store = NULL;
static X509 *ca = NULL;


#pragma comment(lib,"libeay32.lib")
#pragma comment(lib,"ssleay32.lib")


#define PLATFORM_WINSOCK_LOG    printf
#define PLATFORM_WINSOCK_PERROR printf

static X509 *ssl_load_cert(const char *cert_str)
{
    X509 *cert = NULL;
    BIO *in = NULL;
    if (!cert_str) {
        return NULL;
    }

    in = BIO_new_mem_buf((void *)cert_str, -1);

    if (!in) {
        return NULL;
    }

    cert = PEM_read_bio_X509(in, NULL, NULL, NULL);

    if (in) {
        BIO_free(in);
    }
    return cert;
}


static int ssl_ca_store_init(const char *my_ca)
{
    int ret;
    if (!ca_store) {
        if (!my_ca) {
            printf("no global ca string provided \n");
            return -1;
        }
        ca_store = X509_STORE_new();
        ca = ssl_load_cert(my_ca);
        ret = X509_STORE_add_cert(ca_store, ca);
        if (ret != 1) {
            printf("failed to X509_STORE_add_cert ret = %d \n", ret);
            return -1;
        }
    }

    return 0;
}



static int ssl_verify_ca(X509 *target_cert)
{
    STACK_OF(X509) *ca_stack = NULL;
    X509_STORE_CTX *store_ctx = NULL;
    int ret;

    store_ctx = X509_STORE_CTX_new();

    ret = X509_STORE_CTX_init(store_ctx, ca_store, target_cert, ca_stack);

    if (ret != 1) {
        printf("X509_STORE_CTX_init fail, ret = %d", ret);
        goto end;
    }
    ret = X509_verify_cert(store_ctx);
    if (ret != 1) {
        printf("X509_verify_cert fail, ret = %d, error id = %d, %s\n",
               ret, store_ctx->error,
               X509_verify_cert_error_string(store_ctx->error));
        goto end;
    }
end:
    if (store_ctx) {
        X509_STORE_CTX_free(store_ctx);
    }

    return (ret == 1) ? 0 : -1;

}


static int ssl_init(const char *my_ca)
{
    if (ssl_ca_store_init(my_ca) != 0) {
        return -1;
    }

    if (!ssl_ctx) {
        SSL_METHOD *meth;

        SSLeay_add_ssl_algorithms();

        meth = TLSv1_client_method();

        SSL_load_error_strings();
        ssl_ctx = SSL_CTX_new(meth);
        if (!ssl_ctx) {
            printf("fail to initialize ssl context \n");
            return -1;
        }
    } else {
        printf("ssl context already initialized \n");
    }

    return 0;
}


static int ssl_establish(int sock, SSL **ppssl)
{
    int err;
    SSL *ssl_temp;
    X509 *server_cert;

    if (!ssl_ctx) {
        printf("no ssl context to create ssl connection \n");
        return -1;
    }

    ssl_temp = SSL_new(ssl_ctx);

    SSL_set_fd(ssl_temp, sock);
    err = SSL_connect(ssl_temp);

    if (err == -1) {
        printf("failed create ssl connection \n");
        goto err;
    }

    server_cert = SSL_get_peer_certificate(ssl_temp);

    if (!server_cert) {
        printf("failed to get server cert");
        goto err;
    }

    /* if (ssl_verify_ca(server_cert) != 0) */
    /* { */
    /*     goto err; */
    /* } */

    X509_free(server_cert);

    printf("success to verify cert \n");

    *ppssl = (void *)ssl_temp;

    return 0;

err:
    if (ssl_temp) {
        SSL_free(ssl_temp);
    }
    if (server_cert) {
        X509_free(server_cert);
    }

    *ppssl = NULL;
    return -1;
}



void *platform_ssl_connect(void *tcp_fd,
                           const char *server_cert,
                           int server_cert_len)
{
    SSL *pssl;

    if (0 != ssl_init(server_cert)) {
        return NULL;
    }

    if (0 != ssl_establish((int)tcp_fd, &pssl)) {
        return NULL;
    }

    return pssl;
}



int platform_ssl_close(void *ssl)
{
    /* SOCKET sock = (SOCKET)SSL_get_fd( ssl ); */

    SSL_set_shutdown((SSL *)ssl, SSL_SENT_SHUTDOWN | SSL_RECEIVED_SHUTDOWN);
    SSL_free((SSL *)ssl);

    if (ssl_ctx) {
        SSL_CTX_free(ssl_ctx);
        ssl_ctx = NULL;
    }

    if (ca) {
        X509_free(ca);
        ca = NULL;
    }

    if (ca_store) {
        X509_STORE_free(ca_store);
        ca_store = NULL;
    }

    /* ssl_destroy_net( sock ); */

    return 0;
}


static uint64_t time_left(uint64_t t_end, uint64_t t_now)
{
    uint64_t t_left;

    t_left = t_end - t_now;

    if (t_left >= 0x80000000) {
        return 0;
    }

    return t_left;
}


int platform_ssl_recv(void *ssl, char *buf, uint32_t len, int timeout_ms)
{
    int ret, err_code;
    uint32_t len_recv;
    uint64_t t_end, t_left;
    fd_set sets;
    struct timeval timeout;
    int fd = SSL_get_fd(ssl);

    t_end = GetTickCount() + timeout_ms;
    len_recv = 0;
    err_code = 0;

    if ((uint32_t)SSL_pending(ssl) >= len) {
        len_recv = SSL_read(ssl, buf, len);
    } else {
        do {
            t_left = time_left(t_end, GetTickCount());

            FD_ZERO(&sets);
            FD_SET(fd, &sets);

            timeout.tv_sec = t_left / 1000;
            timeout.tv_usec = (t_left % 1000) * 1000;

            ret = select(fd + 1, &sets, NULL, NULL, &timeout);
            if (ret > 0) {
                ret = SSL_read((SSL *)ssl, buf + len_recv, len - len_recv);
                if (ret > 0) {
                    len_recv += ret;
                } else if (0 == ret) {
                    PLATFORM_WINSOCK_LOG("connection is closed");
                    err_code = -1;
                    break;
                } else {
                    if (SSL_ERROR_WANT_READ == SSL_get_error(ssl, ret)) {
                        continue;
                    }
                    PLATFORM_WINSOCK_PERROR("recv fail");
                    err_code = -2;
                    break;
                }
            } else if (0 == ret) {
                break;
            } else {
                PLATFORM_WINSOCK_PERROR("select-read fail");
                err_code = -2;
                break;
            }
        } while ((len_recv < len) && (time_left(t_end, GetTickCount()) > 0));
    }
    /* priority to return data bytes if any data be received from TCP connection. */
    /* It will get error code on next calling */

    return (0 != len_recv) ? len_recv : err_code;
}



int platform_ssl_send(void *ssl, const char *buf, uint32_t len)
{
    int ret = SSL_write((SSL *)ssl, buf, len);

    return (ret > 0) ? ret : -1;
}


struct ssl_info_st {
    long tcp;
    long ssl;
};

uintptr_t HAL_SSL_Establish(const char *host,
                            uint16_t port,
                            const char *ca_crt,
                            size_t ca_crt_len)
{
    long tmp;
    struct ssl_info_st *handle;

    handle = malloc(sizeof(struct ssl_info_st));
    if (NULL == handle) {
        printf("no enough memory\n");
        return (uintptr_t)NULL;
    }

    tmp = HAL_TCP_Establish(host, port);
    if (-1 == tmp) {
        return (uintptr_t)NULL;
    }
    handle->tcp = tmp;

    tmp = (long)platform_ssl_connect((void *)tmp, ca_crt, ca_crt_len);
    if (0 == tmp) {
        HAL_TCP_Destroy(handle->tcp);
        free(handle);
        return (uintptr_t)NULL;
    }

    handle->ssl = tmp;
    return (uintptr_t)handle;
}


int32_t HAL_SSL_Destroy(uintptr_t handle)
{
    struct ssl_info_st *h = (struct ssl_info_st *)handle;

    PLATFORM_WINSOCK_PERROR("close ssl connection\n");

    if (0 != h->ssl) {
        platform_ssl_close((void *)h->ssl);
    }

    if (0 != h->tcp) {
        HAL_TCP_Destroy(h->tcp);
    }

    free((void *)handle);
    return 0;
}


int HAL_SSL_Read(uintptr_t handle, char *buf, int len, int timeout_ms)
{
    return platform_ssl_recv((void *)(((struct ssl_info_st *)handle)->ssl), buf, len, timeout_ms);
}

int HAL_SSL_Write(uintptr_t handle, const char *buf, int len, int timeout_ms)
{
    timeout_ms = timeout_ms;
    return platform_ssl_send((void *)(((struct ssl_info_st *)handle)->ssl), buf, len);
}
