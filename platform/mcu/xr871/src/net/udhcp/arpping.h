/*
 * arpping .h
 */

#ifndef ARPPING_H
#define ARPPING_H

#ifdef DHCPD_LWIP
#include "netif/etharp.h"
#else
#include <netinet/if_ether.h>
#include <net/if_arp.h>
#include <net/if.h>
#include <netinet/in.h>
#endif

#ifdef DHCPD_USRCFG
#include "dhcpd_cfg.h"
#endif
struct arpMsg {
#ifdef DHCPD_LWIP
	struct eth_hdr ethhdr;	 		/* Ethernet header */
#else
	struct ethhdr ethhdr;	 		/* Ethernet header */
#endif
	u_short htype;				/* hardware type (must be ARPHRD_ETHER) */
	u_short ptype;				/* protocol type (must be ETH_P_IP) */
	u_char  hlen;				/* hardware address length (must be 6) */
	u_char  plen;				/* protocol address length (must be 4) */
	u_short operation;			/* ARP opcode */
	u_char  sHaddr[6];			/* sender's hardware address */
	u_char  sInaddr[4];			/* sender's IP address */
	u_char  tHaddr[6];			/* target's hardware address */
	u_char  tInaddr[4];			/* target's IP address */
	u_char  pad[18];			/* pad for min. Ethernet payload (60 bytes) */
};

/* function prototypes */
int arpping(u_int32_t yiaddr, u_int32_t ip, unsigned char *arp, char *interface);

#endif
