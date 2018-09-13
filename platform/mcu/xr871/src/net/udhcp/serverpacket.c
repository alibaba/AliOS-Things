/* serverpacket.c
 *
 * Constuct and send DHCP server packets
 *
 * Russ Dill <Russ.Dill@asu.edu> July 2001
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
#ifdef DHCPD_LWIP
#include <lwip/sockets.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <stdlib.h>
#include <string.h>
#include "packet.h"
#include "debug.h"
#include "dhcpd.h"
#include "options.h"
#include "leases.h"
#ifdef DHCPD_TIMEALT
#include "dhcp_time.h"
#else
#include <time.h>
#endif
/* send a packet to giaddr using the kernel ip stack */
static int send_packet_to_relay(struct dhcpMessage *payload)
{
	DEBUG(LOG_INFO, "Forwarding packet to relay");

	return kernel_packet(payload, server_config.server, SERVER_PORT,
			payload->giaddr, SERVER_PORT);
}


/* send a packet to a specific arp address and ip address by creating our own ip packet */
static int send_packet_to_client(struct dhcpMessage *payload, int force_broadcast)
{
	unsigned char *chaddr;
	u_int32_t ciaddr;

	if (force_broadcast) {
		DEBUG(LOG_INFO, "broadcasting packet to client (NAK)");
		ciaddr = INADDR_BROADCAST;
		chaddr = MAC_BCAST_ADDR;
	} else if (payload->ciaddr) {
		DEBUG(LOG_INFO, "unicasting packet to client ciaddr");
		ciaddr = payload->ciaddr;
		chaddr = payload->chaddr;
	} else if (ntohs(payload->flags) & BROADCAST_FLAG) {
		DEBUG(LOG_INFO, "broadcasting packet to client (requested)");
		ciaddr = INADDR_BROADCAST;
		chaddr = MAC_BCAST_ADDR;
	} else {
		DEBUG(LOG_INFO, "unicasting packet to client yiaddr");
		ciaddr = payload->yiaddr;
		chaddr = payload->chaddr;
	}
	return raw_packet(payload, server_config.server, SERVER_PORT,
			ciaddr, CLIENT_PORT, chaddr, server_config.ifindex);
}


/* send a dhcp packet, if force broadcast is set, the packet will be broadcast to the client */
static int send_packet(struct dhcpMessage *payload, int force_broadcast)
{
	int ret;

	if (payload->giaddr)
		ret = send_packet_to_relay(payload);
	else ret = send_packet_to_client(payload, force_broadcast);
	return ret;
}


static void init_packet(struct dhcpMessage *packet, struct dhcpMessage *oldpacket, char type)
{
	init_header(packet, type);
	packet->xid = oldpacket->xid;
	memcpy(packet->chaddr, oldpacket->chaddr, 16);
	packet->flags = oldpacket->flags;
	packet->giaddr = oldpacket->giaddr;
	packet->ciaddr = oldpacket->ciaddr;
	add_simple_option(packet->options, DHCP_SERVER_ID, server_config.server);
}


/* add in the bootp options */
static void add_bootp_options(struct dhcpMessage *packet)
{
	packet->siaddr = server_config.siaddr;
	if (server_config.sname)
		strncpy((char *)packet->sname, server_config.sname, sizeof(packet->sname) - 1);
	if (server_config.boot_file)
		strncpy((char *)packet->file, server_config.boot_file, sizeof(packet->file) - 1);
}


