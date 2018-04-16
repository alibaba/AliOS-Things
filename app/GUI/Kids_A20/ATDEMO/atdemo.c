#include <stdio.h>
#include "stm32l4xx_hal.h"
#include "atdemo.h"
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern uart_dev_t brd_uart3_dev;
//extern uart_dev_t uart_0;

//#define os_uart uart_0
#define wifi_uart brd_uart3_dev
static aos_mutex_t at_mutex;

static int atcmd_init_mutex()
{
    if (0 != aos_mutex_new(&at_mutex)) {
        LOGE("atdemo", "Creating mutex failed");
        return HAL_ERROR;
    }

    return HAL_OK;
}

static int at_test(enum at_cmd_e id, char *PInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	int32_t ret_val = HAL_OK;
	uint32_t recv_size = 0, recv_size_t = 0;
	char Recv_ch;
	uint8_t time_out = 0, respone_start = 0;

	memset(pOutBuffer, 0, OutLength);
	hal_uart_send(&wifi_uart, (void *)"AT+TEST", strlen("AT+TEST"), 30000);
	ret_val = hal_uart_send(&wifi_uart, (void *)"\r", 1, 30000);
	if(ret_val != HAL_OK)
		return HAL_ERROR;
	while(1){
		do {
			ret_val = hal_uart_recv(&wifi_uart, (void *)&Recv_ch, 1, &recv_size, 3000);
			if(ret_val != HAL_OK){
				time_out++;
				if(time_out >= 10)
					return HAL_ERROR;
				krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND/10);
			}
		}while(recv_size != 1);
		time_out = 0;
		*(pOutBuffer + recv_size_t) = Recv_ch;
		recv_size_t++;
		if(recv_size_t >= 4){
			if(strstr(pOutBuffer, "AT+TEST\r\n")   && (respone_start == 0) ){
				respone_start = 1;
				recv_size_t = 0;
				memset(pOutBuffer, 0, OutLength);
			}
			else if(respone_start){
				if(strstr(pOutBuffer, "ERROR\r\n") || strstr(pOutBuffer, "OK\r\n"))
					break;
			}
		}
		if(recv_size_t >= OutLength){
			ret_val = HAL_ERROR;
			break;
		}
	}
	return ret_val;
}

static int handle_at(enum at_cmd_e id, char *PInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	int32_t ret_val = HAL_OK;
	uint32_t recv_size = 0, recv_size_t = 0;
	char Recv_ch, end_ch, ret_ch;
	uint8_t time_out = 0, respone_start = 0;

	memset(pOutBuffer, 0, OutLength);
	hal_uart_send(&wifi_uart, (void *)"AT", strlen("AT"), 3000);
	ret_val = hal_uart_send(&wifi_uart, (void *)"\r", 1, 3000);
	if(ret_val != HAL_OK)
		return HAL_ERROR;
	while(1){
		do {
			ret_val = hal_uart_recv(&wifi_uart, (void *)&Recv_ch, 1, &recv_size, 1000);
			if(ret_val != HAL_OK){
				time_out++;
				if(time_out >= 10)
					return HAL_ERROR;
				krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND/100);
			}
		}while(recv_size != 1);
		time_out = 0;
		*(pOutBuffer + recv_size_t) = Recv_ch;
		recv_size_t++;
		if(recv_size_t >= 4){
			if(strstr(pOutBuffer, "AT\r\n")   && (respone_start == 0) ){
				respone_start = 1;
				recv_size_t = 0;
				memset(pOutBuffer, 0, OutLength);
			}
			else if(respone_start){
				if(strstr(pOutBuffer, "ERROR\r\n") || strstr(pOutBuffer, "OK\r\n"))
					break;
			}
		}
		if(recv_size_t >= OutLength){
			ret_val = HAL_ERROR;
			break;
		}
	}
	return ret_val;
}

static int at_get_at_verion(enum at_cmd_e id, char *PInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	int32_t ret_val = HAL_OK;
	uint32_t recv_size = 0, recv_size_t = 0;
	char Recv_ch, ret_ch, end_ch;
	uint8_t time_out = 0, respone_start = 0;

	memset(pOutBuffer, 0, OutLength);
	hal_uart_send(&wifi_uart, (void *)"AT+GETATVERSION", strlen("AT+GETATVERSION"), 30000);
	ret_val = hal_uart_send(&wifi_uart, (void *)"\r", 1, 30000);
	if(ret_val != HAL_OK)
		return HAL_ERROR;
	while(1){
		do {
			ret_val = hal_uart_recv(&wifi_uart, (void *)&Recv_ch, 1, &recv_size, 3000);
			if(ret_val != HAL_OK){
				time_out++;
				if(time_out >= 10)
					return HAL_ERROR;
				krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND/10);
			}
		}while(recv_size != 1);
		time_out = 0;
		*(pOutBuffer + recv_size_t) = Recv_ch;
		recv_size_t++;
		if(recv_size_t >= 4){
			if(strstr(pOutBuffer, "AT+GETATVERSION\r\n")   && (respone_start == 0) ){
				respone_start = 1;
				recv_size_t = 0;
				memset(pOutBuffer, 0, OutLength);
			}
			else if(respone_start){
				ret_ch = *(pOutBuffer + recv_size_t - 2);
				end_ch = *(pOutBuffer + recv_size_t - 1);
				if((ret_ch == '\r') && (end_ch == '\n')){
					if(strstr(pOutBuffer, "AT_VERSION") )
					break;
				}
			}
			
		}
		if(recv_size_t >= OutLength){
			ret_val = HAL_ERROR;
			break;
		}
	}
	return ret_val;
}

