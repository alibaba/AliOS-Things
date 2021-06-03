/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>

#include "k_api.h"
#include "ulog/ulog.h"

#define TAG "netdev"

#include <netdev_ipaddr.h>
#include <netdev.h>

/* The list of network interface device */
struct netdev *netdev_list;
/* The default network interface device */
struct netdev *netdev_default;

/**
 * This function will register network interface device and
 * add it to network interface device list.
 *
 * @param netdev the network interface device object
 * @param name the network interface device name
 * @param user_data user-specific data
 *
 * @return  0: registered successfully
 *         -1: registered failed
 */
int netdev_register(struct netdev *netdev, const char *name, void *user_data)
{
    uint16_t flags_mask;
    int index;

    LOG("%s netdev %p name %s\r\n", __func__, netdev, name);

    ASSERT(netdev);

    /* clean network interface device */
    flags_mask = NETDEV_FLAG_UP | NETDEV_FLAG_LINK_UP | NETDEV_FLAG_INTERNET_UP | NETDEV_FLAG_DHCP | NETDEV_FLAG_DHCPD;
    netdev->flags &= ~flags_mask;

    ip_addr_set_zero(&(netdev->ip_addr));
    ip_addr_set_zero(&(netdev->netmask));
    ip_addr_set_zero(&(netdev->gw));
    for (index = 0; index < NETDEV_DNS_SERVERS_NUM; index++)
    {
        ip_addr_set_zero(&(netdev->dns_servers[index]));
    }
    netdev->status_callback = NULL;
    netdev->addr_callback = NULL;

    /* fill network interface device */
    memset(netdev->name, 0, sizeof(netdev->name));
    strncpy(netdev->name, name, sizeof(netdev->name)-1);
    netdev->user_data = user_data;

    /* initialize current network interface device single list */
    slist_init(&(netdev->list));

    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();

    if (netdev_list == NULL)
    {
        LOGD(TAG, "%s netdev %s as the first node in list\r\n", __func__, netdev->name);
        netdev_list = netdev;
        //netdev_default = netdev;
    }
    else
    {
        /* tail insertion */
        //slist_append(&(netdev_list->list), &(netdev->list));
        LOGD(TAG, "%s netdev %s into list tail\r\n", __func__, netdev->name);
        slist_add_tail(&(netdev->list), &(netdev_list->list));
    }

    RHINO_CPU_INTRPT_ENABLE();

    return 0;
}

/**
 * This function will unregister network interface device and
 * delete it from network interface device list.
 *
 * @param netdev the network interface device object
 *
 * @return  0: unregistered successfully
 *         -1: unregistered failed
 */
int netdev_unregister(struct netdev *netdev)
{
    slist_t *node = NULL;
    struct netdev *cur_netdev = NULL;

    ASSERT(netdev);

    if (netdev_list == NULL)
    {
        return -1;
    }

    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();

    slist_for_each_entry_safe(&(netdev_list->list), node, cur_netdev, struct netdev, list)
    {
        if (cur_netdev && (memcmp(cur_netdev, netdev, sizeof(struct netdev)) == 0))
        {
            slist_del(&(cur_netdev->list), &(netdev_list->list));
            RHINO_CPU_INTRPT_ENABLE();

            return 0;
        }
    }

    RHINO_CPU_INTRPT_ENABLE();

    /* not find this network interface device in network interface device list */
    return -1;
}

/**
 * This function will get the first network interface device
 * with the flags in network interface device list.
 *
 * @param flags the network interface device flags
 *
 * @return != NULL: network interface device object
 *            NULL: get failed
 */
struct netdev *netdev_get_first_by_flags(uint16_t flags)
{
    slist_t *node = NULL;
    struct netdev *netdev = NULL;

    if (netdev_list == NULL)
    {
        return NULL;
    }

    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();

    slist_for_each_entry_safe(&(netdev_list->list), node, netdev, struct netdev, list)
    {
        if (netdev && (netdev->flags & flags) != 0)
        {
            RHINO_CPU_INTRPT_ENABLE();
            return netdev;
        }
    }

