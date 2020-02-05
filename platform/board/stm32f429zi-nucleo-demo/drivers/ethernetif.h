#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__


#include "lwip/err.h"
#include "lwip/netif.h"

typedef struct {
    ip4_addr_t ip;
    ip4_addr_t netmask;
    ip4_addr_t gw;
} tcpip_ip_info_t;


extern tcpip_ip_info_t eth_ip_info;

/* Exported types ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
err_t ethernetif_init(struct netif *netif);
int lwip_tcpip_init(void);
void post_ip_addr(tcpip_ip_info_t ip);

#endif
