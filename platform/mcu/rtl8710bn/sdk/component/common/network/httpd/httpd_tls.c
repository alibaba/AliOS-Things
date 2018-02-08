#include "FreeRTOS.h"
#include "task.h"
#include "platform_stdlib.h"

#include "httpd.h"

#if (HTTPD_USE_TLS == HTTPD_TLS_POLARSSL)
#include "polarssl/ssl.h"
#include "polarssl/memory.h"
#include "polarssl/base64.h"

struct httpd_tls {
	ssl_context ctx;                 /*!< Context for PolarSSL */
};

static x509_crt httpd_certs;         /*!< Certificates of server and CA */
static pk_context httpd_key;         /*!< Private key of server */

static int _verify_func(void *data, x509_crt *crt, int depth, int *flags)
{
	char buf[1024];
	x509_crt_info(buf, sizeof(buf) - 1, "", crt);

	if(*flags)
		printf("\n[HTTPD] ERROR: certificate verify\n%s\n", buf);
	else
		printf("\n[HTTPD] Certificate verified\n%s\n", buf);

	return 0;
}

#elif (HTTPD_USE_TLS == HTTPD_TLS_MBEDTLS)
#include "mbedtls/ssl.h"
#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/base64.h"

struct httpd_tls {
	mbedtls_ssl_context ctx;         /*!< Context for mbedTLS */
	mbedtls_ssl_config conf;         /*!< Configuration for mbedTLS */
};

static mbedtls_x509_crt httpd_certs; /*!< Certificates of server and CA */
static mbedtls_pk_context httpd_key; /*!< Private key of server */

static int _verify_func(void *data, mbedtls_x509_crt *crt, int depth, uint32_t *flags)
{
	char buf[1024];
	mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "", crt);

	if(*flags)
		printf("\n[HTTPD] ERROR: certificate verify\n%s\n", buf);
	else
		printf("\n[HTTPD] Certificate verified\n%s\n", buf);

	return 0;
}

static void* _calloc_func(size_t nmemb, size_t size)
{
	size_t mem_size;
	void *ptr = NULL;

	mem_size = nmemb * size;
	ptr = pvPortMalloc(mem_size);

	if(ptr)
		memset(ptr, 0, mem_size);

	return ptr;
}
#endif /* HTTPC_USE_POLARSSL */

static int _random_func(void *p_rng, unsigned char *output, size_t output_len)
{
	rtw_get_random_bytes(output, output_len);
	return 0;
}

int httpd_tls_setup_init(const char *server_cert, const char *server_key, const char *ca_certs)
{
#if (HTTPD_USE_TLS == HTTPD_TLS_POLARSSL)
	int ret = 0;

	memory_set_own(pvPortMalloc, vPortFree);
	memset(&httpd_certs, 0, sizeof(x509_crt));
	memset(&httpd_key, 0, sizeof(pk_context));
	x509_crt_init(&httpd_certs);
	pk_init(&httpd_key);

	// set server certificate for the first certificate
	if((ret = x509_crt_parse(&httpd_certs, (const unsigned char *) server_cert, strlen(server_cert))) != 0) {
		printf("\n[HTTPD] ERROR: x509_crt_parse %d\n", ret);
		ret = -1;
		goto exit;
	}

	// set trusted ca certificates next to server certificate
	if((ret = x509_crt_parse(&httpd_certs, (const unsigned char *) ca_certs, strlen(ca_certs))) != 0) {
		printf("\n[HTTPD] ERROR: x509_crt_parse %d\n", ret);
		ret = -1;
		goto exit;
	}

	if((ret = pk_parse_key(&httpd_key, (const unsigned char *) server_key, strlen(server_key), NULL, 0)) != 0) {
		printf("\n[HTTPD] ERROR: pk_parse_key %d\n", ret);
		ret = -1;
		goto exit;
	}

exit:
	if(ret) {
		x509_crt_free(&httpd_certs);
		pk_free(&httpd_key);
	}

	return ret;
#elif (HTTPD_USE_TLS == HTTPD_TLS_MBEDTLS)
	int ret = 0;

	mbedtls_platform_set_calloc_free(_calloc_func, vPortFree);
	memset(&httpd_certs, 0, sizeof(mbedtls_x509_crt));
	memset(&httpd_key, 0, sizeof(mbedtls_pk_context));
	mbedtls_x509_crt_init(&httpd_certs);
	mbedtls_pk_init(&httpd_key);

	// set server certificate for the first certificate
	if((ret = mbedtls_x509_crt_parse(&httpd_certs, (const unsigned char *) server_cert, strlen(server_cert) + 1)) != 0) {
		printf("\n[HTTPD] ERROR: mbedtls_x509_crt_parse %d\n", ret);
		ret = -1;
		goto exit;
	}

	// set trusted ca certificates next to server certificate
	if((ret = mbedtls_x509_crt_parse(&httpd_certs, (const unsigned char *) ca_certs, strlen(ca_certs) + 1)) != 0) {
		printf("\n[HTTPD] ERROR: mbedtls_x509_crt_parse %d\n", ret);
		ret = -1;
		goto exit;
	}

	if((ret = mbedtls_pk_parse_key(&httpd_key, (const unsigned char *) server_key, strlen(server_key) + 1, NULL, 0)) != 0) {
		printf("\n[HTTPD] ERROR: mbedtls_pk_parse_key %d\n", ret);
		ret = -1;
		goto exit;
	}

exit:
	if(ret) {
		mbedtls_x509_crt_free(&httpd_certs);
		mbedtls_pk_free(&httpd_key);
	}

	return ret;
#endif
}

