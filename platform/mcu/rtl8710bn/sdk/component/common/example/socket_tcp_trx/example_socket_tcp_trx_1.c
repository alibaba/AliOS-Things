#include "FreeRTOS.h"
#include "task.h"
#include <platform/platform_stdlib.h>
#include <lwip/sockets.h>
//#include <osdep_api.h>
#include <osdep_service.h>

#define SERVER_PORT     80
#define LISTEN_QLEN     2

static int tx_exit = 0, rx_exit = 0;
//static _Sema tcp_tx_rx_sema;
static _sema tcp_tx_rx_sema;

static void tx_thread(void *param)
{
	int client_fd = * (int *) param;
	unsigned char buffer[1024];
	memset(buffer, 1, sizeof(buffer));
	printf("\n%s start\n", __FUNCTION__);

	while(1) {
		int ret = 0;

		//RtlDownSema(&tcp_tx_rx_sema);		
		rtw_down_sema(&tcp_tx_rx_sema);
		ret = send(client_fd, buffer, sizeof(buffer), 0); 
		//RtlUpSema(&tcp_tx_rx_sema);		
		rtw_up_sema(&tcp_tx_rx_sema);

		if(ret <= 0)
			goto exit;

		vTaskDelay(100);
	}

exit:
	printf("\n%s exit\n", __FUNCTION__);
	tx_exit = 1;
	vTaskDelete(NULL);
}

static void rx_thread(void *param)
{
	int client_fd = * (int *) param;
	unsigned char buffer[1024];
	printf("\n%s start\n", __FUNCTION__);

	while(1) {
		int ret = 0, sock_err = 0;
		size_t err_len = sizeof(sock_err);

		//RtlDownSema(&tcp_tx_rx_sema);		
		rtw_down_sema(&tcp_tx_rx_sema);
		ret = recv(client_fd, buffer, sizeof(buffer), MSG_DONTWAIT);
		getsockopt(client_fd, SOL_SOCKET, SO_ERROR, &sock_err, &err_len);
		//RtlUpSema(&tcp_tx_rx_sema);		
		rtw_up_sema(&tcp_tx_rx_sema);

		// ret == -1 and socket error == EAGAIN when no data received for nonblocking
		if((ret == -1) && (sock_err == EAGAIN))
			continue;
		else if(ret <= 0)
			goto exit;

		vTaskDelay(10);
	}

exit:
	printf("\n%s exit\n", __FUNCTION__);
	rx_exit = 1;
	vTaskDelete(NULL);
}

static void example_socket_tcp_trx_thread(void *param)
{
	int server_fd = -1, client_fd = -1;
	struct sockaddr_in server_addr, client_addr;
	size_t client_addr_size;

	// Delay to wait for IP by DHCP
	vTaskDelay(10000);
	printf("\nExample: socket tx/rx 1\n");

	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	if(bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
		printf("ERROR: bind\n");
		goto exit;
	}

	if(listen(server_fd, LISTEN_QLEN) != 0) {
		printf("ERROR: listen\n");
		goto exit;
	}

	while(1) {
		client_addr_size = sizeof(client_addr);
		client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_size);

		if(client_fd >= 0) {
			tx_exit = 1;
			rx_exit = 1;
			//RtlInitSema(&tcp_tx_rx_sema, 1);			
			rtw_init_sema(&tcp_tx_rx_sema, 1);

			if(xTaskCreate(tx_thread, ((const char*)"tx_thread"), 512, &client_fd, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
				printf("\n\r%s xTaskCreate(tx_thread) failed", __FUNCTION__);
			else
				tx_exit = 0;

			vTaskDelay(10);

			if(xTaskCreate(rx_thread, ((const char*)"rx_thread"), 512, &client_fd, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
				printf("\n\r%s xTaskCreate(rx_thread) failed", __FUNCTION__);
			else
				rx_exit = 0;

			while(1) {
				if(tx_exit && rx_exit) {
					close(client_fd);
					break;
				}
				else
					vTaskDelay(1000);
			}

			//RtlFreeSema(&tcp_tx_rx_sema);			
			rtw_free_sema(&tcp_tx_rx_sema);
		}
	}

exit:
	close(server_fd);
	vTaskDelete(NULL);
}

void example_socket_tcp_trx_1(void)
{
	if(xTaskCreate(example_socket_tcp_trx_thread, ((const char*)"example_socket_tcp_trx_thread"), 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
		printf("\n\r%s xTaskCreate(example_socket_tcp_trx_thread) failed", __FUNCTION__);
}
