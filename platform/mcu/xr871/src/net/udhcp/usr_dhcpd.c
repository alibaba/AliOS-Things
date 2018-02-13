/* dhcpd.c
 *
 * udhcp Server
 * Copyright (C) 1999 Matthew Ramsay <matthewr@moreton.com.au>
 *			Chris Trew <ctrew@moreton.com.au>
 *
 * Rewrite by Russ Dill <Russ.Dill@asu.edu> July 2001
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <lwip/inet.h>
#include <sys/types.h>
#include "lwip/sockets.h"
#include "debug.h"
#include "dhcpd.h"
#include "arpping.h"
#include "socket.h"
#include "options.h"
#include "files.h"
#include "leases.h"
#include "packet.h"
#include "serverpacket.h"
#include "net/udhcp/usr_dhcpd.h"
#include "dns.h"

/* globals */
struct dhcpOfferedAddr *leases = NULL;
struct server_config_t server_config;

#define DHCPD_THREAD_STACK_SIZE	(2 * 1024)
static OS_Thread_t g_dhcpd_thread;

static void udhcpd_start(void *arg)
{
#ifdef DHCPD_DNS
	fd_set fds;
	int maxfdp;
	int ret;

	int dns_socket = -1;
	char *dns_buf = NULL;
	dns_buf = malloc(DNS_BUF_SIZE);
	if (!dns_buf) {
		DNS_ERR("dns buf malloc faild\n");
		return;
	}
#endif
	int server_socket = -1;
	int bytes = 0;
	struct dhcpMessage *packet = NULL;
	packet = malloc(sizeof(*packet));
	if (!packet) {
		DHCPD_LOG(LOG_ERR, "udhcp server (v%s) started", VERSION);
#ifdef DHCPD_DNS
		free(dns_buf);
#endif
		return;
	}
	unsigned char *state;
	unsigned char *server_id, *requested;
	uint32_t server_id_align, requested_align;
	struct option_set *option;
	struct dhcpOfferedAddr *lease;
	struct dhcp_server_info *server_param = NULL;
	if (arg != NULL)
		server_param = (struct dhcp_server_info *) arg;

	DEBUG(LOG_INFO, "udhcp server (v%s) started", VERSION);

	memset(&server_config, 0, sizeof(struct server_config_t));
	init_config();

	if (server_param != NULL && server_param->lease_time > server_config.min_lease) {
		server_config.lease = server_param->lease_time;
	} else if ((option = find_option(server_config.options, DHCP_LEASE_TIME))) {
		memcpy(&server_config.lease, option->data + 2, 4);
		server_config.lease = ntohl(server_config.lease);
	} else
		server_config.lease = LEASE_TIME;

	if (read_interface(server_config.interface, &server_config.ifindex,
		                 &server_config.server, server_config.arp) < 0) {
		goto exit_server;
	}

	if (server_param != NULL && server_param->max_leases > 0)
		server_config.max_leases = server_param->max_leases;

	if (server_param != NULL && server_param->addr_start != 0
			&& server_param->addr_end != 0
			&& ntohl(server_param->addr_end) >= ntohl(server_param->addr_start)) {
		server_config.start = server_param->addr_start;
		server_config.end = server_param->addr_end;
	}
	if ((ntohl(server_param->addr_end) - ntohl(server_param->addr_start))  > (server_config.max_leases - 1))
		server_config.end = htonl((ntohl(server_config.start) + server_config.max_leases - 1));

	leases = malloc(sizeof(struct dhcpOfferedAddr) * server_config.max_leases);
	memset(leases, 0, sizeof(struct dhcpOfferedAddr) * server_config.max_leases);
	DEBUG(LOG_DEBUG, "start ip=%s", inet_ntoa(server_config.start));
	DEBUG(LOG_DEBUG, "end   ip=%s", inet_ntoa(server_config.end));

	while(1) { /* loop until universe collapses */

		if (server_socket < 0)
			if ((server_socket = listen_socket(INADDR_ANY, SERVER_PORT, server_config.interface)) < 0) {
				DEBUG(LOG_ERR, "FATAL: couldn't create server socket, %s", strerror(errno));
				goto exit_server;
			}
#ifdef DHCPD_DNS
		if (dns_socket < 0)
			if ((dns_socket = dns_listen_socket()) < 0) {
				DNS_ERR("FATAL: couldn't create dns server socket\n");
				goto exit_server;
			}

		FD_ZERO(&fds);
		FD_SET(server_socket, &fds);
		FD_SET(dns_socket, &fds);
		maxfdp = server_socket > dns_socket ? server_socket + 1 : dns_socket + 1;
		ret = select(maxfdp, &fds, NULL, NULL, NULL);
		if (ret < 0)
			goto exit_server;
		else if (ret == 0)
			continue;
		if (FD_ISSET(dns_socket, &fds))
			dns_server(dns_socket, dns_buf, DNS_BUF_SIZE);

		if (FD_ISSET(server_socket, &fds)) {
#endif
			if ((bytes = get_packet(packet, server_socket)) < 0) { /* this waits for a packet - idle */
				if (bytes == -1 && errno != EINTR) {
					DEBUG(LOG_INFO, "error on read, %s, reopening socket", strerror(errno));
					closesocket(server_socket);
					server_socket = -1;
				} else if (bytes == -3) {
					DEBUG(LOG_INFO, "exit server..");
					closesocket(server_socket);
					server_socket = -1;
					goto exit_server;
				}
				continue;
			}

			if ((state = get_option(packet, DHCP_MESSAGE_TYPE)) == NULL) {
				DEBUG(LOG_ERR, "couldn't get option from packet, ignoring");
				continue;
			}
			/* ADDME: look for a static lease */
			lease = find_lease_by_chaddr(packet->chaddr);
			DEBUG(LOG_INFO, "MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					packet->chaddr[0],packet->chaddr[1],
					packet->chaddr[2],packet->chaddr[3],
					packet->chaddr[4],packet->chaddr[5]);
			switch (state[0]) {
				case DHCPDISCOVER:
					DEBUG(LOG_INFO,"received DISCOVER");
					if (sendOffer(packet) < 0) {
						DEBUG(LOG_ERR, "send OFFER failed");
					}
					break;
				case DHCPREQUEST:
					DEBUG(LOG_INFO, "received REQUEST");
					requested = get_option(packet, DHCP_REQUESTED_IP);
					server_id = get_option(packet, DHCP_SERVER_ID);

					if (requested) {
						memcpy(&requested_align, requested, 4);
						DEBUG(LOG_INFO, "requeste id = %s", inet_ntoa(requested_align));
					}
					if (server_id) {
						memcpy(&server_id_align, server_id, 4);
						DEBUG(LOG_INFO, "server id = %s", inet_ntoa(server_id_align));
					}

					if (lease) { /*ADDME: or static lease */
						if (server_id) {
							/* SELECTING State */
							DEBUG(LOG_INFO, "server_id = %08x", ntohl(server_id_align));
							if (server_id_align == server_config.server && requested &&
									requested_align == lease->yiaddr) {
								sendACK(packet, lease->yiaddr);
							}
						} else {
							if (requested) {
								/* INIT-REBOOT State */
								if (lease->yiaddr == requested_align)
									sendACK(packet, lease->yiaddr);
								else sendNAK(packet);
							} else {
								/* RENEWING or REBINDING State */
								if (lease->yiaddr == packet->ciaddr)
									sendACK(packet, lease->yiaddr);
								else {
									/* don't know what to do!!!! */
									sendNAK(packet);
								}
							}
						}

						/* what to do if we have no record of the client */
					} else if (server_id) {
						/* SELECTING State */

					} else if (requested) {
						/* INIT-REBOOT State */
						if ((lease = find_lease_by_yiaddr(requested_align))) {
							if (lease_expired(lease)) {
								/* probably best if we drop this lease */
								memset(lease->chaddr, 0, 16);
								/* make some contention for this address */
							} else sendNAK(packet);
						} else {
							sendNAK(packet);
						}
					} else {
						/* RENEWING or REBINDING State */
					}
					break;
				case DHCPDECLINE:
					DEBUG(LOG_INFO,"received DECLINE");
					if (lease) {
						memset(lease->chaddr, 0, 16);
						lease->expires = time(0) + server_config.decline_time;
					}
					break;
				case DHCPRELEASE:
					DEBUG(LOG_INFO,"received RELEASE");
					if (lease) lease->expires = time(0);
					break;
				case DHCPINFORM:
					DEBUG(LOG_INFO,"received INFORM");
					send_inform(packet);
					break;
				default:
					DEBUG(LOG_WARNING, "unsupported DHCP message (%02x) -- ignoring", state[0]);
			}
#ifdef DHCPD_DNS
		}
#endif
	}

exit_server:
#ifdef DHCPD_DNS
	if (dns_buf != NULL) {
		free(dns_buf);
		dns_buf = NULL;
	}
	if (dns_socket >= 0)
		closesocket(dns_socket);
#endif
	if (packet != NULL ) {
		free(packet);
		packet = NULL;
	}
	if (leases != NULL) {
		free(leases);
		leases = NULL;
	}
	OS_ThreadDelete(&g_dhcpd_thread);
}