void httpd_tls_setup_free(void)
{
#if (HTTPD_USE_TLS == HTTPD_TLS_POLARSSL)
	x509_crt_free(&httpd_certs);
	pk_free(&httpd_key);
#elif (HTTPD_USE_TLS == HTTPD_TLS_MBEDTLS)
	mbedtls_x509_crt_free(&httpd_certs);
	mbedtls_pk_free(&httpd_key);
#endif
}

void *httpd_tls_new_handshake(int *sock, uint8_t secure)
{
#if (HTTPD_USE_TLS == HTTPD_TLS_POLARSSL)
	int ret = 0;
	struct httpd_tls *tls = NULL;
	ssl_context *ssl = NULL;

	if((tls = (struct httpd_tls *) malloc(sizeof(struct httpd_tls))) != NULL) {
		memset(tls, 0, sizeof(struct httpd_tls));
		ssl = &tls->ctx;

		if((ret = ssl_init(ssl)) != 0) {
			printf("\n[HTTPD] ERROR: ssl_init %d\n", ret);
			ret = -1;
			goto exit;
		}

		ssl_set_endpoint(ssl, SSL_IS_SERVER);
		ssl_set_authmode(ssl, SSL_VERIFY_NONE);
		ssl_set_rng(ssl, _random_func, NULL);
		ssl_set_bio(ssl, net_recv, sock, net_send, sock);
		ssl_set_ca_chain(ssl, httpd_certs.next, NULL, NULL);

		if(secure == HTTPD_SECURE_TLS_VERIFY) {
			ssl_set_authmode(ssl, SSL_VERIFY_REQUIRED);
			ssl_set_verify(ssl, _verify_func, NULL);
		}

		if((ret = ssl_set_own_cert(ssl, &httpd_certs, &httpd_key)) != 0) {
			printf("\n[HTTPD] ERROR: ssl_set_own_cert %d\n", ret);
			ret = -1;
			goto exit;
		}

		if((ret = ssl_handshake(ssl)) != 0) {
			printf("\n[HTTPD] ERROR: ssl_handshake %d\n", ret);
			ret = -1;
			goto exit;
		}
		else {
			printf("\n[HTTPD] Use ciphersuite %s\n", ssl_get_ciphersuite(ssl));
		}

	}
	else {
		printf("\n[HTTPD] ERROR: httpd_malloc\n");
		ret = -1;
		goto exit;
	}

exit:
	if(ret && tls) {
		ssl_close_notify(ssl);
		ssl_free(ssl);
		free(tls);
		tls = NULL;
	}

	return (void *) tls;
#elif (HTTPD_USE_TLS == HTTPD_TLS_MBEDTLS)
	int ret = 0;
	struct httpd_tls *tls = NULL;
	mbedtls_ssl_context *ssl;
	mbedtls_ssl_config *conf;

	if((tls = (struct httpd_tls *) malloc(sizeof(struct httpd_tls))) != NULL) {
		memset(tls, 0, sizeof(struct httpd_tls));
		ssl = &tls->ctx;
		conf = &tls->conf;

		mbedtls_ssl_init(ssl);
		mbedtls_ssl_config_init(conf);

		if((ret = mbedtls_ssl_config_defaults(conf,
				MBEDTLS_SSL_IS_SERVER,
				MBEDTLS_SSL_TRANSPORT_STREAM,
				MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {

			printf("\n[HTTPD] ERROR: mbedtls_ssl_config_defaults %d\n", ret);
			ret = -1;
			goto exit;
		}

		mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_NONE);
		mbedtls_ssl_conf_rng(conf, _random_func, NULL);
		mbedtls_ssl_conf_ca_chain(conf, httpd_certs.next, NULL);

		if(secure == HTTPD_SECURE_TLS_VERIFY) {
			mbedtls_ssl_conf_authmode(conf, MBEDTLS_SSL_VERIFY_REQUIRED);
			mbedtls_ssl_conf_verify(conf, _verify_func, NULL);
		}

		if((ret = mbedtls_ssl_conf_own_cert(conf, &httpd_certs, &httpd_key)) != 0) {
			printf("\n[HTTPD] ERROR: mbedtls_ssl_conf_own_cert %d\n", ret);
			ret = -1;
			goto exit;
		}

		if((ret = mbedtls_ssl_setup(ssl, conf)) != 0) {
			printf("\n[HTTPD] ERROR: mbedtls_ssl_setup %d\n", ret);
			ret = -1;
			goto exit;
		}

		mbedtls_ssl_set_bio(ssl, sock, mbedtls_net_send, mbedtls_net_recv, NULL);

		if((ret = mbedtls_ssl_handshake(ssl)) != 0) {
			printf("\n[HTTPD] ERROR: mbedtls_ssl_handshake %d\n", ret);
			ret = -1;
			goto exit;
		}
		else {
			printf("\n[HTTPD] Use ciphersuite %s\n", mbedtls_ssl_get_ciphersuite(ssl));
		}

	}
	else {
		printf("\n[HTTPD] ERROR: httpd_malloc\n");
		ret = -1;
		goto exit;
	}

exit:
	if(ret && tls) {
		mbedtls_ssl_close_notify(ssl);
		mbedtls_ssl_free(ssl);
		mbedtls_ssl_config_free(conf);
		free(tls);
		tls = NULL;
	}

	return (void *) tls;
#endif
}

void httpd_tls_free(void *tls_in)
{
	struct httpd_tls *tls = (struct httpd_tls *) tls_in;

#if (HTTPD_USE_TLS == HTTPD_TLS_POLARSSL)
	ssl_free(&tls->ctx);
	free(tls);
#elif (HTTPD_USE_TLS == HTTPD_TLS_MBEDTLS)
	mbedtls_ssl_free(&tls->ctx);
	mbedtls_ssl_config_free(&tls->conf);
	free(tls);
#endif
}

void httpd_tls_close(void *tls_in)
{
	struct httpd_tls *tls = (struct httpd_tls *) tls_in;

#if (HTTPD_USE_TLS == HTTPD_TLS_POLARSSL)
	ssl_close_notify(&tls->ctx);
#elif (HTTPD_USE_TLS == HTTPD_TLS_MBEDTLS)
	mbedtls_ssl_close_notify(&tls->ctx);
#endif
}

int httpd_tls_read(void *tls_in, uint8_t *buf, size_t buf_len)
{
	struct httpd_tls *tls = (struct httpd_tls *) tls_in;

#if (HTTPD_USE_TLS == HTTPD_TLS_POLARSSL)
	return ssl_read(&tls->ctx, buf, buf_len);
#elif (HTTPD_USE_TLS == HTTPD_TLS_MBEDTLS)
	return mbedtls_ssl_read(&tls->ctx, buf, buf_len);
#endif
}

int httpd_tls_write(void *tls_in, uint8_t *buf, size_t buf_len)
{
	struct httpd_tls *tls = (struct httpd_tls *) tls_in;

#if (HTTPD_USE_TLS == HTTPD_TLS_POLARSSL)
	return ssl_write(&tls->ctx, buf, buf_len);
#elif (HTTPD_USE_TLS == HTTPD_TLS_MBEDTLS)
	return mbedtls_ssl_write(&tls->ctx, buf, buf_len);
#endif
}

int httpd_base64_encode(uint8_t *data, size_t data_len, char *base64_buf, size_t buf_len)
{
#if (HTTPD_USE_TLS == HTTPD_TLS_POLARSSL)
	int ret = 0;

	if((ret = base64_encode(base64_buf, &buf_len, data, data_len)) != 0) {
		printf("\n[HTTPD] ERROR: base64_encode %d\n", ret);
		ret = -1;
	}

	return ret;
#elif (HTTPD_USE_TLS == HTTPD_TLS_MBEDTLS)
	int ret = 0;
	size_t output_len = 0;

	if((ret = mbedtls_base64_encode(base64_buf, buf_len, &output_len, data, data_len)) != 0) {
		printf("\n[HTTPD] ERROR: mbedtls_base64_encode %d\n", ret);
		ret = -1;
	}

	return ret;
#endif
}
