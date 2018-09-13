#include <unistd.h>
#include <string.h>

#ifndef DHCPD_LWIP
#include <netinet/in.h>
#include <sys/socket.h>
#include <features.h>
#endif
#include <sys/types.h>


#if __GLIBC__ >=2 && __GLIBC_MINOR >= 1
#include <netpacket/packet.h>
#include <net/ethernet.h>
#elif __LINUX
#include <asm/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#endif

#ifdef DHCPD_LWIP
#include "netif/etharp.h"
#include "lwip/sockets.h"
#include "lwip/inet_chksum.h"
#include "lwip/ip_addr.h"
#include "lwipopts.h"
#include "stdlib.h"
#endif

#include "packet.h"
#include "debug.h"
#include "dhcpd.h"
#include "options.h"


void init_header(struct dhcpMessage *packet, char type)
{
	memset(packet, 0, sizeof(struct dhcpMessage));
	switch (type) {
	case DHCPDISCOVER:
	case DHCPREQUEST:
	case DHCPRELEASE:
	case DHCPINFORM:
		packet->op = BOOTREQUEST;
		break;
	case DHCPOFFER:
	case DHCPACK:
	case DHCPNAK:
		packet->op = BOOTREPLY;
	}
	packet->htype = ETH_10MB;
	packet->hlen = ETH_10MB_LEN;
	packet->cookie = htonl(DHCP_MAGIC);
	packet->options[0] = DHCP_END;
	add_simple_option(packet->options, DHCP_MESSAGE_TYPE, type);
}


/* read a packet from socket fd, return -1 on read error, -2 on packet error */
int get_packet(struct dhcpMessage *packet, int fd)
{
	int bytes;
	int i;
	const char broken_vendors[][8] = {
		"MSFT 98",
		""
	};
	char unsigned *vendor;

	memset(packet, 0, sizeof(struct dhcpMessage));
#ifndef DHCPD_LWIP
	bytes = read(fd, packet, sizeof(struct dhcpMessage));
#else
	struct sockaddr_in from;
	int from_len = sizeof(from);
	bytes = recvfrom(fd, packet, sizeof(struct dhcpMessage), 0, (struct sockaddr*)&from, (socklen_t *)&from_len);
#endif
	if (bytes < 0) {
		DEBUG(LOG_INFO, "couldn't read on listening socket, ignoring");
		return -1;
	}
	if (ntohl(packet->cookie) != DHCP_MAGIC) {
		if ((strncmp((char *)packet,"<cancel>", strlen("<cancel>")) == 0) &&
				(from.sin_addr.s_addr == htonl(INADDR_LOOPBACK))) {
			DHCPD_LOG(LOG_ALERT, "received stop server message");
			return -3;
		}
		DHCPD_LOG(LOG_ERR, "received bogus message, ignoring");
		return -2;
	}
	DEBUG(LOG_INFO, "Received a packet");

	if (packet->op == BOOTREQUEST && (vendor = get_option(packet, DHCP_VENDOR))) {
		for (i = 0; broken_vendors[i][0]; i++) {
			if (vendor[OPT_LEN - 2] == (unsigned char) strlen(broken_vendors[i]) &&
			    !strncmp((char *)vendor, broken_vendors[i], vendor[OPT_LEN - 2])) {
			    	DEBUG(LOG_INFO, "broken client (%s), forcing broadcast",
			    		broken_vendors[i]);
			    	packet->flags |= htons(BROADCAST_FLAG);
			}
		}
	}


	return bytes;
}


u_int16_t checksum(void *addr, int count)
{
	/* Compute Internet Checksum for "count" bytes
	 *         beginning at location "addr".
	 */
	register int32_t sum = 0;
	u_int16_t *source = (u_int16_t *) addr;

	while (count > 1)  {
		/*  This is the inner loop */
		sum += *source++;
		count -= 2;
	}

	/*  Add left-over byte, if any */
	if (count > 0) {
		/* Make sure that the left-over byte is added correctly both
		 * with little and big endian hosts */
		u_int16_t tmp = 0;
		*(unsigned char *) (&tmp) = * (unsigned char *) source;
		sum += tmp;
	}
	/*  Fold 32-bit sum to 16 bits */
	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	return ~sum;
}
#define ETH_P_IP		ETHTYPE_IP

