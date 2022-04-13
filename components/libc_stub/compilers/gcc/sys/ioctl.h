/*
 * Copyright (C) 2019-2020 Alibaba Group Holding Limited
 */

#ifndef __INCLUDE_SYS_IOCTL_H
#define __INCLUDE_SYS_IOCTL_H
#ifdef __cplusplus
extern "C" {
#endif


#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

int ioctl(int fd, int req, ...);

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* __INCLUDE_SYS_IOCTL_H */