    RHINO_CPU_INTRPT_ENABLE();

    return NULL;
}

/**
 * This function will get the first network interface device
 * in network interface device list by IP address.
 *
 * @param addr the network interface device IP address
 *
 * @return != NULL: network interface device object
 *            NULL: get failed
 */
struct netdev *netdev_get_by_ipaddr(ip_addr_t *ip_addr)
{
    slist_t *node = NULL;
    struct netdev *netdev = NULL;

    if (netdev_list == NULL)
    {
        return NULL;
    }

    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();

    slist_for_each_entry_safe(&(netdev_list->list), node, netdev, struct netdev, list)
    {
        if (netdev && ip_addr_cmp(&(netdev->ip_addr), ip_addr))
        {
           RHINO_CPU_INTRPT_ENABLE();
           return netdev;
        }
    }

    RHINO_CPU_INTRPT_ENABLE();

    return NULL;
}

/**
 * This function will get network interface device
 * in network interface device list by netdev name.
 *
 * @param name the network interface device name
 *
 * @return != NULL: network interface device object
 *            NULL: get failed
 */
struct netdev *netdev_get_by_name(const char *name)
{
    slist_t *node = NULL;
    struct netdev *netdev = NULL;

    if (netdev_list == NULL)
    {
        return NULL;
    }

    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();

    slist_for_each_entry_safe(&(netdev_list->list), node, netdev, struct netdev, list)
    {
        if (netdev && (strncmp(netdev->name, name, strlen(netdev->name)) == 0))
        {
            RHINO_CPU_INTRPT_ENABLE();
            return netdev;
        }
    }

    RHINO_CPU_INTRPT_ENABLE();


    return NULL;
}

#ifdef USING_SAL
/**
 * This function will get the first network interface device
 * in network interface device list by protocol family type.
 *
 * @param family the network interface device protocol family type
 *
 * @return != NULL: network interface device object
 *            NULL: get failed
 */
struct netdev *netdev_get_by_family(int family)
{
    slist_t *node = NULL;
    struct netdev *netdev = NULL;
    struct sal_proto_family *pf = NULL;

    if (netdev_list == NULL)
    {
        return NULL;
    }

    CPSR_ALLOC();
    RHINO_CPU_INTRPT_DISABLE();

    slist_for_each_entry_safe(&(netdev_list->list), node, netdev, struct netdev, list)
    {
        pf = (struct sal_proto_family *) netdev->sal_user_data;
        if (pf && pf->skt_ops && pf->family == family && netdev_is_up(netdev))
        {
            RHINO_CPU_INTRPT_ENABLE();
            return netdev;
        }
    }

    slist_for_each_entry_safe(&(netdev_list->list), node, netdev, struct netdev, list)
    {
        pf = (struct sal_proto_family *) netdev->sal_user_data;
        if (pf && pf->skt_ops && pf->sec_family == family && netdev_is_up(netdev))
        {
            RHINO_CPU_INTRPT_ENABLE();
            return netdev;
        }
    }

    RHINO_CPU_INTRPT_ENABLE();

    return NULL;
}

/**
 * This function will get the family type from network interface device
 *
 * @param netdev network interface device object
 *
 * @return the network interface device family type
 */
int netdev_family_get(struct netdev *netdev)
{
    ASSERT(netdev);

    return ((struct sal_proto_family *)netdev->sal_user_data)->family;
}

#endif /* AOS_USING_SAL */

/**
 * This function will set default network interface device.
 *
 * @param netdev the network interface device to change
 */
void netdev_set_default(struct netdev *netdev)
{
    if (netdev)
    {
        netdev_default = netdev;
        netdev->ops->set_default_netif(netdev);//add by li_zhihai@anyka.oa
        LOGD("Setting default network interface device name(%s) successfully.", netdev->name);
    }
}