static int at_version(enum at_cmd_e id, char *PInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	int32_t ret_val = HAL_OK;
	uint32_t recv_size = 0, recv_size_t = 0;
	char Recv_ch, end_ch, ret_ch;
	uint8_t time_out = 0, respone_start = 0;

	memset(pOutBuffer, 0, OutLength);
	hal_uart_send(&wifi_uart, (void *)"AT+FWVER?", strlen("AT+FWVER?"), 30000);
	ret_val = hal_uart_send(&wifi_uart, (void *)"\r", 1, 30000);
	if(ret_val != HAL_OK)
		return HAL_ERROR;
	while(1){
		do {
			ret_val = hal_uart_recv(&wifi_uart, (void *)&Recv_ch, 1, &recv_size, 3000);
			if(ret_val != HAL_OK){
				time_out++;
				if(time_out >= 10)
					return HAL_ERROR;
				krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND/10);
			}
		}while(recv_size != 1);
		time_out = 0;
		*(pOutBuffer + recv_size_t) = Recv_ch;
		recv_size_t++;
		if(recv_size_t >= 4){
			if(strstr(pOutBuffer, "AT+FWVER?\r\n")   && (respone_start == 0) ){
				respone_start = 1;
				recv_size_t = 0;
				memset(pOutBuffer, 0, OutLength);
			}
			else if(respone_start){
				if(strstr(pOutBuffer, "ERROR\r\n") || strstr(pOutBuffer, "OK\r\n"))
					break;
			}
		}
		if(recv_size_t >= OutLength){
			ret_val = HAL_ERROR;
			break;
		}
	}
	return ret_val;
}

static int at_recover_factory_config(enum at_cmd_e id, char *PInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	int32_t ret_val = HAL_OK;
	uint32_t recv_size = 0, recv_size_t = 0;
	char Recv_ch, end_ch, ret_ch;
	uint8_t time_out = 0, respone_start = 0;

	memset(pOutBuffer, 0, OutLength);
	hal_uart_send(&wifi_uart, (void *)"AT+FACTORY", strlen("AT+FACTORY"), 30000);
	ret_val = hal_uart_send(&wifi_uart, "\r", 1, 30000);
	if(ret_val != HAL_OK)
		return HAL_ERROR;
	while(1){
		do {
			ret_val = hal_uart_recv(&wifi_uart, &Recv_ch, 1, &recv_size, 30000);
			if(ret_val != HAL_OK){
				time_out++;
				//printf("time_out = %d\r\n", time_out);
				if(time_out >= 10)
					return HAL_ERROR;
				krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
			}
		}while(recv_size != 1);
		time_out = 0;
		*(pOutBuffer + recv_size_t) = Recv_ch;
		recv_size_t++;
		if(recv_size_t >= 4){
			if(strstr(pOutBuffer, "AT+FACTORY\r\n")   && (respone_start == 0) ){
				respone_start = 1;
				recv_size_t = 0;
				memset(pOutBuffer, 0, OutLength);
			}
			else if(respone_start){
				if(strstr(pOutBuffer, "ERROR\r\n") || strstr(pOutBuffer, "OK\r\n"))
					break;
			}
		}
		if(recv_size_t >= OutLength){
			ret_val = HAL_ERROR;
			break;
		}
	}
	return ret_val;
}

static int at_set_flash_lock(enum at_cmd_e id, char *PInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	int32_t ret_val = HAL_OK;
	uint32_t recv_size = 0, recv_size_t = 0;
	char send_cmd[128] = "AT+FLASHLOCK=";
	char Recv_ch, end_ch, ret_ch;
	uint8_t time_out = 0;

	if(OutLength < 200)
		return HAL_ERROR;

	strcat(send_cmd, PInBuffer);
	memset(pOutBuffer, 0, OutLength);
	ret_val = hal_uart_send(&wifi_uart, send_cmd, strlen(send_cmd), 30000);
	ret_val = hal_uart_send(&wifi_uart, "\r", 1, 30000);
	if(ret_val != HAL_OK)
		return HAL_ERROR;
	while(1){
		do {
			ret_val = hal_uart_recv(&wifi_uart, (void *)&Recv_ch, 1, &recv_size, 30000);
			if(ret_val != HAL_OK){
				time_out++;
				//printf("xiehj : time_out = %d\n", time_out);
				if(time_out >= 10)
					return HAL_ERROR;
				krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
			}
		}while(recv_size != 1);
		
		*(pOutBuffer + recv_size_t) = Recv_ch;
		recv_size_t++;
		if(recv_size_t >= 4){
			ret_ch = *(pOutBuffer + recv_size_t - 2);
			end_ch = *(pOutBuffer + recv_size_t - 1);
			if((ret_ch == '\r') && (end_ch == '\n')){
				if(strstr(pOutBuffer, "\r\nERROR\r\n") || strstr(pOutBuffer, "OK\r\n"))
						break;
			}
		}
		if(recv_size_t >= OutLength){
			ret_val = HAL_ERROR;
			break;
		}
	}
	return ret_val;
}

static int at_refer_wifi_event_notification(enum at_cmd_e id, char *PInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	int32_t ret_val = HAL_OK;
	uint32_t recv_size = 0, recv_size_t = 0;
	char send_cmd[128] = "AT+WEVENT=";
	char Recv_ch, end_ch, ret_ch;
	uint8_t time_out = 0;

	if(OutLength < 200)
		return HAL_ERROR;

	strcat(send_cmd, PInBuffer);
	memset(pOutBuffer, 0, OutLength);
	ret_val = hal_uart_send(&wifi_uart, send_cmd, strlen(send_cmd), 30000);
	ret_val = hal_uart_send(&wifi_uart, "\r", 1, 30000);
	if(ret_val != HAL_OK)
		return HAL_ERROR;
	while(1){
		do {
			ret_val = hal_uart_recv(&wifi_uart, (void *)&Recv_ch, 1, &recv_size, 30000);
			if(ret_val != HAL_OK){
				time_out++;
				if(time_out >= 10)
					return HAL_ERROR;
				krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
			}
		}while(recv_size != 1);
		
		*(pOutBuffer + recv_size_t) = Recv_ch;
		recv_size_t++;
		if(recv_size_t >= 4){
			ret_ch = *(pOutBuffer + recv_size_t - 2);
			end_ch = *(pOutBuffer + recv_size_t - 1);
			if((ret_ch == '\r') && (end_ch == '\n')){
				if(strstr(pOutBuffer, "\r\nERROR\r\n") || strstr(pOutBuffer, "OK\r\n"))
						break;
			}
		}
		if(recv_size_t >= OutLength){
			ret_val = HAL_ERROR;
			break;
		}
	}
	return ret_val;
}



