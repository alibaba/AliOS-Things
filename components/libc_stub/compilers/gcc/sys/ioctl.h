/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef SYS_IOCTL_H
#define SYS_IOCTL_H

#ifdef __cplusplus
extern "C" {
#endif

int ioctl(int fd, int req, ...);

#ifdef __cplusplus
}
#endif

#endif /* SYS_IOCTL_H */
