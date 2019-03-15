#include <string.h>

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

#define AIRKISS_ACK_DBG(level, fmt, args...) do {		\
					if (level <= g_debuglevel)	\
						printf("[airkiss ack]"fmt,##args);		\
				 } while (0)


static struct netif *wlan_netif;

#define MAX_TIME 4294967295
#define AIRKISS_ACK_UDP_PORT 10000

static const airkiss_config_t ak_config = {
								(airkiss_memset_fn)&memset,
								(airkiss_memcpy_fn)&memcpy,
								(airkiss_memcmp_fn)&memcmp,
								(airkiss_printf_fn)&printf
							};

static uint32_t airkiss_d_time(uint32_t start_time, uint32_t os_time)
{
	uint32_t d_time = 0;
	if (start_time <= os_time)
		d_time = os_time - start_time;
	else
		d_time = MAX_TIME - start_time + os_time;
	return d_time;
}

static int airkiss_ack_successful(uint32_t random_num)
{
	uint8_t num[1];
	num[0] = (uint8_t)random_num;

	struct netif *nif = wlan_netif;

	if (nif == NULL)
		return 0;
	if (netif_is_up(nif) && netif_is_link_up(nif)) {
		AIRKISS_ACK_DBG(INFO, "airkiss ack start\n");

		int ak_ack_socket_id = lwip_socket(AF_INET, SOCK_DGRAM, 0);
		if (ak_ack_socket_id < 0) {
			AIRKISS_ACK_DBG(ERROR, "%s(), %d, create socket fail\n", __func__, __LINE__);
			return 0;
		}

		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));

		addr.sin_port = htons(AIRKISS_ACK_UDP_PORT);
		addr.sin_family = AF_INET;
		if (inet_aton("255.255.255.255", &addr.sin_addr) < 0) {
			AIRKISS_ACK_DBG(ERROR, "%s(), %d, inet_aton error\n", __func__, __LINE__);
			return 0;
		}

		int tmp = 1;
		int ret = setsockopt(ak_ack_socket_id, SOL_SOCKET, SO_BROADCAST, &tmp, sizeof(int));
		if (ret != 0) {
			AIRKISS_ACK_DBG(ERROR, "%s(), %d, setsockopt error\n", __func__, __LINE__);
			lwip_close(ak_ack_socket_id);
			return 0;
		}

		int i = 0;
		for (i = 0; i < 300; i ++) {
			int ret = lwip_sendto(ak_ack_socket_id, num,
								  1, 0,(struct sockaddr *)&addr, sizeof(addr));
			if (ret == -1) {
				AIRKISS_ACK_DBG(ERROR, "%s(), %d, udp send error, %d\n", __func__, __LINE__, OS_GetErrno());
				lwip_close(ak_ack_socket_id);
				return 0;
			}
			OS_MSleep(1);
		}

		lwip_close(ak_ack_socket_id);
		return 1;
	}
	return 0;
}

static uint8_t airkiss_ack_run = 0;

static int airkiss_ack(uint32_t random_num, uint32_t time_out_ms)
{
	uint32_t start_time = OS_JiffiesToMSecs(OS_GetJiffies());
	uint32_t os_time = 0;

	while (airkiss_ack_run) {
		if (airkiss_ack_successful(random_num))
			break;

		os_time = OS_JiffiesToMSecs(OS_GetJiffies());
		uint32_t d_time = airkiss_d_time(start_time, os_time);
		if (d_time >= time_out_ms)
			return -1;

		OS_MSleep(100);
	}

	AIRKISS_ACK_DBG(INFO, "airkiss ack end\n");

	return 0;
}

static int airkiss_ack_start(struct netif *nif, uint32_t random_num, uint32_t timeout_ms)
{
	AIRKISS_ACK_DBG(INFO, "ack start\n");

	if (nif == NULL)
		return -1;

	wlan_netif = nif;
	airkiss_ack_run = 1;

	return airkiss_ack(random_num, timeout_ms);
}

static void airkiss_ack_stop(void)
{
	airkiss_ack_run = 0;
}

int wlan_airkiss_ack_start(struct netif *nif, uint32_t random_num, uint32_t timeout_ms)
{
	return airkiss_ack_start(nif, random_num, timeout_ms);
}

int wlan_airkiss_ack_stop(void)
{
	airkiss_ack_stop();
	return 0;
}
