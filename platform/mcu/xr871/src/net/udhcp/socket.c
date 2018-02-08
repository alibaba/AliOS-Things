/*
 * socket.c -- DHCP server client/server socket creation
 *
 * udhcp client/server
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

#include <sys/types.h>

#ifdef DHCPD_LWIP
#include <lwip/sockets.h>
#else
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <features.h>
#endif

#ifndef	DHCPD_FREERTOS
#include <errno.h>
#endif

#include <unistd.h>
#include <string.h>

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
#include <lwip/netif.h>
#include "dhcpd.h"
#endif
#include "debug.h"

#ifdef DHCPD_USRCFG
#include "dhcpd_cfg.h"
#endif

int read_interface(char *interface, int *ifindex, u_int32_t *addr, unsigned char *arp)
{
#ifndef DHCPD_LWIP
	int fd;
	struct ifreq ifr;
	struct sockaddr_in *our_ip;

	memset(&ifr, 0, sizeof(struct ifreq));
	if((fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) >= 0) {
		ifr.ifr_addr.sa_family = AF_INET;
		strcpy(ifr.ifr_name, interface);

		if (addr) {
			if (ioctl(fd, SIOCGIFADDR, &ifr) == 0) {
				our_ip = (struct sockaddr_in *) &ifr.ifr_addr;
				*addr = our_ip->sin_addr.s_addr;
				DEBUG(LOG_INFO, "%s (our ip) = %s", ifr.ifr_name, inet_ntoa(our_ip->sin_addr));
			} else {
				DHCPD_LOG(LOG_ERR, "SIOCGIFADDR failed, is the interface up and configured?: %s",
						strerror(errno));
				return -1;
			}
		}

		if (ioctl(fd, SIOCGIFINDEX, &ifr) == 0) {
			DEBUG(LOG_INFO, "adapter index %d", ifr.ifr_ifindex);
			*ifindex = ifr.ifr_ifindex;
		} else {
			DHCPD_LOG(LOG_ERR, "SIOCGIFINDEX failed!: %s", strerror(errno));
			return -1;
		}
		if (ioctl(fd, SIOCGIFHWADDR, &ifr) == 0) {
			memcpy(arp, ifr.ifr_hwaddr.sa_data, 6);
			DEBUG(LOG_INFO, "adapter hardware address %02x:%02x:%02x:%02x:%02x:%02x",
				arp[0], arp[1], arp[2], arp[3], arp[4], arp[5]);
		} else {
			DHCPD_LOG(LOG_ERR, "SIOCGIFHWADDR failed!: %s", strerror(errno));
			return -1;
		}
	} else {
		DHCPD_LOG(LOG_ERR, "socket failed!: %s", strerror(errno));
		return -1;
	}
	close(fd);
#else
	struct netif *netif = netif_find(interface);
	if (netif != NULL)
		 *ifindex = netif->num;
	else
		return -1;

	 memcpy(addr, &(netif->ip_addr), 4);
	 memcpy(arp, netif->hwaddr, 6);
	 DHCPD_LOG(LOG_INFO, "Obtain addr :%s,hwaddr:[%02x:%02x:%02x:%02x:%02x:%02x]",inet_ntoa(*addr),arp[0],arp[1],arp[2],arp[3],arp[4],arp[5]);
#endif
	return 0;
}


int listen_socket(unsigned int ip, int port, char *inf)
{
#ifndef DHCPD_LWIP
	struct ifreq interface;
#endif
	int fd;
	struct sockaddr_in addr;
	int n = 1;

	DEBUG(LOG_INFO, "Opening listen socket on 0x%08x:%d %s\n", ip, port, inf);
	if ((fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		DEBUG(LOG_ERR, "socket call failed: %s", strerror(errno));
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = ip;

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &n, sizeof(n)) == -1) {
#ifdef DHCPD_LWIP
		closesocket(fd);
#else
		close(fd);
#endif
		return -1;
	}
	if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, (char *) &n, sizeof(n)) == -1) {
#ifdef DHCPD_LWIP
		closesocket(fd);
#else
		close(fd);
#endif
		return -1;
	}
#ifndef DHCPD_LWIP
	strncpy(interface.ifr_ifrn.ifrn_name, inf, IFNAMSIZ);
	if (setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE,(char *)&interface, sizeof(interface)) < 0) {
		close(fd);
		return -1;
	}
#endif
	if (bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1) {
#ifdef DHCPD_LWIP
		closesocket(fd);
#else
		close(fd);
#endif
		return -1;
	}

	return fd;
}

#define ETH_P_IP ETHTYPE_IP
int raw_socket(int ifindex)
{
#if 0
	int fd;
	struct sockaddr_ll sock;

	DEBUG(LOG_INFO, "Opening raw socket on ifindex %d\n", ifindex);
	if ((fd = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_IP))) < 0) {
		DEBUG(LOG_ERR, "socket call failed: %s", strerror(errno));
		return -1;
	}

	sock.sll_family = AF_PACKET;
	sock.sll_protocol = htons(ETH_P_IP);
	sock.sll_ifindex = ifindex;
	if (bind(fd, (struct sockaddr *) &sock, sizeof(sock)) < 0) {
		DEBUG(LOG_ERR, "bind call failed: %s", strerror(errno));
#ifdef DHCPD_LWIP
		closesocket(fd);
#else
		close(fd);
#endif
		return -1;
	}

	return fd;

#endif
	return 0;
}

