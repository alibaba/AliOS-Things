/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __INCLUDE_SYS_IOCTL_H
#define __INCLUDE_SYS_IOCTL_H

#ifdef __cplusplus
extern "C" {
#endif

int ioctl(int fd, int req, ...);

#ifdef __cplusplus
}
#endif

#endif /* __INCLUDE_SYS_IOCTL_H */