/**
 * This function will enable network interface device .
 *
 * @param netdev the network interface device to change
 *
 * @return  0: set status successfully
 *         -1: set status failed
 */
int netdev_set_up(struct netdev *netdev)
{
    if (!netdev->ops || !netdev->ops->set_up)
    {
        LOGE(TAG, "The network interface device(%s) not support to set status.", netdev->name);
        return -1;
    }

    /* network interface device status flags check */
    if (netdev_is_up(netdev))
    {
        return 0;
    }

    /* execute enable network interface device operations by network interface device driver */
    return netdev->ops->set_up(netdev);
}
/**
 * This function will disable network interface device.
 *
 * @param netdev the network interface device to change
 *
 * @return  0: set status successfully
 *         -1: set sttaus failed
 */
int netdev_set_down(struct netdev *netdev)
{
    ASSERT(netdev);

    if (!netdev->ops || !netdev->ops->set_down)
    {
        LOGE(TAG, "The network interface device(%s) not support to set status.", netdev->name);
        return -1;
    }

    /* network interface device status flags check */
    if (!netdev_is_up(netdev))
    {
        return 0;
    }

    /* execute disable network interface device operations by network interface driver */
    return netdev->ops->set_down(netdev);
}

/**
 * This function will control network interface device DHCP capability enable or disable.
 *
 * @param netdev the network interface device device to change
 * @param is_enable the new DHCP status
 *
 * @return  0: set DHCP status successfully
 *         -1: set DHCP status failed
 */
int netdev_dhcp_enabled(struct netdev *netdev, bool is_enabled)
{
    ASSERT(netdev);

    if (!netdev->ops || !netdev->ops->set_dhcp)
    {
        LOGE(TAG, "The network interface device(%s) not support to set DHCP status.", netdev->name);
        return -1;
    }

    /* network interface device DHCP flags check */
    if (netdev_is_dhcp_enabled(netdev) == is_enabled)
    {
        return 0;
    }

    /* execute network interface device DHCP capability control operations */
    return netdev->ops->set_dhcp(netdev, is_enabled);
}

/**
 * This function will control network interface device DHCP capability enable or disable.
 *
 * @param netdev the network interface device device to change
 * @param is_enable the new DHCP status
 *
 * @return  0: set DHCP status successfully
 *         -1: set DHCP status failed
 */
int netdev_dhcpd_enabled(struct netdev *netdev, bool is_enabled)
{
    ASSERT(netdev);

    if (!netdev->ops || !netdev->ops->set_dhcpd)
    {
        LOGE(TAG, "The network interface device(%s) not support to set DHCPD status.", netdev->name);
        return -1;
    }

    /* network interface device DHCP flags check */
    if (netdev_is_dhcpd_enabled(netdev) == is_enabled)
    {
        return 0;
    }

    if (is_enabled)
    {
        netdev->flags |= NETDEV_FLAG_DHCPD;
    }
    else
    {
        netdev->flags &= ~NETDEV_FLAG_DHCPD;
    }

    /* execute network interface device DHCP capability control operations */
    return netdev->ops->set_dhcpd(netdev, is_enabled);
}

/**
 * This function will set network interface device IP address.
 *
 * @param netdev the network interface device to change
 * @param ipaddr the new IP address
 *
 * @return  0: set IP address successfully
 *         -1: set IP address failed
 */
int netdev_set_ipaddr(struct netdev *netdev, const ip_addr_t *ip_addr)
{
    ASSERT(netdev);
    ASSERT(ip_addr);

    if (!netdev->ops || !netdev->ops->set_addr_info)
    {
        LOGE(TAG, "The network interface device(%s) not support to set IP address.", netdev->name);
        return -1;
    }

//disabled by li_zhihai@any.oa
#if 0
    if (netdev_is_dhcp_enabled(netdev))
    {

        LOGE(TAG, "The network interface device(%s) DHCP capability is enable, not support set IP address.", netdev->name);
        return -1;
    }
#endif

     /* execute network interface device set IP address operations */
    return netdev->ops->set_addr_info(netdev, (ip_addr_t *)ip_addr, NULL, NULL);
}