static int at_set_wifi_scan_option(enum at_cmd_e id, char *PInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	int32_t ret_val = HAL_OK;
	uint32_t recv_size = 0, recv_size_t = 0;
	char send_cmd[128] = "AT+WSCANOPT=";
	char Recv_ch, end_ch, ret_ch;
	uint8_t time_out = 0;

	if(OutLength < 200)
		return HAL_ERROR;

	strcat(send_cmd, PInBuffer);
	memset(pOutBuffer, 0, OutLength);
	ret_val = hal_uart_send(&wifi_uart, send_cmd, strlen(send_cmd), 30000);
	ret_val = hal_uart_send(&wifi_uart, "\r", 1, 30000);
	if(ret_val != HAL_OK)
		return HAL_ERROR;
	while(1){
		do {
			ret_val = hal_uart_recv(&wifi_uart, (void *)&Recv_ch, 1, &recv_size, 30000);
			if(ret_val != HAL_OK){
				time_out++;
				if(time_out >= 10)
					return HAL_ERROR;
				krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
			}
		}while(recv_size != 1);
		
		*(pOutBuffer + recv_size_t) = Recv_ch;
		recv_size_t++;
		if(recv_size_t >= 4){
			ret_ch = *(pOutBuffer + recv_size_t - 2);
			end_ch = *(pOutBuffer + recv_size_t - 1);
			if((ret_ch == '\r') && (end_ch == '\n')){
				if(strstr(pOutBuffer, "\r\nERROR\r\n") || strstr(pOutBuffer, "OK\r\n"))
						break;
			}
		}
		if(recv_size_t >= OutLength){
			ret_val = HAL_ERROR;
			break;
		}
	}
	return ret_val;
}

static int at_set_uart_echo(enum at_cmd_e id, char *PInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	int32_t ret_val = HAL_OK;
	uint32_t recv_size = 0, recv_size_t = 0;
	char send_cmd[128] = "AT+UARTE=";
	char Recv_ch, end_ch, ret_ch;
	uint8_t time_out = 0;

	if(OutLength < 200)
		return HAL_ERROR;

	strcat(send_cmd, PInBuffer);
	memset(pOutBuffer, 0, OutLength);
	ret_val = hal_uart_send(&wifi_uart, send_cmd, strlen(send_cmd), 30000);
	ret_val = hal_uart_send(&wifi_uart, "\r", 1, 30000);
	if(ret_val != HAL_OK)
		return HAL_ERROR;
	while(1){
		do {
			ret_val = hal_uart_recv(&wifi_uart, (void *)&Recv_ch, 1, &recv_size, 30000);
			if(ret_val != HAL_OK){
				time_out++;
				if(time_out >= 10)
					return HAL_ERROR;
				krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
			}
		}while(recv_size != 1);
		
		*(pOutBuffer + recv_size_t) = Recv_ch;
		recv_size_t++;
		if(recv_size_t >= 4){
			ret_ch = *(pOutBuffer + recv_size_t - 2);
			end_ch = *(pOutBuffer + recv_size_t - 1);
			if((ret_ch == '\r') && (end_ch == '\n')){
				if(strstr(pOutBuffer, "\r\nERROR\r\n") || strstr(pOutBuffer, "OK\r\n"))
						break;
			}
		}
		if(recv_size_t >= OutLength){
			ret_val = HAL_ERROR;
			break;
		}
	}
	return ret_val;
}

static int at_set_uart_fomat(enum at_cmd_e id, char *PInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	int32_t ret_val = HAL_OK;
	uint32_t recv_size = 0, recv_size_t = 0;
	char send_cmd[128] = "AT+UARTFOMAT=";
	char Recv_ch, end_ch, ret_ch;
	uint8_t time_out = 0;

	if(OutLength < 200)
		return HAL_ERROR;

	strcat(send_cmd, PInBuffer);
	memset(pOutBuffer, 0, OutLength);
	ret_val = hal_uart_send(&wifi_uart, send_cmd, strlen(send_cmd), 30000);
	ret_val = hal_uart_send(&wifi_uart, "\r", 1, 30000);
	if(ret_val != HAL_OK)
		return HAL_ERROR;
	while(1){
		do {
			ret_val = hal_uart_recv(&wifi_uart, (void *)&Recv_ch, 1, &recv_size, 30000);
			if(ret_val != HAL_OK){
				time_out++;
				if(time_out >= 10)
					return HAL_ERROR;
				krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
			}
		}while(recv_size != 1);
		
		*(pOutBuffer + recv_size_t) = Recv_ch;
		recv_size_t++;
		if(recv_size_t >= 4){
			ret_ch = *(pOutBuffer + recv_size_t - 2);
			end_ch = *(pOutBuffer + recv_size_t - 1);
			if((ret_ch == '\r') && (end_ch == '\n')){
				if(strstr(pOutBuffer, "\r\nERROR\r\n") || strstr(pOutBuffer, "OK\r\n"))
						break;
			}
		}
		if(recv_size_t >= OutLength){
			ret_val = HAL_ERROR;
			break;
		}
	}
	return ret_val;
}

static int at_set_uart_info(enum at_cmd_e id, char *PInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	int32_t ret_val = HAL_OK;
	uint32_t recv_size = 0, recv_size_t = 0;
	char send_cmd[128] = "AT+UART=";
	char Recv_ch, end_ch, ret_ch;
	uint8_t time_out = 0;

	if(OutLength < 200)
		return HAL_ERROR;

	strcat(send_cmd, PInBuffer);
	memset(pOutBuffer, 0, OutLength);
	ret_val = hal_uart_send(&wifi_uart, send_cmd, strlen(send_cmd), 30000);
	ret_val = hal_uart_send(&wifi_uart, "\r", 1, 30000);
	if(ret_val != HAL_OK)
		return HAL_ERROR;
	while(1){
		do {
			ret_val = hal_uart_recv(&wifi_uart, (void *)&Recv_ch, 1, &recv_size, 30000);
			if(ret_val != HAL_OK){
				time_out++;
				if(time_out >= 10)
					return HAL_ERROR;
				krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
			}
		}while(recv_size != 1);
		
		*(pOutBuffer + recv_size_t) = Recv_ch;
		recv_size_t++;
		if(recv_size_t >= 4){
			ret_ch = *(pOutBuffer + recv_size_t - 2);
			end_ch = *(pOutBuffer + recv_size_t - 1);
			if((ret_ch == '\r') && (end_ch == '\n')){
				if(strstr(pOutBuffer, "\r\nERROR\r\n") || strstr(pOutBuffer, "OK\r\n"))
						break;
			}
		}
		if(recv_size_t >= OutLength){
			ret_val = HAL_ERROR;
			break;
		}
	}
	return ret_val;
}

