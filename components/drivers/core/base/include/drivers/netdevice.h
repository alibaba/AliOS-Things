/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#ifndef NETDEVICE_H
#define NETDEVICE_H

#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#include "object.h"
#include "device.h"

#include "aos/kernel.h"
#include "aos/vfs.h"

#ifdef __cplusplus
extern "C" {
#endif
typedef enum net_device_type_e {
    NET_DEVICE_TYPE_WIFI,
    NET_DEVICE_TYPE_ETH
} net_device_type_t;

typedef enum netbuf_type_e {
    NETBUF_TYPE_RAW,
    NETBUF_TYPE_PBUF,
    NETBUF_TYPE_ATBM,
    NETBUF_TYPE_MAX
} netbuf_type_t;

#ifndef ZEROSIZE
#define ZEROSIZE 0
#endif

struct net_device_ops;
typedef struct net_device{
    struct net_device_ops* netdev_ops;
    void* netif;
    net_device_type_t type;
    struct platform_device* device;
    void*  drv_priv;
} net_device_t;

typedef struct net_device_ops{
   int (*ndo_init)(net_device_t* ndev);
   int (*ndo_deinit)(net_device_t* ndev);
   int (*ndo_open)(net_device_t* ndev);
   int (*ndo_close)(net_device_t* ndev);
   int (*ndo_enable)(net_device_t* ndev);
   int (*ndo_disable)(net_device_t* ndev);
   int (*ndo_send)(net_device_t* ndev, netbuf_type_t type, void *buf, int len);
   int (*ndo_recv)(net_device_t* ndev, netbuf_type_t type, void* buf, int len);
   int (*ndo_ioctl)(net_device_t* ndev, struct ifreq *ifr, int cmd);
} net_device_ops_t;

#ifdef __cplusplus
}
#endif
#endif
