/* vi: set sw=4 ts=4: */
/*
 * DHCP server client/server socket creation
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
#include "udhcp_common.h"
#if DHCPD_SUPPORT
//#include <net/if.h>
#include <string.h>
#include "netstack.h"

int FAST_FUNC udhcp_read_interface(const char *interface, int *ifindex, uint32_t *nip, uint8_t *mac)
{
    
    netdev_getipv4info(interface, nip, NULL, NULL);
    netdev_getmacaddr(interface, mac); 
    return 0;
    #if 0
	/* char buffer instead of bona-fide struct avoids aliasing warning */
	char ifr_buf[sizeof(struct ifreq)];
	struct ifreq *const ifr = (void *)ifr_buf;

	int fd;
	struct sockaddr_in *our_ip;

	memset(ifr, 0, sizeof(*ifr));
	fd = xsocket(AF_INET, SOCK_RAW, IPPROTO_RAW);

	ifr->ifr_addr.sa_family = AF_INET;
	strncpy_IFNAMSIZ(ifr->ifr_name, interface);
	if (nip) {
		if (ioctl_or_perror(fd, SIOCGIFADDR, ifr,
			"is interface %s up and configured?", interface)
		) {
			close(fd);
			return -1;
		}
		our_ip = (struct sockaddr_in *) &ifr->ifr_addr;
		*nip = our_ip->sin_addr.s_addr;
		log1("IP %s", inet_ntoa(our_ip->sin_addr));
	}

	if (ifindex) {
		if (ioctl_or_warn(fd, SIOCGIFINDEX, ifr) != 0) {
			close(fd);
			return -1;
		}
		log1("Adapter index %d", ifr->ifr_ifindex);
		*ifindex = ifr->ifr_ifindex;
	}

	if (mac) {
		if (ioctl_or_warn(fd, SIOCGIFHWADDR, ifr) != 0) {
			close(fd);
			return -1;
		}
		memcpy(mac, ifr->ifr_hwaddr.sa_data, 6);
		log1("MAC %02x:%02x:%02x:%02x:%02x:%02x",
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	}

	close(fd);
	return 0;
    #endif
}

/* 1. None of the callers expects it to ever fail */
/* 2. ip was always INADDR_ANY */
int FAST_FUNC udhcp_listen_socket(/*uint32_t ip,*/ int port, const char *inf)
{
	int fd;	
	struct sockaddr_in addr;
#ifndef CONFIG_OS_RHINO
    struct ifreq ifr;
#endif
    //int recv_timeout = 10;

	log1("Opening listen socket on *:%d %s\r\n", port, ""/*inf*/);

	fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

#ifndef CONFIG_OS_RHINO
    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, IF1_NAME);
    setsockopt(fd, SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof(ifr)); 
#endif
	memset((void*)&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htons(INADDR_ANY);
	/* addr.sin_addr.s_addr = ip; - all-zeros is INADDR_ANY */
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
		log1("bind port %d failed\r\n", port);

	//setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,(void *)&recv_timeout,sizeof(recv_timeout));

	return fd;
}
#endif //#if DHCPD_SUPPORT
