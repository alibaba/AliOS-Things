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

#include "airkiss.h"

#define g_debuglevel  ERROR

static int ack_successful(struct netif *nif, uint32_t random_num)
{
	uint8_t num[1];
	int tmp = 1;
	int ret = -1;
	int i;
	struct sockaddr_in addr;
	int socketfd;

	if (nif == NULL)
		return -1;

	num[0] = (uint8_t)random_num;

	if (!netif_is_up(nif) || ip_addr_isany(&nif->ip_addr) || !netif_is_link_up(nif))
		return -1;

	socketfd = lwip_socket(AF_INET, SOCK_DGRAM, 0);
	if (socketfd < 0) {
		AIRKISS_DBG(ERROR, "%s() create socket fail\n", __func__);
		return -1;
	}

	memset(&addr, 0, sizeof(addr));

	addr.sin_port = htons(AIRKISS_ACK_UDP_PORT);
	addr.sin_family = AF_INET;
	if (inet_aton("255.255.255.255", &addr.sin_addr) < 0) {
		AIRKISS_DBG(ERROR, "%s() inet_aton error\n", __func__);
		goto out;
	}

	ret = setsockopt(socketfd, SOL_SOCKET, SO_BROADCAST, &tmp, sizeof(int));
	if (ret) {
		AIRKISS_DBG(ERROR, "%s() setsockopt error\n", __func__);
		goto out;
	}

	for (i = 0; i < 50; i ++) {
		ret = lwip_sendto(socketfd, num, 1, 0, (struct sockaddr *)&addr,
		                  sizeof(addr));
		if (ret == -1) {
			AIRKISS_DBG(ERROR, "%s() udp send error, %d\n", __func__,
			            OS_GetErrno());
			goto out;
		}
		OS_MSleep(2);
	}

	ret = 0;

out:
	lwip_close(socketfd);

	return ret;
}

int airkiss_ack_start(airkiss_priv_t *priv, uint32_t random_num,
                      uint32_t timeout_ms)
{
	int ret = -1;
	uint32_t end_time;

	AIRKISS_DBG(INFO, "ack start\n");
	OS_ThreadSuspendScheduler();
	priv->ack_run |= AK_TASK_RUN;
	OS_ThreadResumeScheduler();
	end_time = OS_JiffiesToMSecs(OS_GetJiffies()) + timeout_ms;

	while (!(priv->ack_run & AK_TASK_STOP) &&
	       OS_TimeBefore(OS_JiffiesToMSecs(OS_GetJiffies()), end_time)) {
		if (!ack_successful(priv->nif, random_num)) {
			ret = 0;
			break;
		}

		OS_MSleep(100);
	}
	OS_ThreadSuspendScheduler();
	priv->ack_run = 0;
	OS_ThreadResumeScheduler();

	AIRKISS_DBG(INFO, "ack end\n");

	return ret;
}

int airkiss_ack_stop(airkiss_priv_t *priv)
{
	OS_ThreadSuspendScheduler();
	priv->ack_run |= AK_TASK_STOP;
	OS_ThreadResumeScheduler();

	while (priv->ack_run & AK_TASK_RUN) {
		OS_MSleep(10);
	}

	return 0;
}
