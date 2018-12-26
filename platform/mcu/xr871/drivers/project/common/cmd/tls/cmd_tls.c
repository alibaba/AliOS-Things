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

#include "../cmd_util.h"
#include "cmd_tls.h"
#include "tls.h"

struct config_exec_arg {
	char     cmd_str[5];
	int      (*handler)(void *var);
	int      config;
};

mbedtls_test_param g_tls_data;

int count_one(unsigned int P)
{
	int n = 0;
	while (P != 0) {
		if ((P & 0x1) != 0) {
			n++;
		}
		P = P>>1;
	}
	return n;
}

static int set_port(void *arg)
{
	char *dest = arg;
	if (dest != NULL) {
		if (cmd_strlen(dest) > (sizeof(g_tls_data.server_port) - 1)) {
			CMD_ERR("invalid param : port.\n");
			return -1;
		}
		cmd_memcpy(g_tls_data.server_port, dest, cmd_strlen(dest));
		g_tls_data.flags |= MBEDTLS_SSL_FLAG_SERVER_PORT;
		g_tls_data.server_port[cmd_strlen(dest)] = '\0';
	}

	return 0;
}

static int set_servername(void *arg)
{
	char *dest = arg;
	if (dest != NULL) {
		if (cmd_strlen(dest) > (sizeof(g_tls_data.server_name) - 1)) {
			CMD_ERR("invalid param : server_name.\n");
			return -1;
		}
		cmd_memcpy(g_tls_data.server_name, dest, cmd_strlen(dest));
		g_tls_data.flags |= MBEDTLS_SSL_FLAG_SERVER_NAME;
		g_tls_data.server_name[cmd_strlen(dest)] = '\0';
	}

	return 0;
}

static int set_timer(void *arg)
{
	char *dest = arg;

	if (*dest) {
		g_tls_data.continue_ms = cmd_atoi(dest);
		g_tls_data.flags |= MBEDTLS_SSL_FLAG_CONTINUE;
	}

	return 0;
}

static int set_client(void *arg)
{
	g_tls_data.flags |= MBEDTLS_SSL_FLAG_CLINET;

	return 0;
}

static int set_server(void *arg)
{
	g_tls_data.flags |= MBEDTLS_SSL_FLAG_SERVER;

	return 0;
}
#if 0
static int set_webserver(void *arg)
{
	g_tls_data.flags |= MBEDTLS_SSL_FLAG_WEBSERVER;

	return 0;
}

static int set_webclient(void *arg)
{
	g_tls_data.flags |= MBEDTLS_SSL_FLAG_WEBCLIENT;

	return 0;
}
#endif
static struct config_exec_arg keywords[] = {

	{"-p",  set_port,       1},    /* set port*/
	{"-n",  set_servername, 1},
	{"-t",  set_timer,      1},
	{"-c",  set_client,     0},
	{"-s",  set_server,     0},
	//{"-ws", set_webserver,  0},
	//-wc", set_webclient,  0},
	{"",    NULL,           0},
};

enum cmd_status cmd_tls_exec(char *cmd)
{
	int argc, i = 0, j = 0;
	char *argv[12];
	unsigned int flags = 0;
	mbedtls_test_param *param = (mbedtls_test_param *)&g_tls_data;
	memset(param, 0, sizeof(mbedtls_test_param));
	argc = cmd_parse_argv(cmd, argv, 12);

	for (j = 0; j < argc; ++j) {
		 for (i = 0; strlen(keywords[i].cmd_str); i++)
			if (cmd_strcmp(argv[j], keywords[i].cmd_str) == 0) {
				if (keywords[i].config == 1)
					j++;
				keywords[i].handler(argv[j]);
			}
	}

	flags = MBEDTLS_SSL_FLAG_CLINET | MBEDTLS_SSL_FLAG_WEBCLIENT;

	if ((param->flags & flags) != 0) {
		if ((param->flags & MBEDTLS_SSL_FLAG_SERVER_NAME) == 0) {
			CMD_ERR("invalid tls cmd.\n");
			return CMD_STATUS_INVALID_ARG;
		}
	}
	flags = MBEDTLS_SSL_FLAG_CLINET|MBEDTLS_SSL_FLAG_SERVER|	\
		MBEDTLS_SSL_FLAG_WEBSERVER|MBEDTLS_SSL_FLAG_WEBCLIENT;

	flags = param->flags & flags;
	if (count_one(flags) > 1 || count_one(flags) == 0) {
		CMD_ERR("invalid tls cmd.\n");
		return CMD_STATUS_INVALID_ARG;
	}

	if (tls_start(param) == 0)
		return CMD_STATUS_OK;
	else
		return CMD_STATUS_FAIL;
}
