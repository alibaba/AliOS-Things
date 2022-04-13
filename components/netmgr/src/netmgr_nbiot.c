/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <uservice/uservice.h>
#include <uservice/eventid.h>
#include <devices/netdrv.h>
#include <netmgr.h>
#include <netmgr_service.h>
#include <devices/hal/nbiot_impl.h>


//static const char *TAG = "netmgr_nbiot";

int netmgr_nbiot_provision(netmgr_dev_t *node)
{
    netmgr_subscribe(EVENT_NBIOT_LINK_UP);
    netmgr_subscribe(EVENT_NBIOT_LINK_DOWN);

    return hal_nbiot_start(node->dev);

}

int netmgr_nbiot_unprovision(netmgr_dev_t *node)
{
    aos_dev_t *dev = node->dev;

    hal_net_set_link_down(dev);
    device_close(dev);

    return 0;

}


int netmgr_nbiot_info(netmgr_dev_t *node)
{
    aos_dev_t  *dev = node->dev;
    netdev_driver_t *drv = dev->drv;
    nbiot_driver_t *nbiot_drv = (nbiot_driver_t *)drv->link_ops;
    net_ops_t  *net_drv = drv->net_ops;
    ip_addr_t ipaddr;
    int csq = 99;
    nbiot_iccid_t ccid;
    int insert = -1;
    int ret;

    memset(&ipaddr, 0, sizeof(ipaddr));
    net_drv->get_ipaddr(dev, &ipaddr, NULL, NULL);

    nbiot_drv->get_csq(dev, &csq);

    ret = nbiot_drv->get_simcard_info(dev, &ccid, &insert);
    printf("\n%s%d\tLink encap:NBIOT\n", drv->drv.name, drv->drv.device_id);
    printf("    \tinet addr:%s\n", ipaddr_ntoa(&ipaddr));
    if(csq == 99 ) {
        printf("    \tSignal Quality: not known\n");
    } else {
        printf("    \tSignal Quality(%d): %d dBm\n", csq, -113 + csq * 2);
    }
    if(ret < 0) {
        printf("    \tSim insert is FAIL\n");
    } else {
        printf("    \tSim insert is %s\n",insert >= 0?"OK":"FAIL");
        printf("    \tSim ccid:%s\n", ccid.iccid);
    }

    return 0;
}

int netmgr_nbiot_service(struct netmgr_uservice *netmgr, rpc_t *rpc)
{
    int ret = 0;

    switch (rpc->cmd_id) {

    case EVENT_NBIOT_LINK_UP: {
        netmgr_dev_t *node = netmgr_find_dev(&netmgr->dev_list, "nbiot");
        aos_dev_t *dev = node->dev;
        netdev_driver_t *drv = dev->drv;
        nbiot_driver_t *nbiot_drv = (nbiot_driver_t *)drv->link_ops;

        if (nbiot_drv->start_nbiot)
            nbiot_drv->start_nbiot(dev);
        //TODO
        event_publish(EVENT_NET_GOT_IP, NULL);

        break;
    }

    case EVENT_NBIOT_LINK_DOWN:
        event_publish(EVENT_NETMGR_NET_DISCON, NULL);

        break;
    }

    return ret;
}

static int netmgr_nbiot_reset(netmgr_dev_t *node)
{
    aos_dev_t *dev = node->dev;
    netdev_driver_t *drv = dev->drv;

    nbiot_driver_t *eth_drv = (nbiot_driver_t *)drv->link_ops;

    if (eth_drv->reset_nbiot(dev) < 0)
        return -1;

    return (netmgr_nbiot_provision(node));
}

static netmgr_dev_t * netmgr_nbiot_init(struct netmgr_uservice *netmgr)
{
    netmgr_dev_t *node = NULL;

    int ival = 0;

    // get eth configuration
#ifdef CONFIG_KV_SMART
    ival = netmgr_kv_getint(KV_NBIOT_EN);
#else
    ival = 1;
#endif

    if (ival == 1) {
        node = (netmgr_dev_t *)aos_zalloc(sizeof(netmgr_dev_t));

        if (node) {
            node->dev = device_open_id("nbiot", 0);
            aos_assert(node->dev);
            node->provision = netmgr_nbiot_provision;
            node->unprovision = netmgr_nbiot_unprovision;
            node->info = netmgr_nbiot_info;
            node->reset = netmgr_nbiot_reset;
            strcpy(node->name, "nbiot");

            slist_add_tail((slist_t *)node, &netmgr->dev_list);
        }

        netmgr_reg_srv_func(EVENT_NBIOT_LINK_UP, netmgr_nbiot_service);
        netmgr_reg_srv_func(EVENT_NBIOT_LINK_DOWN, netmgr_nbiot_service);

    }

    return node;
}

/**
 * @brief net manage init for nbiot
 * @return NULL on error
 */
netmgr_hdl_t netmgr_dev_nbiot_init()
{
    return (netmgr_hdl_t)netmgr_nbiot_init(&netmgr_svc);
}