static int at_set_wifi_power_save(enum at_cmd_e id, char *PInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	int32_t ret_val = HAL_OK;
	uint32_t recv_size = 0, recv_size_t = 0;
	char send_cmd[128] = "AT+WLPC=";
	char Recv_ch, end_ch, ret_ch;
	uint8_t time_out = 0;

	if(OutLength < 200)
		return HAL_ERROR;

	strcat(send_cmd, PInBuffer);
	memset(pOutBuffer, 0, OutLength);
	ret_val = hal_uart_send(&wifi_uart, send_cmd, strlen(send_cmd), 30000);
	ret_val = hal_uart_send(&wifi_uart, "\r", 1, 30000);
	if(ret_val != HAL_OK)
		return HAL_ERROR;
	while(1){
		do {
			ret_val = hal_uart_recv(&wifi_uart, (void *)&Recv_ch, 1, &recv_size, 30000);
			if(ret_val != HAL_OK){
				time_out++;
				if(time_out >= 10)
					return HAL_ERROR;
				krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
			}
		}while(recv_size != 1);
		
		*(pOutBuffer + recv_size_t) = Recv_ch;
		recv_size_t++;
		if(recv_size_t >= 4){
			ret_ch = *(pOutBuffer + recv_size_t - 2);
			end_ch = *(pOutBuffer + recv_size_t - 1);
			if((ret_ch == '\r') && (end_ch == '\n')){
				if(strstr(pOutBuffer, "\r\nERROR\r\n") || strstr(pOutBuffer, "OK\r\n"))
						break;
			}
		}
		if(recv_size_t >= OutLength){
			ret_val = HAL_ERROR;
			break;
		}
	}
	return ret_val;
}

static int at_wl_scan_func(enum at_cmd_e id, char *PInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	int32_t ret_val = HAL_OK;
	uint32_t recv_size = 0, recv_size_t = 0, recv_size_remain_t =0;
	char send_cmd[128] = "AT+WSCAN";
	char Recv_ch, end_ch, ret_ch;
	uint8_t time_out = 0, respone_start = 0;;
	
	memset(pOutBuffer, 0, OutLength);
	ret_val = hal_uart_send(&wifi_uart, send_cmd, strlen(send_cmd), 30000);
	ret_val = hal_uart_send(&wifi_uart, "\r", 1, 30000);
	strcat(send_cmd, "\r\n");
	if(ret_val != HAL_OK)
		return HAL_ERROR;
	while(1){
		do {
			ret_val = hal_uart_recv(&wifi_uart, &Recv_ch, 1, &recv_size, 30000);
			if(ret_val != HAL_OK){
				time_out++;
				if(time_out >= 20){
					return HAL_ERROR;
				}
				krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
			}
		}while(recv_size != 1);
		time_out = 0;
		*(pOutBuffer + recv_size_t) = Recv_ch;
		recv_size_t++;
		time_out = 0;
		if(recv_size_t >= 4){
			ret_ch = *(pOutBuffer + recv_size_t - 2);
			end_ch = *(pOutBuffer + recv_size_t - 1);
			if((ret_ch == '\r') && (end_ch == '\n')){
				if(strstr(pOutBuffer, send_cmd)   && (respone_start == 0) ){
					respone_start = 1;
					recv_size_t = 0;
					memset(pOutBuffer, 0, OutLength);
				}
				else if(respone_start){
					if(strstr(pOutBuffer + recv_size_remain_t, "ERROR\r\n") 
					|| strstr(pOutBuffer + recv_size_remain_t, "OK\r\n"))
						break;
					recv_size_remain_t = recv_size_t;
				}
			}
		}
		if(recv_size_t >= OutLength){
			ret_val = HAL_ERROR;
			break;
		}
	}
	return ret_val;
}

static int at_set_wifi_sta_info_start(enum at_cmd_e id, char *PInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	int32_t ret_val = HAL_OK;
	uint32_t recv_size = 0, recv_size_t = 0;
	char send_cmd[128] = "AT+WJAP=";
	char Recv_ch, end_ch, ret_ch;
	uint8_t time_out = 0, respone_start = 0;

	if(OutLength < 200)
		return HAL_ERROR;

	strcat(send_cmd, PInBuffer);
	memset(pOutBuffer, 0, OutLength);
	ret_val = hal_uart_send(&wifi_uart, send_cmd, strlen(send_cmd), 30000);
	ret_val = hal_uart_send(&wifi_uart, "\r", 1, 30000);
	strcat(send_cmd, "\r\n");
	if(ret_val != HAL_OK)
		return HAL_ERROR;
	while(1){
		do {
			ret_val = hal_uart_recv(&wifi_uart, (void *)&Recv_ch, 1, &recv_size, 30000);
			if(ret_val != HAL_OK){
				time_out++;
				//printf("xiehj : time_out = %d\n", time_out);
				if(time_out >= 10)
					return HAL_ERROR;
				krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND);
			}
		}while(recv_size != 1);
		time_out = 0;
		*(pOutBuffer + recv_size_t) = Recv_ch;
		recv_size_t++;
		if(recv_size_t >= 4){
			if(strstr(pOutBuffer, send_cmd)   && (respone_start == 0) ){
				respone_start = 1;
				recv_size_t = 0;
				memset(pOutBuffer, 0, OutLength);
			}
			else if(respone_start){
				if(strstr(pOutBuffer, "ERROR\r\n") || strstr(pOutBuffer, "OK\r\n"))
					break;
			}
		}
		if(recv_size_t >= OutLength){
			ret_val = HAL_ERROR;
			break;
		}
	}
	return ret_val;
}

