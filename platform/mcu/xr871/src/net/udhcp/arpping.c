/*
 * arpping.c
 *
 * Mostly stolen from: dhcpcd - DHCP client daemon
 * by Yoichi Hariguchi <yoichi@fore.com>
 */
#ifdef DHCPD_LWIP
#include <lwip/sockets.h>
#include <lwip/icmp.h>
#include <lwip/inet_chksum.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/if_ether.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#endif

#ifndef DHCPD_TIMEALT
#include <sys/time.h>
#else
#include "dhcp_time.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <unistd.h>

#include "dhcpd.h"
#include "debug.h"
#include "arpping.h"

/* args:	yiaddr - what IP to ping
 *		ip - our ip
 *		mac - our arp address
 *		interface - interface to use
 * retn: 	1 addr free
 *		0 addr used
 *		-1 error
 */

#ifdef DHCPD_LWIP
#define ETH_P_ARP		ETHTYPE_ARP
#define ARPHRD_ETHER		1
#define ETH_P_IP		ETHTYPE_IP

#define ARPOP_REQUEST		ARP_REQUEST
#endif

/* FIXME: match response against chaddr */
int   arpping(u_int32_t yiaddr, u_int32_t ip, unsigned char *mac, char *interface)
{
	int	timeout = 2;
	int 	optval = 1;
	int	s;			/* socket */
	int	rv = 1;			/* return value */
	//struct sockaddr addr;		/* for interface name */
#ifdef DHCPD_ICMPPING
	char icmp_buf[50];
	struct icmp_echo_hdr *icmp_hdr = (struct icmp_echo_hdr *)icmp_buf;
#else
	struct arpMsg	arp;
#endif
	fd_set		fdset;
	struct timeval	tm;
	time_t		prevTime;
#ifdef DHCPD_ICMPPING
	struct sockaddr_in sl;
#else
	struct sockaddr_ll sl;
#endif
#ifdef DHCPD_ICMPPING
	if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
#else
	if ((s = socket (AF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) == -1) {
#endif
		DHCPD_LOG(LOG_ERR, "arp:Could not open raw socket");
		return -1;
	}

	if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) == -1) {
		DHCPD_LOG(LOG_ERR, "arp:Could not setsocketopt on raw socket");
#ifdef DHCPD_LWIP
		closesocket(s);
#else
		close(s);
#endif
		return -1;
	}
	DHCPD_LOG(LOG_INFO, "arp : check ip %s\n", inet_ntoa(yiaddr));
	/* send arp request */
#ifdef DHCPD_ICMPPING
	memset(icmp_buf, 0, sizeof(icmp_buf));
#else
	memset(&arp, 0, sizeof(arp));
#endif
#ifndef DHCPD_ICMPPING
#ifdef DHCPD_LWIP
	memcpy(arp.ethhdr.dest.addr, MAC_BCAST_ADDR, 6);	/* MAC DA */
	memcpy(arp.ethhdr.src.addr, mac, 6);		/* MAC SA */
	arp.ethhdr.type = htons(ETH_P_ARP);		/* protocol type (Ethernet) */
#else
	memcpy(arp.ethhdr.h_dest, MAC_BCAST_ADDR, 6);	/* MAC DA */
	memcpy(arp.ethhdr.h_source, mac, 6);		/* MAC SA */
	arp.ethhdr.h_proto = htons(ETH_P_ARP);		/* protocol type (Ethernet) */
#endif
#endif
#ifdef DHCPD_ICMPPING
	ICMPH_TYPE_SET(icmp_hdr, ICMP_ECHO);
	ICMPH_CODE_SET(icmp_hdr, 0);
	icmp_hdr->chksum = 0;
	icmp_hdr->id = 0;
	icmp_hdr->seqno = 0;
	icmp_hdr->chksum = inet_chksum(icmp_hdr, sizeof(struct icmp_echo_hdr));
#else
	arp.htype = htons(ARPHRD_ETHER);		/* hardware type */
	arp.ptype = htons(ETH_P_IP);			/* protocol type (ARP message) */
	arp.hlen = 6;					/* hardware address length */
	arp.plen = 4;					/* protocol address length */
	arp.operation = htons(ARPOP_REQUEST);		/* ARP op code */
#endif
#ifndef DHCPD_ICMPPING
#ifndef DHCPD_LWIP
	*((u_int *) arp.sInaddr) = ip;			/* source IP address */
#else
	memcpy(arp.sInaddr,(char *)&ip, 4);
#endif
	memcpy(arp.sHaddr, mac, 6);			/* source hardware address */
#ifndef DHCPD_LWIP
	*((u_int *) arp.tInaddr) = yiaddr;		/* target IP address */
#else
	memcpy(arp.tInaddr,(char *)&yiaddr, 4);
#endif
#endif
	//memset(&addr, 0, sizeof(addr));
	//strcpy(addr.sa_data, interface);
	//if (sendto(s, &arp, sizeof(arp), 0, &addr, sizeof(addr)) < 0)
	memset(&sl, 0, sizeof(sl));
#ifdef DHCPD_ICMPPING
	sl.sin_len = sizeof(sl);
	sl.sin_family = AF_INET;
	sl.sin_addr.s_addr = yiaddr;
#else
	sl.sll_family = AF_PACKET;
    	//sl.sll_addr = MAC_SOURCE;
    	//sl.sll_halen = ETH_ALEN;
	sl.sll_ifindex = 0x2;
#endif
#ifdef DHCPD_ICMPPING
	if (sendto(s, icmp_buf, sizeof(icmp_buf), 0, (struct sockaddr*)&sl, sizeof(sl)) < 0)
#else
	if (sendto(s, &arp, sizeof(arp), 0, (struct sockaddr*)&sl, sizeof(sl)) < 0)
#endif
		rv = 0;

	/* wait arp reply, and check it */
	tm.tv_usec = 0;
	time(&prevTime);

	while (timeout > 0) {
		FD_ZERO(&fdset);
		FD_SET(s, &fdset);
		tm.tv_sec = timeout;
		if (select(s + 1, &fdset, (fd_set *) NULL, (fd_set *) NULL, &tm) < 0) {
			DEBUG(LOG_ERR, "Error on ARPING request: %s", strerror(errno));
			if (errno != EINTR) rv = 0;
		} else if (FD_ISSET(s, &fdset)) {
#ifdef DHCPD_ICMPPING
			if (recv(s, icmp_buf, sizeof(icmp_buf), 0) < 0 ) rv = 0;
			struct ip_hdr *iphdr = (struct ip_hdr *)icmp_buf;
			icmp_hdr = (struct icmp_echo_hdr *)(icmp_buf + IPH_HL(iphdr) * 4);
#else
			if (recv(s, &arp, sizeof(arp), 0) < 0 ) rv = 0;
			u_int *tsInaddr = (u_int *)arp.sInaddr;
#endif
#ifdef DHCPD_ICMPPING
			if (iphdr->src.addr == yiaddr && icmp_hdr->type == ICMP_ER) {
#else
			if (arp.operation == htons(ARP_REPLY) &&
				bcmp(arp.tHaddr, mac, 6) == 0 &&
				*(/*(u_int *) arp.sInaddr*/tsInaddr) == yiaddr) {
#endif
				DEBUG(LOG_INFO, "Valid arp reply receved for this address");
				rv = 0;
				break;
			}
		}
		timeout -= time(NULL) - prevTime;
		time(&prevTime);

	}
#ifdef DHCPD_LWIP
	closesocket(s);
#else
	close(s);
#endif
	DEBUG(LOG_INFO, "%salid arp replies for this address", rv ? "No v" : "V");
	return rv;
}
