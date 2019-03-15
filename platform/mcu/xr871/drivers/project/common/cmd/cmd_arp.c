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
#include "common/framework/net_ctrl.h"
#include "lwip/tcpip.h"
#include "netif/etharp.h"

int arp_clean_table(struct netif *netif)
{
	tcpip_callback((tcpip_callback_fn)etharp_cleanup_netif, netif);
	return 0;
}

int arp_state(struct netif *netif)
{
/*	void etharp_printtable_netif(struct netif *netif);
	tcpip_callback((tcpip_callback_fn)etharp_printtable_netif, netif);
*/
	return 0;
}

int arp_del(struct netif *netif)
{
	return 0;
}

int arp_set(struct netif *netif)
{
	return 0;
}

enum cmd_status cmd_arp_exec(char *cmd)
{
	int ret = 0;
	int argc;
	char *argv[3];

	if (g_wlan_netif == NULL) {
		return CMD_STATUS_FAIL;
	}

	argc = cmd_parse_argv(cmd, argv, cmd_nitems(argv));
	if (argc < 1) {
		CMD_ERR("invalid arp cmd, argc %d\n", argc);
		return CMD_STATUS_INVALID_ARG;
	}

	if (cmd_strcmp(argv[0], "clean") == 0) {
		ret = arp_clean_table(g_wlan_netif);
	} else if (cmd_strcmp(argv[0], "state") == 0) {
		ret = arp_state(g_wlan_netif);
	} else if (cmd_strcmp(argv[0], "del") == 0) {
		ret = arp_del(g_wlan_netif);
	} else if (cmd_strcmp(argv[0], "set") == 0) {
		ret = arp_set(g_wlan_netif);
	} else {
		CMD_ERR("invalid arp cmd <%s>\n", cmd);
		return CMD_STATUS_INVALID_ARG;
	}

	return (ret == 0 ? CMD_STATUS_OK : CMD_STATUS_FAIL);
}


