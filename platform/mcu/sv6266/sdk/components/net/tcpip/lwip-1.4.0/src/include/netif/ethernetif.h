#ifndef __NETIF_ETHERNETIF_H__
#define __NETIF_ETHERNETIF_H__
#include "lwip/err.h" 
#include "lwip/netif.h"

#ifdef CONFIG_OS_RHINO
err_t ethernetif_input(struct netif *pif, void *dat, u32 len);
static err_t low_level_output(struct netif *netif, struct pbuf *p);
#else
err_t ethernetif_input(struct netif *pif, void *dat, u32 len) ATTRIBUTE_SECTION_FAST;
static err_t low_level_output(struct netif *netif, struct pbuf *p) ATTRIBUTE_SECTION_FAST;
#endif
err_t ethernetif_init(struct netif *netif);

#endif //#ifndef __NETIF_ETHERNETIF_H__
