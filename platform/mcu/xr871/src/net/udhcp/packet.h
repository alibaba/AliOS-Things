#ifndef _PACKET_H
#define _PACKET_H

#ifdef DHCPD_LWIP
#include <lwip/ip.h>
#include <lwip/udp.h>
#else
#include <netinet/udp.h>
#include <netinet/ip.h>
#endif
#ifdef DHCPD_USRCFG
#include "dhcpd_cfg.h"
#endif

struct dhcpMessage {
	u_int8_t op;
	u_int8_t htype;
	u_int8_t hlen;
	u_int8_t hops;
	u_int32_t xid;
	u_int16_t secs;
	u_int16_t flags;
	u_int32_t ciaddr;
	u_int32_t yiaddr;
	u_int32_t siaddr;
	u_int32_t giaddr;
	u_int8_t chaddr[16];
	u_int8_t sname[64];
	u_int8_t file[128];
	u_int32_t cookie;
	u_int8_t options[308]; /* 312 - cookie */
};

struct udp_dhcp_packet {
#ifdef DHCPD_LWIP
	struct ip_hdr ip;
	struct udp_hdr udp;
#else
	struct iphdr ip;
	struct udphdr udp;
#endif
	struct dhcpMessage data;
};

void init_header(struct dhcpMessage *packet, char type);
int get_packet(struct dhcpMessage *packet, int fd);
u_int16_t checksum(void *addr, int count);
int raw_packet(struct dhcpMessage *payload, u_int32_t source_ip, int source_port,
		   u_int32_t dest_ip, int dest_port, unsigned char *dest_arp, int ifindex);
int kernel_packet(struct dhcpMessage *payload, u_int32_t source_ip, int source_port,
		   u_int32_t dest_ip, int dest_port);


#endif