static int at_set_common_func(enum at_cmd_e id, char *PInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	int32_t ret_val = HAL_OK;
	uint32_t recv_size = 0, recv_size_t = 0;
	char send_cmd[128] = {0};
	char Recv_ch, end_ch, ret_ch;
	uint8_t time_out = 0, respone_start = 0;

	if(OutLength < 200)
		return HAL_ERROR;

	switch(id){
		case AT_CMD_AT_FLASHLOCK_SET:
			strcat(send_cmd, "AT+FLASHLOCK=");
			break;
		case AT_CMD_AT_WEVENT_SET:
			strcat(send_cmd, "AT+WEVENT=");
			break;
		case AT_CMD_AT_WLPC_SET:
			strcat(send_cmd, "AT+WLPC=");
			break;
		case AT_CMD_AT_UART_SET:
			strcat(send_cmd, "AT+UART=");
			break;
		case AT_CMD_AT_UARTFOMAT_SET:
			strcat(send_cmd, "AT+UARTFOMAT=");
			break;
		case AT_CMD_AT_UARTE_SET:
			strcat(send_cmd, "AT+UARTE=");
			break;
		case AT_CMD_AT_WSCANOPT_SET:
			strcat(send_cmd, "AT+WSCANOPT=");
			break;
		case AT_CMD_AT_WDHCP_SET:
			strcat(send_cmd, "AT+WDHCP=");
			break;
		case AT_CMD_AT_WSAPIP_SET:
			strcat(send_cmd, "AT+WSAPIP=");
			break;
		case AT_CMD_AT_WSAP_SET:
			strcat(send_cmd, "AT+WSAP=");
			break;
		case AT_CMD_AT_WJAPIP_SET:
			strcat(send_cmd, "AT+WJAPIP=");
			break;
		#if 0
		case AT_CMD_AT_SSLCERT_SET:
			strcat(send_cmd, "AT+SSLCERTSET=");
			break;
		case AT_CMD_AT_SSLCERT_GET:
			strcat(send_cmd, "AT+SSLCERTGET=");
			break;
		#endif
		case AT_CMD_AT_CIPRECVCFG_SET:
			strcat(send_cmd, "AT+CIPRECVCFG=");
			break;
		
	}

	strcat(send_cmd, PInBuffer);
	memset(pOutBuffer, 0, OutLength);
	ret_val = hal_uart_send(&wifi_uart, send_cmd, strlen(send_cmd), 30000);
	ret_val = hal_uart_send(&wifi_uart, "\r", 1, 30000);
	strcat(send_cmd, "\r\n");
	if(ret_val != HAL_OK)
		return HAL_ERROR;
	while(1){
		do {
			ret_val = hal_uart_recv(&wifi_uart, (void *)&Recv_ch, 1, &recv_size, 30000);
			if(ret_val != HAL_OK){
				time_out++;
				if(time_out >= 10)
					return HAL_ERROR;
				krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND/10);
			}
		}while(recv_size != 1);
		
		*(pOutBuffer + recv_size_t) = Recv_ch;
		recv_size_t++;
		if(recv_size_t >= 4){
			if(strstr(pOutBuffer, send_cmd)   && (respone_start == 0) ){
				respone_start = 1;
				recv_size_t = 0;
				memset(pOutBuffer, 0, OutLength);
			}
			else if(respone_start){
				if(strstr(pOutBuffer, "ERROR\r\n") || strstr(pOutBuffer, "OK\r\n"))
					break;
			}
		}
		if(recv_size_t >= OutLength){
			ret_val = HAL_ERROR;
			break;
		}
	}
	return ret_val;
}