static int udhcpd_stop(void)
{
	int fd;
	struct sockaddr_in addr_serv;
	char *stop_msg = "<cancel>";
	if ((fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		DEBUG(LOG_ERR, "socket call failed: %s", strerror(errno));
		return -1;
	}
	memset(&addr_serv, 0, sizeof(addr_serv));
	addr_serv.sin_family = AF_INET;
	addr_serv.sin_port = htons(SERVER_PORT);
	addr_serv.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	sendto(fd, stop_msg, strlen(stop_msg), 0,
			(struct sockaddr *)&addr_serv, sizeof(addr_serv));
	closesocket(fd);
	return 0;
}

void dhcp_server_start(const struct dhcp_server_info *arg)
{
	if (OS_ThreadIsValid(&g_dhcpd_thread)) {
		return;
	}

	if (OS_ThreadCreate(&g_dhcpd_thread,
				"",
				udhcpd_start,
				(void *) arg,
				OS_THREAD_PRIO_APP,
				DHCPD_THREAD_STACK_SIZE) != OS_OK) {
		DEBUG(LOG_ERR, "create main task failed\n");
	}
}

void dhcp_server_stop(void)
{
	if (!OS_ThreadIsValid(&g_dhcpd_thread)) {
		return;
	}

	if (udhcpd_stop() != 0) {
		DEBUG(LOG_ERR, "stop dhcp server failed\n");
		return;
	}

	while (OS_ThreadIsValid(&g_dhcpd_thread)) {
		OS_MSleep(1); /* wait for thread termination */
	}
	DEBUG(LOG_INFO, "stop dhcp server success\n");
}
