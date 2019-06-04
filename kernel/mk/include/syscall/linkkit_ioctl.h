/*
 * Copyright (C) 2015-2019 Alibaba Group Holding Limited
 */

#ifndef LINKKIT_IOCTL_H
#define LINKKIT_IOCTL_H

#define IOCTL_SET_PRODUCT_KEY    (0x00UL)
#define IOCTL_GET_PRODUCT_KEY    (0x01UL)
#define IOCTL_SET_PRODUCT_SECRET (0x02UL)
#define IOCTL_GET_PRODUCT_SECRET (0x03UL)
#define IOCTL_SET_DEVICE_NAME    (0x04UL)
#define IOCTL_GET_DEVICE_NAME    (0x05UL)
#define IOCTL_SET_DEVICE_SECRET  (0x06UL)
#define IOCTL_GET_DEVICE_SECRET  (0x07UL)
#define IOCTL_OPEN_LOG           (0x08UL)
#define IOCTL_CLOSE_LOG          (0x09UL)
#define IOCTL_SET_LOG_LEVEL      (0x0AUL)

int linkkit_ioctl(int cmd, void *arg);

#endif /* LINKKIT_IOCTL_H */