int _low_level_dhcp_send(struct dhcpMessage *payload, u_int32_t source_ip, int source_port,
		   u_int32_t dest_ip, int dest_port, unsigned char *dest_arp, int ifindex)
{
	struct netif *netif = netif_find(server_config.interface);
    struct pbuf *p;
    struct eth_hdr *ethhdr;
    struct ip_hdr *iphdr;
    struct udp_hdr *udphdr;

    p = pbuf_alloc(PBUF_LINK,
                   SIZEOF_ETH_HDR + sizeof(struct ip_hdr)
                   + sizeof(struct udp_hdr) + sizeof(struct dhcpMessage),
                   PBUF_RAM);
    if (p == NULL) {
		DHCPD_LOG(LOG_ERR, "Calloc failed...%s %d", __FUNCTION__, __LINE__);
		return -1;
	}

    ethhdr = (struct eth_hdr *)p->payload;
    iphdr  = (struct ip_hdr *)((char *)ethhdr + SIZEOF_ETH_HDR);
    udphdr = (struct udp_hdr *)((char *)iphdr + sizeof(struct ip_hdr));

    ETHADDR32_COPY(&ethhdr->dest, dest_arp);
    ETHADDR16_COPY(&ethhdr->src, server_config.arp);
    ethhdr->type = PP_HTONS(ETHTYPE_IP);

    iphdr->src.addr  = source_ip;
    iphdr->dest.addr = dest_ip;

    IPH_VHL_SET(iphdr, 4, IP_HLEN / 4);
    IPH_TOS_SET(iphdr, 0x00);
    IPH_LEN_SET(iphdr, htons(IP_HLEN + sizeof(struct udp_hdr) + sizeof(struct dhcpMessage)));
    IPH_ID_SET(iphdr, htons(2));
    IPH_OFFSET_SET(iphdr, 0);
    IPH_TTL_SET(iphdr, 255);
    IPH_PROTO_SET(iphdr, IP_PROTO_UDP);
    IPH_CHKSUM_SET(iphdr, 0);
    IPH_CHKSUM_SET(iphdr, inet_chksum(iphdr, IP_HLEN));

    udphdr->src = htons(source_port);
    udphdr->dest = htons(dest_port);
    udphdr->len = htons(sizeof(struct udp_hdr) + sizeof(struct dhcpMessage));
    udphdr->chksum = 0;

    memcpy((char *)udphdr + sizeof(struct udp_hdr),
           payload, sizeof(struct dhcpMessage));

    netif->linkoutput(netif, p);
    pbuf_free(p);

    return 0;
}


/* Constuct a ip/udp header for a packet, and specify the source and dest hardware address */
int raw_packet(struct dhcpMessage *payload, u_int32_t source_ip, int source_port,
		   u_int32_t dest_ip, int dest_port, unsigned char *dest_arp, int ifindex)
{
#ifndef DHCPD_LOW_LEVEL
	int fd;
	int result;
	struct sockaddr_ll dest;
#ifdef DHCPD_HEAP_REPLACE_STACK
	uint32_t interval;
	struct udp_dhcp_packet *packet;
	packet = malloc(sizeof(*packet));
	if (!packet)
		DHCPD_LOG(LOG_ERR, "Calloc failed...%d",__LINE__);
#else
	struct udp_dhcp_packet packet;
#endif

	if ((fd = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_IP))) < 0) {
		DEBUG(LOG_ERR, "socket call failed: %s", strerror(errno));
		return -1;
	}

	memset(&dest, 0, sizeof(dest));
#ifndef DHCPD_HEAP_REPLACE_STACK
	memset(&packet, 0, sizeof(packet));
#else
	memset(packet, 0, sizeof(*packet));
#endif

	dest.sll_family = AF_PACKET;
	dest.sll_protocol = htons(ETH_P_IP);
	dest.sll_ifindex = ifindex;
	dest.sll_halen = 6;
	memcpy(dest.sll_addr, dest_arp, 6);
	if (bind(fd, (struct sockaddr *)&dest, sizeof(struct sockaddr_ll)) < 0) {
		DEBUG(LOG_ERR, "bind call failed: %s", strerror(errno));
#ifndef DHCPD_LWIP
		close(fd);
#else
		closesocket(fd);
#ifdef DHCPD_HEAP_REPLACE_STACK
		if (packet) {
			free(packet);
			packet = NULL;
		}
#endif
#endif
		return -1;
	}
