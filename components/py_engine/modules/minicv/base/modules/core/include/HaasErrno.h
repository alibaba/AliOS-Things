
/*
 * Copyright (C) 2020-2021 Alibaba Group Holding Limited
 */
#ifndef HAL_ERRNO_H
#define HAL_ERRNO_H

#include <cerrno>

enum
{
    STATUS_OK          =  0,
    STATUS_ERROR       = -1,
    STATUS_BUSY        = -2,
    STATUS_TIMEOUT     = -3,
    STATUS_ENODEV      = -4,
    STATUS_EPERM       = -5,
    STATUS_EINVAL      = -6,
    STATUS_EBADF       = -7,
    STATUS_EBUSY       = -8,
    STATUS_EAGAIN      = -9,
    STATUS_ETIMEDOUT   = -10,
    STATUS_ENOSPC      = -11,
    STATUS_ENOMEM      = -12,
    STATUS_ENOSUPPORT  = -13,
};

#endif