static int at_get_common_func(enum at_cmd_e id, char *PInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	int32_t ret_val = HAL_OK;
	uint32_t recv_size = 0, recv_size_t = 0;
	char send_cmd[50] = {0};
	char Recv_ch;
	uint8_t time_out = 0, respone_start = 0;

	switch(id){
		case AT_CMD_AT_TEST:
			strcat(send_cmd, "AT+TEST");
			break;
		case AT_CMD_AT:
			strcat(send_cmd, "AT");
			break;
		case AT_CMD_AT_FWVER:
			strcat(send_cmd, "AT+FWVER?");
			break;
		case AT_CMD_AT_SYSTIME:
			strcat(send_cmd, "AT+SYSTIME?");
			break;
		case AT_CMD_AT_MEMFREE:
			strcat(send_cmd, "AT+MEMFREE?");
			break;
		case AT_CMD_AT_REBOOT:
			strcat(send_cmd, "AT+REBOOT");
			break;
		case AT_CMD_AT_FACTORY:
			strcat(send_cmd, "AT+FACTORY");
			break;
		case AT_CMD_AT_FLASHLOCK_GET:
			strcat(send_cmd, "AT+FLASHLOCK?");
			break;
		case AT_CMD_AT_WEVENT_GET:
			strcat(send_cmd, "AT+WEVENT?");
			break;
		case AT_CMD_AT_WLPC_GET:
			strcat(send_cmd, "AT+WLPC?");
			break;
		case AT_CMD_AT_UART_GET:
			strcat(send_cmd, "AT+UART?");
			break;
		case AT_CMD_AT_UARTFOMAT_GET:
			strcat(send_cmd, "AT+UARTFOMAT?");
			break;
		case AT_CMD_AT_UARTE_GET:
			strcat(send_cmd, "AT+UARTE?");
			break;
		case AT_CMD_AT_WFVER:
			strcat(send_cmd, "AT+WFVER");
			break;
		case AT_CMD_AT_WMAC:
			strcat(send_cmd, "AT+WMAC?");
			break;
		case AT_CMD_AT_WSCANOPT_GET:
			strcat(send_cmd, "AT+WSCANOPT?");
			break;
		case AT_CMD_AT_WDHCP_GET:
			strcat(send_cmd, "AT+WDHCP?");
			break;
		case AT_CMD_AT_WSAPIP_GET:
			strcat(send_cmd, "AT+WSAPIP?");
			break;
		case AT_CMD_AT_WSAP_GET:
			strcat(send_cmd, "AT+WSAP?");
			break;
		case AT_CMD_AT_WSAPQ:
			strcat(send_cmd, "AT+WSAPQ");
			break;
		case AT_CMD_AT_WSAPS:
			strcat(send_cmd, "AT+WSAPS");
			break;
			
		case AT_CMD_AT_WJAPIP_GET:
			strcat(send_cmd, "AT+WJAPIP?");
			break;
		case AT_CMD_AT_WJAP_GET:
			strcat(send_cmd, "AT+WJAP?");
			break;
		case AT_CMD_AT_WJAPQ:
			strcat(send_cmd, "AT+WJAPQ");
			break;
		case AT_CMD_AT_WJAPS:
			strcat(send_cmd, "AT+WJAPS");
			break;
		#if 0
		case AT_CMD_AT_SSLCERT_GET:
			strcat(send_cmd, "AT+SSLCERTGET");
			break;
		case AT_CMD_AT_CIPDOMAIN:
			strcat(send_cmd, "AT+SSLCERTSET");
			break;
		case AT_CMD_AT_CIPAUTOCONN:
			strcat(send_cmd, "AT+CIPAUTOCONN");
			break;
		case AT_CMD_AT_CIPSSLOPT:
			strcat(send_cmd, "AT+CIPSSLOPT");
			break;
		case AT_CMD_AT_CIPSTART:
			strcat(send_cmd, "AT+CIPSTART");
			break;
		#endif
		case AT_CMD_AT_CIPSENDRAW:
			strcat(send_cmd, "AT+CIPSENDRAW");
			break;
		case AT_CMD_AT_CIPRECVCFG_GET:
			strcat(send_cmd, "AT+CIPRECVCFG?");
			break;
			
		
	}
	//strcat(send_cmd, "\r\n");

	memset(pOutBuffer, 0, OutLength);
	ret_val = hal_uart_send(&wifi_uart, send_cmd, strlen(send_cmd), 30000);
	ret_val = hal_uart_send(&wifi_uart, "\r", 1, 30000);
	strcat(send_cmd, "\r\n");
	if(ret_val != HAL_OK)
		return HAL_ERROR;
	while(1){
		do {
			ret_val = hal_uart_recv(&wifi_uart, &Recv_ch, 1, &recv_size, 30000);
			if(ret_val != HAL_OK){
				time_out++;
				if(time_out >= 10)
					return HAL_ERROR;
				krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND/10);
			}
		}while(recv_size != 1);
		time_out = 0;
		*(pOutBuffer + recv_size_t) = Recv_ch;
		recv_size_t++;
		if(recv_size_t >= 4){
			if(strstr(pOutBuffer, send_cmd)   && (respone_start == 0) ){
				respone_start = 1;
				recv_size_t = 0;
				memset(pOutBuffer, 0, OutLength);
			}
			else if(respone_start){
				if(strstr(pOutBuffer, "ERROR\r\n") || strstr(pOutBuffer, "OK\r\n"))
					break;
			}
		}
		if(recv_size_t >= OutLength){
			ret_val = HAL_ERROR;
			break;
		}
	}
	return ret_val;
}

