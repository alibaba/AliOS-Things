/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#include <aos/aos.h>
#include <uservice/uservice.h>
#include <uservice/eventid.h>
#include <devices/netdrv.h>

#include <netmgr.h>
#include <netmgr_service.h>
#include <devices/hal/gprs_impl.h>



typedef struct {
    netmgr_hdl_t hdl;
    gprs_setting_t config;
} param_gprs_setting_t;

static const char *TAG = "netmgr_gprs";

static int netmgr_gprs_provision(netmgr_dev_t *node)
{
    aos_dev_t *dev = node->dev;
    netdev_driver_t *drv = dev->drv;

    gprs_driver_t *gprs_drv = (gprs_driver_t *)drv->link_ops;
    netmgr_subscribe(EVENT_GPRS_LINK_UP);
    netmgr_subscribe(EVENT_GPRS_LINK_DOWN);

    if (gprs_drv->module_init_check(dev) < 0) {
        LOGW(TAG, "GPRS MODULE CHECK ERR");
        return -1;
    }

    gprs_drv->set_mode(dev, CONNECT_MODE_GPRS);

    if (gprs_drv->connect_to_gprs(dev) < 0) {
        return -1;
    }

    return 0;

}

static int netmgr_gprs_unprovision(netmgr_dev_t *node)
{
    return 0;
}

static int netmgr_gprs_reset(netmgr_dev_t *node)
{
    aos_dev_t *dev = node->dev;
    netdev_driver_t *drv = dev->drv;

    gprs_driver_t *gprs_drv = (gprs_driver_t *)drv->link_ops;

    if (gprs_drv->reset(dev) < 0) {
        LOGE(TAG, "GPRS RESET ERR");
        return -1;
    }

    return (netmgr_gprs_provision(node));
}

static int netmgr_gprs_info(netmgr_dev_t *node)
{
    aos_dev_t  *dev = node->dev;
    netdev_driver_t *drv = dev->drv;
    gprs_driver_t *gprs_drv = (gprs_driver_t *)drv->link_ops;
    net_ops_t  *net_drv = drv->net_ops;
    ip_addr_t ipaddr;
    int csq = 99;
    char ccid[21];
    int insert = -1;
    int ret;

    memset(&ipaddr, 0, sizeof(ipaddr));
    net_drv->get_ipaddr(dev, &ipaddr, NULL, NULL);

    gprs_drv->get_csq(dev, &csq);

    ret = gprs_drv->get_simcard_info(dev, ccid, &insert);
    printf("\n%s%d\tLink encap:GPRS\n", drv->drv.name, drv->drv.device_id);
    printf("    \tinet addr:%s\n", ipaddr_ntoa(&ipaddr));
    if(csq == 99 ) {
        printf("    \tSignal Quality: not known\n");
    } else {
        printf("    \tSignal Quality(%d): %d dBm\n", csq, (csq - 31) * 2 - 52);
    }
    if(ret < 0) {
        printf("    \tSim insert is FAIL\n");
    } else {
        printf("    \tSim insert is %s\n",insert >= 0?"OK":"FAIL");
        printf("    \tSim ccid:%s\n", ccid);
    }
    return 0;
}


static int gprs_evt_link_down(struct netmgr_uservice *netmgr, rpc_t *rpc)
{
    netmgr_set_gotip("gprs", 0);
    event_publish(EVENT_NETMGR_NET_DISCON, NULL);
    return 0;
}
static netmgr_dev_t * netmgr_gprs_init(struct netmgr_uservice *netmgr)
{
    netmgr_dev_t *node = NULL;
    int ival = 0;

    // get gprs configuration
#ifdef CONFIG_KV_SMART
    ival = netmgr_kv_getint(KV_GPRS_EN);
#else
    ival = 1;
#endif

    if (ival == 1) {
        netmgr_reg_srv_func(EVENT_GPRS_LINK_DOWN, gprs_evt_link_down);

        node = (netmgr_dev_t *)aos_zalloc(sizeof(netmgr_dev_t));

        if (node) {
            node->dev = device_open_id("gprs", 0);
            aos_assert(node->dev);
            node->provision = netmgr_gprs_provision;
            node->unprovision = netmgr_gprs_unprovision;
            node->info = netmgr_gprs_info;
            node->reset = netmgr_gprs_reset;
            strcpy(node->name, "gprs");

            slist_add_tail((slist_t *)node, &netmgr->dev_list);
        }

    }

    return (netmgr_hdl_t)node;
}

/**
 * @brief net manage init for gprs
 * @return NULL on error
 */
netmgr_hdl_t netmgr_dev_gprs_init()
{
    return netmgr_gprs_init(&netmgr_svc);
}

/**
 * @brief  net manage config for gprs device(developed not yet, reserved)
 * @param  [in] hdl
 * @param  [in] mode
 * @return 0 on success
 */
int netmgr_config_gprs(netmgr_hdl_t hdl, int mode)
{
    int ret = -1;
    param_gprs_setting_t param;
    gprs_setting_t *config = &param.config;

    aos_check_return_einval(hdl);

    param.hdl = hdl;
    config->foo = mode;

    uservice_call_sync(netmgr_svc.srv, API_GPRS_CONFIG, &param, &ret, sizeof(int));

    return ret;
}
