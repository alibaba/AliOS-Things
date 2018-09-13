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

#include "cmd_util.h"
#include "net/lwip/lwip/sockets.h"

#define BROADCAST_THREAD_STACK_SIZE		(2 * 1024)
#define RECEIVE_BUF_LEN 				1500
OS_Thread_t g_broadcast_thread;

typedef enum {
	THREAD_STOP,
	THREAD_RUN,
} run_flag;

typedef enum {
	BROADCAST_RECV,
	BROADCAST_SEND,
} broadcast_mode;

typedef struct {
	int 			port;
	ip_addr_t 		ip_addr;
	broadcast_mode	mode;
	u16_t 			data_range_min;
	u16_t			data_range_max;
	int 			time;
	int				count;
	char*			data_p;
	run_flag		run_flag;
	int 			num;
} broadcast_format_param;

static broadcast_format_param broadcast_param;

static int get_range(char *buf, u16_t *min, u16_t *max)
{
	char *start, *end;
	start = end = buf;
	u16_t min_t = 0, max_t = 0;

	if (buf == NULL || min == NULL || max == NULL)
		return -1;
	if (*start == ',') {
		min_t = 0;
		max_t = (u16_t)atoi(++start);
		if (max_t < min_t)
			return -1;
		*min = min_t;
		*max = max_t;
		return 0;
	} else {
		while (*end != ',' && *end != '\0')
			end++;
		if (*end == '\0')
			return -1;
		*end = '\0';
		min_t = (u16_t)atoi(start);
		start = ++end;
		if (*start == '\0')
			max_t = 65535;
		else
			max_t = (u16_t)atoi(start);
		if (max_t < min_t)
			return -1;
		*min = min_t;
		*max = max_t;
		return 0;
	}
}

int recv_broadcast_packet(broadcast_format_param *param)
{
	int fd;
	int optval = 1;
	int sock_timeout_val = 1000;//1s
	int data_len=0;
	char *message_buf;
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);

	message_buf = malloc(RECEIVE_BUF_LEN);
	if (message_buf == NULL) {
		CMD_ERR("message_buf malloc faild\n");
		return -1;
	}
/*
	CMD_DBG("port=%d ip=%s mode=%d data_range_min=%d data_range_max=%d\n",
			param->port, ipaddr_ntoa((ip_addr_t*)(&(param->ip_addr))),
			param->mode, param->data_range_min, param->data_range_max);
*/
	memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(param->port);

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == -1) {
		CMD_ERR("socket create failed\n");
		return -1;
	}

	setsockopt(fd, SOL_SOCKET, SO_BROADCAST, (void*)&optval, sizeof(optval));
	setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO,
				(void*)&sock_timeout_val, sizeof(sock_timeout_val));
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		CMD_ERR("socket bind failed\n");
		closesocket(fd);
		return -1;
	}
	param->run_flag = THREAD_RUN;
	while (param->run_flag) {
		data_len = recvfrom(fd, message_buf, RECEIVE_BUF_LEN, 0,
							(struct sockaddr *)&addr, &addr_len);
		if ((data_len > 0) && (data_len >= param->data_range_min) &&
			(data_len <= param->data_range_max)) {
			if (param->ip_addr.addr == INADDR_BROADCAST) {
				param->num++;
				//CMD_DBG("Receive message:%.*s\n", data_len, message_buf);
			} else if (param->ip_addr.addr == addr.sin_addr.s_addr) {
				param->num++;
				//CMD_DBG("Receive message:%.*s\n", data_len, message_buf);
			}
		}
    }
	CMD_DBG("broadcast packets num received: %d\n", param->num);
	closesocket(fd);
	free(message_buf);
	return 0;
}

void broadcast_run(void *param)
{
	((broadcast_format_param*)param)->num = 0;
	if (((broadcast_format_param*)param)->mode == BROADCAST_RECV)
		recv_broadcast_packet((broadcast_format_param*)param);

	OS_ThreadDelete(&g_broadcast_thread);
}

int broadcast_start(broadcast_format_param *param)
{
	if (OS_ThreadIsValid(&g_broadcast_thread)) {
		CMD_ERR("broadcast task is running\n");
		return -1;
	}
	if (OS_ThreadCreate(&g_broadcast_thread,
				"",
				broadcast_run,
				(void *)param,
				OS_THREAD_PRIO_APP,
				BROADCAST_THREAD_STACK_SIZE) != OS_OK) {
		CMD_ERR("broadcast task create failed\n");
		return -1;
	}
	return 0;
}

enum cmd_status broadcast_state_exec(char *cmd)
{
	CMD_DBG("broadcast packets num received: %d\n", broadcast_param.num);
	return CMD_STATUS_OK;
}

enum cmd_status broadcast_reset_exec(char *cmd)
{
	broadcast_param.num = 0;
	return CMD_STATUS_OK;
}

enum cmd_status broadcast_stop_exec(char *cmd)
{
	broadcast_param.run_flag = THREAD_STOP;
	return CMD_STATUS_OK;
}

enum cmd_status broadcast_recv_exec(char *cmd)
{
	int argc;
	int port;
	char ip_buf[16];
	char range_buf[12];
	int ret = 0;

	argc = cmd_sscanf(cmd, "p=%d i=%16s r=%12s", &port, ip_buf, range_buf);
	if (argc != 3) {
		CMD_ERR("invalid broadcast cmd, argc %d\n", argc);
		return CMD_STATUS_INVALID_ARG;
	}

	broadcast_param.mode = BROADCAST_RECV;
	if (port > 65535 || port < 0) {
		ret = -1;
		goto exit;
	}
	broadcast_param.port = port;
	if (inet_aton(ip_buf, &(broadcast_param.ip_addr)) == 0) {
		ret = -1;
		goto exit;
	}
	if (get_range(range_buf, &(broadcast_param.data_range_min),
					&(broadcast_param.data_range_max)) != 0) {
		ret = -1;
		goto exit;
	}
	if (broadcast_start(&broadcast_param) != 0) {
		ret = -1;
		goto exit;
	}

exit:
	if (ret != 0) {
		CMD_ERR("invalid broadcast cmd '%s'\n", cmd);
		return CMD_STATUS_INVALID_ARG;
	}
	return CMD_STATUS_OK;
}

enum cmd_status broadcast_send_exec(char *cmd)
{
	return CMD_STATUS_OK;
}

/*
 * dhcp commands
 */
static struct cmd_data g_broadcast_cmds[] = {
	{ "recv",    broadcast_recv_exec },
	{ "send",    broadcast_send_exec },
	{ "state",   broadcast_state_exec },
	{ "reset",   broadcast_reset_exec },
	{ "stop",    broadcast_stop_exec },
};

enum cmd_status cmd_broadcast_exec(char *cmd)
{
	return cmd_exec(cmd, g_broadcast_cmds, cmd_nitems(g_broadcast_cmds));
}
