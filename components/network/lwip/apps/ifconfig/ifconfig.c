/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

#include "lwip/netif.h"
#include "lwip/apps/ifconfig.h"
#include "network/network.h"
#include "network/hal/wifi.h"
#include "lwip/inet.h"


int ifconfig(int argc,char *argv[])
{
	struct netif *net_if;
    hal_wifi_module_t* m;

    m = hal_wifi_get_default_module();
    if(!m) {
        LWIP_DEBUGF(IFCONFIG_DEBUG, ("%s:%d get module failed\n", __func__, __LINE__));
        return -1;
    }

    net_if = hal_wifi_get_netif(m, STATION);
    if(!net_if) {
        LWIP_DEBUGF(IFCONFIG_DEBUG, ("%s:%d get netif failed\n", __func__, __LINE__));
        return -1;
    }

    if ((argc == 0) || (strcmp(argv[0], "status") == 0)) {
	if (netif_is_up(net_if) && !ip_addr_isany(&net_if->ip_addr) && netif_is_link_up(net_if)) {
	    char address[16];
	    char gateway[16];
	    char netmask[16];
	    inet_ntoa_r(net_if->ip_addr, address, sizeof(address));
	    inet_ntoa_r(net_if->gw, gateway, sizeof(gateway));
	    inet_ntoa_r(net_if->netmask, netmask, sizeof(netmask));
            LWIP_DEBUGF(IFCONFIG_DEBUG, ("%c%c%d up, "
		           "address:%s gateway:%s netmask:%s\n",
			    net_if->name[0], net_if->name[1], net_if->num,
			    address, gateway, netmask));
	} else {
            LWIP_DEBUGF(IFCONFIG_DEBUG, ("%c%c%d down\n",
			    net_if->name[0], net_if->name[1], net_if->num));
	}
#if LWIP_IPV6
	if (netif_is_link_up(net_if)) {
	    int i;
	    for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i) {
	        if (ip6_addr_isvalid(netif_ip6_addr_state(net_if, i))) {
                    LWIP_DEBUGF(IFCONFIG_DEBUG, ("IPv6 addr [%d]: %s\n",
			    i, inet6_ntoa(net_if->ip6_addr[i])));
	        }
	    }
	}
#endif /* LWIP_IPV6 */
    } else if (strcmp(argv[0], "up") == 0) {
        LWIP_DEBUGF(IFCONFIG_DEBUG, ("ifconfig up is not support\n"));
    } else if (strcmp(argv[0], "down") == 0) {
        LWIP_DEBUGF(IFCONFIG_DEBUG, ("ifconfig down is not support\n"));
    } else {
        LWIP_DEBUGF(IFCONFIG_DEBUG, ("Invalid command\n"));
    }

    return 0;
}