static const struct at_ap_command at_cmds_table[] = {
    { .id = AT_CMD_AT_TEST, .pre_cmd = "AT+TEST", .help = "AT+TEST", .function = at_test },
    { .id = AT_CMD_AT, .pre_cmd = "AT", .help = "AT", .function = handle_at },
    { .id = AT_CMD_AT_GETATVERSION, .pre_cmd = "AT+GETATVERSION", 
    	.help = "AT+GETATVERSION",.function = at_get_at_verion},
    	
    { .id = AT_CMD_AT_FWVER, .pre_cmd = "AT+FWVER?", .help = "AT+FWVER?",.function = at_version},
    { .id = AT_CMD_AT_SYSTIME, .pre_cmd = "AT+SYSTIME?", .help = "AT+SYSTIME?",.function = at_get_common_func},
    { .id = AT_CMD_AT_MEMFREE, .pre_cmd = "AT+MEMFREE?", .help = "AT+MEMFREE?",.function = at_get_common_func},
    { .id = AT_CMD_AT_REBOOT, .pre_cmd = "AT+REBOOT", .help = "AT+REBOOT",.function = at_get_common_func},  
    { .id = AT_CMD_AT_FACTORY, .pre_cmd = "AT+FACTORY", .help = "AT+FACTORY",.function = at_recover_factory_config},
    {.id = AT_CMD_AT_FLASHLOCK_GET, .pre_cmd = "AT+FLASHLOCK?", .help = "AT+FLASHLOCK?",.function = at_get_common_func}, 
    {.id = AT_CMD_AT_FLASHLOCK_SET, .pre_cmd = "AT+FLASHLOCK", .help = "AT+FLASHLOCK=<mode>",.function = at_set_common_func}, //at_set_flash_lock
    {.id = AT_CMD_AT_WEVENT_GET, .pre_cmd = "AT+WEVENT?", .help = "AT+WEVENT?",.function = at_get_common_func},
    {.id = AT_CMD_AT_WEVENT_SET, .pre_cmd = "AT+WEVENT", .help = "AT+WEVENT=<mode>",.function = at_set_common_func}, //at_refer_wifi_event_notification
    {.id = AT_CMD_AT_WLPC_GET, .pre_cmd = "AT+WLPC?", .help = "AT+WLPC?",.function = at_get_common_func},
    {.id = AT_CMD_AT_WLPC_SET, .pre_cmd = "AT+WLPC", .help = "AT+WLPC=<mode>",.function = at_set_common_func}, //at_set_wifi_power_save

    {.id = AT_CMD_AT_UART_GET, .pre_cmd = "AT+UART?", .help = "AT+UART?",.function = at_get_common_func}, 
    {.id = AT_CMD_AT_UART_SET, .pre_cmd = "AT+UART", 
    	.help = "AT+UART=<baud>,<bits>,<stpbit>,<parity>,<flw_ctl>",.function = at_set_common_func}, //at_set_uart_info
    	
    {.id = AT_CMD_AT_UARTFOMAT_GET, .pre_cmd = "AT+UARTFOMAT?", .help = "AT+UARTFOMAT?",.function = at_get_common_func}, 
    {.id = AT_CMD_AT_UARTFOMAT_SET, .pre_cmd = "AT+UARTFOMAT", 
    	.help = "AT+UARTFOMAT=<length>,<time>",.function = at_set_common_func},  //at_set_uart_fomat

    {.id = AT_CMD_AT_UARTE_GET, .pre_cmd = "AT+UARTE?", .help = "AT+UARTE?",.function = at_get_common_func}, 
    {.id = AT_CMD_AT_UARTE_SET, .pre_cmd = "AT+UARTE", 
    	.help = "AT+UARTE=<OPTION>",.function = at_set_common_func}, //at_set_uart_echo

	//wifi config command
    {.id = AT_CMD_AT_WFVER, .pre_cmd = "AT+WFVER", .help = "AT+WFVER",.function = at_get_common_func}, 
    {.id = AT_CMD_AT_WMAC, .pre_cmd = "AT+WMAC?", .help = "AT+WMAC?",.function = at_get_common_func},
    { .id = AT_CMD_AT_WSCANOPT_GET, .pre_cmd = "AT+WSCANOPT?", .help = "AT+WSCANOPT?", .function = at_get_common_func }, 
    { .id = AT_CMD_AT_WSCANOPT_SET, .pre_cmd = "AT+WSCANOPT", 
    	.help = "AT+WSCANOPT=<OPTION>", .function =  at_set_common_func}, //at_set_wifi_scan_option
    { .id = AT_CMD_AT_WSCAN, .pre_cmd = "AT+WSCAN", .help = "AT+WSCAN", .function = at_wl_scan_func }, 
    { .id = AT_CMD_AT_WDHCP_GET, .pre_cmd = "AT+WDHCP?", .help = "AT+WDHCP?", .function = at_get_common_func }, 
    { .id = AT_CMD_AT_WDHCP_SET, .pre_cmd = "AT+WDHCP", .help = "AT+WDHCP=<option>", .function = at_set_common_func }, 
    //AP
    { .id = AT_CMD_AT_WSAPIP_GET, .pre_cmd = "AT+WSAPIP?", .help = "AT+WSAPIP?", .function = at_get_common_func }, 
    { .id = AT_CMD_AT_WSAPIP_SET, .pre_cmd = "AT+WSAPIP", .help = "AT+WSAPIP=<ip>,<mask>,<gate>", .function = at_set_common_func }, 
    { .id = AT_CMD_AT_WSAP_GET, .pre_cmd = "AT+WSAP?", .help = "AT+WSAP?", .function = at_get_common_func }, 
    { .id = AT_CMD_AT_WSAP_SET, .pre_cmd = "AT+WSAP", .help = "AT+WSAP=<ssid>,<psw>", .function = at_set_common_func }, 
    { .id = AT_CMD_AT_WSAPQ, .pre_cmd = "AT+WSAPQ", .help = "AT+WSAPQ", .function = at_get_common_func }, 
    { .id = AT_CMD_AT_WSAPS, .pre_cmd = "AT+WSAPS", .help = "AT+WSAPS", .function = at_get_common_func }, 
	//sta
    { .id = AT_CMD_AT_WJAPIP_GET, .pre_cmd = "AT+WJAPIP?", .help = "AT+WJAPIP?", .function = at_get_common_func }, 
    { .id = AT_CMD_AT_WJAPIP_SET, .pre_cmd = "AT+WJAPIP", .help = "AT+WJAPIP=<ip>,<mask>,<gate>[,<dns>]", .function = at_set_common_func }, 
    { .id = AT_CMD_AT_WJAP_GET, .pre_cmd = "AT+WJAP?", .help = "AT+WJAP?", .function = at_get_common_func },
    { .id = AT_CMD_AT_WJAP_SET, .pre_cmd = "AT+WJAP", .help = "AT+WJAP=<ssid>,<psw>", .function =  at_set_wifi_sta_info_start},
    { .id = AT_CMD_AT_WJAPQ, .pre_cmd = "AT+WJAPQ", .help = "AT+WJAPQ", .function = at_get_common_func },  
    { .id = AT_CMD_AT_WJAPS, .pre_cmd = "AT+WJAPS", .help = "AT+WJAPS", .function = at_get_common_func },
    //TCP/UDP
    { .id = AT_CMD_AT_SSLCERT_GET, .pre_cmd = "AT+SSLCERTGET", .help = "AT+SSLCERTGET=<type>", .function = NULL }, 
    { .id = AT_CMD_AT_SSLCERT_SET, .pre_cmd = "AT+SSLCERTSET", .help = "AT+SSLCERTSET=<type>", .function = NULL }, 
    { .id = AT_CMD_AT_CIPDOMAIN, .pre_cmd = "AT+CIPDOMAIN", .help = "AT+CIPDOMAIN=<domain>", .function = NULL }, 
    { .id = AT_CMD_AT_CIPAUTOCONN, .pre_cmd = "AT+CIPAUTOCONN", .help = "AT+CIPAUTOCONN=<id>[,option]", .function = NULL }, 
    { .id = AT_CMD_AT_CIPSSLOPT, .pre_cmd = "AT+CIPSSLOPT", .help = "AT+CIPSSLOPT=<id>,<isSSLRoot>,<isSSLClient>[,isSSLCrl]", .function = NULL }, 
    { .id = AT_CMD_AT_CIPSTART, .pre_cmd = "AT+CIPSTART", .help = "AT+CIPSTART=<id>,<type>,[domain],[remote_port],[local_port]", .function = NULL }, 
    { .id = AT_CMD_AT_CIPSTOP, .pre_cmd = "AT+CIPSTOP", .help = "AT+CIPSTOP=<id>[,<remote_port>]", .function = NULL }, 
    { .id = AT_CMD_AT_CIPSTATUS, .pre_cmd = "AT+CIPSTATUS", .help = "AT+CIPSTATUS=<id>", .function = NULL }, 
    { .id = AT_CMD_AT_CIPSEND, .pre_cmd = "AT+CIPSEND", .help = "AT+CIPSEND=<id>,[<remote_port>,]<data_length>", .function = NULL }, 
    { .id = AT_CMD_AT_CIPSENDRAW, .pre_cmd = "AT+CIPSENDRAW", .help = "AT+CIPSENDRAW", .function = at_get_common_func }, 
    { .id = AT_CMD_AT_CIPRECV, .pre_cmd = "AT+CIPRECV", .help = "AT+CIPRECV=<id>[,port]", .function = NULL }, 
    { .id = AT_CMD_AT_CIPRECVCFG_GET, .pre_cmd = "AT+CIPRECVCFG?", .help = "AT+CIPRECVCFG?", .function = at_get_common_func },
    { .id = AT_CMD_AT_CIPRECVCFG_SET, .pre_cmd = "AT+CIPRECVCFG", .help = 	"AT+CIPRECVCFG=<recv mode>", .function = at_set_common_func },
    { .id = AT_CMD_MAX, .help = "end", .function = NULL },
};

