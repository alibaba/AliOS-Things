#include "FreeRTOS.h"
#include "task.h"
#include <platform_stdlib.h>
#include <lwip_netconf.h>

#include "platform_opts.h"

#if CONFIG_USE_POLARSSL

#include "polarssl/net.h"
#include "polarssl/ssl.h"
#include "polarssl/memory.h"
#include "polarssl/certs.h"
#include "polarssl/error.h"


#define SERVER_PORT   443
#define STACKSIZE     1150
extern struct netif xnetif[NET_IF_NUM];


static int my_random(void *p_rng, unsigned char *output, size_t output_len)
{
	rtw_get_random_bytes(output, output_len);
	return 0;
}

static void example_ssl_server_thread(void *param){
#if !defined(POLARSSL_BIGNUM_C) || !defined(POLARSSL_CERTS_C) || \
	!defined(POLARSSL_SSL_TLS_C) || !defined(POLARSSL_SSL_SRV_C) || \
	!defined(POLARSSL_RSA_C) || !defined(POLARSSL_NET_C) || \
	!defined(POLARSSL_X509_CRT_PARSE_C)

	printf("POLARSSL_BIGNUM_C and/or POLARSSL_CERTS_C and/or "
		"POLARSSL_SSL_TLS_C and/or POLARSSL_SSL_SRV_C and/or "
		"POLARSSL_RSA_C and/or POLARSSL_NET_C and/or "
		"POLARSSL_X509_CRT_PARSE_C not defined.\n");

#else
	int ret, server_fd = -1, client_fd = -1;
	x509_crt server_x509;
	pk_context server_pk;
	ssl_context ssl;
	unsigned char buf[512];
	uint8_t *ip;
	char *response = "<HTML><BODY>SSL OK</BODY></HTML>";

	// Delay to wait for IP by DHCP
	vTaskDelay(10000);
	printf("\nExample: SSL server\n");

	memory_set_own(pvPortMalloc, vPortFree);
	memset(&server_x509, 0, sizeof(x509_crt));
	memset(&server_pk, 0, sizeof(pk_context));
	memset(&ssl, 0, sizeof(ssl_context));

	/*
	 * 1. Prepare the certificate and key
	 */
	printf("\n\r  . Preparing the certificate and key...");

	x509_crt_init(&server_x509);
	pk_init(&server_pk);

	if((ret = x509_crt_parse(&server_x509, (const unsigned char *)test_srv_crt, strlen(test_srv_crt))) != 0) {
		printf(" failed\n  ! x509_crt_parse returned %d\n\n", ret);
		goto exit;
	}

	if((ret = x509_crt_parse(&server_x509, (const unsigned char *)test_ca_list, strlen(test_ca_list))) != 0) {
		printf(" failed\n  ! x509_crt_parse returned %d\n\n", ret);
		goto exit;
	}

	if((ret = pk_parse_key(&server_pk, test_srv_key, strlen(test_srv_key), NULL, 0)) != 0) {
		printf(" failed\n  ! pk_parse_key returned %d\n\n", ret);
		goto exit;
	}

	printf(" ok\n");

	/*
	 * 2. Start the connection
	 */
	ip = LwIP_GetIP(&xnetif[0]);
	printf("\n\r  . Starting tcp server /%d.%d.%d.%d/%d...", ip[0], ip[1], ip[2], ip[3], SERVER_PORT);

	if((ret = net_bind(&server_fd, NULL, SERVER_PORT)) != 0) {
		printf(" failed\n  ! net_bind returned %d\n\n", ret);
		goto exit;
	}

	printf(" ok\n");

	/*
	 * 3. Waiting for client to connect
	 */
	printf("\n\r  . Waiting for client to connect...\n\r");

	while((ret = net_accept(server_fd, &client_fd, NULL)) == 0) {
		printf("\n\r  . A client is connecting\n\r");
		/*
		 * 4. Setup stuff
		 */
		printf("\n\r  . Setting up the SSL/TLS structure...");

		if((ret = ssl_init(&ssl)) != 0) {
			printf(" failed\n  ! ssl_init returned %d\n\n", ret);
			goto close_client;
		}

		ssl_set_endpoint(&ssl, SSL_IS_SERVER);
		ssl_set_ca_chain(&ssl, server_x509.next, NULL, NULL);
		ssl_set_authmode(&ssl, SSL_VERIFY_NONE);
		ssl_set_rng(&ssl, my_random, NULL);
		ssl_set_bio(&ssl, net_recv, &client_fd, net_send, &client_fd);
		if((ret = ssl_set_own_cert(&ssl, &server_x509, &server_pk)) != 0) {
			printf(" failed\n  ! ssl_set_own_cert returned %d\n\n", ret);
			goto close_client;
		}
		printf(" ok\n");

		/*
		 * 5. Handshake
		 */
		printf("\n\r  . Performing the SSL/TLS handshake...");

		if((ret = ssl_handshake(&ssl)) != 0) {
			printf(" failed\n  ! ssl_handshake returned %d\n\n", ret);
			goto close_client;
		}
		printf(" ok\n");
		printf("\n\r  . Use ciphersuite %s\n", ssl_get_ciphersuite(&ssl));

		/*
		 * 6. Read the request from client
		 */
		printf("\n\r  > Read request from client:");

		memset(buf, 0, sizeof(buf));
		if((ret = ssl_read(&ssl, buf, sizeof(buf))) <= 0) {
			if(ret != POLARSSL_ERR_NET_WANT_READ && ret != POLARSSL_ERR_NET_WANT_WRITE) {
				printf(" failed\n\r  ! ssl_read returned %d\n", ret);
				goto close_client;
			}
		}
		printf(" %d bytes read\n\r\n\r%s\n", ret, (char *) buf);

		/*
		 * 7. Response the request
		 */
		printf("\n\r  > Response to client:");

		sprintf(buf, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n%s", strlen(response), response);
		if((ret = ssl_write(&ssl, buf, strlen(buf))) <= 0) {
			if(ret != POLARSSL_ERR_NET_WANT_READ && ret != POLARSSL_ERR_NET_WANT_WRITE) {
				printf(" failed\n\r  ! ssl_write returned %d\n", ret);
				goto close_client;
			}
		}
		printf(" %d bytes written\n\r\n\r%s\n", ret, (char *)buf);

close_client:
#ifdef POLARSSL_ERROR_C
		if(ret != 0) {
			char error_buf[100];
			polarssl_strerror(ret, error_buf, 100);
			printf("\n\rLast error was: %d - %s\n", ret, error_buf);
		}
#endif
		ssl_close_notify(&ssl);
		net_close(client_fd);
		ssl_free(&ssl);
	}
	net_close(server_fd);
exit:
	x509_crt_free(&server_x509);
	pk_free(&server_pk);
#endif
	vTaskDelete(NULL);
}

void example_ssl_server(void)
{
	if(xTaskCreate(example_ssl_server_thread, "example_ssl_server_thread", STACKSIZE, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate example_ssl_server_thread failed", __FUNCTION__);
}

#elif CONFIG_USE_MBEDTLS /* CONFIG_USE_POLARSSL */

#include "mbedtls/config.h"
#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/certs.h"

#define SERVER_PORT   "443"
#define STACKSIZE     2048

extern struct netif xnetif[NET_IF_NUM];

static int my_random(void *p_rng, unsigned char *output, size_t output_len)
{
	rtw_get_random_bytes(output, output_len);
	return 0;
}

static void* my_calloc(size_t nelements, size_t elementSize)
{
	size_t size;
	void *ptr = NULL;

	size = nelements * elementSize;
	ptr = pvPortMalloc(size);

	if(ptr)
		memset(ptr, 0, size);

	return ptr;
}

static void example_ssl_server_thread(void *param)
{
#if !defined(MBEDTLS_BIGNUM_C) || !defined(MBEDTLS_CERTS_C) || \
	!defined(MBEDTLS_SSL_TLS_C) || !defined(MBEDTLS_SSL_SRV_C) || \
	!defined(MBEDTLS_RSA_C) || !defined(MBEDTLS_NET_C) || \
	!defined(MBEDTLS_PEM_PARSE_C) || !defined(MBEDTLS_X509_CRT_PARSE_C)

	printf("MBEDTLS_BIGNUM_C and/or MBEDTLS_CERTS_C and/or "
		"MBEDTLS_SSL_TLS_C and/or MBEDTLS_SSL_SRV_C and/or "
		"MBEDTLS_RSA_C and/or MBEDTLS_NET_C and/or "
		"MBEDTLS_PEM_PARSE_C and/or MBEDTLS_X509_CRT_PARSE_C not defined.\n");

#else
	int ret;
	unsigned char buf[512];
	uint8_t *ip;
	mbedtls_net_context server_fd, client_fd;
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config conf;
	mbedtls_x509_crt server_x509;
	mbedtls_pk_context server_pk;

	char *response = "<HTML><BODY>TLS OK</BODY></HTML>";

	// Delay to wait for IP by DHCP
	vTaskDelay(10000);
	printf("\nExample: SSL server\n");

	mbedtls_platform_set_calloc_free(my_calloc, vPortFree);

	/*
	 * 1. Prepare the certificate and key
	 */
	printf("\n\r  . Preparing the certificate and key...");

	mbedtls_x509_crt_init(&server_x509);
	mbedtls_pk_init(&server_pk);

	if((ret = mbedtls_x509_crt_parse(&server_x509, (const unsigned char *) mbedtls_test_srv_crt, mbedtls_test_srv_crt_len)) != 0) {
		printf(" failed\n  ! mbedtls_x509_crt_parse returned %d\n\n", ret);
		goto exit;
	}

	if((ret = mbedtls_x509_crt_parse(&server_x509, (const unsigned char *) mbedtls_test_cas_pem, mbedtls_test_cas_pem_len)) != 0) {
		printf(" failed\n  ! mbedtls_x509_crt_parse returned %d\n\n", ret);
		goto exit;
	}

	if((ret = mbedtls_pk_parse_key(&server_pk, (const unsigned char *) mbedtls_test_srv_key, mbedtls_test_srv_key_len, NULL, 0)) != 0) {
		printf(" failed\n  ! mbedtls_pk_parse_key returned %d\n\n", ret);
		goto exit;
	}

	printf(" ok\n");

	/*
	 * 2. Start the connection
	 */
	ip = LwIP_GetIP(&xnetif[0]);
	printf("\n\r  . Starting tcp server /%d.%d.%d.%d/%s...", ip[0], ip[1], ip[2], ip[3], SERVER_PORT);
	mbedtls_net_init(&server_fd);

	if((ret = mbedtls_net_bind(&server_fd, NULL, SERVER_PORT, MBEDTLS_NET_PROTO_TCP)) != 0) {
		printf(" failed\n  ! mbedtls_net_bind returned %d\n\n", ret);
		goto exit;
	}

	printf(" ok\n");

	/*
	 * 3. Waiting for client to connect
	 */
	printf("\n\r  . Waiting for client to connect...\n\r");
	mbedtls_net_init(&client_fd);

	while((ret = mbedtls_net_accept(&server_fd, &client_fd, NULL, 0, NULL)) == 0) {
		printf("\n\r  . A client is connecting\n\r");
		/*
		 * 4. Setup stuff
		 */
		printf("\n\r  . Setting up the SSL/TLS structure...");
		mbedtls_ssl_init(&ssl);
		mbedtls_ssl_config_init(&conf);

		if((ret = mbedtls_ssl_config_defaults(&conf,
				MBEDTLS_SSL_IS_SERVER,
				MBEDTLS_SSL_TRANSPORT_STREAM,
				MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {

			printf(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret);
			goto close_client;
		}

		mbedtls_ssl_conf_ca_chain(&conf, server_x509.next, NULL);
		mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);
		mbedtls_ssl_conf_rng(&conf, my_random, NULL);

		if((ret = mbedtls_ssl_conf_own_cert(&conf, &server_x509, &server_pk)) != 0) {
			printf(" failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n\n", ret);
			goto close_client;
		}

		if((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
			printf(" failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret);
			goto close_client;
		}

		mbedtls_ssl_set_bio(&ssl, &client_fd, mbedtls_net_send, mbedtls_net_recv, NULL);
		printf(" ok\n");

		/*
		 * 5. Handshake
		 */
		printf("\n\r  . Performing the SSL/TLS handshake...");

		if((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
			printf(" failed\n  ! mbedtls_ssl_handshake returned %d\n\n", ret);
			goto close_client;
		}
		printf(" ok\n");
		printf("\n\r  . Use ciphersuite %s\n", mbedtls_ssl_get_ciphersuite(&ssl));

		/*
		 * 6. Read the request from client
		 */
		printf("\n\r  > Read request from client:");

		memset(buf, 0, sizeof(buf));
		if((ret = mbedtls_ssl_read(&ssl, buf, sizeof(buf))) <= 0) {
			if(ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
				printf(" failed\n\r  ! mbedtls_ssl_read returned %d\n", ret);
				goto close_client;
			}
		}
		printf(" %d bytes read\n\r\n\r%s\n", ret, (char *) buf);

		/*
		 * 7. Response the request
		 */
		printf("\n\r  > Response to client:");

		sprintf(buf, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n%s", strlen(response), response);
		if((ret = mbedtls_ssl_write(&ssl, buf, strlen(buf))) <= 0) {
			if(ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
				printf(" failed\n\r  ! mbedtls_ssl_write returned %d\n", ret);
				goto close_client;
			}
		}
		printf(" %d bytes written\n\r\n\r%s\n", ret, (char *)buf);

close_client:
		mbedtls_ssl_close_notify(&ssl);
		mbedtls_net_free(&client_fd);
		mbedtls_ssl_free(&ssl);
		mbedtls_ssl_config_free(&conf);
	}

	mbedtls_net_free(&server_fd);

exit:
	mbedtls_x509_crt_free(&server_x509);
	mbedtls_pk_free(&server_pk);
#endif
	vTaskDelete(NULL);
}

void example_ssl_server(void)
{
	if(xTaskCreate(example_ssl_server_thread, "example_ssl_server_thread", STACKSIZE, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate example_ssl_server_thread failed", __FUNCTION__);
}

#endif /* CONFIG_USE_POLARSSL */