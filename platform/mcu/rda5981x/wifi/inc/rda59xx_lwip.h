#ifndef _RDA59XX_LWIP_H
#define _RDA59XX_LWIP_H

#include "rda_def.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"

extern const ip_addr_t * rda59xx_get_netif_ip(struct netif *netif);
extern r_void rda59xx_netif_input(struct netif *netif, r_u8 *data, r_u32 len);
extern r_void rda59xx_sta_netif_init(struct netif *netif);
extern r_void rda59xx_ap_netif_init(struct netif *netif);
extern r_void rda59xx_sta_netif_link_failed();
extern r_void rda59xx_netif_link_down(r_u32 netif);
extern r_void rda59xx_netif_link_up(r_u32 netif);

#endif /* _RDA59XX_LWIP_H */

