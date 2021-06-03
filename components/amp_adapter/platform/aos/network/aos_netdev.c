/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */
#include "amp_platform.h"
#include "aos_network.h"
#include "netdev.h"

#define WIFI_DEV "wifi0"

void amp_netdev_set_default(char* netdev_name)
{
    struct netdev* netdev;

    if(netdev_name == NULL) {
        return ;
    }

    netdev = netdev_get_by_name(netdev_name);
    if(netdev == NULL) {
        return ;
    }

    netdev_set_default(netdev);
}

int amp_netdev_set_up(char* netdev_name)
{
    struct netdev* netdev;

    if(netdev_name == NULL) {
        return -1;
    }

    netdev = netdev_get_by_name(netdev_name);
    if(netdev == NULL) {
        return -1;
    }

    return netdev_set_up(netdev);
}

int amp_netdev_set_down(char* netdev_name)
{
    struct netdev* netdev;

    if(netdev_name == NULL) {
        return -1;
    }

    netdev = netdev_get_by_name(netdev_name);
    if(netdev == NULL) {
        return -1;
    }

    return netdev_set_down(netdev);
}

int amp_netdev_dhcp_enabled(char* netdev_name, bool is_enabled)
{
    struct netdev* netdev;

    if(netdev_name == NULL) {
        return -1;
    }

    netdev = netdev_get_by_name(netdev_name);
    if(netdev == NULL) {
        return -1;
    }

    return netdev_dhcp_enabled(netdev, is_enabled);
}

int amp_netdev_set_ipaddr(char* netdev_name, const ip_addr_t* ip_addr)
{
    struct netdev* netdev;

    if(netdev_name == NULL) {
        return -1;
    }

    netdev = netdev_get_by_name(netdev_name);
    if(netdev == NULL) {
        return -1;
    }

    return netdev_set_ipaddr(netdev, ip_addr);
}

int amp_netdev_set_netmask(char* netdev_name, const ip_addr_t* netmask)
{
    struct netdev* netdev;

    if(netdev_name == NULL) {
        return -1;
    }

    netdev = netdev_get_by_name(netdev_name);
    if(netdev == NULL) {
        return -1;
    }

    return netdev_set_netmask(netdev, netmask);
}

int amp_netdev_set_gw(char* netdev_name, const ip_addr_t* gw)
{
    struct netdev* netdev;

    if(netdev_name == NULL) {
        return -1;
    }

    netdev = netdev_get_by_name(netdev_name);
    if(netdev == NULL) {
        return -1;
    }

    return netdev_set_gw(netdev, gw);
}

int amp_netdev_set_dns_server(char* netdev_name, uint8_t dns_num, const ip_addr_t* dns_server)
{
    struct netdev* netdev;

    if(netdev_name == NULL) {
        return -1;
    }

    netdev = netdev_get_by_name(netdev_name);
    if(netdev == NULL) {
        return -1;
    }

    return netdev_set_dns_server(netdev, dns_num, dns_server);
}

void amp_netdev_set_status_callback(char* netdev_name, netdev_callback_fn status_callback)
{
    struct netdev* netdev;

    if(netdev_name == NULL) {
        return;
    }

    netdev = netdev_get_by_name(netdev_name);
    if(netdev == NULL) {
        return;
    }

    netdev_set_status_callback(netdev, status_callback);
}

