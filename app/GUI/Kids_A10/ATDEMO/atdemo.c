#include <stdio.h>
#include "stm32l4xx_hal.h"
#include "atdemo.h"
#include <k_api.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern uart_dev_t brd_uart1_dev;
extern uart_dev_t uart_0;

#define os_uart uart_0
#define wifi_uart brd_uart1_dev

#define UART1_PORT 0
#define UART2_PORT 1

static aos_mutex_t at_mutex;

static int atcmd_init_mutex()
{
    if (0 != aos_mutex_new(&at_mutex)) {
        LOGE("atdemo", "Creating mutex failed");
        return HAL_ERROR;
    }

    return HAL_OK;
}

int handle_at_func(enum at_cmd_e id, char *PInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	int32_t ret_val = HAL_OK;
	uint32_t recv_size = 0, recv_size_t = 0;
	char Recv_ch, end_ch, ret_ch;
	uint8_t time_out = 0;

	memset(pOutBuffer, 0, OutLength);
	hal_uart_send(&brd_uart1_dev, (void *)"AT", 2, 30000);
	ret_val = hal_uart_send(&brd_uart1_dev, (void *)"\r", 1, 30000);
	if(ret_val != HAL_OK)
		return HAL_ERROR;
	while(1){
		do {
			ret_val = hal_uart_recv(&brd_uart1_dev, (void *)&Recv_ch, 1, &recv_size, 3000);
			if(ret_val != HAL_OK){
				time_out++;
				if(time_out >= 10)
					return HAL_ERROR;
				krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND / 4);
			}
		}while(recv_size != 1);
		if(ret_val != HAL_OK)
			break;
		
		*(pOutBuffer + recv_size_t) = Recv_ch;
		recv_size_t++;
		if(recv_size_t >= 4){
			ret_ch = *(pOutBuffer + recv_size_t - 2);
			end_ch = *(pOutBuffer + recv_size_t - 1);
			if((ret_ch == '\r') && (end_ch == '\n')){
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

int at_wl_scan_func(enum at_cmd_e id, char *PInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	int32_t ret_val = HAL_OK;
	uint32_t recv_size = 0, recv_size_t = 0;
	char send_cmd[20] = "AT+WSCAN\r\n";
	char Recv_ch, end_ch, ret_ch;
	uint8_t time_out = 0;
	
	memset(pOutBuffer, 0, OutLength);
	ret_val = hal_uart_send(&brd_uart1_dev, send_cmd, strlen(send_cmd), 30000);
	if(ret_val != HAL_OK)
		return HAL_ERROR;
	while(1){
		do {
			ret_val = hal_uart_recv(&brd_uart1_dev, &Recv_ch, 1, &recv_size, 30000);
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


int at_refer_wifi_sta_info(enum at_cmd_e id, char *PInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	int32_t ret_val = HAL_OK;
	uint32_t recv_size = 0, recv_size_t = 0;
	//char send_cmd[20] = "AT+WJAP?\r\n";
	char Recv_ch, end_ch, ret_ch;
	uint8_t time_out = 0;

	if(OutLength < 200)
		return HAL_ERROR;

	memset(pOutBuffer, 0, OutLength);
	ret_val = hal_uart_send(&brd_uart1_dev, "AT+WJAP?", strlen("AT+WJAP?"), 30000);
	ret_val = hal_uart_send(&brd_uart1_dev, "\r", 1, 30000);
	if(ret_val != HAL_OK)
		return HAL_ERROR;
	while(1){
		do {
			ret_val = hal_uart_recv(&brd_uart1_dev, &Recv_ch, 1, &recv_size, 30000);
			if(ret_val != HAL_OK){
				time_out++;
				if(time_out >= 10)
					return HAL_ERROR;
				krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND/4);
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

int at_set_wifi_sta_info_start(enum at_cmd_e id, char *PInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	int32_t ret_val = HAL_OK;
	uint32_t recv_size = 0, recv_size_t = 0;
	char send_cmd[128] = "AT+WJAP=";
	char Recv_ch, end_ch, ret_ch;
	uint8_t time_out = 0;

	if(OutLength < 200)
		return HAL_ERROR;

	strcat(send_cmd, PInBuffer);
	memset(pOutBuffer, 0, OutLength);
	ret_val = hal_uart_send(&brd_uart1_dev, send_cmd, strlen(send_cmd), 30000);
	ret_val = hal_uart_send(&brd_uart1_dev, "\r", 1, 30000);
	if(ret_val != HAL_OK)
		return HAL_ERROR;
	while(1){
		do {
			ret_val = hal_uart_recv(&brd_uart1_dev, (void *)&Recv_ch, 1, &recv_size, 30000);
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

int at_get_ap_current_status(enum at_cmd_e id, char *PInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	int32_t ret_val = HAL_OK;
	uint32_t recv_size = 0, recv_size_t = 0;
	//char send_cmd[20] = "AT+WJAPS\r\n";
	char Recv_ch, end_ch, ret_ch;
	uint8_t time_out = 0;

	memset(pOutBuffer, 0, OutLength);
	ret_val = hal_uart_send(&brd_uart1_dev, "AT+WJAPS", strlen("AT+WJAPS"), 30000);
	ret_val = hal_uart_send(&brd_uart1_dev, "\r", 1, 30000);
	if(ret_val != HAL_OK)
		return HAL_ERROR;
	while(1){
		do {
			ret_val = hal_uart_recv(&brd_uart1_dev, (void *)&Recv_ch, 1, &recv_size, 30000);
			if(ret_val != HAL_OK){
				time_out++;
				if(time_out >= 10)
					return HAL_ERROR;
				krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND/4);
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

int at_reboot(enum at_cmd_e id, char *PInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	int32_t ret_val = HAL_OK;
	uint32_t recv_size = 0, recv_size_t = 0;
	//char send_cmd[20] = "AT+REBOOT";
	char Recv_ch, end_ch, ret_ch;
	uint8_t time_out = 0;

	memset(pOutBuffer, 0, OutLength);
	ret_val = hal_uart_send(&brd_uart1_dev, "AT+REBOOT", strlen("AT+REBOOT"), 30000);
	ret_val = hal_uart_send(&brd_uart1_dev, "\r", 1, 30000);
	if(ret_val != HAL_OK)
		return HAL_ERROR;
	while(1){
		//ret_val = hal_uart_recv(&brd_uart1_dev, &Recv_ch, 1, &recv_size, 30000);
		do {
			ret_val = hal_uart_recv(&brd_uart1_dev, (void *)&Recv_ch, 1, &recv_size, 30000);
			if(ret_val != HAL_OK){
				time_out++;
				if(time_out >= 10)
					return HAL_ERROR;
				krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND/4);
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

int at_recover_factory_config(enum at_cmd_e id, char *PInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	int32_t ret_val = HAL_OK;
	uint32_t recv_size = 0, recv_size_t = 0;
	//char send_cmd[20] = "AT+FACTORY";
	char Recv_ch, end_ch, ret_ch;
	uint8_t time_out = 0;

	memset(pOutBuffer, 0, OutLength);
	ret_val = hal_uart_send(&brd_uart1_dev, "AT+FACTORY", strlen("AT+FACTORY"), 30000);
	ret_val = hal_uart_send(&brd_uart1_dev, "\r", 1, 30000);
	if(ret_val != HAL_OK)
		return HAL_ERROR;
	while(1){
		do {
			ret_val = hal_uart_recv(&brd_uart1_dev, (void *)&Recv_ch, 1, &recv_size, 30000);
			if(ret_val != HAL_OK){
				time_out++;
				if(time_out >= 10)
					return HAL_ERROR;
				krhino_task_sleep(RHINO_CONFIG_TICKS_PER_SECOND/4);
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

int at_common_func(enum at_cmd_e id, char *PInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	int32_t ret_val = HAL_OK;
	uint32_t recv_size = 0, recv_size_t = 0;
	char send_cmd[50] = {0};
	char Recv_ch, end_ch, ret_ch;
	uint8_t time_out = 0;

	switch(id){
		case AT_CMD_AT_TEST:
			strcat(send_cmd, "AT+TEST");
			break;
		case AT_CMD_AT:
			strcat(send_cmd, "AT");
			break;
		case AT_CMD_AT_WJAP_GET:
			strcat(send_cmd, "AT+WJAP?");
			break;
		case AT_CMD_AT_WJAPS:
			strcat(send_cmd, "AT+WJAPS");
			break;
		case AT_CMD_AT_REBOOT:
			strcat(send_cmd, "AT+REBOOT");
			break;
		case AT_CMD_AT_FACTORY:
			strcat(send_cmd, "AT+FACTORY");
			break;
	}
	//strcat(send_cmd, "\r\n");

	memset(pOutBuffer, 0, OutLength);
	ret_val = hal_uart_send(&brd_uart1_dev, send_cmd, strlen(send_cmd), 30000);
	ret_val = hal_uart_send(&brd_uart1_dev, "\r", 1, 30000);
	if(ret_val != HAL_OK)
		return HAL_ERROR;
	while(1){
		do {
			ret_val = hal_uart_recv(&brd_uart1_dev, &Recv_ch, 1, &recv_size, 30000);
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

static const struct at_ap_command at_cmds_table[] = {
#if 1
    { .id = AT_CMD_AT_TEST, .pre_cmd = "AT+TEST", .help = "AT+TEST", .function = at_common_func }, //
    { .id = AT_CMD_AT, .pre_cmd = "AT", .help = "AT", .function = at_common_func },
    { .id = AT_CMD_AT_WSCAN, .pre_cmd = "AT+WSCAN", .help = "AT+WSCAN", .function = at_wl_scan_func }, 
    { .id = AT_CMD_AT_WJAP_GET, .pre_cmd = "AT+WJAP?", .help = "AT+WJAP?", .function = at_common_func },
    { .id = AT_CMD_AT_WJAP_SET, .pre_cmd = "AT+WJAP", .help = "AT+WJAP=<ssid>,<psw>", .function =  at_set_wifi_sta_info_start},
    { .id = AT_CMD_AT_WJAPS, .pre_cmd = "AT+WJAPS", .help = "AT+WJAPS", .function = at_common_func },
    { .id = AT_CMD_AT_REBOOT, .pre_cmd = "AT+REBOOT", .help = "AT+REBOOT",.function = at_common_func},  
    { .id = AT_CMD_AT_FACTORY, .pre_cmd = "AT+FACTORY", .help = "AT+FACTORY",.function = at_common_func},
#else
    {.name = "AT+TEST",.help = "AT+TEST",.function = at_test},
    {.name = "AT",.help = "AT",.function = handle_at},
    {.name = "AT+GETATVERSION",.help = "AT+GETATVERSION",.function = at_get_at_verion},

    {.name = "AT+FWVER?",.help = "AT+FWVER?",.function = at_version}, //LX ok
    {.name = "AT+SYSTIME?",.help = "AT+SYSTIME?",.function = at_system_run_time},//LX   ok
    {.name = "AT+MEMFREE?",.help = "AT+MEMFREE?",.function = at_system_memory_free},//LX  ok
    {.name = "AT+FACTORY",.help = "AT+FACTORY",.function = at_recover_factory_config},//LX ok
    {.name = "AT+REBOOT",.help = "AT+REBOOT",.function = at_reboot},  //LX ok
    {.name = "AT+FLASHLOCK?",.help = "AT+FLASHLOCK?",.function = at_refer_flash_lock},  //LX  ok
    {.name = "AT+FLASHLOCK",.help = "AT+FLASHLOCK=<mode>",.function = at_set_flash_lock},  //LX  ok
    {.name = "AT+WEVENT?",.help = "AT+WEVENT?",.function = at_refer_wifi_event_notification},  //LX ok
    {.name = "AT+WEVENT",.help = "AT+WEVENT=<mode>",.function = at_set_wifi_event_notification},  //LX OK
    {.name = "AT+WLPC?",.help = "AT+WLPC?",.function = at_refer_wifi_power_save},  //LX ok
    {.name = "AT+WLPC",.help = "AT+WLPC=<mode>",.function = at_set_wifi_power_save},  //LX ok

    {.name = "AT+UART?",.help = "AT+UART?",.function = at_refer_uart_info}, //LX
    {.name = "AT+UART",.help = "AT+UART=<baud>,<bits>,<stpbit>,<parity>,<flw_ctl>",.function = at_set_uart_info}, //LX
    {.name = "AT+UARTFOMAT?",.help = "AT+UARTFOMAT?",.function = at_refer_uart_fomat}, //LX
    {.name = "AT+UARTFOMAT",.help = "AT+UARTFOMAT=<length>,<time>",.function = at_set_uart_fomat}, //LX
    {.name = "AT+UARTE?",.help = "AT+UARTE?",.function = at_refer_uart_echo}, //LX ok
    {.name = "AT+UARTE",.help = "AT+UARTE=<OPTION>",.function = at_set_uart_echo}, //LX ok
////wifi config command
    {.name = "AT+WFVER",.help = "AT+WFVER",.function = at_wifi_firmware_version}, //LX ok
    {.name = "AT+WMAC?",.help = "AT+WMAC?",.function = at_wl_mac},//Lx ok
    {.name = "AT+WSCANOPT?",.help = "AT+WSCANOPT?",.function = at_refer_wifi_scan_option}, //LX ok
    {.name = "AT+WSCANOPT",.help = "AT+WSCANOPT=<OPTION>",.function = at_set_wifi_scan_option}, //LX ok
    {.name = "AT+WSCAN",.help = "AT+WSCAN",.function = at_wl_scan},  //LX ok
    {.name = "AT+WDHCP?",.help = "AT+WDHCP?",.function = at_refer_wifi_dhcp},//LX ok
    {.name = "AT+WDHCP",.help = "AT+WDHCP=<option>",.function = at_set_wifi_dhcp},//LX ok
///AP
    {.name = "AT+WSAPIP?",.help = "AT+WSAPIP?",.function = at_refer_wifi_ap_ip_mask_gate},//LX ok
    {.name = "AT+WSAPIP",.help = "AT+WSAPIP=<ip>,<mask>,<gate>",.function = at_set_wifi_ap_ip_mask_gate},//LX ok
    {.name = "AT+WSAP?",.help = "AT+WSAP?",.function = at_refer_wifi_ap_info},//LX ok
    {.name = "AT+WSAP",.help = "AT+WSAP=<ssid>,<psw>",.function = at_set_wifi_ap_info_start},//LX ok
    {.name = "AT+WSAPQ",.help = "AT+WSAPQ",.function = at_wifi_ap_quit},//LX ok
    {.name = "AT+WSAPS",.help = "AT+WSAPS",.function = at_get_ap_current_status},//LX ok
///sta
    {.name = "AT+WJAPIP?",.help = "AT+WJAPIP?",.function = at_refer_wifi_sta_ip_mask_gate_dns},  //LX ok
    {.name = "AT+WJAPIP",.help = "AT+WJAPIP=<ip>,<mask>,<gate>[,<dns>]",.function = at_set_wifi_sta_ip_mask_gate_dns},//LX ok
    {.name = "AT+WJAP?",.help = "AT+WJAP?",.function = at_refer_wifi_sta_info},//LX ok
    {.name = "AT+WJAP",.help = "AT+WJAP=<ssid>,<psw>",.function = at_set_wifi_sta_info_start},//LX ok
    {.name = "AT+WJAPQ",.help = "AT+WJAPQ",.function = at_wifi_sta_quit},//LX ok
    {.name = "AT+WJAPS",.help = "AT+WJAPS",.function = at_get_sta_current_status},//LX ok
    
///TCP/UDP:
    {.name = "AT+SSLCERTGET",.help = "AT+SSLCERTGET",.function = at_get_ssl_cert},
    {.name = "AT+SSLCERTSET",.help = "AT+SSLCERTSET=<type>",.function = at_set_ssl_cert},
    {.name = "AT+CIPDOMAIN",.help = "AT+CIPDOMAIN",.function = at_get_cip_domain_dns},//LX ok
    {.name = "AT+CIPAUTOCONN",.help = "AT+CIPAUTOCONN=<id>[,option]",.function = at_get_cip_auto_connect},//LX ok
    {.name = "AT+CIPSSLOPT",.help = "AT+CIPSSLOPT",.function = at_get_cip_ssl_opt},
    {.name = "AT+CIPSTART",.help = "AT+CIPSTART",.function = at_get_cip_start},//LX
    {.name = "AT+CIPSTOP",.help = "AT+CIPSTOP",.function = at_get_cip_stop},//LX ok
    {.name = "AT+CIPSTATUS",.help = "AT+CIPSTATUS",.function = at_get_cip_status},//LX ok
    {.name = "AT+CIPSEND",.help = "AT+CIPSEND=<id>,[<remote_port>,]<data_length>",.function = at_cip_send},//LX ok
    {.name = "AT+CIPSENDRAW",.help = "AT+CIPSENDRAW",.function = at_cip_send_raw},//LX ok
    {.name = "AT+CIPRECV",.help = "AT+CIPRECV",.function = at_cip_recv},//LX ok
    {.name = "AT+CIPRECVCFG?",.help = "AT+CIPRECVCFG?",.function = at_refer_cip_recv_cfg},//LX ok
    {.name = "AT+CIPRECVCFG",.help = "AT+CIPRECVCFG",.function = at_cip_recv_cfg},//LX ok
#endif
    { .id = AT_CMD_MAX, .help = "end", .function = NULL },
};

uint32_t at_cmd_request(enum at_cmd_e request_id, char *pInBuffer, char *pOutBuffer, uint16_t OutLength)
{
	uint16_t cmd_index = 0; 
	uint8_t ret_val = HAL_OK;

	 aos_mutex_lock(&at_mutex, AOS_WAIT_FOREVER);

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
	char *pInBuffer = NULL;

	atcmd_init_mutex();
	
	printf("wifi_cmd_task\n");
	while(1){	
		cmd_index = 0xff;
		icnt = 0;
		memset(pOutBuffer, 0, sizeof(pOutBuffer));
		memset(buff_cmd, 0, sizeof(buff_cmd));
		while(1){
			gch = getchar();
			//ret = hal_uart_recv(&os_uart, (void *)&gch, 1, &rev_cnt, 10000);
			if(gch == '\r'){
				buff_cmd[icnt] = '\0';
				break;
			}
			else {
				buff_cmd[icnt++] = gch;
				if(icnt >= 100)
					break;
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

