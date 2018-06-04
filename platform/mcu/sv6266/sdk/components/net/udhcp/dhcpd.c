/* vi: set sw=4 ts=4: */
/*
 * udhcp server
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

//usage:#define udhcpd_trivial_usage
//usage:       "[-fS] [-I ADDR]" IF_FEATURE_UDHCP_PORT(" [-P N]") " [CONFFILE]"
//usage:#define udhcpd_full_usage "\n\n"
//usage:       "DHCP server\n"
//usage:     "\n	-f	Run in foreground"
//usage:     "\n	-S	Log to syslog too"
//usage:     "\n	-I ADDR	Local address"
//usage:	IF_FEATURE_UDHCP_PORT(
//usage:     "\n	-P N	Use port N (default 67)"
//usage:	)

//#include <syslog.h>
#include <string.h>
#include "arch/sys_arch.h"
#include "udhcp_common.h"
#include "dhcpc.h"
#include "udhcpd.h"
#include "osal.h"
#include "netstack.h"

#if DHCPD_SUPPORT
/* globals variable */
struct sockaddr *dest_sin = NULL;
socklen_t dest_length = 0;
struct dyn_lease *g_leases = NULL;

static struct dhcp_packet pdhcp_pkt;
static u32 dhcp_pkt_len = 0;
static bool dhcpd_start = false;
static bool dhcpd_run = false;
static bool dhcpd_end = false;

#define OS_DHCPD_TASK_PRIO OS_TASK_PRIO1
/* dhcpd task */
typedef void (*TASK_FUNC)(void *);

#if 0	//being added in rtos.h
typedef struct task_info_st
{
    const char   *task_name;
    OsMsgQ       qevt;
    u32          qlength;
    u32          prio;
    u32          stack_size;  /* unit: 16 */
    void       *args;
    TASK_FUNC    task_func;
} task_info;
#endif

static void  udhcpd_start_task(void *arg);

struct task_info_st st_dhcpd_task[] =
{
    { "dhcpd_task",  (OsMsgQ)0, 4, OS_DHCPD_TASK_PRIO, 64, NULL, udhcpd_start_task},
};


int dhcpd_lease_get_api(struct dyn_lease *lease, int *count);

extern void FAST_FUNC load_config(struct server_config_t* psconfig);
extern ssize_t FAST_FUNC safe_write(int fd, const void *buf, size_t count);
extern uint32_t ssv_time(void *arg);

void dhcp_pkt_debug(uint8_t* buffer, uint32_t len)
{
	uint32_t i = 0;

	for (i = 0; i < len; i++)
    {
		UDHCP_DEBUG("%02x ", buffer[i]);
		if ((i + 1) % 16 == 0)
		{
			UDHCP_DEBUG("\r\n");
		}
	}
	UDHCP_DEBUG("\r\n");
}

void dhcpd_lease_show()
{
	int i = 0;
    int count = 0;
    struct dyn_lease lease[10];
    int ret = 0;

    LOG_PRINTF("----------------------------------------\r\n");
    LOG_PRINTF("|        MAC         |          IP     |\r\n");
    LOG_PRINTF("----------------------------------------\r\n");

    count = 10;

    ret = dhcpd_lease_get_api(lease, &count);
    if (ret == 0)
    {
    	for (i = 0; i < count; i++)
        {
#ifndef CONFIG_OS_RHINO
            LOG_PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x] --- [%d.%d.%d.%d]\r\n", lease[i].lease_mac[0], lease[i].lease_mac[1], lease[i].lease_mac[2],
                                lease[i].lease_mac[3], lease[i].lease_mac[4], lease[i].lease_mac[5],
                                 ip4_addr1_16(&lease[i].lease_nip),
                      ip4_addr2_16(&lease[i].lease_nip),
                      ip4_addr3_16(&lease[i].lease_nip),
                      ip4_addr4_16(&lease[i].lease_nip));
#endif
    	}
    }
}

int dhcpd_lease_get_api(struct dyn_lease *lease, int *count)
{
    unsigned i = 0;
    int size = 0;

    if (g_leases == NULL)
    {
        return -1;
    }

    if (!lease || !count)
    {
        return -1;
    }

    size = *count;
    *count = 0;

	for (i = 0; i < server_config.max_leases; i++)
    {
		if (g_leases[i].lease_nip != 0)
		{
            if ((*count) > size)
            {
                break;
            }

           memcpy((void*)(lease + *count), (void*)(&g_leases[i]), sizeof(struct dyn_lease));
           (*count)++;
		}
	}

    return 0;
}