/**
 * This function will set network interface device netmask address.
 *
 * @param netdev the network interface device to change
 * @param netmask the new netmask address
 *
 * @return  0: set netmask address successfully
 *         -1: set netmask address failed
 */
int netdev_set_netmask(struct netdev *netdev, const ip_addr_t *netmask)
{
    ASSERT(netdev);
    ASSERT(netmask);

    if (!netdev->ops || !netdev->ops->set_addr_info)
    {
        LOGE(TAG, "The network interface device(%s) not support to set netmask address.", netdev->name);
        return -1;
    }
//disabled by li_zhihai@any.oa
#if 0
    if (netdev_is_dhcp_enabled(netdev))
    {
       LOGE(TAG, "The network interface device(%s) DHCP capability is enable, not support set netmask address.", netdev->name);
        return -1;
    }
#endif
    /* execute network interface device set netmask address operations */
    return netdev->ops->set_addr_info(netdev, NULL, (ip_addr_t *)netmask, NULL);
}

/**
 * This function will set network interface device gateway address.
 *
 * @param netdev the network interface device to change
 * @param gateway the new gateway address
 *
 * @return  0: set gateway address successfully
 *         -1: set gateway address failed
 */
int netdev_set_gw(struct netdev *netdev, const ip_addr_t *gw)
{
    ASSERT(netdev);
    ASSERT(gw);

    if (!netdev->ops || !netdev->ops->set_addr_info)
    {
        LOGE(TAG, "The network interface device(%s) not support to set gateway address.", netdev->name);
        return -1;
    }
//disabled by li_zhihai@any.oa
#if 0
    if (netdev_is_dhcp_enabled(netdev))
    {
        LOGE(TAG, "The network interface device(%s) DHCP capability is enable, not support set gateway address.", netdev->name);
        return -1;
    }
#endif
    /* execute network interface device set gateway address operations */
    return netdev->ops->set_addr_info(netdev, NULL, NULL, (ip_addr_t *)gw);
}

/**
 * This function will set network interface device DNS server address.
 *
 * @param netdev the network interface device to change
 * @param dns_server the new DNS server address
 *
 * @return  0: set netmask address successfully
 *         -1: set netmask address failed
 */
int netdev_set_dns_server(struct netdev *netdev, uint8_t dns_num, const ip_addr_t *dns_server)
{
    ASSERT(netdev);
    ASSERT(dns_server);

    if (dns_num >= NETDEV_DNS_SERVERS_NUM)
    {
        LOGE(TAG, "The number of DNS servers(%d) set exceeds the maximum number(%d).", dns_num + 1, NETDEV_DNS_SERVERS_NUM);
        return -1;
    }

    if (!netdev->ops || !netdev->ops->set_dns_server)
    {
        LOGE(TAG, "The network interface device(%s) not support to set DNS server address.", netdev->name);
        return -1;
    }

    /* execute network interface device set DNS server address operations */
    return netdev->ops->set_dns_server(netdev, dns_num, (ip_addr_t *)dns_server);
}

/**
 * This function will set callback to be called when the network interface device status has been changed.
 *
 * @param netdev the network interface device to change
 * @param status_callback the callback be called when the status has been changed.
 */
void netdev_set_status_callback(struct netdev *netdev, netdev_callback_fn status_callback)
{
    ASSERT(netdev);
    ASSERT(status_callback);

    netdev->status_callback = status_callback;
}

/**
 * This function will set callback to be called when the network interface device address has been changed.
 *
 * @param netdev the network interface device to change
 * @param addr_callback the callback be called when the address has been changed.
 */
void netdev_set_addr_callback(struct netdev *netdev, netdev_callback_fn addr_callback)
{
    ASSERT(netdev);
    ASSERT(addr_callback);

    netdev->addr_callback = addr_callback;
}


