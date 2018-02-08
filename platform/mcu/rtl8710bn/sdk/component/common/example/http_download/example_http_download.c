#include <FreeRTOS.h>
#include <task.h>
#include <platform/platform_stdlib.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#define SERVER_HOST    "176.34.62.248"
#define SERVER_PORT    80
#define RESOURCE       "/repository/IOT/Project_Cloud_A.bin"
#define BUFFER_SIZE    2048
#define RECV_TO        5000	// ms

static void example_http_download_thread(void *param)
{
	int server_fd = -1, ret;
	struct sockaddr_in server_addr;
	struct hostent *server_host;

	// Delay to wait for IP by DHCP
	vTaskDelay(10000);
	printf("\nExample: HTTP download\n");

	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("ERROR: socket\n");
		goto exit;
	}
	else {
		int recv_timeout_ms = RECV_TO;
#if defined(LWIP_SO_SNDRCVTIMEO_NONSTANDARD) && (LWIP_SO_SNDRCVTIMEO_NONSTANDARD == 0)	// lwip 1.5.0
		struct timeval recv_timeout;
		recv_timeout.tv_sec = recv_timeout_ms / 1000;
		recv_timeout.tv_usec = recv_timeout_ms % 1000 * 1000;
		setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout, sizeof(recv_timeout));
#else	// lwip 1.4.1
		setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout_ms, sizeof(recv_timeout_ms));
#endif
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);

	// Support SERVER_HOST in IP or domain name
	server_host = gethostbyname(SERVER_HOST);
	memcpy((void *) &server_addr.sin_addr, (void *) server_host->h_addr, server_host->h_length);

	if(connect(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == 0) {
		unsigned char buf[BUFFER_SIZE + 1];
		int read_size = 0, resource_size = 0, content_len = 0, header_removed = 0;

		sprintf(buf, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", RESOURCE, SERVER_HOST);
		write(server_fd, buf, strlen(buf));

		while((read_size = read(server_fd, buf, BUFFER_SIZE)) > 0) {
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
	else {
		printf("ERROR: connect\n");
	}

exit:
	if(server_fd >= 0)
		close(server_fd);

	vTaskDelete(NULL);
}

void example_http_download(void)
{
	if(xTaskCreate(example_http_download_thread, ((const char*)"example_http_download_thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate failed", __FUNCTION__);
}
