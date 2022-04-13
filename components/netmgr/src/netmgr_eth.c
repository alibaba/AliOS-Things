/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <uservice/uservice.h>
#include <uservice/eventid.h>
#include <netmgr.h>
#include <netmgr_service.h>

#include <devices/hal/ethernet_impl.h>

typedef struct {
    netmgr_hdl_t hdl;
    eth_setting_t config;
} param_eth_setting_t;

static int netmgr_eth_provision(netmgr_dev_t *node)
{
    aos_dev_t *dev = node->dev;
    netdev_driver_t *drv = dev->drv;
    eth_driver_t *eth_drv = (eth_driver_t *)drv->link_ops;
    netmgr_subscribe(EVENT_ETH_LINK_UP);
    netmgr_subscribe(EVENT_ETH_LINK_DOWN);
    netmgr_subscribe(EVENT_ETH_EXCEPTION);

    eth_drv->start(dev);
    return 0;
}

static int netmgr_eth_unprovision(netmgr_dev_t *node)
{
    aos_dev_t *dev = node->dev;

    hal_net_set_link_down(dev);

    device_close(dev);
    return 0;
}

static int netmgr_eth_reset(netmgr_dev_t *node)
{
    return netmgr_eth_provision(node);
}

static int netmgr_eth_info(netmgr_dev_t *node)
{
    aos_dev_t *dev = node->dev;
    netdev_driver_t *drv = dev->drv;
    eth_driver_t *eth_drv = (eth_driver_t *)drv->link_ops;
    eth_drv->ifconfig(dev);
    return 0;
}

/** todo add EVENT_NET_GOT_IP */
int netmgr_eth_service(struct netmgr_uservice *netmgr, rpc_t *rpc)
{
    int ret = 0;

    switch (rpc->cmd_id) {
    case EVENT_ETH_LINK_UP:
        ret = netmgr_start_dhcp(netmgr, "eth");
        break;
    case EVENT_ETH_EXCEPTION:
    case EVENT_ETH_LINK_DOWN:
        netmgr_set_gotip("eth", 0);
        event_publish(EVENT_NETMGR_NET_DISCON, NULL);
        break;

    case API_ETH_CONFIG: {
        // parameters was checked by API
#ifdef CONFIG_KV_SMART
        param_eth_setting_t *param = rpc_get_point(rpc);
        netmgr_dev_t *node = (netmgr_dev_t *)param->hdl;

        eth_setting_t *eth_setting = &param->config;
        eth_setting_t *config = &node->config.eth_config;
        config->duplex = eth_setting->duplex;
        aos_kv_setint(KV_ETH_DUPLEX, config->duplex);
        config->speed = eth_setting->duplex;
        aos_kv_setint(KV_ETH_SPEED, config->speed);
#endif
        ret = 0;

        break;
    }
    }

    return ret;
}


netmgr_dev_t * netmgr_eth_init(struct netmgr_uservice *netmgr)
{
    netmgr_dev_t *node = NULL;

    int ival = 0;
    // get eth configuration
#ifdef CONFIG_KV_SMART
    ival = netmgr_kv_getint(KV_ETH_EN);
#else
    ival = 1;
#endif

    if (ival == 1) {
        node = (netmgr_dev_t *)aos_zalloc(sizeof(netmgr_dev_t));

        if (node) {
            //eth_setting_t *config = &node->config.eth_config;
            //init netif and register netif to lwip
            node->dev = device_open_id("eth", 0);
            aos_assert(node->dev);
            node->provision = netmgr_eth_provision;
            node->unprovision = netmgr_eth_unprovision;
            node->info = netmgr_eth_info;
            node->reset = netmgr_eth_reset;
            strcpy(node->name, "eth");
#ifdef CONFIG_KV_SMART
            node->dhcp_en = netmgr_kv_getint(KV_DHCP_EN);
            char *str = netmgr_kv_get(KV_IP_ADDR);
            ip4addr_aton(str, (ip4_addr_t *)ip_2_ip4(&node->ipaddr));
            str = netmgr_kv_get(KV_IP_NETMASK);
            ip4addr_aton(str, (ip4_addr_t *)ip_2_ip4(&node->netmask));
            str = netmgr_kv_get(KV_IP_GW);
            ip4addr_aton(str, (ip4_addr_t *)ip_2_ip4(&node->gw));
#endif
            slist_add_tail((slist_t *)node, &netmgr->dev_list);
        }

        netmgr_reg_srv_func(API_ETH_CONFIG, netmgr_eth_service);
        netmgr_reg_srv_func(EVENT_ETH_LINK_UP, netmgr_eth_service);
        netmgr_reg_srv_func(EVENT_ETH_LINK_DOWN, netmgr_eth_service);
        netmgr_reg_srv_func(EVENT_ETH_EXCEPTION, netmgr_eth_service);
    }

    return (netmgr_hdl_t)node;

}

/**
 * @brief net manage init for eth
 * @return NULL on error
 */
netmgr_hdl_t netmgr_dev_eth_init()
{
    return netmgr_eth_init(&netmgr_svc);
}

/**
 * @brief  net manage config for wifi device
 * @param  [in] hdl
 * @param  [in] duplex
 * @param  [in] speed
 * @return 0 on success
 */
int netmgr_config_eth(netmgr_hdl_t hdl, eth_mac_duplex_e duplex, eth_mac_speed_e speed)
{
    int ret = -1;
    param_eth_setting_t param;
    eth_setting_t *config = &param.config;

    if ((hdl == NULL)
        || ((duplex < ETH_MAC_DUPLEX_HALF) || (duplex > ETH_MAC_DUPLEX_FULL))
        || ((speed < ETH_MAC_SPEED_10M) || (speed > ETH_MAC_SPEED_1000M))) {
        return -EINVAL;
    }

    param.hdl = hdl;
    config->duplex = duplex;
    config->speed = speed;
    uservice_call_sync(netmgr_svc.srv, API_ETH_CONFIG, &param, &ret, sizeof(int));

    return ret;
}