/* Send a packet to a specific mac address and ip address by creating our own ip packet */
static void send_packet_to_client(int s, struct dhcp_packet *dhcp_pkt, int force_broadcast)
{
	const uint8_t *chaddr;
	uint32_t ciaddr;
	unsigned padding;
	struct sockaddr_in dest_addr;

	// Was:
	//if (force_broadcast) { /* broadcast */ }
	//else if (dhcp_pkt->ciaddr) { /* unicast to dhcp_pkt->ciaddr */ }
	//else if (dhcp_pkt->flags & htons(BROADCAST_FLAG)) { /* broadcast */ }
	//else { /* unicast to dhcp_pkt->yiaddr */ }
	// But this is wrong: yiaddr is _our_ idea what client's IP is
	// (for example, from lease file). Client may not know that,
	// and may not have UDP socket listening on that IP!
	// We should never unicast to dhcp_pkt->yiaddr!
	// dhcp_pkt->ciaddr, OTOH, comes from client's request packet,
	// and can be used.

    memset((void*)&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(CLIENT_PORT);

	if (force_broadcast
	 || (dhcp_pkt->flags & htons(BROADCAST_FLAG))
	 || dhcp_pkt->ciaddr == 0
	) {
		log1("Broadcasting packet to client");
		ciaddr = INADDR_BROADCAST;
		chaddr = MAC_BCAST_ADDR;
		dest_addr.sin_addr.s_addr = INADDR_BROADCAST;
	} else {
		log1("Unicasting packet to client ciaddr");
		ciaddr = dhcp_pkt->ciaddr;
		chaddr = (uint8_t *)dhcp_pkt->chaddr;
		dest_addr.sin_addr.s_addr = ciaddr;
	}

	memcpy((void*)dest_sin, (void*)&dest_addr, dest_length);
	padding = DHCP_OPTIONS_BUFSIZE - 1 - udhcp_end_option((void*)dhcp_pkt->options);
	if (padding > DHCP_SIZE - 300)
		padding = DHCP_SIZE - 300;

	dhcp_pkt_len = DHCP_SIZE - padding;

	memset((void*)&pdhcp_pkt, 0, DHCP_SIZE);
	memcpy((void*)&pdhcp_pkt , (void*)dhcp_pkt, dhcp_pkt_len);

	dhcp_pkt_debug((u8*)dhcp_pkt, dhcp_pkt_len);

    #if 0
	udhcp_send_raw_packet(dhcp_pkt,
		/*src*/ server_config.server_nip, SERVER_PORT,
		/*dst*/ ciaddr, CLIENT_PORT, chaddr,
		server_config.ifindex);
    #endif
}

/* Send a packet to gateway_nip using the kernel ip stack */
static void send_packet_to_relay(struct dhcp_packet *dhcp_pkt)
{
	log1("Forwarding packet to relay");

	udhcp_send_kernel_packet(dhcp_pkt,
			server_config.server_nip, SERVER_PORT,
			dhcp_pkt->gateway_nip, SERVER_PORT);
}

static void send_packet(int s, struct dhcp_packet *dhcp_pkt, int force_broadcast)
{
	if (dhcp_pkt->gateway_nip)
		send_packet_to_relay(dhcp_pkt);
	else
		send_packet_to_client(s, dhcp_pkt, force_broadcast);
}

static void init_packet(struct dhcp_packet *packet, struct dhcp_packet *oldpacket, char type)
{
	/* Sets op, htype, hlen, cookie fields
	 * and adds DHCP_MESSAGE_TYPE option */
	udhcp_init_header(packet, type);

	packet->xid = oldpacket->xid;
	memcpy((void*)packet->chaddr, (void*)oldpacket->chaddr, sizeof(oldpacket->chaddr));
	packet->flags = oldpacket->flags;
	packet->gateway_nip = oldpacket->gateway_nip;
	packet->ciaddr = oldpacket->ciaddr;
	udhcp_add_simple_option(packet, DHCP_SERVER_ID, server_config.server_nip);
}

/* Fill options field, siaddr_nip, and sname and boot_file fields.
 * TODO: teach this code to use overload option.
 */
static void add_server_options(struct dhcp_packet *packet)
{
	struct option_set *curr = server_config.options;

	while (curr) {
		if (curr->data[OPT_CODE] != DHCP_LEASE_TIME)
			udhcp_add_binary_option(packet, curr->data);
		curr = curr->next;
	}

	packet->siaddr_nip = server_config.siaddr_nip;

	if (server_config.sname)
		//strncpy((char*)packet->sname, server_config.sname, sizeof(packet->sname) - 1);
        memcpy((void*)packet->sname,(void*)server_config.sname,sizeof(packet->sname) - 1);
	if (server_config.boot_file)
		//strncpy((char*)packet->file, server_config.boot_file, sizeof(packet->file) - 1);
        memcpy((void*)packet->file,(void*)server_config.boot_file,sizeof(packet->file) - 1);
}

static uint32_t select_lease_time(struct dhcp_packet *packet)
{
	uint32_t lease_time_sec = server_config.max_lease_sec;
	uint8_t *lease_time_opt = udhcp_get_option(packet, DHCP_LEASE_TIME);
	if (lease_time_opt) {
		move_from_unaligned32(lease_time_sec, lease_time_opt);
		lease_time_sec = ntohl(lease_time_sec);
		if (lease_time_sec > server_config.max_lease_sec)
			lease_time_sec = server_config.max_lease_sec;
		if (lease_time_sec < server_config.min_lease_sec)
			lease_time_sec = server_config.min_lease_sec;
	}
	return lease_time_sec;
}

void  clearlease_bymac(u8 *addr)
{
	struct dyn_lease *lease;
    lease = find_lease_by_mac(addr);
    if(lease)
        memset(lease, 0, sizeof(struct dyn_lease));
}

/* We got a DHCP DISCOVER. Send an OFFER. */
/* NOINLINE: limit stack usage in caller */
static NOINLINE void send_offer(int s, struct dhcp_packet *oldpacket,
		uint32_t static_lease_nip,
		struct dyn_lease *lease,
		uint8_t *requested_ip_opt)
{
	struct dhcp_packet *packet = NULL;
	uint32_t lease_time_sec;
	struct in_addr addr;

    packet = (struct dhcp_packet *)OS_MemAlloc(sizeof(struct dhcp_packet));
    if(NULL==packet)
    {
        LOG_PRINTF("%s(%d):malloc fail\r\n",__FUNCTION__,__LINE__);
        return;
    }

	init_packet(packet, oldpacket, DHCPOFFER);

	/* If it is a static lease, use its IP */
	packet->yiaddr = static_lease_nip;
	/* Else: */
	if (!static_lease_nip) {
		/* We have no static lease for client's chaddr */
		uint32_t req_nip;
		const char *p_host_name;

		if (lease) {
			/* We have a dynamic lease for client's chaddr.
			 * Reuse its IP (even if lease is expired).
			 * Note that we ignore requested IP in this case.
			 */
			packet->yiaddr = lease->lease_nip;
		}
		/* Or: if client has requested an IP */
		else if (requested_ip_opt != NULL
		 /* (read IP) */
		 && (move_from_unaligned32(req_nip, requested_ip_opt), 1)
		 /* and the IP is in the lease range */
		 && ntohl(req_nip) >= server_config.start_ip
		 && ntohl(req_nip) <= server_config.end_ip
		 /* and */
		 && (  !(lease = find_lease_by_nip(req_nip)) /* is not already taken */
		    || is_expired_lease(lease) /* or is taken, but expired */
		    )
		) {
			packet->yiaddr = req_nip;
		}
		else {
			/* Otherwise, find a free IP */
			packet->yiaddr = find_free_or_expired_nip((void*)oldpacket->chaddr);
		}

		if (!packet->yiaddr) {
			bb_error_msg("1 no free IP addresses. OFFER abandoned");
            OS_MemFree((void *)packet);
			return;
		}
		/* Reserve the IP for a short time hoping to get DHCPREQUEST soon */
		p_host_name = (const char*) udhcp_get_option(oldpacket, DHCP_HOST_NAME);
		lease = add_lease((void*)packet->chaddr, packet->yiaddr,
				server_config.offer_time,
				(void*)p_host_name,
				p_host_name ? (unsigned char)p_host_name[OPT_LEN - OPT_DATA] : 0
		);
		if (!lease) {
			bb_error_msg("2 no free IP addresses. OFFER abandoned");
            OS_MemFree((void *)packet);
			return;
		}
	}

	lease_time_sec = select_lease_time(oldpacket);
	udhcp_add_simple_option(packet, DHCP_LEASE_TIME, htonl(lease_time_sec));
	udhcp_add_simple_option(packet, DHCP_SUBNET, htonl(server_config.subnet));
	udhcp_add_simple_option(packet, DHCP_ROUTER, htonl(server_config.gw));
	udhcp_add_simple_option(packet, DHCP_DNS_SERVER, htonl(server_config.dns));
	add_server_options(packet);

	addr.s_addr = packet->yiaddr;
	bb_info_msg("Sending OFFER of %s", inet_ntoa(addr));
	/* send_packet emits error message itself if it detects failure */
	send_packet(s, packet, /*force_bcast:*/ 0);
    OS_MemFree((void *)packet);
}

/* NOINLINE: limit stack usage in caller */
static NOINLINE void send_NAK(int s, struct dhcp_packet *oldpacket)
{
	struct dhcp_packet *packet = NULL;

    packet = (struct dhcp_packet *)OS_MemAlloc(sizeof(struct dhcp_packet));
    if(NULL==packet)
    {
        LOG_PRINTF("%s(%d):malloc fail\r\n",__FUNCTION__,__LINE__);
        return;
    }
	init_packet(packet, oldpacket, DHCPNAK);

	log1("Sending NAK");
	send_packet(s, packet, /*force_bcast:*/ 1);
    OS_MemFree((void *)packet);
}

/* NOINLINE: limit stack usage in caller */
static NOINLINE void send_ACK(int s, struct dhcp_packet *oldpacket, uint32_t yiaddr)
{
	struct dhcp_packet *packet = NULL;
	uint32_t lease_time_sec;
	struct in_addr addr;
	const char *p_host_name;

    packet = (struct dhcp_packet *)OS_MemAlloc(sizeof(struct dhcp_packet));
    if(NULL==packet)
    {
        LOG_PRINTF("%s(%d):malloc fail\r\n",__FUNCTION__,__LINE__);
        return;
    }
	init_packet(packet, oldpacket, DHCPACK);
	packet->yiaddr = yiaddr;

	lease_time_sec = select_lease_time(oldpacket);
	udhcp_add_simple_option(packet, DHCP_LEASE_TIME, htonl(lease_time_sec));
	udhcp_add_simple_option(packet, DHCP_SUBNET, htonl(server_config.subnet));
	udhcp_add_simple_option(packet, DHCP_ROUTER, htonl(server_config.gw));
	udhcp_add_simple_option(packet, DHCP_DNS_SERVER, htonl(server_config.dns));

	add_server_options(packet);

	addr.s_addr = yiaddr;
	bb_info_msg("Sending ACK to %s", inet_ntoa(addr));
	send_packet(s, packet, /*force_bcast:*/ 0);

	p_host_name = (const char*) udhcp_get_option(oldpacket, DHCP_HOST_NAME);
	add_lease((void*)packet->chaddr, packet->yiaddr,
		lease_time_sec,
		(void*)p_host_name,
		p_host_name ? (unsigned char)p_host_name[OPT_LEN - OPT_DATA] : 0
	);
	if (ENABLE_FEATURE_UDHCPD_WRITE_LEASES_EARLY) {
		/* rewrite the file with leases at every new acceptance */
		write_leases();
	}

    OS_MemFree((void *)packet);
}

/* NOINLINE: limit stack usage in caller */
static NOINLINE void send_inform(int s, struct dhcp_packet *oldpacket)
{
	struct dhcp_packet *packet = NULL;

	/* "If a client has obtained a network address through some other means
	 * (e.g., manual configuration), it may use a DHCPINFORM request message
	 * to obtain other local configuration parameters.  Servers receiving a
	 * DHCPINFORM message construct a DHCPACK message with any local
	 * configuration parameters appropriate for the client without:
	 * allocating a new address, checking for an existing binding, filling
	 * in 'yiaddr' or including lease time parameters.  The servers SHOULD
	 * unicast the DHCPACK reply to the address given in the 'ciaddr' field
	 * of the DHCPINFORM message.
	 * ...
	 * The server responds to a DHCPINFORM message by sending a DHCPACK
	 * message directly to the address given in the 'ciaddr' field
	 * of the DHCPINFORM message.  The server MUST NOT send a lease
	 * expiration time to the client and SHOULD NOT fill in 'yiaddr'."
	 */
//TODO: do a few sanity checks: is ciaddr set?
//Better yet: is ciaddr == IP source addr?
    packet = (struct dhcp_packet *)OS_MemAlloc(sizeof(struct dhcp_packet));
    if(NULL==packet)
    {
        LOG_PRINTF("%s(%d):malloc fail\r\n",__FUNCTION__,__LINE__);
        return;
    }
	init_packet(packet, oldpacket, DHCPACK);
	add_server_options(packet);

	send_packet(s, packet, /*force_bcast:*/ 0);
    OS_MemFree((void *)packet);
}

void  udhcpd_run(void *arg)
{
	int send_size = 0;
	int recv_size = 0;
	struct option_set *option = NULL;
	uint32_t num_ips = 0;
	u8 *state = NULL;
	u8 *requested_ip_opt = NULL;
	u32 requested_nip = 0;
	u8 *server_id_opt = NULL;
	u32 static_lease_nip = 0;
    int server_socket = -1;
	struct dyn_lease *lease, fake_lease;
    struct dhcp_packet *packet = NULL;

    UDHCP_DEBUG("udhcpd run\r\n");

	load_config(&server_config);

	/*setup the maximum lease time*/
	option = udhcp_find_option(server_config.options, DHCP_LEASE_TIME);
	if (option) {
		UDHCP_DEBUG("udhcp_find_option function return value %p\r\n", option);
		move_from_unaligned32(server_config.max_lease_sec, option->data + OPT_DATA);
		server_config.max_lease_sec = ntohl(server_config.max_lease_sec);
	} else
		server_config.max_lease_sec = DEFAULT_LEASE_TIME;

	/* Sanity check */
	num_ips = server_config.end_ip - server_config.start_ip + 1;
	if (server_config.max_leases > num_ips)
    {
		UDHCP_DEBUG("max_leases=%u is too big, setting to %u\r\n",
			(unsigned)server_config.max_leases, num_ips);
		server_config.max_leases = num_ips;
	}

	/*setup the server_mac and server_ip*/
	if (udhcp_read_interface(IF1_NAME,
				&server_config.ifindex,
				(server_config.server_nip == 0 ? &server_config.server_nip : NULL),
				server_config.server_mac))
    {
		return;
	}

    if (dest_sin)
    {
        memset((void *) dest_sin, 0, sizeof(struct sockaddr));
    }

	g_leases = (struct dyn_lease *)OS_MemAlloc(server_config.max_leases * sizeof(g_leases[0]));
    if (g_leases == NULL)
    {
        UDHCP_DEBUG("OS_MemAlloc failed!\r\n");
        return;
    }

    memset((char *)g_leases, 0, server_config.max_leases * sizeof(g_leases[0]));

	server_socket = udhcp_listen_socket(SERVER_PORT,NULL);

	if (server_socket < 0){
        UDHCP_DEBUG("udhcp_listen_socket failed!\r\n");
	} else {
		packet = (struct dhcp_packet *)OS_MemAlloc(sizeof(struct dhcp_packet));
        if (packet == NULL)
        {
            OS_MemFree((void *)g_leases);
            UDHCP_DEBUG("OS_MemAlloc failed!\r\n");
            return;
        }

        memset((char *)packet, 0, sizeof(struct dhcp_packet));
		while (dhcpd_run == true)
        {
			recv_size = udhcp_recv_kernel_packet(packet, server_socket);
			if (recv_size < 0){
				//UDHCP_DEBUG("Server Recieve Data Failed!\n");
			} else {
				UDHCP_DEBUG("recvfrom function recv data size %d %d\r\n", recv_size, sizeof(struct dhcp_packet));
				if (packet->hlen != 6) {
					UDHCP_DEBUG("MAC length != 6, ignoring packet\r\n");
				}
				if (packet->op != BOOTREQUEST) {
					UDHCP_DEBUG("not a REQUEST, ignoring packet\r\n");
				}

				/* Get message type if present */
				state = udhcp_get_option(packet, DHCP_MESSAGE_TYPE);
				if (state == NULL || state[0] < DHCP_MINTYPE || state[0] > DHCP_MAXTYPE) {
					UDHCP_DEBUG("no or bad message type option, ignoring packet\r\n");
				}

				/* Get SERVER_ID if present */
				server_id_opt = udhcp_get_option(packet, DHCP_SERVER_ID);
				if (server_id_opt) {
					u32 server_id_network_order;
					move_from_unaligned32(server_id_network_order, server_id_opt);
					if (server_id_network_order != server_config.server_nip) {
						/* client talks to somebody else */
						UDHCP_DEBUG("server ID doesn't match, ignoring\r\n");
					}
				}

				/* Look for a static/dynamic lease */
				static_lease_nip = get_static_nip_by_mac(server_config.static_leases, (void*)packet->chaddr);
				if (static_lease_nip) {
					UDHCP_DEBUG("Found static lease: %x\r\n", static_lease_nip);
					memcpy((void*)&fake_lease.lease_mac, (void*)packet->chaddr, 6);
					fake_lease.lease_nip = static_lease_nip;
					fake_lease.expires = 0;
					lease = &fake_lease;
				} else {
					UDHCP_DEBUG("static lease not found, Find the lease that matches MAC: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
							packet->chaddr[0], packet->chaddr[1],packet->chaddr[2],
							packet->chaddr[3],packet->chaddr[4],packet->chaddr[5]);
					lease = find_lease_by_mac((void*)packet->chaddr);
				}

				/* Get REQUESTED_IP if present */
				requested_ip_opt = udhcp_get_option(packet, DHCP_REQUESTED_IP);
				if (requested_ip_opt) {
					move_from_unaligned32(requested_nip, requested_ip_opt);
				}

				switch (state[0]) {
					case DHCPDISCOVER:
						UDHCP_DEBUG("Received DISCOVER\r\n");
						send_offer(server_socket, packet, static_lease_nip, lease, requested_ip_opt);
						send_size = safe_write(server_socket, &pdhcp_pkt, dhcp_pkt_len);
						if (send_size < 0)
							UDHCP_DEBUG("safe_write function send data failed\r\n");
						else
							UDHCP_DEBUG("safe_write function send data size %d\r\n", send_size);
						break;

					case DHCPREQUEST:
						UDHCP_DEBUG("Received REQUEST\r\n");
                        /* RFC 2131:

                        o DHCPREQUEST generated during SELECTING state:

                           Client inserts the address of the selected server in 'server
                           identifier', 'ciaddr' MUST be zero, 'requested IP address' MUST be
                           filled in with the yiaddr value from the chosen DHCPOFFER.

                           Note that the client may choose to collect several DHCPOFFER
                           messages and select the "best" offer.  The client indicates its
                           selection by identifying the offering server in the DHCPREQUEST
                           message.  If the client receives no acceptable offers, the client
                           may choose to try another DHCPDISCOVER message.  Therefore, the
                           servers may not receive a specific DHCPREQUEST from which they can
                           decide whether or not the client has accepted the offer.

                        o DHCPREQUEST generated during INIT-REBOOT state:

                           'server identifier' MUST NOT be filled in, 'requested IP address'
                           option MUST be filled in with client's notion of its previously
                           assigned address. 'ciaddr' MUST be zero. The client is seeking to
                           verify a previously allocated, cached configuration. Server SHOULD
                           send a DHCPNAK message to the client if the 'requested IP address'
                           is incorrect, or is on the wrong network.

                           Determining whether a client in the INIT-REBOOT state is on the
                           correct network is done by examining the contents of 'giaddr', the
                           'requested IP address' option, and a database lookup. If the DHCP
                           server detects that the client is on the wrong net (i.e., the
                           result of applying the local subnet mask or remote subnet mask (if
                           'giaddr' is not zero) to 'requested IP address' option value
                           doesn't match reality), then the server SHOULD send a DHCPNAK
                           message to the client.

                           If the network is correct, then the DHCP server should check if
                           the client's notion of its IP address is correct. If not, then the
                           server SHOULD send a DHCPNAK message to the client. If the DHCP
                           server has no record of this client, then it MUST remain silent,
                           and MAY output a warning to the network administrator. This
                           behavior is necessary for peaceful coexistence of non-
                           communicating DHCP servers on the same wire.

                           If 'giaddr' is 0x0 in the DHCPREQUEST message, the client is on
                           the same subnet as the server.  The server MUST broadcast the
                           DHCPNAK message to the 0xffffffff broadcast address because the
                           client may not have a correct network address or subnet mask, and
                           the client may not be answering ARP requests.

                           If 'giaddr' is set in the DHCPREQUEST message, the client is on a
                           different subnet.  The server MUST set the broadcast bit in the
                           DHCPNAK, so that the relay agent will broadcast the DHCPNAK to the
                           client, because the client may not have a correct network address
                           or subnet mask, and the client may not be answering ARP requests.

                        o DHCPREQUEST generated during RENEWING state:

                           'server identifier' MUST NOT be filled in, 'requested IP address'
                           option MUST NOT be filled in, 'ciaddr' MUST be filled in with
                           client's IP address. In this situation, the client is completely
                           configured, and is trying to extend its lease. This message will
                           be unicast, so no relay agents will be involved in its
                           transmission.  Because 'giaddr' is therefore not filled in, the
                           DHCP server will trust the value in 'ciaddr', and use it when
                           replying to the client.

                           A client MAY choose to renew or extend its lease prior to T1.  The
                           server may choose not to extend the lease (as a policy decision by
                           the network administrator), but should return a DHCPACK message
                           regardless.

                        o DHCPREQUEST generated during REBINDING state:

                           'server identifier' MUST NOT be filled in, 'requested IP address'
                           option MUST NOT be filled in, 'ciaddr' MUST be filled in with
                           client's IP address. In this situation, the client is completely
                           configured, and is trying to extend its lease. This message MUST
                           be broadcast to the 0xffffffff IP broadcast address.  The DHCP
                           server SHOULD check 'ciaddr' for correctness before replying to
                           the DHCPREQUEST.

                           The DHCPREQUEST from a REBINDING client is intended to accommodate
                           sites that have multiple DHCP servers and a mechanism for
                           maintaining consistency among leases managed by multiple servers.
                           A DHCP server MAY extend a client's lease only if it has local
                           administrative authority to do so.
                        */
						if (!requested_ip_opt) {
							requested_nip = packet->ciaddr;
							if (requested_nip == 0) {
								UDHCP_DEBUG("no requested IP and no ciaddr, ignoring\r\n");
								break;
							}
						}
						if (lease && requested_nip == lease->lease_nip) {
							/* client requested or configured IP matches the lease.
							 * ACK it, and bump lease expiration time. */
							send_ACK(server_socket, packet, lease->lease_nip);
							send_size = safe_write(server_socket, &pdhcp_pkt, dhcp_pkt_len);
							if (send_size < 0)
								UDHCP_DEBUG("safe_write function send data failed\r\n");
							else
								UDHCP_DEBUG("safe_write function send data size %d\r\n", send_size);
							break;
						}

						/* No lease for this MAC, or lease IP != requested IP */
						if (server_id_opt    /* client is in SELECTING state */
						 || requested_ip_opt /* client is in INIT-REBOOT state */
						) {
							/* "No, we don't have this IP for you" */
							send_NAK(server_socket, packet);
							send_size = safe_write(server_socket, &pdhcp_pkt, dhcp_pkt_len);
							if (send_size < 0)
								UDHCP_DEBUG("safe_write function send data failed\r\n");
							else
								UDHCP_DEBUG("safe_write function send data size %d\r\n", send_size);
						} /* else: client is in RENEWING or REBINDING, do not answer */

						break;

					case DHCPDECLINE:
            			/* RFC 2131:
            			 * "If the server receives a DHCPDECLINE message,
            			 * the client has discovered through some other means
            			 * that the suggested network address is already
            			 * in use. The server MUST mark the network address
            			 * as not available and SHOULD notify the local
            			 * sysadmin of a possible configuration problem."
            			 *
            			 * SERVER_ID must be present,
            			 * REQUESTED_IP must be present,
            			 * chaddr must be filled in,
            			 * ciaddr must be 0 (we do not check this)
            			 */
						UDHCP_DEBUG("Received DECLINE\r\n");
						if (server_id_opt
								 && requested_ip_opt
								 && lease  /* chaddr matches this lease */
								 && requested_nip == lease->lease_nip
							) {
							memset((void*)lease->lease_mac, 0, sizeof(lease->lease_mac));
							lease->expires = ssv_time(NULL) + server_config.decline_time;
						}

						break;

					case DHCPRELEASE:
            			/* "Upon receipt of a DHCPRELEASE message, the server
            			 * marks the network address as not allocated."
            			 *
            			 * SERVER_ID must be present,
            			 * REQUESTED_IP must not be present (we do not check this),
            			 * chaddr must be filled in,
            			 * ciaddr must be filled in
            			 */
						UDHCP_DEBUG("Received RELEASE\r\n");
						if (server_id_opt
								 && lease  /* chaddr matches this lease */
								 && packet->ciaddr == lease->lease_nip
							) {
							lease->expires = ssv_time(NULL);
						}

						break;

					case DHCPINFORM:
						UDHCP_DEBUG("Received INFORM\r\n");
						send_inform(server_socket, packet);
						send_size = safe_write(server_socket, &pdhcp_pkt, dhcp_pkt_len);
						if (send_size < 0)
							UDHCP_DEBUG("safe_write function send data failed\r\n");
						else
							UDHCP_DEBUG("safe_write function send data size %d\r\n", send_size);

						break;

					default :
						break;
				}

			}

		}
		OS_MemFree((void*)packet);
	}

    if (g_leases)
    {
	    OS_MemFree((void*)g_leases);
    }

    g_leases = NULL;

	close(server_socket);

    UDHCP_DEBUG("udhcpd off\r\n");
}

void  udhcpd_start_task(void *arg)
{
    OsMsgQEntry msg_evt;
    OsMsgQ mbox = st_dhcpd_task[0].qevt;
    s32 res = 0;

    for(;;)
    {
        res = OS_MsgQDequeue(mbox, &msg_evt, portMAX_DELAY);
        if (res != OS_SUCCESS)
        {
            //UDHCP_DEBUG("Could not receive from the queue!\r\n");
        }
        /* Start dhcpd*/
        else if (msg_evt.MsgData != NULL) {
            if( (*(int *)msg_evt.MsgData) == 1) {
                OS_MemFree(msg_evt.MsgData);
                dhcpd_start = true;
                udhcpd_run(0);
                dhcpd_start = false;
            }
            else if (((*(int *)msg_evt.MsgData) == -1) && (dhcpd_start == false)) {
                OS_MemFree(msg_evt.MsgData);
                break;
            }
        }
    }
    
    //Free dest_sin
    if(dest_sin != NULL)
    {
        OS_MemFree(dest_sin);
        dest_sin = NULL;
    }
    
    //Delete MsgQ
    if (st_dhcpd_task[0].qevt != 0)
    {
        OS_MsgQDelete(st_dhcpd_task[0].qevt);
    }        
    dhcpd_end = true;
    OS_TaskDelete(NULL);
}

int udhcpd_init(void)
{
    OsMsgQ *msgq = NULL;
    s32 qsize = 0;
	s32 rlt = 0;

    if (dest_sin == NULL)
    {
        dest_sin = (struct sockaddr *)OS_MemAlloc(sizeof(struct sockaddr));
    }

    if (dest_sin == NULL)
    {
        UDHCP_DEBUG("OS_MemAlloc failed\r\n");
        //return -1;
        rlt = -1;
        goto exit_error;
    }

    memset((void *) dest_sin, 0, sizeof(struct sockaddr));

    dest_length = sizeof(struct sockaddr);

    msgq = &st_dhcpd_task[0].qevt;
    qsize = (s32)st_dhcpd_task[0].qlength;
    if (OS_MsgQCreate(msgq, qsize) != OS_SUCCESS)
    {
        UDHCP_DEBUG("OS_MsgQCreate failed\r\n");
        //return -1;
        rlt = -1;
        goto exit_error;
    }

    if (OS_TaskCreate(st_dhcpd_task[0].task_func,
                  st_dhcpd_task[0].task_name,
                  st_dhcpd_task[0].stack_size<<4,
                  NULL,
                  st_dhcpd_task[0].prio, NULL) != 1)
    {
        UDHCP_DEBUG("OS_TaskCreate failed\r\n");
//        return -1;
		rlt = -1;
		goto exit_error;
    }

    dhcpd_end = false;
    return 0;

exit_error:
//	printf("<%s>%d rlt=%d\n",__func__,__LINE__,rlt);
	return rlt;
	
}

int udhcpd_start(void)
{
    OsMsgQEntry msg_evt;
    int times = 0;
    int *p_enable = NULL;
	s32 rlt=0;

	//printf("<%s>%d\n",__func__,__LINE__);

    if (dhcpd_start == false)
    {
        dhcpd_run = true;

        times = 10;
        
        p_enable = (int *)OS_MemAlloc(sizeof(int));
        while (p_enable == NULL)
        {            
            if (--times <= 0)
                break;
            else
                OS_TickDelay(1);
            p_enable = (int *)OS_MemAlloc(sizeof(int));
        }
        if (times <= 0)
        {
            dhcpd_run = false;
            UDHCP_DEBUG("udhcpd start failed 0\r\n");
            //return -1;
            rlt = -1;
			goto exit_error;
        }

        msg_evt.MsgCmd = 0;
        msg_evt.MsgData = p_enable;
        *p_enable = 1;
        OS_MsgQEnqueue(st_dhcpd_task[0].qevt, &msg_evt);

        times = 100;
        /* waiting for dhcpd task stop */
        while (dhcpd_start == false)
        {
            OS_MsDelay(100);
            if (--times <= 0)
            {
                break;
            }
        }

        if (times <= 0)
        {
            dhcpd_run = false;
            UDHCP_DEBUG("udhcpd start failed 1\r\n");
            //return -1;
            rlt = -2;
			goto exit_error;
        }
        else
        {
            UDHCP_DEBUG("udhcpd start success\r\n");
        }
    }
    else
    {
        UDHCP_DEBUG("udhcpd already start\r\n");
    }

//	printf("<%s>%d\n",__func__,__LINE__);
    return 0;

exit_error:
	return rlt;
	
}


int udhcpd_stop(void)
{
    int times = 100;
	s32 rlt = 0;

	//printf("<%s>%d\n",__func__,__LINE__);


    if (dhcpd_start == true)
    {
        dhcpd_run = false;

        /* waiting for dhcpd task stop */
        while (dhcpd_start == true)
        {
            OS_MsDelay(500);
            if (--times <= 0)
            {
                break;
            }
        }

        if (times <= 0)
        {
            UDHCP_DEBUG("udhcpd stop failed\r\n");
            //return -1;
            rlt = -1;
			goto exit_error;
        }
        else
        {
            UDHCP_DEBUG("udhcpd stop success\r\n");
        }
    }
    else
    {
        UDHCP_DEBUG("udhcpd already stop\r\n");
    }


//	printf("<%s>%d\n",__func__,__LINE__);
    return 0;

exit_error:
	return rlt;
	
}

int udhcpd_exit(void)
{    
    OsMsgQEntry msg_evt;   
    int ret = 0, times = 0;
    int *p_end = NULL;


//	printf("<%s>%d\n",__func__,__LINE__);
	
    if (dhcpd_start == true)
        ret = udhcpd_stop();
    if (ret != 0)
    {
        UDHCP_DEBUG("udhcpd can not be stop\r\n");
        return -1;
    }
    
    times = 10;
        
    p_end = (int *)OS_MemAlloc(sizeof(int));
    while (p_end == NULL)
    {            
        if (--times <= 0)
            break;
        else
            OS_TickDelay(2);
        p_end = (int *)OS_MemAlloc(sizeof(int));
    }

    if ((times <= 0) && (p_end == NULL))
    {
        dhcpd_run = false;
        UDHCP_DEBUG("udhcpd end failed \r\n");
        return -1;
    }

    msg_evt.MsgCmd = 0;
    msg_evt.MsgData = p_end;
    *p_end = -1;
    OS_MsgQEnqueue(st_dhcpd_task[0].qevt, &msg_evt);
    times = 50;
    while (dhcpd_end == false)
    {
        OS_MsDelay(100);
        if (--times <= 0)
        {
            break;
        }
    }

    if (dhcpd_end == true)
    {
        //Delete thread
        st_dhcpd_task[0].qevt = NULL;
        UDHCP_DEBUG("udhcpd end success\r\n");        
    }
    else
    {
        UDHCP_DEBUG("udhcpd end failed\r\n");
        return -1;
    }

//	printf("<%s>%d\n",__func__,__LINE__);
	
    return 0;
}

#endif //#if DHCPD_SUPPORT
