/**
 * @file netmgr_wifi.h
 * netmgr Wi-Fi API header file.
 *
 * @version   V2.0
 * @date      2020-11-22
 * @copyright Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef NETMGR_WIFI_H
#define NETMGR_WIFI_H

#include <stdint.h>
#include <stdbool.h>
#include <netmgr.h>

#if defined(__cplusplus)
extern "C"
{
#endif

typedef struct {
    uint8_t dhcp;     /* DHCP mode: @ref DHCP_Disable, @ref DHCP_Client, @ref DHCP_Server. */
    char    ip[16];   /* Local IP address on the target wlan interface: @ref wlanInterfaceTypedef. */
    char    gate[16]; /* Router IP address on the target wlan interface: @ref wlanInterfaceTypedef. */
    char    mask[16]; /* Netmask on the target wlan interface: @ref wlanInterfaceTypedef. */
    char    dns[16];  /* DNS server IP address. */
    char    mac[16];  /* MAC address, example: "C89346112233". */
    char    broadcastip[16];
} netmgr_ip_stat_t;

typedef enum netmgr_eth_ip_mode_e {
    NETMGR_ETH_IP_MODE_AUTO,             /* Use dhcp to get ip address */
    NETMGR_ETH_IP_MODE_STATIC,           /* Use static ip address */
} netmgr_eth_ip_mode_t;


int netmgr_eth_init(netmgr_hdl_t hdl);
int netmgr_eth_set_ifconfig(netmgr_ifconfig_info_t *info);
int netmgr_eth_get_ip_stat(char *ip_addr, char *mask, char *gw, char *dns_server, bool dhcp_en);
netmgr_conn_state_t netmgr_eth_get_stat(void);

#if defined(__cplusplus)
}
#endif

#endif /* NETMGR_WIFI_H */
