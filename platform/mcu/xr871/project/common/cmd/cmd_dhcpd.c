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
#include "cmd_dhcpd.h"
#include "net/udhcp/usr_dhcpd.h"
#include "net/lwip/ipv4/lwip/inet.h"

#define CMD_DHCPD_ADDR_START "192.168.51.150"
#define CMD_DHCPD_ADDR_END   "192.168.51.155"

static struct dhcp_server_info dhcpd_info;

enum cmd_status dhcpd_start_exec(char *cmd)
{
	if (dhcpd_info.addr_start == 0)
		dhcpd_info.addr_start =  inet_addr(CMD_DHCPD_ADDR_START);
	if (dhcpd_info.addr_end == 0)
		dhcpd_info.addr_end = inet_addr(CMD_DHCPD_ADDR_END);
	if (dhcpd_info.lease_time == 0)
		dhcpd_info.lease_time = 60*60*12;

	dhcp_server_start(&dhcpd_info);
	return CMD_STATUS_OK;
}

enum cmd_status dhcpd_stop_exec(char *cmd)
{
	dhcp_server_stop();
	return CMD_STATUS_OK;
}

enum cmd_status dhcpd_set_ippool_exec(char *cmd)
{
	int argc;
	char *argv[2];
	ip_addr_t ip_addr_start;
	ip_addr_t ip_addr_end;

	argc = cmd_parse_argv(cmd, argv, cmd_nitems(argv));
	if (argc < 2) {
		CMD_ERR("invalid dhcp cmd, argc %d\n", argc);
		return CMD_STATUS_INVALID_ARG;
	}
	if (inet_aton(argv[0], &ip_addr_start) ==0 ||
		inet_aton(argv[1], &ip_addr_end) == 0) {
		CMD_ERR("invalid dhcp cmd <%s %s>\n", argv[0], argv[1]);
		return CMD_STATUS_INVALID_ARG;
	}

	dhcpd_info.addr_start = ip_addr_start.addr;
	dhcpd_info.addr_end   = ip_addr_end.addr;

	return CMD_STATUS_OK;
}

enum cmd_status dhcpd_set_max_leases_exec(char *cmd)
{
	int argc;
	char *argv[1];
	int leases;

	argc = cmd_parse_argv(cmd, argv, cmd_nitems(argv));
	if (argc < 1) {
		CMD_ERR("invalid dhcp cmd, argc %d\n", argc);
		return CMD_STATUS_INVALID_ARG;
	}

	leases = cmd_atoi(argv[0]);
	if (leases > 254) {
		CMD_ERR("invalid dhcp cmd, leases=%s", argv[0]);
		return CMD_STATUS_INVALID_ARG;
	}

	dhcpd_info.max_leases = leases;
	return CMD_STATUS_OK;
}

enum cmd_status dhcpd_set_lease_time_exec(char *cmd)
{
	int argc;
	char *argv[1];
	int lease_time;

	argc = cmd_parse_argv(cmd, argv, cmd_nitems(argv));
	if (argc < 1) {
		CMD_ERR("invalid dhcp cmd, argc %d\n", argc);
		return CMD_STATUS_INVALID_ARG;
	}
	if ((lease_time = cmd_atoi(argv[0])) < 0) {
		CMD_ERR("invalid dhcp cmd, leasetime=%d", lease_time);
		return CMD_STATUS_INVALID_ARG;
	}

	dhcpd_info.lease_time = lease_time;
	return CMD_STATUS_OK;
}

/*
 * dhcp commands
 */
static struct cmd_data g_dhcpd_cmds[] = {
	{ "start",	    dhcpd_start_exec },
	{ "stop",	    dhcpd_stop_exec },
	{ "ippool",	    dhcpd_set_ippool_exec },
	{ "leases",	    dhcpd_set_max_leases_exec },
	{ "leasetime",  dhcpd_set_lease_time_exec },
};

enum cmd_status cmd_dhcpd_exec(char *cmd)
{
	return cmd_exec(cmd, g_dhcpd_cmds, cmd_nitems(g_dhcpd_cmds));
}
