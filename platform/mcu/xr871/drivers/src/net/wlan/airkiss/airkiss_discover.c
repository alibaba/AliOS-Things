#include "string.h"

#include "kernel/os/os.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "lwip/ip.h"

#include "airkiss.h"

enum loglevel{
	OFF = 0,
	ERROR = 1,
	INFO = 2,
};

#define g_debuglevel  ERROR

#define AIRKISS_DISCOVER_DBG(level, fmt, args...) do {		\
					if (level <= g_debuglevel)	\
						printf("[airkiss ack]"fmt,##args);		\
				 } while (0)

typedef struct {
	char *app_id;
	char *device_id;
	uint32_t ack_period_ms;
} Airkiss_Online_Ack_Info;

const airkiss_config_t akconf ={
	(airkiss_memset_fn)&memset,
	(airkiss_memcpy_fn)&memcpy,
	(airkiss_memcmp_fn)&memcmp,
	(airkiss_printf_fn)&printf
};

#define AIRKISS_LAN_PORT 12476
static Airkiss_Online_Ack_Info ack_info;

/************************************************************************************************************/
/**
 * allwin
 */
static void airkiss_device_online_ack(Airkiss_Online_Ack_Info *info, int socket_id)
{
	AIRKISS_DISCOVER_DBG(INFO, "into airkiss_device_online_ack\n");

	uint8_t ak_online_ack_buf[200];
	uint16_t ak_online_buf_len = 200;
	int  ret = airkiss_lan_pack(AIRKISS_LAN_SSDP_NOTIFY_CMD,
								//wechat_public_id, device_id,
								info->app_id, info->device_id,
								0, 0,
								ak_online_ack_buf, &ak_online_buf_len,
								&akconf);

	if (ret == AIRKISS_LAN_PAKE_READY) {
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));

		addr.sin_port = htons(AIRKISS_LAN_PORT);
		addr.sin_family = AF_INET;
		if (inet_aton("255.255.255.255", &addr.sin_addr) < 0) {
			AIRKISS_DISCOVER_DBG(ERROR, "%s(), %d, inet_aton error\n", __func__, __LINE__);
			return;
		}

		int tmp = 1;
		int ret = setsockopt(socket_id, SOL_SOCKET, SO_BROADCAST, &tmp, sizeof(int));
		if (ret != 0) {
			AIRKISS_DISCOVER_DBG(ERROR, "%s(), %d, setsockopt error\n", __func__, __LINE__);
			return;
		}
		ret = lwip_sendto(socket_id, ak_online_ack_buf, ak_online_buf_len,
						  0,(const struct sockaddr *)&addr, sizeof(addr));
		if (ret == -1)
			AIRKISS_DISCOVER_DBG(ERROR, "%s(), %d, udp send error\n", __func__, __LINE__);
	}
}

static OS_Thread_t g_airkiss_cycle_ack_thread;
#define AIRKISS_CYCLE_ACK_THREAD_STACK_SIZE	1024 * 1
static uint8_t airkiss_cycle_ack_run = 0;