#ifdef DHCPD_LWIP
	packet->ip._proto = IPPROTO_UDP;
	interval = source_ip;
	memcpy(&(packet->ip.src), &interval, 4);

	interval = dest_ip;
	memcpy(&(packet->ip.dest), &interval, 4);
#else
	packet.ip.protocol = IPPROTO_UDP;
	packet.ip.saddr = source_ip;
	packet.ip.daddr = dest_ip;
#endif
#ifdef DHCPD_LWIP
	packet->udp.src = htons(source_port);
	packet->udp.dest = htons(dest_port);
	packet->udp.len = htons(sizeof(packet->udp) + sizeof(struct dhcpMessage)); /* cheat on the psuedo-header */
#else
	packet.udp.source = htons(source_port);
	packet.udp.dest = htons(dest_port);
	packet.udp.len = htons(sizeof(packet.udp) + sizeof(struct dhcpMessage)); /* cheat on the psuedo-header */
#endif

#ifdef DHCPD_LWIP
	packet->ip._len = packet->udp.len;
	memcpy(&(packet->data), payload, sizeof(struct dhcpMessage));
	packet->udp.chksum = checksum(packet, sizeof(struct udp_dhcp_packet));
#else
	packet.ip.tot_len = packet.udp.len;
	memcpy(&(packet.data), payload, sizeof(struct dhcpMessage));
	packet.udp.check = checksum(&packet, sizeof(struct udp_dhcp_packet));
#endif


#ifdef DHCPD_LWIP
	packet->ip._len = htons(sizeof(struct udp_dhcp_packet));
	#define IPVERSION 4
	packet->ip._v_hl = (IPVERSION << 4) |(sizeof(packet->ip) >> 2);
	//packet.ip._v_hl = sizeof(packet.ip) >> 2;
	//IPH_V(packet.ip) = IPVERSION;
	///**********/packet.ip.version = IPVERSION;
	packet->ip._ttl = IP_DEFAULT_TTL;
	packet->ip._chksum = checksum(&(packet->ip), sizeof(packet->ip));
#else
	packet.ip.tot_len = htons(sizeof(struct udp_dhcp_packet));
	packet.ip.ihl = sizeof(packet.ip) >> 2;
	packet.ip.version = IPVERSION;
	packet.ip.ttl = IPDEFTTL;
	packet.ip.check = checksum(&(packet.ip), sizeof(packet.ip));
#endif
#ifdef DHCPD_HEAP_REPLACE_STACK
	result = sendto(fd, packet, sizeof(struct udp_dhcp_packet), 0, (struct sockaddr *) &dest, sizeof(dest));
#else
	result = sendto(fd, &packet, sizeof(struct udp_dhcp_packet), 0, (struct sockaddr *) &dest, sizeof(dest));
#endif
	if (result <= 0) {
		DEBUG(LOG_ERR, "write on socket failed: %s", strerror(errno));
	}
#ifdef DHCPD_HEAP_REPLACE_STACK
	if (packet)
		free(packet);
#endif
#ifdef DHCPD_LWIP
	closesocket(fd);
#else
	close(fd);
#endif
	return result;
#else

	return _low_level_dhcp_send(payload, source_ip, source_port,
								dest_ip, dest_port, dest_arp, ifindex);

#endif
}


/* Let the kernel do all the work for packet generation */
int kernel_packet(struct dhcpMessage *payload, u_int32_t source_ip, int source_port,
		   u_int32_t dest_ip, int dest_port)
{
	int n = 1;
	int fd, result;
	struct sockaddr_in client;

	if ((fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		return -1;

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &n, sizeof(n)) == -1)
		return -1;

	memset(&client, 0, sizeof(client));
	client.sin_family = AF_INET;
	client.sin_port = htons(source_port);
	client.sin_addr.s_addr = source_ip;

	if (bind(fd, (struct sockaddr *)&client, sizeof(struct sockaddr)) == -1)
		return -1;

	memset(&client, 0, sizeof(client));
	client.sin_family = AF_INET;
	client.sin_port = htons(dest_port);
	client.sin_addr.s_addr = dest_ip;

	if (connect(fd, (struct sockaddr *)&client, sizeof(struct sockaddr)) == -1)
		return -1;

	result = write(fd, payload, sizeof(struct dhcpMessage));
#ifdef DHCPD_LWIP
	closesocket(fd);
#else
	close(fd);
#endif
	return result;
}

