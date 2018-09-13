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

#include "lwip/inet.h"

#include "cmd_util.h"

#include "common/framework/net_ctrl.h"

enum cmd_status cmd_ifconfig_exec(char *cmd)
{
	struct netif *nif = g_wlan_netif;

	if (nif == NULL) {
		return CMD_STATUS_FAIL;
	}

	if (cmd_strcmp(cmd, "status") == 0) {
		if (netif_is_up(nif) && netif_is_link_up(nif)) {
			char address[16];
			char gateway[16];
			char netmask[16];
			inet_ntoa_r(nif->ip_addr, address, sizeof(address));
			inet_ntoa_r(nif->gw, gateway, sizeof(gateway));
			inet_ntoa_r(nif->netmask, netmask, sizeof(netmask));
			cmd_write_respond(CMD_STATUS_OK, "%c%c%d up, "
				              "address:%s gateway:%s netmask:%s",
			                  nif->name[0], nif->name[1], nif->num,
			                  address, gateway, netmask);
		} else {
			cmd_write_respond(CMD_STATUS_OK, "%c%c%d down",
			                  nif->name[0], nif->name[1], nif->num);
		}
		return CMD_STATUS_ACKED;
	} else if (cmd_strcmp(cmd, "up") == 0) {
		net_config(nif, 1);
	} else if (cmd_strcmp(cmd, "down") == 0) {
		net_config(nif, 0);
	} else {
		CMD_ERR("invalid argument '%s'\n", cmd);
		return CMD_STATUS_INVALID_ARG;
	}

	return CMD_STATUS_OK;
}
