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

enum cmd_status cmd_iperf_exec(char *cmd)
{
	int argc, handle, ret;
	char *argv[20];

	argc = cmd_parse_argv(cmd, &argv[1], cmd_nitems(argv) - 1);
	argv[0] = "iperf";
	handle = iperf_parse_argv(argc + 1, argv);

	if (handle == -1) {
		CMD_DBG("handle=%d\n", handle);
		return CMD_STATUS_FAIL;
	}

	struct netif *nif = g_wlan_netif;
	if (nif == NULL || !NET_IS_IP4_VALID(nif)) {
		CMD_ERR("net is down, iperf start failed\n");
		iperf_handle_free(handle);
		return CMD_STATUS_FAIL;
	}

	if (handle >= 0 && handle < IPERF_ARG_HANDLE_MAX) {
		ret = iperf_handle_start(g_wlan_netif, handle);
		if (ret == -1)
			iperf_handle_free(handle);
	}

	return CMD_STATUS_OK;
}
