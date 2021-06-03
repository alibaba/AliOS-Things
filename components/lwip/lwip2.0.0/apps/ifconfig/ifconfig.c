/*
 * Copyright (C) 2017-2020 Alibaba Group Holding Limited
 */

#include "lwip/opt.h"
#include "lwip/netif.h"
#include "lwip/apps/ifconfig.h"
#include <lwip/def.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <aos/cli.h>

int ifconfig(int argc,char *argv[])
{
	struct netif *net_if = NULL;

    if ((argc == 0) || (argc >= 1) && (strcmp(argv[0], "status") == 0)) {
        NETIF_FOREACH(net_if) {
            if (netif_is_up(net_if) && !ip_addr_isany(&net_if->ip_addr) && netif_is_link_up(net_if)) {
                char address[16];
                char gateway[16];
                char netmask[16];
                inet_ntoa_r(net_if->ip_addr, address, sizeof(address));
                inet_ntoa_r(net_if->gw, gateway, sizeof(gateway));
                inet_ntoa_r(net_if->netmask, netmask, sizeof(netmask));
                aos_cli_printf("%c%c%d up, "
                            "address:%s gateway:%s netmask:%s\n",
                            net_if->name[0], net_if->name[1], net_if->num,
                            address, gateway, netmask);
            } else {
                aos_cli_printf("%c%c%d down\n",
                            net_if->name[0], net_if->name[1], net_if->num);
            }
#if LWIP_IPV6
            if (netif_is_link_up(net_if)) {
                int i;
                for (i = 0; i < LWIP_IPV6_NUM_ADDRESSES; ++i) {
                    if (ip6_addr_isvalid(netif_ip6_addr_state(net_if, i))) {
                        aos_cli_printf("IPv6 addr [%d]: %s\n",
                        i, inet6_ntoa(net_if->ip6_addr[i]));
                    }
                }
            }
#endif /* LWIP_IPV6 */
        }
    } else if (strcmp(argv[1], "up") == 0) {
        aos_cli_printf("ifconfig up is not support\n");
    } else if (strcmp(argv[1], "down") == 0) {
        aos_cli_printf("ifconfig down is not support\n");
    } else {
        aos_cli_printf("Invalid command\n");
    }

    return 0;
}
