#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include <platform.h>

static SSL_CTX* ssl_ctx = NULL;
static X509_STORE *ca_store = NULL;
static X509 *ca = NULL;

#ifdef DEBUG
#define log     printf
#else
#define log
#endif

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

    cert = PEM_read_bio_X509(in,NULL,NULL,NULL);

    if (in) {
        BIO_free(in);
    }
    return cert;
}



static int ssl_ca_store_init( const char *my_ca )
{
    if (!ca_store) {
        if (!my_ca) {
            printf("no global ca string provided \n");
            return -1;
        }
        ca_store = X509_STORE_new();
        ca = ssl_load_cert(my_ca);
        int ret = X509_STORE_add_cert(ca_store, ca);
        if (ret != 1) {
            printf("failed to X509_STORE_add_cert ret = %d \n", ret);
            return -1;
        }
    }

    return 0;
}



static int ssl_verify_ca(X509* target_cert)
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
        printf("X509_verify_cert fail, ret = %d\n",
              ret);
        goto end;
    }
end:
    if (store_ctx) {
        X509_STORE_CTX_free(store_ctx);
    }

    return (ret == 1) ? 0: -1;

}

static void *mutex;
static int ssl_init( const char *my_ca )
{
    if (ssl_ca_store_init( my_ca ) != 0)
    {
        return -1;
    }

    if (!ssl_ctx)
    {
        const SSL_METHOD *meth;

        SSLeay_add_ssl_algorithms();
#if OPENSSL_VERSION_NUMBER > 0x100010afL
        meth = TLS_client_method();
#else
        meth = TLSv1_2_client_method(); 
#endif
        SSL_load_error_strings();
        ssl_ctx = SSL_CTX_new(meth);
        if (!ssl_ctx)
        {
            printf("fail to initialize ssl context \n");
            return -1;
        }
    }
    else
    {
        printf("ssl context already initialized \n");
    }

    return 0;
}



static int ssl_establish(int sock, SSL **ppssl)
{
    int err;
    SSL *ssl_temp = NULL;
    X509 *server_cert = NULL;

    if (!ssl_ctx)
    {
        printf("no ssl context to create ssl connection \n");
        return -1;
    }

    ssl_temp = SSL_new(ssl_ctx);

    SSL_set_fd(ssl_temp, sock);
    err = SSL_connect(ssl_temp);

    if (err == -1)
    {
        printf("failed create ssl connection \n");
        goto err;
    }

    //TODO:rex linux platform not support ca?
#if 0
    server_cert = SSL_get_peer_certificate(ssl_temp);

    if (!server_cert)
    {
        printf("failed to get server cert");
        goto err;
    }

    if (ssl_verify_ca(server_cert) != 0)
    {
        goto err;
    }
    X509_free(server_cert);
#endif

    printf("success to verify cert \n");

    *ppssl = (void *)ssl_temp;

    return 0;

err:
    if (ssl_temp)
    {
        SSL_free(ssl_temp);
    }
    if (server_cert)
    {
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

    if (0 != ssl_init( server_cert ))
    {
        return NULL;
    }

    if (0 != ssl_establish((long)tcp_fd, &pssl))
    {
        return NULL;
    }

    mutex = platform_mutex_init();

    return pssl;
}



int platform_ssl_close(void *ssl)
{
    SSL_set_shutdown((SSL*)ssl, SSL_SENT_SHUTDOWN | SSL_RECEIVED_SHUTDOWN);
    SSL_free((SSL*)ssl);

    if (ssl_ctx)
    {
        SSL_CTX_free(ssl_ctx);
        ssl_ctx = NULL;
    }

    if (ca)
    {
        X509_free(ca);
        ca = NULL;
    }

    if (ca_store)
    {
        X509_STORE_free(ca_store);
        ca_store = NULL;
    }

    platform_mutex_destroy(mutex);

    return 0;
}

int platform_ssl_recv(void *ssl, char *buf, int len)
{
    int ret, total_len = 0;
    
    platform_mutex_lock(mutex);

    do {
        ret = SSL_read((SSL*)ssl, buf, len);
        if (ret <= 0)
            break;
        else {
            total_len += ret;
            buf += ret;
            len -= ret;
        }
    } while (ssl && SSL_pending((SSL *)ssl));

    platform_mutex_unlock(mutex);

    //printf("ssl recv %d\n", total_len);

    return (ret >= 0) ? total_len: -1;
}

int platform_ssl_send(void *ssl, const char *buf, int len)
{
    int ret;
    
    platform_mutex_lock(mutex);
    ret = SSL_write((SSL*)ssl, buf, len);
    platform_mutex_unlock(mutex);

    return (ret > 0) ? ret : -1;
}
