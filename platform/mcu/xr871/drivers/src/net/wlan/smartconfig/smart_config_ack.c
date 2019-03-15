/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>
#include "kernel/os/os.h"
#include "lwip/sockets.h"
#include "lwip/inet.h"
#include "lwip/ip.h"

enum loglevel{
	OFF = 0,
	ERROR = 1,
	INFO = 2,
};

#define g_debuglevel  ERROR

#define SMART_ACK_DBG(level, fmt, args...) do {		   \
					if (level <= g_debuglevel)	       \
						printf("[sc ack]"fmt,##args);  \
				 } while (0)


static struct netif *wlan_netif;

#define MAX_TIME 4294967295
#define SC_ACK_UDP_PORT 10000

static uint32_t sc_d_time(uint32_t start_time, uint32_t os_time)
{
	uint32_t d_time = 0;
	if (start_time <= os_time)
		d_time = os_time - start_time;
	else
		d_time = MAX_TIME - start_time + os_time;
	return d_time;
}

static int sc_ack_successful(uint32_t random_num)
{
	uint8_t num[1];
	num[0] = (uint8_t)random_num;

	struct netif *nif = wlan_netif;

	if (nif == NULL)
		return 0;
	if (netif_is_up(nif) && netif_is_link_up(nif)) {
		SMART_ACK_DBG(INFO, "airkiss ack start\n");

		int ak_ack_socket_id = lwip_socket(AF_INET, SOCK_DGRAM, 0);
		if (ak_ack_socket_id < 0) {
			SMART_ACK_DBG(ERROR, "%s(), %d, create socket fail\n", __func__, __LINE__);
			return 0;
		}

		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));

		addr.sin_port = htons(SC_ACK_UDP_PORT);
		addr.sin_family = AF_INET;
		if (inet_aton("239.0.0.1", &addr.sin_addr) < 0) {
			SMART_ACK_DBG(ERROR, "%s(), %d, inet_aton error\n", __func__, __LINE__);
			return 0;
		}

		int tmp = 1;
		int ret = setsockopt(ak_ack_socket_id, SOL_SOCKET, SO_BROADCAST, &tmp, sizeof(int));
		if (ret != 0) {
			SMART_ACK_DBG(ERROR, "%s(), %d, setsockopt error\n", __func__, __LINE__);
			lwip_close(ak_ack_socket_id);
			return 0;
		}

		int i = 0;
		for (i = 0; i < 300; i ++) {
			int ret = lwip_sendto(ak_ack_socket_id, num,
								  1, 0,(struct sockaddr *)&addr, sizeof(addr));
			if (ret == -1) {
				SMART_ACK_DBG(ERROR, "%s(), %d, udp send error, %d\n", __func__, __LINE__, OS_GetErrno());
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

static uint8_t sc_ack_run = 0;

static int sc_ack(uint32_t random_num, uint32_t time_out_ms)
{
	uint32_t start_time = OS_JiffiesToMSecs(OS_GetJiffies());
	uint32_t os_time = 0;

	while (sc_ack_run) {
		if (sc_ack_successful(random_num))
			break;

		os_time = OS_JiffiesToMSecs(OS_GetJiffies());
		uint32_t d_time = sc_d_time(start_time, os_time);
		if (d_time >= time_out_ms)
			return -1;

		OS_MSleep(100);
	}

	SMART_ACK_DBG(INFO, "smart_config ack end\n");

	return 0;
}

static int smart_config_ack_start(struct netif *nif, uint32_t random_num, uint32_t timeout_ms)
{
	SMART_ACK_DBG(INFO, "%s(), %d smart_config ask start\n", __func__, __LINE__);

	if (nif == NULL)
		return -1;

	wlan_netif = nif;
	sc_ack_run = 1;

	return sc_ack(random_num, timeout_ms);
}

static void smart_config_ack_stop(void)
{
	sc_ack_run = 0;
}

int wlan_smart_config_ack_start(struct netif *nif, uint32_t random_num, uint32_t timeout_ms)
{
	return smart_config_ack_start(nif, random_num, timeout_ms);
}

int wlan_smart_config_ack_stop(void)
{
	smart_config_ack_stop();
	return 0;
}
