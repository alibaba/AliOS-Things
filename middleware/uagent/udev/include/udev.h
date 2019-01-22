/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef __UDEV_H_
#define __UDEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#define UDEV_BROKER_HOST     "116.62.245.240"
#define UDEV_BROKER_PORT     1883

/* mqtt broker keepalive (ms) */
#define UDEV_KEEPALIVE      10000

int udev_init(const char *pk, const char *dn);

#ifdef __cplusplus
}
#endif

#endif /* __UDEV_H_ */