/**
 * This function will set network interface device IP address.
 * @NOTE it can only be called in the network interface device driver.
 *
 * @param netdev the network interface device to change
 * @param ipaddr the new IP address
 */
void netdev_low_level_set_ipaddr(struct netdev *netdev, const ip_addr_t *ip_addr)
{
    ASSERT(ip_addr);

    if (netdev && ip_addr_cmp(&(netdev->ip_addr), ip_addr) == 0)
    {
        ip_addr_copy(netdev->ip_addr, *ip_addr);

#ifdef USING_SAL
        /* set network interface device flags to internet up */
        if (netdev_is_up(netdev) && netdev_is_link_up(netdev))
        {
            sal_check_netdev_internet_up(netdev);
        }
#endif /* AOS_USING_SAL */

        /* execute IP address change callback function */
        if (netdev->addr_callback)
        {
            netdev->addr_callback(netdev, NETDEV_CB_ADDR_IP);
        }
    }
}

/**
 * This function will set network interface device netmask address.
 * @NOTE it can only be called in the network interface device driver.
 *
 * @param netdev the network interface device to change
 * @param netmask the new netmask address
 */
void netdev_low_level_set_netmask(struct netdev *netdev, const ip_addr_t *netmask)
{
    ASSERT(netmask);

    if (netdev && ip_addr_cmp(&(netdev->netmask), netmask) == 0)
    {
        ip_addr_copy(netdev->netmask, *netmask);

#ifdef AOS_USING_SAL
        /* set network interface device flags to internet up */
        if (netdev_is_up(netdev) && netdev_is_link_up(netdev) &&
                !ip_addr_isany(&(netdev->ip_addr)))
        {
            sal_check_netdev_internet_up(netdev);
        }
#endif /* AOS_USING_SAL */

        /* execute netmask address change callback function */
        if (netdev->addr_callback)
        {
            netdev->addr_callback(netdev, NETDEV_CB_ADDR_NETMASK);
        }
    }
}

/**
 * This function will set network interface device gateway address.
 * @NOTE it can only be called in the network interface device driver.
 *
 * @param netdev the network interface device to change
 * @param gateway the new gateway address
 */
void netdev_low_level_set_gw(struct netdev *netdev, const ip_addr_t *gw)
{
    ASSERT(gw);

    if (netdev && ip_addr_cmp(&(netdev->gw), gw) == 0)
    {
        ip_addr_copy(netdev->gw, *gw);

#ifdef USING_SAL
        /* set network interface device flags to internet up */
        if (netdev_is_up(netdev) && netdev_is_link_up(netdev) &&
                !ip_addr_isany(&(netdev->ip_addr)))
        {
            sal_check_netdev_internet_up(netdev);
        }
#endif /* AOS_USING_SAL */

        /* execute gateway address change callback function */
        if (netdev->addr_callback)
        {
            netdev->addr_callback(netdev, NETDEV_CB_ADDR_GATEWAY);
        }
    }
}

/**
 * This function will set network interface device DNS server address.
 * @NOTE it can only be called in the network interface device driver.
 *
 * @param netdev the network interface device to change
 * @param dns_server the new DNS server address
 *
 */
void netdev_low_level_set_dns_server(struct netdev *netdev, uint8_t dns_num, const ip_addr_t *dns_server)
{
    int index;

    ASSERT(dns_server);

    /* check DNS servers is exist */
    for (index = 0; index < NETDEV_DNS_SERVERS_NUM; index++)
    {
        if (ip_addr_cmp(&(netdev->dns_servers[index]), dns_server))
        {
            return;
        }
    }

    if (netdev && dns_num < NETDEV_DNS_SERVERS_NUM)
    {
        ip_addr_copy(netdev->dns_servers[dns_num], *dns_server);

        /* execute DNS servers address change callback function */
        if (netdev->addr_callback)
        {
            netdev->addr_callback(netdev, NETDEV_CB_ADDR_DNS_SERVER);
        }
    }
}

