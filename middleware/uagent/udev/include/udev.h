/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef UDEV_H
#define UDEV_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define UDEV_BROKER_HOST     "116.62.245.240"
#define UDEV_BROKER_PORT     17173

#define UDEV_KEEPALIVE      10000   /* mqtt broker keepalive (ms) */

/**
* @brief udev component init
* @param pk     product key
* @param dn     device name
*
* @return -1: fail  0: succeed
*/
int udev_init(const char *pk, const char *dn);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* UDEV_H */
