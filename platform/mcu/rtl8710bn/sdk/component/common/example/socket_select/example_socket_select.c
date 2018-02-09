#include "FreeRTOS.h"
#include "task.h"
#include <platform/platform_stdlib.h>
#include <lwip/sockets.h>

#define CONNECT_REMOTE  0

#if CONNECT_REMOTE
#define REMOTE_HOST     "192.168.13.14"
#define REMOTE_PORT     5000
#endif
#define MAX_SOCKETS     10
#define SELECT_TIMEOUT  10
#define SERVER_PORT     5000
#define LISTEN_QLEN     2

static void example_socket_select_thread(void *param)
{
	int max_socket_fd = -1;
#if CONNECT_REMOTE
	struct sockaddr_in remote_addr;
	int remote_fd = -1;
#endif
	struct sockaddr_in server_addr;
	int server_fd = -1;
	int socket_used[MAX_SOCKETS];

	// Delay to wait for IP by DHCP
	vTaskDelay(10000);
	printf("\nExample: socket select\n");

	memset(socket_used, 0, sizeof(socket_used));

#if CONNECT_REMOTE
reconnect:
	if((remote_fd = socket(AF_INET, SOCK_STREAM, 0)) >= 0) {
		remote_addr.sin_family = AF_INET;
		remote_addr.sin_addr.s_addr = inet_addr(REMOTE_HOST);
		remote_addr.sin_port = htons(REMOTE_PORT);

		if(connect(remote_fd, (struct sockaddr *) &remote_addr, sizeof(remote_addr)) == 0) {
			printf("connect socket fd(%d)\n", remote_fd);
			socket_used[remote_fd] = 1;

			if(remote_fd > max_socket_fd)
				max_socket_fd = remote_fd;
		}
		else {
			printf("connect error\n");
			close(remote_fd);
			goto reconnect;
		}
	}
	else {
		printf("socket error\n");
		goto exit;
	}
#endif
	if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) >= 0) {
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(SERVER_PORT);
		server_addr.sin_addr.s_addr = INADDR_ANY;

		if(bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
			printf("bind error\n");
			goto exit;
		}

		if(listen(server_fd, LISTEN_QLEN) != 0) {
			printf("listen error\n");
			goto exit;
		}

		socket_used[server_fd] = 1;

		if(server_fd > max_socket_fd)
			max_socket_fd = server_fd;
	}
	else {
		printf("socket error\n");
		goto exit;
	}

	while(1) {
		int socket_fd;
		unsigned char buf[512];
		fd_set read_fds;
		struct timeval timeout;

		FD_ZERO(&read_fds);
		timeout.tv_sec = SELECT_TIMEOUT;
		timeout.tv_usec = 0;

		for(socket_fd = 0; socket_fd < MAX_SOCKETS; socket_fd ++)
			if(socket_used[socket_fd])
				FD_SET(socket_fd, &read_fds);

		if(select(max_socket_fd + 1, &read_fds, NULL, NULL, &timeout)) {
			for(socket_fd = 0; socket_fd < MAX_SOCKETS; socket_fd ++) {
				if(socket_used[socket_fd] && FD_ISSET(socket_fd, &read_fds)) {
					if(socket_fd == server_fd) {
						struct sockaddr_in client_addr;
						unsigned int client_addr_size = sizeof(client_addr);
						int fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_size);

						if(fd >= 0) {
							printf("accept socket fd(%d)\n", fd);
							socket_used[fd] = 1;

							if(fd > max_socket_fd)
								max_socket_fd = fd;
						}
						else {
							printf("accept error\n");
						}
					}
					else {
						int read_size = read(socket_fd, buf, sizeof(buf));

						if(read_size > 0) {
							write(socket_fd, buf, read_size);
						}
						else {
							printf("socket fd(%d) disconnected\n", socket_fd);
							socket_used[socket_fd] = 0;
							close(socket_fd);
						}
					}
				}
			}
		}
		else {
			printf("TCP server: no data in %d seconds\n", SELECT_TIMEOUT);
		}

		vTaskDelay(10);
	}

exit:
	if(server_fd >= 0)
		close(server_fd);

	vTaskDelete(NULL);
}

void example_socket_select(void)
{
	if(xTaskCreate(example_socket_select_thread, ((const char*)"example_socket_select_thread"), 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(init_thread) failed", __FUNCTION__);
}
