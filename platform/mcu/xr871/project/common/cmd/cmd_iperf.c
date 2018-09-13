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

#include "common/iperf/iperf.h"
#include "common/framework/net_ctrl.h"

static const char *iperf_mode_str[IPERF_MODE_NUM] = {
	"udp-send",
	"udp-recv",
	"tcp-send",
	"tcp-recv",
}; /* index by enum IPERF_MODE */
#define IPERF_STOP_SIGNAL	"stop"
enum cmd_status cmd_iperf_exec(char *cmd)
{
	int argc, i;
	char *argv[5];
	struct iperf_data idata;
	cmd_memset(&idata, 0, sizeof(idata));
	argc = cmd_parse_argv(cmd, argv, 5);
	if (cmd_strcmp(argv[0], IPERF_STOP_SIGNAL) == 0) {
		if (iperf_stop(NULL) == 0)
			return CMD_STATUS_OK;
		else
			return CMD_STATUS_FAIL;
	}
	if (argc < 3) {
		CMD_ERR("invalid iperf cmd, argc %d\n", argc);
		return CMD_STATUS_INVALID_ARG;
	}

	for (i = 0; i < IPERF_MODE_NUM; ++i) {
		if (cmd_strcmp(argv[0], iperf_mode_str[i]) == 0) {
			idata.mode = (enum IPERF_MODE)i;
			break;
		}
	}

	if (i >= IPERF_MODE_NUM) {
		CMD_ERR("invalid iperf mode '%s'\n", argv[0]);
		return CMD_STATUS_INVALID_ARG;
	}

	cmd_strlcpy(idata.remote_ip, argv[1], 16);
	idata.run_time = cmd_atoi(argv[2]);
	if (argc == 4)
		idata.port = cmd_atoi(argv[3]);

	if (iperf_start(g_wlan_netif, &idata) == 0)
		return CMD_STATUS_OK;
	else
		return CMD_STATUS_FAIL;
}
