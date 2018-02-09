#include "FreeRTOS.h"
#include "task.h"
#include <platform/platform_stdlib.h>
#include <lwip/sockets.h>

#define SERVER_IP              "192.168.13.1"
#define SERVER_PORT            80

// Need to define ERRNO in lwipopts.h
int errno = 0;

static void example_nonblock_connect_thread(void *param)
{
	int server_fd = -1;
	struct sockaddr_in server_addr;

	// Delay to wait for IP by DHCP
	vTaskDelay(10000);
	printf("\nExample: Non-blocking socket connect\n");

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(server_fd, F_SETFL, fcntl(server_fd, F_GETFL, 0) | O_NONBLOCK);

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	server_addr.sin_port = htons(SERVER_PORT);
	connect(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));

	if(errno == EINPROGRESS) {
		fd_set wfds;
		struct timeval time_out;

		time_out.tv_sec = 3;	// Set select timeout of 3 seconds
		time_out.tv_usec = 0;
		FD_ZERO(&wfds) ;
		FD_SET(server_fd, &wfds);	// Only set server fd

		// Use select to wait for non-blocking connect
		if(select(server_fd + 1, NULL, &wfds, NULL, &time_out) == 1)
			printf("Server connection successful\n");
		else
			printf("Server connection failed\n");
	}
	else {
		printf("ERROR: connect\n");
	}

	close(server_fd);
	vTaskDelete(NULL);
}

void example_nonblock_connect(void)
{
	if(xTaskCreate(example_nonblock_connect_thread, ((const char*)"example_nonblock_connect_thread"), 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(init_thread) failed", __FUNCTION__);
}