uint32_t at_cmd_request(enum at_cmd_e request_id, char *pInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	uint16_t cmd_index = 0; 
	uint8_t ret_val = HAL_OK;

	if(aos_mutex_lock(&at_mutex, AOS_WAIT_FOREVER))
	{
		printf("at cmd busy\n");
		return HAL_BUSY;
	}
	
	if(request_id >= AT_CMD_MAX || (pOutBuffer == NULL))
	{
		printf("request_id = %d not support\n", request_id);
		ret_val =  HAL_ERROR;
		goto out;
	}
	
	for(cmd_index = 0; ;cmd_index++){
		if(at_cmds_table[cmd_index].id == AT_CMD_MAX){
			printf("request_id = %d not support\n", request_id);
			ret_val =  HAL_ERROR;
			goto out;
		}
		
		if(request_id == at_cmds_table[cmd_index].id){
			 break;
		}	
	}
	if(at_cmds_table[cmd_index].function == NULL)
	{
		printf("cmd = %s not support\n", at_cmds_table[cmd_index].pre_cmd);
		ret_val =  HAL_ERROR;
	}
	else
		ret_val = at_cmds_table[cmd_index].function(request_id, pInBuffer, pOutBuffer, OutLength);
out:
	 aos_mutex_unlock(&at_mutex);
	return ret_val;
}

static void pre_hand_cmd(char *buff_cmd, char *cmd)
{
	char *p = buff_cmd;
	while(*p == ' ')
		p++;
	while((*p) && (*p != ' ')){
		*cmd++ = *p++;
	}
	*cmd = '\0';
}

static int look_up_cmd(char *buff, uint8_t *cmd_id)
{
	uint16_t cmd_index = 0; 
	uint8_t ret_val = HAL_OK;
	char pre_cmd[50] = {0};
	char len = 0;

	if((buff == NULL)  ||  strlen(buff) < 2){
		*cmd_id = 0xff;
		return HAL_ERROR; 
	}
	len = (strlen(buff) > sizeof(pre_cmd)) ? sizeof(pre_cmd) : strlen(buff);
	memcpy(pre_cmd, buff, len);

	len = 0;
	while(pre_cmd[len] && pre_cmd[len] != '=')
		len++;
	pre_cmd[len] = '\0';
	//printf("pre_cmd = %s\n", pre_cmd);
	for(cmd_index = 0; ;cmd_index++){
		if(at_cmds_table[cmd_index].id == AT_CMD_MAX){
			printf("cmd : %s not support\r\n", pre_cmd);
			return HAL_ERROR;
		}
		
		if(strcmp(pre_cmd, at_cmds_table[cmd_index].pre_cmd) == 0){	
			*cmd_id = at_cmds_table[cmd_index].id;
			break;
		}
	}

	return ret_val;
}

void wifi_cmd_task(void *arg)
{
	//struct nt_cli_command *command = NULL;
	uint8_t cmd_index = 0xff;
	uint32_t ret;
	char pOutBuffer[2000] = "OK";
	//char pInBuffer[100];
	char buff_cmd[100] = {0};
	char cmd[100] = {0};
	char icnt;
	char gch;
	char AT_start = 0;
	char *pInBuffer = NULL;
	
	atcmd_init_mutex();
	
	printf("wifi_cmd_task\n");
	#if 0
	while(AT_start == 0){
		ret = hal_uart_send(&wifi_uart, (void *)"AT\r\n", strlen("AT\r\n"), 30000);
		if(ret == 0){
			do{
				ret = hal_uart_recv(&wifi_uart, (void *)&gch, 1, &recv_size, 3000);
				if(ret != HAL_OK){
					krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND/10);
				}
			}while(recv_size != 1);
			printf("gch = %c\r\n", gch);
		}
	}
	#endif
	while(1){	
		cmd_index = 0xff;
		icnt = 0;
		memset(pOutBuffer, 0, sizeof(pOutBuffer));
		memset(buff_cmd, 0, sizeof(buff_cmd));
		while(1){
			gch = getchar();
			//printf("gch = 0x%x\n", gch);
			if(gch == 'A' && AT_start == 0){
				AT_start = 1;
				icnt = 0;
			}
			if(AT_start){
				if((gch == '\r') || (gch == '\n')){
					buff_cmd[icnt] = '\0';
					if(strlen(buff_cmd) >= 2)
						break;
					else{
						icnt = 0;
						AT_start = 0;
						printf("\r\ninput error\r\n");
						continue;
					}
				}
				else{
					buff_cmd[icnt++] = gch;
					if(icnt >= 100){
						icnt = 0;
						AT_start = 0;
					}
						
				}
			}
		}
		
		pre_hand_cmd(buff_cmd, cmd);
		ret = look_up_cmd(cmd, &cmd_index);
		if(ret != HAL_OK)
			continue;
		//printf("cmd_index = 0x%x\r\n", cmd_index);
		pInBuffer = strstr(cmd, "=") + 1;
		//printf("pInBuffer = %s\r\n", pInBuffer);
		printf("\r\n");
		ret = at_cmd_request((enum at_cmd_e)cmd_index, pInBuffer, pOutBuffer, 2000);
		if(ret == HAL_OK)
			//printf("ret = %d\r\n", ret);
			printf("%s", pOutBuffer);
		else
			printf("AT cmd failed\r\n");
	}
}

