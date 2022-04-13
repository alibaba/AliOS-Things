#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__

#include "lwip/netif.h"

typedef struct {
    ip4_addr_t ip;
    ip4_addr_t netmask;
    ip4_addr_t gw;
} ip_info_t;


int lwip_tcpip_init(void);

#endif
