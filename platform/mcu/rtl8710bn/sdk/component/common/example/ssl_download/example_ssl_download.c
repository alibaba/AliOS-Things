#include <FreeRTOS.h>
#include <task.h>
#include <platform/platform_stdlib.h>

#include "platform_opts.h"

#if CONFIG_USE_POLARSSL

#include <lwip/sockets.h>
#include <polarssl/config.h>
#include <polarssl/memory.h>
#include <polarssl/ssl.h>

#define SERVER_HOST    "176.34.62.248"
#define SERVER_PORT    443
#define RESOURCE       "/repository/IOT/Project_Cloud_A.bin"
#define BUFFER_SIZE    2048

static int my_random(void *p_rng, unsigned char *output, size_t output_len)
{
	rtw_get_random_bytes(output, output_len);
	return 0;
}

static void example_ssl_download_thread(void *param)
{
	int server_fd = -1, ret;
	struct sockaddr_in server_addr;
	ssl_context ssl;

	// Delay to wait for IP by DHCP
	vTaskDelay(10000);
	printf("\nExample: SSL download\n");

	memory_set_own(pvPortMalloc, vPortFree);
	memset(&ssl, 0, sizeof(ssl_context));

	if((ret = net_connect(&server_fd, SERVER_HOST, SERVER_PORT)) != 0) {
		printf("ERROR: net_connect ret(%d)\n", ret);
		goto exit;
	}

	if((ret = ssl_init(&ssl)) != 0) {
		printf("ERRPR: ssl_init ret(%d)\n", ret);
		goto exit;
	}

	ssl_set_endpoint(&ssl, SSL_IS_CLIENT);
	ssl_set_authmode(&ssl, SSL_VERIFY_NONE);
	ssl_set_rng(&ssl, my_random, NULL);
	ssl_set_bio(&ssl, net_recv, &server_fd, net_send, &server_fd);

	if((ret = ssl_handshake(&ssl)) != 0) {
		printf("ERROR: ssl_handshake ret(-0x%x)", -ret);
		goto exit;
	}
	else {
		unsigned char buf[BUFFER_SIZE + 1];
		int read_size = 0, resource_size = 0, content_len = 0, header_removed = 0;

		printf("SSL ciphersuite %s\n", ssl_get_ciphersuite(&ssl));
		sprintf(buf, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", RESOURCE, SERVER_HOST);
		ssl_write(&ssl, buf, strlen(buf));

		while((read_size = ssl_read(&ssl, buf, BUFFER_SIZE)) > 0) {
			if(header_removed == 0) {
				char *header = NULL;

				buf[read_size] = 0;
				header = strstr(buf, "\r\n\r\n");

				if(header) {
					char *body, *content_len_pos;

					body = header + strlen("\r\n\r\n");
					*(body - 2) = 0;
					header_removed = 1;
					printf("\nHTTP Header: %s\n", buf);

					// Remove header size to get first read size of data from body head
					read_size = read_size - ((unsigned char *) body - buf);

					content_len_pos = strstr(buf, "Content-Length: ");
					if(content_len_pos) {
						content_len_pos += strlen("Content-Length: ");
						*(char*)(strstr(content_len_pos, "\r\n")) = 0;
						content_len = atoi(content_len_pos);
					}
				}
				else {
					printf("ERROR: HTTP header\n");
					goto exit;
				}
			}

			printf("read resource %d bytes\n", read_size);
			resource_size += read_size;
		}

		printf("exit read. ret = %d\n", read_size);
		printf("http content-length = %d bytes, download resource size = %d bytes\n", content_len, resource_size);
	}

exit:
	if(server_fd >= 0)
		net_close(server_fd);

	ssl_free(&ssl);
	vTaskDelete(NULL);
}

void example_ssl_download(void)
{
	if(xTaskCreate(example_ssl_download_thread, ((const char*)"example_ssl_download_thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(init_thread) failed", __FUNCTION__);
}

#elif CONFIG_USE_MBEDTLS /* CONFIG_USE_POLARSSL */

#include <mbedTLS/config.h>
#include <mbedTLS/platform.h>
#include <mbedtls/net_sockets.h>
#include <mbedTLS/ssl.h>

#define SERVER_HOST    "176.34.62.248"
#define SERVER_PORT    "443"
#define RESOURCE       "/repository/IOT/Project_Cloud_A.bin"
#define BUFFER_SIZE    2048

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

static void example_ssl_download_thread(void *param)
{
	int ret;
	mbedtls_net_context server_fd;
	mbedtls_ssl_context ssl;
	mbedtls_ssl_config conf;

	// Delay to wait for IP by DHCP
	vTaskDelay(10000);
	printf("\nExample: SSL download\n");

	mbedtls_platform_set_calloc_free(my_calloc, vPortFree);

	mbedtls_net_init(&server_fd);
	mbedtls_ssl_init(&ssl);
	mbedtls_ssl_config_init(&conf);

	if((ret = mbedtls_net_connect(&server_fd, SERVER_HOST, SERVER_PORT, MBEDTLS_NET_PROTO_TCP)) != 0) {
		printf("ERROR: mbedtls_net_connect ret(%d)\n", ret);
		goto exit;
	}

	mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

	if((ret = mbedtls_ssl_config_defaults(&conf,
			MBEDTLS_SSL_IS_CLIENT,
			MBEDTLS_SSL_TRANSPORT_STREAM,
			MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {

		printf("ERRPR: mbedtls_ssl_config_defaults ret(%d)\n", ret);
		goto exit;
	}

	mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);
	mbedtls_ssl_conf_rng(&conf, my_random, NULL);

	if((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
		printf("ERRPR: mbedtls_ssl_setup ret(%d)\n", ret);
		goto exit;
	}

	if((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
		printf("ERROR: mbedtls_ssl_handshake ret(-0x%x)", -ret);
		goto exit;
	}
	else {
		unsigned char buf[BUFFER_SIZE + 1];
		int read_size = 0, resource_size = 0, content_len = 0, header_removed = 0;

		printf("SSL ciphersuite %s\n", mbedtls_ssl_get_ciphersuite(&ssl));
		sprintf(buf, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", RESOURCE, SERVER_HOST);
		mbedtls_ssl_write(&ssl, buf, strlen(buf));

		while((read_size = mbedtls_ssl_read(&ssl, buf, BUFFER_SIZE)) > 0) {
			if(header_removed == 0) {
				char *header = NULL;

				buf[read_size] = 0;
				header = strstr(buf, "\r\n\r\n");

				if(header) {
					char *body, *content_len_pos;

					body = header + strlen("\r\n\r\n");
					*(body - 2) = 0;
					header_removed = 1;
					printf("\nHTTP Header: %s\n", buf);

					// Remove header size to get first read size of data from body head
					read_size = read_size - ((unsigned char *) body - buf);

					content_len_pos = strstr(buf, "Content-Length: ");
					if(content_len_pos) {
						content_len_pos += strlen("Content-Length: ");
						*(strstr(content_len_pos, "\r\n")) = 0;
						content_len = atoi(content_len_pos);
					}
				}
				else {
					printf("ERROR: HTTP header\n");
					goto exit;
				}
			}

			printf("read resource %d bytes\n", read_size);
			resource_size += read_size;
		}

		printf("exit read. ret = %d\n", read_size);
		printf("http content-length = %d bytes, download resource size = %d bytes\n", content_len, resource_size);
	}

exit:
	mbedtls_net_free(&server_fd);
	mbedtls_ssl_free(&ssl);
	mbedtls_ssl_config_free(&conf);

	vTaskDelete(NULL);
}

void example_ssl_download(void)
{
	if(xTaskCreate(example_ssl_download_thread, ((const char*)"example_ssl_download_thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(init_thread) failed", __FUNCTION__);
}

#endif /* CONFIG_USE_POLARSSL */