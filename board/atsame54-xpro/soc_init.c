/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include "hal/hal.h"
#include "k_config.h"
#include "board.h"
#if defined(DEV_SAL_WILC1000)
#include <lwip/tcpip.h>
#endif

#if defined(DEV_SAL_WILC1000)
struct netif lwip_netif;
static void tcpip_init_done(void *arg)
{
#if LWIP_IPV4
    ip4_addr_t ipaddr, netmask, gw;
    memset(&ipaddr, 0, sizeof(ipaddr));
    memset(&netmask, 0, sizeof(netmask));
    memset(&gw, 0, sizeof(gw));
    //netif_add(&lwip_netif, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input);
#endif

#if LWIP_IPV6
#if !LWIP_IPV4
    netif_add(&lwip_netif, NULL, ethernetif_init, tcpip_input);
#endif
    netif_create_ip6_linklocal_address(&lwip_netif, 1);
    lwip_netif.ip6_autoconfig_enabled = 1;
#endif

}

void same5x_wilc1000_init(void)
{
   
    /*ethernet if init*/
    tcpip_init(NULL, NULL);
    tcpip_init_done(NULL);
    LOG("TCP/IP initialized.");
}
#endif