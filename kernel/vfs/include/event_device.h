/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef EVENT_DEVICE_H
#define EVENT_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#define IOCTL_WRITE_NORMAL 0x1
#define IOCTL_WRITE_URGENT 0x2

#define MK_CMD(c, l) ((l << 4) | (c))
#define _GET_LEN(cmd) ((cmd) >> 4)
#define _GET_CMD(cmd) ((cmd) & 0xf)

#ifdef __cplusplus
}
#endif

#endif