static void airkiss_cycle_ack_task(void* param)
{
	Airkiss_Online_Ack_Info *info = (Airkiss_Online_Ack_Info *) param;
	int socket_id;
	struct sockaddr_in addr;

	socket_id = socket(AF_INET ,SOCK_DGRAM, 0);
	if (socket_id < 0) {
		AIRKISS_DISCOVER_DBG(ERROR,"create sock error! %s, %d\n", __func__, __LINE__);
		return;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_port= htons(AIRKISS_LAN_PORT);
	addr.sin_addr.s_addr=htonl(INADDR_ANY) ;

	if (bind(socket_id, (struct sockaddr*)&addr, sizeof(addr))<0) {
		AIRKISS_DISCOVER_DBG(ERROR,"bind sock error! %s, %d\n", __func__, __LINE__);
		return;
	}

	while(airkiss_cycle_ack_run){
		airkiss_device_online_ack(info, socket_id);
		OS_MSleep(info->ack_period_ms);
	}
	/*since lan discovery should be send always,so we will never reach this*/
	OS_ThreadDelete(&g_airkiss_cycle_ack_thread);
}

static int airkiss_online_cycle_ack_start(Airkiss_Online_Ack_Info *param)
{
	ack_info = *param;
	airkiss_cycle_ack_run = 1;

	if (OS_ThreadCreate(&g_airkiss_cycle_ack_thread,
						"",
						airkiss_cycle_ack_task,
						(void *)&ack_info,
						OS_THREAD_PRIO_APP,
						AIRKISS_CYCLE_ACK_THREAD_STACK_SIZE) != OS_OK) {
		AIRKISS_DISCOVER_DBG(ERROR, "%s(), %d create airkiss ask thread failed\n", __func__, __LINE__);
		return -1;
	}

	return 0;
}

static void airkiss_online_cycle_ack_stop(void)
{
	airkiss_cycle_ack_run = 0;
	while (OS_ThreadIsValid (&g_airkiss_cycle_ack_thread))
		OS_MSleep(100);
}

static int airkiss_send_active_lan_discovery_packets(int client_socket_fd, Airkiss_Online_Ack_Info *info)
{
	int ret = -1;
	struct sockaddr_in to_addr;
	uint8_t lan_buf[200];
	uint16_t lan_buf_len = sizeof(lan_buf);

	ret = airkiss_lan_pack(AIRKISS_LAN_SSDP_NOTIFY_CMD, info->app_id, info->device_id, 0, 0, lan_buf, &lan_buf_len, &akconf);
	if (ret != AIRKISS_LAN_PAKE_READY) {
		AIRKISS_DISCOVER_DBG(ERROR, "airkiss pack lan packet error!\n");
		return -1;
	}


	FD_ZERO(&to_addr);
	to_addr.sin_family = AF_INET;
	to_addr.sin_port = htons(AIRKISS_LAN_PORT);
	to_addr.sin_addr.s_addr =inet_addr("255.255.255.255");

	int tmp = 1;
	ret = setsockopt(client_socket_fd, SOL_SOCKET, SO_BROADCAST, &tmp, sizeof(int));
	if (ret != 0) {
		AIRKISS_DISCOVER_DBG(ERROR, "%s(), %d, setsockopt error\n", __func__, __LINE__);
		return -1;
	}

	ret = sendto(client_socket_fd, (unsigned char *)lan_buf, lan_buf_len, 0, (struct sockaddr *) &to_addr, sizeof(struct sockaddr));
	if(ret == -1){
		AIRKISS_DISCOVER_DBG(ERROR, "UDP send error %s, %d\n", __func__, __LINE__);
		return -1;
	}

	return 0;
}

static void airkiss_lan_server_reply(int client_socket_fd, struct sockaddr_in addr, Airkiss_Online_Ack_Info *info,
																					char *pdata, unsigned short len)
{
	airkiss_lan_ret_t ret = -1;
	airkiss_lan_ret_t pack_ret;
	uint8_t lan_buf[200];
	uint16_t lan_buf_len = sizeof(lan_buf);

	ret = airkiss_lan_recv(pdata, len, &akconf);

	switch (ret){
	case AIRKISS_LAN_SSDP_REQ:
		addr.sin_port = htons(AIRKISS_LAN_PORT);
		lan_buf_len = sizeof(lan_buf);
		pack_ret = airkiss_lan_pack(AIRKISS_LAN_SSDP_RESP_CMD,  info->app_id, info->device_id, 0, 0, lan_buf, &lan_buf_len, &akconf);
		if (pack_ret != AIRKISS_LAN_PAKE_READY) {
			AIRKISS_DISCOVER_DBG(ERROR, "Pack lan packet error! %s, %d\n", __func__, __LINE__);
			return;
		}

		AIRKISS_DISCOVER_DBG(ERROR, "AIRKISS_LAN_SSDP_REQ !\n");
		ret = sendto(client_socket_fd, (unsigned char *)lan_buf, lan_buf_len, 0, (struct sockaddr *) &addr, sizeof(struct sockaddr));
		if(ret != 0){
			AIRKISS_DISCOVER_DBG(ERROR, "lan_server_reply sendto ret=%d\n", ret);
		}
		break;
	default:
		AIRKISS_DISCOVER_DBG(ERROR,"Pack is not ssdq req!\n");
		break;
	}
}

static OS_Thread_t g_airkiss_online_dialog_thread;
#define AIRKISS_ONLINE_DIALOG_THREAD_STACK_SIZE	1024 * 1
static uint8_t airkiss_dialog_run = 0;

static void airkiss_online_dialog_task(void *arg)
{
	Airkiss_Online_Ack_Info *info = (Airkiss_Online_Ack_Info *)arg;
	int server_sock_fd,len;
	struct sockaddr_in addr;
	int sock_timeout_val = 1000; /* 1000 ms */
	int addr_len = sizeof(struct sockaddr_in);
	char buffer[256] = {0};

	if((server_sock_fd = socket(AF_INET,SOCK_DGRAM,0)) < 0){
		AIRKISS_DISCOVER_DBG(ERROR, "create sock error!\n");
		airkiss_dialog_run = 0;
		return;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_port = htons(AIRKISS_LAN_PORT);
	addr.sin_addr.s_addr=htonl(INADDR_ANY) ;

	if (bind(server_sock_fd, (struct sockaddr *)&addr, sizeof(addr))<0) {
		AIRKISS_DISCOVER_DBG(ERROR, "bind sock error!\n");
		airkiss_dialog_run = 0;
		return;
	}

	int ret = setsockopt(server_sock_fd, SOL_SOCKET, SO_RCVTIMEO, &sock_timeout_val, sizeof(sock_timeout_val));
	if (ret != 0) {
		AIRKISS_DISCOVER_DBG(ERROR, "%s(), %d, setsockopt error\n", __func__, __LINE__);
	}

	memset(&buffer, 0, sizeof(buffer));
	len = recvfrom(server_sock_fd, buffer, sizeof(buffer), 0 , (struct sockaddr *)&addr , (socklen_t *)&addr_len);

	if (len != -1) {
		airkiss_lan_server_reply(server_sock_fd, addr, info, buffer, len);
	}

	while (airkiss_dialog_run) {
		if (airkiss_send_active_lan_discovery_packets(server_sock_fd, info) == -1)
			break;
		OS_MSleep(info->ack_period_ms);
	}

	OS_ThreadDelete(&g_airkiss_online_dialog_thread);
}

static int airkiss_online_dialog_mode_start(Airkiss_Online_Ack_Info *param)
{
	ack_info = *param;
	if (airkiss_dialog_run) {
		AIRKISS_DISCOVER_DBG(ERROR, "airkiss online already start %s(), %d \n", __func__, __LINE__);
		return -1;
	}
	airkiss_dialog_run = 1;

	if (OS_ThreadCreate(&g_airkiss_online_dialog_thread,
						"",
						airkiss_online_dialog_task,
						(void *)&ack_info,
						OS_THREAD_PRIO_APP,
						AIRKISS_ONLINE_DIALOG_THREAD_STACK_SIZE) != OS_OK) {
		AIRKISS_DISCOVER_DBG(ERROR, "create airkiss ask thread failed %s(), %d \n", __func__, __LINE__);
		return -1;
	}

	return 0;
}

static void airkiss_online_dialog_mode_stop(void)
{
	airkiss_dialog_run = 0;
	while (OS_ThreadIsValid (&g_airkiss_cycle_ack_thread))
		OS_MSleep(100);
}

int wlan_airkiss_online_cycle_ack_start(char *app_id, char *drv_id, uint32_t period_ms)
{
	Airkiss_Online_Ack_Info param;
	param.app_id = app_id;
	param.device_id = drv_id;
	param.ack_period_ms = period_ms;
	return airkiss_online_cycle_ack_start(&param);
}

int wlan_airkiss_online_cycle_ack_stop(void)
{
	airkiss_online_cycle_ack_stop();
	return 0;
}

int wlan_airkiss_online_dialog_mode_start(char *app_id, char *drv_id, uint32_t period_ms)
{
	Airkiss_Online_Ack_Info param;
	param.app_id = app_id;
	param.device_id = drv_id;
	param.ack_period_ms = period_ms;
	return airkiss_online_dialog_mode_start(&param);
}

int wlan_airkiss_online_dialog_mode_stop(void)
{
	airkiss_online_dialog_mode_stop();
	return 0;
}