#ifdef NETDEV_USING_AUTO_DEFAULT
/* Change to the first link_up network interface device automatically */
static void netdev_auto_change_default(struct netdev *netdev)
{
    struct netdev *new_netdev = NULL;

    if (rt_memcmp(netdev, netdev_default, sizeof(struct netdev)) == 0)
    {
        new_netdev = netdev_get_first_by_flags(NETDEV_FLAG_LINK_UP);
        if (new_netdev)
        {
            netdev_set_default(new_netdev);
        }
    }
}
#endif /* NETDEV_USING_AUTO_DEFAULT */

/**
 * This function will set network interface device status.
 * @NOTE it can only be called in the network interface device driver.
 *
 * @param netdev the network interface device to change
 * @param is_up the new status
 */
void netdev_low_level_set_status(struct netdev *netdev, bool is_up)
{
    if (netdev && netdev_is_up(netdev) != is_up)
    {
        if (is_up)
        {
            netdev->flags |= NETDEV_FLAG_UP;
            if(netdev_default == NULL)
            {
                netdev_default = netdev;
                netdev_set_default(netdev);
            }
        }
        else
        {
            netdev->flags &= ~NETDEV_FLAG_UP;

#ifdef NETDEV_USING_AUTO_DEFAULT
            /* change to the first link_up network interface device automatically */
            netdev_auto_change_default(netdev);
#endif /* NETDEV_USING_AUTO_DEFAULT */
        }

        /* execute  network interface device status change callback function */
        if (netdev->status_callback)
        {
            netdev->status_callback(netdev, is_up ? NETDEV_CB_STATUS_UP : NETDEV_CB_STATUS_DOWN);
        }
    }
}

/**
 * This function will set network interface device active link status.
 * @NOTE it can only be called in the network interface device driver.
 *
 * @param netdev the network interface device to change
 * @param is_up the new link status
 */
void netdev_low_level_set_link_status(struct netdev *netdev, bool is_up)
{
    if (netdev && netdev_is_link_up(netdev) != is_up)
    {
        if (is_up)
        {
            netdev->flags |= NETDEV_FLAG_LINK_UP;

#ifdef USING_SAL
            /* set network interface device flags to internet up */
            if (netdev_is_up(netdev) && !ip_addr_isany(&(netdev->ip_addr)))
            {
                sal_check_netdev_internet_up(netdev);
            }
#endif /* USING_SAL */
        }
        else
        {
            netdev->flags &= ~NETDEV_FLAG_LINK_UP;

            /* set network interface device flags to internet down */
            netdev->flags &= ~NETDEV_FLAG_INTERNET_UP;

#ifdef NETDEV_USING_AUTO_DEFAULT
            /* change to the first link_up network interface device automatically */
            netdev_auto_change_default(netdev);
#endif /* NETDEV_USING_AUTO_DEFAULT */
        }

        /* execute link status change callback function */
        if (netdev->status_callback)
        {
            netdev->status_callback(netdev, is_up ? NETDEV_CB_STATUS_LINK_UP : NETDEV_CB_STATUS_LINK_DOWN);
        }
    }
}

/**
 * This function will set network interface device DHCP status.
 * @NOTE it can only be called in the network interface device driver.
 *
 * @param netdev the network interface device to change
 * @param is_up the new DHCP status
 */
void netdev_low_level_set_dhcp_status(struct netdev *netdev, bool is_enable)
{
    if (netdev && netdev_is_dhcp_enabled(netdev) != is_enable)
    {
        if (is_enable)
        {
            netdev->flags |= NETDEV_FLAG_DHCP;
        }
        else
        {
            netdev->flags &= ~NETDEV_FLAG_DHCP;
        }

        /* execute DHCP status change callback function */
        if (netdev->status_callback)
        {
            netdev->status_callback(netdev, is_enable ? NETDEV_CB_STATUS_DHCP_ENABLE : NETDEV_CB_STATUS_DHCP_DISABLE);
        }
    }
}