/* send a DHCP OFFER to a DHCP DISCOVER */
int sendOffer(struct dhcpMessage *oldpacket)
{
#ifdef DHCPD_HEAP_REPLACE_STACK
	struct dhcpMessage *packet;
#else
	struct dhcpMessage packet;
#endif
	struct dhcpOfferedAddr *lease = NULL;
	u_int32_t req_align, lease_time_align = server_config.lease;
	unsigned char *req, *lease_time;
	struct option_set *curr;
	//struct in_addr addr;

#ifdef DHCPD_HEAP_REPLACE_STACK
	packet = calloc(1,sizeof(struct dhcpMessage));
	if (!packet) {
		DHCPD_LOG(LOG_ERR, "Calloc failed...%d",__LINE__);
		return -1;
	}
	init_packet(packet, oldpacket, DHCPOFFER);
#else
	init_packet(&packet, oldpacket, DHCPOFFER);
#endif

	/* ADDME: if static, short circuit */
	/* the client is in our lease/offered table */
	if ((lease = find_lease_by_chaddr(oldpacket->chaddr))) {
		if (!lease_expired(lease))
			lease_time_align = lease->expires - time(0);
#ifdef DHCPD_HEAP_REPLACE_STACK
		packet->yiaddr = lease->yiaddr;
#else
		packet.yiaddr = lease->yiaddr;
#endif
	/* Or the client has a requested ip */
	} else if ((req = get_option(oldpacket, DHCP_REQUESTED_IP)) &&

		   /* Don't look here (ugly hackish thing to do) */
		   memcpy(&req_align, req, 4) &&

		   /* and the ip is in the lease range */
		   ntohl(req_align) >= ntohl(server_config.start) &&
		   ntohl(req_align) <= ntohl(server_config.end) &&

		   /* and its not already taken/offered */ /* ADDME: check that its not a static lease */
		   ((!(lease = find_lease_by_yiaddr(req_align)) ||

		   /* or its taken, but expired */ /* ADDME: or maybe in here */
		   lease_expired(lease)))) {
#ifdef DHCPD_HEAP_REPLACE_STACK
				packet->yiaddr = req_align; /* FIXME: oh my, is there a host using this IP? */
#else
				packet.yiaddr = req_align; /* FIXME: oh my, is there a host using this IP? */
#endif

	/* otherwise, find a free IP */ /*ADDME: is it a static lease? */
	} else {
#ifdef DHCPD_HEAP_REPLACE_STACK
		packet->yiaddr = find_address(0);
		/* try for an expired lease */
		if (!packet->yiaddr) packet->yiaddr = find_address(1);
#else
		packet.yiaddr = find_address(0);

		/* try for an expired lease */
		if (!packet.yiaddr) packet.yiaddr = find_address(1);
#endif
	}
#ifdef DHCPD_HEAP_REPLACE_STACK
	if(!(packet->yiaddr)) {
		if (packet != NULL)
			free(packet);

#else
	if(!packet.yiaddr) {
#endif
		DHCPD_LOG(LOG_WARNING, "no IP addresses to give -- OFFER abandoned");
		return -1;
	}
#ifdef DHCPD_HEAP_REPLACE_STACK
	if (!add_lease(packet->chaddr, packet->yiaddr, server_config.offer_time)) {
		if (packet != NULL) {
			free(packet);
			packet = NULL;
		}
#else
	if (!add_lease(packet.chaddr, packet.yiaddr, server_config.offer_time)) {
#endif
		DHCPD_LOG(LOG_WARNING, "lease pool is full -- OFFER abandoned");
		return -1;
	}

	if ((lease_time = get_option(oldpacket, DHCP_LEASE_TIME))) {
		memcpy(&lease_time_align, lease_time, 4);
		lease_time_align = ntohl(lease_time_align);
		if (lease_time_align > server_config.lease)
			lease_time_align = server_config.lease;
	}

	/* Make sure we aren't just using the lease time from the previous offer */
	if (lease_time_align < server_config.min_lease)
		lease_time_align = server_config.lease;
	/* ADDME: end of short circuit */
#ifdef DHCPD_HEAP_REPLACE_STACK
	struct netif *netif = netif_find(server_config.interface);

	add_simple_option(packet->options, DHCP_LEASE_TIME, htonl(lease_time_align));
#ifdef __CONFIG_LWIP_V1
	add_simple_option(packet->options, DHCP_SUBNET, ip4_addr_get_u32(&netif->netmask));
	add_simple_option(packet->options, DHCP_ROUTER, ip4_addr_get_u32(&netif->gw));
	add_simple_option(packet->options, DHCP_DNS_SERVER, ip4_addr_get_u32(&netif->ip_addr));
#elif LWIP_IPV4 /* now only for IPv4 */
	add_simple_option(packet->options, DHCP_SUBNET, ip4_addr_get_u32(ip_2_ip4(&netif->netmask)));
	add_simple_option(packet->options, DHCP_ROUTER, ip4_addr_get_u32(ip_2_ip4(&netif->gw)));
	add_simple_option(packet->options, DHCP_DNS_SERVER, ip4_addr_get_u32(ip_2_ip4(&netif->ip_addr)));
#else
	#error "IPv4 not support!"
#endif

#else
	add_simple_option(packet.options, DHCP_LEASE_TIME, htonl(lease_time_align));
#endif

	curr = server_config.options;
	while (curr) {
		if (curr->data[OPT_CODE] != DHCP_LEASE_TIME)
#ifdef DHCPD_HEAP_REPLACE_STACK
			add_option_string(packet->options, curr->data);
#else
			add_option_string(packet.options, curr->data);
#endif
		curr = curr->next;
	}
#ifdef DHCPD_HEAP_REPLACE_STACK
	add_bootp_options(packet);

	//addr.s_addr = packet->yiaddr;
	DHCPD_LOG(LOG_INFO, "sending OFFER of %s", inet_ntoa(packet->yiaddr));
	int ret = send_packet(packet, 0);
	if (packet != NULL) {
		free(packet);
		packet = NULL;
	}
	return ret;
#else
	add_bootp_options(&packet);

	addr.s_addr = packet.yiaddr;
	DHCPD_LOG(LOG_INFO, "sending OFFER of %s", inet_ntoa(addr));
	return send_packet(&packet, 0);
#endif
}


int sendNAK(struct dhcpMessage *oldpacket)
{
#ifdef DHCPD_HEAP_REPLACE_STACK
	struct dhcpMessage *packet;
#else
	struct dhcpMessage packet;
#endif
#ifdef DHCPD_HEAP_REPLACE_STACK
	packet = calloc(1,sizeof(struct dhcpMessage));
	if (!packet)
		DHCPD_LOG(LOG_ERR, "Calloc failed...%d",__LINE__);
	init_packet(packet, oldpacket, DHCPNAK);
#else
	init_packet(&packet, oldpacket, DHCPNAK);
#endif

	DEBUG(LOG_INFO, "sending NAK");
#ifdef DHCPD_HEAP_REPLACE_STACK
	int ret = send_packet(packet, 1);
	if (packet != NULL)
		free(packet);
	return ret;
#else
	return send_packet(&packet, 1);
#endif
}


int sendACK(struct dhcpMessage *oldpacket, u_int32_t yiaddr)
{
#ifdef DHCPD_HEAP_REPLACE_STACK
	struct dhcpMessage *packet;
#else
	struct dhcpMessage packet;
#endif
	struct option_set *curr;
	unsigned char *lease_time;
	u_int32_t lease_time_align = server_config.lease;
	//struct in_addr addr;

#ifdef DHCPD_HEAP_REPLACE_STACK
	packet = calloc(1,sizeof(struct dhcpMessage));
	if (!packet)
		DHCPD_LOG(LOG_ERR, "Calloc failed...%d",__LINE__);
	init_packet(packet, oldpacket, DHCPACK);
	packet->yiaddr = yiaddr;
#else
	init_packet(&packet, oldpacket, DHCPACK);

	packet.yiaddr = yiaddr;
#endif

	if ((lease_time = get_option(oldpacket, DHCP_LEASE_TIME))) {
		memcpy(&lease_time_align, lease_time, 4);
		lease_time_align = ntohl(lease_time_align);
		if (lease_time_align > server_config.lease)
			lease_time_align = server_config.lease;
		else if (lease_time_align < server_config.min_lease)
			lease_time_align = server_config.lease;
	}
#ifdef DHCPD_HEAP_REPLACE_STACK
	struct netif *netif = netif_find(server_config.interface);

	add_simple_option(packet->options, DHCP_LEASE_TIME, htonl(lease_time_align));
#ifdef __CONFIG_LWIP_V1
	add_simple_option(packet->options, DHCP_SUBNET, ip4_addr_get_u32(&netif->netmask));
	add_simple_option(packet->options, DHCP_ROUTER, ip4_addr_get_u32(&netif->gw));
	add_simple_option(packet->options, DHCP_DNS_SERVER, ip4_addr_get_u32(&netif->ip_addr));
#elif LWIP_IPV4 /* now only for IPv4 */
	add_simple_option(packet->options, DHCP_SUBNET, ip4_addr_get_u32(ip_2_ip4(&netif->netmask)));
	add_simple_option(packet->options, DHCP_ROUTER, ip4_addr_get_u32(ip_2_ip4(&netif->gw)));
	add_simple_option(packet->options, DHCP_DNS_SERVER, ip4_addr_get_u32(ip_2_ip4(&netif->ip_addr)));
#else
	#error "IPv4 not support!"
#endif

#else
	add_simple_option(packet.options, DHCP_LEASE_TIME, htonl(lease_time_align));
#endif
	curr = server_config.options;
	while (curr) {
		if (curr->data[OPT_CODE] != DHCP_LEASE_TIME)
#ifdef DHCPD_HEAP_REPLACE_STACK
			add_option_string(packet->options, curr->data);
#else
			add_option_string(packet.options, curr->data);
#endif
		curr = curr->next;
	}
#ifdef DHCPD_HEAP_REPLACE_STACK
	add_bootp_options(packet);

	//addr.s_addr = packet->yiaddr;
	DHCPD_LOG(LOG_INFO, "sending ACK to %s", inet_ntoa(packet->yiaddr));
	int ret = 0;
	if (send_packet(packet, 0) < 0)
		ret = -1;

	add_lease(packet->chaddr, packet->yiaddr, lease_time_align);

	if (packet)
		free(packet);
	return ret;
#else
	add_bootp_options(&packet);

	//addr.s_addr = packet.yiaddr;
	DHCPD_LOG(LOG_INFO, "sending ACK to %s", inet_ntoa(packet->yiaddr));

	if (send_packet(&packet, 0) < 0)
		return -1;

	add_lease(packet.chaddr, packet.yiaddr, lease_time_align);
	return 0;
#endif
}


int send_inform(struct dhcpMessage *oldpacket)
{
#ifdef DHCPD_HEAP_REPLACE_STACK
	struct dhcpMessage *packet;
#else
	struct dhcpMessage packet;
#endif
	struct option_set *curr;
#ifdef DHCPD_HEAP_REPLACE_STACK
	packet = calloc(1,sizeof(struct dhcpMessage));
	if (!packet)
		DHCPD_LOG(LOG_ERR, "Calloc failed...%d",__LINE__);
	init_packet(packet, oldpacket, DHCPACK);
#else
	init_packet(&packet, oldpacket, DHCPACK);
#endif

	curr = server_config.options;
	while (curr) {
		if (curr->data[OPT_CODE] != DHCP_LEASE_TIME)
#ifdef DHCPD_HEAP_REPLACE_STACK
			add_option_string(packet->options, curr->data);
#else
			add_option_string(packet.options, curr->data);
#endif
		curr = curr->next;
	}
#ifdef DHCPD_HEAP_REPLACE_STACK
	add_bootp_options(packet);

	int ret = send_packet(packet, 0);
	if (packet)
		free(packet);
	return ret;
#else
	add_bootp_options(&packet);

	return send_packet(&packet, 0);
#endif
}



